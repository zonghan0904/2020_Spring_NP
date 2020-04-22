CC=gcc
CFLAGS=-g -Wall -ansi
EXE=server
LIB=sqlite3
INCDIR=.

ALL:${EXE}

${EXE}:main.o net_setting.o system.o
	${CC} -o $@ main.o net_setting.o system.o -l${LIB}
	gcc create_db.c -o create_db -lsqlite3
	./create_db

%.o: %.c
	${CC} -I${INCDIR} ${CFLAGS} -c $<

.PHONY: clean

DB:
	gcc create_db.c -o create_db -lsqlite3
	./create_db

clean:
	rm -rf *.o ${EXE} users.db create_db
