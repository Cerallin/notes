#define PI 3.14159

int zero = 0;
float pi = 3.14159;
double e = 2.718281828;

// Returns a/b
int divide_int(int a, int b) {
    int ret = a / b;
    return ret; // ret是return的缩写
}

double divide_double(double a, double b) {
    double ret = a / b;
    return ret;
}

// Calculate area of a circle
double circle_area(double radius) {
    double area = PI * radius * radius;
    return area;
}
