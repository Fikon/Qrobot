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
#include "../include/PeopleDetection.h"
#include "../include/Text2Audio.h"
#include "../include/AudioController.h"
#include "../include/WavPlayer.h"

#define AUDIO_NAME "hw:0,0"			                         ///< QRobot设备名称
#define FRAME_NUM 32		                                 ///< 帧的数量
#define SAMPLE_RATE 44100                                    ///< 采样率
#define CHANNELS 2                                           ///< 声道数
#define LOGIN_CONFIGS "appid = 53a38a05, work_dir =   .  "   ///< 语音识别参数1
#define VOICE2TEXT_PARAM "sub=iat,ssm=1,auf=audio/L16;rate=16000,aue=raw,ent=sms16k,rst=plain,rse=utf8,cfd=1000,ptt=0;" ///< 语音室别参数2
const int THREAD_NUM = 5;                                    ///< 线程数

pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond0 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

int onsetsNum = 8;                                           ///< 当不是onsets的数量达到该数值时做动作
AudioController * audioController;                           ///< 音频输入输出对象
MusicAnalysis * musicAnalysis;                               ///< 音乐处理对象
QRobotController * qRobotController;                         ///< 机器人控制对象
QRobotAction * qRobotAction;                                 ///< 机器人动作对象
MusicPlayer * musicPlayer;                                   ///< 音乐播放对象
Audio2Text * audio2Text;                                     ///< 语音转换对象
PeopleDetection * peopleDetection;                           ///< 表情识别对象
WavPlayer * wavPlayer;                                       ///< wav格式文件播放函数
Text2Audio * text2Audio;                                     ///< 语音合成函数
Emotion emotion;                                             ///< 表情
bool ifEmotionDetected;                                      ///< 标记拍照是否成功
bool ifDetctOver;                                            ///< 标记表情识别是否结束
bool audioBufferOver;                                        ///< 标记录音缓存块是否已充满新数据`
int onsetsCount;                                             ///< 标记onsets个数
clock_t tStart, tEnd;                                        ///< 计时器，用于计算节奏快慢
int speedInt;                                                ///< 音乐速度
bool isOutMusicPlay;                                         ///< 标记是否是外部播放音乐
int qRobotState = 0;                                         ///< 当前状态，0为关闭，1为待机，2为自己播放音乐，3为外部播放音乐
char *commandChar[] = {
	"关机","待机", "播放音乐", "听音乐", "录像"
};                                                           ///< 命令，0代表关机，1代表待机，2代表播放音乐，3代表外部音乐，4代表录像

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

//!语音合成函数
/*!
 * \brief 用于语音合成及播放
 *
 *  \param text 要转换为语音的字符串
 *
 */
void textToAudio( char * text ){
	if ( text2Audio == NULL ){
		text2Audio = new Text2Audio();
	}
	char * tempAudio = "tempAudio.wav";
	text2Audio->getAudio(text, tempAudio);
	if ( wavPlayer == NULL ){
		wavPlayer = new WavPlayer();
	}
	wavPlayer->playWav( tempAudio);
}

//!选歌函数
/*!
 * \brief 根据给定表情选择歌曲
 *
 * 总共有7种表情，根据表情，选择相应的音乐
 *
 */
void selectSong(){
	char * songDir = NULL;
	char * songName = NULL;
	char * text = NULL;
	switch( emotion ){
		case sad: 
			text = "你看起来不高兴";
			songName = "反方向的钟";
			songDir = "../../Music/sad/反方向的钟.mp3";
			break;
		case happy:
			text = "你看起来很开心";
			songName = "Beat It";
			songDir = "../../Music/happy/杰克逊.mp3";
			break;
		case angry:
			text = "你看起来很生气";
			songName = "斗牛";
			songDir = "../../Music/angry/斗牛.mp3";
			break;
		case calm :
			text = "你看起来很平静";
			songName = "秋";
			songDir = "../../Music/calm/秋.mp3";
			break;
		case confused :
			text = "你看起来很疑惑";
			songName = "阳光宅男";
			songDir = "../../Music/confused/阳光宅男.mp3";
			break;
		case disgust :
			text = "你看起来很厌恶";
			songName = "斗牛";
			songDir = "../../Music/disgust/斗牛.mp3";
			break;
		case surprised :
			text = "你看起来很惊讶";
			songName = "双截棍";
			songDir = "../../Music/surprise/双截棍.mp3";
			break;
	}
	textToAudio(text);
	printf("正在播放歌曲 ： %s\n", songName);
	musicPlayer->pcmBufferInit(FRAME_NUM);
	audioBufferOver = true;
	pthread_cond_signal(&cond1);
	musicPlayer->playMusic( songDir, "default", 44100, 2 );
}

//!关机函数
/*!
 * \brief 调用该函数，用以关机
 *
 * 调用该函数，结束各个线程
 *
 */
