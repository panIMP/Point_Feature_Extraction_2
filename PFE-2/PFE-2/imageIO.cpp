#include "imageIO.h"
#include <iostream>


std::pair<cv::Mat, cv::Mat>
displayInit
(
std::string leftImgStr,
std::string rightImgStr,
std::string mergeImgStr
)
{
	cv::Mat leftImg = cv::imread(leftImgStr, cv::IMREAD_GRAYSCALE);
	cv::Mat rightImg = cv::imread(rightImgStr, cv::IMREAD_GRAYSCALE);

	//cv::Mat leftImg = cv::Mat(20, 20, CV_8UC1, cv::Scalar(255));
	//cv::Mat rightImg = cv::Mat(20, 20, CV_8UC1, cv::Scalar(255));

	std::pair<cv::Mat, cv::Mat> imgPair = std::make_pair(leftImg, rightImg);

	cv::Size size(leftImg.cols + rightImg.cols, MAX(leftImg.rows, rightImg.rows));
	cv::Mat mergeImg(size, CV_8UC1);
	cv::Mat leftMergeImg = mergeImg(cv::Rect(0, 0, leftImg.cols, leftImg.rows));
	cv::Mat rightMergeImg = mergeImg(cv::Rect(leftImg.cols, 0, rightImg.cols, rightImg.rows));
	leftImg.copyTo(leftMergeImg);
	rightImg.copyTo(rightMergeImg);

	cv::imwrite(mergeImgStr, mergeImg);

	cv::namedWindow("Before processing");
	cv::imshow("Before processing", mergeImg);
	cv::waitKey(0);

	return imgPair;
}


void
drawCross
(
uchar* img,
uchar* imgMark,
uchar len,
short width,
short height
)
{
	int i, j, k;

	short x, y;

	int startPoint = width * len + len;
	int endPoint = width * (height - len) - len;
	int pointNum = 0;

	imgMark += startPoint;
	for (k = startPoint; k < endPoint; ++k) {
		if (*imgMark++ == 0)
			continue;

		pointNum++;

		x = k % width;
		y = k / width;

		for (i = x - len; i < x + len; ++i) {
			*(img + y * width + i) = 255;
		}

		for (j = y - len; j < y + len; ++j) {
			*(img + j * width + x) = 255;
		}
	}

	//std::cout << pointNum << " interest points has been detected" << std::endl;
}


void
displayResult
(
std::pair<cv::Mat, cv::Mat> imgPair,
std::pair<unsigned char*, unsigned char*> imgMarkPair,
pMatchCouple couple,
int matchNum,
std::string leftResultStr,
std::string rightResultStr,
std::string mergeResultStr
)
{
	cv::Mat leftImg = imgPair.first;
	cv::Mat rightImg = imgPair.second;

	drawCross(leftImg.data, imgMarkPair.first, 5, leftImg.cols, leftImg.rows);
	drawCross(rightImg.data, imgMarkPair.second, 5, rightImg.cols, rightImg.rows);

	cv::Size size(leftImg.cols + rightImg.cols, MAX(leftImg.rows, rightImg.rows));
	cv::Mat merge(size, CV_8UC3);
	cv::Mat leftMerge = merge(cv::Rect(0, 0, leftImg.cols, leftImg.rows));
	cv::Mat rightMerge = merge(cv::Rect(leftImg.cols, 0, rightImg.cols, rightImg.rows));
	cv::cvtColor(leftImg, leftMerge, CV_GRAY2BGR);
	cv::cvtColor(rightImg, rightMerge, CV_GRAY2BGR);

	if (couple != NULL)
	{
		for (int i = 0; i < matchNum; i += 3) 
			cv::line(merge, cv::Point(couple[i].Lx, couple[i].Ly), cv::Point(couple[i].Rx + leftMerge.cols, couple[i].Ry), cv::Scalar(255, 0, 0));
		
		for (int i = 1; i < matchNum; i += 3) 
			cv::line(merge, cv::Point(couple[i].Lx, couple[i].Ly), cv::Point(couple[i].Rx + leftMerge.cols, couple[i].Ry), cv::Scalar(0, 255, 0));
		
		for (int i = 2; i < matchNum; i += 3) 
			cv::line(merge, cv::Point(couple[i].Lx, couple[i].Ly), cv::Point(couple[i].Rx + leftMerge.cols, couple[i].Ry), cv::Scalar(0, 0, 255));
		
	}

	// Save results
	cv::imwrite(leftResultStr, leftImg);
	cv::imwrite(rightResultStr, rightImg);
	cv::imwrite(mergeResultStr, merge);

	cv::namedWindow("After processing");
	cv::imshow("After processing", merge);
	cv::waitKey(0);
}