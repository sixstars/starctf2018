#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void welcome() {
	puts("");
	puts(" #   #    ####    #####  ######");
	puts("  # #    #    #     #    #");
	puts("### ###  #          #    #####");
	puts("  # #    #          #    #");
	puts(" #   #   #    #     #    #");
	puts("          ####      #    #");
	puts("");
    puts("Let's warmup now!");
    return;
}

void info(int fd, char log[]) {
    write(fd, log, strlen(log));
}

char *bak = NULL;
int main () {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    size_t leak;
    char name[8];
    char *canary = NULL;
    welcome();
    bak = (char*)malloc(8);
    canary = bak;
    memset(name, 0, 8);

    puts("What are you looking for?");
    scanf("%zu", &leak);
    printf("%#zx\n", *(size_t *)leak);

    puts("What's your name?");
    scanf("%s", name);
    
    if (canary == bak) {
        puts("Bye bye.");
        return 0;
    } else {
        int fd = open("/dev/tty", O_RDWR);
        if (fd != -1) {
            info(fd, "[INFO] A hacker is coming!\n");
            info(fd, "[INFO] Exiting...\n");
            _exit(1);
        } else {
            puts("Something is broken!");
            return 0;
        }
    }
} 