void closeRobot(){
	qRobotState = 0;
	(*qRobotController).reset();
	printf("关机...\n");
	pthread_cancel(0);
	pthread_cancel(1);
	pthread_cancel(2);
	pthread_cancel(3);
}

//!语音输入
/*!
 * \brief 接受语音，翻译成字符串
 *
 * 调用该函数，开始录音，并进行语音转换
 *
 * \param outText 转换的结果的指针
 *
 */
void audioToText( char * & outText ){
	if ( audio2Text == NULL ){
		audio2Text = new Audio2Text( LOGIN_CONFIGS, VOICE2TEXT_PARAM );
	}
	if ( audioController != NULL ){
		delete audioController;
		audioController = NULL;
	}
	audioController = new AudioController( "default", 512, 16000, 1);
	printf("请说话...\n");
	char * tempWavFile = "tempWav.wav";
	(*audioController).pcmToWav(tempWavFile, 4);
	printf("说话结束\n");
	//audio2Text->getText(tempWavFile, outText);                     //讯飞语音
	audio2Text->getTextFromBD(tempWavFile,"wav",16000,outText);      //百度语音
	//printf("outText: %s\n", outText);
}

//!字符串匹配
/*!
 * \brief 判断两字符串是否匹配
 *
 * 判断字符串2是否是字符串1的字串
 *
 * \param str1 要匹配的字符串1
 * \param str2 要匹配的字符串2
 *
 * \retval true 匹配
 * \retval false 不匹配
 *
 */
bool strMatch( char *& str1, char *& str2 ){
	for ( int i = 0 ; i < strlen(str1) ; i ++ ){
		if ( str2[0] == str1[i] ){
			for ( int j = 1 ; j < strlen(str2) ; j ++ ){
				if ( str1[i+j] != str2[j] ){
					break;
				}
				if ( j == strlen(str2)-1 ){
					return true;
				}
			}
		}
	}
	return false;
}

//!命令匹配
/*!
 * \brief 用给定的字符串匹配已有命令
 *
 * 用给定的字符串匹配已有命令，并返回
 *
 * \param text 给定的字符串
 *
 * \retval 0 关机命令
 * \retval 1 待机命令
 * \retval 2 播放音乐命令
 * \retval 3 听外部音乐命令
 * \retval 4 录像命令
 * \retval -1 指令错误（没有可以匹配的命令）
 *
 */
int commandMatch( char *& text ){
	for ( int i = 0 ; i < 5 ; i ++ ){
		if ( strMatch(text, commandChar[i]) ){
			return i;
		}
	}
	return -1;
}


//!线程函数
/*!
 * \brief 用于实现线程的动作
 *
 * 线程的具体动作在此函数实现。总共有5个线程，线程0负责外部音乐录入；
 * 线程1负责音乐节奏分析及调用动作；线程2负责语音录入、语音识别、歌曲
 * 检索和歌曲播放；线程3用于视频录入及表情识别；线程4用于控制其他线程
 * 及整体流程。
 * 线程0和线程2不能同时进行，线程1只能对内部（线程0）或外部（线程1）1
 * 种音乐进行处理
 *
 * \param threadId 线程id
 *
 */
