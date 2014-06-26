#include "zernikeMatch.h"
#include "stdfunc.h"
#include <math.h>


void 
normalZernikeFeat(zFeat* feats, int actPointNum, int featNum) {
	int i;
	int fNum;

	double maxMode = 0.0;
	double mode;

	zFeat* featPtr = feats;

	for (fNum = 0; fNum < featNum; ++fNum) {
		if (fNum == 86) {
			fNum++;
			fNum--;
		}

		maxMode = 1.0;

		featPtr = feats + fNum;
		for (i = 0; i < actPointNum; ++i) {
			mode = featPtr->feat.Mode;

			if (mode > maxMode) {
				maxMode = mode;
			}

			featPtr += featNum;
		}

		printf_simplify("******Feat : %4d, maxMode: %lf*****\n", fNum, maxMode);

		featPtr = feats + fNum;
		for (i = 0; i < actPointNum; ++i) {
			featPtr->feat.Zr /= maxMode;
			featPtr->feat.Zi /= maxMode;

			printf_simplify("Point: %4d, Feat: %4d, Zr: %lf, Zi: %lf\n",
				i, fNum, featPtr->feat.Zr, featPtr->feat.Zi);

			featPtr += featNum;
		}
	}
}


int
describeZernike(unsigned char* img, unsigned char* imgMark, zFeat* feats, int width, int height, int window, int orderMax)
{
	int	i = 0;
	int startPoint = width * window / 2 + window / 2;
	int endPoint = width * (height - window / 2) - window / 2;
	int pNumCounter = 0;
	int actPointNum = 0;

	int orderN = 0;
	int orderM = 0;
	int featNum = calcFeatNum(orderMax);


	for (i = startPoint; i < endPoint; ++i) {
		int fNum = 0;
		int x, y;

		if (imgMark[i] == 0){
			continue;
		}

		x = i % width;
		y = i / width;
		printf_simplify("\n====================================================\n");
		printf_simplify("point = %d, x = %d, y = %d\n", pNumCounter, x, y);

		// get feat for each point
		for (orderN = 0; orderN <= orderMax; ++orderN)
		{
			// ! requirements : m <= n && (n-m) % 2 = 0
			for (orderM = 0; orderM <= orderN; orderM++) {
				if ((orderN - orderM) % 2 != 0) {
					continue;
				}

				Complex z = Zernike(orderN, orderM, window, &img[i], width);
				feats[pNumCounter * featNum + fNum].feat = z;
				feats[pNumCounter * featNum + fNum].x = x;
				feats[pNumCounter * featNum + fNum].y = y;
				fNum++;

				printf_simplify("fNum = %4d, n = %4d, m = %4d; shi:%lf, xu:%lf, mode:%lf\n", fNum, orderN, orderM, z.Zr, z.Zi, z.Mode);
			}
		}

		// calculate real point feature num
		pNumCounter++;
	}
	actPointNum = pNumCounter;
	printf_detailed("actPointNum = %d\n\n", actPointNum);

	/*------------------- Normalization ---------------------*/
	normalZernikeFeat(feats, actPointNum, featNum);

	return actPointNum;
}



double 
matchZernike(zFeat* featsL, int matchNum, zFeat* featsR, int pointRNum, int featNum, MatchCouple* couple) {
	int LeftNum, rightNum;
	int matchRightNum;

	double zModeDist = 0.0;
	double minZModeDist = 1000.0;

	zFeat* pointsLHead = featsL;
	zFeat* pointsRHead = featsR;

	for (LeftNum = 0; LeftNum < matchNum; ++LeftNum) {
		for (rightNum = 0; rightNum < pointRNum; ++rightNum) {
			zModeDist = calcZernikeModeDist(featsL, featsR, featNum);

			if (zModeDist < minZModeDist) {
				minZModeDist = zModeDist;
				matchRightNum = rightNum;
			}
			printf_simplify("L: %4d, R: %4d, zModeDist: %lf\n", LeftNum, rightNum, zModeDist);
			featsR += featNum;
		}

		addZernikeNewPair(couple++, LeftNum, matchRightNum, &pointsLHead[featNum * LeftNum], &pointsRHead[featNum * matchRightNum]);

		featsR -= pointRNum * featNum;
		featsL += featNum;
		minZModeDist = 1000.0;
		matchRightNum = 0;
	}

	return 1.0;
}


double 
calcZernikeModeDist(zFeat* featL, zFeat* featR, int featNum) {
	int i;

	double zModeDist = 0.0;
	//    double modeL, modeR;
	double modeDist;
	double modeL_zi, modeL_zr, modeR_zi, modeR_zr;

	for (i = 0; i < featNum; ++i) {
		//        modeL = pointL[i].feat.Mode;
		//        modeR = pointR[i].feat.Mode;

		//        modeDist = modeL - modeR;
		//        zModeDist += modeDist * modeDist;

		modeL_zi = featL[i].feat.Zi;
		modeL_zr = featL[i].feat.Zr;
		modeR_zi = featR[i].feat.Zi;
		modeR_zr = featR[i].feat.Zr;

		modeDist = modeL_zi - modeR_zi;
		zModeDist += modeDist * modeDist;
		modeDist = modeL_zr - modeR_zr;
		zModeDist += modeDist * modeDist;
	}

	zModeDist = sqrt(zModeDist);

	return zModeDist;
}


void 
addZernikeNewPair(MatchCouple* pairs, int L_i, int R_j, zFeat* featsL, zFeat* featsR) 
{
	static int num = 0;

	pairs->Lx= featsL->x;
	pairs->Ly = featsL->y;
	pairs->Rx = featsR->x;
	pairs->Ry = featsR->y;

	printf_simplify("Pair %4d X,Y: (%4d, %4d), (%4d, %4d)\n",num++, pairs->Lx, pairs->Ly, pairs->Rx, pairs->Ry);
}


// Get zernike feat
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
)
{
	unsigned char* leftImg = imgPair.first;
	unsigned char* rightImg = imgPair.second;
	unsigned char* leftImgMark = imgMarkPair.first;
	unsigned char* rightImgMark = imgMarkPair.second;
	int leftPointNum = pointNum.first;
	int rightPointNum = pointNum.second;

	// get feature
	int featNum = calcFeatNum(orderMax);
	zFeat* featsL = (zFeat*)calloc_check(leftPointNum * featNum, sizeof(zFeat));
	int actLeftPointNum = describeZernike(leftImg, leftImgMark, featsL, width, height, window, orderMax);
	zFeat* featsR = (zFeat*)calloc_check(rightPointNum * featNum, sizeof(zFeat));
	int actRightPointNum = describeZernike(rightImg, rightImgMark, featsR, width, height, window, orderMax);

	// match 
	int matchNum = MIN(actLeftPointNum, actRightPointNum);
	*couple = (pMatchCouple)calloc_check(matchNum, sizeof(MatchCouple));
	matchZernike(featsL, matchNum, featsR, actRightPointNum, featNum, *couple);

	// free unneeded memory
	free(featsL);
	free(featsR);

	// return matched points
	return matchNum;
}