# refer to http://www.mersenneforum.org/showthread.php?s=2dd8c3a71bd4fee05762e3860541e67c&t=23281 for more details
from pwn import *
import string
from hashlib import sha256

context.log_level='debug'
def dopow():
    chal = c.recvline()
    post = chal[12:28]
    tar = chal[33:-1]
    c.recvuntil(':')
    found = iters.bruteforce(lambda x:sha256(x+post).hexdigest()==tar, string.ascii_letters+string.digits, 4)
    c.sendline(found)

#c = remote('127.0.0.1',10001)
c = remote('47.75.4.252',10004)
dopow()
num='286b7fb2af5f5d27d216771c90ac6f43a9892a690c48e4b06bcbc1cd'
c.sendlineafter('factorize:',num)
print c.recv()
c.interactive()
