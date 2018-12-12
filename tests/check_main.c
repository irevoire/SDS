#include <check.h>
#include <stdlib.h>
#include "../include/stif.h"

const unsigned char base_picture[] = {
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
	0xFF, // PIXEL
	// BLOCK DATA
	0x01, // BLOCK TYPE DATA
	0x03, 0x00, 0x00, 0x00, // BLOCK SIZE
	0xCA, 0xCB, 0xCC // PIXEL
};

const unsigned char base_block[] = {
	// BLOCK DATA
	0x01, // BLOCK TYPE DATA
	0x01, 0x00, 0x00, 0x00, // BLOCK SIZE
	0xFF, // PIXEL
};

//////////////////////////////////////////////////////////
/////////////////// READ STIF BLOCK //////////////////////
//////////////////////////////////////////////////////////
START_TEST(read_stif_block_not_null){
	stif_block_t *block = NULL;
	size_t size = 0;
	block = read_stif_block(base_block, sizeof(base_block), &size);
	ck_assert(block != NULL);
	stif_block_free(block);
}
END_TEST

START_TEST(read_stif_block_increment_size){
	stif_block_t *block = NULL;
	size_t size = 0;
	block = read_stif_block(base_block, sizeof(base_block), &size);
	ck_assert(block != NULL);
	ck_assert(size == (STIF_BLOCK_MIN_SIZE + (size_t)block->block_size));
	stif_block_free(block);
}
END_TEST

START_TEST(read_stif_block_check_type_DATA){
	stif_block_t *block = NULL;
	size_t size = 0;
	block = read_stif_block(base_block, sizeof(base_block), &size);
	ck_assert(block != NULL);
	ck_assert(block->block_type == STIF_BLOCK_TYPE_DATA);
	stif_block_free(block);
}
END_TEST

START_TEST(read_stif_block_check_type_HEADER){
	stif_block_t *block = NULL;
	unsigned char buf[sizeof(base_block)];
	memcpy(buf, base_block, sizeof(base_block));
	buf[0] = 0x00; // block type = header
	size_t size = 0;
	block = read_stif_block(buf, sizeof(base_block), &size);
	ck_assert(block != NULL);
	ck_assert(block->block_type == STIF_BLOCK_TYPE_HEADER);
	stif_block_free(block);
}
END_TEST

START_TEST(read_stif_block_check_size){
	stif_block_t *block = NULL;
	size_t size = 0;
	block = read_stif_block(base_block, sizeof(base_block), &size);
	ck_assert(block != NULL);
	ck_assert(block->block_size == 1);
	stif_block_free(block);
}
END_TEST

START_TEST(read_stif_block_check_pixel){
	stif_block_t *block = NULL;
	size_t size = 0;
	block = read_stif_block(base_block, sizeof(base_block), &size);
	ck_assert(block != NULL);
	ck_assert(block->data[0] == base_block[5]);
	stif_block_free(block);
}
END_TEST

START_TEST(read_stif_block_bad_buffer_size){
	stif_block_t *block = NULL;
	size_t size = 2;
	block = read_stif_block(base_block, sizeof(base_block), &size);
	ck_assert(block == NULL);
}
END_TEST

START_TEST(read_stif_block_buffer_null){
	stif_block_t *block = NULL;
	size_t size = 20;
	block = read_stif_block(NULL, 10, &size);
	ck_assert(block == NULL);
}
END_TEST

//////////////////////////////////////////////////////////
////////////////////// PARSE STIF ////////////////////////
//////////////////////////////////////////////////////////
START_TEST(parse_stif_not_null){
	stif_t *stif = NULL;
	stif = parse_stif(base_picture, sizeof(base_picture));
	ck_assert(stif != NULL);
	stif_free(stif);
}
END_TEST

START_TEST(parse_stif_check_header){
	stif_t *stif = NULL;
	stif = parse_stif(base_picture, sizeof(base_picture));
	ck_assert(stif != NULL);
	ck_assert(stif->header.width == 2);
	ck_assert(stif->header.height == 2);
	ck_assert(stif->header.color_type == STIF_COLOR_TYPE_GRAYSCALE);
	ck_assert(stif->grayscale_pixels != NULL);
	ck_assert(stif->rgb_pixels == NULL);
	stif_free(stif);
}
END_TEST

