/**
 * 主函数所在类
 **/

#include <cstdio>
#include "QRobotController.h"
#include "MusicAnalysis.h"

#define AUDIO_ID ""                 //QRobot设备id
#define FRAME_NUM 512               //帧的数量
#define SAMPLE_RATE 44100           //采样率
#define CHANNELS 1                  //声道数

short * audioBuffer;                //录音缓存
int audioBufferNum;                 //录音缓存块的数量

int main(){
	AudioController * audioController = new AudioController( AUDIO_ID, FRAME_NUM, SAMPLE_RATE, CHANNELS);
	MusicAnalysis * musicAnalysis = new MusicAnalysis();
	while( true ){
		if ( !audioController.audioReadIn() ){
			musicAnalysis.setPcmData( audioController.buffer, audioController.frames, audioController.hz, CHANNELS);
			musicAnalysis.pcmAnalysis();
			musicAnalysis.onsetsAnalysis();
		}
	}
}
