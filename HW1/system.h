# ifndef __SYSTEM_H__
# define __SYSTEM_H__

# include "net_setting.h"
# include <sqlite3.h>

int greeting(int fd);
int cmd_parser(char buf[], char delim[], char *cmd[]);
void Regist(char *name, char *email, char *passwd);
void Login(char *name, char *passwd);
void Logout();
void Whoami();
void Exit();
int TCPechod(int fd);


# endif
