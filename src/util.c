#include "kernel.h"

void* memset(void* dest, int value, size_t count) {
    u8* ptr = (u8*)dest;
    for (size_t i = 0; i < count; i++) {
        ptr[i] = (u8)value;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    u8* d = (u8*)dest;
    const u8* s = (const u8*)src;
    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }
    return dest;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(u8*)s1 - *(u8*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(u8*)s1 - *(u8*)s2;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}
