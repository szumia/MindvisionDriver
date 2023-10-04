#include"mindvision.h"
#include<unistd.h>
#include<iostream>
#include<opencv2/opencv.hpp>

using namespace cv;


void Mindvsion::init_camera(int trigger_mode)
{
    // SDK 初始化
    CameraSdkInit(1);
    tSdkCameraDevInfo camera_enum_lister;
    int camera_counts = 1;
    int camera_status = -1;

    // 枚举设备，并建立设备列表
    camera_status = CameraEnumerateDevice(&camera_enum_lister, &camera_counts);

//    std::cout << "Camera enumerate state: " << camera_status << std::endl;
    std::cout << "Camera counts: " << camera_counts << std::endl;

    if( camera_counts == 0)
    {
        std::cout << "No camera found!!!" << std::endl;
        return;
    }

    // 相机初始化
    camera_status = CameraInit(&camera_enum_lister, -1, -1, &camera_handle);
    std::cout << "Camera init state: " << camera_status<< std::endl;
    if( camera_status != CAMERA_STATUS_SUCCESS)
    {
        std::cout << "Camera init failed!!!" << std::endl;
        return;
    }

    // 获取相机的参数范围
    CameraGetCapability(camera_handle, &camera_capbility);

    // 当前帧内存
    g_pRgbBuffer = (unsigned char*)malloc(
                camera_capbility.sResolutionRange.iHeightMax*camera_capbility.sResolutionRange.iWidthMax*3);


    CameraSetTriggerMode(camera_handle,trigger_mode);
    if(trigger_mode==2){
    // 设置硬触发的触发方式
        CameraSetExtTrigSignalType(camera_handle,0) ;
//    CameraSetTriggerCount(camera_handle, 1);
    //    EXT_TRIG_LEADING_EDGE = 0,     //上升沿触发，默认为该方式
    //    EXT_TRIG_TRAILING_EDGE,        //下降沿触发
    //    EXT_TRIG_HIGH_LEVEL,           //高电平触发
    //    EXT_TRIG_LOW_LEVEL             //低电平触发

    }

    //////////////////////相机参数//////////////////////
    //读取曝光时间
    double exposure=0;
    double Exposure = 2800.0; //TODO::1500.0！！设置曝光
//    CameraSetAeState(camera_handle, TRUE);
    CameraSetAeState(camera_handle, FALSE);
    camera_status = CameraSetExposureTime(camera_handle, Exposure);
    camera_status =CameraGetExposureTime(camera_handle, &exposure);
    cout<<"曝光时间为:  "<<exposure<<endl;
//    读gama
    int gama=0;            //！！设置gama

    camera_status =CameraSetGamma(camera_handle,100);
    camera_status =CameraGetGamma(camera_handle,&gama);
    cout<<"伽马值为：   "<<gama<<endl;

//    读对比度
//    int contrast=0;
//     camera_status =CameraSetGamma(camera_handle,2);
//    camera_status =CameraGetGamma(camera_handle,&contrast);
//    cout<<"对比度为：   "<<contrast<<endl;

//    读增益
     int r=100,g=100,b=100;
     camera_status =CameraSetGain(camera_handle,r,g,b);
//    camera_status =CameraGetGain(camera_handle,&r,&g,&b);
//    cout<<"增益值为: "<< "r: "<< r << "g: "<< g << "b: "<< b << endl;

    //////////////////// 设置分辨率 //////////////////////

    tSdkImageResolution pImageResolution = *camera_capbility.pImageSizeDesc;
    pImageResolution.iIndex = 0xff; //表示自定义分辨率（roi）
    pImageResolution.iHeight = 1024;
    pImageResolution.iHeightFOV = 1024;
    pImageResolution.iVOffsetFOV = 304;
    pImageResolution.iHOffsetFOV = 0;
    pImageResolution.iWidth = 1280;
    pImageResolution.iWidthFOV = 1280;
    camera_status = CameraSetImageResolution(camera_handle, &pImageResolution);


    // 设置图像处理的输出格式，彩色黑白都支持RGB24位
    if(camera_capbility.sIspCapacity.bMonoSensor){
        channel=1;
        CameraSetIspOutFormat(camera_handle,CAMERA_MEDIA_TYPE_MONO8);
    }else{
        channel=3;
        CameraSetIspOutFormat(camera_handle,CAMERA_MEDIA_TYPE_BGR8);
    }

    CameraPlay(camera_handle);
    struct tm *newtime;
    char tmpbuf[128];
    time_t test;
    time(&test);
    newtime=localtime(&test);
    strftime(tmpbuf, 128, "%c", newtime);
    cout<<tmpbuf<<"起始时间"<<endl;
}

