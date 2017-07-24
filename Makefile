CC=gcc
INSTALL_PREFIX=/usr/local/bin
TARGET=lcs

all:
	gcc -Wall -Wextra -O2 -o $(TARGET) lcs.c

debug:
	gcc -I/usr/local/include -Wall -Wextra -Wno-unused -ggdb3 -g3 -fno-stack-protector -fsanitize=address -fbuiltin -O0 -o $(TARGET) lcs.c

install: all
	cp lcs $(INSTALL_PREFIX)/lcs
	chmod +x $(INSTALL_PREFIX)/lcs

uninstall:
	rm -f $(INSTALL_PREFIX)/lcs

