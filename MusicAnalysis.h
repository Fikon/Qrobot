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
	MusicAnalysis();
	MusicAnalysis( int channels, int sampleRate, int samples );
	~MusicAnalysis();
	/**用fft处理pcm数据*/
	void pcmAnalysisInit();
	void pcmAnalysis();
	/**对fft处理的结果进行处理，得到onsets*/
	void onsetsInit();
	bool onsetsAnalysis();
	/**get,set函数*/
    void setPcmData(short * data, int samples, int sampleRate, int channels);
	void setPcmData(short * data);
private:
	int channels;
	int sampleRate;
	int samples;   
	float * pcmData;
	float * fftData;
	OnsetsDS * ods;
	float * odsdata;
	float * pcmIn;
	fftw_plan plan;
};

#endif