extern bool camera_error;
void Mindvsion::get_img()
{
There:
    int status = -1;
    status = CameraGetImageBufferPriority(camera_handle, &sFrameInfo, &pbyBuffer, 1000, 2);
    if( status == CAMERA_STATUS_SUCCESS){
        status = CameraImageProcess(camera_handle, pbyBuffer, g_pRgbBuffer,&sFrameInfo);
        if(status != CAMERA_STATUS_SUCCESS)
        {
            cout<<"API: CameraImageProcess failed: "<< status << endl;
        }
        src = Mat(
            cv::Size(sFrameInfo.iWidth,sFrameInfo.iHeight),
            sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
            g_pRgbBuffer);
        status = CameraReleaseImageBuffer(camera_handle, pbyBuffer);
        if(status != CAMERA_STATUS_SUCCESS)
        {
            cout<<"API: CameraReleaseImageBuffer failed: "<< status <<endl;
        }
        sFrameInfo.uiTimeStamp;

    }
    else
    {
        cout<<"API: CameraGetImageBuffer failed: " << status << endl;
        struct tm *newtime;
        char tmpbuf[128];
        time_t test;
        time(&test);
        newtime=localtime(&test);
        strftime(tmpbuf, 128, "%c", newtime);
        cout<<tmpbuf<<"相机异常状态"<<endl;
        //防相机掉线
        CameraUnInit(camera_handle);
        free(g_pRgbBuffer);
        //酌情看是否添加延时
        cv::waitKey(200);
        if(status == -12){
            camera_error = true;
            int trigger_mode = 0;
            init_camera(trigger_mode);
            if(!is_hard_trigger_success()){
                trigger_mode = 0;
                set_trigger_mode(trigger_mode);
            }
            int tmp_mode;
            CameraGetTriggerMode(camera_handle, &tmp_mode);
            goto There;
        }else{
            close(-1);
            exit(-1);
        }
    }
}

Mindvsion::~Mindvsion()
{
    std::cout << "camera close !!!" <<std::endl;
    CameraUnInit(camera_handle);
    free(g_pRgbBuffer);
}


void Mindvsion::changeCameraParam(int mode, int my_color)
{
    if(now_mode == mode && now_color == my_color) return;
    // int success = 1;
    now_mode = mode;
    now_color = my_color;
    //zimiao shaobing
    if(now_mode == 5|| now_mode == 1 || now_mode == 0){
        std::cout << "switch to zimiao/shaobing!!!!" << std::endl;
        if(my_color == 1)
        {
            //TODO::exposure & gain red-zimiao
            double Expourse = 2700;
            CameraSetExposureTime(camera_handle,Expourse);
            std::cout << "detect red!!!    expourse " <<Expourse<< std::endl;
        }
        else if(my_color == 0)
        {

            //TODO::exposure & gain blue-zimiao
            double Expourse = 3000;
            CameraSetExposureTime(camera_handle,Expourse);
            std::cout << "detect blue!!!    expourse " <<Expourse<< std::endl;
//            int r=100,g=100,b=100;
//            CameraSetGain(camera_handle,r,g,b);
        }
    }
    else if(now_mode == 3){
        std::cout << "switch to qianshao!!!!" << std::endl;
        if(my_color == 1)
        {
            //TODO::exposure & gain red-qianshao
            double Expourse = 2700;
            CameraSetExposureTime(camera_handle,Expourse);
            std::cout << "detect red!!!    expourse " <<Expourse<< std::endl;
        }
        else if(my_color == 0)
        {
            //TODO::exposure & gain blue-qianshao
            double Expourse = 3000;
            CameraSetExposureTime(camera_handle,Expourse);
            std::cout << "detect blue!!!    expourse " <<Expourse<< std::endl;
        }
        // set_resolution(-1,1280,0,-1,1024,0);
    }
    else if(now_mode == 2)            //jidi
    {
        std::cout << "switch to jidi!!!!" << std::endl;
        //TODO::exposure & gain jidi
        if(my_color == 1)
        {
            //TODO::exposure & gain red-zimiao
            double Expourse = 2700;
            CameraSetExposureTime(camera_handle,Expourse);
            std::cout << "detect red!!!    expourse " <<Expourse<< std::endl;
        }
        else if(my_color == 0)
        {
            //TODO::exposure & gain blue-zimiao
            double Expourse = 3000;
            CameraSetExposureTime(camera_handle,Expourse);
            std::cout << "detect blue!!!    expourse " <<Expourse<< std::endl;
        }

    } else{;}

}


