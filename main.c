#include <stdint.h>
#include <stdio.h>
#include "stif.h"

const unsigned char picture[] = {
	0xFE, 0xCA, // MAGIC

	// HEADER
	10, 0, 0, 0, // WIDTH
	15, 0, 0, 0, // HEIGHT
	STIF_COLOR_TYPE_RGB, // COLOR TYPE
	0, 0, 0 // stif_header padding
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

	return 0;
}
