#include "MusicAnalysis.h"
#include <fftw3.h>
#include "onsetsds.h"

MusicAnalysis::MusicAnalysis(){
}

MusicAnalysis::~MusicAnalysis(){
	delete []pcmData;
	delete []fftData;

	pcmData = NULL;
	fftData = NULL;
}

void MusicAnalysis::pcmAnalysis(){
	float * in;
	fftw_plan plan;
	int _count;

	_count = samples * channels;
	in = (float *)fftw_malloc(sizeof(float) * _count + 1);
	fftData = (float *)fftw_malloc(sizeof(float) * _count + 1);
	plan = fftw_plan_r2r_1d(_count, (double*)in, (double*)fftData, FFTW_R2HC, FFTW_ESTIMATE);

	for(int i = 0; i < _count; i++){
		in[i] = pcmData[i];
	}

	fftw_execute(plan);
}

void MusicAnalysis::onsetsInit(){
	ods = (OnsetsDS *)malloc(sizeof(OnsetsDS));
	odsdata = NULL;
	onsetsds_init( ods, odsdata, ODS_FFT_FFTW3_HC, ODS_ODF_POWER, samples* channels, 11,sampleRate);
}

void MusicAnalysis::onsetsAnalysis(){
	onsetsds_process(ods, fftData);
}

void MusicAnalysis::setPcmData(short * data, int samples, int sampleRate, int channels){
	pcmData = new float[samples * channels];
	this->channels = channels;
	this->sampleRate = sampleRate;
	this->samples = samples;
	for ( int i = 0 ; i < samples*channels ; i ++ ){
		pcmData[i] = data[i];
	}
}

/**测试用*/
int main(){
	return 0;
}
