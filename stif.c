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
	stif_block_t *block = NULL;

	block = malloc(sizeof(*block));

	// GET THE TYPE
	block->block_type = (int8_t)*(buffer + *bytes_read);
	*bytes_read += 1;

	// GET THE SIZE
	block->block_size = *(int32_t *)(buffer + *bytes_read);
	*bytes_read += 4;

	// NOT ENOUGH BYTES TO READ
	if (*bytes_read + (size_t)block->block_size > buffer_size)
		return NULL;

	// GET THE DATA
	block->data = malloc((size_t)block->block_size);
	memcpy(block->data, buffer + *bytes_read, (size_t)block->block_size);

	// INIT NEXT
	block->next = NULL;

	return block;
}

void stif_block_free(stif_block_t *b)
{
	printf("TODO");
}

static int parse_stif_header(stif_header_t *header, stif_block_t *block)
{
	const unsigned char *current = NULL;

	if (block->block_type != STIF_BLOCK_TYPE_HEADER)
		return -1;
	if (block->block_size != 9)
		return -1; // sizeof(stif_header_t) without padding

	current = block->data;
	header->width = *(int32_t *)current;
	current += 4;
	header->height = *(int32_t *)current;
	current += 4;

	switch(*current)
	{
		case STIF_COLOR_TYPE_GRAYSCALE:
			header->color_type = STIF_COLOR_TYPE_GRAYSCALE;
			break;
		case STIF_COLOR_TYPE_RGB:
			header->color_type = STIF_COLOR_TYPE_RGB;
			break;
		default:
			return -1;
			break;
	}
	return 0;
}

stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size)
{
	size_t read = 0;
	stif_t *stif = NULL;

	if (buffer_size < 2) // STIF_MAGIC
		return NULL;

	if ( ( *(int16_t *) buffer) != (int16_t) STIF_MAGIC)
		return NULL;
	read += 2;

	stif = malloc(sizeof(*stif));
	stif->block_head = read_stif_block(buffer, buffer_size, &read);

	if (parse_stif_header(&(stif->header), stif->block_head))
		goto error;

	// malloc the pixels array with a size appropriate for the type
	if (stif->header.color_type == STIF_COLOR_TYPE_GRAYSCALE)
	{
		size_t size = (size_t)(stif->header.width * stif->header.height);
		stif->grayscale_pixels = malloc(size * sizeof(pixel_grayscale_t));
	}
	else if (stif->header.color_type == STIF_COLOR_TYPE_RGB)
	{
		size_t size = (size_t)(stif->header.width * stif->header.height);
		stif->rgb_pixels = malloc(size * sizeof(pixel_rgb_t));
	}
	else
		goto error;

	return stif;

	for (stif_block_t *block = stif->block_head;
			read < buffer_size;
			block = block->next)
	{
		block->next = read_stif_block(buffer, buffer_size, &read);
		if (block->next == NULL)
			goto error;
	}

	return stif;

error:
	fprintf(stderr, "ERROR\n");
	if (stif == NULL)
		return NULL;

	for (stif_block_t *block = stif->block_head, *next = NULL;
			block != NULL;
			block = next)
	{
		next = block->next;
		stif_block_free(block);
	}

	if (stif->grayscale_pixels != NULL)
		free(stif->grayscale_pixels);
	if (stif->rgb_pixels != NULL)
		free(stif->rgb_pixels);
	free(stif);
	return NULL;
}

void print_stif(stif_t *s)
{
	printf("==== PRINT STIF ====\n");
	printf("== Header ==\n");
	printf("width => %d\n", s->header.width);
	printf("heigth => %d\n", s->header.height);
	printf("color_type => %d\n", s->header.color_type);
}



