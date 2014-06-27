/*!
 * \file Audio2Text.h 
 *
 * \brief define the Audio2Text class
 *
 * Audio2Text is a class has some function which transform the audio
 * file to the plain text
 *
 * \author kiky.jiang@gmail.com
 *
 */

#ifndef AUDIO_2_TEXT_H
#define AUDIO_2_TEXT_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

using namespace std;

class Audio2Text
{
private:
     //!some information on logging in
     char *login_configs;
   
     //!some parameters for Xunfei API
     char *param;
   
     //!a function to check if two (char *) is equal
     bool compare(char * str1,char *str2)
     {
        int min=strlen(str1)>=strlen(str2)?strlen(str2):strlen(str1);
        for(int i=0;i<min;i++)
           if(str1[i]!=str2[i])
                 return false;
        return true;
     }
public:
     //!constructor method
     /*!
      * initial the login_configs and param
      *
      */
     Audio2Text(char * _config,char * _param);
    
     //!getter method for login_configs
     char * getConfigs()
     {
         return login_configs;
     }

     //!getter method for param
     char * getParam()
     {
         return param;
     }

     //!the main function that transform the audio to text
     /*!
      * \param fileName the input audio file's absolute name
      * \param text the plain text which tramsform form the audio file
      *
      */
     void getText(const char * fileName, char *text);
    
     //!find the mucis file in a specific direcotry
     /*!
      * \param dir the directory to be found
      * \param name the name of file to be found
      *
      */
     char * findAudio(char *dir,char *name);
};


































#endif
