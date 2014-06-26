#pragma once

#include "zernike.h"
#include "stdfunc.h"
#include <utility>


typedef struct _POINT_FEAT {
	Complex feat;
	int   x;
	int   y;
}zFeat, *pZFeat;


int
describeZernike(unsigned char* img, unsigned char* img_Mark, zFeat* feats, int width, int height, int window, int orderMax);

double
matchZernike(zFeat* featsL, int matchNum, zFeat* featsR, int pointRNum, int featNum, MatchCouple* couple);

double
calcZernikeModeDist(zFeat* featL, zFeat* featR, int featNum);

void
addZernikeNewPair(MatchCouple* pairs, int L_i, int R_j, zFeat* featsL, zFeat* featsR);

void normalZernikeFeat(zFeat* feats, int actPointNum, int featNum);

int
matchByZernike
(
std::pair<unsigned char*, unsigned char*> imgPair,
std::pair<unsigned char*, unsigned char*> imgMarkPair,
std::pair<int, int> pointNum,
pMatchCouple* couple,
int orderMax,
int window,
int width,
int height
);