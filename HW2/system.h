# ifndef __SYSTEM_H__
# define __SYSTEM_H__

# include "net_setting.h"
# include <time.h>
# include <sqlite3.h>


int greeting(int fd);
int cmd_parser(char buf[], char delim[], char *cmd[]);
int regist_callback(void *NotUsed, int argc, char **argv, char **azColName);
int login_callback(void *data, int argc, char **argv, char **azColName);
void Regist(char *name, char *email, char *passwd);
void Login(char *name, char *passwd);
void Logout();
void Whoami();
void Exit();
int TCPechod(int fd);

int board_exist_callback(void *NotUsed, int argc, char **argv, char **azColName);
int post_exist_callback(void *data, int argc, char **argv, char **azColName);
int list_board_callback(void *NotUsed, int argc, char **argv, char **azColName);
int list_post_callback(void *NotUsed, int argc, char **argv, char **azColName);
int read_callback(void *NotUsed, int argc, char **argv, char **azColName);
int delete_post_callback(void *NotUsed, int argc, char **argv, char **azColName);
char *get_date();
void str_replace(char *target, const char *needle, const char *replacement);

# endif
