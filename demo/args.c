#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG(fmt, ...) \
    printf("[%s-%d]" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// c语言不定参的使用
void printNum(int count, ...) {
    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++) {
        int num = va_arg(ap, int);
        printf("params[%d]:%d\n", i, num);
    }
    va_end(ap);
}

void myprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *res;
    int ret = vasprintf(&res, fmt, ap);
    if (ret != -1) {
        printf((const char *)res);
        printf("\n");
        free(res);
    }
    va_end(ap);
}

int main() {
    // LOG("hello");
    // LOG("hello, %s", "123");
    printNum(5, 5, 4, 3, 2, 1);
    myprintf("hello %d", 123);
    myprintf("hello world");
    return 0;
}
