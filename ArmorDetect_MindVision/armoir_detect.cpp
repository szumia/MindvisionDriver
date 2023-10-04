#include "armor_detect.h"



void ArmorDetector::detect_steps()
{
    cv::namedWindow("original",cv::WINDOW_NORMAL);
    cv::imshow("original",this->src);
    cv::waitKey(1);
}
