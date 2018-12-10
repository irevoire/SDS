#include <stdint.h>
#include <stdio.h>
#include "stif.h"

const unsigned char picture[] = {0xCA, 0xFE, 10, 15, STIF_COLOR_TYPE_GRAYSCALE, 0, 0, 0, 0};

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
