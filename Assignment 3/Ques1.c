#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SEED 6
#define KS_SAMPLES 10
#define CHI_SAMPLES 2000
#define KS_TABLE_VALUE 0.410 // x_0.05^2 with 9 DOF
#define CHI_TABLE_VALUE 18.3
#define CHI_INTERVALS 10

void seed_rand(unsigned int seed) {
    srand(seed);
}

int rand_int() {
    return rand();
}

int compare_floats(const void *a, const void *b) {
    double diff = (*(double*)a) - (*(double*)b);
    return (diff < 0) ? -1 : (diff > 0) ? 1 : 0;
}

void ks_test(double values[], int length, double critical_value) {
    printf("--- Beginning KS Test ---\n");
    
    // Sort values.
    qsort(values, length, sizeof(double), compare_floats);

    // Calculate d_plus.
    double d_plus_set[length];
    for (int i = 0; i < length; ++i) {
        d_plus_set[i] = (i / (double)length) - values[i];
    }
    qsort(d_plus_set, length, sizeof(double), compare_floats);
    double d_plus = d_plus_set[length - 1];
    printf("d_plus: %lf\n", d_plus);

    // Calculate d_minus.
    double d_minus_set[length];
    for (int i = 0; i < length; ++i) {
        d_minus_set[i] = values[i] - ((i - 1) / (double)length);
    }
    qsort(d_minus_set, length, sizeof(double), compare_floats);
    double d_minus = d_minus_set[length - 1];
    printf("d_minus: %lf\n", d_minus);

    // Choose the max.
    double d_max = (d_plus >= d_minus) ? d_plus : d_minus;
    printf("d_max: %lf\n", d_max);
    printf("Critical Value: %lf\n", critical_value);

    // Decide on reject/accept.
    if (d_max <= critical_value) {
        printf("Choice: Accept.\n");
    } else {
        printf("Choice: Reject.\n");
    }
}

void chi_squared_test(double values[], int samples, int intervals, double expected) {
    printf("--- Beginning Chi Squared Test ---\n");
    int expected_in_bins = samples / intervals;
    double interval_size = 1.0 / intervals;

    // Create bins.
    int bins[intervals];
    for (int i = 0; i < intervals; ++i) {
        bins[i] = 0;
    }

    // Bin values.
    for (int i = 0; i < samples; ++i) {
        int destination_bin = (int)(values[i] / interval_size);
        bins[destination_bin]++;
    }
    printf("Instance bins: ");
    for (int i = 0; i < intervals; ++i) {
        printf("%d ", bins[i]);
    }
    printf("\n");

    // Calculate chi-squared result.
    double sum = 0.0;
    for (int i = 0; i < intervals; ++i) {
        double computed = pow(bins[i] - expected_in_bins, 2) / (double)expected_in_bins;
        sum += computed;
    }
    printf("Chi Squared Result: %lf\n", sum);
    printf("Critical Value: %lf\n", expected);

    // Accept or reject based on table.
    if (sum > expected) {
        printf("Choice: Reject.\n");
    } else {
        printf("Choice: Accept.\n");
    }
}

int main() {
    // Seed and gather a sample of randoms.
    seed_rand(SEED);
    double randoms[CHI_SAMPLES];
    for (int i = 0; i < CHI_SAMPLES; ++i) {
        randoms[i] = (rand_int() % 100) / 100.0;
    }

    ks_test(randoms, KS_SAMPLES, KS_TABLE_VALUE);
    chi_squared_test(randoms, CHI_SAMPLES, CHI_INTERVALS, CHI_TABLE_VALUE);

    return 0;
}
