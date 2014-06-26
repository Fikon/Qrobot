/*
*\file QRobotAction.cpp
*\
*
*\author 丁东辉
*/
#include"QRobotAction.h"
using namespace std;

//构造函数
QRobotAction::QRobotAction(){
	controller = QRobotController::getInstance();
	speed = 1;
	//水平方向时间间隔标准
	hInterval1 = 10000;
	hInterval2 = 9000;
	hInterval3 = 8000;

	//垂直方向时间间隔标准
	vInterval1 = 10500;
	vInterval2 = 9000;
	vInterval3 = 8000;
}
	
/*
*设置速度
* param speed1 速度值 
*/
void QRobotAction::setSpeed(int speed1){
	speed = speed1;
}

/*
*随机动作一
*水平转动一定角度,两个点
*/
void QRobotAction::randAction1(){
	int number1 = 0;
	int number2 = 0;
	int temp = 0;
	number1 = rand() % 101;
	controller->horizontalHead(speed, number1);
	usleep(number1 * sHInterval / 2);
	//眼睛表情和颜色
	setEyeAndColor();
	number2 = rand() % 101;
	temp = number2 > number1? number2 - number1 : number1 - number2;
	controller->horizontalHead(speed, number2);
	usleep(temp * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
}
	
/*
*随机动作二
*垂直转动一定角度，两个点
*/
void QRobotAction::randAction2(){
	int number1 = 0;
	int number2 = 0;
	int temp = 0;
	number1 = rand() % 41;
	controller->verticalHead(speed, number1);
	usleep(number1 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	number2 = rand() % 41;
	temp = number2 > number1? number2 - number1 : number1 - number2;
	controller->verticalHead(speed, number2);
	usleep(temp * sVInterval);
	//眼睛表情和颜色
	setEyeAndColor();
}


/*
*随机动作三
*水平来回转动一次
*/
void QRobotAction::randAction3(){
	int number = 0;
	int temp = 0;
	int count = 2;
	number = rand() % 20 + 20;
	while (count --){
	controller->horizontalHead(speed, 50 + number);
	usleep(2 * number * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller->horizontalHead(speed, 50 - number);
	usleep(2 * number * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	}
}

/*
*随机动作四
*垂直来回转动一次
*/
void QRobotAction::randAction4(){
	int number = 0;
	int temp = 0;
	int count = 2;
	number = rand() % 8 + 13;
	//当前垂直方向角度
	while(count --){
	controller->verticalHead(speed, 20 + number);
	usleep(2 * number * sVInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller->verticalHead(speed, 20 - number);
	usleep(2 * number * sVInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	}
}	

/*
*随机动作五
*水平来回转动
*/
void QRobotAction::randAction5(){
	int number = 0;
	int temp = 0;
	number = rand()%11 + 40;
	controller->horizontalHead(speed, 50 + number);
	usleep(number * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	temp = rand()%11;
	controller->horizontalHead(speed, 50 - temp);
	usleep((temp + number) * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();

	for(int i=0; i < 4; i++){
	controller->horizontalHead(1, 100);
	usleep(100000);
	//眼睛表情和颜色
	setEyeAndColor();
	controller->horizontalHead(1, 0);	
	usleep(100000);
	}
	controller->horizontalHead(speed, 50 + number);
	usleep(number* sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
}



/*
*随机动作六
*水平来回转动
*/
void QRobotAction::randAction6(){
	int number = 0;
	int temp = 0;
	number = rand()%21 + 30;
	controller->horizontalHead(speed, 50 - number);
	usleep(number * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	temp = rand()%11;
	controller->horizontalHead(speed, 50 - temp);
	usleep((temp + number) * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();

	for(int i=0; i < 4; i++){
	controller->horizontalHead(1, 100);
	usleep(100000);
	//眼睛表情和颜色
	setEyeAndColor();
	controller->horizontalHead(1, 0);	
	usleep(100000);
	}
	controller->horizontalHead(speed, 50 - number);
	usleep(number* sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
}

void QRobotAction::randAction7(){
	int count = 4;
	while(count --){
	controller->verticalHead(1, 40);
	usleep(125000);
	//眼睛表情和颜色
	setEyeAndColor();
	controller->verticalHead(1, 0);	
	usleep(125000);
	//眼睛表情和颜色
	setEyeAndColor();
	}
}

void QRobotAction::randAction8(){
	int count = 4;
	while(count --){
	controller->verticalHead(3, 40);
	usleep(80000);
	//眼睛表情和颜色
	setEyeAndColor();
	controller->verticalHead(3, 0);	
	usleep(80000);
	//眼睛表情和颜色
	setEyeAndColor();
	}
}

//上下颤动运动
void QRobotAction::randAction9(){
	int count = 3;
	while(count --){
	controller->verticalHead(2, 40);
	usleep(113000);
	//眼睛表情和颜色
	setEyeAndColor();
	controller->verticalHead(2, 0);	
	usleep(113000);
	//眼睛表情和颜色
	setEyeAndColor();
	}
}

//斜线往右运动
void QRobotAction::randAction10(){
	int count = 2;
	while(count --){
	controller -> horizontalHead(speed, 50);
	controller -> verticalHead(speed, 0);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 10);
	controller -> verticalHead(speed, 40);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
}
}

//斜线往左运动
void QRobotAction::randAction11(){
	int count = 2;
	while(count --){
	controller -> horizontalHead(speed, 50);
	controller -> verticalHead(speed, 0);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 90);
	controller -> verticalHead(speed, 40);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
}
}



//斜线往左然后往右
void QRobotAction::randAction12(){
	controller -> horizontalHead(speed, 50);
	controller -> verticalHead(speed, 0);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 90);
	controller -> verticalHead(speed, 40);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 50);
	controller -> verticalHead(speed, 0);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 10);
	controller -> verticalHead(speed, 40);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 50);
	controller -> verticalHead(speed, 0);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
}

//斜线三角形
void QRobotAction::randAction13(){
	controller -> horizontalHead(speed, 50);
	controller -> verticalHead(speed, 0);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 90);
	controller -> verticalHead(speed, 40);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 10);
	//controller -> verticalHead(speed, 40);
	usleep(80 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 50);
	controller -> verticalHead(speed, 0);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
}

//特定的水平向左摆动
void QRobotAction::randAction14(){
	int count = 2;
	while(count --){
	controller -> horizontalHead(speed, 50);
	usleep(35 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 10);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	}
}

//特定的水平向右摆动
void QRobotAction::randAction15(){
	int count = 2;
	while(count --){
	controller -> horizontalHead(speed, 50);
	usleep(35 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	controller -> horizontalHead(speed, 90);
	usleep(40 * sHInterval);
	//眼睛表情和颜色
	setEyeAndColor();
	}
}

//设置随机眼睛和胸部颜色
void QRobotAction::setEyeAndColor(){
	controller->setHeartColor(rand() % 256,rand() %256,rand()%256);
	controller->setEyePic(rand() %50 +1,rand() % 255 + 1);
}

//动作函数
void QRobotAction::dance(){
	srand(time(NULL));
	int temp = 0;
	temp = rand() % 13;
	//眼睛表情和颜色
	setEyeAndColor();
	cout<<"**************temp = "<<temp<<"********"<<endl;
	switch(speed){
		case 1: sHInterval = hInterval1;
			sVInterval = vInterval1;
			break;
		case 2: sHInterval = hInterval2;
			sVInterval = vInterval2;
			break;
		case 3: sHInterval = hInterval3;
			sVInterval = vInterval3;
			break;
	}
	switch(temp){
		case 0:
			randAction1();
			break;
		case 1:	
			randAction2();
			break;
		case 2: 
			randAction3();
			break;
		case 3:
			randAction4();
			break;
		case 4:
			randAction5();
			break;
		case 5:	
			randAction6();
			break;
		case 6: 
			switch(speed){
				case 1: randAction7(); break;
				case 2: randAction8(); break;
				case 3: randAction9(); break;
				}
			break;
		case 7:
			randAction10();
			break;
		case 8:
			randAction11();
			break;
		case 9: randAction12();
			break;
		case 10: randAction13();
			break;
		case 11: randAction14();
			break;
		case 12: randAction15();
			break;
	}
			//眼睛表情和颜色
	setEyeAndColor();
}
int main(){
	QRobotAction* action = new QRobotAction();
	action -> dance();
}
