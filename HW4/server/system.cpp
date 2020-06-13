# include "system.h"


/* register / login state */
# define OFFLINE 0
# define ONLINE  1
# define USER_EXIST 0
# define USER_NON_EXIST 1
# define BOARD_EXIST 0
# define BOARD_NON_EXIST 1
int state = OFFLINE;
int exist = USER_NON_EXIST;
int board_exist = BOARD_NON_EXIST;
int post_exist = BOARD_NON_EXIST;
char current_user[BUFSIZ];
char current_board[BUFSIZ];
char verify_passwd[BUFSIZ];

/* command */
char *REGISTER = "register";/*{{{*/
char *LOGIN = "login";
char *LOGOUT = "logout";
char *WHOAMI = "whoami";
char *EXIT = "exit";
char *CREATE_BOARD = "create-board";
char *CREATE_POST = "create-post";
char *LIST_BOARD = "list-board";
char *LIST_POST = "list-post";
char *DELETE_POST = "delete-post";
char *UPDATE_POST = "update-post";
char *COMMENT = "comment";
char *READ = "read";/*}}}*/

/* system message.*/
char *USAGE_REGIST = "Usage: register <username> <email> <password>\n";/*{{{*/
char *USAGE_LOGIN = "Usage: login <username> <password>\n";
char *USAGE_CREATE_BOARD = "Usage: create-board <name>\n";
char *USAGE_CREATE_POST = "Usage: create-post <board-name> --title <title> --content <content>\n";
char *USAGE_LIST_BOARD = "Usage: list-board ##<key>\n";
char *USAGE_LIST_POST = "Usage: list-post <board-name> ##<key>\n";
char *USAGE_READ = "Usage: read <post-id>\n";
char *USAGE_DELETE_POST = "delete-post <post-id>\n";
char *USAGE_UPDATE_POST = "update-post <post-id> --title/content <new>\n";
char *USAGE_COMMENT = "comment <post-id> <comment>\n";
char *REGIST_SUCCESS = "Register successfully.\n";
char *REGIST_FAIL = "Username is already used.\n";
char LOGIN_SUCCESS[BUFSIZ];
char *LOGIN_FAIL = "Login failed.\n";
char *LOGIN_FIRST = "Please login first.\n";
char LOGOUT_SUCCESS[BUFSIZ];
char *LOGOUT_FIRST = "Please logout first.\n";
char USER_NAME[BUFSIZ];
char *CREATE_BOARD_FAIL = "Board already exist.\n";
char *CREATE_BOARD_SUCCESS = "Create board successfully.\n";
char *CREATE_POST_FAIL = "Board does not exist.\n";
char *CREATE_POST_SUCCESS = "Create post successfully.\n";
char *LIST_POST_FAIL = "Board does not exist.\n";
char *READ_FAIL = "Post does not exist.\n";
char *DELETE_POST_SUCCESS = "Delete successfully.\n";
char *DELETE_POST_FAIL = "Post does not exist.\n";
char *UPDATE_POST_SUCCESS = "Update successfully.\n";
char *UPDATE_POST_FAIL = "Post does not exist.\n";
char *COMMENT_SUCCESS = "Comment successfully.\n";
char *COMMENT_FAIL = "Post does not exist.\n";
char *NOT_POST_OWNER = "Not the post owner.\n";
char BOARD_MSG[BUFSIZ];
char READ_MSG[BUFSIZ];
char COMMENT_MSG[BUFSIZ];
char POST_MSG[BUFSIZ];/*}}}*/

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
	    if (!strcmp(cmd[0], REGISTER)){ /* {{{ */
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
	    } /* }}} */

	    /* FOR login.*/
	    if (!strcmp(cmd[0], LOGIN)){ /* {{{ */
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
	    } /*}}}*/

	    /* FOR logout.*/
	    if (!strcmp(cmd[0], LOGOUT)){ /* {{{ */
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
	    } /* }}} */

	    /* FOR whoami.*/
	    if (!strcmp(cmd[0], WHOAMI)){ /*{{{*/
		/* TODO */
		if (state == ONLINE){
		    sprintf(USER_NAME, "%s\n", current_user);
		    send(fd, USER_NAME, strlen(USER_NAME), 0);
		}
		else if (state == OFFLINE){
		    send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		}
	    } /*}}}*/

	    /* FOR exit.*/
	    if (!strcmp(cmd[0], EXIT)){ /*{{{*/
		/* TODO */
		sqlite3_close(db);
		close(fd);
		return 0;
	    }/*}}}*/

	    /* FOR create_board.*/
	    if (!strcmp(cmd[0], CREATE_BOARD)){ /* {{{ */
		/* TODO */
		if (cmd_cnt != 2){
		    send(fd, USAGE_CREATE_BOARD, strlen(USAGE_CREATE_BOARD), 0);
		}
		else{
		    if (state == OFFLINE){
			send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		    }
		    else{
			char *boardname = cmd[1];
    		    	char sql[BUFSIZ];

    		    	sprintf(sql, "SELECT UID FROM BOARDS WHERE Boardname = '%s';", boardname);
    		    	rc = sqlite3_exec(db, (const char *)sql, board_exist_callback, (void *) boardname, &zErrmsg);

    		    	if (rc != SQLITE_OK){
    		    	    fprintf(stderr, "Failed to select data\n");
    		    	    fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    	    sqlite3_free(zErrmsg);
    		    	    sqlite3_close(db);

    		    	    return 1;
    		    	}

    		    	if (board_exist == BOARD_EXIST){
    		    	    send(fd, CREATE_BOARD_FAIL, strlen(CREATE_BOARD_FAIL), 0);
    		    	}

    		    	if (board_exist == BOARD_NON_EXIST){
    		    	    sprintf(sql, "INSERT INTO BOARDS (Boardname, Username) "\
    		    		    	 "VALUES ('%s', '%s');", boardname, current_user);
    		    	    rc = sqlite3_exec(db, (const char *)sql, 0, 0, &zErrmsg);

			    if (rc != SQLITE_OK){
		       	        fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    		sqlite3_free(zErrmsg);
    		    		sqlite3_close(db);

    		    		return 1;
    		    	    }
    		    	    else{
    		    		send(fd, CREATE_BOARD_SUCCESS, strlen(CREATE_BOARD_SUCCESS), 0);
    		    	    }
    		    	}
    		    	board_exist = BOARD_NON_EXIST;
		   }
		}
	    } /* }}} */

	    /* FOR list-board. */
	    if (!strcmp(cmd[0], LIST_BOARD)){ /* {{{ */
		/* TODO */
		if (cmd_cnt > 2){
		    send(fd, USAGE_LIST_BOARD, strlen(USAGE_LIST_BOARD), 0);
		}
		else{
		    char *boardname;
		    char sql[BUFSIZ];

		    if (cmd_cnt == 1){
			sprintf(sql, "SELECT * FROM BOARDS;");
			sprintf(BOARD_MSG, "\t%s\t\t%s\t\t%s\n", "Index", "Name", "Moderator");
		    }
		    else if (cmd[1][0] == '#' && cmd[1][1] == '#'){
			boardname = cmd[1]+2;
			str_replace(boardname, "\r", "");
			sprintf(sql, "SELECT * FROM BOARDS WHERE Boardname like '%%%s%%'", boardname);
			sprintf(BOARD_MSG, "\t%s\t\t%s\t\t%s\n", "Index", "Name", "Moderator");
		    }
		    else{
			send(fd, USAGE_LIST_BOARD, strlen(USAGE_LIST_BOARD), 0);
		    }
		    rc = sqlite3_exec(db, (const char *)sql, list_board_callback, 0, &zErrmsg);
		    send(fd, BOARD_MSG, strlen(BOARD_MSG), 0);

    		    if (rc != SQLITE_OK){
    		    	fprintf(stderr, "Failed to select data\n");
    		    	fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    	sqlite3_free(zErrmsg);
    		    	sqlite3_close(db);

    		    	return 1;
    		    }

		}
	    } /* }}} */

	    /* FOR list-post. */
	    if (!strcmp(cmd[0], LIST_POST)){ /* {{{ */
		/* TODO */
		if (cmd_cnt > 3 || cmd_cnt < 2){
		    send(fd, USAGE_LIST_POST, strlen(USAGE_LIST_POST), 0);
		}
		else{
		    char *boardname = cmd[1];
		    if (cmd_cnt == 2){
			boardname[strlen(boardname) - 1] = 0;
		    }
		    char *title;
		    char sql[BUFSIZ];

		    strcpy(current_board, boardname);

    		    sprintf(sql, "SELECT * FROM BOARDS;");
    		    rc = sqlite3_exec(db, (const char *)sql, post_exist_callback, 0, &zErrmsg);

    		    if (rc != SQLITE_OK){
    		        fprintf(stderr, "Failed to select data\n");
    		        fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		        sqlite3_free(zErrmsg);
    		        sqlite3_close(db);

    		        return 1;
    		    }

		    if (post_exist == BOARD_NON_EXIST){
			send(fd, LIST_POST_FAIL, strlen(LIST_POST_FAIL), 0);
		    }
		    else if (cmd_cnt == 3 && (cmd[2][0] != '#' || cmd[2][1] != '#')){
		    	send(fd, USAGE_LIST_POST, strlen(USAGE_LIST_POST), 0);
		    }
		    else{
			if (cmd_cnt == 2){
			    str_replace(boardname, "\r", "");
		    	    sprintf(sql, "SELECT * FROM POSTS WHERE Boardname = '%s';", boardname);
		    	    sprintf(POST_MSG, "\t%s\t\t%s\t\t%s\t\t%s\n", "ID", "Title", "Author", "Date");
		    	}
		    	else if (cmd[2][0] == '#' && cmd[2][1] == '#'){
		    	    title = cmd[2]+2;
		    	    str_replace(title, "\r", "");
		    	    sprintf(sql, "SELECT * FROM POSTS WHERE Title like '%%%s%%'", title);
		    	    sprintf(POST_MSG, "\t%s\t\t%s\t\t%s\t\t%s\n", "ID", "Title", "Author", "Date");
		    	}

			rc = sqlite3_exec(db, (const char *)sql, list_post_callback, 0, &zErrmsg);
		    	send(fd, POST_MSG, strlen(POST_MSG), 0);

    		    	if (rc != SQLITE_OK){
    		    		fprintf(stderr, "Failed to select data\n");
    		    		fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    		sqlite3_free(zErrmsg);
    		    		sqlite3_close(db);

    		    		return 1;
    		    	}

		    }
		    post_exist = BOARD_NON_EXIST;
		}
	    } /* }}} */

	    /* FOR read. */
	    if (!strcmp(cmd[0], READ)){ /* {{{ */
		/* TODO */
		if (cmd_cnt != 2){
		    send(fd, USAGE_READ, strlen(USAGE_READ), 0);
		}
		else{
		    char *uid = cmd[1];
		    int uid_num = atoi(uid);
		    char sql[BUFSIZ];

		    sprintf(sql, "SELECT * FROM POSTS WHERE UID = %d;", uid_num);

		    memset(READ_MSG, 0, sizeof(READ_MSG));

		    rc = sqlite3_exec(db, (const char *)sql, read_callback, 0, &zErrmsg);
		    if (post_exist == BOARD_NON_EXIST){
			send(fd, READ_FAIL, strlen(READ_FAIL), 0);
		    }
		    else{
			send(fd, READ_MSG, strlen(READ_MSG), 0);
		    }

    		    if (rc != SQLITE_OK){
    		    	fprintf(stderr, "Failed to select data\n");
    		    	fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    	sqlite3_free(zErrmsg);
    		    	sqlite3_close(db);

    		    	return 1;
    		    }

		    post_exist = BOARD_NON_EXIST;
		}
	    } /* }}} */

	    /* FOR delete-post. */
	    if (!strcmp(cmd[0], DELETE_POST)){ /* {{{ */
		/* TODO */
		if (cmd_cnt != 2){
		    send(fd, USAGE_DELETE_POST, strlen(USAGE_DELETE_POST), 0);
		}
		else{
		    if (state == OFFLINE){
			send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		    }
		    else{
			char *uid = cmd[1];
		    	int uid_num = atoi(uid);
		    	char sql[BUFSIZ];

		    	sprintf(sql, "SELECT * FROM POSTS WHERE UID = %d;", uid_num);
		    	rc = sqlite3_exec(db, (const char *)sql, delete_post_callback, 0, &zErrmsg);

    		    	if (rc != SQLITE_OK){
    		    		fprintf(stderr, "Failed to select data\n");
    		    		fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    		sqlite3_free(zErrmsg);
    		    		sqlite3_close(db);

    		    		return 1;
    		    	}

		    	if (post_exist == BOARD_NON_EXIST){
		    	    send(fd, DELETE_POST_FAIL, strlen(DELETE_POST_FAIL), 0);
		    	}
			else if (strcmp(USER_NAME, current_user)){
			    send(fd, NOT_POST_OWNER, strlen(NOT_POST_OWNER), 0);
			}
		    	else{
			    sprintf(sql, "DELETE FROM POSTS WHERE UID = %d;", uid_num);
		    	    rc = sqlite3_exec(db, (const char *)sql, 0, 0, &zErrmsg);

    		    	    if (rc != SQLITE_OK){
    		    	    	fprintf(stderr, "Failed to select data\n");
    		    	    	fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    	    	sqlite3_free(zErrmsg);
    		    	    	sqlite3_close(db);

    		    	    	return 1;
    		    	    }
			    else{
				send(fd, DELETE_POST_SUCCESS, strlen(DELETE_POST_SUCCESS), 0);
			    }
		    	}

			USER_NAME[0] = 0;
		    	post_exist = BOARD_NON_EXIST;
		    }
		}
	    } /* }}} */

	    /* FOR update-post. */
	    if (!strcmp(cmd[0], UPDATE_POST)){ /* {{{ */
		/* TODO */
		if (cmd_cnt < 4){
		    send(fd, USAGE_UPDATE_POST, strlen(USAGE_UPDATE_POST), 0);
		}
		else{
		    if (state == OFFLINE){
			send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		    }
		    else{
			char *uid = cmd[1];
			char *object = cmd[2];
			char new_content[BUFSIZ] = {0};
		    	int uid_num = atoi(uid);
		    	char sql[BUFSIZ];
			int wrong_cmd = 0;

			if (!strcmp(object, "--title")){
			    int i;
			    for (i = 3; i < cmd_cnt; i++){
				strcat(new_content, cmd[i]);
				if (i != cmd_cnt - 1) strcat(new_content, " ");
			    }
			}
			else if (!strcmp(object, "--content")){
			    int i;
			    for (i = 3; i < cmd_cnt; i++){
				strcat(new_content, cmd[i]);
				if (i != cmd_cnt - 1) strcat(new_content, " ");
			    }
			    str_replace(new_content, "<br>", "\n");
			}
			else{
			    send(fd, USAGE_UPDATE_POST, strlen(USAGE_UPDATE_POST), 0);
			    wrong_cmd = 1;
			}

		    	sprintf(sql, "SELECT * FROM POSTS WHERE UID = %d;", uid_num);
		    	rc = sqlite3_exec(db, (const char *)sql, update_post_callback, 0, &zErrmsg);

    		    	if (rc != SQLITE_OK){
    		    		fprintf(stderr, "Failed to select data\n");
    		    		fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    		sqlite3_free(zErrmsg);
    		    		sqlite3_close(db);

    		    		return 1;
    		    	}

		    	if (post_exist == BOARD_NON_EXIST && wrong_cmd == 0){
		    	    send(fd, UPDATE_POST_FAIL, strlen(UPDATE_POST_FAIL), 0);
		    	}
			else if (strcmp(USER_NAME, current_user)&& wrong_cmd == 0){
			    send(fd, NOT_POST_OWNER, strlen(NOT_POST_OWNER), 0);
			}
		    	else{
			    if (!strcmp(object, "--title"))
				sprintf(sql, "UPDATE POSTS SET %s = '%s' WHERE UID = %d;", "Title", new_content, uid_num);
			    else if (!strcmp(object, "--content"))
				sprintf(sql, "UPDATE POSTS SET %s = '%s' WHERE UID = %d;", "Content", new_content, uid_num);
			    else
				sprintf(sql, "UPDATE POSTS SET %s = '%s' WHERE UID = %d;", "Content", new_content, 0);

			    rc = sqlite3_exec(db, (const char *)sql, 0, 0, &zErrmsg);

    		    	    if (rc != SQLITE_OK){
    		    	    	fprintf(stderr, "Failed to select data\n");
    		    	    	fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    	    	sqlite3_free(zErrmsg);
    		    	    	sqlite3_close(db);

    		    	    	return 1;
    		    	    }
			    else if (wrong_cmd == 0){
				send(fd, UPDATE_POST_SUCCESS, strlen(UPDATE_POST_SUCCESS), 0);
			    }
		    	}

			USER_NAME[0] = 0;
		    	post_exist = BOARD_NON_EXIST;
		    }
		}
	    } /* }}} */

	    /* FOR comment. */
	    if (!strcmp(cmd[0], COMMENT)){ /* {{{ */
		/* TODO */
		if (cmd_cnt < 3){
		    send(fd, USAGE_COMMENT, strlen(USAGE_COMMENT), 0);
		}
		else{
		    if (state == OFFLINE){
			send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		    }
		    else{
			char *uid = cmd[1];
			char buf[BUFSIZ] = {0};
			char comment[BUFSIZ] = {0};
		    	int uid_num = atoi(uid);
		    	char sql[BUFSIZ];

			int i;
			for (i = 2; i < cmd_cnt; i++){
			    strcat(buf, cmd[i]);
			    if (i != cmd_cnt - 1) strcat(buf, " ");
			}
			sprintf(comment, "\t%s: %s\n", current_user, buf);
			str_replace(comment, "\r", "");

		    	sprintf(sql, "SELECT * FROM POSTS WHERE UID = %d;", uid_num);
		    	rc = sqlite3_exec(db, (const char *)sql, comment_callback, 0, &zErrmsg);

    		    	if (rc != SQLITE_OK){
    		    		fprintf(stderr, "Failed to select data\n");
    		    		fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    		sqlite3_free(zErrmsg);
    		    		sqlite3_close(db);

    		    		return 1;
    		    	}

		    	if (post_exist == BOARD_NON_EXIST){
		    	    send(fd, COMMENT_FAIL, strlen(COMMENT_FAIL), 0);
		    	}
		    	else{
			    strcat(COMMENT_MSG, comment);
			    str_replace(COMMENT_MSG, "\r", "");
			    sprintf(sql, "UPDATE POSTS SET Comment = '%s' WHERE UID = %d;", COMMENT_MSG, uid_num);

			    rc = sqlite3_exec(db, (const char *)sql, 0, 0, &zErrmsg);

    		    	    if (rc != SQLITE_OK){
    		    	    	fprintf(stderr, "Failed to select data\n");
    		    	    	fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    	    	sqlite3_free(zErrmsg);
    		    	    	sqlite3_close(db);

    		    	    	return 1;
    		    	    }
		    	}

		    	post_exist = BOARD_NON_EXIST;
		    }
		}
	    } /* }}} */

	    /* FOR create_post.*/
	    if (!strcmp(cmd[0], CREATE_POST)){ /* {{{ */
		/* TODO */
		if (cmd_cnt < 6){
		    send(fd, USAGE_CREATE_POST, strlen(USAGE_CREATE_POST), 0);
		}
		else{
		    if (state == OFFLINE){
			send(fd, LOGIN_FIRST, strlen(LOGIN_FIRST), 0);
		    }
		    else{
			char *boardname = cmd[1];
			char title[BUFSIZ] = "";
			char content[BUFSIZ] = "";
    		    	char sql[BUFSIZ];
			char *date = get_date();
			int i;
			/* concatenate the title string */
			for (i = 3; i < cmd_cnt; i++){
			    if (strcmp(cmd[i], "--content")){
				strcat(title, cmd[i]);
			    }
			    else{
				break;
			    }
			    strcat(title, " ");
			}

			/* concatenate the content string */
			for (i = i + 1; i < cmd_cnt; i++){
			    strcat(content, cmd[i]);

			    if (i != cmd_cnt - 1) strcat(content, " ");
			}

			/* replace <br> sign to create a new line */
			str_replace(content, "<br>", "\n");

			strcpy(current_board, boardname);

    		    	sprintf(sql, "SELECT * FROM BOARDS;");
    		    	rc = sqlite3_exec(db, (const char *)sql, post_exist_callback, 0, &zErrmsg);

    		    	if (rc != SQLITE_OK){
    		    	    fprintf(stderr, "Failed to select data\n");
    		    	    fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    	    sqlite3_free(zErrmsg);
    		    	    sqlite3_close(db);

    		    	    return 1;
    		    	}

    		    	if (post_exist == BOARD_EXIST){
    		    	    sprintf(sql, "INSERT INTO POSTS (Title, Author, Content, Date, Boardname) "\
    		    		    	 "VALUES ('%s', '%s', '%s', '%s', '%s');"
					 , title, current_user, content, date, boardname);
    		    	    rc = sqlite3_exec(db, (const char *)sql, 0, 0, &zErrmsg);

			    if (rc != SQLITE_OK){
    		    		fprintf(stderr, "SQL error: %s\n", zErrmsg);

    		    	        sqlite3_free(zErrmsg);
    		         	sqlite3_close(db);

    		    		return 1;
    		    	    }
    		    	    else{
    		    		send(fd, CREATE_POST_SUCCESS, strlen(CREATE_POST_SUCCESS), 0);
    		    	    }
    		    	}

    		    	if (post_exist == BOARD_NON_EXIST){
    		    	    send(fd, CREATE_POST_FAIL, strlen(CREATE_POST_FAIL), 0);
    		    	}

			post_exist = BOARD_NON_EXIST;
		   }
		}
	    } /* }}} */

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

