#include <check.h>
#include "../parser/parser.h"

START_TEST(test_literal_chars) {
	char *regex = "abc\\+ABC\\?123";
	struct TokenArray *token_arr = parse_regex(regex);
	struct Token* tokens = token_arr->tokens;
	char *test_string = "abc+ABC?123";
	int i;
	for(i = 0; i < strlen(test_string); i++) {
		ck_assert(tokens[i].value == test_string[i]);
		ck_assert(tokens[i].type == LITERAL_CHAR);
	}
	free(tokens);
	free(token_arr);
}
END_TEST

START_TEST(test_special_chars) {
	char *regex = "a|b?c*d+";
	struct TokenArray *token_arr = parse_regex(regex);
	struct Token* tokens = token_arr->tokens;

	ck_assert(tokens[1].value == '|');
	ck_assert(tokens[1].type == PIPE);

	ck_assert(tokens[3].value == '?');
	ck_assert(tokens[3].type == QUESTION_MARK);

	ck_assert(tokens[5].value == '*');
	ck_assert(tokens[5].type == STAR);

	ck_assert(tokens[7].value == '+');
	ck_assert(tokens[7].type == PLUS);

	free(tokens);
	free(token_arr);
}
END_TEST

START_TEST(test_incorrect_input) {
	char *regex_1 = "ab++c";
	struct TokenArray *tokens_1 = parse_regex(regex_1);
    ck_assert(tokens_1 == NULL);

    char *regex_2 = "ab\\";
    struct TokenArray *tokens_2 = parse_regex(regex_2);
    ck_assert(tokens_2 == NULL);

    char *regex_3 = "a||b";
    struct TokenArray *tokens_3 = parse_regex(regex_3);
    ck_assert(tokens_3 == NULL);

    char* regex_4 = "ab()())aa";
    struct TokenArray *tokens_4 = parse_regex(regex_4);
    ck_assert(tokens_4 == NULL);

    char* regex_5 = "ab()((aaa";
    struct TokenArray *tokens_5 = parse_regex(regex_5);
    ck_assert(tokens_5 == NULL);

    char* regex_6 = "ab(a||a)cd";
    struct TokenArray *tokens_6 = parse_regex(regex_6);
    ck_assert(tokens_6 == NULL);
}
END_TEST

START_TEST(test_capturing_groups) {
	char *regex = "a(ab(c+)d)c()*";
	struct TokenArray *token_arr = parse_regex(regex);
	struct Token* tokens = token_arr->tokens;

	ck_assert(tokens[1].type == CAPT_GROUP);
	ck_assert_str_eq(tokens[1].string_value, "ab(c+)d");

	ck_assert(tokens[3].type == CAPT_GROUP);
	ck_assert_str_eq(tokens[3].string_value, "");

	free(tokens);
	free(token_arr);
}
END_TEST

Suite *parser_suite(void) {
	Suite *s;
	TCase *tc_literals;
	TCase *tc_specials;
	TCase *tc_incorrect;
	TCase *tc_capt_group;

	s = suite_create("Parser");

	tc_literals = tcase_create("Literals");
	tc_specials = tcase_create("Specials");
	tc_incorrect = tcase_create("Incorrect");
	tc_capt_group = tcase_create("Capturing group");

	tcase_add_test(tc_literals, test_literal_chars);
	tcase_add_test(tc_specials, test_special_chars);
	tcase_add_test(tc_incorrect, test_incorrect_input);
	tcase_add_test(tc_capt_group, test_capturing_groups);
	suite_add_tcase(s, tc_literals);
	suite_add_tcase(s, tc_specials);
	suite_add_tcase(s, tc_incorrect);
	suite_add_tcase(s, tc_capt_group);

	return s;
}
