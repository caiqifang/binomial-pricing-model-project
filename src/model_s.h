// this is the sequantial implementation of the binomial pricing model

#define MAXPERIOD 50

struct state{
    double stockPrice;
    int period;
};

class ModelS{
    private:
        double buffer[(MAXPERIOD+1)*(MAXPERIOD+2)/2]; // intermediate result buffer

    public:
        double strike; // strike price  -> we are modeling a call option
        double initialPrice;
        double up, down, rate;

        virtual ModelS() {};

        virtual double safePower(double base, int power) = 0;

        virtual int stateToIndex(double stockPrice, int period) = 0;

        virtual state indexToState(int idx) = 0;

        virtual double worker(double p, double q, double r, double s,
                double u, double d, int startPeriod, int stopPeriod) = 0;

        virtual double calculate(double up, double down,
                                  double rate, double s0) = 0;

        virtual ~ModelS() {};
};
