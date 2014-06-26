#include "huMatch.h"
#include <math.h>
#include <opencv2\core\core.hpp>
#include "stdfunc.h"
#include <math.h>
#include <string.h>


/*Create sevaral integral image of input image*/
void
createIntSImg
(
unsigned char* img,
double*	imgInt,
int width,
int height
)
{
	// size of the init image and integral image
	int fullSize = width * height;
	int fullSizeInt = (width + 1) * (height + 1);

	double* imgTmp = (double*)calloc_check(fullSize, sizeof(double));

	int seq = 0;
	for (int xOrder = 0; xOrder <= 3; ++xOrder)
	{
		for (int yOrder = 0; yOrder <= 3; ++yOrder)
		{
			// only calculate: x0y0, x0y1, x0y2, x0y3, x1y0, x1y1, x1y2, x2y0, x2y1, x3y0
			if (xOrder + yOrder > 3)
				continue;

			// Step 1. construct a image that are used as img, x*img, y*img, x*y*img,...
			for (int i = 0; i < fullSize; ++i)
			{
				int x = i % width;
				int y = i / width;

				imgTmp[i] = pow((double)x, xOrder) * pow((double)y, yOrder) * (double)img[i];
			}

			// Step 2. construct the corresponding integral image
			createIntImg(imgTmp, imgInt + seq * fullSizeInt, width, height);

			// Step 3. point to the next integral image
			seq++;
		}
	}

	free(imgTmp);
}



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
)
{
	// image size information
	int widthInt = width + 1;
	int heightInt = height + 1;
	int fullSizeInt = widthInt * heightInt;

	// 10 integral image: x0y0, x1y0, x0y1, x2y0, x0y2, x1y1, x3y0, x2y1, x1y2, x0y3 and calculate them
	double* imgInt = (double*)calloc_check(fullSizeInt * 10, sizeof(double));
	createIntSImg(img, imgInt, width, height);

	// the first effective pixel position of every int image
	double* imgIntX0Y0Ptr = imgInt;
	double* imgIntX0Y1Ptr = imgIntX0Y0Ptr + fullSizeInt;
	double* imgIntX0Y2Ptr = imgIntX0Y1Ptr + fullSizeInt;
	double* imgIntX0Y3Ptr = imgIntX0Y2Ptr + fullSizeInt;
	double* imgIntX1Y0Ptr = imgIntX0Y3Ptr + fullSizeInt;
	double* imgIntX1Y1Ptr = imgIntX1Y0Ptr + fullSizeInt;
	double* imgIntX1Y2Ptr = imgIntX1Y1Ptr + fullSizeInt;
	double* imgIntX2Y0Ptr = imgIntX1Y2Ptr + fullSizeInt;
	double* imgIntX2Y1Ptr = imgIntX2Y0Ptr + fullSizeInt;
	double* imgIntX3Y0Ptr = imgIntX2Y1Ptr + fullSizeInt;

	// district that can calculate feature
	int dist = window / 2;
	int xStart = dist;
	int xEnd = width - xStart;
	int yStart = dist;
	int yEnd = height - yStart;
	int actPointNum = 0;

	for (int y = yStart; y < yEnd; ++y)
	{
		for (int x = xStart; x < xEnd; ++x)
		{
			int pixNum = y * width + x;

			// only those marked as interest point will be described with hu-matrix
			if (imgMark[pixNum] == 0)
				continue;

			// four corners of the window which (x,y) is the center in the integral image
			int pLT = (y - dist) * widthInt + x - dist;
			int pRT = (y - dist) * widthInt + x + dist + 1;
			int pLB = (y + dist + 1) * widthInt + x - dist;
			int pRB = (y + dist + 1) * widthInt + x + dist + 1;

			// calculate n moments
			double m00 = imgIntX0Y0Ptr[pRB] - imgIntX0Y0Ptr[pLB] - imgIntX0Y0Ptr[pRT] + imgIntX0Y0Ptr[pLT];
			double m10 = imgIntX1Y0Ptr[pRB] - imgIntX1Y0Ptr[pLB] - imgIntX1Y0Ptr[pRT] + imgIntX1Y0Ptr[pLT];
			double m01 = imgIntX0Y1Ptr[pRB] - imgIntX0Y1Ptr[pLB] - imgIntX0Y1Ptr[pRT] + imgIntX0Y1Ptr[pLT];
			double m20 = imgIntX2Y0Ptr[pRB] - imgIntX2Y0Ptr[pLB] - imgIntX2Y0Ptr[pRT] + imgIntX2Y0Ptr[pLT];
			double m11 = imgIntX1Y1Ptr[pRB] - imgIntX1Y1Ptr[pLB] - imgIntX1Y1Ptr[pRT] + imgIntX1Y1Ptr[pLT];
			double m02 = imgIntX0Y2Ptr[pRB] - imgIntX0Y2Ptr[pLB] - imgIntX0Y2Ptr[pRT] + imgIntX0Y2Ptr[pLT];
			double m30 = imgIntX3Y0Ptr[pRB] - imgIntX3Y0Ptr[pLB] - imgIntX3Y0Ptr[pRT] + imgIntX3Y0Ptr[pLT];
			double m21 = imgIntX2Y1Ptr[pRB] - imgIntX2Y1Ptr[pLB] - imgIntX2Y1Ptr[pRT] + imgIntX2Y1Ptr[pLT];
			double m12 = imgIntX1Y2Ptr[pRB] - imgIntX1Y2Ptr[pLB] - imgIntX1Y2Ptr[pRT] + imgIntX1Y2Ptr[pLT];
			double m03 = imgIntX0Y3Ptr[pRB] - imgIntX0Y3Ptr[pLB] - imgIntX0Y3Ptr[pRT] + imgIntX0Y3Ptr[pLT];

			double xE = m10 / (m00);
			double yE = m01 / (m00);

			// calculate ¦Ì00, ¦Ì01, ¦Ì10, ...
			double u00 = m00;
			double u20 = m20 - 2 * xE * m10 + xE * xE * m00;
			double u11 = m11 - xE * m01 - yE * m10 + xE * yE * m00;
			double u02 = m02 - 2 * yE * m01 + yE * yE * m00;
			double u30 = m30 - 3 * xE * m20 + 3 * xE * xE * m10 - xE * xE * xE * m00;
			double u21 = m21 - 2 * xE * m11 + xE * xE * m01 - yE * m20 + 2 * xE * yE * m10 - xE * xE * yE * m00;
			double u12 = m12 - 2 * yE * m11 + yE * yE * m10 - xE * m02 + 2 * yE * xE * m01 - yE * yE * xE * m00;
			double u03 = m03 - 3 * yE * m02 + 3 * yE * yE * m01 - yE * yE * yE * m00;

			// calculate n11, n12, n21, ...
			double u00sqrt = sqrt(u00);
			double u00square = u00 * u00;
			double n00 = u00;
			double n20 = u20 / u00square;
			double n02 = u02 / u00square;
			double n11 = u11 / u00square;
			double n30 = u30 / (u00square * u00sqrt);
			double n21 = u21 / (u00square * u00sqrt);
			double n12 = u12 / (u00square * u00sqrt);
			double n03 = u03 / (u00square * u00sqrt);

			// calculte hu-moments: M1, M2, M3, ...
			double M1 = n20;
			double M2 = n02;
			double M3 = n11;
			double M4 = n30;
			double M5 = n21;
			double M6 = n12;
			double M7 = n03;

			//M1 = n20 + n02;
			//M2 = pow(n20 - n02, 2) + 4 * n11 * n11;
			//M3 = pow(n30 - 3 * n12, 2) + pow(3 * n21 - n03, 2);
			//M4 = pow(n30 + n12, 2) + pow(n21 + n03, 2);
			//M5 = (n30 - 3 * n12)*(n30 + n12)*(pow(n30 + n12, 2) - 3 * pow(n21 + n03, 2)) + (3 * n21 - n03)*(n21 + n03)*(3 * pow(n12 + n30, 2) - pow(n21 + n03, 2));
			//M6 = (n20 - n02)*(pow(n30 + n12, 2) - pow(n21 + n03, 2)) + 4 * n11 * (n30 + n12) * (n21 + n03);
			//M7 = (3 * n21 - n03) * (n30 + n12) * (pow(n30 + n12, 2) - 3 * pow(n21 + n03, 2)) - (n30 - 3 * n12) * (n21 + n03) * (3 * pow(n30 + n12, 2) - pow(n21 + n03, 2));

			// store the feature 
			huMoment moment = { M1, M2, M3, M4, M5, M6, M7 };
			huFeat feat = { moment, x, y };
			feats[actPointNum] = feat;

			// print it, for debug
			printf_simplify("Interest Point %3d :(%3d, %3d) : (%e, %e, %e, %e, %e, %e, %e)\n", actPointNum, x, y, M1, M2, M3, M4, M5, M6, M7);

			actPointNum++;
		}
	}

	printf_simplify("%d interest points are described\n\n", actPointNum);

	return actPointNum;
}



