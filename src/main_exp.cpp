#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "cudaModel.h"
#include "model_s.h"

int main(int argc, char** argv)
{
    clock_t t;
    CudaModel model = CudaModel(2000.0);
    printf("Start calculation  ---  prediction\n");
    int length = 0;
    double u = 1.005264382904;
    double d = 0.996038098354;
    double r = -0.000051635011;
    //double out[40];
    char value[1024];
    char* line;
    char* record;
    double s[40];
    double out[40];
    //double out2[20];
    //double s[20] = {1921.41,1948.51,1987.89,1981.41,1921.81,1928.51,1887.89, 1982.01, 1981.06, 1941.01, 1927.41,1928.51,1957.89,1981.31,1921.71,1948.11,1987.49, 1983.01, 1921.03, 1989.01};
    printf("U: %9.9f  D: %9.9f  R: %9.9f\n", u, d, r);
    FILE* file = fopen("ProjectData.csv", "r");
    if(file == NULL) return 0;
    int idx = 0;
    int start = 0;
    while((line=fgets(value,sizeof(value),file))!=NULL){
        if(start == 0){
            start = 1;
            continue;
        }
        //printf("%s\n", line);
        record = strtok(line, ",");
        record = strtok(NULL, ",");
        s[idx] = atof(record);
        //printf("%s\n", record);
        length++;
        idx++;
    }
    t = clock();
    model.calculate(u, d, r, s, out, length);
    //model.calculate(u2, d, s, out2, length);
    t = clock() - t;
    //long double result2 = model.calculate(u+0.0001, d, 0.000l, 219.07l);
    printf("CUDA Time elapsed %Lf milliseconds \n" ,((long double)t)/(CLOCKS_PER_SEC/1000));
    for(int i = 0; i < length; i++){
        printf("%9.9f \n",out[i]);
        //printf("CUDA result %d  s0= %9.9f vhat = %9.9f \n",i, s[i], out[i]);
        // printf("CUDA result out2 %d = %9.9f \n",i, out2[i]);
    }
    /*
       ModelS model_s = ModelS(2000.0);
       t = clock();
       long double result = model_s.calculate(u,d,0.0,s[1]);
       t = clock() - t;
       printf("Sequential Result: %Lf\n", result);
       printf("Sequantial Time elapsed %Lf milliseconds \n" ,((long double)t)/(CLOCKS_PER_SEC/1000));
       */
    return 0;
}


