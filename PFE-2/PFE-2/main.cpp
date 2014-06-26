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
	
	// Read image
	std::pair<cv::Mat, cv::Mat> imgMatPair = displayInit(srcFd + "view1_gray.jpg", srcFd + "view5_gray.jpg", dstFd + "match.jpg");
	//std::pair<cv::Mat, cv::Mat> imgMatPair = displayInit(srcFd + "view0.png", srcFd + "view5.png", dstFd + "match.jpg");
	//std::pair<cv::Mat, cv::Mat> imgMatPair = displayInit(srcFd + "church01.jpg", srcFd + "church02.jpg", dstFd + "match.jpg");
	//std::pair<cv::Mat, cv::Mat> imgMatPair = displayInit(srcFd + "parliament1.bmp", srcFd + "parliament2.bmp", dstFd + "match.jpg");
	//std::pair<cv::Mat, cv::Mat> imgMatPair = displayInit(srcFd + "9.bmp", srcFd + "15.bmp", dstFd + "match.jpg");

	cv::Mat leftImgMat = imgMatPair.first;
	cv::Mat rightImgMat = imgMatPair.second;
	unsigned char* leftImg = leftImgMat.data;
	unsigned char* rightImg = rightImgMat.data;
	std::pair<unsigned char*, unsigned char*> imgPair = std::make_pair(leftImg, rightImg);
	
	// image size information
	int width = leftImgMat.cols;
	int height = rightImgMat.rows;
	int fullSize = width * height;

	// mark image (mark the interest points)
	unsigned char* leftImgMark = (unsigned char*)calloc_check(fullSize, sizeof(unsigned char));
	unsigned char* rightImgMark = (unsigned char*)calloc_check(fullSize, sizeof(unsigned char));
	std::pair<unsigned char*, unsigned char*> imgMarkPair = std::make_pair(leftImgMark, rightImgMark);

	// Image Interest Points Extracting Stage
	int leftPointNum = surf(leftImg, leftImgMark, width, height, 1, 3, hessinTemplate, 2000.0);
	int rightPointNum = surf(rightImg, rightImgMark, width, height, 1, 3, hessinTemplate, 2000.0);
	std::pair<int, int> pointNumPair = std::make_pair(leftPointNum, rightPointNum);

	// Get feat and match feat
	pMatchCouple couple = NULL;
	int matchNum = 0;
	
	// Method 1: use zernike descriptors
	//matchNum = matchByZernike(imgPair, imgMarkPair, pointNumPair, &couple, 4, 15, width, height);

	// Method 2: usse hu-moment descriptors
	matchNum = matchByHuMatrix(imgPair, imgMarkPair, pointNumPair, &couple, 41, width, height);

	// Result Display Stage
	displayResult(imgMatPair, imgMarkPair, couple, matchNum, dstFd + "left.jpg", dstFd + "right.jpg", dstFd + "matchResult.jpg");

	// Free unused memory
	free(couple);
	free(leftImgMark);
	free(rightImgMark);


	return 0;
}

