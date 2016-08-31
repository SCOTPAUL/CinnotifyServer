CFLAGS=-g -c -Wall -Werror -Wextra
GLIB_TARGETS =server.o notify.o cJSON.o
TARGETS = $(GLIB_TARGETS) networking.o crypto.o
INSTALL_LOCATION ?=/usr/local/bin


all: cinnotify-server
install: cinnotify-server
	mkdir -p $(INSTALL_LOCATION)
	cp cinnotify-server $(INSTALL_LOCATION)

uninstall:
	rm $(INSTALL_LOCATION)/cinnotify-server

cinnotify-server: $(TARGETS)
	$(CC) $^  `pkg-config --cflags --libs gtk+-2.0 glib-2.0 libnotify openssl` -o $@ -lm

server.o: server.c
cJSON.o: lib/cJSON/cJSON.c
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
