/*
 *\file AudioController.cpp
 *
 *对AudioController类的实现
 *
 *\author 蒋慧强
 *
 */

#include <iostream>
#include <stdio.h>
#include "AudioController.h"
#include <fstream>

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
   
   buffer=(short *)malloc(frames*4);
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

//!conver the pcm to the wav!!~
void AudioController::pcmToWav(char * fileName,int time)
{
   
   int rate,bytePerSec,fileLen,size=0,times;
   const char header[]={
   // RIFF WAVE Chunk 
   0x52, 0x49, 0x46, 0x46, // "RIFF" 
   0x30, 0x40, 0x06, 0x00, // 总长度 整个wav文件大小减去ID和Size所占用的字节数 
   0x57, 0x41, 0x56, 0x45, // "WAVE" 

   //Format Chunk 
   0x66, 0x6D, 0x74, 0x20, // "fmt " 
   0x10, 0x00, 0x00, 0x00, // 块长度 
   0x01, 0x00, // 编码方式 wFormatTag 
   0x01, 0x00, // 声道数目 wChannels 
   0x44, 0xAC, 0x00, 0x00, // 采样频率 dwSamplesPerSec 
   0x88, 0x58, 0x01, 0x00, // 每秒所需字节数 dwAvgBytesPerSec 
   0x02, 0x00, // 每个样本需要的字节数 wBlockAlign 
   0x10, 0x00, // 每个样本需要的位数 wBitsPerSample 
   // Fact Chunk 
   0x66, 0x61, 0x63, 0x74, // "fact" 
   0x04, 0x00, 0x00, 0x00, // 块长度 
   0x00, 0xBE, 0x00, 0x00, 
   // Data Chunk 
   0x64, 0x61, 0x74, 0x61, // "data" 
   0x00, 0x40, 0x06, 0x00 // 块长度 
   //0x00, 0x00, 0x00, 0x00
   };
   //!计算出循环的次数
   times=getRate()/512;
   //!数据流的大小
   size=times*1024;
　 //!文件的总大小
   fileLen=size+48;
   //!采样率
   rate=getRate();
   //!每秒传输的速率
   bytePerSec=rate*2;
   ofstream out(fileName,ios::binary);
   //!给pcm数据流加上头，以转化为wav格式
   out.write(header,56);
   out.seekp(4);
   out.write((char *)&fileLen,4);
   out.seekp(24);
   out.write((char *)&rate,4);
   out.seekp(28);
   out.write((char *)&bytePerSec,4);
   out.seekp(52);
   out.write((char *)&size,4);
   out.seekp(0,ios::end);
   cout<<out.tellp()<<endl;
   short buf[1024];
   for(int i=0;i<times;i++)
   {
      audioReadIn();
      for(int i=0;i<1024;i++)
          buf[i]=getBuffer()[2*i];
      out.write((char *) buf,1024);
   }
   out.close();
}





















































