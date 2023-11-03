BIN     = dswm

PREFIX  = /usr

CC      = cc
LD      = $(CC)

CFLAGS  = -std=c99 -pedantic -Wall -Os -I/usr/include
LDFLAGS = -lxcb -lxcb-util -lxcb-keysyms -L/usr/lib
