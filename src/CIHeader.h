#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_BUFFER 2048

double solMB[3];
double errMB[3];
int keyRTK;

typedef struct {
    char *buffer[MAX_BUFFER];
    int head, tail;
    int full, empty;
    pthread_mutex_t *mutex;
    pthread_cond_t *notFull, *notEmpty;
} queue;

typedef struct {
    fd_set serverReadFds;
    int socketFd;
    int clientSockets[MAX_BUFFER];
    int numClients;
    pthread_mutex_t *clientListMutex;
    queue *queue;
} chatDataVars;

typedef struct {
    chatDataVars *data;
    int clientSocketFd;
} clientHandlerVars;


void startChat(int socketFd);
void buildMessage(char *result, char *name, char *msg);
void bindSocket(struct sockaddr_in *serverAddr, int socketFd, long port);
void removeClient(chatDataVars *data, int clientSocketFd);
int startMB();

void *newClientHandler(void *data);
void *clientHandler(void *chv);
void *messageHandler(void *data);

void queueDestroy(queue *q);
queue* queueInit(void);
void queuePush(queue *q, char* msg);
char* queuePop(queue *q);