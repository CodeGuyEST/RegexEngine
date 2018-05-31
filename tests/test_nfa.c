#include <stdlib.h>
#include <check.h>
#include "../match.h"
#include "../nfa/state.h"
#include "../nfa/transition.h"
#include "../nfa/nfa.h"

START_TEST(test_nfa_literal_chars) {
	char *regex = "ab\\+c";
	char *test_string = "ab+c";
	struct Nfa *nfa = create_nfa(regex);
	printf("NFA CREATED\n");
	ck_assert(nfa->start != NULL);
	ck_assert(nfa->final_states != NULL);
	ck_assert(nfa->states != NULL);
	ck_assert_int_eq(nfa->states->state_count, 5);
	ck_assert(!state_in_array(nfa->final_states, nfa->start));
	struct State *current_state = nfa->start;
	int i;
	for(i = 0; i < 4; i++) {
		ck_assert(current_state != NULL);
		ck_assert(current_state->transitions != NULL);
		ck_assert(current_state->transitions[0].c == test_string[i]);
		current_state = current_state->transitions->next_state;
	}
	free_nfa(nfa);
}
END_TEST

START_TEST(test_nfa_question_mark) {
	char *regex = "ab?c";
	char *test_string = "abc";
	struct Nfa *nfa = create_nfa(regex);
	ck_assert(nfa->start != NULL);
	ck_assert(nfa->final_states != NULL);
	ck_assert(!state_in_array(nfa->final_states, nfa->start));
	struct State *current_state = nfa->start;
	int j;
	for(j = 0; !state_in_array(nfa->final_states, current_state); j++) {
		ck_assert(current_state != NULL);
		ck_assert(current_state->transitions != NULL);
		ck_assert(current_state->transitions[0].c == test_string[j]);
		if(j == 1) {
			// Should have NULL transition because of question mark.
			ck_assert(current_state->transitions[1].c == NULL);
			// Next state when following b char.
			struct State *next_state = current_state->transitions[0].next_state;
			ck_assert_ptr_eq(current_state->transitions[1].next_state, next_state);
		}
	    current_state = current_state->transitions->next_state;
	}
	free_nfa(nfa);
}
END_TEST

START_TEST(test_nfa_plus) {
	char *regex = "ab+c";
	char *test_string = "abc";
	struct Nfa *nfa = create_nfa(regex);
	ck_assert(nfa->start != NULL);
	ck_assert(nfa->final_states != NULL);
	ck_assert(!state_in_array(nfa->final_states, nfa->start));
	struct State *current_state = nfa->start;
	int i;
	for(i = 0; !state_in_array(nfa->final_states, current_state); i++) {
		ck_assert(current_state != NULL);
		ck_assert(current_state->transitions != NULL);
		ck_assert(current_state->transitions[0].c == test_string[i]);
		if(i == 1) {
			// Should have transition to itself because of plus.
			ck_assert(current_state->transitions[0].next_state == current_state);
			// Next state when following b char.
			ck_assert(current_state->transitions[1].c == 'b');
			current_state = current_state->transitions[1].next_state;
			continue;
		}
		current_state = current_state->transitions->next_state;
	}
	free_nfa(nfa);
}
END_TEST

START_TEST(test_nfa_star) {
	char *regex = "ab*c";
	char *test_string = "abc";
	struct Nfa *nfa = create_nfa(regex);
	ck_assert(nfa->start != NULL);
	ck_assert(nfa->final_states != NULL);
	ck_assert(!state_in_array(nfa->final_states, nfa->start));
	struct State *current_state = nfa->start;
	int i;
	for(i = 0; !state_in_array(nfa->final_states, current_state); i++) {
		ck_assert(current_state != NULL);
		ck_assert(current_state->transitions != NULL);
		ck_assert(current_state->transitions[0].c == test_string[i]);
		if(i == 1) {
			ck_assert_int_eq(current_state->transition_count, 3);
			// Should have transition to itself because of plus.
			ck_assert(current_state->transitions[0].next_state == current_state);
			// Next state when following b char.
			ck_assert(current_state->transitions[1].c == 'b');
			// Should have NULL transition to next state.
			ck_assert(current_state->transitions[2].c == NULL);
			current_state = current_state->transitions[1].next_state;
			continue;
		}
		current_state = current_state->transitions->next_state;
	}
	free_nfa(nfa);
}
END_TEST

START_TEST(test_nfa_pipe) {
	char *regex = "ab|ac*|d";
	struct Nfa *nfa = create_nfa(regex);
	ck_assert(nfa->start != NULL);
	ck_assert(nfa->final_states != NULL);
	ck_assert_int_eq(nfa->start->transition_count, 3);
	ck_assert_int_eq(nfa->final_states->state_count, 3);
	int i;
	for(i = 0; i < 3; i++) {
		ck_assert((nfa->start->transitions + i)->c == NULL);
	}
	struct State *current_state = nfa->start->transitions->next_state;
	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions->c == 'a');

	current_state = current_state->transitions->next_state;
	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions->c == 'b');

	current_state = current_state->transitions->next_state;
	ck_assert_int_eq(current_state->transition_count, 0);
	ck_assert(state_in_array(nfa->final_states, current_state));

	current_state = (nfa->start->transitions + 1)->next_state;
	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions->c == 'a');

	current_state = current_state->transitions->next_state;
	ck_assert_int_eq(current_state->transition_count, 3);
	ck_assert(current_state->transitions->c == 'c');
	ck_assert_ptr_eq(current_state->transitions->next_state, current_state);
	ck_assert((current_state->transitions + 1)->c == 'c');
	ck_assert((current_state->transitions + 2)->c == NULL);
	ck_assert_ptr_eq((current_state->transitions + 1)->next_state, (current_state->transitions + 2)->next_state);

	current_state = (current_state->transitions + 1)->next_state;
	ck_assert_int_eq(current_state->transition_count, 0);
	ck_assert(state_in_array(nfa->final_states, current_state));

	current_state = (nfa->start->transitions + 2)->next_state;
	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions->c == 'd');

	current_state = current_state->transitions->next_state;
    ck_assert_int_eq(current_state->transition_count, 0);
	ck_assert(state_in_array(nfa->final_states, current_state));

	free_nfa(nfa);
}
END_TEST

