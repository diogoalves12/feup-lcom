# name of the program (Minix service)
PROG=lab5

# source code files to be compiled
SRCS = lab5.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies
DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -llcf -L. -lvideo

LIB_NAME = libvideo.a
LIB_SRCS = video.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
