# name of the program (Minix service)
PROG=lab4

# source code files to be compiled
SRCS = lab4.c mouse.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -I../lab2

# list of library dependencies
DPADD += ${LIBLCF} ../lab2/libtimer.a
LDADD += -llcf -L../lab2 -ltimer

LIB_NAME = libmouse.a
LIB_OBJS = mouse.o
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
