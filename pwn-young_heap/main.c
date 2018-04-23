/*
 * main.c
 * Copyright (C) 2018 hzshang <hzshang15@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "heap.h"
#define LENGTH 0x8

void* pool[LENGTH];

void init(void);
size_t read_str(char*,size_t);
unsigned int read_num(void);
void menu(void);
void add_heap(void);
void edit_heap(void);
void del_heap(void);



int main() {
    init();
    while(1) {
        menu();
        unsigned int opt = read_num();
        switch(opt) {
            case 1:
                add_heap();
                break;
            case 2:
                edit_heap();
                break;
            case 3:
                del_heap();
                break;
            case 4:
                exit(0);
                break;
            default:
                puts("Invalid Choice!");
                break;
        }
    }
    return 0;
}

void init() {
    char *logo =
        "\n"
        " #   #    ####   #####  ######\n"
        "  # #    #    #    #    #\n"
        "### ###  #         #    #####\n"
        "  # #    #         #    #\n"
        " #   #   #    #    #    #\n"
        "          ####     #    #\n"
        "\n";
    puts(logo);
    heap_init();
    memset(pool, 0, LENGTH * sizeof(void*));
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stdin, 0, 2, 0);
    puts("Welcome to Young Heap!");
}
size_t read_str(char *buf, size_t size) {
    size_t t = read(0, buf, size);
    if(buf[t - 1] == '\n') {
        buf[t - 1] = '\x00';
    }
    return t;
}
unsigned int read_num(void) {
    char buf[0x10];
    read_str(buf, 0x10);
    return (unsigned int )atoi(buf);
}

void menu() {
    puts("=======MENU========");
    puts("1. Malloc");
    puts("2. Edit");
    puts("3. Free");
    puts("4. Exit");
    printf(">> ");
}

void add_heap() {
    int i = 0;
    while(i < LENGTH && pool[i])
        i++;
    if(i == LENGTH) {
        puts("Pool is full.");
        return;
    }
    printf("Size :");
    size_t size = read_num();
    void *ptr = malloc(size);
    if(ptr == NULL) {
        puts("Malloc failed.");
    } else {
        pool[i] = ptr;
        printf("Content :");
        read_str(ptr, size);
        puts("Done!");
    }
}

void edit_heap() {
    printf("Index of heap :");
    unsigned int index = read_num();
    if(index >= LENGTH || pool[index] == NULL) {
        puts("No such heap.");
        return;
    }
    printf("Content :");
    size_t size = strlen(pool[index]);
    read_str(pool[index], size);
    puts("Done!");
    return;
}

void del_heap() {
    printf("Index of heap :");
    unsigned int index = read_num();
    if(index >= LENGTH || pool[index] == NULL) {
        puts("No such heap.");
        return;
    }
    free(pool[index]);
    pool[index] = NULL;
    puts("Done!");
    return;
}
