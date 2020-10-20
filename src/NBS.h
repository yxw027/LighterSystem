#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT  4444

#define TRUE             1
#define FALSE            0


double solMB[3];
double errMB[3];

void startRTK ();