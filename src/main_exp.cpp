#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "cudaModel.h"
#include "model_s.h"

int main(int argc, char** argv)
{
    clock_t t;
    CudaModel model = CudaModel(2000.0);
    printf("start calculation\n");
    int length = 20;
    double u[20] = {1.003,1.01,1.02,1.03,1.04,1.05,1.06,1.07,1.08,1.09, 1.103,1.15,1.11,1.12,1.13,1.14,1.16,1.17,1.18,1.25};
    double d[20] = {0.998,0.99,0.99,0.98,0.99,0.98,0.99,0.98,0.99,0.98, 0.998,0.99,0.98,0.99,0.98,0.99,0.98,0.99,0.98,0.99};
    double out[20];
    double s[20] = {1921.41,1948.51,1987.89,1981.41,1921.81,1928.51,1887.89, 1982.01, 1981.06, 1941.01, 1927.41,1928.51,1957.89,1981.31,1921.71,1948.11,1987.49, 1983.01, 1921.03, 1989.01};
    printf("U: %f  D: %f\n", u[0], d[0]);
    t = clock();
    model.calculate(u, d, s, out, length);
    t = clock() - t;
    //long double result2 = model.calculate(u+0.0001, d, 0.000l, 219.07l);
    printf("CUDA Time elapsed %Lf milliseconds \n" ,((long double)t)/(CLOCKS_PER_SEC/1000));
    for(int i = 0; i < length; i++){
        printf("CUDA result %d = %f \n",i, out[i]);
    }
    ModelS model_s = ModelS(2000.0);
    t = clock();
    long double result = model_s.calculate(u[0],d[0],0.0,s[0]);
    t = clock() - t;
    printf("Sequential Result: %Lf\n", result);
    printf("Sequantial Time elapsed %Lf milliseconds \n" ,((long double)t)/(CLOCKS_PER_SEC/1000));
    return 0;
}