/* welcome message to users.*/
int greeting(int fd){ /* {{{ */
    char *msg = "********************************\n"
		"** Welcome to the BBS server. **\n"
		"********************************\n"
		"% ";

    if (send(fd, msg, strlen(msg), 0) <= 0){
	printf("could send greeting msg to %d client.\n", fd);
    }


} /*}}}*/

/* parse the argument*/
int cmd_parser(char buf[], char delim[], char *cmd[]){/* {{{ */
    int cmd_cnt = 0;
    char *ptr = strtok(buf, delim);
    while (ptr != NULL){
        cmd[cmd_cnt++] = ptr;
        ptr = strtok(NULL, delim);
    }
    return cmd_cnt;
}/* }}} */

/* register part's callback function, check whether the username exist.*/
int regist_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;

    for (i = 0; i < argc; i++) {
	if (argv[i]){
	    exist = USER_EXIST;
	}
    }

    return 0;
}/* }}} */

/* create_board part's callback function, check whether the board exist.*/
int board_exist_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;

    for (i = 0; i < argc; i++) {
	if (argv[i]){
	    board_exist = BOARD_EXIST;
	}
    }

    return 0;
}/* }}} */

/* create_post part's callback function, check whether the board exist.*/
int post_exist_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;
    char buf[BUFSIZ];

    if (post_exist == BOARD_NON_EXIST){
	for (i = 0; i < argc; i+=3) {
    	    if (argv[i]){
    	        sprintf(buf, "%s", argv[i+1]);
    	        str_replace(buf, "\r", "");
    	        if (!strcmp(buf, current_board)){
		    post_exist = BOARD_EXIST;
		}
    	        else {
		    post_exist = BOARD_NON_EXIST;
		}
    	    }
    	}

    }

    return 0;
}/* }}} */

