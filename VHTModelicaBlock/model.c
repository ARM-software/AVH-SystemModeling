#include "model.h"

#if !defined(TEST)
#include "ModelicaUtilities.h"
#else
#include <stdio.h>
#endif

#include <math.h>

#if defined(WINDOWSVERSION)
#pragma comment (lib, "Ws2_32.lib")
#endif

#if defined(LINUXVERSION)
extern char **environ;
#endif

#define DEFAULT_BUFLEN 2048
#define DEFAULT_PORT "50007"

#define VSIOUTPUT 0
#define VSIINPUT 1
#define NOVSI 2

#if defined(TEST)
  #define ModelicaFormatError printf
  #define ModelicaMessage printf
  #define ModelicaFormatMessage printf
  #define DEBUG printf
#else
  #define DEBUG(...) 
#endif

typedef struct  {
#if defined(WINDOWSVERSION)
  SOCKET s;
#endif
#if defined(LINUXVERSION)
  int s;
#endif
  int readPos;
  int writePos;
  char *data;
} VHTConnection;

typedef struct {
  VHTConnection **vhtOutputs;
  VHTConnection **vhtInputs;
#if defined(WINDOWSVERSION)
  PROCESS_INFORMATION p;
#endif
#if defined(LINUXVERSION)
  int p;
#endif
  int launchProcess;
  int nbInputs;
  int nbOutputs;
} VHT;

#define CHECKNULL(s,p) if ((p)==NULL)                       \
{                                                           \
    ModelicaFormatError("Memory allocation error : %s\n",s);\
}

static VHT vht;

void getNewDataFromServer(void *vht)
{
    VHTConnection *client = (VHTConnection *)vht;
    int iResult;
    //ModelicaFormatWarning("%03d:%03d:%03d:%03d", client->data[0],client->data[1],client->data[2],client->data[3]);

    if ((client->readPos>0) && (client->readPos <= client->writePos))
    {
         //ModelicaFormatWarning("memcpy r=%d w=%d", client->readPos,client->writePos);

        memcpy(client->data,client->data+client->readPos,client->writePos-client->readPos);
        client->writePos -= client->readPos;
        client->readPos=0;
        //ModelicaFormatWarning("%03d:%03d:%03d:%03d", client->data[0],client->data[1],client->data[2],client->data[3]);

    }
    //ModelicaFormatWarning("receive r=%d w=%d", client->readPos,client->writePos);

    // We read data from client only when enough free area in FIFO
    // and not enough data in the FIFO
    // If we try to read although there are already enough data in FIFO
    // it may block
    // Otherwise client is waiting
    if ((DEFAULT_BUFLEN - client->writePos > 2) && ((client->writePos - client->readPos) < 2))
    {
        int nbRead=0;
        iResult =0;
        /* At least one word16 is read */
        while (nbRead < 2)
        {
#if defined(WINDOWSVERSION)
            iResult = recv(client->s, client->data+client->writePos, DEFAULT_BUFLEN - client->writePos, 0);
#endif 
#if defined(LINUXVERSION)
            iResult = read(client->s, client->data+client->writePos, DEFAULT_BUFLEN - client->writePos);
#endif 
            if (iResult > 0) 
            {
                 client->writePos += iResult;
                 nbRead += iResult;
            }
        }
        //ModelicaFormatWarning("data received r=%d w=%d", client->readPos,client->writePos);
        //ModelicaFormatWarning("%03d:%03d:%03d:%03d", client->data[0],client->data[1],client->data[2],client->data[3]);

    }
    
}