/*Calculate the ou - distance between two feat*/
double 
calcHuFeatDistance(huFeat* featL, huFeat* featR)
{
	double dist = 0.0;

	dist += pow(featL->moment.m1 - featR->moment.m1, 2);
	dist += pow(featL->moment.m2 - featR->moment.m2, 2);
	//dist += pow(featL->moment.m3 - featR->moment.m3, 2);
	//dist += pow(featL->moment.m4 - featR->moment.m4, 2);
	//dist += pow(featL->moment.m5 - featR->moment.m5, 2);
	//dist += pow(featL->moment.m6 - featR->moment.m6, 2);
	//dist += pow(featL->moment.m7 - featR->moment.m7, 2);

	return dist;
}


/*Add the matched pair to the couple*/
void
addHuNewPair(MatchCouple* couple, huFeat* featL, huFeat* featR)
{
	static int num = 1;

	couple->Lx = featL->x;
	couple->Ly = featL->y;
	couple->Rx = featR->x;
	couple->Ry = featR->y;

	printf_simplify("Pair %4d X,Y: (%4d, %4d), (%4d, %4d)\n", num++, couple->Lx, couple->Ly, couple->Rx, couple->Ry);
}


/*match the hu-moment feat*/
int
matchHu(std::pair<huFeat*, huFeat*>feats, std::pair<int, int> pointNum, pMatchCouple couples)
{
	huFeat* featsL = feats.first;
	huFeat* featsR = feats.second;
	int pointNumL = pointNum.first;
	int pointNumR = pointNum.second;

	// set the mininum of the two as matchNum
	int matchNum = 0;

	// set maxDist;
	double minFeatDist = 1E20;

	// set record matched j of the right image
	int matchJ = 0;
	int matchI = 0;
	
	// Step 1. use mutual mininum Euclidean distance to form first match, from left to match right
	for (int i = 0; i < pointNumL; ++i)
	{
		// find the point(matchJ) int the right image that is nearest to the i in the left
		for (int j = 0; j < pointNumR; ++j)
		{
			double featDist = calcHuFeatDistance(featsL + i, featsR + j);
			if (featDist < minFeatDist)
			{
				minFeatDist = featDist;
				matchJ = j;
			}
		}

		// reverse find -- find if i is the nearest for matchJ in the left image
		
		matchI = i;
		for (int newI = 0; newI < pointNumL; ++newI)
		{
			double featDist = calcHuFeatDistance(featsL + newI, featsR + matchJ);

			// minFeatDist is recorded from above
			if (featDist < minFeatDist)
			{
				minFeatDist = featDist;
				matchI = newI;
			}
		}

		// if (i, matchJ) == (matchI, matchJ), then the two points are coupled
		if (matchI == i)
		{
			printf_simplify("Left %3d is Matched to Right %3d: ", matchI, matchJ);
			addHuNewPair(couples++, featsL + matchI, featsR + matchJ);
			matchNum++;
		}

		// reset those compare variables for another point in left image
		minFeatDist = 1E20;
		matchJ = 0;
	}

	printf_simplify("%3d pairs are matched before ransac\n", matchNum);


	// Step 2. use ransac to modify the first match
	couples -= matchNum;
	projMat mat = ransac(couples, matchNum, 25);


	// Step 3. use the projection matrix to rectify the couples
	for (int cpIterate = 0; cpIterate < matchNum; ++cpIterate)
	{
		pMatchCouple cp = couples + cpIterate;
		cp->Rx = cp->Lx * mat.m0 + cp->Ly * mat.m1 + mat.m2;
		cp->Ry = cp->Lx * mat.m3 + cp->Ly * mat.m4 + mat.m5;
	}


	return matchNum;
}



