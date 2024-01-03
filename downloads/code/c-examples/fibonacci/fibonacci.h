#ifndef FIBONACCI_H
#define FIBONACCI_H

#include <inttypes.h>
#include <stddef.h>

typedef uint64_t FIBO_INT;
typedef size_t size_t;
typedef FIBO_INT FIBO_MAT[2][2];

#define FIBO_INT_PLACEHOLDER   "%" PRIu64
#define size_t_PLACEHOLDER "%zu"

// Get nth item of fibonacci sequence
FIBO_INT fibonacci(size_t n);
// Recursion solution of fibonacci sequence
FIBO_INT fibonacciRecursion(FIBO_INT n);
FIBO_INT fibonacciFast(size_t num);
#endif /* FIBONACCI_H */