void sendNewDataToServer(void *vht)
{
    VHTConnection *client = (VHTConnection *)vht;
    if (client->writePos - client->readPos >= 2)
    {
        int nb = client->writePos - client->readPos;
        int nbSent=0;
        while(nb > 0)
        {
#if defined(WINDOWSVERSION)
           int iSendResult = send( client->s, client->data + client->readPos + nbSent, nb, 0 );
           // We assume that iSendREsult == nb
           if (iSendResult == SOCKET_ERROR) 
           {
              ModelicaFormatError("send failed with error: %d\n", WSAGetLastError());
           }
#endif 
#if defined(LINUXVERSION)
           int iSendResult = write( client->s, client->data + client->readPos + nbSent, nb );
           // We assume that iSendREsult == nb
           if (iSendResult < 0) 
           {
              int errsv = errno;
              ModelicaFormatError("send failed with error: %d\n", errsv);
           }
#endif       
           else
           {
              nbSent += iSendResult;
              nb -= iSendResult;
           }
        }
        
        client->readPos=client->writePos; 
        if (client->readPos>0)
        {
           client->writePos=0;
           client->readPos=0;

        }
       
    }
}



int identifyClient(void *vhtClient)
{
    unsigned char a,b,id;
    VHTConnection *client = (VHTConnection *)vhtClient;
    getNewDataFromServer(vhtClient);
    a = client->data[client->readPos];
    b = client->data[client->readPos+1];
    id = a>>1;

    client->readPos += 2;
    if ((a&1) == VSIOUTPUT)
    {
        ModelicaFormatMessage("VHT Output connected ID = %d\n",id);
        vht.vhtOutputs[id] = client;
        return(VSIOUTPUT);
    }
    if ((a&1) == VSIINPUT)
    {
        ModelicaFormatMessage("VHT Input connected ID = %d\n",id);
        vht.vhtInputs[id] = client;
        return(VSIINPUT);
    }
    return(NOVSI);
}

void quitVHT(void * config)
{
    VHT *vht = (VHT *)config;
    if (vht->launchProcess)
    {
        ModelicaMessage("Stop VHT\n");
#if defined(WINDOWSVERSION)
        TerminateProcess(vht->p.hProcess,0);
 
        CloseHandle( vht->p.hProcess );
        CloseHandle( vht->p.hThread );
#endif
#if defined(LINUXVERSION)
       kill(vht->p,SIGKILL);
#endif
    }
}



void * createConfig(int launchVHT, 
    const char *vhtCommand, 
    const char *vhtCommandLine,
    const int nbInputs,
    const int nbOutputs)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;
#if defined(WINDOWSVERSION)
    SOCKET ListenSocket = INVALID_SOCKET;
#endif
#if defined(LINUXVERSION)
    int ListenSocket = -1;
    struct sockaddr_in serv_addr;
#endif
    vht.launchProcess = 0;
    vht.vhtInputs=(VHTConnection**)malloc(sizeof(VHTConnection*)*nbInputs);
    CHECKNULL("vht inputs",vht.vhtInputs);
    vht.vhtOutputs=(VHTConnection**)malloc(sizeof(VHTConnection*)*nbOutputs);
    CHECKNULL("vht outputs",vht.vhtOutputs);

    for(int i=0; i < nbInputs; i++)
    {
        vht.vhtInputs[i]=NULL;
    }

    vht.nbInputs=nbInputs;


    for(int i=0; i < nbOutputs; i++)
    {
        vht.vhtOutputs[i]=NULL;
    }
    vht.nbOutputs=nbOutputs;


#if defined(WINDOWSVERSION)
    WSADATA *wsaData;
    wsaData = malloc(sizeof(WSADATA));
    CHECKNULL("wsaData",wsaData);
#endif

    ModelicaMessage("Start VHT server\n");

    int iResult;

    // Initialize Winsock
#if defined(WINDOWSVERSION)
    iResult = WSAStartup(MAKEWORD(2,2), wsaData);

    if (iResult != 0) {
        ModelicaFormatError("WSAStartup failed with error: %d\n", iResult);
        return NULL;
    }

    ZeroMemory(&hints, sizeof(hints));

    
#endif

#if defined(LINUXVERSION)
    bzero((char *) &hints, sizeof(hints));
