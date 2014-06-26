#include "zernike.h"
#include "stdfunc.h"

#include <math.h>

/*------------------------
求阶乘
-------------------------*/
int Factorial(int x)
{
	int fact = x;
	if(x == 0)
		return 1;
	while(--x != 0)
	{
		fact *= x;
	}
	
	return fact;
}
/*---------------------------
定义：径向函数基
rou:为半径
n:为order
m:为repetition
输出为：系数
----------------------------*/
double RadialPolynomial(double rou , int n , int m)
{
	double radial = 0;	//输出系数
	int	s;				//环 控制
	double c;
	
	if(m < 0)
		m = -m;

	for(s = 0;s <= (n - m)/2;s++)
	{
		if(s%2 == 0)
		{
			c = Factorial(n-s) / (Factorial(s) * Factorial((n+m)/2-s) * Factorial((n-m)/2-s));
		}
		else
		{
			c = - Factorial(n-s) / (Factorial(s) * Factorial((n+m)/2-s) * Factorial((n-m)/2-s));
		}
		radial += c * pow(rou , n-2*s);
	}
	return radial;
}

/*---------------------------
定义：径向函数基(chong方法）
Rm4:第m+4个系数
Rm2:第m+2个
rou:为半径
n:为order
m:为repetition
输出为：系数
----------------------------*/
double RadialPolynomial(double Rm4 , double Rm2 , double rou , int n , int m)
{
	double h1 , h2 , h3;
	double radial;
	
	h3 = - 4.0 * (m+2) * (m+1) / ((n+m+2) * (n-m));
	h2 = h3 * (n+m+4) * (n-m-2) / 4.0 / (m+3) + m + 2;
	h1 = (m+4) * (m+3) / 2.0 - (m+4) * h2 + h3 * (n+m+6) * (n-m-4) / 8.0;

	radial = h1 * Rm4 + (h2 + h3 / (rou * rou)) *Rm2;
	
	return radial;
}

/*-----------------------------
求zernike moment，为n阶的所有m系数
n:order
imageWidth:图像宽
imageHeight:图像高
image:图像源
z:传出参数，所有的m系数
输出程序运行状态
-------------------------------*/
/*
int ZernikeMoments(int n , int imageWidth , int imageHeight , 
				   unsigned char *image , Complex * z)
{
	int N;
	int cnt = 0;
	int i , j;
	double rou , theta;
	double * radial;
	int count;
	int m;			//repetition
	unsigned char h1,h2,h3,h4,h5,h6,h7,h8;

	radial = (double *)malloc_check(sizeof(double) * n/2);
	if(imageWidth > imageHeight)
		N = imageWidth;
	else
		N = imageHeight;

	for(i = 0;i <= n/2;i++)
	{
		z[i].Zr = 0;
		z[i].Zi = 0;
	}

	for(i = 0;i < N/2;i++)//height
	{
		for(j = N-1-i;j < N;j++)//width
		{
			rou = sqrt(((2*i-N+1)*(2*i-N+1)+(N-1-2*j)*(N-1-2*j))/2.0)/(N-1);
			
			if(rou <= 1)
			{
				count = 0;
				theta = atan2((double)(N-1-2*i),(double)(2*j-N+1));
				h1 = image[i*imageWidth+j];
				h2 = image[(N-1-j)*imageWidth+N-1-i];
				h3 = image[(N-1-j)*imageWidth+i];
				h4 = image[i*imageWidth+N-1-j];
				h5 = image[(N-1-i)*imageWidth+N-1-j];
				h6 = image[j*imageWidth+i];
				h7 = image[j*imageWidth+N-1-i];
				h8 = image[(N-1-i)*imageWidth+j];
				for(m = n;m >= 0; m -= 2)
				{
					if(m == n)
						radial[count] = pow(rou , n);
					else if(m == n - 2)
						radial[count] = n * radial[count-1] - (n - 1) * pow(rou , n - 2);
					else
						radial[count] = RadialPolynomial(radial[count - 2] , 
														radial[count - 1] ,rou,n,m);
					
					if(i + j == N - 1)
					{
						switch(m%4)
						{
						case 0:
							z[count].Zr += (h1 + h4 + h5 + h8) * radial[count];
							z[count].Zi += 0;
							break;
						case 1:
							z[count].Zr += (h1 - h4 - h5 + h8) * radial[count] * cos(m*theta); 
							z[count].Zi += (h1 + h4 - h5 - h8) * radial[count] * sin(m*theta); 
							break;
						case 2:
							z[count].Zr += 0;
							z[count].Zi += (h1 - h4 + h5 - h8) * radial[count];
							break;
						case 3:
							z[count].Zr += (h1 - h4 - h5 + h8) * radial[count] * cos(m*theta);
							z[count].Zi += (h1 + h4 - h5 - h8) * radial[count] * sin(m*theta);
							break;
						}
					}
					else
					{
						switch(m%4)
						{
						case 0:
							z[count].Zr += (h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8)
											* radial[count] * cos(m*theta);
							z[count].Zi += (h1 - h2 + h3 - h4 + h5 - h6 + h7 - h8)
											* radial[count] * sin(m*theta);
							break;
						case 1:
							z[count].Zr += (h1 - h4 - h5 + h8) * radial[count] * cos(m*theta) 
											+ (h2 - h3 - h6 + h7) * radial[count] * sin(m*theta);
							z[count].Zi += (h1 + h4 - h5 - h8) * radial[count] * sin(m*theta) 
											+ (h2 + h3 - h6 - h7) * radial[count] * cos(m*theta);
							break;
						case 2:
							z[count].Zr += (h1 - h2 - h3 + h4 + h5 - h6 - h7 + h8)
											* radial[count] * cos(m*theta);
							z[count].Zi += (h1 + h2 - h3 - h4 + h5 + h6 - h7 - h8)
											* radial[count] * sin(m*theta);
							break;
						case 3:
							z[count].Zr += (h1 - h4 - h5 + h8) * radial[count] * cos(m*theta) 
											+ (-h2 + h3 + h6 - h7) * radial[count] * sin(m*theta);
							z[count].Zi += (h1 + h4 - h5 - h8) * radial[count] * sin(m*theta) 
											+ (-h2 - h3 + h6 + h7) * radial[count] * cos(m*theta);
							break;
						}
					}
					count ++;
				}
				if(i + j == N - 1)
					cnt += 4;
				else
					cnt += 8;
			}
		}
		
	}
	for(i = 0;i <= n/2;i++)
	{
		z[i].Zr = (n+1)*z[i].Zr/cnt;
		z[i].Zi = (n+1)*z[i].Zi/cnt;
	}

	return 1;
}*/
/*-----------------------------
求zernike moment，一个重载函数完成n阶以下的所有值
n:order
imageWidth:图像宽
imageHeight:图像高
image:图像源
z:传出参数，所有的m系数
输出程序运行状态
-------------------------------*/

