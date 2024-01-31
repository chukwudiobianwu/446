#include <stdio.h>
#include <stdint.h>

uint64_t linear_congruential_method(uint64_t last, uint64_t multiplier, uint64_t increment, uint64_t modulus) {
    return ((multiplier * last) + increment) % modulus;
}

void find_lcm_cycle(uint64_t x_0, uint64_t multiplier, uint64_t increment, uint64_t modulus, uint64_t* result, int* length) {
    uint64_t x_list[1000]; // Assuming a maximum cycle length of 1000 to avoid infinite loop
    int index = 1; // Prepopulated with x_0
    x_list[0] = x_0;

    while (1) {
        uint64_t next = linear_congruential_method(x_list[index - 1], multiplier, increment, modulus);

        int found = 0;
        for (int i = 0; i < index; ++i) {
            if (x_list[i] == next) {
                found = 1;
                break;
            }
        }

        if (found) {
            break; // Done, found a cycle.
        }

        x_list[index++] = next;
    }

    *length = index;
    for (int i = 0; i < index; ++i) {
        result[i] = x_list[i];
    }
}

int main() {
    uint64_t result[1000]; // Assuming a maximum cycle length of 1000
    int length;

    find_lcm_cycle(7, 11, 0, 16, result, &length);
    printf("a=11, m=16, x_0=7 -> [");
    for (int i = 0; i < length; ++i) {
        printf("%llu", result[i]);
        if (i < length - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    find_lcm_cycle(8, 11, 0, 16, result, &length);
    printf("a=11, m=16, x_0=8 -> [");
    for (int i = 0; i < length; ++i) {
        printf("%llu", result[i]);
        if (i < length - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    find_lcm_cycle(7, 7, 0, 16, result, &length);
    printf("a=7, m=16, x_0=7 -> [");
    for (int i = 0; i < length; ++i) {
        printf("%llu", result[i]);
        if (i < length - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    find_lcm_cycle(8, 7, 0, 16, result, &length);
    printf("a=7, m=16, x_0=8 -> [");
    for (int i = 0; i < length; ++i) {
        printf("%llu", result[i]);
        if (i < length - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    return 0;
}
