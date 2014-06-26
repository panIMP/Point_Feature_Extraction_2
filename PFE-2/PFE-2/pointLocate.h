#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\calib3d\calib3d.hpp>


#define _CIRCLR_NEIGHBOUR 1


/*get the template to generate hessin pyramids*/
int*
getHessinTemplate(void);


/*OpenCV method to find the interest points by using harris*/
void
opecvHarris
(
const cv::Mat& img,
cv::Mat& imgMark,
double harrisThresh,
double binaryThresh
);


/*Generate the hessin-pyramid*/
void
createHessinPyramid
(
unsigned char* img,
double* imgInt,
double* imgHesPyr,
int octaveNum,
int layerNum,
const int* hesTempl,
unsigned char* imgMark,
int width,
int height,
double thresh
);


/*Display hessin pyramid images*/
void
displayHessinPyramid
(
double* imgHesPyr,
int octaveNum,
int layerNum,
int width,
int height
);


/*Non-maximum suppression*/
void
suppressNonMaximum
(
double* imgHesPyr,
int octaveNum,
int layerNum,
const int* hesTempl,
unsigned char* imgMark,
int width,
int height
);


/*Conduct accurate sub-pixel interest point location & edge suppression & threshold suppression*/
int
locateRefine
(
double* imgHesPyr,
int octaveNum,
int layerNum,
const int* hesTempl,
double fxThresh,
double edgeThresh,
unsigned char* imgMark
);


/*Using surf to find the interest points by appointed surf parameters*/
int
surf
(
unsigned char* img,
unsigned char* imgMark,
int width,
int height,
int octaveNum,
int layerNum,
const int* hesTempl,
double thresh
);