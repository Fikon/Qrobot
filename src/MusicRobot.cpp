/**
 * 主函数所在类
 **/

#include <cstdio>
#include "../include/QRobotController.h"
#include <pthread.h>
#include "../include/MusicAnalysis.h"
#include <time.h>
#include "../include/QRobotController.h"
#include "../include/QRobotAction.h"
#include "../include/MusicPlayer.h"
#include "../include/Audio2Text.h"

#define AUDIO_NAME "default"			                         //QRobot设备名称
#define FRAME_NUM 64		                                 //帧的数量
#define SAMPLE_RATE 44100                                    //采样率
#define CHANNELS 2                                           //声道数
#define ONSETS_NUM 16                                        //当不是onsets的数量达到该数值时做动作
#define LOGIN_CONFIGS "appid = 53a38a05, work_dir =   .  "   //语音识别参数1
#define VOICE2TEXT_PARAM "sub=iat,ssm=1,auf=audio/L16;rate=16000,aue=raw,ent=sms16k,rst=plain,rse=utf8,cfd=1000,ptt=0;" //语音室别参数2                                      

const int THREAD_NUM = 4;                                    //线程数
pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond0 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
AudioController * audioController;                           //音频输入输出对象
MusicAnalysis * musicAnalysis;                               //音乐处理对象
QRobotController * qRobotController;                         //机器人控制对象
QRobotAction * qRobotAction;                                 //机器人动作对象
MusicPlayer * musicPlayer;                                   //音乐播放类
Audio2Text * audio2Text;                                     //语音转换
bool audioBufferOver;                                        //标记录音缓存块是否已充满新数据`
int onsetsCount;                                             //标记onsets个数
clock_t tStart, tEnd;
int speedInt;                                                //音乐速度
bool isOutMusicPlay;                                         //标记是否是外部播放音乐

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
		while ( true ){

			pthread_mutex_lock(&mutex0);
			pthread_cond_wait(&cond0,&mutex0);
			pthread_mutex_unlock(&mutex0);

			audioController = new AudioController( AUDIO_NAME, FRAME_NUM, SAMPLE_RATE, 1);
			printf("外部音乐开始录入...\n");
			int time = ONSETS_NUM;
			while( isOutMusicPlay ){

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
				if ( isOutMusicPlay ){
					(*musicAnalysis).setPcmData( (*audioController).getBuffer());
					audioBufferOver = false;                                       //重置标识符,录音部分继续录音
				}else{
					if ( musicPlayer->getIsPcmBufferFull() ){
						(*musicAnalysis).setPcmData( (*musicPlayer).getPcmBuffer());
						(*musicPlayer).setIsPcmBufferFull(false);
					}else{
						continue;
					}
				}
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
					//printf("%d\n", tEnd-tStart);
					setSpeed((int)(tEnd-tStart));
					//setSpeed(1700000);
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
		while ( true ){

			pthread_mutex_lock(&mutex2);
			pthread_cond_wait(&cond2,&mutex2);
			pthread_mutex_unlock(&mutex2);

			if ( isOutMusicPlay ){
				continue;
			}

			
			printf("请说话...\n");
			audioController = new AudioController( AUDIO_NAME, 512, 16000, 1);
			printf("~~~~~~~~~ %d\n", audioController->getRate());
			char * tempWavFile = "tempWav.wav";
			(*audioController).pcmToWav(tempWavFile, 5);
			printf("说话结束\n");

			char outText[1024]; 
			audio2Text->getText(tempWavFile, outText);
			printf("%s\n", outText);
			char * dir = audio2Text->findAudio(".",outText);
			printf("正在播放歌曲 ： %s\n", outText);
			
			musicPlayer->pcmBufferInit(FRAME_NUM);
			audioBufferOver = true;
			pthread_cond_signal(&cond1);
			
			musicPlayer->playMusic(dir,AUDIO_NAME);
		}
		printf("线程2结束\n");
	}else if( tId == 3 ){
		while ( true ){
			int touchInfo = (*qRobotController).getTouchInfo();
			if ( touchInfo == 0x02 ){                                //听音乐舞蹈
				isOutMusicPlay = true;
				if ( musicPlayer != NULL ){
					musicPlayer->setIsPlay(false);
				}
				pthread_cond_signal(&cond0);
				pthread_cond_signal(&cond1);
			}else if( touchInfo == 0x12 ){
				printf("关机...\n");
				(*qRobotController).reset();
				pthread_cancel(0);
				pthread_cancel(1);
				pthread_cancel(2);
				break;
			}else if( touchInfo == 0x04 ){                                      //点歌
				isOutMusicPlay = false;
				if ( musicPlayer == NULL ){
					musicPlayer = new MusicPlayer();
				}
				if ( audio2Text == NULL ){
					audio2Text = new Audio2Text(LOGIN_CONFIGS, VOICE2TEXT_PARAM);
				} 
				pthread_cond_signal(&cond2);
			}
		}
		printf("线程3结束\n");
	}
	return NULL;
}

int main(){

	isOutMusicPlay = false;
	audioBufferOver = false;
	audio2Text = NULL;
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
