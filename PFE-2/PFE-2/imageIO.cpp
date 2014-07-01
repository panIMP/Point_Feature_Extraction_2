#include "imageIO.h"
#include <iostream>



/*Draw cross at the interest points at matched images*/
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



/*Display the initial images*/
std::pair<cv::Mat, cv::Mat>
displayInit
(
std::pair<std::string, std::string> srcFilePair,
std::string mergeImgStr
)
{
	// Parse pairs
	cv::Mat imgL = cv::imread(srcFilePair.first, cv::IMREAD_GRAYSCALE);
	cv::Mat imgR = cv::imread(srcFilePair.second, cv::IMREAD_GRAYSCALE);

	// Form initial merge image
	cv::Size size(imgL.cols + imgR.cols, MAX(imgL.rows, imgR.rows));
	cv::Mat mergeImg(size, CV_8UC1);
	cv::Mat leftMergeImg = mergeImg(cv::Rect(0, 0, imgL.cols, imgL.rows));
	cv::Mat rightMergeImg = mergeImg(cv::Rect(imgL.cols, 0, imgR.cols, imgR.rows));
	imgL.copyTo(leftMergeImg);
	imgR.copyTo(rightMergeImg);

	// Save initial merge image
	cv::imwrite(mergeImgStr, mergeImg);

	// Display initial merge image
	cv::namedWindow("Before processing");
	cv::imshow("Before processing", mergeImg);
	cv::waitKey(0);

	
	std::pair<cv::Mat, cv::Mat> imgPair = std::make_pair(imgL, imgR);
	return imgPair;
}



/*Display the result images, marked and matched*/
void
displayResult
(
std::pair<cv::Mat, cv::Mat> imgPair,
std::pair<unsigned char*, unsigned char*> imgMarkPair,
pMatchCouple couple,
int matchNum,
std::pair<std::string, std::string> dstFilePair,
std::string mergeResultStr
)
{
	// Parse pairs
	cv::Mat imgL, imgR;
	imgPair.first.copyTo(imgL);
	imgPair.second.copyTo(imgR);
	std::string dstFileL = dstFilePair.first;
	std::string dstFileR = dstFilePair.second;

	// Draw cross to mark the matched interest points
	drawCross(imgL.data, imgMarkPair.first, 5, imgL.cols, imgL.rows);
	drawCross(imgR.data, imgMarkPair.second, 5, imgR.cols, imgR.rows);

	// Form merge image
	cv::Size size(imgL.cols + imgR.cols, MAX(imgL.rows, imgR.rows));
	cv::Mat merge(size, CV_8UC3);
	cv::Mat mergeL = merge(cv::Rect(0, 0, imgL.cols, imgL.rows));
	cv::Mat mergeR = merge(cv::Rect(imgL.cols, 0, imgR.cols, imgR.rows));
	cv::cvtColor(imgL, mergeL, CV_GRAY2BGR);
	cv::cvtColor(imgR, mergeR, CV_GRAY2BGR);

	// Draw lines to indicate match points
	if (couple != NULL)
	{
		for (int i = 0; i < matchNum; i += 3) 
			cv::line(merge, cv::Point(couple[i].Lx, couple[i].Ly), cv::Point(couple[i].Rx + mergeL.cols, couple[i].Ry), cv::Scalar(255, 0, 0));
		
		for (int i = 1; i < matchNum; i += 3) 
			cv::line(merge, cv::Point(couple[i].Lx, couple[i].Ly), cv::Point(couple[i].Rx + mergeL.cols, couple[i].Ry), cv::Scalar(0, 255, 0));
		
		for (int i = 2; i < matchNum; i += 3) 
			cv::line(merge, cv::Point(couple[i].Lx, couple[i].Ly), cv::Point(couple[i].Rx + mergeL.cols, couple[i].Ry), cv::Scalar(0, 0, 255));
		
	}

	// Save results
	cv::imwrite(dstFileL, imgL);
	cv::imwrite(dstFileR, imgR);
	cv::imwrite(mergeResultStr, merge);

	// Display result
	cv::namedWindow("After processing");
	cv::imshow("After processing", merge);
	cv::waitKey(0);
}