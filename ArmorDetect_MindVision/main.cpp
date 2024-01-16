#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <signal.h>
#include <chrono>
#include <ratio>
#include <ctime>
#include "armor_detect.h"


using namespace std::chrono;
using namespace std;

#include "sensors/mindvision.h"
#include "sensors/save_video.h"

#define camera
//#define capture_on



volatile bool camera_error = false;
volatile bool camera_error_lock = false;
volatile bool isrun = 1;
int current_frame=0;

Mat img;
mutex m;
volatile int process_index = 0;
volatile int receive_index = 0;
volatile int capture_index = 0;

duration<double,std::ratio<1,1000>> trigger_dur_g;

void sign_handler(int sign)
{
    isrun = 0;
}

void process()
{
    ArmorDetector ad;
    high_resolution_clock::time_point pre_time = high_resolution_clock::now();

    while(isrun)
    {
        while((receive_index - process_index) < 1 && isrun);
        
        //m.lock();
        if(camera_error_lock){
            camera_error_lock = false;
        }
        //Mat src = img.clone();
        //m.unlock();
        process_index++;


        //TODO::在这里调用你的识别代码
        //ad.receive_img(src);
        ad.receive_img(img);
        ad.detect_steps();




        high_resolution_clock::time_point now = high_resolution_clock::now();
        duration<double,std::ratio<1,1000>> trigger_dur_tmp(now- pre_time);
        trigger_dur_g=trigger_dur_tmp;
        pre_time = now;
        pre_time = now;



    }
}

//取图线程
void receive()
{
#ifdef camera
    Mindvsion md;
    int trigger_mode=0;
    //TODO::在init_camera里改相机参数
    md.init_camera(trigger_mode);
    if(!md.is_hard_trigger_success()){
        trigger_mode=0;
        md.set_trigger_mode(trigger_mode);
    }
    int tmp_mode;
    CameraGetTriggerMode(md.camera_handle,&tmp_mode);
    std::cout<<"trigger mode:"<<tmp_mode<<endl;
#endif


    high_resolution_clock::time_point pre_time = high_resolution_clock::now();
    high_resolution_clock::time_point now_time = high_resolution_clock::now();
    duration<double,std::ratio<1,1000>> trigger_duration(now_time-pre_time);
    high_resolution_clock::time_point pre = high_resolution_clock::now();

    while(isrun)
    {
#ifdef camera
        md.get_img();
#endif

        // while((receive_index - process_index) > 0 && isrun);
        if((receive_index - process_index) > 0 && isrun)
            continue;
        m.lock();
        if(camera_error){
            camera_error_lock = true;
            camera_error = false;
        }

#ifdef camera
        img = md.src;
#endif
        m.unlock();
        receive_index++;
        now_time = high_resolution_clock::now();
        duration<double,std::ratio<1,1000>> duration_tmp(now_time-pre_time);
        trigger_duration = duration_tmp;
#ifdef camera

        //TODO::change get-img fps  5-200 10-100
        while(trigger_duration.count() <= 8)
        {
            now_time = high_resolution_clock::now();
            duration<double,std::ratio<1,1000>> duration_tmp(now_time-pre_time);
            trigger_duration = duration_tmp;
        }

#endif
        now_time = high_resolution_clock::now();
        pre_time = now_time;

        high_resolution_clock::time_point now = high_resolution_clock::now();
        duration<double,std::ratio<1,1000>> trigger_dur_tmp(now- pre);
        pre = now;
    }
}

//录制视频线程
void capture()
{
    SaveVideo sv;
    while(isrun)
    {
        while((receive_index - capture_index) < 1 && isrun);
        m.lock();
        Mat src = img.clone();
        m.unlock();
        capture_index++;
        if(sv.getState())
        {
            resize(src,src,sv.size);
            sv.updateImage(src);
        }
    }
}


int main()
{
    signal(SIGINT, sign_handler);
    std::thread receive_thread(receive);
    std::thread process_thread(process);
#ifdef capture_on
    std::thread capture_thread(capture);
#endif
    if(receive_thread.joinable())
        receive_thread.join();
    if(process_thread.joinable())
        process_thread.join();
#ifdef capture_on
    if(capture_thread.joinable())
        capture_thread.join();
#endif


    return 0;
}
