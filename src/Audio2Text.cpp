#include "../include/Audio2Text.h"

//!constructor
Audio2Text::Audio2Text(char *_configs,char *_param)
{
   login_configs=_configs;
   param=_param;
}

void Audio2Text::getText(const char *fileName, char * text)
{
    int ret=0;
    *text='\0';
    
    ret=MSPLogin(NULL,NULL,getConfigs());
    if(ret!=MSP_SUCCESS)
    {
       cout<<"cannot log in XunFei!!~"<<endl;
    }
    char rec_result[1024*4]={0};
    const char *sessionID;
    char *p_pcm=NULL;
    FILE *f_pcm=NULL;
    int lastAudio=0;
    int audStat=2;
    int epStatus=0;
    int recStatus=0;
    int pcmCount=0;
    int pcmSize=0;
    ret=0;
    sessionID=QISRSessionBegin(NULL,getParam(),&ret);
    if(ret!=0)
    {
       cout<<"QISRSessionBegin failed!!~"<<endl;
    }
    f_pcm=fopen(fileName,"rb");
    if(f_pcm!=NULL)
    {
       fseek(f_pcm,0,SEEK_END);
       pcmSize=ftell(f_pcm);
       fseek(f_pcm,0,SEEK_SET);
       p_pcm=(char *)malloc(pcmSize);
       fread((void *)p_pcm,pcmSize,1,f_pcm);
       fclose(f_pcm);
       f_pcm=NULL;
    }
    while(true)
    {
       unsigned int len=6400;
       if(pcmSize<12800)
       {
          len=pcmSize;
          lastAudio=1;
       }
       audStat=2;
       if(pcmCount==0)
       {
          audStat=1;
       }
       if(0)
       {
          if(audStat==1)
          {
              audStat=5;
          }
          else
          {
  	      audStat=4;
          }
       }
       if(len<=0)
       {
          break;
       }
       ret=QISRAudioWrite(sessionID,(const void *)&p_pcm[pcmCount],len,audStat,&epStatus,&recStatus);
       if(ret!=0)
       {
          break;
       }
       pcmCount+=(long)len;
       pcmSize+=(long)len;
       if(recStatus==0)
       {
           const char *rslt=QISRGetResult(sessionID,&recStatus,0,&ret);
           if(ret!=MSP_SUCCESS)
           {
              cout<<"QISTGetResult failed!!~"<<endl;
              break;
           }
           if(rslt!=NULL)
               strcat(text,rslt);
       }
       if(epStatus==MSP_EP_AFTER_SPEECH)
           break;
       usleep(150000);
   }
   ret=QISRAudioWrite(sessionID,(const void *)NULL,0,4,&epStatus,&recStatus);
   if(ret!=0)
   {
       cout<<"QISRAudioWrite failed !!~"<<endl;
   }
   free(p_pcm);
   p_pcm=NULL;
   while(recStatus!=5&ret==0)
   {
       const char *rslt=QISRGetResult(sessionID,&recStatus,0,&ret);
       if(rslt!=NULL)
          strcat(text,rslt);
       usleep(15000);
   }
   ret=QISRSessionEnd(sessionID,NULL);
   if(ret!=MSP_SUCCESS)
       cout<<"session end failed!!~"<<endl;
   usleep(100);
}

char * Audio2Text::findAudio(char *dir,char *name)
{
   DIR *p_dir;
   char buf[1024];
   strcpy(buf,dir);
   strcat(buf,"/");
   struct dirent *p_dirent;
   cout<<"f"<<endl;
   if((p_dir=opendir(dir))==NULL)
   {
       cout<<"cannot open the directory!!~"<<endl;
       return "傻逼";
   }
   cout<<"open"<<endl;
   while((p_dirent=readdir(p_dir)))
   {
       cout<<p_dirent->d_name<<endl;
       if(p_dirent->d_type==8)
       {
          if(compare(name,p_dirent->d_name))
               return strcat(buf,p_dirent->d_name);
       }
   }
   return "傻逼";
}










































