
INSTALL_PATH = /usr/local/bin

all:
	gcc -Wall -O3 -o h h.c
	cp h hfind

install:
	install h $(INSTALL_PATH)
	install hfind $(INSTALL_PATH)

