#include "wave.h"
#include "fir.h"
#include "model.h"

#define NUM_CHANNELS 1
#define SAMPLE_RATE 16000
#define BLOCK_SIZE 480

#if defined(SERVERTEST)
#define NBIN 3 
#define NBOUT 2
#endif

int16_t buf[100];

int main(int argc, char const *argv[])
{
#if defined(WAVETEST)
   void *waveIn;
   void *waveOut;

   printf("Start\n");

   waveIn = openWaveForReading("test.wav");
   if (waveIn == NULL)
   {
     printf("Err wave in\n");
   }

   waveOut = openWaveForWriting("output.wav",16000);
   if (waveOut == NULL)
   {
     closeWave(waveIn);
     printf("Err wave out\n");
   }

   
   int sample;
   for(int i=0;i<65535;i++)
   {
     sample=readWavSample(waveIn);

     writeWavSample(waveOut,sample);

   }

   closeWave(waveIn);
   closeWave(waveOut);
#endif 
  
#if defined (FIRTEST)
   FIR *f = newFIR("rir.csv");

   double y=filterSample(f,1.0);
   y=filterSample(f,0.0);
   printf("%.20f\n",y);

   closeFIR(f);
#endif

#if defined(SERVERTEST)
   #if defined(WINTEST)
   void *config=createConfig(1, "C:\\\\Users\\\\SOMEUSER\\\\AppData\\\\Local\\\\Programs\\\\Python\\\\Python310\\\\python.exe", "../testscripts/runclients.py",NBIN,NBOUT);
   #else
   //void *config=createConfig(1, "/usr/bin/python3.8", "../testscripts/runclients.py",NBIN,NBOUT);
   void *config=createConfig(1, "/opt/arm_vsi/VHT-Corstone-300","-V /home/ubuntu/VHTModelicaDemos/EchoCanceller/VSI/audio/python -f /home/ubuntu/VHTModelicaDemos/EchoCanceller/fvp_config.txt -a /home/ubuntu/VHTModelicaDemos/EchoCanceller/Objects/OrtaEchoCanceller.axf",NBIN,NBOUT);
   //void *config=createConfig(1, "/bin/sh", "launchapp.sh",NBIN,NBOUT);

   #endif
   

   if (config)
   {
      destroyConfig(config);
   }
#endif

    printf("END TEST\n");
  
    return 0;
}