int ZernikeMoments(int nOrigin , int imageWidth , int imageHeight , 
				   unsigned char *image , Complex * z)
{
	int N;
	int cnt = 0;
	int i , j;
	double rou , theta;
	double * radial;
	int count;
	int m;			//repetition
	int n = nOrigin;//ORDER
	unsigned char h1,h2,h3,h4,h5,h6,h7,h8;

	int number = 0;
	do
	{
		number += n / 2 + 1;
		n --;
	}while(n != -1);
	radial = (double *)malloc_check(sizeof(double) * number);
	if(imageWidth > imageHeight)
		N = imageWidth;
	else
		N = imageHeight;
	
	for(i = 0;i < number;i++)
	{
		z[i].Zr = 0;
		z[i].Zi = 0;
	}

	for(i = 0;i < N/2;i++)//height
	{
		for(j = N-1-i;j < N;j++)//width
		{
			rou = sqrt(((2*i-N+1)*(2*i-N+1)+(N-1-2*j)*(N-1-2*j))/2.0)/(N-1);

			if(rou <= 1)
			{
				count = 0;
				theta = atan2((double)(N-1-2*i),(double)(2*j-N+1));
				h1 = image[i*imageWidth+j];
				h2 = image[(N-1-j)*imageWidth+N-1-i];
				h3 = image[(N-1-j)*imageWidth+i];
				h4 = image[i*imageWidth+N-1-j];
				h5 = image[(N-1-i)*imageWidth+N-1-j];
				h6 = image[j*imageWidth+i];
				h7 = image[j*imageWidth+N-1-i];
				h8 = image[(N-1-i)*imageWidth+j];

				n = nOrigin;
				while(n != -1)
				{
					for(m = n;m >= 0; m -= 2)
					{
						if(m == n)
							radial[count] = pow(rou , n);
						else if(m == n - 2)
							radial[count] = n * radial[count-1] - (n - 1) * pow(rou , n - 2);
						else
							radial[count] = RadialPolynomial(radial[count - 2] , 
															 radial[count - 1] ,rou,n,m);

						if(i + j == N - 1)
						{
							switch(m%4)
							{
							case 0:
								z[count].Zr += (h1 + h4 + h5 + h8) * radial[count];
								z[count].Zi += 0;
								break;
							case 1:
								z[count].Zr += (h1 - h4 - h5 + h8) * radial[count] * cos(m*theta); 
								z[count].Zi += (h1 + h4 - h5 - h8) * radial[count] * sin(m*theta); 
								break;
							case 2:
								z[count].Zr += 0;
								z[count].Zi += (h1 - h4 + h5 - h8) * radial[count];
								break;
							case 3:
								z[count].Zr += (h1 - h4 - h5 + h8) * radial[count] * cos(m*theta);
								z[count].Zi += (h1 + h4 - h5 - h8) * radial[count] * sin(m*theta);
								break;
							}
						}
						else
						{
							switch(m%4)
							{
							case 0:
								z[count].Zr += (h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8)
									* radial[count] * cos(m*theta);
								z[count].Zi += (h1 - h2 + h3 - h4 + h5 - h6 + h7 - h8)
									* radial[count] * sin(m*theta);
								break;
							case 1:
								z[count].Zr += (h1 - h4 - h5 + h8) * radial[count] * cos(m*theta) 
									+ (h2 - h3 - h6 + h7) * radial[count] * sin(m*theta);
								z[count].Zi += (h1 + h4 - h5 - h8) * radial[count] * sin(m*theta) 
									+ (h2 + h3 - h6 - h7) * radial[count] * cos(m*theta);
								break;
							case 2:
								z[count].Zr += (h1 - h2 - h3 + h4 + h5 - h6 - h7 + h8)
									* radial[count] * cos(m*theta);
								z[count].Zi += (h1 + h2 - h3 - h4 + h5 + h6 - h7 - h8)
									* radial[count] * sin(m*theta);
								break;
							case 3:
								z[count].Zr += (h1 - h4 - h5 + h8) * radial[count] * cos(m*theta) 
									+ (-h2 + h3 + h6 - h7) * radial[count] * sin(m*theta);
								z[count].Zi += (h1 + h4 - h5 - h8) * radial[count] * sin(m*theta) 
									+ (-h2 - h3 + h6 + h7) * radial[count] * cos(m*theta);
								break;
							}
						}
						count ++;
					}
					-- n;
				}
				if(i + j == N - 1)
					cnt += 4;
				else
					cnt += 8;
			}
		}
	}
	number = 0;
	for(;nOrigin >= 0;nOrigin--)
	{
		for(i = 0;i <= nOrigin / 2;i++)
		{
			z[i + number].Zr = (nOrigin+1)*z[i + number].Zr/cnt;
			z[i + number].Zi = -(nOrigin+1)*z[i + number].Zi/cnt;
		}
		number += nOrigin / 2 + 1;
	}

	return 1;
}

