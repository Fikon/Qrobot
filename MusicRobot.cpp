/**
 * 主函数所在类
 **/

#include <cstdio>
#include "QRobotController.h"
#include <pthread.h>
#include "MusicAnalysis.h"
#include <time.h>

#define AUDIO_NAME "default"			//QRobot设备id
#define FRAME_NUM 512               //帧的数量
#define SAMPLE_RATE 44100           //采样率
#define CHANNELS 2                  //声道数

const int THREAD_NUM = 2;           //线程数

AudioController * audioController;  //音频输入输出对象
MusicAnalysis * musicAnalysis;      //音乐处理对象
bool audioBufferOver;               //标记录音缓存块是否已充满新数据


/**线程0为录音线程，线程1为音乐处理线程*/
void * threadRun( void *threadId ){
	long tId = (long)threadId;
	if ( tId == 0 ){
		audioController = new AudioController( AUDIO_NAME, FRAME_NUM, SAMPLE_RATE, 1);
		printf("录音开始...\n");
		while( true ){
			if ( !audioBufferOver ){   
				if ( !(*audioController).audioReadIn() ){                    //录音完成
					/*for ( int i = 0 ; i < FRAME_NUM ; i ++ ){
						printf("%d\n", ((*audioController).getBuffer())[i]);
					}*/
					audioBufferOver = true;
				}
			}
		}
		printf("线程0结束\n");
	}else{
		musicAnalysis = new MusicAnalysis(CHANNELS, SAMPLE_RATE, FRAME_NUM );
		(*musicAnalysis).onsetsInit();
		(*musicAnalysis).pcmAnalysisInit();
		while ( true ){
			if ( audioBufferOver ){
				(*musicAnalysis).setPcmData( (*audioController).getBuffer());
				audioBufferOver = false;                                       //重置标识符,录音部分继续录音
				(*musicAnalysis).pcmAnalysis();
				if ( (*musicAnalysis).onsetsAnalysis() ){                      //有重音
					printf("Y\n");
				}else{
					printf("N\n");
				}
			}
		}
	}
	return NULL;
}

int main(){

	audioBufferOver = false;

	pthread_t threads[THREAD_NUM];
	pthread_attr_t attr;
	pthread_attr_init(&attr); 
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	for ( int i = 0 ; i < THREAD_NUM ; i ++ ){
		pthread_create(&threads[i], NULL, threadRun, (void *)i);
	}
	pthread_attr_destroy(&attr); 
	for ( int i = 0 ; i < THREAD_NUM ; i ++ ){
		pthread_join(threads[i], NULL);
	}


	/*
		musicAnalysis = new MusicAnalysis(CHANNELS, SAMPLE_RATE, FRAME_NUM );
		(*musicAnalysis).onsetsInit();
		(*musicAnalysis).pcmAnalysisInit();
		short * data = new short[FRAME_NUM*2];
		srand((int)time(0));
		for ( int i = 0 ; i < FRAME_NUM*2 ; i ++ ){
			data[i] = (short)rand() % 10;
		}
		for (int i = 0 ; i < 50 ; i ++ ){
			(*musicAnalysis).setPcmData( data);
			(*musicAnalysis).pcmAnalysis();
			printf("%d : ", i);
			if ( (*musicAnalysis).onsetsAnalysis() ){                      //有重音
				printf("Y\n");
			}else{
				printf("N\n");
			}
		}
		*/

	return 0;
}
