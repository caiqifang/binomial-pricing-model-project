// implememtation of the sequential model
#include "model_s.h"
#include <math.h>
#include <fenv.h>
#include <stdexcept>
#include <stdlib.h>
#include <stdio.h>

// constructor
ModelS::ModelS(long double s){
    strike = s;
}

// safe power calculation
long double ModelS::safePower(long double base, int power){
    feclearexcept(FE_OVERFLOW);
    feclearexcept(FE_UNDERFLOW); // overflow and underflow detection
    long double ret = powl(base, (long double)power);
    if((bool)fetestexcept(FE_OVERFLOW)
            || (bool)fetestexcept(FE_UNDERFLOW)){
        throw std::runtime_error("power raise exception!\n");
    }
    return ret;
}

// translate array index to position in binomial tree
//  and calcute the value in the node
state ModelS::indexToState(int idx){
    state result;
    int period = MAXPERIOD/2;
    int period_start = 0;
    int period_end = MAXPERIOD+1;
    int start;
    // find the period
    while(period >= 0 && period <= MAXPERIOD){
        start = period*(period+1)/2;
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
    long double price = initialPrice * safePower(up, period);
    int diff = idx - start;
    for (int c = 0; c < diff; c++){
        price = price / up * down;
    }
    result.stockPrice = price;
    return result;
}

// worker function that process every single nodes
long double ModelS::worker(int idx){
    state now = indexToState(idx);
    if(now.period == MAXPERIOD){
        // leaf of tree model calculate option value
        return std::max(now.stockPrice - strike, 0.0L);
    }
    else
    {
        int head = nextUp(idx, now);
        int tail = head + 1;
        return 1.0/(1+rate)*(p*buffer[head] + q*buffer[tail]);
    }
}

// find the next level index in the array given its a head coin toss
int ModelS::nextUp(int idx, state s){
    int begin = (s.period+1)*(s.period+2)/2;
    int diff = idx - (s.period+1)*(s.period)/2;
    return begin + diff;
}

// main calculate function that schedule all calculation
long double ModelS::calculate(long double u,long double d,long double r,long double s0){
    up = u;
    down = d;
    rate = r;
    initialPrice = s0;
    p = (1 + rate - down) /(up - down);
    q = (up - rate - 1) /(up - down);
    // printf(" p: %Lf  q: %Lf  \n", p, q);
    for(int i = (MAXPERIOD+1)*(MAXPERIOD+2)/2 - 1; i >= 0; i--){
        buffer[i] = worker(i);
        //    printf("Finish Calculating %d \n", i);
    }

    return buffer[0];
}
