PROG = proj

.PATH: ${.CURDIR}/src

SRCS = main.c

CFLAGS += -pedantic
CFLAGS += -I${.CURDIR}/src

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
