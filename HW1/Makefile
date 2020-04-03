CC=gcc
CFLAGS=-g -Wall -ansi
EXE=server
INCDIR=.

ALL:${EXE}

${EXE}:main.o net_setting.o system.o
	${CC} -o $@ main.o net_setting.o system.o

%.o: %.c
	${CC} -I${INCDIR} ${CFLAGS} -c $<

.PHONY: clean

clean:
	rm -rf *.o ${EXE}
