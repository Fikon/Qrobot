/*!
 * \file PeopleDetection.h
 *
 * \brief 定义了一个可以进行通过表情判断用户心情的类PeopleDetection
 *  并且还定义了一个心情的枚举类型Emotion
 *
 * 首先开启一个摄像头程序进行视频流捕捉，等到用户对焦等操作完成之后
 * 发出拍照指令，程序进行照片呢采集，之后交由表情识别模块进行心情判断
 *
 * \author kiky.jiang@gmail.com
 *
 */

#ifndef PEOPLE_DETECTION_H
#define PEOPLE_DETECTION_H

#include "json/json.h"
#include "Base64Codec.h"

#include <fstream>
#include <sstream>
#include <streambuf>
#include <opencv/highgui.h>
#include <curl/curl.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

enum Emotion
{
   happy,confused,calm,sad,angry,disgust,surprised,null
};


class PeopleDetection
{
private:
     //!用来判断是否应该进行拍照的标记值
     bool capture;
    
     //!摄像头的设备号
     int device;

private:
     //!根据设置的参数，与服务器进行交互，返回需要的数据
     bool APICall(const string &api_addr_base,
                  const map<string,string> &query_config,
                  Json::Value *response);

    //!与识别远端服务器进行交互涌动的辅助函数
    static int writer(char *data,size_t size,size_t nmemb,string *writedata);

public:
     //!构造函数
     /*!
      *
      * \param device 设置摄像头的设备号
      *
      */
     PeopleDetection(int _device);
 
     //!标记值的getter函数
     bool isCapture()
     {
         return capture;
     }
     
     //!标记值的setter函数
     void setCapture(bool _capture)
     {
         capture=_capture;
     }

     //!device's getter method
     int getDevice()
     {
         return device;
     }

     //!摄像头视频流捕捉函数
     void captureVideo();

     //!心情判断函数,若是失败，则flag被赋值为false
     enum Emotion getEmotion(bool &flag);
};

#endif
