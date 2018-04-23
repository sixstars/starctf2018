//gcc -fno-stack-protector -o stack stack.c
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char *s;
unsigned char buf[2];
int i;

uint8_t boom(int size) {
    s = alloca(size+8);
    i=0;
    while (1) {
        if (read(0, buf, 1)!=1)
            break;
        if (buf[0]=='\n')
            break;
        if (buf[0]<32) {
            puts("not printable!");
            exit(0);
        }
        s[i]=buf[0];
        i++;
    }
    if (i==0)
        return 255;
    if (i>size) {
        puts("OVERFLOW!");
        return 0;
    } else {
        puts("no overflow");
        return 1;
    }
}


int main() {
    uint8_t size;
    uint64_t tmp;
    uint64_t cnt1=0;
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("This is a demo for overflow checking.\nTo avoid pwning, you cannot use unprintable characters.\n");
    printf("size: ");
    scanf("%hhu",&size);
    while (1) {
        printf("content: ");
        tmp = boom(size);
        if (tmp==0)
            cnt1++;
        if (tmp>1)
            break;
    }
    printf("overflow count: %lu\n", cnt1);
    return 0;
}
