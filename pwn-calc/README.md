## idea
there is a bug when parsing input, it doesn't check whether it's a legal expression.  
so we can input like "++" to leak stack data, in this way we can get canary value, libc address and stack address.   

Then we use some illeagal input to modify stack data to construct ROP, but the input size is limited so you should make full use of the stack space.

The most difficult things that we can't get the shell because of seccomp, when program return, it will close 0 1 2, which means we can't get any output and send any payload again!  

we can use the following steps to read the flag

- call mprotect to make stack excuateable
- build a socket fd using syscall
- connect your own remote server using syscall  
- read shellcode to stack from your socket fd
- jump to shellcode, open,read,write flag to your socket fd
