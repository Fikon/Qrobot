/*!
 *\file AudioController.h
 *
 *\brief 定义AudioController类
 *
 *这个类主要用于控制机器人音频设备的IO
 *
 *\author 蒋慧强
 *
 */

#ifndef AUDIO_CONTROLLER_H
#define AUDIO_CONTROLLER_H

#include <alsa/asoundlib.h>
#include <string>
#include <string.h>

class AudioController
{
private:
      //!持有音频设备的句柄
      snd_pcm_t *handle;
     
      //!音频设备的参数数据结构
      snd_pcm_hw_params_t *params;
      
      //!一个 period读入的帧数量
      snd_pcm_uframes_t frames;
      
      //!输入输出音频的频率
      unsigned int hz;
      
      //!用于输入输出的数组
      short *buffer;
      
public:
      /*!
       * \brief 构造函数
       *
       * 对音频设备的初始化
       *
       * 并且传入一些配置的参数
       *
       * \param deviceName 音频设备的标识ID
       * \param frames 输入输出音频的帧大小
       * \param hz 输入输出音频的频率
       * \param mode 打开设备的模式1是录音，2是播放
       */
      AudioController(char * deviceName,int frames,int hz,int mode);
       
      //!复制构造函数
      AudioController(const AudioController&)
      {
      }

      //!析构函数
      /*!
       * 释放音频设备的持有句柄以及IO数组
       *
       */
      virtual ~AudioController()
      {
          snd_pcm_drain(handle);
          snd_pcm_close(handle);
          free(buffer);
      }

      //!录音
      /*!
       * \brief 从设备中读取音频数据，将数据储存在类成员buffer中
       *
       * 调用该函数的方法应该是在一个循环当中，每次调用之后应该立即
       * 使用buffer中读取的音频数据，防止被下一次输入所覆盖
       *
       * \retval -1 error
       * \retval 0 success
       * \retval 1 overrun 表示处理buffer缓存区不够快而被新的数据覆盖了
       * \retval 2 shortread 表示buffer缓存区未读满
       */
      int audioReadIn();
    
      //!播放
      /*!
       * 控制设备播放特定的音频数据
       *
       * \retval -1 error
       * \retval 0 success
       * \retval 1 end of file 在传入的数据是一个文件且已经到了文件末
       * \retval 2 underrun 应用程序写到buffer的速度太慢，导致饿死
       * \retval 3 shortwrite 剩余传输的数据不够一个buffer
       *
       */
      int audioWriteOut();
 
      //!buffer 的getter函数
      short * getBuffer()
      {
          return buffer;
      }

      //!convert the pcm-file to the wav-file
      /*!
       * \param fileName the name of the file for storing wav file
       * \param time the time of reading in audios!!~
       *
       */ 
      void pcmToWav(char * fileName,int time);
      
      //!hz 的getter 函数
      unsigned int getRate()
      {
          return hz;
      }
     
      //!frame 的getter 函数
      int getFrames()
      {
          return frames;
      }
};

#endif
