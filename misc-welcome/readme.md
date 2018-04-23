# welcome

## Idea

Since I spent half a day installing simplescalar, I find it may be an interesting misc problem.

So I use simplescalar tools to compile a binary with architecture pisa. 

If you realize it is compiled by simplescalar tools and then you know you need simplescalar simulator to run it. 

But I think everyone will understanding what I mean and it will be a check-in challenge, but actually I am wrong /(ㄒoㄒ)/~~

## Solve

1. Get the binary `welcome`
2. Download `simplesim-3v0e.tgz` from [http://www.simplescalar.com](http://www.simplescalar.com)
3. Install the simplesim by following steps:
```shell
tar xvf simplesim-3v0e.tgz
cd simplesim-3.0
make config-pisa
make sim-fast #(others is also ok)
./sim-fast welcome
```
4. And you will find the flag is printed to the screen: `*ctf{we1_t0_*ctf}`