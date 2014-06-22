#include "voiceTransform.h"

const char *login_configs = "appid = 53a38a05, work_dir =   .  ";
const char *param = "sub=iat,ssm=1,auf=audio/L16;rate=16000,aue=raw,ent=sms16k,rst=plain,rse=utf8,cfd=900,ptt=0;";

void getMusicName(const char *filename, char *output)
{
	int ret = 0;
	*output = '\0';

	ret = MSPLogin(NULL, NULL, login_configs);
	if(ret != MSP_SUCCESS)
	{
		printf("无法登录到讯飞语音.\n");
	}

	char rec_result[1024 * 4] = {0};
	const char *sessionID;
	char *pPCM = NULL;
	FILE *f_pcm = NULL;
	int lastAudio = 0;
	int audStat = 2;
	int epStatus = 0;
	int recStatus = 0;
	long pcmCount = 0;
	long pcmSize = 0;
	ret = 0;

	sessionID = QISRSessionBegin(NULL, param, &ret);
	if(ret != 0)
	{
		printf("QISRSessionBegin Failed, errorCode = %d\n", ret);
	}
	
	f_pcm = fopen(filename, "rb");
	if(f_pcm != NULL)
	{
		fseek(f_pcm, 0, SEEK_END);
		pcmSize = ftell(f_pcm);
		fseek(f_pcm, 0, SEEK_SET);
		pPCM = (char *)malloc(pcmSize);
		fread((void *)pPCM, pcmSize, 1, f_pcm);
		fclose(f_pcm);
		f_pcm = NULL;
	}
	while(true)
	{
		unsigned int len = 6400;
		
		if(pcmSize < 12800)
		{
			len = pcmSize;
			lastAudio = 1;
		}
		audStat = 2;
		if(pcmCount == 0)
			audStat = 1;
		if(0) {
			if(audStat == 1)
				audStat = 5;
			else
				audStat = 4;
		}
		if(len <= 0)
			break;
		ret = QISRAudioWrite(sessionID, (const void *)&pPCM[pcmCount], len, audStat, &epStatus, &recStatus);
		if(ret != 0)
			break;
		pcmCount += (long)len;
		pcmSize += (long)len;
		if(recStatus == 0)
		{
			const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &ret);
			if(ret != MSP_SUCCESS)
			{
				printf("QISRGetResult Failed, errorCode = %d\n", ret);
				break;
			}
			if(rslt != NULL)
				strcat(output, rslt);
			//	printf("%s\n", rslt);
		}
		if(epStatus == MSP_EP_AFTER_SPEECH)
			break;
		usleep(150000);
	}
	ret = QISRAudioWrite(sessionID, (const void *)NULL, 0, 4, &epStatus, &recStatus);
	if(ret != 0)
	{
		printf("QISRAudioWrite Failed, errorCode = %d\n", ret);
	}
	free(pPCM);
	pPCM = NULL;
	while(recStatus != 5 && ret == 0)
	{
		const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &ret);
		if(rslt != NULL)
			strcat(output, rslt);
		usleep(15000);
	}
	ret = QISRSessionEnd(sessionID, NULL);
	if(ret != MSP_SUCCESS)
	{
		printf("QISRSessionEnd Failed, errorCode = %d\n", ret);
	}

	usleep(100);
}
