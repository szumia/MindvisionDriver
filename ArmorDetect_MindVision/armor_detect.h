#ifndef DRONE_ARMOR_DETECT_H
#define DRONE_ARMOR_DETECT_H

#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>

//TODO::实现你的自瞄识别

//demo::
class ArmorDetector
{
public:
    cv::Mat src;
    ArmorDetector(){};

    void receive_img(cv::Mat input)
    {
        this->src = input.clone();
    }
    void detect_steps();
    ~ArmorDetector(){};
};











#endif //DRONE_ARMOR_DETECT_H