Complex Zernike(int n , int m , int N , unsigned char *image, int width) // n >= m  && (n - m) % 2 = 0
{
	Complex z;
	int cnt = 0;
	int i , j;
	double rou , theta , radial;
//	int N = 20;
	int flag = 0;
	z.Zi = 0;
	z.Zr = 0;
//	unsigned char h1,h2,h3,h4,h5,h6,h7,h8;
	if(m < 0)
	{
		flag = 1;
		m = -m;
	}

	for(i = 0;i < N;i++)
	{
		for(j = 0;j < N;j++)
		{
			rou = sqrt(((2*i-N+1)*(2*i-N+1)+(N-1-2*j)*(N-1-2*j))/2.0)/(N-1);

			if(rou <= 1)
			{
				radial = RadialPolynomial(rou,n,m);
				theta = atan2((double)(N-1-2*i),(double)(2*j-N+1));

//				z.Zr += image[i*N+j] * radial * cos(m*theta);
//				z.Zi -= image[i*N+j] * radial * sin(m*theta);
				z.Zr += image[(i-N/2)*width+j-N/2] * radial * cos(m*theta);
				z.Zi -= image[(i-N/2)*width+j-N/2] * radial * sin(m*theta);

				cnt ++;
			}

		}
	}

	if(flag == 0)
	{
		z.Zi = (n+1) * z.Zi / cnt;
	}
	else 
		z.Zi = -(n+1) * z.Zi / cnt;
	z.Zr = (n+1) * z.Zr / cnt;
	z.Mode = sqrt(z.Zi * z.Zi + z.Zr * z.Zr);

	return z;
}


int calcFeatNum(unsigned char order)
{
	unsigned char  i = 0;
	int featNum = 0;

	for (i = 0; i <= order; ++i) {
		featNum += (1 + i / 2);
	}

	return featNum;
}