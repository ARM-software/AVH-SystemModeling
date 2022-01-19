#ifndef _MODEL_H_
#define _MODEL_H_ 

#undef UNICODE

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WINDOWSVERSION

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__linux__)
#define LINUXVERSION
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#else
#   error "Unknown compiler"
#endif


#include <stdlib.h>

extern void *createConfig(int launchVHT, 
    const char *vhtCommand, 
    const char *vhtCommandLine,
    const int nbInputs,
    const int nbOutputs);
extern void destroyConfig(void * config);

//extern double gain(void *server,double x);
extern double getSample(void *vhtData,int i);
extern void writeSample(void *vhtData,int i,double x);

#endif