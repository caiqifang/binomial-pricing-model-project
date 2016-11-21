// this is the sequantial implementation of the binomial pricing model

#define MAXPERIOD 500

struct state{
    double stockPrice;
    int period;
};

class ModelS{
    private:
        double buffer[(MAXPERIOD+1)*(MAXPERIOD+2)/2]; // intermediate result buffer
        double strike; // strike price  -> we are modeling a call option
        double initialPrice;
        double up, down, rate;
        double p, q;

        double safePower(double base, int power);

        state indexToState(int idx);

        double worker(int idx);

        int nextUp(int idx, state s);

    public:
        // take in contract strike price at class allocation
        ModelS(double s);

        // up -> up factor of stock (up >= 1)
        // down -> down factor of stock (1 >= down >= 0)
        // rate -> interest rate   rate >=0, typically rate < 1
        // s0 -> original stock price
        double calculate(double up, double down,
                                  double rate, double s0);

         ~ModelS() {};
};
