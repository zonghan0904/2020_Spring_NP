CC=g++
CFLAGS=-g -Wall -ansi
EXE=client
LIB=-lsqlite3 -lpthread
INCDIR=.

ALL:${EXE}

${EXE}:main.o net_setting.o
	${CC} -o $@ main.o net_setting.o ${LIB}

%.o: %.cpp
	${CC} -I${INCDIR} ${CFLAGS} -c $<

.PHONY: clean

clean:
	rm -rf *.o ${EXE}
