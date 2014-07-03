/*!
 * \file MusicRobot.cpp
 * \brief 主函数所在类
 *
 * 该cpp文件为主函数所在类，用于启动程序，及调度各线程，控制整体流程
 *
 * \author 孙正扬
 * \version 1.0
 *
 */

#include <cstdio>
#include "../include/QRobotController.h"
#include <pthread.h>
#include "../include/MusicAnalysis.h"
#include <time.h>
#include "../include/QRobotController.h"
#include "../include/QRobotAction.h"
#include "../include/MusicPlayer.h"
#include "../include/Audio2Text.h"

#define AUDIO_NAME "hw:2,0"			                         ///< QRobot设备名称
#define FRAME_NUM 32		                                 ///< 帧的数量
#define SAMPLE_RATE 44100                                    ///< 采样率
#define CHANNELS 2                                           ///< 声道数
#define LOGIN_CONFIGS "appid = 53a38a05, work_dir =   .  "   ///< 语音识别参数1
#define VOICE2TEXT_PARAM "sub=iat,ssm=1,auf=audio/L16;rate=16000,aue=raw,ent=sms16k,rst=plain,rse=utf8,cfd=1000,ptt=0;" ///< 语音室别参数2
const int THREAD_NUM = 4;                                    ///< 线程数

pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond0 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

int onsetsNum = 8;                                           ///< 当不是onsets的数量达到该数值时做动作
AudioController * audioController;                           ///< 音频输入输出对象
MusicAnalysis * musicAnalysis;                               ///< 音乐处理对象
QRobotController * qRobotController;                         ///< 机器人控制对象
QRobotAction * qRobotAction;                                 ///< 机器人动作对象
MusicPlayer * musicPlayer;                                   ///< 音乐播放类
Audio2Text * audio2Text;                                     ///< 语音转换
bool audioBufferOver;                                        ///< 标记录音缓存块是否已充满新数据`
int onsetsCount;                                             ///< 标记onsets个数
clock_t tStart, tEnd;                                        ///< 计时器，用于计算节奏快慢
int speedInt;                                                ///< 音乐速度
bool isOutMusicPlay;                                         ///< 标记是否是外部播放音乐
int qRobotState = 0;                                         ///< 当前状态，0为关闭，1为待机，2为自己播放音乐，3为外部播放音乐

//!用于控制翅膀运动的函数
void wingMovement(){
	(*qRobotController).leftWingUp(speedInt, 8 );
	(*qRobotController).rightWingUp(speedInt, 8 );
}

//!调节速度
/*!
 * \brief 用于调节动作的速度
 *
 * 根据传入的时间调节速度，速度分三档，时间小于1600000us，是3档，
 * 大于1600000us且小于2000000us是2档，大于2000000us是1档
 *
 * \param onsets间隔时间
 *
 */
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

//!线程函数
/*!
 * \brief 用于实现线程的动作
 *
 * 线程的具体动作在此函数实现。总共有4个线程，线程0负责外部音乐录入；
 * 线程1负责音乐节奏分析及调用动作；线程2负责语音录入、语音识别、歌曲
 * 检索和歌曲播放；线程3用于控制其他线程。
 * 线程0和线程3不能同时进行，线程1只能对内部（线程0）或外部（线程1）1
 * 种音乐进行处理
 *
 * \param 线程id
 *
 */
