/*!
 * \file MusicAnalysis.h
 *
 * \brief MusicAnalysis类的头文件
 *
 * 定义MusicAnalysis类
 *
 * \author 孙正扬
 *
 */

#ifndef MusicAnalysis_h 
#define MusicAnalysis_h

#include <cstdio>
#include <fftw3.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "onsetsds.h"

class MusicAnalysis{
public:

	//!构造函数1
	MusicAnalysis();

	//!构造函数2
	/*!
	 * \brief 构造函数，可用于初始化一些变量
	 *
	 * 可初始化channels,sampleRate,samples
	 *
	 * \param channels 声道数
	 * \param sampleRate 采样率
	 * \param samples 采样数
	 *
	 */
	MusicAnalysis( int channels, int sampleRate, int samples );

	//!析构函数
	~MusicAnalysis();

    //!pcm分析初始化函数
	void pcmAnalysisInit();

	//!pcm数据分析函数，必须在调用pcmAnalysisInit()之后调用
	void pcmAnalysis();

	//!onsets检测初始化函数
	void onsetsInit();

	//!onsets检测函数，必须在调用onsetsInit()之后调用
	bool onsetsAnalysis();
	
	//!设置pcm数据，同时可以设置一些参数
    void setPcmData(short * data, int samples, int sampleRate, int channels);
	
	//!设置pcm数据
	void setPcmData(short * data);

private:

	int channels;                   ///< 声道数
	int sampleRate;                 ///< 采样率
	int samples;                    ///< 采样数
	float * pcmData;                ///< pcm数据指针
	float * fftData;                ///< fft数据指针
	OnsetsDS * ods;                 ///< OnsetDs对象指针
	float * odsdata;                ///< ods数据指针
	float * pcmIn;                  ///< 中间变量，暂存pcm数据
	//fftw_plan plan;
};
#endif
