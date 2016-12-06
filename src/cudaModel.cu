#include <algorithm>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <stdio.h>


#include "cudaModel.h"
/*
  This file is the implementation of parallel binomial model
   */

////////// KERNEL

/////  __device__
// kernel calculate inline

// find index

// next index



///  __global__
__global__ void kernel_calc(long double * device_data, bool * flag){
        // check flag
        int idx = blockDim.x * blockIdx.x + threadIdx.x;
        // calculate data

        // write result


}
//////////// HOST





