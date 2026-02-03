#include "branch.h"

// Check if number is even
bool judgeEven(int num) {
    bool ret;

    if (num % 2 == 0) {
        ret = true;
    } else {
        ret = false;
    }

    return ret;
}

// Returns -1 if num < 0, 1 if num > 0, 0 if num == 0
int judgeZero(int num) {
    int ret;

    if (num > 0) {
        ret = 1;
    } else if (num < 0) {
        ret = -1;
    } else {
        ret = 0;
    }

    return ret;
}
