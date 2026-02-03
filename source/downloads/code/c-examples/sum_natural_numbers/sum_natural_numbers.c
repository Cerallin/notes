#include <stdio.h>

int sumNaturalNumbers(int n) {
    int sum;
    // 实现你的代码

    return sum;
}

int main() {
    int sum;
    int number;

    // 从标准输入读取一个数字
    scanf("%d", &number);
    // 计算自然数前N项和并存储到 sum 变量
    sum = sumNaturalNumbers(number);
    // 输出结果到标准输出
    printf("sum(%d) = %d\n", number, sum);

    return 0;
}
