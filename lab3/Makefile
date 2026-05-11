PROG = lab3

SRCS = lab3.c keyboard.c utils.c

CFLAGS += -pedantic -DLAB3

DPADD += ${LIBLCF} ../lab2/libtimer.a
LDADD += -llcf -L../lab2 -ltimer

LIB_NAME = libkbd.a
LIB_OBJS = keyboard.o utils.o
CLEANFILES += ${LIB_NAME} trace.txt output.txt libkbc.a libkbc_pic.a

timer_lib:
	cd ../lab2 && ${MAKE} lib

lab3: timer_lib

lib: timer_lib ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

.include <minix.lcom.mk>
