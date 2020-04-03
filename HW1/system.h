# ifndef __SYSTEM_H__
# define __SYSTEM_H__

# include "net_setting.h"

void Regist(char *name, char *email, char *passwd);
void Login(char *name, char *passwd);
void Logout();
void Whoami();
void Exit();
int cmd_parser(char buf[], char delim[], char *cmd[]);
int TCPechod(int fd);


# endif
