#include <cstdio>
#include "../include/MusicPlayer.h"

int main(){
	MusicPlayer * musicPlayer = new MusicPlayer();
	musicPlayer->pcmBufferInit(16);
	musicPlayer->playMusic("./test3.mp3", "default");
	return 0;
}
