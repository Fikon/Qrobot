/*!
 * \file Text2Audio.h
 *
 * \brief this file mainly define the class Text2Audio
 *
 * that is a class which convert the text to a audio file
 * whose format is: wav,16000hz,mono,16-bit.and this file also define 
 * a wav header for pcm stream and the default wav header
 * 
 * \author kiky.jiang@gmail.com
 *
 */

#ifndef TEXT_2_AUDIO_H
#define TEXT_2_AUDIO_H

#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef int SR_DWORD;
typedef short int SR_WORD;

using namespace std;

//the wav header for the pcm stream
struct wave_pcm_hdr

{

	char            riff[4];                       

	SR_DWORD        size_8;                        

	char            wave[4];                       

	char            fmt[4];                     

	SR_DWORD        dwFmtSize;                      



	SR_WORD         format_tag;             

	SR_WORD         channels;                    

	SR_DWORD        samples_per_sec;        

	SR_DWORD        avg_bytes_per_sec;     

	SR_WORD         block_align;            

	SR_WORD         bits_per_sample;         



	char            data[4];                      

	SR_DWORD        data_size;          

} ;

//the default wav header for the pcm stream
const struct wave_pcm_hdr default_pcmwavhdr = 

{

	{ 'R', 'I', 'F', 'F' },

	0,

	{'W', 'A', 'V', 'E'},

	{'f', 'm', 't', ' '},

	16,

	1,

	1,

	16000,

	32000,

	2,

	16,

	{'d', 'a', 't', 'a'},

	0  

};


class Text2Audio
{
private:
   //!developer's login information
   char * login_config;

   //!some parameters for converting
   char * params;

public:
   //!constructor 
   /*!
    * initial the login_config and params
    *
    */
   Text2Audio(); 
  
   virtual ~Text2Audio()
   {
      MSPLogout();
   }
 
   //!the method for converting text to audio file
   int getAudio(const char * text,const char * _fileName);


};

#endif
