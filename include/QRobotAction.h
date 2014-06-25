#include"QRobotController.h"
#include<iostream>
#include<cstdlib>
#include<time.h>

using namespace std;

class QRobotAction{
private:
	//!controller实例
	QRobotController* controller;
	//!速度
	int speed;

public:
	QRobotAction();

	//!设置速度
	void setSpeed(int speed1);
	void randAction1();
	void randAction2();
	void randAction3();
	void randAction4();
	
	void dance();
};
