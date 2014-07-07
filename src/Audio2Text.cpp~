#include "../include/Audio2Text.h"
#include "../include/AudioController.h"

//!一个辅助函数，计算匹配相似度
static int similarRate(const char *str1,const char *str2)
{
    int min=strlen(str1)>=strlen(str2)?strlen(str2):strlen(str1);
    for(int i=0;i<min;i++)
       if(str1[i]!=str2[i])
           return i;
    return min;
}

//!constructor
Audio2Text::Audio2Text(char *_configs,char *_param)
{
   login_configs=_configs;
   param=_param;
   token="24.691f5d653750a42f11c89608fbf17cb3.2592000.1406565669.282335-3021490";
   cuid="13580445201";
   api_key="mS7270GzUlhdGkXhxr9M5En8";
   secret_key="h0rVSMHQPkoLjBexBtbOrCiT7Sx8lAQ6";
}

size_t Audio2Text::writeMethod(void *ptr,size_t size,size_t nmemb,char **result)
{
   size_t result_len=size*nmemb;
   *result=(char *)realloc(*result,result_len+1);
   if(*result==NULL)
   {
      cout<<"realloc failed!!~"<<endl;
      return 1;
   }
   memcpy(*result,ptr,result_len);
   (*result)[result_len]='\0';
   printf("%s\n",*result);
   return result_len;
}

void Audio2Text::getTextFromBD(char *fileName,char *format,int rate,char * & text)
{	
   FILE * fp=NULL;
   fp=fopen(fileName,"r");
   if(fp==NULL)
   {
       cout<<"open the file:"<<fileName<<" failed!!~"<<endl;
   }
   fseek(fp,0,SEEK_END);
   int content_len=ftell(fp);
   fseek(fp,0,SEEK_SET);
   char *audioData=(char *)malloc(content_len);
   fread(audioData,content_len,sizeof(char),fp);
   char host[MAX_BUFFER_SIZE];
   snprintf(host,sizeof(host),"http://vop.baidu.com/server_api");
   FILE *fpp=NULL;
   char tmp[MAX_BUFFER_SIZE];
   memset(tmp,0,sizeof(tmp));
   char body[MAX_BODY_SIZE];
   memset(body,0,sizeof(body));
   string decode_data=base64_encode((const unsigned char *)audioData,content_len);
   if(decode_data.length()==0)
   {
      cout<<"base64 encoded data is empty!!~"<<endl;
   }
   Json::Value buffer;
   Json::FastWriter trans;
   buffer["format"]=format;
   buffer["rate"]=rate;
   buffer["channel"]=1;
   buffer["token"]=getToken();
   buffer["cuid"]=getCuid();
   buffer["speech"]=decode_data;
   buffer["len"]=content_len;
   content_len=trans.write(buffer).length();
   memcpy(body,trans.write(buffer).c_str(),content_len);
   CURL *curl;
   CURLcode res;
   char *resultBuf=NULL;
   struct curl_slist *header_list=NULL;
   snprintf(tmp,sizeof(tmp),"%s","Content-Type: application/json; charset=utf-8");
   header_list=curl_slist_append(header_list,tmp);
   snprintf(tmp,sizeof(tmp),"Content-Length: %d", content_len);
   header_list=curl_slist_append(header_list,tmp);
   curl=curl_easy_init();
   curl_easy_setopt(curl,CURLOPT_URL,host);
   curl_easy_setopt(curl,CURLOPT_POST,1);
   curl_easy_setopt(curl,CURLOPT_TIMEOUT,30);
   curl_easy_setopt(curl,CURLOPT_HTTPHEADER,header_list);
   curl_easy_setopt(curl,CURLOPT_POSTFIELDS,body);
   curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,content_len);
   curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writeMethod);
   curl_easy_setopt(curl,CURLOPT_WRITEDATA,&resultBuf);
   res=curl_easy_perform(curl);
   if(res!=CURLE_OK)
   {
      cout<<"perform curl failed!!~ error code: "<<res<<endl;
   }
   curl_slist_free_all(header_list);
   curl_easy_cleanup(curl);
   Json::Reader reader;
   Json::Value root;
   Json::Value results;
   string err_msg;
   string retval="";
   if(!reader.parse(resultBuf,root,false))
   {
       cout<<"parse the json failed!!~"<<endl;
   }
   else
   {
       cout<<resultBuf<<endl;
       err_msg=root["err_msg"].asString();
       if(err_msg=="success.")
       {
           results=root["result"];
           retval=results[0].asString();
       }
       else
       {
           cout<<"transform failed!!~"<<endl;
       }
   }
   fclose(fp);
   free(audioData);
   cout<<text<<" "<<retval<<endl;
   strcpy(text,retval.c_str());
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

char * Audio2Text::findAudio(char *dir,char *fileName)
{
    DIR *p_dir;
    char buf[1024];
    strcpy(buf,dir);
    strcat(buf,"/");
    struct dirent *p_dirent;
    int max_rate=-1;
    char tmp[1024];
    if((p_dir=opendir(dir))==NULL)
    {
        cout<<"failed to open the directory: "<<dir<<endl;
        return "no";
    }
    while((p_dirent=readdir(p_dir)))
    {
        if(p_dirent->d_type==8)
        {
            int _tmp=similarRate(p_dirent->d_name,fileName);
            if(_tmp>max_rate)
            {
                max_rate=_tmp;
                strcpy(tmp,p_dirent->d_name);
            }
        }
    }
    strcat(buf,tmp);
    printf(">>>>>>>>>>%s\n", buf);
    return buf;
}

























