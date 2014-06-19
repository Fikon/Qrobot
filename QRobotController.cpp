/*!
 *
 *\file QRobotController.cpp 
 *
 *对QRobotController的实现
 *
 *\author 蒋慧强
 *
 */

#include "QRobotController.h"
#include <iostream>

using namespace std;

//!定义usb接口的ID 用于打开设备
#define USB_RUICHEN_COID 0x0471
#define USB_RUICHEN_PDTID 0x1700
#define USB_AUDIO_COID 0x05e1
#define USB_AUDIO_PDTID 0x202a

/*!
 *
 *定义的一些常量，标记了机器人各个部位所对应在交互的数组
 *中的位置
 *
 */
//!写数据
const int LWING_UD_AND_SPD=13;

const int LWING_TIME=14;

const int RWING_UP_AND_SPD=11;

const int RWING_TIME=12;

const int HEAD_VERTICAL_SPD=7;

const int HEAD_VERTICAL_RANGE=8;

const int HEAD_HORIZONTAL_SPD=9;

const int HEAD_HORIZONTAL_RANGE=10;

const int EYE_ORDER=15;

const int EYE_SHOW_TIME=16;

const int HEART_COLOR1=17;

const int HEART_COLOR2=18;
//!读数据
const int HEAD_VERTICAL_PST=5;

const int HEAD_HORIZONTAL_PST=6;

const int HEAD_TOUCH=10;

const int POWER=11;

const unsigned int TIME_OUT=0;

//!定义与usb交互的字节数组
unsigned char resetBuffer[25]={0xF7, 0xF7,0x00 ,0x14 ,0x00 ,0x0F ,0x0F ,0x2B ,0x90 ,0x2E ,0x80 ,0x4E ,0x00 ,0x4E ,0x00 ,0x00 ,0x00 ,0xFF ,0xF0 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x0E};

//////////////////////////////////////////
void printBuff(unsigned char buff[])
{
   for(int i=0;i<25;i++)
      cout<<(int)buff[i]<<",";
   cout<<endl;
}
//////////////////////////////////////////
//!构造函数
/*!
 *
 *初始化usb设备
 *
 */
QRobotController * QRobotController::controller=NULL;
QRobotController::QRobotController()
{
   //!设置机器人的电源状态为开启
   powerState=true;
   transferred=0;
   int rv=0;
   //!初始化usb设备
   rv= libusb_init(NULL);
   if(rv<0)
   {
     cout<<"usb device init failed!!~"<<endl;
   }
   //!开启机器人的控制设备usb
   command_device_handle=libusb_open_device_with_vid_pid(NULL,USB_RUICHEN_COID,USB_RUICHEN_PDTID);
   if(command_device_handle==NULL)
   {
     cout<<"failed to open the command usb device!!~"<<endl;
   }
   cout<<"inited!!~"<<endl;
}

//!reset the robot~
bool QRobotController::reset()
{
   cout<<"reset the robot..."<<endl;
   int ret=0;
   ret=libusb_interrupt_transfer(command_device_handle,1,resetBuffer,
            25,&transferred,TIME_OUT);
   
   if(ret==0)
      return true;
   else
      return false;
}

