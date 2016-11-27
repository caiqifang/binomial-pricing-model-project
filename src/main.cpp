#include <stdio.h>
#include <stdlib.h>
#include "model_s.h"
#include <time.h>
#include <math.h>

int main(int argc, char** argv)
{
    clock_t t;
    ModelS model = ModelS(219);
    t = clock();
    printf("start calculation\n");
    printf("U: %f  D: %f  R: %f\n", 1.0003, .9995, .000);
    double result1 = model.calculate(1.0003, 0.9995, 0.000, 219.07);
    printf("U: %f  D: %f  R: %f\n", 1.000301, .9995, .000);
    double result2 = model.calculate(1.000301, 0.9995, 0.000, 219.07);
    t = clock() - t;
    printf("1 time calculation %f milliseconds \n" ,((double)t)/(CLOCKS_PER_SEC/1000));
    printf(" result1 = %f \n", result1);
    printf(" result2 = %f \n", result2);
    return 0;
}


