#include "imageIO.h"
#include "pointLocate.h"
#include "zernikeMatch.h"
#include "huMatch.h"
#include "stdfunc.h"
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/nonfree/nonfree.hpp"  
#include "opencv2/nonfree/features2d.hpp"  


int
main(int argc, char* argv[])
{
	int* hessinTemplate = getHessinTemplate();

	// Image Load and Storage Folder
	std::string srcFd = "E:/Pics/Dolls/Illum2/Exp1/";
	std::string dstFd = "E:/Pics/Results/";
	std::string srcFileL = srcFd + "view2_doll3.png";
	std::string srcFileR = srcFd + "view0.png";
	std::pair<std::string, std::string> srcFilePair = std::make_pair(srcFileL, srcFileR);
	std::string dstFile = dstFd + "match.jpg";
	std::string dstFileL = dstFd + "left.jpg";
	std::string dstFileR = dstFd + "right.jpg";
	std::pair<std::string, std::string> dstFilePair = std::make_pair(dstFileL, dstFileR);

	std::string dstMatchFile = dstFd + "matchResult.jpg";
	
	// Read image
	std::pair<cv::Mat, cv::Mat> imgMatPair = displayInit(srcFilePair, dstFile);

	// Image information: pointer, size
	// 1. Pointer
	cv::Mat leftImgMat = imgMatPair.first;
	unsigned char* leftImg = leftImgMat.data;
	cv::Mat rightImgMat = imgMatPair.second;
	unsigned char* rightImg = rightImgMat.data;
	std::pair<unsigned char*, unsigned char*> imgPair = std::make_pair(leftImg, rightImg);

	// 2. Size
	int widthL = leftImgMat.cols;
	int heightL = leftImgMat.rows;
	int fullSizeL = widthL * heightL;
	int widthR = rightImgMat.cols;
	int heightR = rightImgMat.rows;
	int fullSizeR = widthR * heightR;
	std::pair<int, int> imgWidthPair = std::make_pair(widthL, widthR);
	std::pair<int, int> imgHeightPair = std::make_pair(heightL, heightR);

	// Mark image (mark the interest points)
	unsigned char* leftImgMark = (unsigned char*)calloc_check(fullSizeL, sizeof(unsigned char));
	unsigned char* rightImgMark = (unsigned char*)calloc_check(fullSizeR, sizeof(unsigned char));
	std::pair<unsigned char*, unsigned char*> imgMarkPair = std::make_pair(leftImgMark, rightImgMark);

	// Image Interest Points Extracting Stage
	int leftPointNum = surf(leftImg, leftImgMark, widthL, heightL, 1, 3, hessinTemplate, 1000.0);
	int rightPointNum = surf(rightImg, rightImgMark, widthR, heightR, 1, 3, hessinTemplate, 1000.0);
	std::pair<int, int> pointNumPair = std::make_pair(leftPointNum, rightPointNum);

	// Describe and match the interest points
	// arrays to store the matched couples.
	pMatchCouple couples = NULL;
	// get the value for the description window size
	int window = MAX(sqrt((double)fullSizeL / (double)leftPointNum), sqrt((double)fullSizeR / (double)rightPointNum));

	// Method 1: use zernike descriptors
	//int matchNum = matchByZernike(imgPair, imgMarkPair, pointNumPair, &couple, 4, 15, width, height);

	// Method 2: usse hu-moment descriptors
	int matchNum = matchByHuMatrix(imgPair, imgMarkPair, pointNumPair, imgWidthPair, imgHeightPair, &couples, (window/2)*2 + 3);

	// Result Display Stage
	displayResult(imgMatPair, imgMarkPair, couples, matchNum, dstFilePair, dstMatchFile);

	// Free unused memory
	free(couples);
	free(leftImgMark);
	free(rightImgMark);


	return 0;
}

