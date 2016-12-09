#include <stdio.h>
#include <stdlib.h>
#include "model_s.h"
#include <time.h>
#include <math.h>
#include <algorithm>
#include "cudaModel.h"

double derivative_cal(CudaModel &model, double u, double d, double r, double step, double u_step, double d_step, double r_step, double *s, double *v, int len) {
    // double *itr_s = s;
    // double *itr_v = v;
    //int len = sizeof(s) / sizeof(double);
    double derivative_delta = 0;
    double delta_v[10];
    double original_v[10];
    model.calculate(u + u_step, d + d_step, s, delta_v, len);
    model.calculate(u, d, s, original_v, len);

    for(int i = 0; i < len; ++i) {
        derivative_delta += ((delta_v[i] - v[i]) * (delta_v[i] - v[i]) - (original_v[i] - v[i]) * (original_v[i] - v[i])) / step;
    }

    // for(int i = 0; i < len; ++i) {
    //     double cur_s = *(itr_s + i);
    //     double cur_v = *(itr_v + i);
    //     double delta_v = model.calculate(u + u_step, d + d_step, r + r_step, cur_s);
    //     double original_v = model.calculate(u, d, r, cur_s);
    //     printf("delta_v %f\n", delta_v);
    //     printf("original_v %f\n", original_v);
    //     derivative_delta += ((delta_v - cur_v) * (delta_v - cur_v) - (original_v - cur_v) * (original_v - cur_v)) / step;
    //}
    //printf("len %d\n", len);
    printf("derivative_delta %f\n", derivative_delta);
    return derivative_delta / len;
}

double error_cal(CudaModel &model, double u, double d, double r, double *s, double *v, int len) {
    //double *itr_s = s;
    //double *itr_v = v;
    //int len = sizeof(s) / sizeof(double);
    double total_error = 0;
    double delta_v[10];
    model.calculate(u, d, s, delta_v, len);

    for(int i = 0; i < len; ++i) {
        total_error += (delta_v[i] - v[i]) * (delta_v[i] - v[i]);
    }
    // for(int i = 0; i < len; ++i) {
    //     double cur_s = *(itr_s + i);
    //     double cur_v = *(itr_v + i);
    //     double v = model.calculate(u, d, r, cur_s);
    //     printf("v %f\n", v);
    //     total_error += (v - cur_v) * (v - cur_v);
    // }
    printf("total_error %f\n", total_error);
    return total_error / len;
}

double training_learing_rate(double error, double derivative) {
    return powl(2.7, error / 100000) / 10000;
}

double step_divisor_balancer(double error, double epsilon) {
    return 0.75 * powl(0.999999, error - epsilon) + 0.25;
}

double step_divisor(double error, double epsilon) {
    return 8.999999 * powl(0.999999, (error - epsilon) / step_divisor_balancer(error, epsilon)) + 1.000001;
}

double step_limit(double error, double gap, double epsilon) {
    return powl(gap, epsilon / error) * gap / step_divisor(error, epsilon);
}

int training(CudaModel &model, double &u, double &d, double &r, double *s, double *v, double &epsilon, int &len) {
    double u_step = 0.00001;
    double d_step = 0.00001;
    double u_limit_step = 0.000001;
    double d_limit_step = 0.000001;
    //double r_step = 0;
    double u_learning_rate = 0.0001;
    double d_learning_rate = 0.0001;
    //double learning_rate_min_limit = 0.0000001;
    //double learning_rate_max_limit = 0.0001;

    double u_limit = 1.8;
    double d_limit = 0.5;
    //ModelS model = ModelS(4.5);
    double error = 100000.0;
    double *itr_s = s;
    double *itr_v = v;
    //printf("test %f\n", model.calculate(1.004758, 0.995445, 0, 219.07));
    int count = 0;
    int step_count = 1;
    while(error > epsilon) {
        double derivative_u = derivative_cal(model, u, d, r, u_step, u_step, 0, 0, s, v, len);
        double derivative_d = derivative_cal(model, u, d, r, d_step, 0, d_step, 0, s, v, len);
        //double derivative_r = derivative_cal(model, u, d, r, r_step, 0, 0, r_step, s, v);
        printf("derivative_u %f\n", derivative_u);
        printf("derivative_d %f\n", derivative_d);

        double u_cal_step = u_learning_rate * derivative_u;
        double cur_u_gap = u_cal_step > 0 ? u - 1.0 : 1.8 - u;
        double u_cur_step_limit = step_limit(error, cur_u_gap, epsilon);
        double u_cur_step = u_cal_step > 0 ? std::min(u_cal_step, u_cur_step_limit) : std::max(u_cal_step, -u_cur_step_limit);
        u -= u_cur_step;
        if(u <= 1.0) {
            u = 1.0 + step_count * u_limit_step;
            ++step_count;
        }
        else if(u >= u_limit) {
            u = u_limit - step_count * u_limit_step;
            ++step_count;
        }
        //u = u > 1.0 ? u : 1.0 + step_count * u_step;
        //u = std::min(u_limit, u);
        double d_cal_step = d_learning_rate * derivative_d;
        double cur_d_gap = d_cal_step > 0 ? d - 0.5 : 1.0 - d;
        double d_cur_step_limit = step_limit(error, cur_d_gap, epsilon);
        double d_cur_step = d_cal_step > 0 ? std::min(d_cal_step, d_cur_step_limit) : std::max(d_cal_step, -d_cur_step_limit);
        d -= d_cur_step;
        //max limimt
        if(d >= 1.0) {
            d = 1.0 - step_count * d_limit_step;
            ++step_count;
        }
        else if(d <= d_limit) {
            d = d_limit + step_count * d_limit_step;
            ++step_count;
        }
        //d = d < 1.0 ? d : 1.0 - step_count * d_step;
        //min limit
        //d = std::max(d_limit, d);
        printf("step_count %d\n", step_count);
        printf("u %f\n", u);
        printf("d %f\n", d);
        //r -= r_learning_rate * derivative_r;
        error = error_cal(model, u, d, r, s, v, len);
        u_learning_rate = training_learing_rate(error, derivative_u);
        //u_learning_rate = std::min(training_learing_rate(error), learning_rate_max_limit);
        d_learning_rate = training_learing_rate(error, derivative_d);
        //d_learning_rate = std::min(training_learing_rate(error), learning_rate_max_limit);
        printf("error %f\n", error);
        printf("learning_rate %f\n", u_learning_rate);
        count += 5 * len;
    }
    return count;
}

int main(int argc, char** argv)
{
    clock_t t;
    CudaModel model = CudaModel(2000.0);
    t = clock();
    printf("start calculation\n");
    double s[10] = {1921.42, 1948.51, 1987.89, 1981.01, 1991.76, 2012.74, 2014.54, 2016.16, 2003.02, 1992.73};
    double v[10] = {36.9, 39.6, 59.3, 56.5, 59.95, 70.05, 70.0, 67.3, 62.2, 57.4};
    //int len = sizeof(s) / sizeof(double);
    double u = 1.0001, d = 0.9999, r = 0;
    double epsilon = 0.25;
    int len = 10;
    int count = training(model, u, d, r, s, v, epsilon, len);
    printf("count %d\n", count);
    //printf("u %f\n", u);
    //printf("d %f\n", d);
    //printf("r %f\n", r);
    //double result = model.calculate(1.23, .89, 0.03, 3);
    t = clock() - t;
    printf("1 time calculation %f milliseconds \n" ,((double)t)/(CLOCKS_PER_SEC/1000));
    //printf(" result = %f \n", result);
    return 0;
}


