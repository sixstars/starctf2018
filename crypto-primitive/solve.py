from pwn import *
import string
from hashlib import sha256

#context.log_level='debug'
def dopow():
    chal = c.recvline()
    post = chal[12:28]
    tar = chal[33:-1]
    c.recvuntil(':')
    found = iters.bruteforce(lambda x:sha256(x+post).hexdigest()==tar, string.ascii_letters+string.digits, 4)
    c.sendline(found)

#c = remote('127.0.0.1',10001)
c = remote('47.75.4.252',10001)
dopow()
pt='GoodCipher'

def doswap(a,b):
    if a==b:
        return
    if a>b:
        tmp=b
        b=a
        a=tmp
    ans=[]
    ans.append((0,256-a))
    b-=a
    a=0
    while b!=1:
        tmp=0
        lo=1
        while b&lo==0:
            lo<<=1
            tmp+=1
        if b==lo:
            ans.append((1,8-tmp))
            break
        if tmp!=0:
            ans.append((1,8-tmp))
        b>>=tmp
        ans.append((2,1))
        b^=1
        ans.append((0,255))
        b-=1
    ans.append((0,254))

    for a,b in ans:
        c.sendline('%d %d'%(a,b))
        c.recvline()
    for a,b in [(0,2),(1,7),(0,255),(1,1)]:
        c.sendline('%d %d'%(a,b))
        c.recvline()
    for a,b in ans[::-1]:
        if a==0:
            c.sendline('%d %d'%(a,256-b))
        elif a==1:
            c.sendline('%d %d'%(a,8-b))
        elif a==2:
            c.sendline('%d %d'%(a,b))
        c.recvline()

for i in range(3):
    print i
    m=range(256)
    c.recvuntil('ciphertext is ')
    ct=c.recvline().strip()
    ct=ct.decode('hex')
    assert len(ct)==10
    for i in range(10):
        a=ord(ct[i])
        b=ord(pt[i])
        #print m[a],b
        doswap(m[a],b)
        for j in range(256):
            if m[j]==b:
                m[j]=m[a]
                m[a]=b
                break
    c.sendline('-1')

c.recvuntil('Your flag here.\n')
print c.recvline()
