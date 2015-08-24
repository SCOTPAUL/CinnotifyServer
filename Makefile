CC=gcc
CFLAGS= -c -Wall

all: cinnotify-server

cinnotify-server: notify.o server.o networking.o
	$(CC) notify.o server.o networking.o `pkg-config --cflags --libs libnotify` -o cinnotify-server

notify.o: notify.c
	$(CC) $(CFLAGS) `pkg-config --cflags --libs glib-2.0` notify.c

server.o: server.c
	$(CC) $(CFLAGS) `pkg-config --cflags --libs glib-2.0` server.c

networking.o: networking.c
	$(CC) $(CFLAGS) networking.c

.PHONY: clean
clean:
	@echo "Cleaning..."
	-rm -f cinnotify-server *.o
	@echo "Finished cleaning."
