#include "fibonacci.h"

#include <stdio.h>

int main() {
    size_t num;

    scanf("%zu", &num);
    printf("Fibonacci normal O(n): \t\t" FIBO_INT_PLACEHOLDER "\n",
           fibonacci(num));
    printf("Fibonacci fast O(log(n)): \t" FIBO_INT_PLACEHOLDER "\n",
           fibonacciFast(num));
    printf("\n");

    return 0;
}
