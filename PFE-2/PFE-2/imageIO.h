#pragma  once

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <string>
#include <utility>
#include "stdfunc.h"



std::pair<cv::Mat, cv::Mat>
displayInit
(
std::string leftImgStr,
std::string rightImgStr,
std::string mergeImgStr
);


void
drawCross
(
uchar* img, 
uchar* imgMark, 
uchar len, 
short width, 
short height
);


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
);