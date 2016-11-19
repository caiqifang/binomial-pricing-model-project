// this is the sequantial implementation of the binomial pricing model

#define MAXPERIOD 50

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

        virtual double safePower(double base, int power) = 0;

        virtual state indexToState(int idx) = 0;

        virtual double worker(int idx) = 0;

        virtual int nextUp(int idx, state s) = 0;

    public:
        // take in contract strike price at class allocation
        virtual ModelS(double s) = 0;

        // up -> up factor of stock (up >= 1)
        // down -> down factor of stock (1 >= down >= 0)
        // rate -> interest rate   rate >=0, typically rate < 1
        // s0 -> original stock price
        virtual double calculate(double up, double down,
                                  double rate, double s0) = 0;

        virtual ~ModelS() {};
};
