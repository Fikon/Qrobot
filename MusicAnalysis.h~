#ifndef MusicAnalysis_h 
#define MusicAnalysis_h

#include <cstdio>
#include <fftw3.h>

class MusicAnalysis{
public:
	MusicAnalysis();            
	~MusicAnalysis();
	/**读取pcm数据，用fft处理*/
	void pcmAnalysis();
	/**对fft处理的结果进行处理，得到onsets*/
	void onsetsAnalysins();
	/**get,set函数*/
    void setPcmData();  //参数格式再说，等老蒋
private:
	int beats;
	int channels;
	int sampleRate;
	int frames;
	int format;
	int sectios;
	int seekable;
	fftw_complex * pcmData;
	fftw_complex * fftData;
};

#endif
