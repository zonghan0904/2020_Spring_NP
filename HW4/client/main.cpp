# include "net_setting.h"
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <stdio.h>

int     errno;

#define QLEN              32    /* maximum connection queue length      */
#define BUFSIZE         4096
#define LINELEN         128

char    rbuf[BUFSIZE];         /* buffer for one line of text  */
char	wbuf[BUFSIZE];
char	buf[BUFSIZE];
int     s, n;                   /* socket descriptor, read count*/

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

/* thread for reading */
void* child(void *data){
    while(n = recv(s, rbuf, sizeof(rbuf), 0)){
	rbuf[n] = 0;
        fputs(rbuf, stdout);
	memset(rbuf, 0, sizeof(rbuf));
    }
    exit(0);
}

/*------------------------------------------------------------------------
 * main - Concurrent TCP server for ECHO service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {
    char    *host;    /* host to use if none supplied */
    char    *service;      /* default service name         */

    if (argc !=3) {
        fprintf(stderr, "usage: %s host port\n", argv[0]);
        exit(1);
    }

    service = argv[2];
    host = argv[1];

    s = connectTCP(host, service);

    memset(rbuf, 0, sizeof(rbuf));
    memset(wbuf, 0, sizeof(wbuf));

    pthread_t t;
    pthread_create(&t, NULL, child, NULL);

    /* thread for writing */
    memset(wbuf, 0, sizeof(wbuf));
    while (1){
	gets(wbuf);
	strcpy(buf, wbuf);
	strcat(wbuf, "\r\n");
	if (send(s, wbuf, strlen(wbuf), 0) < 0)
	    printf("error in sending message to server\n");
	usleep(500);
	memset(wbuf, 0, sizeof(wbuf));
    }

    exit(0);
}