/* login part's callback function, will record the user's password in order to make a further check.*/
int login_callback(void *data, int argc, char **argv, char **azColName){/* {{{ */
    int i;

    for (i = 0; i < argc; i++) {
	if (argv[i]){
	    /* get the password of the current user in order to verify. */
	    strcpy(verify_passwd, argv[i]);
	}
    }
    return 0;
}/* }}} */

/* list_board part's callback function, will list the board information.*/
int list_board_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;
    char buf[BUFSIZ];

    for (i = 0; i < argc; i+=3) {
	if (argv[i]){
	    sprintf(buf, "\t%s\t\t%s\t\t%s\n", argv[i], argv[i+1], argv[i+2]);
	    strcat(BOARD_MSG, buf);
	}
    }
    str_replace(BOARD_MSG, "\r", "");

    return 0;
}/* }}} */

/* list_post part's callback function, will list the post information.*/
int list_post_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;
    char buf[BUFSIZ];
    char dt[BUFSIZ];
    char yr[BUFSIZ];
    char mon[BUFSIZ];
    char day[BUFSIZ];

    for (i = 0; i < argc; i+=7) {
	if (argv[i]){
	    sprintf(dt, "%s", argv[i+6]);
	    sscanf(dt, "%[^-]-%[^-]-%s", yr, mon, day);
	    sprintf(dt, "%s/%s", mon, day);
	    sprintf(buf, "\t%s\t\t%s\t\t%s\t\t%s\n", argv[i], argv[i+1], argv[i+2], dt);
	    strcat(POST_MSG, buf);
	}
    }
    str_replace(POST_MSG, "\r", "");

    return 0;
}/* }}} */

