include config.mk

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

.POSIX:

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

install: $(BIN)
	mkdir -p $(DESTDIR)$(PREFIX)/bin/
	cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all clean
