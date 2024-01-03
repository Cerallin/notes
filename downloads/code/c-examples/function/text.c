#include <stdio.h>

#include "functions.h"

int main() {
    int num;
    int a, b, n;
    double radius;

    num = -10;
    printf("sign of %d is %d, ", num, sign(num));
    printf("abs(%d) = %d\n", num, abs(num));

    num = 8;
    printf("sign of %d is %d, ", num, sign(num));
    printf("abs(%d) = %d\n", num, abs(num));

    num = 0;
    printf("for y = x^2 + 2x - 1, y(%d) = %d\n", num, quadratic(num));
    num = 1;
    printf("for y = x^2 + 2x - 1, y(%d) = %d\n", num, quadratic(num));
    num = 2;
    printf("for y = x^2 + 2x - 1, y(%d) = %d\n", num, quadratic(num));

    radius = 1;
    printf("area of circle with radius %.4lf is %.4lf\n", radius,
           circleArea(radius));
    radius = 1.0 / 2;
    printf("area of circle with radius %.4lf is %.4lf\n", radius,
           circleArea(radius));
    radius = 2.333333;
    printf("area of circle with radius %.4lf is %.4lf\n", radius,
           circleArea(radius));

    a = 1;
    b = 2;
    printf("add(%d, %d) = %d\n", a, b, add(a, b));

    a = 2;
    b = 1;
    n = 3;
    printf("sum(n = %d) of {%dx + %d} = %d\n", n, a, b, arithSum(a, b, n));

    a = 1;
    b = 0;
    n = 5;
    printf("sum(n = %d) of {%dx + %d} = %d\n", n, a, b, arithSum(a, b, n));

    return 0;
}
