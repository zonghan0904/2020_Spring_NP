# include "net_setting.h"
# include "aws_utils.h"
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <stdio.h>

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
char *user_name;
char prefix[BUFSIZ];
char title[BUFSIZ];
char content[BUFSIZ];

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
char *USAGE_LOGIN = "Usage: login <username> <password>";
char *USAGE_CREATE_BOARD = "Usage: create-board <name>";
char *USAGE_CREATE_POST = "Usage: create-post <board-name> --title <title> --content <content>";
char *USAGE_LIST_BOARD = "Usage: list-board ##<key>";
char *USAGE_LIST_POST = "Usage: list-post <board-name> ##<key>";
char *USAGE_READ = "Usage: read <post-id>";
char *USAGE_DELETE_POST = "delete-post <post-id>";
char *USAGE_UPDATE_POST = "update-post <post-id> --title/content <new>";
char *USAGE_COMMENT = "comment <post-id> <comment>";
char *REGIST_SUCCESS = "Register successfully.";
char *REGIST_FAIL = "Username is already used.";
char LOGIN_SUCCESS[BUFSIZ];
char *LOGIN_FAIL = "Login failed.";
char *LOGIN_FIRST = "Please login first.";
char LOGOUT_SUCCESS[BUFSIZ];
char *LOGOUT_FIRST = "Please logout first.";
char USER_NAME[BUFSIZ];
char *CREATE_BOARD_FAIL = "Board already exist.";
char *CREATE_BOARD_SUCCESS = "Create board successfully.";
char *CREATE_POST_FAIL = "Board does not exist.";
char *CREATE_POST_SUCCESS = "Create post successfully.";
char *LIST_POST_FAIL = "Board does not exist.";
char *READ_FAIL = "Post does not exist.";
char *DELETE_POST_SUCCESS = "Delete successfully.";
char *DELETE_POST_FAIL = "Post does not exist.";
char *UPDATE_POST_SUCCESS = "Update successfully.";
char *UPDATE_POST_FAIL = "Post does not exist.";
char *COMMENT_SUCCESS = "Comment successfully.";
char *COMMENT_FAIL = "Post does not exist.";
char *NOT_POST_OWNER = "Not the post owner.";
char BOARD_MSG[BUFSIZ];
char READ_MSG[BUFSIZ];
char COMMENT_MSG[BUFSIZ];
char POST_MSG[BUFSIZ];/*}}}*/

int     errno;


#define QLEN              32    /* maximum connection queue length      */
#define BUFSIZE         4096
#define LINELEN         128

char    rbuf[BUFSIZE];         /* buffer for one line of text  */
char	wbuf[BUFSIZE];
char	buf[BUFSIZE];
int     s, n;                   /* socket descriptor, read count*/
char delim[] = " ";
int cmd_cnt = 0;
char *cmd[BUFSIZ];

int cmd_parser(char buf[], char delim[], char *cmd[]){
    int cnt = 0;
    char *ptr = strtok(buf, delim);
    while (ptr != NULL){
        cmd[cnt++] = ptr;
        ptr = strtok(NULL, delim);
    }
    return cnt;
}
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

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

/* thread for reading */
void* child(void *data){
    while(n = recv(s, rbuf, sizeof(rbuf), 0)){
	rbuf[n] = 0;
        fputs(rbuf, stdout);
	if (!strncmp(rbuf, REGIST_SUCCESS, strlen(REGIST_SUCCESS))){
	    strcpy(prefix, "np20200628");
	    strcat(prefix, user_name);
	    const Aws::String bucket_name = prefix;
	    create_bucket(bucket_name);
	}
	if (!strncmp(rbuf, CREATE_POST_SUCCESS, strlen(CREATE_POST_SUCCESS))){
	    strcpy(prefix, "np20200628");
	    strcat(prefix, current_user);
	    // printf("bucketname = %s\n", prefix);
	    const Aws::String bucket_name = prefix;
	    strcpy(prefix, "np20200628");
	    strcat(prefix, title);
	    // printf("keyname = %s\n", prefix);
	    const Aws::String key_name = prefix;
	    const std::string cont = content;
	    put_object(bucket_name, key_name, cont);
	}

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
	cmd_cnt = cmd_parser(buf, delim, cmd);
	if (cmd_cnt){
	    if (cmd_cnt == 1) cmd[0][strlen(cmd[0]) - 1] = 0;
	    /* FOR register. */
	    if (!strcmp(cmd[0], REGISTER)){ /* {{{ */
		/* TODO */
		if (cmd_cnt != 4){

		}
		else{
		    user_name = cmd[1];
		}
	    } /* }}} */

	    /* FOR login.*/
	    if (!strcmp(cmd[0], LOGIN)){ /* {{{ */
		/* TODO */
		if (cmd_cnt != 3){

		}
		else{
		    strcpy(current_user,cmd[1]);
		}
	    } /*}}}*/

	    /* FOR logout.*/
	    if (!strcmp(cmd[0], LOGOUT)){ /* {{{ */
		/* TODO */
		user_name = "";
	    } /* }}} */

	    /* FOR whoami.*/
	    if (!strcmp(cmd[0], WHOAMI)){ /*{{{*/
		/* TODO */
	    } /*}}}*/

	    /* FOR create_board.*/
	    if (!strcmp(cmd[0], CREATE_BOARD)){ /* {{{ */
		/* TODO */
	    } /* }}} */

	    /* FOR list-board. */
	    if (!strcmp(cmd[0], LIST_BOARD)){ /* {{{ */
	    } /* }}} */

	    /* FOR list-post. */
	    if (!strcmp(cmd[0], LIST_POST)){ /* {{{ */
		/* TODO */
	    } /* }}} */

	    /* FOR read. */
	    if (!strcmp(cmd[0], READ)){ /* {{{ */
		/* TODO */
	    } /* }}} */

	    /* FOR delete-post. */
	    if (!strcmp(cmd[0], DELETE_POST)){ /* {{{ */
	    } /* }}} */

	    /* FOR update-post. */
	    if (!strcmp(cmd[0], UPDATE_POST)){ /* {{{ */
		/* TODO */
	    } /* }}} */

	    /* FOR comment. */
	    if (!strcmp(cmd[0], COMMENT)){ /* {{{ */
		/* TODO */
	    } /* }}} */

	    /* FOR create_post.*/
	    if (!strcmp(cmd[0], CREATE_POST)){ /* {{{ */
		/* TODO */
		if (cmd_cnt < 6){

		}
		else{
		    char *boardname = cmd[1];
		    strcpy(title, "");
		    strcpy(content, "");
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
		}
	    } /* }}} */
	}
	strcat(wbuf, "\r\n");
	if (send(s, wbuf, strlen(wbuf), 0) < 0)
	    printf("error in sending message to server\n");
	usleep(500);
	memset(wbuf, 0, sizeof(wbuf));
    }

    exit(0);
}
