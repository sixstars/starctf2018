// gcc urlparse.c -o urlparse -Wl,-s,-z,relro,-z,now -fPIE -pie

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define min(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

struct URLList {
    struct URLList *next;
    char url[];
};

struct URLList *g_head;

unsigned int total_size = 0;
char g_buf[0x100000];

void print_logo() {
    puts("");
    puts(" #   #    ####    #####  ######");
    puts("  # #    #    #     #    #");
    puts("### ###  #          #    #####");
    puts("  # #    #          #    #");
    puts(" #   #   #    #     #    #");
    puts("          ####      #    #");
    puts("");
}

static inline bool is_hex(char c) {
    return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}

static inline char to_hex(unsigned char c) {
    if (c <= 9) {
        return c + '0';
    }
    if (c < 16) {
        return c - 10 + 'A';
    }
    exit(0);
}

char *url_encode(char *text) {
    char *cp, *xp;
    unsigned int size;

    for (cp = text, xp = g_buf; *cp; cp++) {
        *(xp++) = '%';
        *(xp++) = to_hex(*cp / 16);
        *(xp++) = to_hex(*cp % 16);
    }
    strncpy(text, g_buf, strlen(text));
    return text;
}

char *url_decode(char *text) {
    char *cp, *xp;

    for (cp = text, xp = text; *cp; cp++) {
        if (*cp == '%') {
            if (is_hex(*(cp + 1)) && is_hex(*(cp + 2))) {
                if (islower(*(cp + 1)))
                    *(cp + 1) = toupper(*(cp + 1));
                if (islower(*(cp + 2)))
                    *(cp + 2) = toupper(*(cp + 2));
                *(xp) = (*(cp + 1) >= 'A' ? *(cp + 1) - 'A' + 10 : *(cp + 1) - '0') * 16 + (*(cp + 2) >= 'A' ? *(cp + 2) - 'A' + 10 : *(cp + 2) - '0');
                xp++;
            }
            cp += 2;
        } else {
            *(xp++) = *cp;
        }
    }

    xp = min(xp, text + strlen(text));
    memset(xp, 0, cp - xp);
    return text;
}

void print_menu(void) {
    puts("===== Menu =====");
    puts("1: create url");
    puts("2: encode url");
    puts("3: decode url");
    puts("4: list urls");
    puts("5: delete url");
    puts("6: exit");
    puts("================\n");
}

int get_integer(void) {
    int ret = 0;
    char buf[16];
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%d", &ret);
    return ret;
}

void create_url(void) {
    unsigned int size;
    struct URLList *node;

    printf("size: ");
    size = get_integer();
    if (size + sizeof(struct URLList) < size) {
        puts("Error: invalid size");
        exit(0);
    }
    if (size < 0x80) {
        puts("Error: too small");
        exit(0);
    }
    if (size > 0x11111) {
        puts("Error: too large");
        exit(0);
    }
    total_size += size;
    if (total_size > 222222) {
        puts("Error: too large (total)");
        exit(0);
    }

    node = malloc(sizeof(struct URLList) + size);
    if (!node) {
        puts("Error: malloc failed");
        exit(0);
    }

    printf("URL: ");
    memset(g_buf, 0, sizeof(g_buf));
    fgets(g_buf, size, stdin);
    url_decode(g_buf);
    if (strlen(g_buf) > 0x400) {
        printf("As we need to pay for the data volume, we refuse long input!");
        exit(-1);
    }
    size = min(size - 1, strlen(g_buf));
    strncpy(node->url, g_buf, size);
    node->url[size] = '\0';
    node->next = g_head;
    g_head = node;
}

void encode_url(void) {
    int idx;

    printf("index: ");
    idx = get_integer();

    struct URLList *node = g_head;
    while (idx--) {
        if (node == NULL)
            break;
        node = node->next;
    }

    if (node == NULL) {
        puts("Error: no such url");
        exit(0);
    }

    url_encode(node->url);
}

void decode_url(void) {
    int idx;

    printf("index: ");
    idx = get_integer();

    struct URLList *node = g_head;
    while (idx--) {
        if (node == NULL)
            break;
        node = node->next;
    }

    if (node == NULL) {
        puts("Error: no such url");
        exit(0);
    }

    url_decode(node->url);
}

void list_urls(void) {
    int idx = 0;
    struct URLList *node = g_head;

    puts("LIST START");
    while (node) {
        printf("%d: %s\n", idx, node->url);
        node = node->next;
        idx++;
    }
    puts("LIST END");
}

void delete_url(void) {
    int idx;

    printf("index: ");
    idx = get_integer();

    struct URLList *prev = NULL;
    struct URLList *node = g_head;
    while (idx--) {
        if (node == NULL)
            break;
        prev = node;
        node = node->next;
    }

    if (node == NULL) {
        puts("Error: no such url");
        exit(0);
    }

    if (prev == NULL) {
        g_head = node->next;
    } else {
        prev->next = node->next;
    }

    free(node);
}

int main() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    print_logo();

    while (1) {
        int cmd;

        print_menu();
        printf("> ");
        cmd = get_integer();

        if (cmd == 1) {
            create_url();
        } else if (cmd == 2) {
            encode_url();
        } else if (cmd == 3) {
            decode_url();
        } else if (cmd == 4) {
            list_urls();
        } else if (cmd == 5) {
            delete_url();
        } else if (cmd == 6) {
            exit(0);
        } else {
            puts("Error: undefined command");
        }
    }
    return 0;
}