/* read part's callback function, will get more post information.*/
int read_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;
    char buf[BUFSIZ];

    for (i = 0; i < argc; i+=7) {
	sprintf(buf, "\t%s\t: %s\n", azColName[i+2], argv[i+2]);
	strcat(READ_MSG, buf);
	sprintf(buf, "\t%s\t: %s\n", azColName[i+1], argv[i+1]);
	strcat(READ_MSG, buf);
	sprintf(buf, "\t%s\t: %s\n", azColName[i+6], argv[i+6]);
	strcat(READ_MSG, buf);
	sprintf(buf, "\t--\n");
	strcat(READ_MSG, buf);
	sprintf(buf, "\t%s\n", argv[i+3]);
	str_replace(buf, "\n", "\n\t");
	strcat(READ_MSG, buf);
	sprintf(buf, "--\n");
	strcat(READ_MSG, buf);
	if (argv[i+5]){
	    sprintf(buf, "%s\n", argv[i+5]);
	    strcat(READ_MSG, buf);
	}

	post_exist = BOARD_EXIST;
    }
    str_replace(READ_MSG, "\r", "");

    return 0;
}/* }}} */

/* delete_post part's callback function, will delete the post information.*/
int delete_post_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;

    for (i = 0; i < argc; i+=7) {
	if (argv[i]){
	    sprintf(USER_NAME, "%s", argv[i+2]);

	    post_exist = BOARD_EXIST;
	}
    }
    str_replace(USER_NAME, "\r", "");

    return 0;
}/* }}} */

