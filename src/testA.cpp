#include <iostream>
#include "../include/AudioController.h"

using namespace std;

int main(){
	AudioController * audioController = new AudioController( "hw:2,0", 512, 16000, 1 );
	audioController->pcmToWav("test.wav", 4);
}
