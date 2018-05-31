#include <stdlib.h>
#include <check.h>
#include "../match.h"

START_TEST(test_simple_match) {
	char *regex = "bc";
	char *string = "abc";
	struct Match *match = find_match(regex, string);
	ck_assert_int_eq(match->start_pos, 1);
	ck_assert_int_eq(match->end_pos, 3);
	ck_assert_str_eq(match->string, "bc");
	free_match(match);
}
END_TEST

START_TEST(test_question_mark) {
	char *regex = "bc?";
	char *string_1 = "abc";
	struct Match *match = find_match(regex, string_1);
	ck_assert_int_eq(match->start_pos, 1);
	ck_assert_int_eq(match->end_pos, 3);
	ck_assert_str_eq(match->string, "bc");
	free_match(match);

	char *string_2 = "abde";
	match = find_match(regex, string_2);
	ck_assert_int_eq(match->start_pos, 1);
	ck_assert_int_eq(match->end_pos, 2);
	ck_assert_str_eq(match->string, "b");
	free_match(match);
}
END_TEST

START_TEST(test_plus) {
	char *regex = "bc+";
	char *string_1 = "abc";
	struct Match *match = find_match(regex, string_1);
	ck_assert_int_eq(match->start_pos, 1);
	ck_assert_int_eq(match->end_pos, 3);
	ck_assert_str_eq(match->string, "bc");
	free_match(match);

	char *string_2 = "abcccde";
	match = find_match(regex, string_2);
	ck_assert_int_eq(match->start_pos, 1);
	ck_assert_int_eq(match->end_pos, 5);
	ck_assert_str_eq(match->string, "bccc");
	free_match(match);

	char *string_3 = "abdc";
	match = find_match(regex, string_3);
	ck_assert(match == NULL);
}
END_TEST

START_TEST(test_star) {
	char *regex = "ab*c";
	char *string_1 = "0123abc";
	struct Match *match_1 = find_match(regex, string_1);
	ck_assert_int_eq(match_1->start_pos, 4);
	ck_assert_int_eq(match_1->end_pos, 7);
	ck_assert_str_eq(match_1->string, "abc");
	free_match(match_1);

	char *string_2 = "abbbcde";
	struct Match *match_2 = find_match(regex, string_2);
	ck_assert_int_eq(match_2->start_pos, 0);
	ck_assert_int_eq(match_2->end_pos, 5);
	ck_assert_str_eq(match_2->string, "abbbc");
	free_match(match_2);

	char *string_3 = "ac";
	struct Match *match_3 = find_match(regex, string_3);
	ck_assert_int_eq(match_3->start_pos, 0);
	ck_assert_int_eq(match_3->end_pos, 2);
	ck_assert_str_eq(match_3->string, "ac");
	free_match(match_3);
}
END_TEST

START_TEST(test_pipe) {
	char *regex_1 = "ab|ab*|b";
	char *regex_2 = "ab*|ab";

	char *string_1 = "0123abc";
	struct Match *match_1 = find_match(regex_1, string_1);
	ck_assert_int_eq(match_1->start_pos, 4);
	ck_assert_int_eq(match_1->end_pos, 6);
	ck_assert_str_eq(match_1->string, "ab");
	free_match(match_1);

	char *string_2 = "abbbcde";
	struct Match *match_2 = find_match(regex_2, string_2);
	ck_assert_int_eq(match_2->start_pos, 0);
	ck_assert_int_eq(match_2->end_pos, 4);
	ck_assert_str_eq(match_2->string, "abbb");
	free_match(match_2);

	char *string_3 = "cba";
	struct Match *match_3 = find_match(regex_1, string_3);
	ck_assert_int_eq(match_3->start_pos, 1);
	ck_assert_int_eq(match_3->end_pos, 2);
	ck_assert_str_eq(match_3->string, "b");
	free_match(match_3);
}
END_TEST

START_TEST(test_capt_group) {
	char *regex = "a(b+c)*";

	char *str_1 = "bcad";
	struct Match *match_1 = find_match(regex, str_1);
	ck_assert_int_eq(match_1->start_pos, 2);
	ck_assert_int_eq(match_1->end_pos, 3);
	ck_assert_str_eq(match_1->string, "a");
	free_match(match_1);

	char *str_2 = "123abcbbcbcc";
	struct Match *match_2 = find_match(regex, str_2);
	ck_assert_int_eq(match_2->start_pos, 3);
	ck_assert_int_eq(match_2->end_pos, 11);
	ck_assert_str_eq(match_2->string, "abcbbcbc");
	free_match(match_2);
}
END_TEST

Suite *match_suite(void) {
	Suite *s;
	TCase *tc_simple;
	TCase *tc_question_mark;
	TCase *tc_plus;
	TCase *tc_star;
	TCase *tc_pipe;
	TCase *tc_capt_group;

	s = suite_create("Match");

	tc_simple = tcase_create("Simple");
	tc_question_mark = tcase_create("Question");
	tc_plus = tcase_create("Plus");
	tc_star = tcase_create("Star");
	tc_pipe = tcase_create("Pipe");
	tc_capt_group = tcase_create("Capturing group");

	tcase_add_test(tc_simple, test_simple_match);
	tcase_add_test(tc_question_mark, test_question_mark);
	tcase_add_test(tc_plus, test_plus);
	tcase_add_test(tc_star, test_star);
	tcase_add_test(tc_pipe, test_pipe);
	tcase_add_test(tc_capt_group, test_capt_group);
	suite_add_tcase(s, tc_simple);
	suite_add_tcase(s, tc_question_mark);
	suite_add_tcase(s, tc_plus);
	suite_add_tcase(s, tc_star);
	suite_add_tcase(s, tc_pipe);
	suite_add_tcase(s, tc_capt_group);

	return s;
}
