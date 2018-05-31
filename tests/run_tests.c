#include <check.h>
#include <stdlib.h>
#include "test_matching.h"
#include "test_parser.h"
#include "test_nfa.h"

int main(void) {
	int number_failed = 0;

	Suite *s_match;
	SRunner *r_match;

	s_match = match_suite();
	r_match = srunner_create(s_match);

	Suite *s_parser;
	SRunner *r_parser;

	s_parser = parser_suite();
	r_parser = srunner_create(s_parser);

	Suite *s_nfa;
	SRunner *r_nfa;

	s_nfa = nfa_suite();
	r_nfa = srunner_create(s_nfa);

	srunner_run_all(r_parser, CK_NORMAL);
	number_failed += srunner_ntests_failed(r_parser);
	srunner_free(r_parser);

	srunner_run_all(r_nfa, CK_NORMAL);
	number_failed += srunner_ntests_failed(r_nfa);
	srunner_free(r_nfa);

	srunner_run_all(r_match, CK_NORMAL);
	number_failed += srunner_ntests_failed(r_match);
	srunner_free(r_match);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
