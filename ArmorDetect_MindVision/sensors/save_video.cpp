#include "save_video.h"


SaveVideo::SaveVideo()
{
    open_video_writer();
}

SaveVideo::~SaveVideo()
{
    video_writer_.release();
    std::cout << "video close!!!" << std::endl;
}

void SaveVideo::updateImage(Mat &img)
{
    video_writer_ << img;
    frame_count++;
    if(frame_count%(125*40)==0){
        video_writer_.release();
        open_video_writer();
    }
}

