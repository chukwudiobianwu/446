#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SEED 1
#define MEAN 1.0
#define SAMPLES 1000
#define CLASS_INTERVALS 30
#define MULTIPLE 5.0

double exponential(double rand, double mean) {
    return (-1.0 * mean) * log(rand);
}

double inverse_exponential(double rand, double mean) {
    return (-1.0 * log(1.0 - rand)) / mean;
}

int float_sort(const void *a, const void *b) {
    double arg1 = *(const double *)a;
    double arg2 = *(const double *)b;
    
    if (arg1 < arg2) {
        return -1;
    } else if (arg1 > arg2) {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    srand(SEED);

    double values[SAMPLES][2];

    for (int i = 0; i < SAMPLES; i++) {
        double num = (double)rand() / RAND_MAX;
        values[i][0] = exponential(num, MEAN);
        values[i][1] = 0.0;
    }

    qsort(values, SAMPLES, sizeof(values[0]), float_sort);

    for (int i = 0; i < SAMPLES; i++) {
        double num_for_inverse = ((double)i - 0.5) / SAMPLES;
        values[i][1] = inverse_exponential(num_for_inverse, MEAN);
    }

    printf("\n\n");

    double mean = 0.0;

    for (int i = 0; i < SAMPLES; i++) {
        mean += values[i][0];
    }

    mean /= SAMPLES;
    printf("Mean: %f\n", mean);

    double variance = 0.0;

    for (int i = 0; i < SAMPLES; i++) {
        variance += pow(values[i][0], 2.0);
    }

    variance = (variance - (SAMPLES * pow(mean, 2.0))) / (SAMPLES - 1);
    printf("Variance: %f\n", variance);

    int intervals[CLASS_INTERVALS] = {0};

    for (int i = 0; i < SAMPLES; i++) {
        int index = (int)(values[i][0] * MULTIPLE);
        if (index >= CLASS_INTERVALS) {
            index = CLASS_INTERVALS - 1;
        }
        intervals[index]++;
    }

    printf("Class Interval Buckets: ");
    for (int i = 0; i < CLASS_INTERVALS; i++) {
        printf("%d ", intervals[i]);
    }
    printf("\n");

    double chi_squared = 0.0;

    for (int i = 0; i < CLASS_INTERVALS; i++) {
        double x = (i * CLASS_INTERVALS) / (double)SAMPLES + 0.015;
        double expected_frequency = 15.0;
        double result = pow(intervals[i] - expected_frequency, 2.0) / expected_frequency;
        printf("Bin %f\n", x);
        printf("Observed %d, Expected Frequency: %f, Result: %f\n", intervals[i], expected_frequency, result);
        chi_squared += result;
    }

    printf("Chi Squared: %f\n", chi_squared);

    return 0;
}
