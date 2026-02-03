#include "fibonacci.h"

#define LOG_SQRT_5 log(sqrt(5))
#define MAGIC_NUM  log((1 + sqrt(5)) / 2)

FIBO_INT fibonacci(size_t n) {
    FIBO_INT ret;

    if (n <= 2) {
        // for n <= 2, i.e. n \in {1,2}, result is 1
        ret = 1;
    } else {
        FIBO_INT f1, f2;
        // initialize both f1 and f2 to 1
        f1 = f2 = 1;
        for (size_t i = 2; i < n; i++) {
            // recursice defination of fibonacci sequence
            FIBO_INT tmp = f1 + f2;
            f1 = f2;
            f2 = tmp;
        }

        ret = f2;
    }

    return ret;
}

FIBO_INT fibonacciRecursion(size_t n) {
    FIBO_INT ret;

    if (n <= 2) {
        ret = 1;
    } else {
        ret = fibonacciRecursion(n - 1) + fibonacciRecursion(n - 2);
    }

    return ret;
}

// Multiply matrices A and B, and restore results to A.
static inline void matrixMultiply(FIBO_INT A[2][2], FIBO_INT B[2][2]) {
    FIBO_INT result[2][2];

    result[0][0] = (A[0][0] * B[0][0]) + (A[0][1] * B[1][0]);
    result[0][1] = (A[0][0] * B[0][1]) + (A[0][1] * B[1][1]);
    result[1][0] = (A[1][0] * B[0][0]) + (A[1][1] * B[1][0]);
    result[1][1] = (A[1][0] * B[0][1]) + (A[1][1] * B[1][1]);

    A[0][0] = result[0][0];
    A[0][1] = result[0][1];
    A[1][0] = result[1][0];
    A[1][1] = result[1][1];
}

FIBO_INT fibonacciFast(size_t num) {
    size_t n = num - 1;
    if (n <= 1) {
        return 1;
    }
    /**
     * [F(n + 1),   F(n)      ]
     * [F(n),       F(n - 1)  ]
     */
    FIBO_INT result[2][2] = {{1, 1}, {1, 0}};
    FIBO_INT base[2][2] = {{1, 1}, {1, 0}};

    n--;
    while (n > 0) {
        if (n % 2 == 1) {
            matrixMultiply(result, base);
        }
        matrixMultiply(base, base);
        n /= 2;
    }

    return result[0][0];
}
