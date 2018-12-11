#include <stdint.h>
#include <stdio.h>
#include "stif.h"

const unsigned char picture[] = {
	0xFE, 0xCA, // MAGIC
	// HEADER
	0x00, // BLOCK TYPE HEADER
	0x09, 0x00, 0x00, 0x00, // BLOCK SIZE
	// HEADER BLOCK DATA
	0x02, 0x00, 0x00, 0x00, // WIDTH
	0x02, 0x00, 0x00, 0x00, // HEIGHT
	0x00, // COLOR TYPE
	// BLOCK DATA
	0x01, // BLOCK TYPE DATA
	0x01, 0x00, 0x00, 0x00, // BLOCK SIZE
	0xFF // PIXEL
};

int main(void)
{
	stif_free(NULL);

	stif_t *s = parse_stif(picture, sizeof(picture));
	if (s == NULL)
	{
		printf("FAILED\n");
		return 1;
	}

	print_stif(s);

	stif_free(s);

	return 0;
}
