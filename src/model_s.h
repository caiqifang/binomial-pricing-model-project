// this is the sequantial implementation of the binomial pricing model

#define RECURLENGTH 10
#define MAXPERIOD 30

class ModelS{
    private:
        int timePeriod; // n period of time step

        int totalStage; // break the state process to reduce memory consumed

        double strike; // strike price  -> we are modeling a call option

        double *buffer; // intermediate result buffer

    public:
        virtual ModelS(int period, double k);

        virtual double safePower(double base, int power) = 0;

        virtual double worker(double p, double q, double r, double s,
                double u, double d, int startPeriod, int stopPeriod) = 0;

        virtual double calculate(double up, double down, double rate, double s0) = 0;

        virtual ~ModelS();
};
