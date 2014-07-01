#pragma once

#include "stdfunc.h"
#include <utility>


#pragma once

// Hu matrix piont feature -- treat point neighbourhood pixels as an image
typedef struct HU_MOMENT {
	double m1;
	double m2;
	double m3;
	double m4;
	double m5;
	double m6;
	double m7;
}huMoment, *pHuMoment;

// Hu point feature -- point coordinates included
typedef struct HU_FEAT {
	huMoment moment;
	int x;
	int y;
}huFeat, *pHuFeat;


/*Create sevaral integral image of input image*/
void
createIntSImg
(
unsigned char* img,
double*	imgInt,
int width,
int height
);


/*Get the hu matrix by appointed window of the appointed image, imgInt is the integral image of source image*/
int
getHuMoments
(
unsigned char* img,
unsigned char* imgMark,
huFeat* feats,
int window,
int width,
int height
);


/*The 6 unknown coefficients for projection matrix*/
/*
	|	m0	m1	m2	|		|	x	|		|	x^	|
	|				|		|		|		|		|
	|	m3	m4	m5	|	*	| 	y	|	= 	|	y^	|
	|				|		|		|		|		|
	|	0	0	1	|		|	1	|		|	1	|
*/
typedef struct _PROJECT_MATRIX
{
	double m0;
	double m1;
	double m2;
	double m3;
	double m4;
	double m5;
}projMat;


namespace PFE
{
	enum MATCH_METHOD
	{
		EUDIST = 1,
		CORRILATION,
	};
}


/*Get the nearest point for the given point(point to match)*/
bool getNearestPoint(const huFeat* feat2match, huFeat* feats, int featNum, int& matchSeq, PFE::MATCH_METHOD method);


/*Add the matched pair to the couple*/
void
addHuNewPair(MatchCouple* couple, huFeat* featL, huFeat* featR);


/*match the hu-moment feat*/
int
matchHu(std::pair<huFeat*, huFeat*>featsPair, std::pair<int, int> pointNumPair, pMatchCouple couples);


/*get hu-moment feat and match them*/
int matchByHuMatrix
(
std::pair<unsigned char*, unsigned char*> imgPair,
std::pair<unsigned char*, unsigned char*> imgMarkPair,
std::pair<int, int> pointNumPair,
std::pair<int, int> imgWidthPair,
std::pair<int, int> imgHeightPair,
pMatchCouple* couples,
int window
);


/*Calculate the distance between expected match point and current match point*/
inline double
calcMatchDistance(MatchCouple cp, projMat mat);


/*ransac match method*/
projMat
ransac(pMatchCouple couples, int matchNum, double distThresh);
