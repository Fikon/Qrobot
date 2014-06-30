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
#include "curl/include/curl/curl.h"
#include "curl/include/curl/easy.h"
#include "json-cpp/include/json.h"
#include "base64.h"

#define MAX_BUFFER_SIZE 512
#define MAX_BODY_SIZE 1000000

using namespace std;

class Audio2Text
{
private:
     //!百度云语音识别需要用到的密钥
     char * token;
   
     //!语音识别时的一个唯一标识符
     char *cuid;

     //!api-key
     char *api_key;
    
     //!secret-key
     char *secret_key;

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
    
     //!write method
     static size_t writeMethod(void *ptr,size_t size,size_t nmemb,char **result);
     
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
     
     //!token's getter method
     char *getToken()
     {
         return token;
     }

     //!cuid's getter method
     char *getCuid()
     {
         return cuid;
     }
     
     //!api-key's getter method
     char *getApiKey()
     {
         return api_key;
     }

     //!secret-key's getter method
     char *getSecretKey()
     {
         return secret_key;
     }

     //!the main function that transform the audio to text
     /*!
      * \param fileName the input audio file's absolute name
      * \param text the plain text which tramsform form the audio file
      *
      */
     void getText(const char * fileName, char *text);
     
     //!another function that transform the audio to text
     /*!
      * \param fileName the input audio file's absolute path
      * \param format the audio's format
      * \param the audio's sample's rate
      * \param text the result
      *
      */
     void getTextFromBD(char *fileName,char *format,int rate,char *&text);   
    
     //!find the mucis file in a specific direcotry
     /*!
      * \param dir the directory to be found
      * \param name the name of file to be found
      *
      * \retval no cannot find the audio file
      * \retval absolute path the corect result
      *
      */
     char * findAudio(char *dir,char *name);
};


































#endif