void * threadRun( void *threadId ){
	long tId = (long)threadId;
	if ( tId == 0 ){
		while ( true ){
			printf("线程0\n");
			pthread_mutex_lock(&mutex0);
			pthread_cond_wait(&cond0,&mutex0);
			pthread_mutex_unlock(&mutex0);
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
		printf("线程1\n");
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
					//printf("Y\n");

				}else{
					onsetsCount ++;
					//printf("N\n");
				}
				pastCount --;
				if ( onsetsCount == onsetsNum && pastCount <= 0 ){

					tEnd = clock();
					printf("%d\n", (int)(tEnd-tStart));
					setSpeed((int)(tEnd-tStart));
					//setSpeed(1700000);
					qRobotAction->dance();
					//qRobotController->verticalHead(1, 40);
					//sleep(1);
					wingMovement();
					//qRobotAction->suit1RandAction();
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
			printf("线程2\n");
			pthread_mutex_lock(&mutex2);
			pthread_cond_wait(&cond2,&mutex2);
			pthread_mutex_unlock(&mutex2);

			if ( isOutMusicPlay ){
				continue;
			}
			char * outText = new char[1024];
			audioToText(outText);
			printf("正在播放歌曲 ： %s\n", outText);
			musicPlayer->pcmBufferInit(FRAME_NUM);
			audioBufferOver = true;
			pthread_cond_signal(&cond1);

			musicPlayer->playMusic( audio2Text->findAudio("../../Music", outText), "default", 44100, 2 );

			delete outText;
			outText = NULL;
		}
		printf("线程2结束\n");
	}else if( tId == 3 ){
		while ( true ){
			ifDetctOver = false;
			printf("线程3\n");
			pthread_mutex_lock(&mutex3);
			pthread_cond_wait(&cond3,&mutex3);
			pthread_mutex_unlock(&mutex3);
			//usleep(1000000);
			for ( int i = 0 ; i < 10000000 ; i ++ ){
			
			}
			emotion = peopleDetection->getEmotion(ifEmotionDetected);
			ifDetctOver = true;
		}
		printf("线程3结束\n");
	}else if( tId == 4 ){
		peopleDetection = new PeopleDetection(1);
		qRobotState = 1;
		/*
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
		   closeRobot();
		   break;
		   }else if( ( qRobotState == 1 || qRobotState == 3 ) && touchInfo == 0x04 ){                                      //内部播放音乐
		   printf("内部音乐\n");
		   qRobotState = 2;
		   int info = (*qRobotController).getTouchInfo();
		   printf("ok!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		   if ( musicPlayer == NULL ){
		   musicPlayer = new MusicPlayer();
		   }
		   if ( info == 0x02 ){              //拍照对焦
		   printf("正在录像...\n");
		   peopleDetection->captureVideo();                            //拍照
		   while ( (*qRobotController).getTouchInfo() == 0x02 ){
		   emotion = null;
		   peopleDetection->setCapture(true);
		   pthread_cond_signal(&cond3);
		   while ( !ifDetctOver ){}
		   if ( emotion != null ){
		   selectSong();
		   break;
		   }
		   }
		   printf("录像结束\n");
		   }else if( info == 0x04 ){         //语音点歌

		   isOutMusicPlay = false;
		   if ( audio2Text == NULL ){
		   audio2Text = new Audio2Text(LOGIN_CONFIGS, VOICE2TEXT_PARAM);
		   } 
		   }
		   if ( audioController != NULL ){
		   delete audioController;
		   audioController = NULL;
		   audioController = new AudioController( "default", 512, 16000, 1);
		   onsetsNum = 10;
		   pthread_cond_signal(&cond2);
		   }
		   }
		   }
		   */
		while ( true ){
			int touchInfo = (*qRobotController).getTouchInfo();
			if ( touchInfo == 0x02 ){
				char * outText = new char[1024];
				audioToText(outText);
				printf("outText2:%s\n", outText);
				int commandInt = commandMatch(outText);
				switch( commandInt ){
					case 0 : 
						closeRobot();
						break;
					case 1 : 
						break;
					case 2 :
						printf("当前命令：播放音乐\n");
						textToAudio("请点歌");
						if ( musicPlayer == NULL ){
							musicPlayer = new MusicPlayer();
						}
						isOutMusicPlay = false;
						onsetsNum = 10;
						pthread_cond_signal(&cond2);
						break;
					case 3 :
						printf("当前命令：听音乐\n");
						textToAudio("请播放音乐");
						audioBufferOver = false;
						if ( musicPlayer != NULL ){
							musicPlayer->setIsPlay(false);
						}
						onsetsNum = 16;
						if ( audioController != NULL ){
							delete audioController;
							audioController = NULL;
						}
						printf("正在打开设备...\n");
						audioController = new AudioController( AUDIO_NAME, FRAME_NUM, SAMPLE_RATE, 1);
						pthread_cond_signal(&cond0);
						isOutMusicPlay = true;
						if ( musicPlayer == NULL ){
							pthread_cond_signal(&cond1);
						}
						break;
					case 4 :
						printf("当前命令：录像\n");
						if ( musicPlayer != NULL ){
							musicPlayer->setIsPlay(false);
						}else{
							musicPlayer = new MusicPlayer();
						}
						printf("正在录像...\n");
						textToAudio("正在录像，按c键拍照");
						peopleDetection->captureVideo();                            //录像
						while ( true ){
							//int info = -1;
							//info = (*qRobotController).getTouchInfo();
							if ( peopleDetection->isCapture() ){
								peopleDetection->setCapture(false);
								printf("拍照\n");
								emotion = null;
								peopleDetection->setCapture(true);
								pthread_cond_signal(&cond3);
								while ( !ifDetctOver ){}
								ifDetctOver = false;
								if ( emotion != null ){
									selectSong();
									break;
								}
							}
						}
						printf("录像结束\n");
						break;
					default :
						textToAudio("没有命令或命令错误，请重新操作");
						printf("没有命令或错误命令\n");
						break;

				}
				delete outText;
				outText = NULL;
			}else if( touchInfo == 0x12 ){
				closeRobot();
			}
		}
		printf("线程4结束\n");
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
	text2Audio = new Text2Audio();

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
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);
	pthread_cond_destroy(&cond0);
	pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond2);
	pthread_cond_destroy(&cond3);
	

	deleteClass();

	return 0;
}
