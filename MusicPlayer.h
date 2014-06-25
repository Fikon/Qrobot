# include <stdio.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/mman.h>


#include <string.h>
#include<fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <alsa/asoundlib.h>

# include <mad.h>

#define VOICE_NAME "hw:2,0"

static int decode(unsigned char const *, unsigned long);
int set_pcm();
snd_pcm_t*             handle=NULL;        //PCI设备句柄
snd_pcm_hw_params_t*   params=NULL;//硬件信息和PCM流配置

/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */
struct buffer {
  unsigned char const *start;
  unsigned long length;
};

int set_pcm();
static enum mad_flow input(void *data,struct mad_stream *stream);
static inline signed int scale(mad_fixed_t sample);

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */
static enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm);

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame);

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */
static
int decode(unsigned char const *start, unsigned long length);

/*入口函数*/
int playMusic(char * musicName);
