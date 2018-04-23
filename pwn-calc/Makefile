CFLAG = -m32 -fstack-protector-all -D_FORTIFY_SOURCE=2  -z now -fPIE -pie -w
PROG=calc

all: calc.c
	gcc $< $(CFLAG) -o $(PROG)
	strip -s $(PROG)


debug: calc.c
	gcc $< $(CFLAG) -g -o $(PROG)

install: $(PROG) libc.so.6
	cp $^ ../share

clean: 
	rm -f $(PROG)