#endif 

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
#if defined(WINDOWSVERSION)
    if ( iResult != 0 ) {
        ModelicaFormatError("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return NULL;
    }
#endif
#if defined(LINUXVERSION)
    if ( iResult < 0 ) {
        ModelicaFormatError("getaddrinfo failed with error: %d\n", iResult);
        return NULL;
    }
#endif


    // Create a SOCKET for connecting to server
    #if defined(WINDOWSVERSION)
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        ModelicaFormatError("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return NULL;
    }
    #endif
    #if defined(LINUXVERSION)
    ListenSocket = socket(result->ai_family, result->ai_socktype | SOCK_CLOEXEC, result->ai_protocol);
     if (ListenSocket < 0) {
        int errsv = errno;
        ModelicaFormatError("socket failed with error: %d\n", errsv);
        freeaddrinfo(result);
        return NULL;
    }
    int enable = 1;
    setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    #endif

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    #if defined(WINDOWSVERSION)
    if (iResult == SOCKET_ERROR) {
        ModelicaFormatError("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return NULL;
    }
    #endif
    #if defined(LINUXVERSION)
    if (iResult < 0) {
        int errsv = errno;
        ModelicaFormatError("bind failed with error: %d\n", errsv);
        freeaddrinfo(result);
        close(ListenSocket);
        return NULL;
    }
    #endif

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    #if defined(WINDOWSVERSION)
    if (iResult == SOCKET_ERROR) {
        ModelicaFormatError("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return NULL;
    }
    #endif 
    #if defined(LINUXVERSION)
    if (iResult < 0) {
        int errsv = errno;
        ModelicaFormatError("listen failed with error: %d\n", errsv);
        close(ListenSocket);
        return NULL;
    }
    #endif


    if (launchVHT)
    {
        char commandLine[1000];
        vht.launchProcess=1;

        ModelicaMessage("Launch the VHT\n");

#if defined(WINDOWSVERSION)
        STARTUPINFO si;

        strcpy(commandLine,vhtCommand);
        strcat(commandLine," ");
        strcat(commandLine,vhtCommandLine);

        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &vht.p, sizeof(vht.p) );


        if( !CreateProcess( vhtCommand,   // No module name (use command line)
            commandLine,        // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory 
            &si,            // Pointer to STARTUPINFO structure
            &vht.p )           // Pointer to PROCESS_INFORMATION structure
        ) 
        {
            ModelicaFormatError( "CreateProcess failed (%d).\n", GetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return NULL;
        }
#endif 

#if defined(LINUXVERSION)
        #define MAXNBARGS 20
        #define MAXARGLEN 256
        pid_t pid;
        char *argv[MAXNBARGS];
        int status;
        char *token;


        strcpy(commandLine,vhtCommand);
        strcat(commandLine," ");
        strcat(commandLine,vhtCommandLine);

        token = strtok(commandLine, " ");
        argv[0] = token;
   
        int nb=1;
        while(( token != NULL ) && (nb < MAXNBARGS-1)) 
        {
           token = strtok(NULL, " ");
           if (token != NULL)
           {
              argv[nb]=token;
              nb++;
           }
        }
        argv[nb] = NULL;
        nb++;
        
        #if 0
        // To debug
        printf("nb=%d\n",nb);

        for(int i=0;i<nb-1;i++)
        {
           printf("#%s#\n",argv[i]);
        }
        printf("--\n");
        

        char **env=environ;
        char *p=*environ;
        while(p)
        {
            printf("%s\n",p);
            p=*env++;
        }
        #endif
        

        status = posix_spawn(&vht.p, vhtCommand, NULL, NULL, argv, environ);

        if (status !=0)
        {
            ModelicaFormatError( "CreateProcess failed (%d).\n", status);
            close(ListenSocket);
            return NULL;
        }
        else
        {
            ModelicaFormatMessage("VHT PID %jd\n",(long int)vht.p);
        }

#endif
    }
    else
    {
        ModelicaMessage("Wait for VHT to start\n");
    }

    /* Wait for connections */
    int remainingInputs=nbInputs;
    int remainingOutputs=nbOutputs;


    while((remainingInputs>0) || (remainingOutputs>0))
    {
        VHTConnection *client=(VHTConnection*)malloc(sizeof(VHTConnection));
        CHECKNULL("VHTConnection",client);

        client->data=(char*)malloc(sizeof(char)*DEFAULT_BUFLEN);
        CHECKNULL("Client data",client->data);
        #if defined(WINDOWSVERSION)
        client->s = INVALID_SOCKET;
        #endif 
        #if defined(LINUXVERSION)
        client->s = -1;
        #endif
        client->s = accept(ListenSocket, NULL, NULL);
        client->writePos=0;
        client->readPos=0;
        memset(client->data,0,DEFAULT_BUFLEN);

        #if defined(WINDOWSVERSION)
        if (client->s == INVALID_SOCKET) {
           ModelicaFormatError("accept failed with error: %d\n", WSAGetLastError());
           closesocket(ListenSocket);
           quitVHT((void*)&vht);
           WSACleanup();
           return NULL;
        }
        #endif 
        #if defined(LINUXVERSION)
        if (client->s <0) {
           int errsv = errno;
           ModelicaFormatError("accept failed with error: %d\n", errsv);
           close(ListenSocket);
           quitVHT((void*)&vht);
           return NULL;
        }
        #endif


        int res=identifyClient(client);
        if (res==VSIOUTPUT)
        {
            DEBUG("OUTPUT IDENTIFIED\n");
            remainingOutputs--;
        }
        if (res==VSIINPUT)
        {
            DEBUG("INPUT IDENTIFIED\n");
            remainingInputs--;
        }
        if (res==NOVSI)
        {
           ModelicaFormatError("Unknown client connected\n");
           #if defined(WINDOWSVERSION)
           closesocket(ListenSocket);
           #endif 

           #if defined(LINUXVERSION)
           close(ListenSocket);
           #endif 
           quitVHT((void*)&vht);
           #if defined(WINDOWSVERSION)
           WSACleanup();
           #endif
           return NULL;
        }
    }

    ModelicaMessage("All clients are connected\n");
    
    
    // No longer need server socket
    #if defined(WINDOWSVERSION)
    closesocket(ListenSocket);
    #endif 
    #if defined(LINUXVERSION)
    close(ListenSocket);
    #endif 

  
    return(&vht);
}



void destroyConfig(void * config)
{
    VHT *vht = (VHT *)config;
    //ModelicaMessage("Stop VHT server");
    quitVHT(config);
   


    if (vht->vhtOutputs)
    {
        for(int i=0; i<vht->nbOutputs;i++)
        {
             // shutdown the connection since we're done
            if (vht->vhtOutputs[i])
            {
                #if defined(WINDOWSVERSION)
                int iResult = shutdown(vht->vhtOutputs[i]->s, SD_BOTH);
                if (iResult == SOCKET_ERROR) {
                    ModelicaFormatError("shutdown failed with error: %d\n", WSAGetLastError());
                }
                closesocket(vht->vhtOutputs[i]->s);
                #endif 
                #if defined(LINUXVERSION)
                 int iResult = shutdown(vht->vhtOutputs[i]->s, SHUT_RDWR);
                 if (iResult < 0) {
                    int errsv = errno;
                    ModelicaFormatError("shutdown failed with error: %d\n", errsv);
                }
                close(vht->vhtOutputs[i]->s);
                #endif


                if (vht->vhtOutputs[i]->data)
                {
                   free(vht->vhtOutputs[i]->data);
                }
                
                free(vht->vhtOutputs[i]);
           }
        }
        free(vht->vhtOutputs);
    }

    if (vht->vhtInputs)
    {
        for(int i=0; i<vht->nbInputs;i++)
        {
             if (vht->vhtInputs[i])
             {
               #if defined(WINDOWSVERSION)
                int iResult = shutdown(vht->vhtInputs[i]->s, SD_BOTH);
                if (iResult == SOCKET_ERROR) {
                    ModelicaFormatError("shutdown failed with error: %d\n", WSAGetLastError());
                }
                closesocket(vht->vhtInputs[i]->s);
               #endif
               #if defined(LINUXVERSION)
                 int iResult = shutdown(vht->vhtInputs[i]->s, SHUT_RDWR);
                 if (iResult < 0) {
                    int errsv = errno;
                    ModelicaFormatError("shutdown failed with error: %d\n", errsv);
                }
                close(vht->vhtInputs[i]->s);
               #endif
        
              
              if (vht->vhtInputs[i]->data)
              {
                   free(vht->vhtInputs[i]->data);
              }
              free(vht->vhtInputs[i]);
            }
        }
    
        free(vht->vhtInputs);
    }

    #if defined(WINDOWSVERSION)
    WSACleanup();
    #endif 
}





double getSample(void *vhtData,int i)
{
    //ModelicaWarning("Read From VHT");
    VHT *vhtPtr = (VHT *)vhtData; 
    unsigned char a,b;
    int v;
    double res;
    //ModelicaFormatWarning("r=%d w=%d", vhtPtr->vhtOutputs[i]->readPos,vhtPtr->vhtOutputs[i]->writePos);

    getNewDataFromServer(vhtPtr->vhtOutputs[i]);
    //ModelicaFormatWarning("r=%d w=%d", vhtPtr->vhtOutputs[i]->readPos,vhtPtr->vhtOutputs[i]->writePos);

    a = vhtPtr->vhtOutputs[i]->data[vhtPtr->vhtOutputs[i]->readPos];
    b = vhtPtr->vhtOutputs[i]->data[vhtPtr->vhtOutputs[i]->readPos+1];

    //ModelicaFormatWarning("r %03d:%03d", vhtPtr->vhtOutputs[i]->data[vhtPtr->vhtOutputs[i]->readPos],
    //    vhtPtr->vhtOutputs[i]->data[vhtPtr->vhtOutputs[i]->readPos+1]);

    vhtPtr->vhtOutputs[i]->readPos += 2;

    v = (((int)b<<8) | a); 
    if (v > 32767)
    {
       v = v - 65536;
    }

    res = v / 32767.0;

    //ModelicaFormatWarning("%f a=%d b=%d r=%d w=%d", res,a,b,vhtPtr->vhtOutputs[i]->readPos,vhtPtr->vhtOutputs[i]->writePos);

    return(res);
}

void writeSample(void *vhtData,int i,double x)
{
    //ModelicaWarning("Write To VHT");
    VHT *vhtPtr = (VHT *)vhtData; 
    unsigned char a,b;

    /* Saturate value for Q15 representation */
    if (x < -1.0)
    {
        x=-1.0;
    }
    if (x > 0x7FFF/32678.0)
    {
        x=0x7FFF/32678.0;
    }

    int r = (int)floor(x*32767.0);
    a = r & 0x0FF;
    b = (r >> 8);

    //ModelicaFormatWarning("x=%f a=%d b=%d", x, a,b);
    vhtPtr->vhtInputs[i]->data[vhtPtr->vhtInputs[i]->writePos]=a;
    vhtPtr->vhtInputs[i]->data[vhtPtr->vhtInputs[i]->writePos+1]=b;
    vhtPtr->vhtInputs[i]->writePos +=2;

    //ModelicaFormatWarning("w %03d:%03d", vhtPtr->vhtInputs[i]->data[vhtPtr->vhtInputs[i]->writePos],
    //    vhtPtr->vhtInputs[i]->data[vhtPtr->vhtInputs[i]->writePos+1]);



    sendNewDataToServer(vhtPtr->vhtInputs[i]);

}

