SRC = ../../worker

all:
	gcc -o worker ${SRC}/common.h ${SRC}/agent.h ${SRC}/agent.c ${SRC}/array.h ${SRC}/array.c ${SRC}/function.h ${SRC}/function.c ${SRC}/rw.h ${SRC}/rw.c ${SRC}/util.h ${SRC}/util.c ${SRC}/worker.h ${SRC}/worker.c worker.c -lev -lpthread -lmsgpackc -g
