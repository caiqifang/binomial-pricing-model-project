// this is the sequantial implementation of the binomial pricing model
// define maximum binomial tree level to be 600
#define MAXPERIOD 600

// private data structure
struct state{
    long double stockPrice;
    int period;
};

class ModelS{
    // private data field
    private:
        // intermediate result buffer
        long double buffer[(MAXPERIOD+1)*(MAXPERIOD+2)/2];
        // call option strike price
        long double strike;
        long double initialPrice;
        long double up, down, rate;
        long double p, q;

        // private helper functions: //////////////////////
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

        // class destructor
        ~ModelS() {};
};
