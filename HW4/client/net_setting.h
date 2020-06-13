# ifndef __NET_SETTING_H__
# define __NET_SETTING_H__

#define _USE_BSD
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>

void	reaper(int sig);

int     errexit(const char *format, ...);

/* for client */
int     connectsock(const char *host, const char *service, const char *transport );

int     connectTCP(const char *host, const char *service );

# endif
