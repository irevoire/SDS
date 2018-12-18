#include "stif.h"

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

	free(s);
}

stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read)
{
	stif_block_t *block = NULL;
	int8_t block_type;
	int32_t block_size;

	// NOT ENOUGH BYTES TO READ
	if (*bytes_read + STIF_BLOCK_MIN_SIZE > buffer_size)
		return NULL;

	// GET THE TYPE
	memcpy(&block_type, buffer + *bytes_read, sizeof(int8_t));
	*bytes_read += 1;

	// GET THE SIZE
	memcpy(&block_size, buffer + *bytes_read, sizeof(int32_t));
	*bytes_read += 4;

	// NOT ENOUGH BYTES TO READ
	if (*bytes_read + (size_t)block_size > buffer_size)
		return NULL;

	block = malloc(sizeof(*block) + (uint32_t) block_size);
	if (block == NULL)
		return NULL;

	block->block_type = block_type;
	block->block_size = block_size;
	block->next = NULL;

	memcpy(block->data, buffer + *bytes_read, (size_t)block->block_size);
	*bytes_read += (size_t)block->block_size;

	return block;
}

void stif_block_free(stif_block_t *b)
{
	if (b == NULL)
		return;
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
	memcpy(&header->width, current, sizeof(int32_t));
	current += 4;
	memcpy(&header->height, current, sizeof(int32_t));
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
	}
	return 0;
}

stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size)
{
	size_t read = 0;
	int16_t magic = 0;
	unsigned char *pixel_progression = NULL;
	size_t pixel_read = 0, image_size = 0;
	// tmp stif on the stack before we can malloc the real stif
	stif_t tstif = {0};
	stif_t *stif = &tstif;

	if (buffer == NULL)
		return NULL;

	if (buffer_size < 2) // STIF_MAGIC
		return NULL;

	memcpy(&magic, buffer, sizeof(int16_t));
	if (magic != (int16_t)STIF_MAGIC)
		return NULL;
	read += 2;

	stif->block_head = NULL;
	stif->grayscale_pixels = NULL;
	stif->rgb_pixels = NULL;

	stif->block_head = read_stif_block(buffer, buffer_size, &read);
	if (stif->block_head == NULL)
		return NULL;

	if (parse_stif_header(&(stif->header), stif->block_head))
		return NULL;

	if (stif->header.width < 0)
		goto error;
	if (stif->header.height < 0)
		goto error;

	// malloc the pixels array with a size appropriate for the type
	// malloc the pixels array with a size appropriate for the type
	if (stif->header.color_type == STIF_COLOR_TYPE_GRAYSCALE)
		image_size = (size_t)(stif->header.width * stif->header.height) * sizeof(pixel_grayscale_t);
	else if (stif->header.color_type == STIF_COLOR_TYPE_RGB)
		image_size = (size_t)(stif->header.width * stif->header.height) * sizeof(pixel_rgb_t);

	stif = malloc(sizeof(*stif) + image_size);
	if (stif == NULL)
		return NULL;

	*stif = tstif;
	pixel_progression = stif->data;
	stif->grayscale_pixels = stif->data;
	stif->rgb_pixels = (pixel_rgb_t *)stif->data;

	for (stif_block_t *block = stif->block_head;
			read < buffer_size;
			block = block->next)
	{
		block->next = read_stif_block(buffer, buffer_size, &read);
		if (block->next == NULL)
			goto error;
		if (block->next->block_type != STIF_BLOCK_TYPE_DATA)
			goto error;

		pixel_read += (size_t)block->next->block_size;
		if (pixel_read > image_size)
			goto error;

		memcpy(pixel_progression, block->next->data, (size_t)block->next->block_size);
		pixel_progression += block->next->block_size;
	}

	if (image_size != pixel_read)
		goto error;

	return stif;
error:
	stif_free(stif);
	return NULL;
}