/*ransac match method*/
projMat 
ransac(pMatchCouple couples, int matchNum, double distThresh)
{
	// variables that store the map coefficients
	projMat mat;

	// maximum needed loop time to pick 3 random compound from matchNum matches
	int loopTimes = matchNum - 2;

	// Variables to store the inner points num and total distance
	int* innerPointNum = (int*)calloc_check(loopTimes, sizeof(int));

	// Set the mark for suitable coefficients
	int maxInnerPointNum = 0;
	projMat suitMat = { 0 };

	// Generate the random alignment of the couples
	int* sequ = (int*)calloc_check(matchNum, sizeof(int));
	
	for (int i = 0; i < matchNum; ++i)
		sequ[i] = i;
	
	for (int i = matchNum - 1; i >= 1; --i)
	{
		int j = rand() % i;

		int tmp = sequ[i];
		sequ[i] = sequ[j];
		sequ[j] = tmp;
	}

	// Start loop to find the best coefficients
	for (int loop = 0; loop < loopTimes; ++loop)
	{
		// fetch 3 random couples to calculate the ms by minimum-square method
		int p0 = sequ[loop];
		int p1 = sequ[loop + 1];
		int p2 = sequ[loop + 2];

		MatchCouple cp0 = couples[p0];
		MatchCouple cp1 = couples[p1];
		MatchCouple cp2 = couples[p2];

		// use opencv mat to store coefficients and variables in the matrix function: A * X = B 
		cv::Mat matA, matB, mat012, mat345;

		// Calculate the coefficients based on the 3 couples -- opencv functions are used
		matA = (cv::Mat_<double>(3, 3) << cp0.Lx, cp0.Ly, 1, cp1.Lx, cp1.Ly, 1, cp2.Lx, cp2.Ly, 1);
		matB = (cv::Mat_<double>(3, 1) << cp0.Rx, cp1.Rx, cp2.Rx);
		mat012 = cv::Mat_<double>(3, 1);

		// if m0, m1, m2 are solved successfully
		if (!cv::solve(matA, matB, mat012, cv::DECOMP_NORMAL | cv::DECOMP_SVD))
			continue;

		matB = (cv::Mat_<double>(3, 1) << cp0.Ry, cp1.Ry, cp2.Ry);
		mat345 = cv::Mat_<double>(3, 1);

		// if m3, m4, m5 are solved successfully
		if (!cv::solve(matA, matB, mat345, cv::DECOMP_NORMAL | cv::DECOMP_SVD))
			continue;

		// if all the above solve succeed, the coeffients are solved
		mat.m0 = mat012.at<double>(0, 0);
		mat.m1 = mat012.at<double>(1, 0);
		mat.m2 = mat012.at<double>(2, 0);
		mat.m3 = mat345.at<double>(0, 0);
		mat.m4 = mat345.at<double>(1, 0);
		mat.m5 = mat345.at<double>(2, 0);

		// use the current coefficients to fit the project matrix -- calculate inner points number.
		for (int cpIterate = 0; cpIterate < matchNum; ++cpIterate)
		{
			// calculate the distance between expected right points with current matched right points
			double distance = calcMatchDistance(couples[cpIterate], mat);

			// if not bigger than dist thresh, mark it as inner points under current coefficients
			if (distance <= distThresh)
				innerPointNum[loop]++;
		}

		// mark the coefficients with the biggest inner point num and minimum toatal distance
		if (innerPointNum[loop] >= maxInnerPointNum)
		{
			maxInnerPointNum = innerPointNum[loop];
			suitMat = mat;
		}
	}

	// Free memory
	free(innerPointNum);

	return suitMat;
}