void Mindvsion::change_resolution(int now_mode, int pre_mode, int my_color,int trigger_mode)
{
    //打符模式
//    if((now_mode == 2 || now_mode == 3 || now_mode==5) && now_mode != pre_mode)
//    {
//        CameraSetTriggerMode(camera_handle,trigger_mode);
//        if(trigger_mode==2){
//            // 设置硬触发的触发方式
//            CameraSetExtTrigSignalType(camera_handle,0) ;
//        }
//        if(my_color==0){
//            CameraSetExposureTime(camera_handle,1500); //3000
//        }else{
//            CameraSetExposureTime(camera_handle,1500); //3000
//        }
//        CameraSetGain(camera_handle,100,100,100);
//        tSdkImageResolution pImageResolution = *camera_capbility.pImageSizeDesc;
//        pImageResolution.iIndex = 0xff; //表示自定义分辨率（roi）
//        pImageResolution.iHeight = 1024;
//        pImageResolution.iHeightFOV = 1024;
//        pImageResolution.iVOffsetFOV = 0;
//        pImageResolution.iHOffsetFOV = 160;
//        pImageResolution.iWidth = 960;
//        pImageResolution.iWidthFOV = 960;
//        CameraSetImageResolution(camera_handle, &pImageResolution);
//    }
//    else


        //0-reset  1-兑矿 2-取矿 3-non
        if( (now_mode == 0 || now_mode == 1||now_mode==2||now_mode==3 )&& now_mode != pre_mode)
    {
        // 设置硬触发
        CameraSetTriggerMode(camera_handle,trigger_mode);
        if(trigger_mode==2){
            // 设置硬触发的触发方式
            CameraSetExtTrigSignalType(camera_handle,0) ;
        }
        std::cout << "switch to autoaim!!!!" << std::endl;
        if(my_color == 1)  //me-blue enemy-red
        {
            std::cout << "detect red!!!" << std::endl;
            CameraSetExposureTime(camera_handle, 3000);
            CameraSetGain(camera_handle,100,100,100);
//            CameraSetSaturation(camera_handle, 100);
        }
        else if(my_color == 0)   //me-red enemy-blue
        {
            std::cout << "detect blue!!!" << std::endl;
            CameraSetExposureTime(camera_handle, 3000);
            CameraSetGain(camera_handle,100,100,100);
        }

        tSdkImageResolution pImageResolution = *camera_capbility.pImageSizeDesc;
        pImageResolution.iIndex = 0xff; //表示自定义分辨率（roi）
        pImageResolution.iHeight = 1024;
        pImageResolution.iHeightFOV = 1024;
        pImageResolution.iVOffsetFOV = 304;
        pImageResolution.iHOffsetFOV = 0;
        pImageResolution.iWidth = 1280;
        pImageResolution.iWidthFOV = 1280;
        CameraSetImageResolution(camera_handle, &pImageResolution);

    }
}

bool Mindvsion::is_hard_trigger_success(){
    int status = -1;
    status = CameraGetImageBufferPriority(camera_handle, &sFrameInfo, &pbyBuffer, 1000, 2);
    status = CameraReleaseImageBuffer(camera_handle, pbyBuffer);
    if(status != CAMERA_STATUS_SUCCESS)
    {
        cout<<"API: CameraReleaseImageBuffer failed: "<< status <<endl;
    }
    return status==CAMERA_STATUS_SUCCESS;
}

void Mindvsion::set_trigger_mode(int trigger_mode){
    CameraSetTriggerMode(camera_handle,trigger_mode);
    if(trigger_mode==2){
        // 设置硬触发的触发方式
        CameraSetExtTrigSignalType(camera_handle,0) ;
    }

}
