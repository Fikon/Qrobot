#include <cstdio>
#include "voiceTransform.h"

int main()
{
	char *str = "wav/test.wav";
	char res[1024 * 4];
	getMusicName(str, res);
	printf("<<<<<>>>>>%s\n", res);
}
