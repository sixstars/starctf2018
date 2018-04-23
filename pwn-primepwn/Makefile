#
# Makefile
# Copyright (C) 2018 hzshang <hzshang15@gmail.com>
#
# Distributed under terms of the MIT license.
PROG=primepwn
CFLAGS=-lm -fstack-protector-all  -z execstack -z now
all:main.c
	@echo "Makefile needs your attention"
	gcc $^ -o $(PROG) $(CFLAGS) 
	strip -s $(PROG)
clean:
	rm -f $(PROG)
