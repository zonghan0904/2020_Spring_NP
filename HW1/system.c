# include "system.h"

char *REGISTER = "register";
char *LOGIN = "login";
char *LOGOUT = "logout";
char *WHOAMI = "whoami";
char *EXIT = "exit";


int greeting(int fd){
    char *msg = "********************************\n"
		"** Welcome to the BBS server. **\n"
		"********************************\n";

    if (send(fd, msg, strlen(msg), 0) <= 0){
	printf("could send greeting msg to %d client.\n", fd);
    }
}

int cmd_parser(char buf[], char delim[], char *cmd[]){
    int cmd_cnt = 0;
    int i;
    char *ptr = strtok(buf, delim);
    while (ptr != NULL){
        cmd[cmd_cnt++] = ptr;
        ptr = strtok(NULL, delim);
    }
    return cmd_cnt;
}

void Regist(char *name, char *email, char *passwd){

}

void Login(char *name, char *passwd){

}

void Logout(){

}

void Whoami(){

}

void Exit(){

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
    char *cmd[BUFSIZ];

    if (send(fd, prompt, strlen(prompt), 0) <= 0){
	printf("could send prompt to %d client.\n", fd);
    }

    while (bytes = read(fd, buf, sizeof buf)){
        if (bytes < 0){
            errexit("echo read: %s\n", strerror(bytes));
	}

	buf[bytes-1] = 0;
	cmd_cnt = cmd_parser(buf, delim, cmd);

	if (cmd_cnt){
	    if (!strncmp(cmd[0], REGISTER, strlen(REGISTER))){
		/* TODO */
	    }
	    if (!strncmp(cmd[0], LOGIN, strlen(LOGIN))){
		/* TODO */
	    }
	    if (!strncmp(cmd[0], LOGOUT, strlen(LOGOUT))){
		/* TODO */
	    }
	    if (!strncmp(cmd[0], WHOAMI, strlen(WHOAMI))){
		/* TODO */
	    }
	    if (!strncmp(cmd[0], EXIT, strlen(EXIT))){
		/* TODO */
	    }
	}


	if (send(fd, prompt, strlen(prompt), 0) <= 0){
    	    printf("could send prompt to %d client.\n", fd);
    	}
    }
    return 0;
}
