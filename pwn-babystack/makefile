all: bs poc
bs: bs.c
	#gcc -fstack-protector-all -s -pthread bs.c -o bs -Wl,-z,now,-z,relro
	gcc -fstack-protector-strong -s -pthread bs.c -o bs -Wl,-z,now,-z,relro
poc: poc.c
	gcc -fstack-protector-all -s -pthread poc.c -o poc -Wl,-z,now,-z,relro
clean:
	rm poc bs
