#ifndef _ZERNIKE_H
#define _ZERNIKE_H


typedef struct _COMPLEX
{
	double Zr;
	double Zi;
	double Mode;
}Complex , *pComplex;

int Factorial(int);

double RadialPolynomial(double, double,double, int, int);

double RadialPolynomial(double, int, int);

int ZernikeMoments(int, int, int, unsigned char*, Complex*);

Complex Zernike(int, int, int, unsigned char*, int width);

int calcFeatNum (unsigned char order);


#endif