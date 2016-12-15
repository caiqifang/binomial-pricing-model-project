#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <algorithm>
#include "cudaModel.h"
#include "model_s.h"

double derivative_cal(CudaModel &model, double u, double d, double r, double step, double u_step, double d_step, double r_step, double *s, double *v, int len) {
    double derivative_delta = 0;
    double delta_v[10];
    double original_v[10];

    //calculate value with delta
    model.calculate(u + u_step, d + d_step, r + r_step, s, delta_v, len);

    //calculate original value
    model.calculate(u, d, r, s, original_v, len);
    
    //calculate derivative
    for(int i = 0; i < len; ++i) {
        derivative_delta += ((delta_v[i] - v[i]) * (delta_v[i] - v[i]) - (original_v[i] - v[i]) * (original_v[i] - v[i])) / step;
    }

    return derivative_delta / len;
}

double error_cal(CudaModel &model, double u, double d, double r, double *s, double *v, int len) {
    double total_error = 0;
    double delta_v[10];

    //calculate values with update u, d, r
    model.calculate(u, d, r, s, delta_v, len);

    //calculate total error
    for(int i = 0; i < len; ++i) {
        total_error += (delta_v[i] - v[i]) * (delta_v[i] - v[i]);
    }
	
    //calculate average error
    return total_error / len;
}

//equation for adjusting learning rate according to error
double training_learing_rate(double error, double derivative) {
    return powl(2.7, error / 100000) / 10000;
}

//equation to balance step_divisor accroding to error
double step_divisor_balancer(double error, double epsilon) {
    return 0.75 * powl(0.999999999999, error - epsilon) + 0.25;
}

//equation to adjust magnitude of step_divisor according to error
double step_divisor_weight(double error) {
    return 100 - 90 * (atan(error) / M_PI * 2); 
}

//equation to balance step_limit in combintion with step_divisor_balancer and step_divisor_weight
double step_divisor(double error, double epsilon) {
    return step_divisor_weight(error) * powl(0.999999999999, (error - epsilon) / step_divisor_balancer(error, epsilon)) + 1.000000000001;
}

//equation to calculate the move step of a parameter (u and d)
double step_limit(double error, double gap, double epsilon) {
    return (atan(error / step_divisor(error, epsilon)) / M_PI * 2) * gap / step_divisor(error, epsilon);
}

//equation to calculate the move step of "r"
double r_step_limit(double error, double r_step) {
    return atan(error) / M_PI * 2 * r_step;
}

int training(CudaModel &model, double &u, double &d, double &r, double *s, double *v, double &epsilon, int &len) {
    //delta for different parameters
    double u_step = 0.00001;
    double d_step = 0.00001;
    double r_step = 0.000001;

    //learning rate for different parameters
    double u_learning_rate = 0.0001;
    double d_learning_rate = 0.0001;
    double r_learning_rate = 0.000001;
    
    //limit for different parameters
    double u_limit = 1.8;
    double d_limit = 0.5;
    double shared_limit = 1.0;

    //initilizatio of error
    double error = 10000000.0;

    //used to debug
    int count = 0;
    int step_count = 1;
    
    //used to record best training result	
    double cur_best_error = error;
    double cur_best_u = u;
    double cur_best_d = d;
    double cur_best_r = r;

    while(error > epsilon) {
        double derivative_u = derivative_cal(model, u, d, r, u_step, u_step, 0, 0, s, v, len);
        double derivative_d = derivative_cal(model, u, d, r, d_step, 0, d_step, 0, s, v, len);
        double derivative_r = derivative_cal(model, u, d, r, r_step, 0, 0, r_step, s, v, len);
        
        //calculate move step of u
        double u_cal_step = u_learning_rate * derivative_u;
        double cur_u_gap = u_cal_step > 0 ? u - shared_limit : u_limit - u;
        double u_cur_step_limit = step_limit(error, cur_u_gap, epsilon);
        double u_cur_step = u_cal_step > 0 ? std::min(u_cal_step, u_cur_step_limit) : std::max(u_cal_step, -u_cur_step_limit);
        u -= u_cur_step;
	
        //calculate move step of d
        double d_cal_step = d_learning_rate * derivative_d;
        double cur_d_gap = d_cal_step > 0 ? d - 0.5 : 1.0 - d;
        double d_cur_step_limit = step_limit(error, cur_d_gap, epsilon);
        double d_cur_step = d_cal_step > 0 ? std::min(d_cal_step, d_cur_step_limit) : std::max(d_cal_step, -d_cur_step_limit);
        d -= d_cur_step;
        
        //calculate move step of r
	double r_cal_step = r_learning_rate * derivative_r;
	double r_cur_step_limit = r_step_limit(error, r_step);
        double r_cur_step = r_cal_step > 0 ? std::min(r_cal_step, r_cur_step_limit) : std::max(r_cal_step, -r_cur_step_limit);
        r -= r_cur_step;

        //display current result
        printf("%9.12f,", u);
        printf("%9.12f,", d);
	printf("%9.12f,", r);
        
        //get error with update u, d, r
        error = error_cal(model, u, d, r, s, v, len);
	if(error < cur_best_error) {
	    cur_best_error = error;
	    cur_best_u = u;
	    cur_best_d = d;
	    cur_best_r = r;
	}
        
        //adjust learning rate according to error
        u_learning_rate = training_learing_rate(error, derivative_u);
        d_learning_rate = training_learing_rate(error, derivative_d);
        r_learning_rate = training_learing_rate(error, derivative_r);
        printf("%9.12f\n", error);
       
        count += 7;
    }
    return count;
}

int main(int argc, char** argv)
{
    clock_t t;
    CudaModel model = CudaModel(2000.0);
    t = clock();
    printf("start calculation\n");
    
    //currently two sets of training data
    /*double s[10] = {2031.38,
	2030.57,
	2019.02,
	2050.29,
	2075.91,
	2071.16,
	2064.45,
	2086.86,
	2089.99,
	2084.58};*/
    double s[10] = {1921.42, 1948.51, 1987.89, 1981.01, 1991.76, 2012.74, 2014.54, 2016.16, 2003.02, 1992.73};
    double v[10] = {36.9, 39.6, 59.3, 56.5, 59.95, 70.05, 70.0, 67.3, 62.2, 57.4};
    /*double v[10] = {69.6,
	71.55,
	66.2,
	86.9,
	102.3,
	100,
	95.42,
	104.6,
	110,
	110.95};*/
    
    //initial guess of u, d, r
    double u = 1.001, d = 0.999, r = 0.00002;
    
    //expected error    
    double epsilon = 4.5;
    
    //length of training data array
    int len = 10;
    int count = training(model, u, d, r, s, v, epsilon, len);
    printf("count %d\n", count);
    t = clock() - t;
    printf("1 time calculation %f milliseconds \n" ,((double)t)/(CLOCKS_PER_SEC/1000));
    return 0;
}


