#
# Makefile
# Copyright (C) 2018 hzshang <hzshang15@gmail.com>
#
# Distributed under terms of the MIT license.

PROG=note
CFLAGS=-lcrypto -fno-stack-protector -z now
all:main.c
	gcc $^ $(CFLAGS) -o $(PROG)
	strip -s $(PROG)

debug:main.c
	gcc $^ $(CFLAGS) -o $(PROG) -g
clean:
	rm -rf $(PROG)
