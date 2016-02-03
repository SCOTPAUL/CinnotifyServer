CC=gcc
CFLAGS= -c -Wall
GLIB_TARGETS = server.o notify.o
TARGETS = $(GLIB_TARGETS) networking.o message_parser.o

all: cinnotify-server

cinnotify-server: $(TARGETS)
	$(CC) $^  `pkg-config --cflags --libs gtk+-2.0 glib-2.0 libnotify` -o $@

server.o: server.c
notify.o: notify.c

$(GLIB_TARGETS):
	$(CC) $(CFLAGS) `pkg-config --cflags glib-2.0 gtk+-2.0` $<

%.o: %.c
	$(CC) $(CFLAGS) $<

.PHONY: clean
clean:
	@echo "Cleaning..."
	-rm -f cinnotify-server *.o
	@echo "Finished cleaning."
