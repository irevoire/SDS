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
	0xCA, 0xCA, 0xCA // PIXEL
};

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
	TCase *tc_core;

	s = suite_create("Parse");

	/* Core test case */
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, parse_stif_null_buffer);
	tcase_add_test(tc_core, parse_stif_small_buffer);
	tcase_add_test(tc_core, parse_stif_no_magic);
	tcase_add_test(tc_core, parse_stif_first_block_is_data);
	tcase_add_test(tc_core, parse_stif_two_headers);
	tcase_add_test(tc_core, parse_stif_inconsistent_pixel_size);
	tcase_add_test(tc_core, parse_stif_no_header);

	suite_add_tcase(s, tc_core);

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
