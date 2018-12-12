#include <check.h>
#include <stdlib.h>
#include "../include/stif.h"


START_TEST(parse_stif_small_buffer){
	ck_assert(parse_stif((const unsigned char *) "hello", 1)==NULL);
}
END_TEST

START_TEST(parse_stif_null_buffer){
	ck_assert(parse_stif(NULL, 10)==NULL);
}
END_TEST

Suite * parse_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Parse");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, parse_stif_null_buffer);
    tcase_add_test(tc_core, parse_stif_small_buffer);
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
