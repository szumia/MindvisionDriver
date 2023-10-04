#ifndef MINDVISION_H
#define MINDVISION_H

#include<CameraApi.h>
#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;
class Mindvsion{
public:
    ~Mindvsion();
    void init_camera(int trigger_mode=0);
    void get_img();
    void changeCameraParam(int mode, int my_color);
    void change_resolution(int now_mode, int pre_mode, int my_color,int trigger_mode=0);
    bool is_hard_trigger_success();
    void set_trigger_mode(int trigger_mode);
public:
    int                     camera_handle;
    tSdkCameraCapbility     camera_capbility;      //设备描述信息
    tSdkFrameHead           sFrameInfo;
    BYTE*			        pbyBuffer = NULL;
    int                     channel=3;
    Mat                     src;
    unsigned char *         g_pRgbBuffer = NULL;     //处理后数据缓存区
    int now_mode = -1;
    int now_color = -1;


};


#endif // MINDVISION_H