void * threadRun( void *threadId ){
	long tId = (long)threadId;
	if ( tId == 0 ){
		while ( true ){

			pthread_mutex_lock(&mutex0);
			pthread_cond_wait(&cond0,&mutex0);
			pthread_mutex_unlock(&mutex0);
			printf("OK1\n");
			printf("OK4\n");
			printf("外部音乐开始录入...\n");
			int time = onsetsNum;
			while( isOutMusicPlay ){

				if ( !audioBufferOver ){   
					if ( !(*audioController).audioReadIn() ){                    //录音完成
						/*for ( int i = 0 ; i < FRAME_NUM ; i ++ ){
						  printf("%d\n", ((*audioController).getBuffer())[i]);
						  }*/
						if ( time <= 0 ){
							time = onsetsNum;
							//wingMovement();
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
		if ( musicAnalysis != NULL ){
			delete musicAnalysis;
			musicAnalysis = NULL;
		}
		musicAnalysis = new MusicAnalysis(CHANNELS, SAMPLE_RATE, FRAME_NUM );
		if ( qRobotAction == NULL ){
			qRobotAction = new QRobotAction();
		}
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
					printf("Y\n");

				}else{
					onsetsCount ++;
					printf("N\n");
				}
				pastCount --;
				if ( onsetsCount == onsetsNum && pastCount <= 0 ){

					tEnd = clock();
					printf("%d\n", tEnd-tStart);
					setSpeed((int)(tEnd-tStart));
					//setSpeed(1700000);
					qRobotAction->dance();
					wingMovement();
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
			printf("*************************************************************************\n");
			pthread_mutex_lock(&mutex2);
			pthread_cond_wait(&cond2,&mutex2);
			pthread_mutex_unlock(&mutex2);

			if ( isOutMusicPlay ){
				continue;
			}
			
			
			printf("请说话...\n");
			char * tempWavFile = "tempWav.wav";
			(*audioController).pcmToWav(tempWavFile, 4);
			printf("说话结束\n");

			char * outText = new char[1024]; 
			//audio2Text->getText(tempWavFile, outText);                     //讯飞语音
			audio2Text->getTextFromBD(tempWavFile,"wav",16000,outText);      //百度语音
			printf("%s\n", outText);
			//string dir = new string(audio2Text->findAudio("../../Music",outText));
			printf("正在播放歌曲 ： %s\n", outText);
			//printf("路径1： %s\n", audio2Text->findAudio("../../Music", outText));
			//printf("路径： %s \n", dir);
			//cout<<dir<<endl;
			musicPlayer->pcmBufferInit(FRAME_NUM);
			audioBufferOver = true;
			pthread_cond_signal(&cond1);

			musicPlayer->playMusic( audio2Text->findAudio("../../Music", outText), "default" );

			delete outText;
			outText = NULL;
		}
		printf("线程2结束\n");
	}else if( tId == 3 ){
		qRobotState = 1;
		while ( true ){
			int touchInfo = (*qRobotController).getTouchInfo();
			if ( ( qRobotState == 1 || qRobotState == 2 ) && touchInfo == 0x02 ){                                //听音乐舞蹈
				qRobotState = 3;
				audioBufferOver = false;
				if ( musicPlayer != NULL ){
					musicPlayer->setIsPlay(false);
				}
				onsetsNum = 16;
				if ( audioController != NULL ){
					delete audioController;
					audioController = NULL;
				}
				audioController = new AudioController( AUDIO_NAME, FRAME_NUM, SAMPLE_RATE, 1);
				pthread_cond_signal(&cond0);
				isOutMusicPlay = true;
				if ( musicPlayer == NULL ){
					pthread_cond_signal(&cond1);
				}
			}else if( touchInfo == 0x12 ){
				qRobotState = 0;
				printf("关机...\n");
				(*qRobotController).reset();
				pthread_cancel(0);
				pthread_cancel(1);
				pthread_cancel(2);
				break;
			}else if( ( qRobotState == 1 || qRobotState == 3 ) && touchInfo == 0x04 ){                                      //点歌
				qRobotState = 2;
				isOutMusicPlay = false;
				if ( musicPlayer == NULL ){
					musicPlayer = new MusicPlayer();
				}
				if ( audio2Text == NULL ){
					audio2Text = new Audio2Text(LOGIN_CONFIGS, VOICE2TEXT_PARAM);
				} 
				if ( audioController != NULL ){
					delete audioController;
					audioController = NULL;
				}
				audioController = new AudioController( "default", 512, 16000, 1);
				onsetsNum = 10;
				pthread_cond_signal(&cond2);
			}
		}
		printf("线程3结束\n");
	}
	return NULL;
}

//!类空间释放
void deleteClass(){
	//delete qRobotController;
	if ( qRobotAction != NULL ){
		delete qRobotAction;
		qRobotAction = NULL;
	}
	if ( musicPlayer != NULL ){
		delete musicPlayer;
		musicPlayer = NULL;
	}
	if ( musicAnalysis != NULL ){
		delete musicAnalysis;
		musicAnalysis = NULL;
	}
	if ( audio2Text != NULL ){
		delete audio2Text;
		audio2Text = NULL;
	}
	if ( audioController != NULL ){
		delete audioController;
		audioController = NULL;
	}
}

//!主函数
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

	deleteClass();

	return 0;
}
