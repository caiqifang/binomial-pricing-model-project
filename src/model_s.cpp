#include "model_s.h"
#include <math.h>
#include <fenv.h>
#include <stdexcept>
#include <stdlib.h>
#include <stdio.h>


ModelS::ModelS(double s){
    strike = s;
}

// safe calculation
double ModelS::safePower(double base, int power){
    feclearexcept(FE_OVERFLOW);
    feclearexcept(FE_UNDERFLOW); // overflow and underflow detection
    double ret = pow(base, power);
    if((bool)fetestexcept(FE_OVERFLOW)
              || (bool)fetestexcept(FE_UNDERFLOW)){
        throw std::runtime_error("power raise exception!\n");
    }
    return ret;
}

state ModelS::indexToState(int idx){
    state result;
    int period = MAXPERIOD/2;
    int period_start = 0;
    int period_end = MAXPERIOD+1;
    int start;
    // find the period
    while(period >= 0 && period <= MAXPERIOD){
        start = period*(period+1)/2;
     //   printf("start idx %d\n", start);
     //   printf("-period start %d\n", period_start);
     //   printf("-period %d\n", period);
     //   printf("-period end %d\n", period_end);
        if( start <= idx && idx <= (start+period))
            break; // period correct
        if(idx < start){
            period_end = period;
            period = (period_start + period) / 2;
        }
        else{
            period_start = period;
            period = (period_end + period) / 2;
        }
    }
    result.period = period;
    // calculate state stock price
    double price = initialPrice * safePower(up, period);
    int diff = idx - start;
    for (int c = 0; c < diff; c++){
        price = price / up * down;
    }
    result.stockPrice = price;
    return result;
}

double ModelS::worker(int idx){
    //printf("here is worker working on idx %d \n", idx);
    state now = indexToState(idx);
    if(now.period == MAXPERIOD){
        // leaf of tree model calculate option value
        return std::max(now.stockPrice - strike, 0.0);
    }
    else
    {
        int head = nextUp(idx, now);
        int tail = head + 1;
        return 1.0/(1+rate)*(p*buffer[head] + q*buffer[tail]);
    }
}

int ModelS::nextUp(int idx, state s){
    int begin = (s.period+1)*(s.period+2)/2;
    int diff = idx - (s.period+1)*(s.period)/2;
    return begin + diff;
}

double ModelS::calculate(double u, double d, double r, double s0){
    up = u;
    down = d;
    rate = r;
    initialPrice = s0;
    p = (1 + rate - down) /(up - down);
    q = (up - rate - 1) /(up - down);
    for(int i = (MAXPERIOD+1)*(MAXPERIOD+2)/2 - 1; i >= 0; i--){
    printf("calculating idx %d \n", i);
        buffer[i] = worker(i);
    //    printf("Finish Calculating %d \n", i);
    }

    return buffer[0];
}
