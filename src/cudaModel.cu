#include <algorithm>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>
#include <stdio.h>

#include "cudaModel.h"

/*
   This file is the implementation of parallel binomial model
 */

////////// KERNEL /////////////////////////
// In this project, we are calculating the call price
// This kernel function calculate the call option value based on strike price
__device__ __inline__ double getValue(double strike, double stock){
    double value = stock - strike;
    if(value > 0.0)
        return value;
    return 0.0;
}

// get the starting index in the array based on current level and define
__device__ __inline__ int getIndex(int length, int maxL, int level){
    return length*((maxL+1+level+2)*(maxL-level)/2);
}

// calculate the leaf level node value in the binomial tree
// finding intrinsic value of the financial product
__global__ void kernelFinalStage(int length, int maxL,  double strike,
        double* device_buf, double device_u, double device_d, double* device_s){
    int index = blockDim.x * blockIdx.x + threadIdx.x;
    int size = maxL+1;
    if( index >= length * size)
        return; // output bound
    int element = index / size;
    double rank = __int2double_rn(index % size);
    double stock = device_s[element]; // s0
    //double up = device_u[element];
    //double down = device_d[element];
    double maxLevel = __int2double_rn(maxL);
    stock = stock * pow(device_u, maxLevel-rank) * pow(device_d, rank);
    device_buf[index] = getValue(strike, stock);
    return;
}

// parallel compute the value in all nodes in the same level,
// skiping the leaf level nodes
__global__ void kernelCalc(int level, int maxL, int length, double device_p,
        double device_q, double device_r, double* device_buf){
    int index = blockDim.x * blockIdx.x + threadIdx.x;
    int size = level + 1;
    if( index >= length * size)
        return; // output bound
    // WATCH!!
    int curr_start = getIndex( length, maxL, level);
    int prev_start = getIndex( length, maxL, level+1);
    int element = index / size;
    int rank = index % size;
    //double p = device_p[element];
    //double q = device_q[element];
    int head = prev_start + element*(size+1) + rank;
    int tail = head + 1;
    device_buf[curr_start+index] = 1.0/(1+device_r)*(device_p*device_buf[head] +
            device_q*device_buf[tail]);
    return;
}

// parallel copy all outputs to the CPU output array
__global__ void kernelOutput(double* device_output, double* device_buf,
        int length, int maxL){
    int index = blockDim.x * blockIdx.x + threadIdx.x;
    if (index >= length) return; /// out of bound
    int total_length = length*(maxL+2)*(maxL+1)/2;
    device_output[index] = device_buf[total_length - length + index];
    return;
}

//////////// HOST CODE ///////////////////////////
// initialize the buffer and related structure
CudaModel::CudaModel(double s){
    cudaMalloc(&device_buf, sizeof(double)*MAXLENGTH*BUFSIZE);
    cudaMalloc(&device_output, sizeof(double)*MAXLENGTH);
    cudaMalloc(&device_s, sizeof(double)*MAXLENGTH);
    strike = s;
}

// class destructor
CudaModel::~CudaModel(){
    cudaFree(device_buf);
    cudaFree(device_output);
    cudaFree(device_s);
}


// main calculation function
// public to the Machine learning part in the project
void CudaModel::calculate(double up, double down, double rate, double* array_s,
        double* array_output, int length){
    // length check
    if(length > MAXLENGTH){
        printf("error: ---length larger than system max!\n");
        return;
    }
    device_u = up;
    device_d = down;
    device_r = rate;
    cudaMemcpy(device_s, array_s, length*sizeof(double),cudaMemcpyHostToDevice);
    device_p = (1.0 + rate - down) / (up - down);
    device_q = 1.0 - device_p;

    // setup final stage, calculate all the leaf nodes
    int total_calc = length * (MAXLEVEL+1);
    int block_n = (total_calc + THREAD_PER_BLOCK -1) / THREAD_PER_BLOCK;
    kernelFinalStage<<<block_n, THREAD_PER_BLOCK>>>(length, MAXLEVEL, strike,
            device_buf, device_u, device_d, device_s);
    cudaThreadSynchronize();

    // calculate all nodes above leave
    for(int level = MAXLEVEL-1; level >= 0; level--){
        total_calc = length * (level+1);
        block_n = (total_calc + THREAD_PER_BLOCK -1) / THREAD_PER_BLOCK;
        kernelCalc<<<block_n, THREAD_PER_BLOCK>>>(level, MAXLEVEL, length,
                device_p, device_q, device_r, device_buf);
        cudaThreadSynchronize();
    }

    // output result
    kernelOutput<<<1, length>>>(device_output, device_buf, length, MAXLEVEL);
    cudaThreadSynchronize();
    cudaMemcpy(array_output, device_output,
            length*sizeof(double), cudaMemcpyDeviceToHost);
    return;
}
