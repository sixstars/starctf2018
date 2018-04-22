P = warmup
CFLAGS = -Wall -fno-stack-protector -Wl,-z,now,-z,relro -s

$(P): 

poc: poc.c
	gcc poc.c -g3 -ggdb3 -o poc

clean: 
	rm warmup