/* update_post part's callback function, will update the post information.*/
int update_post_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;

    for (i = 0; i < argc; i+=7) {
	if (argv[i]){
	    sprintf(USER_NAME, "%s", argv[i+2]);

	    post_exist = BOARD_EXIST;
	}
    }
    str_replace(USER_NAME, "\r", "");

    return 0;
}/* }}} */

/* comment part's callback function, will update the post comment information.*/
int comment_callback(void *NotUsed, int argc, char **argv, char **azColName){/* {{{ */
    NotUsed = 0;
    int i;

    for (i = 0; i < argc; i+=7) {
	if (argv[i]){
	    sprintf(COMMENT_MSG, "%s", argv[i+5] ? argv[i+5] : "");

	    post_exist = BOARD_EXIST;
	}
    }
    str_replace(COMMENT_MSG, "\r", "");

    return 0;
}/* }}} */

/* replace substring pattern with new pattern.*/
void str_replace(char *target, const char *needle, const char *replacement){/* {{{ */
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);

        /* walked past last occurrence of needle; copy remaining part */
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        /* copy part before needle */
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        /* copy replacement string */
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        /* adjust pointers, move on */
        tmp = p + needle_len;
    }

    /* write altered string back to target */
    strcpy(target, buffer);
}/* }}} */

/* get current date.*/
char *get_date(){/* {{{ */
    char date[10] = {0};
    char *ret;

    time_t now = time(0);
    struct tm *ltm = localtime(&now);

    sprintf(date, "%04d-%02d-%02d",1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    strcpy(ret, date);

    return ret;
} /* }}} */
