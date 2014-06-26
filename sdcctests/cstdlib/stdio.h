/**
 * @file stdio.h
 * @brief Some 'poor man' routines similar to stdio
 *
 * Re-implements some of the stdio routines useful for debug messages
 * The routines are made more suitable for embedded usage
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdint.h>

typedef uint8_t size_t;

int snprintf(char *dest, size_t n, const char *fmt, void *ptr);
int strncat(char *dest, size_t n, const char *source);

#endif /*__STDIO_H__*/
