# include "system.h"


int greeting(int fd){
    char *msg = "********************************\n"
		"** Welcome to the BBS server. **\n"
		"********************************\n";

    if (send(fd, msg, strlen(msg), 0) <= 0){
	printf("could send greeting msg to %d client.\n", fd);
    }
}

/*------------------------------------------------------------------------
 * TCPechod - echo data until end of file
 *------------------------------------------------------------------------
 */
int TCPechod(int fd){
    char buf[BUFSIZ];
    char *prompt = "% ";
    char delim[] = " ";
    int cmd_cnt = 0;
    int bytes;

    if (send(fd, prompt, strlen(prompt), 0) <= 0){
	printf("could send prompt to %d client.\n", fd);
    }

    while (bytes = read(fd, buf, sizeof buf)){
        if (bytes < 0)
            errexit("echo read: %s\n", strerror(bytes));


        if (errno = write(fd, buf, bytes) < 0)
            errexit("echo write: %s\n", strerror(errno));


	if (send(fd, prompt, strlen(prompt), 0) <= 0){
    	    printf("could send prompt to %d client.\n", fd);
    	}
    }
    return 0;
}
