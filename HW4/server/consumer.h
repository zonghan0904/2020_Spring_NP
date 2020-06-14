#ifndef __CONSUMER_H__
#define __CONSUMER_H__

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
# include "net_setting.h"


/* Typical include path would be <librdkafka/rdkafka.h>, but this program
 * is builtin from within the librdkafka source tree and thus differs. */
//#include <librdkafka/rdkafka.h>
#include <librdkafka/rdkafka.h>

extern volatile sig_atomic_t run_consume;

/**
 * @brief Signal termination of program
 */
extern void stop_consume (int sig);



/**
 * @returns 1 if all bytes are printable, else 0.
 */
extern int is_printable (const char *buf, size_t size);

void Subscribe (const char* brokers, const char** topics, int fd, char* KEYWORD);

#endif
