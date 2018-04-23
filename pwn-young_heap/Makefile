#
# Makefile

CFLAGS=-fstack-protector
LDFLAGS=-L. -lheap -Wl,-R -Wl,./
PROG=young_heap

all:main.c libheap.so
	gcc $< $(CFLAGS) $(LDFLAGS) -o $(PROG)
	strip -s $(PROG)

debug:main.c libheap.so
	gcc $< $(CFLAGS) $(LDFLAGS) -o $(PROG) -g

libheap.so:heap.c heap.h
	gcc $< $(CFLAGS) -fPIC -shared -o $@
	strip -s $@

install:$(PROG) libheap.so
	cp $^ ../share

clean:
	rm -f libheap.so $(PROG)


