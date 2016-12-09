// this is the sequantial implementation of the binomial pricing model

#define MAXPERIOD 600

struct state{
    long double stockPrice;
    int period;
};

class ModelS{
    private:
        long double buffer[(MAXPERIOD+1)*(MAXPERIOD+2)/2]; // intermediate result buffer
        long double strike; // strike price  -> we are modeling a call option
        long double initialPrice;
        long double up, down, rate;
        long double p, q;

        long double safePower(long double base, int power);

        state indexToState(int idx);

        long double worker(int idx);

        int nextUp(int idx, state s);

    public:
        // take in contract strike price at class allocation
        ModelS(long double s);

        // up -> up factor of stock (up >= 1)
        // down -> down factor of stock (1 >= down >= 0)
        // rate -> interest rate   rate >=0, typically rate < 1
        // s0 -> original stock price
        long double calculate(long double up, long double down,
                long double rate, long double s0);

        ~ModelS() {};
};
