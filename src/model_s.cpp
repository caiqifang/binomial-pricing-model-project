#include <model_s.h>
#include <cmath>
#include <cfenv>
#include <stdexcept>
#include <stdlib.h>

ModelS::ModelS(double s){
    strike = s;
}

// safe calculation
double ModelS::safePower(double base, int power){
    std::feclearexcept(FE_OVERFLOW);
    std::feclearexcept(FE_UNDERFLOW); // overflow and underflow detection
    double ret = pow(base, power);
    if((bool)std::fetestexcept(FE_OVERFLOW)
              || (bool)std::fetestexcept(FE_UNDERFLOW)){
        throw std::runtime_error("power raise exception!\n");
    }
    return ret;
}

// hash function from state info to idx in buffer
int ModelS::stateToIndex(double stockPrice, int period){
    int idx = period*(period+1)/2;  // starting idx
    int maxPrice = initialPrice * safePower(up,period);
    // TODO

    return idx;
}

state ModelS::indexToState(int idx){
    state result;
    int length = (MAXPERIOD+1)*(MAXPERIOD+2)/2;
    int period = MAXPERIOD/2;
    int period_start = 0;
    int period_end = MAXPERIOD;
    int start;
    // find the period
    while(period >= 0 && period < MAXPERIOD){
        start = period*(period+1)/2;
        if( start <= idx && idx <= (start+period))
            break; // period correct
        if(idx < start){
            period = (period_start + period) / 2;
            period_end = period;
        }
        else{
            period = (period_end + period) / 2;
            period_start = period;
        }
    }
    result.period = period;
    // TODO
    return result;
}

double ModelS::worker(int idx){
    state now = indexToState(idx);
    if(now.period == MAXPERIOD){
        // leaf of tree model calculate option value
        return max(now.stockPrice - strike, 0.0);
    }
    else
    {   // need backwork induction
        int head = nextUp(idx, now.period);
        int tail = nextDown(idx, now.period);
        return 1.0/(1+rate)*(p*buffer[head] + q*buffer[tail]);
    }
}

int ModelS::nextUp(int idx){
    // TODO
}

int ModelS::nextDown(int idx){
    // TODO
}

double ModelS::calculate(double u, double d, double r, double s0){
    up = u;
    down = d;
    rate = r;
    p = (1 + rate - down) /(up - down);
    q = (up - rate - 1) /(up - down);

    for (int i = (MAXPERIOD+1)*(MAXPERIOD+2)/2 - 1; i >= 0; i--){
        buffer[i] = worker(i);
    }

    return buffer[0];
}

bool ModelS::isEqual(double a, double b){
    // error margin 0.00001
    if(std::abs(a - b) < 0.00001)
        return true;
    return false;
}
