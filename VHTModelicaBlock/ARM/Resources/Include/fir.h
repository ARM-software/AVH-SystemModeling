#ifndef _FIR_H_ 
#define _FIR_H_

#ifdef   __cplusplus
extern "C"
{
#endif

typedef struct {
double *coefs;
double *state;
int pos;
int nbTaps;
} FIR;

extern FIR *newFIR(char *csvPath);
extern void closeFIR(FIR *fir);
extern double filterSample(FIR *fir,double sample);

#ifdef   __cplusplus
}
#endif

#endif
