#pragma once
#include <opencv2/opencv.hpp>
//#include "../others/config.h"

using namespace cv;
/**
 * @brief 保存视频类
 */
class SaveVideo
{
public:
    cv::Size size;
    SaveVideo();
    ~SaveVideo();
    void updateImage(Mat &img);
    bool getState(){
        return state_;
    }
private:
    VideoWriter video_writer_;
    bool state_ = false;
    int frame_count=0;
    void open_video_writer(){
        struct tm *newtime;
        char tmpbuf[128];
        time_t test;
        time(&test);
        newtime=localtime(&test);
        strftime(tmpbuf, 128, "%c", newtime);
        sprintf(tmpbuf, "%s.avi", tmpbuf);
        // CV_FOURCC('I', 'Y', 'U', 'V')CV_FOURCC('M', 'J', 'P', 'G')
        //    video_writer_.open(tmpbuf, CV_FOURCC('X', 'V', 'I', 'D'), 300, cv::Size(640, 480));

        size=cv::Size(1280,1024);
        std::cout<<"tmpbuf:"<<tmpbuf<<std::endl;
//        std::string user_name = config::config_map_without_type->at("USER_NAME");
//        std::string base = "/home/"+user_name+"/Videos/";

        //TODO::saved video path
        std::string base = "../";
//        std::string user_name="nuc//Desktop/savedVideos/";
//        std::string base = "/home/"+user_name;

        //TODO::改录制帧数
        video_writer_.open(base+tmpbuf, VideoWriter::fourcc('M', 'J', 'P', 'G')  , 125, size);
        if (!video_writer_.isOpened()) {
            std::cout << "videowriter opened failure!" << std::endl;
            state_ = false;
        }else
        {
            std::cout << "videowriter opened successfully!!! " << std::endl;
            state_ = true;
        }

    }
};
