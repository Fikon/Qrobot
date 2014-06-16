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
	~MusicAnalysis();
	/**用fft处理pcm数据*/
	void pcmAnalysis();
	/**对fft处理的结果进行处理，得到onsets*/
	void onsetsAnalysis();
	/**get,set函数*/
    void setPcmData(unsigned char * data, int samples, int sampleRate, int channels);
private:
	int channels;
	int sampleRate;
	int samples;   
	float * pcmData;
	float * fftData;
};

#endif
