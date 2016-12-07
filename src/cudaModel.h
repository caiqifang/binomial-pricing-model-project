#ifndef __CUDA_MODEL_H__
#define __CUDA_MODEL_H__

class CudaModel
{
    private:
        double strike;
        double* device_p;
        double* device_q;
        double* device_buf;
        double* device_u;
        double* device_d;
        double* device_s;
        double array_p[MAXLENGTH];
        double array_q[MAXLENGTH];

    public:
        CudaModel(double s);
        ~CudaModel();

        void calculate(double* array_u, double* array_d, double* array_s,
                    double* array_output, int length);
}
#endif
