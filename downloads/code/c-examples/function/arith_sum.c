// Returns nth sum of arithmetic sequence { An+B }
int arithSum(int A, int B, int n) {
    int item = (A * 0) + B; // a_0
    int sum = 0;            // to store value of sum

    for (int i = 1; i <= n; i++) {
        item = item + A;
        sum = sum + item;
    }

    return sum;
}
