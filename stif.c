#include "stif.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>


void stif_free(stif_t *s)
{
	if (s == NULL)
		return;
	free(s);
}

stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read)
{
	(void *) buffer;
	(void *) buffer_size;
	(void *) bytes_read;
	printf("TODO");
	return NULL;
}

void stif_block_free(stif_block_t *b)
{
	printf("TODO");
}

stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size)
{
	if (buffer_size < sizeof(stif_header_t) + 2) // STIF_MAGIC
		return NULL;

	if ( ( *(int16_t *) buffer) != (int16_t) STIF_MAGIC)
		return NULL;
	buffer += 2;

	stif_t *stif = malloc(sizeof(*stif));
	memcpy(&(stif->header), buffer, sizeof(stif->header));

	return stif;
}

void print_stif(stif_t *s)
{
	printf("==== PRINT STIF ====\n");
	printf("== Header ==\n");
	printf("width => %d\n", s->header.width);
	printf("heigth => %d\n", s->header.height);
	printf("color_type => %d\n", s->header.color_type);
}



