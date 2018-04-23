/*
 * main.c
 * Copyright (C) 2018 hzshang <hzshang15@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/md5.h>
#define LENGTH 0x100

FILE* note;
char ID[LENGTH];

char *md5(unsigned char *data){
    unsigned char md[16];
    char buf[33]={0};
    MD5_CTX ctx; 
    MD5_Init(&ctx);
    MD5_Update(&ctx, data, strlen(data));
    MD5_Final(md, &ctx);
    for(int i=0;i<16;i++){
        sprintf(buf+i*2,"%02X",md[i]);
    }
    return strdup(buf);
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
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stdin, 0, 2, 0);
    chdir("/tmp");
}

void menu(){
    puts("=====MENU======");
    puts("1. Edit note");
    puts("2. Show note");
    puts("3. Save note");
    puts("4. Change ID");
    puts("5. Exit");
}

char* read_file(){
    char buf[LENGTH];
    if(note){
        fseek(note,0,SEEK_SET);
        fscanf(note,"%256s",buf);
        return strdup(buf);
    }else{
        return NULL;
    }
}

void save_file(char *buf){
    if(buf){
        fseek(note,0,SEEK_SET);
        fputs(buf,note);
    }
}

char* read_str(){
    char buffer[LENGTH];
    scanf("%256s",buffer);
    return strdup(buffer);
}

void login(){
    printf("Input your ID:");
    scanf("%256s",ID);
    char *file=md5(ID);
    note=fopen(file,"r+");
    if(!note){
        note=fopen(file,"w+");
    }
}

void main() {
    asm(
        "shr $0x8,%rsp\n"
        "shl $0x8,%rsp\n"
        "xor $0x80,%rsp\n"
        "push %rbp\n"
        "mov %rsp,%rbp\n"
        "sub $0x20,%rsp\n"
    );
    init();
    char *fmt="%d";
    int opt;
    login();
    char *buffer=read_file();
    menu();
    while(1){
        printf("> ");
        if(scanf(fmt,&opt)<=0){
            exit(0);
        };
        switch(opt){
            case 1:
                printf("Note:");
                buffer=read_str();
                break;
            case 2:
                printf("Note:%s\n",buffer);
                break;
            case 3:
                save_file(buffer);
                puts("Saved!");
                break;
            case 4:
                fclose(note);
                unlink(md5(ID));
                login();
                puts("Done!");
                break;
            case 5:
                if(note)
                    fclose(note);
                exit(0);
                break;
            default:
                puts("Invalid choice");
                break;
        }
    }
}

