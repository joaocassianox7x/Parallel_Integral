#include <stdio.h>
#include <stdlib.h>
#include <omp.h> //compile using -fopenmp floag
#include <math.h> //math library
#include <time.h>


double my_fun(double x)
{
    return 1/(x*x+1);
}

double paralel_integral(double xmin, double xmax, int n, int part, int number_divisions)
{
    double integral = 0;
    double delta = (xmax-xmin)/n;

    int is_first = (part == 0); //take the first point only one time
    int is_last = (part == number_divisions-1); //that's to avoid a sum over two equal points

    for(int i=is_first; i<n; i++)
    {
        integral += my_fun(xmin+i*delta)*delta;
    }

    integral = (is_last*my_fun(xmax)+is_first*my_fun(xmin))*0.5*delta+integral; //trapezoidal rule

    return integral;
}

int main()
{
    int n = pow(10,6); //number of points
    int div = 10; //number of interval divisions 
    double xmin = 0;
    double xmax = 1.0;
    double delta = (xmax-xmin)/div;
    double integral;

    clock_t t0, t1; // to measure time
    double ttot; // total time
    double tthread; // time per thread

    double time_spent; //time spent on for


    FILE *fp = fopen("output_time_exe.txt","w");

    for(int thrs = 2; thrs<= 4; thrs++)
    {
        integral = 0.;
        t0 = clock();
        omp_set_num_threads(thrs); //by default uses env variable OMP_NUM_THREADS 
        #pragma omp parallel for reduction(+: integral) //pragma give a instruction to compiler
        for(int i=0;i<div;i++) integral += paralel_integral(xmin+i*delta, xmin+(i+1)*delta, n, i, div);

        t1 = clock();
        
        time_spent = (double)(t1-t0)/(CLOCKS_PER_SEC*thrs);

        printf("%f\n",integral);
        fprintf(fp,"Value of integral: %f with %d threads spent %f s\n",integral,thrs,time_spent);
    }
    fclose(fp);

    return 0;
}

