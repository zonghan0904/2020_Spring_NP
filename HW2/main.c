# include "net_setting.h"
# include "system.h"

int     errno;


#define QLEN              32    /* maximum connection queue length      */
#define BUFSIZE         4096


/*------------------------------------------------------------------------
 * main - Concurrent TCP server for ECHO service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {
    char    *service;      /* service name or port number  */
    struct  sockaddr_in fsin;       /* the address of a client      */
    unsigned int    alen;           /* length of client's address   */
    int     msock;                  /* master server socket         */
    int     ssock;                  /* slave server socket          */

    if (argc !=2) errexit("usage: %s port\n", argv[0]);

    service = argv[1];

    msock = passiveTCP(service, QLEN);

    (void) signal(SIGCHLD, reaper);

    while (1) {
        alen = sizeof(fsin);
        ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
        if (ssock < 0) {
            if (errno == EINTR)
                continue;
            errexit("accept: %s\n", strerror(ssock));
        }
        printf("Accept connection %d from %s:%d\n", ssock, inet_ntoa(fsin.sin_addr), (int)ntohs(fsin.sin_port));
        switch (fork()){
    	    case 0:
    		(void) close(msock);
		greeting(ssock);
    		TCPechod(ssock);
    		close(ssock);
    		exit(0);
    	    default:
    		close(ssock);
    		break;
    	    case -1:
    		errexit("fork: %s\n", strerror(errno));
    	}
    }
}

