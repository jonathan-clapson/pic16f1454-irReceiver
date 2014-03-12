#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdint.h>

typedef uint8_t size_t;

int snprintf(char *dest, size_t n, const char *fmt, void *ptr);
int strncat(char *dest, size_t n, const char *source);

#endif /*__STDIO_H__*/
