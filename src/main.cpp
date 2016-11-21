#include <stdio.h>
#include <stdlib.h>
#include "model_s.h"
#include <time.h>
#include <math.h>

int main(int argc, char** argv)
{
    clock_t t;
    ModelS model = ModelS(4.5);
    t = clock();
    printf("start calculation\n");
    double result = model.calculate(1.23, .89, 0.03, 3);
    t = clock() - t;
    printf("1 time calculation %f milliseconds \n" ,((double)t)/(CLOCKS_PER_SEC/1000));
    printf(" result = %f \n", result);
    return 0;
}