//!make the left wing up move!!~
bool QRobotController::leftWingUp(int speed,int time)
{
   cout<<"left wing up moving!!~"<<endl;
   unsigned char buffer[25];
   //!两个临时变量，用于将输入的十进制转化为十六进制数据
   char convertBuff[20];
   int tmp;
   generateBuffer(buffer);
   switch(speed)
   {
   case 1:
      buffer[LWING_UD_AND_SPD]=0x27;
      break;
   case 2:
      buffer[LWING_UD_AND_SPD]=0x2B;
      break;
   case 3:
      buffer[LWING_UD_AND_SPD]=0x2E;
   }
   sprintf(convertBuff,"%X",int(time*2.55));
   sscanf(convertBuff,"%X",&tmp);
   buffer[LWING_TIME]=tmp;
   generateAudit(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   printBuff(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   int retval=0;
   retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
   if(retval==0)
      return true;
   else
   {
      cout<<"Error!--------->left wing up move failed!!~"<<endl;
      return false;
   }
}

//!make the left wing reset!!~
bool QRobotController::leftWingOrg(int speed)
{
   cout<<"left wing reset!!~"<<endl;
   unsigned char buffer[25];
   generateBuffer(buffer);
   switch(speed)
   {
   case 1:
      buffer[LWING_UD_AND_SPD]=0x47;
      break;
   case 2:
      buffer[LWING_UD_AND_SPD]=0x4B;
      break;
   case 3:
      buffer[LWING_UD_AND_SPD]=0x4E;
   }
   generateAudit(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<
   printBuff(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<
   int retval=0;
   retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
   if(retval==0)
      return true;
   else
   {
      cout<<"Error!-------------->left wing reset failed!!~"<<endl;
      return false;
   }
}

//!make the left wing down!!~
bool QRobotController::leftWingDown(int speed,int time)
{
   cout<<"left wing move down!!~"<<endl;
   unsigned char buffer[25];
   //!两个临时变量用于将十进制数据转化为十六进制
   char convertBuff[20];
   int tmp;
   generateBuffer(buffer);
   switch(speed)
   {
   case 1:
      buffer[LWING_UD_AND_SPD]=0x37;
      break;
   case 2:
      buffer[LWING_UD_AND_SPD]=0x3B;
      break;
   case 3:
      buffer[LWING_UD_AND_SPD]=0x3E;
    }
   sprintf(convertBuff,"%X",int(time*2.55));
   sscanf(convertBuff,"%X",&tmp);
   buffer[LWING_TIME]=tmp;
   generateAudit(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   printBuff(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   int retval=0;
   retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
   if(retval==0)
      return true;
   else
   {
      cout<<"Error!----------->left wing down failed!!~"<<endl;
      return false;
   }
}

//make the right wing up move!!~
bool QRobotController::rightWingUp(int speed,int time)
{
   cout<<"right wing up move!!~"<<endl;
   unsigned char buffer[25];
   generateBuffer(buffer);
   //!临时变量
   char convertBuff[20];
   int tmp;
   int retval;
   switch(speed)   
   {
   case 1:
      buffer[RWING_UP_AND_SPD]=0x27;
      break;
   case 2:
      buffer[RWING_UP_AND_SPD]=0x2B;
      break;
   case 3:
      buffer[RWING_UP_AND_SPD]=0x2E;
    }
   sprintf(convertBuff,"%X",int(time*2.55));
   sscanf(convertBuff,"%X",&tmp);
   buffer[RWING_TIME]=tmp;
   generateAudit(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    printBuff(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,
&transferred,TIME_OUT);
   if(retval==0)
      return true;
   else
   {
     cout<<"Error!------------>right wing up move failed!!~"<<endl;
      return false;
   }
}

//!make the right wing down move!!~
bool QRobotController::rightWingDown(int speed,int time)
{
   cout<<"right wing move down!!~"<<endl;
   unsigned char buffer[25];
   generateBuffer(buffer);
   //!临时变量
   char convertBuff[20];
   int tmp;
   int retval;
   switch(speed)
   {
   case 1:
      buffer[RWING_UP_AND_SPD]=0x37;
      break;
   case 2:
      buffer[RWING_UP_AND_SPD]=0x3B;
      break;
   case 3:
      buffer[RWING_UP_AND_SPD]=0x3E;
   }
   sprintf(convertBuff,"%X",int(time*2.55));
   sscanf(convertBuff,"%X",&tmp);
   buffer[RWING_TIME]=tmp;
   generateAudit(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<
   printBuff(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<
   retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
   if(retval==0)
      return true;
   else
   {
      cout<<"Error!------------->right wing move failed!!~"<<endl;
      return false;
   }
}

//!make the right wing reset!!~
bool QRobotController::rightWingOrg(int speed)
{
   cout<<"right wing reset!!~"<<endl;
   unsigned char buffer[25];
   generateBuffer(buffer);
   //!临时变量
   char convertBuff[20];
   int tmp;
   int retval;
   switch(speed)
   {
   case 1:
      buffer[RWING_UP_AND_SPD]=0x47;
      break;
   case 2:
      buffer[RWING_UP_AND_SPD]=0x4B;
      break;
   case 3:
      buffer[RWING_UP_AND_SPD]=0x4E;
   }
   generateAudit(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   printBuff(buffer);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
   if(retval==0)
       return true;
   else
   {
       cout<<"Error!------------->right wing reset failed!!~"<<endl;
       return false;
   }
}

//!make the head move horizontally!!~
bool QRobotController::horizontalHead(int speed,int range)
{
    cout<<"head move horizontally!!~"<<endl;
    unsigned char buffer[25];
    generateBuffer(buffer);
    //!临时变量
    char convertBuff[20];
    int tmp;
    int retval;
    switch(speed)
    {
    case 1:
       buffer[HEAD_HORIZONTAL_SPD]=0x27;
       break;
    case 2:
       buffer[HEAD_HORIZONTAL_SPD]=0x2B;
       break;
    case 3:
       buffer[HEAD_HORIZONTAL_SPD]=0x2E;
    }
    sprintf(convertBuff,"%X",int(range*1.6));
    sscanf(convertBuff,"%X",&tmp);
    buffer[HEAD_HORIZONTAL_RANGE]=(0x30+tmp);
    generateAudit(buffer);
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    printBuff(buffer);
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
    if(retval==0)
        return true;
    else
    {
        cout<<"head horizontal move failed!!~"<<endl;
        return false;
    }
}

//!make the head reset horizontally!!~
bool QRobotController::horizontalHeadOrg(int speed)
{
   cout<<"head reset horizontally!!~"<<endl;
   unsigned char buffer[25];
   generateBuffer(buffer);
   int retval;
   switch(speed)
   {
    case 1:
       buffer[HEAD_HORIZONTAL_SPD]=0x27;
       break;
    case 2:
       buffer[HEAD_HORIZONTAL_SPD]=0x2B;
       break;
    case 3:
       buffer[HEAD_HORIZONTAL_SPD]=0x2E;
    }
    buffer[HEAD_HORIZONTAL_RANGE]=0x80;
    generateAudit(buffer);
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    printBuff(buffer);
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<,
    retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
    if(retval==0)
       return true;
    else
    {
       cout<<"Error!----------------->head horizontal reset failed!!~"<<endl;
       return false;
    }
}

//!make the head move vertically!!~
bool QRobotController::verticalHead(int speed,int range)
{
    cout<<"head move vertically!!~"<<endl;
    unsigned char buffer[25];
    generateBuffer(buffer);
    //!临时变量
    char convertBuff[20];
    int tmp;
    int retval;
    switch(speed)
    {
    case 1:
       buffer[HEAD_VERTICAL_SPD]=0x27;
       break;
    case 2:
       buffer[HEAD_VERTICAL_SPD]=0x29;
       break;
    case 3:
       buffer[HEAD_VERTICAL_SPD]=0x2B;
       break;
     }
     sprintf(convertBuff,"%X",range);
     sscanf(convertBuff,"%X",&tmp);
     buffer[HEAD_VERTICAL_RANGE]=(0x90-tmp);
     generateAudit(buffer);
     //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     printBuff(buffer);
     //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
     if(retval==0)
        return true;
     else
     {
        cout<<"Error!------>head vertical moving failed!!~"<<endl;
        return false;
     }
}

//!make the head reset vertically!!~
bool QRobotController::verticalHeadOrg(int speed)
{
    cout<<"head reset vertically!!~"<<endl;
    unsigned char buffer[25];
    generateBuffer(buffer);
    int retval;
    switch(speed)
    {
     case 1:
         buffer[HEAD_VERTICAL_SPD]=0x27;
         break;
     case 2:
         buffer[HEAD_VERTICAL_SPD]=0x29;
         break;
     case 3:
         buffer[HEAD_VERTICAL_SPD]=0x2B;
    }
    buffer[HEAD_VERTICAL_RANGE]=0x7C;
    generateAudit(buffer);
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    printBuff(buffer);
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
    if(retval==0)
        return true;
    else
    {
        cout<<"Error!--------------->head vertical reset failed!!~"<<endl;
        return false;
    }
}

//!set the heart's color!!~
bool QRobotController::setHeartColor(int red,int green,int blue)
{
    cout<<"set heart's color"<<endl;
    unsigned char buffer[25];
    generateBuffer(buffer);
    //!临时变量
    char convertBuff[20];
    int tmp1,tmp2,tmp3;
    int retval;
    tmp1=((red-1)/17*16)|((green-1)/17);
    tmp2=(blue-1)/17*16;
    sprintf(convertBuff,"%X",tmp1);
    sscanf(convertBuff,"%X",&tmp3);
    buffer[HEART_COLOR1]=tmp3;
    sprintf(convertBuff,"%X",tmp2);
    sscanf(convertBuff,"%X",&tmp3);
    buffer[HEART_COLOR2]=tmp3;
    generateAudit(buffer);
    retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
    if(retval==0)
        return true;
    else
    {
        cout<<"set color failed!!~"<<endl;
        return false;
    }
}

//!set the eye's the picture!!~
bool QRobotController::setEyePic(int time,int face)
{
    cout<<"set the eye's picture!!~"<<endl;
    unsigned char buffer[25];
    generateBuffer(buffer);
    //!临时变量
    char convertBuff1[20],convertBuffer2[20];
    int tmp1,tmp2;
    int retval;
    sprintf(convertBuff1,"%X",face);
    sscanf(convertBuff1,"%X",&tmp1);
    buffer[EYE_ORDER]=tmp1;
    sprintf(convertBuffer2,"%X",time);
    sscanf(convertBuffer2,"%X",&tmp2);
    buffer[EYE_SHOW_TIME]=tmp2;
    generateAudit(buffer);
    //,,,,,,,,,,,,,,,,,,,,,,,,,,,,
    printBuff(buffer);
    //，，，，，，，，，，，，，，，，，
    retval=libusb_bulk_transfer(command_device_handle,1,buffer,25,&transferred,TIME_OUT);
    if(retval==0)
        return true;
    else
    {
        cout<<"Error---->Set the eye's picture failed!!~"<<endl;
        return false;
    }
}

//!get the head's vertical information!!~
int QRobotController::getHeadVertical()
{
   unsigned char buffer[25];
   libusb_interrupt_transfer(command_device_handle,0x81,buffer,25,&transferred,0);
   return buffer[5]-0x68;
}

//!get the head's horizontal information!!~
int QRobotController::getHeadHorizontal()
{
    unsigned char buffer[25];
    libusb_interrupt_transfer(command_device_handle,0x81,buffer,25,&transferred,0);
    return buffer[6]-0x30;
}

//!get the touch-information!!~
int QRobotController::getTouchInfo()
{
   unsigned char buffer[25];
   libusb_interrupt_transfer(command_device_handle,0x81,buffer,25,&transferred,0);
   return buffer[10];
}









































































   







