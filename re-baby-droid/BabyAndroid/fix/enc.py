#!/usr/bin/env python
# coding:utf-8

from pwn import *

msg = '1nTr3stIng-AnDr0id-A&D!'
key = 0x6674632a ** 2
for i in xrange(8):
    t = u64(msg[i:i + 8])
    t ^= key
    msg = msg[:i] + p64(t) + msg[i + 8:]

msg = msg[:-8] + p64(u64(msg[-8:]) ^ key)
print repr(msg)