START_TEST(test_nfa_capt_group_and_pipe) {
	char *regex = "(a+b)?|(c|d)+";
	struct Nfa *nfa = create_nfa(regex);
	ck_assert(nfa->start != NULL);
	ck_assert_int_eq(nfa->start->transition_count, 2);
	ck_assert(nfa->start->transitions->c == NULL);
	ck_assert((nfa->start->transitions + 1)->c == NULL);
	ck_assert(nfa->final_states != NULL);
	ck_assert_int_eq(nfa->final_states->state_count, 2);

	struct State *current_state = nfa->start->transitions->next_state;
	ck_assert_int_eq(current_state->transition_count, 2);
	ck_assert(current_state->transitions[0].c == NULL);
	ck_assert(current_state->transitions[1].c == NULL);
	ck_assert(state_in_array(nfa->final_states, current_state->transitions[1].next_state));
	current_state = current_state->transitions->next_state;

	ck_assert_int_eq(current_state->transition_count, 2);
	ck_assert(current_state->transitions[0].c == 'a');
	ck_assert_ptr_eq(current_state, current_state->transitions->next_state);
	ck_assert(current_state->transitions[1].c == 'a');
	current_state = (current_state->transitions + 1)->next_state;

	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions[0].c == 'b');
	current_state = current_state->transitions->next_state;

	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions[0].c == NULL);
	current_state = current_state->transitions->next_state;

	ck_assert_int_eq(current_state->transition_count, 0);
	ck_assert(state_in_array(nfa->final_states, current_state));
	current_state = (nfa->start->transitions + 1)->next_state;

	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions->c == NULL);
	current_state = current_state->transitions->next_state;

	// State from where two alternative paths originate because of pipe character
    struct State *conditional_start = current_state;

	ck_assert_int_eq(current_state->transition_count, 2);
	ck_assert(current_state->transitions->c == NULL);
	ck_assert((current_state->transitions + 1)->c == NULL);
	current_state = current_state->transitions->next_state;

	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions->c == 'c');
	current_state = current_state->transitions->next_state;

	ck_assert_int_eq(current_state->transition_count, 2);
	ck_assert(current_state->transitions[0].c == NULL);
	ck_assert_ptr_eq(current_state->transitions[0].next_state, conditional_start);
	ck_assert(current_state->transitions[1].c == NULL);
	current_state = (current_state->transitions + 1)->next_state;

	ck_assert_int_eq(current_state->transition_count, 0);
	ck_assert(state_in_array(nfa->final_states, current_state));
	current_state = (conditional_start->transitions + 1)->next_state;

	ck_assert_int_eq(current_state->transition_count, 1);
	ck_assert(current_state->transitions->c == 'd');
	current_state = current_state->transitions->next_state;

	ck_assert_int_eq(current_state->transition_count, 2);
	ck_assert(current_state->transitions[0].c == NULL);
	ck_assert_ptr_eq(current_state->transitions[0].next_state, conditional_start);
	ck_assert(current_state->transitions[1].c == NULL);
	current_state = (current_state->transitions + 1)->next_state;

	ck_assert_int_eq(current_state->transition_count, 0);
	ck_assert(state_in_array(nfa->final_states, current_state));

	free_nfa(nfa);
}
END_TEST

Suite *nfa_suite(void) {
	Suite *s;
	TCase *tc_literal_chars;
	TCase *tc_question_mark;
	TCase *tc_plus;
	TCase *tc_star;
	TCase *tc_pipe;
	TCase *tc_capt_group;

	s = suite_create("Nfa");

	tc_literal_chars = tcase_create("Literals");
	tc_question_mark = tcase_create("Question");
	tc_plus = tcase_create("Plus");
	tc_star = tcase_create("Star");
	tc_pipe = tcase_create("Pipe");
	tc_capt_group = tcase_create("Capturing group");

	tcase_add_test(tc_literal_chars, test_nfa_literal_chars);
	tcase_add_test(tc_question_mark, test_nfa_question_mark);
	tcase_add_test(tc_plus, test_nfa_plus);
	tcase_add_test(tc_star, test_nfa_star);
	tcase_add_test(tc_pipe, test_nfa_pipe);
	tcase_add_test(tc_capt_group, test_nfa_capt_group_and_pipe);
	suite_add_tcase(s, tc_literal_chars);
	suite_add_tcase(s, tc_question_mark);
	suite_add_tcase(s, tc_plus);
	suite_add_tcase(s, tc_star);
	suite_add_tcase(s, tc_pipe);
	suite_add_tcase(s, tc_capt_group);

	return s;
}
