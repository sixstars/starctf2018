/*
 * main.c
 * Copyright (C) 2018 hzshang <hzshang15@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#define LENGTH 20
unsigned long value;
unsigned long *p;

int is_prime(unsigned long n) {
    if(n == 2 || n == 3 || n == 5 || n == 7)
        return 1;
    if(n == 1 || n % 2 == 0 || n % 3 == 0)
        return 0;
    unsigned long s = sqrtl(n);
    for(unsigned long i = 5; i <= s; i += 6) {
        if( n % i == 0 || n % (i + 2) == 0) {
            return 0;
        }
    }
    return 1;
}

void init() {
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stdin, 0, 2, 0);
    char *logo =
        "\n"
        " #   #    ####    #####  ######\n"
        "  # #    #    #     #    #\n"
        "### ###  #          #    #####\n"
        "  # #    #          #    #\n"
        " #   #   #    #     #    #\n"
        "          ####      #    #\n"
        "\n";
    puts(logo);
}
int main() {
    init();
    p = &value;
    scanf("%lu", p);
    if(!is_prime(value)) {
        puts("Bye");
        exit(0);
    }
    asm(
        "xor %%rax,%%rax\n\t"
        "xor %%rbx,%%rbx\n\t"
        "xor %%rcx,%%rcx\n\t"
        "xor %%rdx,%%rdx\n\t"
        "xor %%rsi,%%rsi\n\t"
        "xor %%rdi,%%rdi\n\t"
        "xor %%rsp,%%rsp\n\t"
        "xor %%rbp,%%rbp\n\t"
        "xor %%r8,%%r8\n\t"
        "xor %%r9,%%r9\n\t"
        "xor %%r10,%%r10\n\t"
        "xor %%r11,%%r11\n\t"
        "xor %%r12,%%r12\n\t"
        "xor %%r13,%%r13\n\t"
        "xor %%r14,%%r14\n\t"
        "xor %%r15,%%r15\n\t"
        "jmp *%0\n\t"
        ::"m"(p)
    );
    return 0;
}
