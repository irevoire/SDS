#include "stif.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void free_all_blocks(stif_block_t *block)
{
	if (block == NULL)
		return;

	free_all_blocks(block->next);
	stif_block_free(block);
}

void stif_free(stif_t *s)
{
	if (s == NULL)
		return;
	free_all_blocks(s->block_head);
	if (s->grayscale_pixels != NULL)
		free(s->grayscale_pixels);
	if (s->rgb_pixels != NULL)
		free(s->rgb_pixels);

	free(s);
}

stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read)
{
	stif_block_t *block = NULL;

	// NOT ENOUGH BYTES TO READ -- 5 => sizeof(stif_block_t) without padding
	if (*bytes_read + 5 > buffer_size)
		return NULL;

	block = malloc(sizeof(*block));

	// GET THE TYPE
	block->block_type = (int8_t)*(buffer + *bytes_read);
	*bytes_read += 1;

	// GET THE SIZE
	block->block_size = *(int32_t *)(buffer + *bytes_read);
	*bytes_read += 4;

	// NOT ENOUGH BYTES TO READ
	if (*bytes_read + (size_t)block->block_size > buffer_size)
	{
		free(block);
		return NULL;
	}

	// GET THE DATA
	block->data = malloc((size_t)block->block_size);
	memcpy(block->data, buffer + *bytes_read, (size_t)block->block_size);
	*bytes_read += (size_t)block->block_size;

	// INIT NEXT
	block->next = NULL;

	return block;
}

void stif_block_free(stif_block_t *b)
{
	if (b == NULL)
		return;
	if (b->data != NULL)
		free(b->data);
	free(b);
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
	unsigned char *pixel_progression = NULL;
	size_t pixel_read = 0, image_size = 0;
	stif_t *stif = NULL;

	if (buffer_size < 2) // STIF_MAGIC
		return NULL;

	if ( ( *(int16_t *) buffer) != (int16_t) STIF_MAGIC)
		return NULL;
	read += 2;

	stif = malloc(sizeof(*stif));
	stif->block_head = NULL;
	stif->grayscale_pixels = NULL;
	stif->rgb_pixels = NULL;

	stif->block_head = read_stif_block(buffer, buffer_size, &read);
	if (stif->block_head == NULL)
		goto error;

	if (parse_stif_header(&(stif->header), stif->block_head))
		goto error;


	// malloc the pixels array with a size appropriate for the type
	if (stif->header.color_type == STIF_COLOR_TYPE_GRAYSCALE)
	{
		image_size = (size_t)(stif->header.width * stif->header.height) * sizeof(pixel_grayscale_t);
		stif->grayscale_pixels = malloc(image_size);
		pixel_progression = stif->grayscale_pixels;
	}
	else if (stif->header.color_type == STIF_COLOR_TYPE_RGB)
	{
		image_size = (size_t)(stif->header.width * stif->header.height) * sizeof(pixel_rgb_t);
		stif->rgb_pixels = malloc(image_size);
		pixel_progression = (unsigned char *)stif->rgb_pixels;
	}
	else
		goto error;

	for (stif_block_t *block = stif->block_head;
			read < buffer_size;
			block = block->next)
	{
		block->next = read_stif_block(buffer, buffer_size, &read);
		if (block->next == NULL)
			goto error;
		if (block->next->block_type == STIF_BLOCK_TYPE_DATA)
		{
			memcpy(pixel_progression, block->next->data, (size_t)block->next->block_size);
			pixel_progression += block->next->block_size;
			pixel_read += (size_t)block->next->block_size;
		}
		if (block->next == NULL)
			goto error;
	}

	if (image_size != pixel_read)
		goto error;

	return stif;
error:
	stif_free(stif);
	return NULL;
}

void print_stif(stif_t *stif)
{
	printf("==== PRINT STIF ====\n");
	printf("== Header ==\n");
	printf("width => %d\n", stif->header.width);
	printf("heigth => %d\n", stif->header.height);
	printf("color_type => %d\n", stif->header.color_type);

	printf("== BLOCKS ==\n");
	for (stif_block_t *block = stif->block_head;
			block != NULL;
			block = block->next)
	{
		printf("type => %d\n", block->block_type);
		printf("size => %d\n", block->block_size);
		printf("data => ");
		for (int32_t i = 0; i < block->block_size; i++)
			printf("%x ", *(block->data + i));
		printf("\n=============\n");
	}

	printf("== PIXEL(S) ==\n");
	unsigned char *pixel = NULL;
	size_t size = 0;
	if (stif->header.color_type == STIF_COLOR_TYPE_GRAYSCALE)
	{
		pixel = stif->grayscale_pixels;
		size = (size_t)(stif->header.width * stif->header.height);
	}
	else if (stif->header.color_type == STIF_COLOR_TYPE_RGB)
	{
		pixel = (unsigned char *)stif->rgb_pixels;
		size = (size_t)(stif->header.width * stif->header.height) * sizeof(pixel_rgb_t);
	}

	for (size_t i = 0; i < size; i++)
		printf("%x ", pixel[i]);

	printf("\n");
}
