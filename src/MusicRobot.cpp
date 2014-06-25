/**
 * 主函数所在类
 **/

#include <cstdio>
#include "../include/QRobotController.h"
#include <pthread.h>
#include "../include/MusicAnalysis.h"
#include <time.h>
#include "../include/QRobotController.h"
#include "QRobotAction.h"

#define AUDIO_NAME "hw:2,0"			                         //QRobot设备名称
#define FRAME_NUM 64		                                 //帧的数量
#define SAMPLE_RATE 44100                                    //采样率
#define CHANNELS 2                                           //声道数
#define ONSETS_NUM 16                                        //当不是onsets的数量达到该数值时做动作

const int THREAD_NUM = 3;                                    //线程数
pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond0 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
AudioController * audioController;                           //音频输入输出对象
MusicAnalysis * musicAnalysis;                               //音乐处理对象
QRobotController * qRobotController;                         //机器人控制对象
QRobotAction * qRobotAction;                                 //机器人动作对象
bool audioBufferOver;                                        //标记录音缓存块是否已充满新数据`
int onsetsCount;                                             //标记onsets个数
clock_t tStart, tEnd;
int speedInt;                                                //音乐速度

/**做动作*/
void wingMovement(){
	(*qRobotController).leftWingUp(speedInt, 8 );
	(*qRobotController).rightWingUp(speedInt, 8 );
}

/**调节速度*/
void setSpeed( int time ){
	if ( time >= 1000000 ){
		if ( time < 1600000 ){
			speedInt = 3;
		}else if( time < 2000000 ){
			speedInt = 2;
		}else{
			speedInt = 1;
		}
	}
	(*qRobotAction).setSpeed(speedInt);
	printf("速度：%d \n", speedInt);
}

/**线程0为录音线程，线程1为音乐处理线程, 线程2为机器人信号(触摸)接受线程*/
void * threadRun( void *threadId ){
	long tId = (long)threadId;
	if ( tId == 0 ){

		pthread_mutex_lock(&mutex0);
		pthread_cond_wait(&cond0,&mutex0);
		pthread_mutex_unlock(&mutex0);

		audioController = new AudioController( AUDIO_NAME, FRAME_NUM, SAMPLE_RATE, 1);
		printf("录音开始...\n");
		int time = ONSETS_NUM;
		while( true ){

			if ( !audioBufferOver ){   
				if ( !(*audioController).audioReadIn() ){                    //录音完成
					/*for ( int i = 0 ; i < FRAME_NUM ; i ++ ){
					  printf("%d\n", ((*audioController).getBuffer())[i]);
					  }*/
					if ( time <= 0 ){
						time = ONSETS_NUM;
						wingMovement();
					}
					time --;
					audioBufferOver = true;
				}
			}
		}
		printf("线程0结束\n");
	}else if( tId == 1 ){

		pthread_mutex_lock(&mutex1);
		pthread_cond_wait(&cond1,&mutex1);
		pthread_mutex_unlock(&mutex1);

		musicAnalysis = new MusicAnalysis(CHANNELS, SAMPLE_RATE, FRAME_NUM );
		qRobotAction = new QRobotAction();
		(*musicAnalysis).onsetsInit();
		(*musicAnalysis).pcmAnalysisInit();
		onsetsCount = 0;
		int pastCount = 0;                                                     //当出现重音时，略过的采样
		tStart = clock();
		while ( true ){

			if ( audioBufferOver ){
				(*musicAnalysis).setPcmData( (*audioController).getBuffer());
				audioBufferOver = false;                                       //重置标识符,录音部分继续录音
				(*musicAnalysis).pcmAnalysis();
				if ( (*musicAnalysis).onsetsAnalysis() ){                      //有重音
					onsetsCount = 0;
					//printf("Y\n");

				}else{
					onsetsCount ++;
					//printf("N\n");
				}
				pastCount --;
				if ( onsetsCount == ONSETS_NUM && pastCount <= 0 ){
					tEnd = clock();
					printf("%d\n", tEnd-tStart);
					setSpeed((int)(tEnd-tStart));
					qRobotAction->dance();
					tStart = clock();
					onsetsCount = 0;
					//printf("Y\n");
					//pastCount = 1024;                                           //音乐与敲打时数值不同(音乐的时候注释掉)
				}
			}
		}
		printf("线程1结束\n");
	}else if( tId == 2 ){
		bool isOpen = false;                                                    //标记机器人是否是开启状态
		while ( true ){
			int touchInfo = (*qRobotController).getTouchInfo();
			if ( !isOpen && touchInfo == 0x02 ){
				printf("启动...\n");
				isOpen = true;
				pthread_cond_signal(&cond0);
				pthread_cond_signal(&cond1);
			}else if( touchInfo == 0x12 ){
				printf("关机...\n");
				(*qRobotController).reset();
				pthread_cancel(0);
				pthread_cancel(1);
				break;
			}
		}
	}
	return NULL;
}

int main(){

	audioBufferOver = false;
	qRobotController = QRobotController::getInstance();

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

	pthread_mutex_destroy(&mutex0);
	pthread_mutex_destroy(&mutex1);
	pthread_cond_destroy(&cond0);
	pthread_cond_destroy(&cond1);

	return 0;
}
