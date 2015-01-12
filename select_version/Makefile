CC = gcc
CFLAGS =
OBJS = main.o tap.o udp.o
TARGET = usertap

default: ${OBJS}
	@${CC} -o ${TARGET} ${CFLAGS} ${OBJS}
clean:
	@rm -f ${TARGET} ${OBJS}
