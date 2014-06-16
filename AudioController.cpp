/*
 *\file AudioController.cpp
 *
 *对AudioController类的实现
 *
 *\author 蒋慧强
 *
 */

#include <iostream>
#include "AudioController.h"

using namespace std;

//!实现构造函数
AudioController::AudioController(char * deviceName,int _frames,int _hz,int mode)
{
   frames=_frames;
   hz=_hz;
   int ret;

   //!选择打开模式1为录音模式，2为播放模式
   if(mode==1)
   {
      ret=snd_pcm_open(&handle,deviceName,SND_PCM_STREAM_CAPTURE,0);
   }
   else
   {
      ret=snd_pcm_open(&handle,deviceName,SND_PCM_STREAM_PLAYBACK,0);
   }
   if(ret<0)
   {
      cout<<"failed to open the device!!~"<<endl;
      snd_strerror(ret);
      exit(1);
   }

   //!Allocate a hardware parameters object
   snd_pcm_hw_params_alloca(&params);
   
   //!fill the parameter with the default values
   snd_pcm_hw_params_any(handle,params);
   
   //!set the IO mode with interleaced
   snd_pcm_hw_params_set_access(handle,params,SND_PCM_ACCESS_RW_INTERLEAVED);
   
   //!set 16-bit litter-endian format
   snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_S16_LE);
   
   //!set two channels
   snd_pcm_hw_params_set_channels(handle,params,2);

   //!set the samples' rate
   snd_pcm_hw_params_set_rate_near(handle,params,&hz,&ret);
      
   //!set the period size
   snd_pcm_hw_params_set_period_size_near(handle,params,&frames,&ret);
   
   //!write the parameters to the driver
   ret=snd_pcm_hw_params(handle,params);
   if(ret<0)
   {
      cout<<"failed to set parameters!!~"<<endl;
      snd_strerror(ret);
      exit(1);
   }
   
   //!user a buffer large enough to hold one period
   snd_pcm_hw_params_get_period_size(params,&frames,&ret);
   
   buffer=(char *)malloc(frames*4);
}

//录音
int AudioController::audioReadIn()
{
   int ret;
   ret=snd_pcm_readi(handle,buffer,frames);
   if(ret==-EPIPE)
   {
      cout<<"onverrun!!~"<<endl;
      cout<<"prepare to recover!!~"<<endl;
      snd_pcm_prepare(handle);
      return 1;
   }
   else if(ret<0)
      return -1;
   else if(ret!=(int)frames)
   {
      cout<<"shortread!!~"<<endl;
      return 2;
   }
   return 0;
}

//!播放
int AudioController::audioWriteOut()
{
   int ret;
   ret=snd_pcm_writei(handle,buffer,frames);
   if(ret==-EPIPE)
   {
      cout<<"underrun!!~"<<endl;
      return 2;
   }
   else if(ret<0)
      return -1;
   else if(ret!=(int)frames)
   {
      cout<<"shortwrite!!~"<<endl;
      return 3;
   }
   return 0;
}

























































