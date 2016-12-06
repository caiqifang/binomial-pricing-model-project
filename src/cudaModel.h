#ifndef __CUDA_MODEL_H__
#define __CUDA_MODEL_H__

class CudaModel
{
    private:
        long double strike;

    public:
        CudaModel(long double s);
        ~CudaModel();

        void init();
        void train();
        void calculate();
        void getOutput();

}

#endif
