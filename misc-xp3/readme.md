# xp3

## Idea

I find the kirikiri has some interesting data format and there are some many tools to deal with the special data format such as `.xp3`, `.tlg` and so on... So I think it will be an interesting challenge about `.xp3` file.

## Writeup

1. First, use `crass` and you will get many files from `misc.xp3`. I hope you can find there is a hint in `g01.ks`: `the “Flag” is out there ―― there is a secret in the script and pictures`.
2. Then, you may find `504B0304` is at the end of `g01.ks`, and other `.ks` files also have some strange text like this. If you are familiar with `zip` format, you will understand there is a zip file in scripts.
3. It's easy to write a python script to get the zip file from `g01.ks` ~ `g55.ks`. You will find it is encrypted and you need a key.
4. You will find the `pic25.tlg` is `237KB` while others are `50KB`
5. Use `tlg2png` and make `.tlg` => `.png`. You only need to get `pic25.tlg.png` and one of others such as `pic2.tlg.png`.
6. Useing [BlindWaterMark Tool](https://github.com/chishaxie/BlindWaterMark) and you will find the key `Key: NoGalgameNoLife`
7. With the key, you can get flag from zip: `*ctf{Hope_Every0ne_Has_A_Happy_End2333}`

## Script

```python
import codecs
from pwn import *
with open('./out.zip', 'w') as f1:
    for i in range(1, 56):
        ks = './g%02d.ks' % (i)
        f2 = codecs.open(ks, 'r', 'utf16')
        line = f2.readlines()[-1]
        data = line[3::]
        f1.write(p32(int(data, 16))[::-1])
        f2.close()
```

## And so on...

Actually you don't need to care about what the text is but someone has read it(lll￢ω￢)

The text is useless since I just want to use long text to make you confused.

This challenge can be solved by basic tool such as `crass`, you don't need to find out what the game is and whether you need a special tool.

And, as the flag mentioned, I hope everyone has a happy end~~