START_TEST(parse_stif_check_pixels){
	stif_t *stif = NULL;
	stif = parse_stif(base_picture, sizeof(base_picture));
	ck_assert(stif != NULL);
	ck_assert(stif->grayscale_pixels[0] == base_picture[21]);
	ck_assert(stif->grayscale_pixels[1] == base_picture[27]);
	ck_assert(stif->grayscale_pixels[2] == base_picture[28]);
	ck_assert(stif->grayscale_pixels[3] == base_picture[29]);
	stif_free(stif);
}
END_TEST

START_TEST(parse_stif_small_buffer){
	ck_assert(parse_stif(base_picture, 1) == NULL);
}
END_TEST

START_TEST(parse_stif_no_magic){
	unsigned char picture[] = "hello";
	ck_assert(parse_stif(picture, sizeof(picture)) == NULL);
}
END_TEST

START_TEST(parse_stif_null_buffer){
	ck_assert(parse_stif(NULL, 10)==NULL);
}
END_TEST

START_TEST(parse_stif_first_block_is_data){
	unsigned char picture[sizeof(base_picture)];
	memcpy(picture, base_picture, sizeof(base_picture));
	picture[2] = 0x01; // first block type = data
	picture[16] = 0x00; // second block type = header
	ck_assert(parse_stif(picture, sizeof(picture)) == NULL);
}
END_TEST

START_TEST(parse_stif_no_header){
	unsigned char picture[sizeof(base_picture)];
	memcpy(picture, base_picture, sizeof(base_picture));
	picture[2] = 0x01; // first block type = data
	ck_assert(parse_stif(picture, sizeof(picture)) == NULL);
}
END_TEST

START_TEST(parse_stif_inconsistent_pixel_size){
	unsigned char picture[sizeof(base_picture)];
	memcpy(picture, base_picture, sizeof(base_picture));
	// remove the last pixel
	ck_assert(parse_stif(picture, sizeof(picture) - 1) == NULL);
}
END_TEST

START_TEST(parse_stif_two_headers){
	unsigned char picture[sizeof(base_picture)];
	memcpy(picture, base_picture, sizeof(base_picture));
	picture[16] = 0x00; // first block type = data
	ck_assert(parse_stif(picture, sizeof(picture)) == NULL);
}
END_TEST

static Suite *parse_suite(void)
{
	Suite *s;
	TCase *tc_parse_stif, *tc_read_stif;

	s = suite_create("parse_stif");

	// parse_stif_block
	tc_read_stif = tcase_create("read_stif");

	tcase_add_test(tc_read_stif, read_stif_block_not_null);
	tcase_add_test(tc_read_stif, read_stif_block_increment_size);
	tcase_add_test(tc_read_stif, read_stif_block_check_type_DATA);
	tcase_add_test(tc_read_stif, read_stif_block_check_type_HEADER);
	tcase_add_test(tc_read_stif, read_stif_block_check_size);
	tcase_add_test(tc_read_stif, read_stif_block_check_pixel);
	tcase_add_test(tc_read_stif, read_stif_block_bad_buffer_size);
	tcase_add_test(tc_read_stif, read_stif_block_buffer_null);

	suite_add_tcase(s, tc_read_stif);

	// parse_stif
	tc_parse_stif = tcase_create("parse_stif");

	tcase_add_test(tc_parse_stif, parse_stif_not_null);
	tcase_add_test(tc_parse_stif, parse_stif_check_header);
	tcase_add_test(tc_parse_stif, parse_stif_check_pixels);
	tcase_add_test(tc_parse_stif, parse_stif_small_buffer);
	tcase_add_test(tc_parse_stif, parse_stif_no_magic);
	tcase_add_test(tc_parse_stif, parse_stif_null_buffer);
	tcase_add_test(tc_parse_stif, parse_stif_first_block_is_data);
	tcase_add_test(tc_parse_stif, parse_stif_two_headers);
	tcase_add_test(tc_parse_stif, parse_stif_inconsistent_pixel_size);
	tcase_add_test(tc_parse_stif, parse_stif_no_header);

	suite_add_tcase(s, tc_parse_stif);

	return s;
}

int main(void) {
	int no_failed = 0;                   
	Suite *s;                            
	SRunner *runner;                     

	s = parse_suite();                   
	runner = srunner_create(s);          

	srunner_run_all(runner, CK_NORMAL);  
	no_failed = srunner_ntests_failed(runner); 
	srunner_free(runner);                      
	return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;  
}
