#include <stdio.h>

int addInt(int, int);

int main() {
    int res;
    int a, b;

    scanf("%d%d", &a, &b);
    res = addInt(a, b);
    printf("%d + %d = %d\n", a, b, res);

    return 0;
}
