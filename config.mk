BIN     = wm

PREFIX  = /usr

CC      = cc
LD      = $(CC)

CFLAGS  = -g -std=c99 -pedantic -Wall -Os -I/usr/include
LDFLAGS = -lxcb -lxcb-util -lxcb-cursor -L/usr/lib