/*Calculate the distance between expected match point and current match point*/
inline double
calcMatchDistance(MatchCouple cp, projMat mat)
{
	double rXexpected = cp.Lx * mat.m0 + cp.Ly * mat.m1 + mat.m2;
	double rYexpected = cp.Lx * mat.m3 + cp.Ly * mat.m4 + mat.m5;
	
	double distance = pow(cp.Rx - rXexpected, 2) + pow(cp.Ry - rYexpected, 2);
	
	if (distance < 0)
		return 1e100;

	return distance;
}



/*get hu-moment feat and match them*/
int matchByHuMatrix
(
std::pair<unsigned char*, unsigned char*> imgPair,
std::pair<unsigned char*, unsigned char*> imgMarkPair,
std::pair<int, int> pointNum,
pMatchCouple* couples,
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
	huFeat* featsL = (huFeat*)calloc_check(leftPointNum, sizeof(huFeat));
	int actLeftPointNum = getHuMoments(leftImg, leftImgMark, featsL, window, width, height);

	huFeat* featsR = (huFeat*)calloc_check(rightPointNum, sizeof(huFeat));
	int actRightPointNum = getHuMoments(rightImg, rightImgMark, featsR, window, width, height);
	
	// make feature pair and feature point pair
	std::pair<huFeat*, huFeat*> feats = std::make_pair(featsL, featsR);
	std::pair<int, int> actPointNum = std::make_pair(actLeftPointNum, actRightPointNum);

	// match
	*couples = (pMatchCouple)calloc_check(MIN(actLeftPointNum, actRightPointNum), sizeof(MatchCouple));
	int matchNum = matchHu(feats, actPointNum, *couples);
	

	return matchNum;
}
