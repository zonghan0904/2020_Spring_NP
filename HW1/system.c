# include "system.h"

/* register / login state */
# define OFFLINE 0
# define ONLINE  1
# define USER_EXIST 0
# define USER_NON_EXIST 1
int state = OFFLINE;
int exist = USER_NON_EXIST;
char current_user[BUFSIZ];
char verify_passwd[BUFSIZ];

/* system message.*/
char *REGISTER = "register";
char *LOGIN = "login";
char *LOGOUT = "logout";
char *WHOAMI = "whoami";
char *EXIT = "exit";
char *USAGE_REGIST = "Usage: register <username> <email> <password>\n";
char *USAGE_LOGIN = "Usage: login <username> <password>\n";
char *REGIST_SUCCESS = "Register successfully.\n";
char *REGIST_FAIL = "Username is already used.\n";
char LOGIN_SUCCESS[BUFSIZ];
char *LOGIN_FAIL = "Login failed.\n";
char *LOGIN_FIRST = "Please login first.\n";
char LOGOUT_SUCCESS[BUFSIZ];
char *LOGOUT_FIRST = "Please logout first.\n";
char USER_NAME[BUFSIZ];


/* welcome message to users.*/
int greeting(int fd){
    char *msg = "********************************\n"
		"** Welcome to the BBS server. **\n"
		"********************************\n";

    if (send(fd, msg, strlen(msg), 0) <= 0){
	printf("could send greeting msg to %d client.\n", fd);
    }
}

/* parse the argument*/
int cmd_parser(char buf[], char delim[], char *cmd[]){
    int cmd_cnt = 0;
    char *ptr = strtok(buf, delim);
    while (ptr != NULL){
        cmd[cmd_cnt++] = ptr;
        ptr = strtok(NULL, delim);
    }
    return cmd_cnt;
}

/* register part's callback function, check whether the username exist.*/
int regist_callback(void *NotUsed, int argc, char **argv, char **azColName){
    NotUsed = 0;
    int i;

    for (i = 0; i < argc; i++) {
	if (argv[i]){
	    exist = USER_EXIST;
	}
    }

    return 0;
}

/* login part's callback function, will record the user's password in order to make a further check.*/
int login_callback(void *data, int argc, char **argv, char **azColName){
    int i;

    for (i = 0; i < argc; i++) {
	if (argv[i]){
	    /* get the password of the current user in order to verify. */
	    strcpy(verify_passwd, argv[i]);
	}
    }
    return 0;
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


    /* open the database to load/store users data. */
    sqlite3 *db;
    char *zErrmsg = 0;
    int rc;

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
	    /* if there is only one command, strlen will take \0 into account.*/
	    if (cmd_cnt == 1) cmd[0][strlen(cmd[0]) - 1] = 0;

	    /* FOR register. */
	    if (!strcmp(cmd[0], REGISTER)){
		/* TODO */
		if (cmd_cnt != 4){
		    send(fd, USAGE_REGIST, strlen(USAGE_REGIST), 0);
		}
		else{
		    char *username = cmd[1];
		    char *email = cmd[2];
		    char *passwd = cmd[3];
		    char sql[BUFSIZ];

		    sprintf(sql, "SELECT Password FROM USERS WHERE Username = '%s';", username);
		    rc = sqlite3_exec(db, (const char *)sql, regist_callback, 0, &zErrmsg);

		    if (rc != SQLITE_OK){
			fprintf(stderr, "Failed to select data\n");
			fprintf(stderr, "SQL error: %s\n", zErrmsg);

			sqlite3_free(zErrmsg);
			sqlite3_close(db);

			return 1;
		    }

		    if (exist == USER_EXIST){
			send(fd, REGIST_FAIL, strlen(REGIST_FAIL), 0);
		    }

		    if (exist == USER_NON_EXIST){
			sprintf(sql, "INSERT INTO USERS (Username, Email, Password) "\
		    	    	 "VALUES ('%s', '%s', '%s' );", username, email, passwd);
		    	rc = sqlite3_exec(db, (const char *)sql, 0, 0, &zErrmsg);

		    	if (rc != SQLITE_OK){
		    	    fprintf(stderr, "SQL error: %s\n", zErrmsg);

		    	    sqlite3_free(zErrmsg);
		    	    sqlite3_close(db);

		    	    return 1;
		    	}
		    	else{
		    	    send(fd, REGIST_SUCCESS, strlen(REGIST_SUCCESS), 0);
		    	}
		    }
		    exist = USER_NON_EXIST;
		}
	    }

	    /* FOR login.*/
	    if (!strcmp(cmd[0], LOGIN)){
		/* TODO */
		if (cmd_cnt != 3){
		    send(fd, USAGE_LOGIN, strlen(USAGE_LOGIN), 0);
		}
		else{
		    if (state == ONLINE){
			send(fd, LOGOUT_FIRST, strlen(LOGOUT_FIRST), 0);
		    }
		    else if (state == OFFLINE){
			char *username = cmd[1];
		    	char *passwd = cmd[2];
		    	char sql[BUFSIZ];

		    	sprintf(sql, "SELECT Password FROM USERS WHERE Username = '%s';", username);
		    	rc = sqlite3_exec(db, (const char *)sql, login_callback, (void *) passwd, &zErrmsg);

		    	if (rc != SQLITE_OK){
		    	    fprintf(stderr, "Failed to select data\n");
		    	    fprintf(stderr, "SQL error: %s\n", zErrmsg);

		    	    sqlite3_free(zErrmsg);
		    	    sqlite3_close(db);

		    	    return 1;
		    	}

			if (!strcmp(passwd, verify_passwd)){
			    sprintf(LOGIN_SUCCESS, "Welcome, %s.\n", username);
			    send(fd, LOGIN_SUCCESS, strlen(LOGIN_SUCCESS), 0);
			    state = ONLINE;
			    strcpy(current_user, username);
			}
			else{
			    send(fd, LOGIN_FAIL, strlen(LOGIN_FAIL), 0);
			}
		    }
		}
	    }

	    /* FOR logout.*/
	    if (!strcmp(cmd[0], LOGOUT)){
		/* TODO */
		if (state == ONLINE){
		    sprintf(LOGOUT_SUCCESS, "Bye, %s.\n", current_user);
		    send(fd, LOGOUT_SUCCESS, strlen(LOGOUT_SUCCESS), 0);
		    state = OFFLINE;
		    current_user[0] = 0;
		}
		else if (state == OFFLINE){
		    send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		}
	    }

	    /* FOR whoami.*/
	    if (!strcmp(cmd[0], WHOAMI)){
		/* TODO */
		if (state == ONLINE){
		    sprintf(USER_NAME, "%s\n", current_user);
		    send(fd, USER_NAME, strlen(USER_NAME), 0);
		}
		else if (state == OFFLINE){
		    send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		}
	    }

	    /* FOR exit.*/
	    if (!strcmp(cmd[0], EXIT)){
		/* TODO */
		sqlite3_close(db);
		close(fd);
    		return 0;
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
