#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SAMPLES 1000
#define NUM_INTERVALS 10
#define RANGE_BOTTOM 0.0
#define RANGE_TOP 1.0
#define TABLE_RESULT 16.9 // x_0.05^2 with 9 DOF

double* generate() {
    // Necessary for the first two...
    double* output = (double*)malloc(SAMPLES * sizeof(double));
    if (output == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    // Initialize the output.
    output[0] = ((double)rand() / RAND_MAX) * (RANGE_TOP - RANGE_BOTTOM) + RANGE_BOTTOM;
    output[1] = ((double)rand() / RAND_MAX) * (RANGE_TOP - RANGE_BOTTOM) + RANGE_BOTTOM;

    // Calculate future values.
    for (int index = 2; index < SAMPLES; ++index) {
        double next = output[index - 1] + output[index - 2];
        if (next >= 1.0) {
            next -= 1.0;
        }
        output[index] = next;
    }
    return output;
}

void chi_squared_test(double* values, int intervals, double expected) {
    int samples = SAMPLES;
    int expected_in_bins = samples / intervals;
    double interval_size = 1.0 / intervals;

    // Create bins.
    int* bins = (int*)calloc(intervals, sizeof(int));
    if (bins == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    // Bin values.
    for (int i = 0; i < samples; ++i) {
        int destination_bin = (int)(values[i] / interval_size);
        bins[destination_bin]++;
    }
    printf("Bins: ");
    for (int i = 0; i < intervals; ++i) {
        printf("%d ", bins[i]);
    }
    printf("\n");

    // (O_i - E_i)^2 / E_i
    double sum = 0.0;
    for (int i = 0; i < intervals; ++i) {
        double computed = ((double)bins[i] - expected_in_bins) * ((double)bins[i] - expected_in_bins) / expected_in_bins;
        sum += computed;
    }

    printf("Chi Squared result: %lf, expected: %lf\n", sum, expected);

    // Accept or reject based on table.
    if (sum > expected) {
        printf("Reject\n");
    } else {
        printf("Accept\n");
    }

    free(bins);
}

int main() {
    // Seed the random number generator.
    srand((unsigned int)time(NULL));

    // Generate 'randoms'.
    double* randoms = generate();
    chi_squared_test(randoms, NUM_INTERVALS, TABLE_RESULT);

    free(randoms);
    return 0;
}
