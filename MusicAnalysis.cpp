#include "MusicAnalysis.h"
#include <fftw3.h>
#include "onsetsds.h"

MusicAnalysis::MusicAnalysis(){
	pcmData = NULL;
	fftData = NULL;
	pcmIn = NULL;
}

MusicAnalysis::MusicAnalysis( int channels, int sampleRate, int samples ){
	pcmData = NULL;
	fftData = NULL;
	pcmIn = NULL;

	this->channels = channels;
	this->sampleRate = sampleRate;
	this->samples = samples;
}

MusicAnalysis::~MusicAnalysis(){
	delete []pcmData;
	delete []fftData;

	pcmData = NULL;
	fftData = NULL;

	free(ods);
	free(odsdata);
	fftw_free(pcmIn);
	fftw_free(fftData);
	fftw_destroy_plan(plan);
}

void MusicAnalysis::pcmAnalysisInit(){
	pcmIn = (float *)fftw_malloc(sizeof(float) * samples);
	fftData = (float *)fftw_malloc(sizeof(float) * samples);
	memset(fftData, 0, samples);
	//plan = fftw_plan_r2r_1d(samples, (double*)pcmIn, (double*)fftData, FFTW_R2HC, FFTW_ESTIMATE);
}

void MusicAnalysis::pcmAnalysis(){
//	float * in = NULL;
//	fftw_plan plan;
//	int _count;

//	_count = samples;
//	in = (float *)fftw_malloc(sizeof(float) * _count + 1);
//	if ( pcmIn == NULL ){
//		pcmIn = (float *)fftw_malloc(sizeof(float) * _count + 1);
//	}
//	if ( fftData == NULL ){
//		fftData = (float *)fftw_malloc(sizeof(float) * _count + 1);
//	}
	plan = fftw_plan_r2r_1d(samples, (double*)pcmIn, (double*)fftData, FFTW_R2HC, FFTW_ESTIMATE);
//	plan = fftw_plan_r2r_1d(_count, (double*)in, (double*)fftData, FFTW_R2HC, FFTW_ESTIMATE);

	memset(fftData, 0, samples);
	for(int i = 0; i < samples; i++){
		pcmIn[i] = pcmData[i];
	}
	
	fftw_execute(plan);
	fftw_destroy_plan(plan);
}

void MusicAnalysis::onsetsInit(){
	ods = (OnsetsDS *)malloc(sizeof(OnsetsDS));
	odsdata = (float*)malloc(onsetsds_memneeded(ODS_ODF_POWER, samples, 11));
	onsetsds_init( ods, odsdata, ODS_FFT_FFTW3_HC, ODS_ODF_POWER, samples, 11,sampleRate);
}

bool MusicAnalysis::onsetsAnalysis(){
	return onsetsds_process(ods, fftData);
}

void MusicAnalysis::setPcmData(short * data, int samples, int sampleRate, int channels){
	if ( pcmData == NULL ){
		pcmData = new float[samples];
	}
	if ( channels == 1 ){
		this->channels = channels;
		this->sampleRate = sampleRate;
		this->samples = samples;
		for ( int i = 0 ; i < samples ; i ++ ){
			pcmData[i] = data[i];
		}
	}else if( channels == 2 ){
		for ( int i = 0 ; i < samples ; i ++ ){
			pcmData[i] = data[i*2];
		}
	}
}

void MusicAnalysis::setPcmData(short * data){
	if ( pcmData == NULL ){
		pcmData = new float[samples];
	}
	if ( channels == 1 ){
		for ( int i = 0 ; i < samples ; i ++ ){
			pcmData[i] = data[i];
		}
	}else if( channels == 2 ){
		for ( int i = 0 ; i < samples ; i ++ ){
			pcmData[i] = data[i*2];
		}
	}
}

