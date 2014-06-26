#pragma once


#include <stdlib.h>
#include <stdio.h>


typedef struct _MATCH_COUPLE {
	short Lx;
	short Ly;
	short Rx;
	short Ry;
}MatchCouple, *pMatchCouple;


// Debug control
#define DEBUG   1


// Debug macros
#if DEBUG

#define printf_detailed(format, ...) \
	printf("\n" "File: " __FILE__ "\n" "Line: %05d" "\n" format, __LINE__, ##__VA_ARGS__)

#define printf_simplify(format, ...) printf (format, ##__VA_ARGS__)

#else
#define printf_detailed(format, ...)
#define printf_simplify(format, ...)

#endif


#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif


// Packaged functions
void *__cdecl malloc_check(size_t _Size);
void *__cdecl calloc_check(size_t _NumOfElements, size_t _SizeOfElements);


// Solve the 3X3 hessin -- A * X = B
void solve3_3(const double* A, const double* B, double* result);


/*Create the integral image of input image*/
void
createIntImg
(
unsigned char* img,
double*	imgInt,
int width,
int height
);

void
createIntImg
(
double* img,
double*	imgInt,
int width,
int height
);