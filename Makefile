########################################################################
# vim: ts=8 sw=8

PREFIX	:=${HOME}/opt/$(shell uname -m)
BINDIR	=${PREFIX}/bin

CC	=ccache gcc -march=i686
OPT	=-Os
CFLAGS	=${OPT} -pipe -Wall -Werror -pedantic -g
LDFLAGS	=-g
LDLIBS	=

all::	shorten

check::	shorten
	./shorten Makefile
