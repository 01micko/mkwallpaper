CC?=cc
PREFIX?=/usr/local
CFLAGS+=-Wall -pedantic -std=gnu99 $(shell pkg-config --cflags pangocairo)
LDFLAGS+=$(shell pkg-config --libs pangocairo)

all: mkwallpaper

mkwallpaper: mkwallpaper.o
	$(CC) -o $@ $^ $(LDFLAGS)

mkwallpaper.o: mkwallpaper.c
	$(CC) -o $@ $(CFLAGS) -c $^

install:
	install -d -m 0755 $(DESTDIR)$(PREFIX)
	install -s -m 0755 mkwallpaper $(DESTDIR)$(PREFIX)/bin

clean:
	-rm -f *.o mkwallpaper
