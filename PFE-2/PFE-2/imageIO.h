#pragma  once

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <string>
#include <utility>
#include "stdfunc.h"


/*Draw cross at the interest points at matched images*/
void
drawCross
(
uchar* img,
uchar* imgMark,
uchar len,
short width,
short height
);


/*Display the initial images*/
std::pair<cv::Mat, cv::Mat>
displayInit
(
std::pair<std::string, std::string> srcFilePair,
std::string mergeImgStr
);


/*Display the result images*/
void
displayResult
(
std::pair<cv::Mat, cv::Mat> imgPair,
std::pair<unsigned char*, unsigned char*> imgMarkPair,
pMatchCouple couple,
int matchNum,
std::pair<std::string, std::string> dstFilePair,
std::string mergeResultStr
);