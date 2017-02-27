########################################################################
# vim: ts=8 sw=8
# ######################################################################
PREFIX	:=/opt/shorten
BINDIR	=${PREFIX}/bin

CC	=ccache gcc -march=i686 -std=gnu99
CFLAGS	=-Os -Wall -Werror -pedantic -g -pipe
LDFLAGS	=-g
LDLIBS	=

all::	shorten

check::	shorten
	./shorten ${ARGS} Makefile

clean::
	${RM} *.o a.out core.* lint tags

distclean clobber:: clean
	${RM} shorten

install:: shorten
	install -d ${BINDIR}
	install -c shorten ${BINDIR}/

uninstall::
	${RM} ${BINDIR}/shorten

tags::
	ctags -R .
