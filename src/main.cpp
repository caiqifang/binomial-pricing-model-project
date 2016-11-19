#include <stdio.h>
#include <stdlib.h>
#include <model_s.h>
#include <time.h>
#include <math.h>

int main(int argc, char** argv)
{
    clock_t t;
    ModelS model = ModelS(4);
    t = clock();
    model.calculate(2, .5, 0.03, 4);
    t = clock() - t;
    cout << "1 time calculation " << ((double)t)/CLOCKS_PER_SEC
                << " seconds" <<endl;

  return 0;
}


