#include <model_s.h>
#include <cmath>
#include <cfenv>
#include <stdexcept>
#include <stdlib.h>

ModelS::ModelS(int period, double k){
    if(period > MAXPERIOD) timePeriod = MAXPERIOD;
    else timePeriod = period;
    strike = k;
    totalStage = (timePeriod + RECURLENGTH - 1)/ RECURLENGTH;
}

double ModelS::safePower(double base, int power){
    std::feclearexcept(FE_OVERFLOW);
    std::feclearexcept(FE_UNDERFLOW); // overflow and underflow detection
    double ret = pow(base, power);
    if((bool)std::fetestexcept(FE_OVERFLOW) || (bool)std::fetestexcept(FE_UNDERFLOW)){
        throw std::invalid_argument("power raise exception!\n");
    }
    return ret;
}

double ModelS::worker(double p, double q, double r, double s, double u, double d,
                    int startPeriod, int stopPeriod){
  // recursive
  // backward induction

}

double ModelS::calculate(double up, double down, double rate, double s0){
    double result;
    // Allocate buffer
    buffer = (double*) calloc(1<<(MAXPERIOD - RECURLENGTH),sizeof(double));
    int currentStage;
    double p = (1 + rate - down) /(up - down);
    double q = (up - rate - 1) /(up - down);
    for (int i = 0; i < totalStage; i++){
        currentStage = totalStage - i;

    }
    return result;
}

ModelS::~ModelS(){
    free(buffer);
}
