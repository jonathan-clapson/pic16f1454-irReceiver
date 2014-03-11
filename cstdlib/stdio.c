#include "stdio.h"
#include "../uart/uart.h"

#define MAX_NUM_SIZE 15

int strncat(char *dest, size_t n, const char *src)
{
	uint16_t i;
	/* get to end of dest */
	for (i=0; i<n; i++) {
		if (dest[i] == '\0') break;
	}
	/* copy src to end of dest */
	while (*src) {
		dest[i] = *src;
		i++;
		src++;
	}
	return 0;
}

int snprintf(char *dest, size_t n, const char *fmt, char *ptr)
{
	int16_t d;
	uint16_t u;
	int dest_offset = 0;
	char num_buffer[MAX_NUM_SIZE];
	char num_offset = 0;

	while (*fmt != '\0') {
		if (*fmt == '%') {
			fmt++;
			switch(*fmt) {
			case 'c':
				dest[dest_offset++] = *ptr;
				break;
			case 's':
				while(*ptr) dest[dest_offset++] = *ptr++;
				break;
			case 'd':
				d = (int16_t) *ptr;
				/* handle negative */
				if (d < 0) {
					d = -d;
					dest[dest_offset] = '-';
					dest_offset++;
				}
				do {
					/* store digits from lowest first */
					num_buffer[num_offset] = '0' + d%10;
					num_offset++;
					d/=10;
				} while(d);

				/* check what we're about to copy won't exceed buffer */
				if (dest_offset+num_offset>n-1) {
					uart_puts("#ov");
					dest[dest_offset] = '\0';
					return -1;
				}
				num_offset--;
				for (; num_offset >= 0; num_offset--){
					dest[dest_offset] = num_buffer[num_offset];
					dest_offset++;
				}
				break;
			case 'u':
				u = (uint16_t) *ptr;
				do {
					/* store digits from lowest first */
					num_buffer[num_offset] = '0' + u%10;
					num_offset++;
					u/=10;
				} while(u);

				/* check what we're about to copy won't exceed buffer */
				if (dest_offset+num_offset>n-1) {
					uart_puts("#ov");
					dest[dest_offset] = '\0';
					return -1;
				}
				num_offset--;
				for (; num_offset >= 0; num_offset--){
					dest[dest_offset] = num_buffer[num_offset];
					dest_offset++;
				}
				break;
			}
			fmt++;
		} else { /* *fmt is not % */
			dest[dest_offset] = *fmt;
			*fmt++;
			dest_offset++;
		}

		/* check theres still room for adding stuff */
		if (dest_offset>n-1) {
			dest[dest_offset]='/0';
			return -1;
		}
	}

	/* terminate string */
	dest[dest_offset] = '\0';
	return 0;
}
