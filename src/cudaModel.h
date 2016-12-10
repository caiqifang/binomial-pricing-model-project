#ifndef __CUDA_MODEL_H__
#define __CUDA_MODEL_H__

#define MAXLEVEL 600
#define BUFSIZE (MAXLEVEL+1)*(MAXLEVEL+2)/2
#define MAXLENGTH 20
#define THREAD_PER_BLOCK 128

/////////// KERNEL ////////////
// kernelFinalStage(int length, int maxL,  double strike,
// double* device_buf, double* device_u, double* device_d, double* device_s)
//
// kernelCalc( int level, int maxL, int length, double* device_p
//  double* device_q, double* device_buf) ;
//
// kernelOutput ( double* device_output, double* device_buf, int length, int maxL)

///////////// HOST/////////////

class CudaModel
{
    private:
        double strike;
        double  device_p;
        double  device_q;
        double  device_r;
        double* device_buf;
        double* device_output;
        double  device_u;
        double  device_d;
        double* device_s;

    public:
        // Class constructor,   need to have a strike price to create
        CudaModel(double s);
        ~CudaModel();

        // Host code:
        // calculate function that can handle up to 20 groups of calculation
        // array_u -> an array of up factors  u
        // array_d -> an array of down factors d
        // array_s -> an array of original stock price s0
        // array_output-> place to store calculation result
        // length-> number of group you want to calculate  up to 10
        // All data match by their array index
        void calculate(double up, double down, double rate, double* array_s,
                double* array_output, int length);
};
#endif
