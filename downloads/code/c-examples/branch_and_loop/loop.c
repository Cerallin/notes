// Sum of the first n terms of natural numbers.
// Written with for loop.
int sum_for(int num) {
    int total;

    for (int n = 0; n < num; n++) {
        total += n;
    }

    return total;
}

// Sum of the first n terms of natural numbers
// Written with while loop.
int sum_while(int num) {
    int total;

    int n = 0;
    while (n < num) {
        total += n;
        n++;
    }

    return total;
}
