PROG = proj

.PATH: ${.CURDIR}/src
.PATH: ${.CURDIR}/src/graphics
.PATH: ${.CURDIR}/src/game
.PATH: ${.CURDIR}/src/iu
.PATH: ${.CURDIR}/../lab2
.PATH: ${.CURDIR}/../lab3
.PATH: ${.CURDIR}/../lab5

SRCS = main.c renderer.c physics.c game.c arena.c menu.c timer.c keyboard.c video.c utils.c

CFLAGS += -pedantic
CFLAGS += -I${.CURDIR}/src
CFLAGS += -I${.CURDIR}/src/graphics
CFLAGS += -I${.CURDIR}/src/game
CFLAGS += -I${.CURDIR}/src/iu
CFLAGS += -I../lab2
CFLAGS += -I../lab3
CFLAGS += -I../lab5

DPADD += ${LIBLCF}
LDADD += -llcf

all: timer_lib kbd_lib video_lib

timer_lib:
	cd ../lab2 && ${MAKE} lib

kbd_lib: timer_lib
	cd ../lab3 && ${MAKE} lib

video_lib: timer_lib kbd_lib
	cd ../lab5 && ${MAKE} lib

.include <minix.lcom.mk>
