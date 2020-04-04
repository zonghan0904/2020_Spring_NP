# include "system.h"

# define OFFLINE 0
# define ONLINE  1

/* system message.*/
char *REGISTER = "register";
char *LOGIN = "login";
char *LOGOUT = "logout";
char *WHOAMI = "whoami";
char *EXIT = "exit";
char *USAGE_REGIST = "Usage: register <username> <email> <password>\n";
char *USAGE_LOGIN = "Usage: login <username> <password>\n";
char *LOGIN_FAIL = "Login failed.\n";
char *LOGIN_FIRST = "Please login first.\n";
char *LOGOUT_FIRST = "Please logout first.\n";


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
    int state = OFFLINE;
    char buf[BUFSIZ];
    char *prompt = "% ";
    char delim[] = " ";
    int cmd_cnt = 0;
    int bytes;
    char *cmd[BUFSIZ];


    /* open the database to load/store users data. */
    sqlite3 *db;
    char *zErrmsg = 0;
    int rc;
    char *sql;

    rc = sqlite3_open("users.db", &db);
    if(rc){
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       return(0);
    }
    else{
       fprintf(stderr, "Opened database successfully\n");
    }


    /* prompt the user to input the command.*/
    if (send(fd, prompt, strlen(prompt), 0) <= 0){
	printf("could send prompt to %d client.\n", fd);
    }


    while (bytes = read(fd, buf, sizeof buf)){
	/* check the whether the input data is received.*/
        if (bytes < 0){
            errexit("echo read: %s\n", strerror(bytes));
	}


	/* correct the string form and parse the command to analyze what to do.*/
	buf[bytes-1] = 0;
	cmd_cnt = cmd_parser(buf, delim, cmd);


	/* according to different command, take corresponding actions.*/
	if (cmd_cnt){
	    /* FOR register. */
	    if (!strncmp(cmd[0], REGISTER, strlen(REGISTER))){
		/* TODO */
		if (cmd_cnt != 4){
		    send(fd, USAGE_REGIST, strlen(USAGE_REGIST), 0);
		}
		else{

		}
	    }

	    /* FOR login.*/
	    if (!strncmp(cmd[0], LOGIN, strlen(LOGIN))){
		/* TODO */
		if (cmd_cnt != 3){
		    send(fd, USAGE_LOGIN, strlen(USAGE_LOGIN), 0);
		}
		else{
		    if (state == ONLINE){
			send(fd, LOGOUT_FIRST, strlen(LOGOUT_FIRST), 0);
		    }
		    else if (state == OFFLINE){

		    }
		}
	    }

	    /* FOR logout.*/
	    if (!strncmp(cmd[0], LOGOUT, strlen(LOGOUT))){
		/* TODO */
		if (state == ONLINE){

		}
		else if (state == OFFLINE){
		    send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		}
	    }

	    /* FOR whoami.*/
	    if (!strncmp(cmd[0], WHOAMI, strlen(WHOAMI))){
		/* TODO */
		if (state == ONLINE){

		}
		else if (state == OFFLINE){
		    send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		}
	    }

	    /* FOR exit.*/
	    if (!strncmp(cmd[0], EXIT, strlen(EXIT))){
		/* TODO */
		break;
	    }
	}


	/* prompt the user again.*/
	if (send(fd, prompt, strlen(prompt), 0) <= 0){
    	    printf("could send prompt to %d client.\n", fd);
    	}
    }

    /* close the database.*/
    sqlite3_close(db);
    return 0;
}
