//Complex Number Support
#include <math.h>
typedef struct complex{
	double real;
	double imag;
} complex;
complex* complex_add(complex* x, complex* y) {
	complex* result = malloc(sizeof(complex));
	result->real = x->real + y->real;
	result->imag = x->imag + y->imag;
	return result;
}
complex* complex_sub(complex* x, complex* y) {
	complex* result = malloc(sizeof(complex));
	result->real = x->real - y->real;
	result->imag = x->imag - y->imag;
	return result;
}
complex* complex_mul(complex* x, complex* y) {
	complex* result = malloc(sizeof(complex));
	result->real = (x->real * y->real) - (x->imag * y->imag);
	result->imag = (x->real * y->imag) + (x->imag * y->real);
	return result;
}
complex* complex_div(complex* x, complex* y) {
	complex* result = malloc(sizeof(complex));
	result->real = (x->imag*y->imag)/(y->imag*y->imag + y->real*y->real) + (x->real*y->real)/(y->imag*y->imag + y->real*y->real);
	result->imag = (-((x->real*y->imag)/(y->imag*y->imag + y->real*y->real)) + (x->imag*y->real)/(y->imag*y->imag + y->real*y->real));
	return result;
}
void complex_addx(complex* x, complex* y) {
	x->real = x->real + y->real;
	x->imag = x->imag + y->imag;
}
void complex_subx(complex* x, complex* y) {
	x->real = x->real - y->real;
	x->imag = x->imag - y->imag;
}
void complex_mulx(complex* x, complex* y) {
	x->real = (x->real * y->real) - (x->imag * y->imag);
	x->imag = (x->real * y->imag) + (x->imag * y->real);
}
void complex_divx(complex* x, complex* y) {
	x->real = (x->imag*y->imag)/(y->imag*y->imag + y->real*y->real) + (x->real*y->real)/(y->imag*y->imag + y->real*y->real);
	x->imag = (-((x->real*y->imag)/(y->imag*y->imag + y->real*y->real)) + (x->imag*y->real)/(y->imag*y->imag + y->real*y->real));
}
double complex_norm(complex* x) {
	return sqrt((x->real*x->real)+(x->imag*x->imag));
}
void complex_clone(complex* src, complex* dest) {
	dest->real = src->real;
	dest->imag = src->imag;
}
void complex_print(complex* x) {
	printf("%f+%fi\n",x->real,x->imag);
}
