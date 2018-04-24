#!/usr/bin/env python
# coding:utf-8

import os
import shutil
import commands

from pwn import *

from forcecrc32 import fix_crc32

key = u32('*ctf')

if os.path.exists('app'):
    shutil.rmtree('app')
os.system('apktool d ../app/release/app-release.apk -o app')

elf = ELF('app/lib/armeabi-v7a/libb.so')
jni_onload = elf.symbols['JNI_OnLoad'] & ~1
log.info('jni onload: %#x', jni_onload)

# patch from JNI_OnLoad end to init func
begin_addr = elf.search('loaded').next() + 12
# elf.write(elf.search('loaded').next(), '\0' * 6)
# for sec in elf.sections:
#     if sec.name == '.text':
#         break
# begin_addr = sec.header['sh_offset']
end_addr = u32(elf.section('.init_array')[:4]) & (~1)
log.info('encrypt from %#x to %#x', begin_addr, end_addr)
begin_addr -= jni_onload
end_addr -= jni_onload

for i in xrange(begin_addr, end_addr, 4):
    addr = jni_onload + i
    data = u32(elf.read(addr, 4))
    data ^= key
    data = p32(data)
    elf.write(addr, data[2:])
    elf.write(addr + 2, data[:2])

begin_ptr = elf.search(p32(0xdead0002)).next()
end_ptr = elf.search(p32(0xdeadff02)).next()
elf.write(begin_ptr, p32(begin_addr))
elf.write(end_ptr, p32(end_addr))

# patch JNI_OnLoad start to init func
begin_addr = jni_onload
end_addr = u32(elf.section('.init_array')[:4]) & (~1)
log.info('encrypt from %#x to %#x', begin_addr, end_addr)
begin_addr -= jni_onload
end_addr -= jni_onload

for i in xrange(begin_addr, end_addr, 4):
    addr = jni_onload + i
    data = u32(elf.read(addr, 4))
    data ^= key
    elf.write(addr, p32(data))

begin_ptr = elf.search(p32(0xdead0001)).next()
end_ptr = elf.search(p32(0xdeadff01)).next()
elf.write(begin_ptr, p32(begin_addr))
elf.write(end_ptr, p32(end_addr))

elf.save('./libb.so')
tpl = 'dd if=./libb.so of=app/lib/armeabi-v7a/libb.so bs=1 count={:d}'
os.system(tpl.format(elf.header.e_shoff))
fix_crc32('app/lib/armeabi-v7a/liba.so', key)
fix_crc32('app/lib/armeabi-v7a/libb.so', key)

os.system('apktool b app')

cmd = 'jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore ~/common/debug.keystore -signedjar signed.apk app/dist/app-release.apk debug'
os.system("echo $KEYSTORE_PASSPHRASE | {}".format(cmd))
os.system('zipalign -f -v 4 signed.apk aligned.apk')
