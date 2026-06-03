PROG = proj

.PATH: ${.CURDIR}/src
.PATH: ${.CURDIR}/src/graphics
.PATH: ${.CURDIR}/src/game
.PATH: ${.CURDIR}/src/input
.PATH: ${.CURDIR}/src/ui
.PATH: ${.CURDIR}/../lab2
.PATH: ${.CURDIR}/../lab3
.PATH: ${.CURDIR}/../lab5

SRCS = main.c game.c renderer.c arena.c arena_layout.c arena_draw.c player.c collision.c combat.c keyboard_input.c game_input.c mouse_input.c menu.c game_over.c pause_menu.c timer.c keyboard.c video.c utils.c

CFLAGS += -pedantic -D_LCOM_OPTIMIZED_ -O2
CFLAGS += -I${.CURDIR}/src
CFLAGS += -I${.CURDIR}/src/graphics
CFLAGS += -I${.CURDIR}/src/game
CFLAGS += -I${.CURDIR}/src/input
CFLAGS += -I${.CURDIR}/src/ui
CFLAGS += -I../lab2
CFLAGS += -I../lab3
CFLAGS += -I../lab4
CFLAGS += -I../lab5

DPADD += ${LIBLCF}
DPADD += ../lab4/libmouse.a
LDADD += -llcf
LDADD += -lm
LDADD += -L../lab4 -lmouse

all: timer_lib kbd_lib mouse_lib video_lib

timer_lib:
	cd ../lab2 && ${MAKE} lib

kbd_lib: timer_lib
	cd ../lab3 && ${MAKE} lib

video_lib: timer_lib kbd_lib
	cd ../lab5 && ${MAKE} lib

mouse_lib:
	cd ../lab4 && ${MAKE} lib

.include <minix.lcom.mk>
