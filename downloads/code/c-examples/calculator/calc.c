#include <stdio.h>

double calc(char operator, int a, int b) {
    double ret;

    switch (operator) {
    case '+':
        ret = a + b;
        break;
    case '-':
        ret = a - b;
        break;
    case '*':
        ret = a * b;
        break;
    case '/':
        ret = (1.0 * a) / b;
        break;
    default:
        fprintf(stderr, "Cannot recongnize operator: %c\n", operator);
        ret = 0;
        break;
    }

    return ret;
}
