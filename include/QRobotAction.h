/*
*\file QRobotAction.h
*\
*
*\author 丁东辉
*/
#include"../include/QRobotController.h"
#include<iostream>
#include<cstdlib>
#include<time.h>
using namespace std;

class QRobotAction{
private:
	//!action实例
	//static QRobotAction* action;
	//!controller实例
	QRobotController* controller;
	//!速度
	int speed;
	//水平方向时间间隔标准
	int hInterval1;
	int hInterval2;
	int hInterval3;
	int sHInterval;

	//垂直方向时间间隔标准
	int vInterval1;
	int vInterval2;
	int vInterval3;
	int sVInterval;	
public:
	QRobotAction();
	
	//!设置速度
	//以下是随机动作
	void setSpeed(int speed1);
	void setEyeAndColor();
	void randAction1();
	void randAction2();
	void randAction3();
	void randAction4();
	void randAction5();
	void randAction6();
	void randAction7();
	void randAction8();
	void randAction9();
	void randAction10();
	void randAction11();
	void randAction12();
	void randAction13();
	void randAction14();
	void randAction15();
	//动作函数，供外部调用
	void dance();
};

