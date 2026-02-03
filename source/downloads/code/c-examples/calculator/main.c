#include <stdio.h>

double calc(char operator, int a, int b);

int main() {
    int a, b;
    char operator = '?';

    do {
        printf("Please input (e.g. 1+2):");
        scanf("%d%c%d", &a, &operator, & b);
        double res = calc(operator, a, b);
        printf("%d %c %d == %.2lf\n", a, operator, b, res);
    } while (scanf("%d%c%d", &a, &operator, & b) != EOF);

    return 0;
}
