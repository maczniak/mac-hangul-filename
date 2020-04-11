
INSTALL_PATH = /usr/local/bin

all: h.h h.c zip.c
	gcc -Wall -O3 -o h h.c zip.c
	cp h hfind

install:
	install h $(INSTALL_PATH)
	install hfind $(INSTALL_PATH)

