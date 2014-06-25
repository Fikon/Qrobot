/*
 *\file QRobotAction.cpp
 *\
 *
 *\author 丁东辉
 */
#include "../include/QRobotAction.h"

QRobotAction::QRobotAction(){
	controller = QRobotController::getInstance();
	speed = 1;
}

//!设置速度
void QRobotAction::setSpeed(int speed1){
	speed = speed1;
}

void QRobotAction::randAction1(){
	int number = 0;
	int temp = 0;
	number = rand() % 81;
	controller->horizontalHead(speed, number);
	number = 40 - number;
	usleep(15000);
}

void QRobotAction::randAction2(){
	int number = 0;
	int temp = 0;
	number = rand() % 41;
	controller->verticalHead(speed, number);
	number = 20 - number;
	usleep(10000);
}

void QRobotAction::randAction3(){
	int number = 0;
	int temp = 0;
	temp = rand()% 81;
	number = temp -40;
	controller->horizontalHead(speed, 40 + number);
	int speedInt;
	usleep(temp * 3000);
	controller->horizontalHead(speed, 40 - number);
}

void QRobotAction::randAction4(){
	int number = 0;
	int temp = 0;
	temp = rand() % 41;
	number = temp - 20;
	controller->verticalHead(speed, 20 + number);
	usleep(temp * 2000);
	controller->verticalHead(speed, 20 - number);
}


void QRobotAction::dance(){
	srand(time(NULL));
	int number = 0;
	int temp = 0;
	controller->setHeartColor(rand() % 256,rand() % 256,rand()%256);
	controller->setEyePic(rand()%20,rand() % 256);
	switch(rand() % 8){

		case 0:
			randAction1();
			break;
		case 1:	//上下随机头
			randAction2();
			break;
		case 2: //水平两次头
			randAction3();
			break;
		case 3://上下两次头
			randAction4();
			break;
		case 4:
			randAction1();
			randAction4();
			break;
		case 5:	//上下随机头
			randAction2();
			randAction3();
			break;
		case 6: //水平两次头
			randAction1();
			randAction2();
			break;
		case 7://上下两次头
			randAction3();
			randAction4();
			break;
	}

}

