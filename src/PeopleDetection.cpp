/*!
 * \file PeopleDetection.cpp
 *
 * 对PeopleDetection类的实现
 *
 * \author kiky.jiang@gmail.com
 *
 */

#include "../include/PeopleDetection.h"

int PeopleDetection::writer(char *data,size_t size,size_t nmemb,string *writedata)
{
    if(writedata==NULL)
    {
          cout<<"error copy html!~"<<endl;
          return 0;
    }
    writedata->append(data,size*nmemb);
    return size*nmemb;
}

bool PeopleDetection::APICall(const string &api_addr_base,
                              const map<string,string> &query_config,
                              Json::Value *response){
        CURL *curl;
	curl = curl_easy_init();
	if (!curl) {
		cerr << "CURL init error!" << endl;
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        curl_easy_setopt(curl, CURLOPT_USERAGENT,"Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US) " "AppleWebKit/534.7 (KHTML, like Gecko) Chrome/7.0.517.41 Safari/534.7");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_URL, api_addr_base.c_str());

	string post_fields = "";
	for (map<string, string>::const_iterator iter = query_config.begin();
			iter != query_config.end(); ++iter) {
		post_fields.append("&");
		post_fields.append(iter->first);
		post_fields.append("=");
		post_fields.append(iter->second);
	}

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());

	string photos_page;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &photos_page);

	try {
                CURLcode res=curl_easy_perform(curl);
                cout<<res<<endl;
		if ( res!= CURLE_OK) {
			cerr << "CURL calling error!" << endl;
			curl_easy_cleanup(curl);
			return false;
		}
	} catch (...) {
		cerr << "CURL calling error!" << endl;
		curl_easy_cleanup(curl);
		return false;
	}

	Json::Reader reader;
	if (!reader.parse(photos_page, *response)) {
		cout << "Failed to parse response." << endl;
		curl_easy_cleanup(curl);
		return false;
	}
	curl_easy_cleanup(curl);

	return true;
}

PeopleDetection::PeopleDetection(int _device)
{
    device=_device;
    capture=false;
}

void PeopleDetection::captureVideo()
{
    IplImage *img=NULL;
    CvCapture *cap=cvCaptureFromCAM(getDevice());
    while(true)
    {
       img=cvQueryFrame(cap);
       cvFlip(img,NULL,1);
       char key=cvWaitKey(50);
       //if(key==27) break;
       if(key=='c')
       {
          cvSaveImage("capture.jpg",img);
          cvXorS(img,cvScalarAll(255),img);
          cvShowImage("Camera",img);
          cvWaitKey(100);
          break;
       }
       cvShowImage("Camera",img);
     }
     cvReleaseCapture(&cap);
     return ;
}

enum Emotion PeopleDetection::getEmotion(bool &flag)
{
     Json::Value response;
     Emotion _emotion;
     const string api_addr_base="https://rekognition.com/func/api/?";
     map<string,string>  query_config;
     query_config["api_key"]="A6cqwvap4dPF5yHT";
     query_config["api_secret"]="KTvIUtKrTzFlezwE";
     query_config["jobs"]="face_recognize_nodetect_emotion";
     query_config["name_space"]="qrobot";
     query_config["user_id"]="fikon";
     query_config["num_return"]="1";

     ifstream t("capture.jpg");
     string buff_str((istreambuf_iterator<char>(t)),istreambuf_iterator<char>());
     
     rekognition_api::Base64Codec base64;
     string encoded;
     base64.Encode(buff_str,&encoded);
     query_config["base64"]=encoded;
     t.close();
     if(!APICall(api_addr_base,query_config,&response))
     {
         cerr<<"API call failure!!~"<<endl;
         flag=false;
         return null;
     }
     else
     {
         //cout<<response<<endl;
         Json::Value status=response["usage"];
         if(status["status"]!="Succeed.")
         {
             flag=false;
             cout<<status["status"]<<endl;
             return null;
         }
         else
         {
             Json::Value detection=response["face_detection"];
             int tmp=0;
             Json::Value emotion=detection[tmp]["emotion"];
             //cout<<"2"<<endl;
             string emotions[]={"happy","confused","calm","sad","angry","disgust","surprised"};
             string retEmotion="null";
             double emotionVal=-1.0;
             for(int i=0;i<7;i++)
             {
                 if(!emotion[emotions[i]].isNull())
                 {
                    if(emotion[emotions[i]].asDouble()>emotionVal)
                    {
                        emotionVal=emotion[emotions[i]].asDouble();
                        retEmotion=emotions[i];
                    }
                 }
             }
             if(retEmotion=="null")
             {
                 flag=false;
                 cout<<"capture the emotion failed!!~"<<endl;
                 return null;
             }
             else
             {
                 flag=true;
                 switch(retEmotion[0])
                 {
                 case 'h':
                       return happy;
                 case 'a':
                       return angry;
                 case 'd':
                       return disgust;
                 case 'c':
                       if(retEmotion[1]=='o')
                           return confused;
                       return calm;
                 case 's':
                       if(retEmotion[1]=='a')
                           return sad;
                       return surprised;
                 }
              }
           }
       }
}
             

















