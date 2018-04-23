#include <asm/unistd.h>
#include <errno.h>
#include <linux/audit.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <unistd.h>
#define LENGTH 0x18
#define BUF_SIZE 0x18

//BPF system call filter.
const struct sock_filter execve_filter[] = {
    // Load arch number
    // BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, arch))),
    // Jump forward 4 instructions if architecture does not match x86
    // BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_I386, 0, 3), this bug is too easy
    // Load system call number.
    BPF_STMT(BPF_LD | BPF_ABS, offsetof(struct seccomp_data, nr)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_execve, 2, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_execveat, 1, 0),
    // Permit this system call.
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
    // Reject this system call.
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),
};

const struct sock_fprog execve_filter_program = {
    .len = sizeof(execve_filter) / (sizeof(execve_filter[0])),
    .filter = (struct sock_filter*)execve_filter
};

// Install the seccomp filter.
static void disable_execve(void) {
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0) {
        abort();
    }
    if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &execve_filter_program) != 0) {
        abort();
    }
}

typedef struct {
    int top;
    int pool[LENGTH];
} stack;
int myatoi(char* buf) {
    int sum = 0;
    while (*buf) {
        sum = sum * 10 + (*buf - '0');
        buf++;
    }
    return sum;
}
int read_str(char* buf, int n) {
    int t = read(0, buf, n-1);
    if (buf[t - 1] == '\n') {
        buf[t - 1] = '\x00';
    }
    buf[t]='\x00';
    return t;
}
int input_pass(char* buf, int size) {
    int ret = 1;
    for (int i = 0; i < size; i++) {
        char c = buf[i];
        if (!((c >= '0' && c <= '9') || c == '+' || c == '-' || c == '*' || c == '/' || c == '\x00' || c == '(' || c == ')')) {
            ret = 0;
            break;
        }
    }
    return ret;
}
void push(stack* s, int value) {
    if (s->top >= 0) {
        s->top--;
        s->pool[s->top] = value;
    }
}
int priority(int a) {
    int ret;
    switch (a) {
        case '(':
            ret = 5;
            break;
        case '*':
        case '/':
            ret = 4;
            break;
        case '+':
        case '-':
            ret = 3;
            break;
        case ')':
            ret=2;
            break;
        case 0:
            ret = 1;
            break;
        case -1:
            ret = 0;
            break;
    }
    return ret;
}
int deal_data(stack* array, stack* opt) {
    char c = opt->pool[opt->top];
    opt->top++;
    int a = array->pool[array->top];
    array->top++;
    int b = array->pool[array->top];
    switch (c) {
        case '+':
            array->pool[array->top] = a + b;
            break;
        case '-':
            array->pool[array->top] = b - a;
            break;
        case '*':
            array->pool[array->top] = a * b;
            break;
        case '/':
            if (a != 0)
                array->pool[array->top] = b / a;
            else
                array->pool[array->top] = 0xFFFFFFFF;
            break;
        default:
            break;
    }
}
void init() {
    disable_execve();
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stdin, 0, 2, 0);
    char* logo = "\n"
                 " #   #    ####    #####  ######\n"
                 "  # #    #    #     #    #\n"
                 "### ###  #          #    #####\n"
                 "  # #    #          #    #\n"
                 " #   #   #    #     #    #\n"
                 "          ####      #    #\n"
                 "\n";
    puts(logo);
}
int main(int argc, char const* argv[]) {
    init();
    struct {
        char dest[BUF_SIZE];
        char buf[BUF_SIZE];
        stack opt;
        stack array;
    } var;
    // loop
    while (1) {
        // init
        char *temp, *str;
        var.array.top = LENGTH;
        var.opt.top = LENGTH;
        push(&var.opt, 0xFFFFFFFF);

        int size = read_str(var.buf, BUF_SIZE);
        if (!input_pass(var.buf, size)) {
            printf("input illegal!\n");
            break;
        }
        // calc
        str = var.buf;
        while (str - var.buf < size) {
            temp = var.dest;
            while (str - var.buf < size && *str >= '0' && *str <= '9') {
                *temp = *str;
                temp++;
                str++;
            }
            *temp = '\x00';
            if (*var.dest) { // bug
                int num = myatoi(var.dest);
                push(&var.array, num);
            }
            while (1) {
                if ( var.opt.pool[var.opt.top] == '(' || priority(var.opt.pool[var.opt.top]) < priority((int)*str)) {
                    if(*str != ')')                    
                        push(&var.opt, (int)*str);
                    else
                        var.opt.top++;
                    break;
                } else {
                    deal_data(&var.array, &var.opt);
                }
            }
            if (*str == '\x00') {
                break;
            }
            str++;
        }
        printf("%s = %d\n", var.buf, var.array.pool[var.array.top]);
    }
    close(0);
    close(1);
    close(2);
    return 0;
}
