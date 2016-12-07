#include <algorithm>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>
#include <stdio.h>

#include "cudaModel.h"

#define MAXLEVEL 600
// single buffer array size
#define BUFSIZE (MAXLEVEL+1)*(MAXLEVEL+2)/2
#define MAXLENGTH 10

#define THREAD_PER_BLOCK 128
/*
  This file is the implementation of parallel binomial model
   */

////////// KERNEL /////////////////////////
// In this project, we are calculating the call price
__device__ __inline__ double getValue(double strike, double stock){
        double value = stock - strike;
        if(value > 0.0)
            return value;
        return 0.0;
}
// kernel calculate inline


__global__ void kernelFinalStage(int length, double strike,
                            ){
        int index = blockDim.x * blockIdx.x + threadIdx.x;
        if( index >= length * (MAXLEVEL+1))
            return; // output bound



}


__global__ void kernel_calc(long double * device_data, bool * flag){
        // check flag
        int idx = blockDim.x * blockIdx.x + threadIdx.x;
        // calculate data

        // write result


}
//////////// HOST ///////////////////////////
// initialize the buffer and related structure
CudaModel::CudaModel(double s){
        cudaMalloc(&device_p, sizeof(double)*MAXLENGTH);
        cudaMalloc(&device_q, sizeof(double)*MAXLENGTH);
        cudaMalloc(&device_buf, sizeof(double)*MAXLENGTH*BUFSIZE);
        cudaMalloc(&device_u, sizeof(double)*MAXLENGTH);
        cudaMalloc(&device_d, sizeof(double)*MAXLENGTH);
        cudaMalloc(&device_s, sizeof(double)*MAXLENGTH);
        strike = s;
}

CudaModel::~CudaModel(){
        cudaFree(device_p);
        cudaFree(device_q);
        cudaFree(device_buf);
        cudaFree(device_u);
        cudaFree(device_d);
        cudaFree(device_s);
}

void CudaModel::calculate(double* array_u, double* array_d, double* array_s,
                          double* array_output, int length){
    if(length > MAXLENGTH){
        printf("error: ---length larger than system max!\n");
        return;
    }

    cudaMemcpy(device_u, array_u, length*sizeof(double),cudaMemcpyHostToDevice);
    cudaMemcpy(device_d, array_d, length*sizeof(double),cudaMemcpyHostToDevice);
    cudaMemcpy(device_s, array_s, length*sizeof(double),cudaMemcpyHostToDevice);
    for(int c = 0; c < length; c++){
        double p = (1.0 - array_d[c]) / (array_u[c] - array_d[c]);
        double q = 1.0 - p;
        array_p[c] = p;
        array_q[c] = q;
    }
    cudaMemcpy(device_p, array_p, length*sizeof(double),cudaMemcpyHostToDevice);
    cudaMemcpy(device_q, array_q, length*sizeof(double),cudaMemcpyHostToDevice);
        // setup final stage
        int total_calc = length * (MAXLEVEL+1);
        int block_n = (total_calc + THREAD_PER_BLOCK -1) / THREAD_PER_BLOCK;
        kernelFinalStage<<<block_n, THREAD_PER_BLOCK>>>( );
        // parallel by level
        for(int level = MAXLEVEL-1; level >= 0; level--){


        }
        // output result

}
