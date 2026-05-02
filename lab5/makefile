# name of the program (Minix service)
PROG=lab5

# source code files to be compiled
SRCS = lab5.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -I../lab2

# list of library dependencies
DPADD += ${LIBLCF} ${LIB_NAME} ../lab3/libkbc.a ../lab2/libtimer.a
LDADD += -llcf -L. -lvideo -L../lab3 -lkbc -L../lab2 -ltimer

LIB_NAME = libvideo.a
LIB_SRCS = video.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

timer_lib:
	cd ../lab2 && ${MAKE} lib

kbc_lib:
	cd ../lab3 && ${MAKE} lib

all: timer_lib kbc_lib lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
