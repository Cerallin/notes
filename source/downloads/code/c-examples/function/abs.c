int abs(int x) {
    int ret; // 声明变量

    if (x > 0) {  // 如果 x > 0
        ret = x;  // 设置 ret 的值为 x
    } else {      // 否则
        ret = -x; // 设 ret 的值为 -x
    }

    return ret; // 函数返回值
}
