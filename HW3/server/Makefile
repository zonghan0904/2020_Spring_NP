CC=g++
CFLAGS=-g -Wall -ansi
EXE=server
LIB=-lsqlite3
INCDIR=.

ALL:${EXE}

${EXE}:main.o net_setting.o system.o
	${CC} -o $@ main.o net_setting.o system.o ${LIB}
	g++ create_db.cpp -o create_db -lsqlite3
	./create_db

%.o: %.cpp
	${CC} -I${INCDIR} ${CFLAGS} -c $<

.PHONY: clean

DB:
	g++ create_db.cpp -o create_db -lsqlite3
	./create_db

clean:
	rm -rf *.o ${EXE} users.db create_db
