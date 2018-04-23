#!/usr/bin/python3

import random,sys
import socketserver
import binascii
import os
import hmac,hashlib
from hashlib import sha256
from Crypto.Util.number import isPrime
import string
from subprocess import run, PIPE, TimeoutExpired
from flag import FLAG

basedir = '/home/sixstars/*ctf/yafu/yafu-1.34'
cmd = basedir+'/yafu'
timelimit = 2

def do_init():
    url = 'https://sourceforge.net/projects/yafu/files/1.34/yafu-1.34.zip/download'
    if os.path.isfile(cmd):
        return
    os.system("wget %s -O %s/yafu-1.34.zip" % (url, basedir))
    os.system("unzip %s/yafu-1.34.zip -d %s/" % (basedir, basedir))
    os.system("chmod +x %s" % (cmd))


def do_factor(num):
    try:
        res = run(cmd, stdout=PIPE, input=('factor(%d)'%num).encode(), timeout=timelimit)
    except TimeoutExpired:
        return False
    tmp = res.stdout.decode()
    tmp = tmp[tmp.find('***factors found***\n\n')+21:].split('\n')
    for line in tmp:
        pos = line.find(' = ')
        if pos == -1:
            break
        factor = int(line[pos+3:])
        if line[0] == 'P' and not isPrime(factor):
            return True
    return False

class Task(socketserver.BaseRequestHandler):
    def proof_of_work(self):
        proof = ''.join([random.choice(string.ascii_letters+string.digits) for _ in range(20)])
        print(proof)
        digest = sha256(proof.encode('ascii')).hexdigest()
        self.request.send(str.encode("sha256(XXXX+%s) == %s\n" % (proof[4:],digest)))
        self.request.send(str.encode('Give me XXXX:'))
        x = self.request.recv(10).decode()
        x = x.strip()
        wtfpy3 = x+proof[4:]
        if len(x) != 4 or sha256(wtfpy3.encode('ascii')).hexdigest() != digest: 
            return False
        return True

    def handle(self):
        if not self.proof_of_work():
            return
        self.request.settimeout(3)
        do_init()
        self.request.sendall(str.encode("No bruteforce required. Do not DOS plz >_<\n"))
        self.request.sendall(str.encode("The number to factorize: "))
        x = self.request.recv(1024)
        try:
            x = int(x, 16)
        except:
            return
        if do_factor(x):
            self.request.sendall(str.encode("What happened? Anyway your flag here: %s\n" % FLAG))
        else:
            self.request.sendall(str.encode("Bye\n"))

class ThreadedServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass

if __name__ == "__main__":
    HOST, PORT = '0.0.0.0', 10004
    print(HOST)
    print(PORT)
    server = ThreadedServer((HOST, PORT), Task)
    server.allow_reuse_address = True
    server.serve_forever()
