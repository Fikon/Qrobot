/*
 * libmad - MPEG audio decoder library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: minimad.c,v 1.4 2004/01/23 09:41:32 rob Exp $
 */

# include "../include/MusicPlayer.h"

snd_pcm_t * MusicPlayer::handle = NULL;
short * MusicPlayer::pcmBuffer = NULL;
int MusicPlayer::sampleNum = 16;
int MusicPlayer::sampleCount = 0;

MusicPlayer::MusicPlayer(){
	handle = NULL;
	params = NULL;
}

MusicPlayer::~MusicPlayer(){

}

void MusicPlayer::pcmBufferInit( int samples ){
	sampleNum = samples;
	pcmBuffer = new short[samples*2];
	sampleCount = 0;
}

short * MusicPlayer::getPcmBuffer(){
	return pcmBuffer;
}

int MusicPlayer::playMusic(char * musicName, char * diviceName)
{
	this->diviceName = diviceName;

	struct stat stat;
	void *fdm;

	int fd;
	fd=open(musicName,O_RDWR);
	if(fd<0)
	{
		perror("open file failed:");
		return 1;
	}   

	if (fstat(fd, &stat) == -1 ||stat.st_size == 0)
	{
		printf("fstat failed:\n");
		return 2;
	}
	//printf("stat.st_size=%d\n",stat.st_size);

	fdm = mmap(0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (fdm == MAP_FAILED)
		return 3;


	if(set_pcm()!=0)                 //设置pcm 参数
	{
		printf("set_pcm fialed:\n");
		return 1;  
	}
	decode((const unsigned char*)fdm, stat.st_size);

	if (munmap(fdm, stat.st_size) == -1)
		return 4;

	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	return 0;
}


int MusicPlayer::set_pcm()
{
	int    rc;    
	int  dir=0;
	int rate = 44100;;                /* 采样频率 44.1KHz*/
	int format = SND_PCM_FORMAT_S16_LE; /*     量化位数 16      */
	int channels = 2;                 /*     声道数 2           */

	rc=snd_pcm_open(&handle, diviceName, SND_PCM_STREAM_PLAYBACK, 0);
	if(rc<0)
	{
		perror("\nopen PCM device failed:");
		exit(1);
	}
	snd_pcm_hw_params_alloca(&params); //分配params结构体

	rc=snd_pcm_hw_params_any(handle, params);//初始化params
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params_any:");
		exit(1);
	}
	rc=snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);                                 //初始化访问权限
	if(rc<0)
	{
		perror("\nsed_pcm_hw_set_access:");
		exit(1);

	}

	rc=snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);             //设置16位采样精度 
	if(rc<0)
	{
		perror("snd_pcm_hw_params_set_format failed:");
		exit(1);
	}

	rc=snd_pcm_hw_params_set_channels(handle, params, channels);  //设置声道,1表示单声>道，2表示立体声
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params_set_channels:");
		exit(1);
	}   

	rc=snd_pcm_hw_params_set_rate_near(handle, params, (unsigned int*)&rate, &dir);  //设置>频率
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params_set_rate_near:");
		exit(1);
	}  


	rc = snd_pcm_hw_params(handle, params);
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params: ");
		exit(1);
	}

	return 0;             
}



/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */

enum mad_flow MusicPlayer::input(void *data,
		struct mad_stream *stream)
{
	struct buffer *buffer = (struct buffer *)data;

	printf("this is input\n");
	if (!buffer->length)
		return MAD_FLOW_STOP;

	mad_stream_buffer(stream, buffer->start, buffer->length);

	buffer->length = 0;

	return MAD_FLOW_CONTINUE;
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */

signed int MusicPlayer::scale(mad_fixed_t sample)
{
	/* round */
	sample += (1L << (MAD_F_FRACBITS - 16));

	/* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */

enum mad_flow MusicPlayer::output(void *data,
		struct mad_header const *header,
		struct mad_pcm *pcm)
{
	unsigned int nchannels, nsamples,n;
	mad_fixed_t const *left_ch, *right_ch;

	/* pcm->samplerate contains the sampling frequency */

	nchannels = pcm->channels;
	n=nsamples  = pcm->length;
	left_ch   = pcm->samples[0];
	right_ch  = pcm->samples[1];

	//printf("%d %d\n", left_ch, right_ch);

	unsigned char Output[6912], *OutputPtr; 
	int fmt, wrote, speed, exact_rate, err, dir;


	//   printf("This is output\n");



	OutputPtr = Output; 

	while (nsamples--)
	{
		signed int sample;

		/* output sample(s) in 16-bit signed little-endian PCM */

		sample = scale(*left_ch++);
		pcmBuffer[sampleCount] = sample;
		sampleCount ++;
		if ( sampleCount == sampleNum ){
			sampleCount = 0;
		}
		//printf("%x\n", sample);

		*(OutputPtr++) = sample >> 0; 
		*(OutputPtr++) = sample >> 8; 
		if (nchannels == 2) 
		{ 
			sample = scale (*right_ch++); 
			*(OutputPtr++) = sample >> 0; 
			*(OutputPtr++) = sample >> 8; 
		} 

	}

	OutputPtr = Output; 
	snd_pcm_writei (handle, OutputPtr, n); 
	OutputPtr = Output;    

	return MAD_FLOW_CONTINUE;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */

enum mad_flow MusicPlayer::error(void *data,
		struct mad_stream *stream,
		struct mad_frame *frame)
{
	struct buffer *buffer = (struct buffer *)data;
	printf("this is mad_flow error\n");
	fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %u\n",
			stream->error, mad_stream_errorstr(stream),
			stream->this_frame - buffer->start);

	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */

	return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */

int MusicPlayer::decode(unsigned char const *start, unsigned long length)
{
	struct buffer buffer;
	struct mad_decoder decoder;
	int result;

	/* initialize our private message structure */

	buffer.start  = start;
	buffer.length = length;

	/* configure input, output, and error functions */

	mad_decoder_init(&decoder, &buffer,
			MusicPlayer::input, 0 /* header */, 0 /* filter */, MusicPlayer::output,
			MusicPlayer::error, 0 /* message */);

	/* start decoding */

	result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

	/* release the decoder */

	mad_decoder_finish(&decoder);

	return result;
}
