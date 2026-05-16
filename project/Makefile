PROG = proj

.PATH: ${.CURDIR}/src

SRCS = main.c

CFLAGS += -pedantic
CFLAGS += -I${.CURDIR}/src
CFLAGS += -I../lab2
CFLAGS += -I../lab3
CFLAGS += -I../lab5

DPADD += ${LIBLCF} ../lab2/libtimer.a ../lab3/libkbd.a ../lab5/libvideo.a
LDADD += -llcf -L../lab2 -ltimer -L../lab3 -lkbd -L../lab5 -lvideo

all: timer_lib kbd_lib video_lib

timer_lib:
	cd ../lab2 && ${MAKE} lib

kbd_lib: timer_lib
	cd ../lab3 && ${MAKE} lib

video_lib: timer_lib kbd_lib
	cd ../lab5 && ${MAKE} lib

.include <minix.lcom.mk>
