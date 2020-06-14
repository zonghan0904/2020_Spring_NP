CC=g++
CFLAGS=-g -Wall -ansi
EXE=server
LIB=-lsqlite3 -lrdkafka -lz -llz4 -L/usr/local/lib
OBJ=main.o net_setting.o system.o producer.o consumer.o
INC=-I./ -I/usr/include

all:${OBJ}
	${CC} -o ${EXE} ${OBJ} ${LIB}
	g++ create_db.cpp -o create_db ${LIB}
	./create_db
	rm -rf *.o

${OBJ}:
	${CC} ${INC} ${CFLAGS} -std=c++11 -c *.cpp ${LIB}
	gcc ${INC} ${CFLAGS} -std=gnu99 -c *.c ${LIB}


.PHONY: clean

DB:
	g++ create_db.cpp -o create_db ${LIB}
	./create_db

clean:
	rm -rf *.o ${EXE} users.db create_db
