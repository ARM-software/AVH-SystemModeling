#include "wave.h"
#include <stdlib.h>
#include <math.h>

// http://soundfile.sapp.org/doc/WaveFormat/
typedef struct WAVEHeader {
  uint32_t ChunkID;
  uint32_t ChunkSize;
  uint32_t Format;
  uint32_t Subchunk1ID;
  uint32_t Subchunk1Size;
  uint16_t AudioFormat;
  uint16_t NumChannels;
  uint32_t SampleRate;
  uint32_t ByteRate;
  uint16_t BlockAlign;
  uint16_t BitsPerSample;
  uint32_t Subchunk2ID;
  uint32_t Subchunk2Size;
} WAVEHeader;

// Nb of bytes for an INT16 sample
#define SAMPLEFORMAT 2
#define NBCHANNELS 1

#define WAVE_BUF_NBSAMPLES 100

typedef struct {
   FILE *f;
   /* Either bytes remining to read or
   bytes which have been written */
   unsigned long remaining;
   int mode;
   uint32_t sampleRate;
   int16_t buf[WAVE_BUF_NBSAMPLES];
   short posInBuf;
} WAVE;

long switchEndianism(long x)
{
    unsigned char a,b,c,d;
    a=(x >> 0) & 0x0FF;
    b=(x >> 8) & 0x0FF;
    c=(x >> 16) & 0x0FF;
    d=(x >> 24) & 0x0FF;
    return((a << 24) | (b << 16) | (c << 8) | d);
}



#define CHECK(v,x)                  \
{                                   \
    if (switchEndianism((v)) != (x))\
    {                               \
        closeWave(wave);            \
        free(wave);                 \
        return(NULL);               \
    }                               \
}

static size_t waveRead(void *ptr)
{
   WAVE *wave = (WAVE*)ptr;
   if (wave->remaining >= WAVE_BUF_NBSAMPLES)
   {
      size_t ret = fread((char*)wave->buf, SAMPLEFORMAT, WAVE_BUF_NBSAMPLES, wave->f);
      memset(wave->buf + ret, 0,(WAVE_BUF_NBSAMPLES - ret)*SAMPLEFORMAT);
      wave->remaining -= (unsigned long)ret;
      wave->posInBuf=0;
      return(ret);
   } 
   else if (wave->remaining > 0)
   {
      size_t ret = fread(wave->buf,  SAMPLEFORMAT, wave->remaining, wave->f);
      memset(wave->buf + ret, 0,(WAVE_BUF_NBSAMPLES - ret)*SAMPLEFORMAT);

      wave->remaining -= (unsigned long)ret;
      wave->posInBuf=0;
      return(ret);
   }
   else
   {
     memset(wave->buf,0,WAVE_BUF_NBSAMPLES * SAMPLEFORMAT);
     wave->posInBuf=0;
     return(0);
   }
}

static void waveWrite(void *ptr)
{
    WAVE *wave = (WAVE*)ptr;
    wave->remaining += WAVE_BUF_NBSAMPLES*SAMPLEFORMAT;
    fwrite(wave->buf, SAMPLEFORMAT, WAVE_BUF_NBSAMPLES, wave->f);
    wave->posInBuf=0;
    memset(wave->buf,0,WAVE_BUF_NBSAMPLES * SAMPLEFORMAT);
}

void closeWave(void *ptr)
{
    WAVE *wave = (WAVE*)ptr;
    if (wave->f != NULL)
    {     
        /* Write mode : we need to update the header before closing the file */
        if (wave->mode==1)
        {
            if (wave->posInBuf>0)
            {
                waveWrite(wave);
            }

            uint32_t nbBytes = wave->remaining * NBCHANNELS * SAMPLEFORMAT;

  
            fseek(wave->f, 4, SEEK_SET);
            uint32_t chunkSize_len = 36 + nbBytes;
            fwrite(&chunkSize_len, sizeof(uint32_t), 1, wave->f);

            fseek(wave->f, 40, SEEK_SET);
            fwrite(&nbBytes, sizeof(uint32_t), 1, wave->f);

        }
        fclose(wave->f); 
        wave->f = NULL;
    } 
    free(wave); 
}


void *openWaveForReading(char *path)
{
  WAVEHeader header;
  WAVE *wave;

  wave = (WAVE*)malloc(sizeof(WAVE));

  wave->f = fopen(path, "rb");
  if (wave->f==NULL)
  {
    free(wave);
    return(NULL);
  }
  wave->mode=0;
  wave->posInBuf=WAVE_BUF_NBSAMPLES;
  size_t ret = fread(&header, sizeof(WAVEHeader), 1, wave->f);
  
  CHECK(header.ChunkID,0x52494646); // "RIFF"
  CHECK(header.Format ,0x57415645);         // "WAVE"
  CHECK(header.Subchunk1ID ,0x666d7420);    // "fmt "

  CHECK(header.Subchunk2ID,0x64617461);    // "data"

  wave->remaining=header.Subchunk2Size;

  wave->sampleRate = header.SampleRate;

  return((void*)wave);
}



void *openWaveForWriting(char *path,uint32_t sampleRate)
{
  WAVEHeader header;
  WAVE *wave;

  wave = (WAVE*)malloc(sizeof(WAVE));

  wave->f = fopen(path, "wb");
  if (wave->f==NULL)
  {
    free(wave);
    return(NULL);
  }
  wave->mode=1;
  wave->posInBuf=0;

  header.ChunkID = switchEndianism(0x52494646); // "RIFF"
  header.ChunkSize = 0; // filled when the file is closed
  header.Format = switchEndianism(0x57415645); // "WAVE"
  header.Subchunk1ID = switchEndianism(0x666d7420); // "fmt "
  header.Subchunk1Size = 16; // PCM
  header.AudioFormat = 1; // 1 PCM, 3 IEEE float
  header.NumChannels = NBCHANNELS;
  header.SampleRate = sampleRate;
  header.ByteRate = sampleRate * NBCHANNELS * SAMPLEFORMAT;
  header.BlockAlign = NBCHANNELS * SAMPLEFORMAT;
  header.BitsPerSample = 8*SAMPLEFORMAT;
  header.Subchunk2ID = switchEndianism(0x64617461); // "data"
  header.Subchunk2Size = 0; // filled when the file is closed

  wave->sampleRate = sampleRate;

  // write WAV header
  fwrite(&header, sizeof(WAVEHeader), 1, wave->f);


  return((void*)wave);
}

int readWavSample(void* ptr)
{
    WAVE *wave = (WAVE*)ptr;
    if (wave->posInBuf==WAVE_BUF_NBSAMPLES)
    {
      waveRead(wave);
    }
    int out=wave->buf[wave->posInBuf];
    wave->posInBuf++;
    return(out);
}

void writeWavSample(void *ptr,int writeSample)
{
    WAVE *wave = (WAVE*)ptr;
    if (wave->posInBuf==WAVE_BUF_NBSAMPLES)
    {
        waveWrite(wave);
    }

    wave->buf[wave->posInBuf] = writeSample;
    wave->posInBuf++;
}

double readWavSampleF32(void *wave)
{
   int c = readWavSample(wave);
   return(c/32767.0);
}

void writeWavSampleF32(void *wave,double value)
{
   if (value >= 1.0)
   {
     value=1.0;
   }
   if (value <= -1.0)
   {
     value=-1.0;
   }
   int intVal=(int)floor(value*32767.0);
   writeWavSample(wave,intVal);
}