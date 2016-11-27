#include <stdio.h>
#include <stdlib.h>
#include "model_s.h"
#include <time.h>
#include <math.h>

int main(int argc, char** argv)
{
    clock_t t;
    ModelS model = ModelS(219.00l);
    t = clock();
    printf("start calculation\n");
    printf("U: %f  D: %f  R: %f\n", 1.0003, .9995, .000);
    long double result1 = model.calculate(1.0003l, 0.9995l, 0.000l, 219.07l);
    printf("U: %f  D: %f  R: %f\n", 1.000301, .9995, .000);
    long double result2 = model.calculate(1.000301l, 0.9995l, 0.000l, 219.07l);
    t = clock() - t;
    printf("1 time calculation %f milliseconds \n" ,((double)t)/(CLOCKS_PER_SEC/1000));
    printf(" result1 = %Lf \n", result1);
    printf(" result2 = %Lf \n", result2);
    return 0;
}


