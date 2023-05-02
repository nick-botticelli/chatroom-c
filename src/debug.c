#include "debug.h"

#include <stdlib.h>

#include "main.h"

void debug_hexdump(void *data, size_t size) {
#ifndef NDEBUG
	char ascii[17];
	ascii[16] = '\0';

	for (size_t i = 0; i < size; i++) {
		printf("%02X ", ((unsigned char *) data)[i]);

		if (((unsigned char *) data)[i] >= ' ' && ((unsigned char *) data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char *) data)[i];
		} else {
			ascii[i % 16] = '.';
		}

		if ((i + 1) % 8 == 0 || i + 1 == size) {
			printf(" ");

			if ((i + 1) % 16 == 0) {
				printf("|  %s \n", ascii);
			}
            else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';

				if ((i + 1) % 16 <= 8) {
					printf(" ");
				}

				for (size_t j = (i + 1) % 16; j < 16; j++) {
					printf("   ");
				}

				printf("|  %s \n", ascii);
			}
		}
	}
#endif
}
