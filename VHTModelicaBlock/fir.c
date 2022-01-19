#include "fir.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STRBUFSIZE 256
static char temp[STRBUFSIZE];

static int getLine(FILE *f)
{
    int done=0;
    int eofOccured=0;
    int nb=0;
    while((!done) && (nb < STRBUFSIZE-1))
    {
        int c = fgetc(f);
        /* Any of those characters is an end of line */
        if ((c=='\n') || (c=='\r') || (c==EOF))
        {
            if (c==EOF)
            {
                eofOccured=1;
                done=1;
            }
            /* If end of line if a combination or \r and \n then we
            filter those characters at the beginning of  aline */
            else if (nb!=0)
            {
               done=1;
            }
        }
        else
        {
           temp[nb]=c;
           nb++;
        }
    }

    temp[nb]=0;
    if (eofOccured)
    {
        return(-1);
    }
    return(1);
}

void closeFIR(FIR *fir)
{
    free(fir->coefs);
    free(fir->state);
    free(fir);
}

FIR *newFIR(char *csvPath)
{
    int nbLines=0;
    int done=0;
    FILE *f=fopen(csvPath,"r");
    while(!done)
    {
        int res=getLine(f);
        if (res!=1)
        {
            done=1;
        }
        else
        {
            nbLines++;
        }
    }

    fclose(f);
    
    FIR *fir=malloc(sizeof(FIR));
    fir->nbTaps=nbLines;
    fir->pos=0;
    fir->coefs=malloc(sizeof(double)*nbLines);
    fir->state=malloc(sizeof(double)*nbLines);

    memset(fir->state,0,sizeof(double)*nbLines);

    f=fopen(csvPath,"r");
    int i=0;
    done=0;
    while(!done)
    {
        int res=getLine(f);
        if (res==1)
        {
           fir->coefs[i]=atof(temp);
           i++;
        }
        else
        {
            done=1;
        }
    }
    if (i<nbLines)
    {
        closeFIR(fir);
        return(NULL);
    }

    return(fir);
}



double filterSample(FIR *fir,double sample)
{
   memmove(fir->state+1,fir->state,sizeof(double)*(fir->nbTaps-1));
   fir->state[0] = sample;
   double y=0;

   for(int i=0;i<fir->nbTaps;i++)
   {
     y += fir->coefs[i] * fir->state[i];
   }

   return(y);
}