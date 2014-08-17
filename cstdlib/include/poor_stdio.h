/**
 * @file poor_stdio.h
 * @brief Some 'poor man' routines similar to stdio
 *
 * Re-implements some of the stdio routines useful for debug messages
 * The routines are made more suitable for embedded usage
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#ifndef __POOR_STDIO_H__
#define __POOR_STDIO_H__

#include <stdint.h>

typedef unsigned size_t;

int8_t poor_snprintf(char *dest, size_t n, const char *fmt, void *ptr);

// This conflicts with microchip definition, shouldn't be in this file anyway
//int strncat(char *dest, size_t n, const char *source);

#endif /*__POOR_STDIO_H__*/
