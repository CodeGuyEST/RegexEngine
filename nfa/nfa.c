#include <stdio.h>
#include "nfa.h"
#include "state.h"
#include "transition.h"
#include "../parser/parser.h"
#include "../match.h"

struct Nfa *create_sub_nfa(struct TokenArray *token_arr, int start, int end) {
	struct Nfa *nfa = malloc(sizeof(struct Nfa));
	nfa->states = get_state_array();
	nfa->final_states = get_state_array();
	nfa->start = create_state();
	add_state(nfa->states, nfa->start);
	struct StateArray *current_states = get_state_array();
	struct StateArray *prev_states = get_state_array();
	add_state(current_states, nfa->start);
	int i;
	for(i = start; i <= end; i++) {
		struct Token token = token_arr->tokens[i];
		if(token.type == CAPT_GROUP) {
			if(token.string_value == "") { // Not much to do with empty capturing group
				if(i < end){
					struct Token next_token = token_arr->tokens[i + 1];
					if(next_token.type == QUESTION_MARK || next_token.type == STAR ||
					   next_token.type == PLUS) {
						i++;
					}
				}
			    continue;
			}
			struct Nfa *sub_nfa = create_nfa(token.string_value);
			int j;
			for(j = 0; j < sub_nfa->states->state_count; j++) {
				add_state(nfa->states, sub_nfa->states->states[j]);
			}
			struct Transition *sub_start_transition = create_transition(NULL, sub_nfa->start);
			append_transition_to_arr(current_states, sub_start_transition);

			// Add NULL-transition to next state to every final state of sub NFA
			struct State *new_state = create_state();
			add_state(nfa->states, new_state);
			struct Transition *next_state_transition = create_transition(NULL, new_state);

			append_transition_to_arr(sub_nfa->final_states, next_state_transition);

			if(i < end) {
				struct Token next_token = token_arr->tokens[i + 1];
				if(next_token.type == QUESTION_MARK) {
					// Add transition so that it would be possible to skip states of sub NFA
					append_transition_to_arr(current_states, next_state_transition);
					free(prev_states);
					prev_states = current_states;
					current_states = get_state_array();
					add_state(current_states, new_state);
					i++;
					continue;
				}
				else if(next_token.type == PLUS) {
					free(prev_states);
					prev_states = current_states;
					current_states = get_state_array();
					add_state(current_states, new_state);
					// Add prioritized NULL-transition to start of sub NFA
					// to every final state of NFA.
					add_transition_to_arr(sub_nfa->final_states, sub_start_transition, 0);
					i++;
					continue;
				}
				else if(next_token.type == STAR) {
					// Getting to sub NFA states is optional.
					append_transition_to_arr(current_states, next_state_transition);
					// Add prioritized NULL-transition to start of sub NFA
					// to every final state of NFA.
					struct StateArray *final_states = sub_nfa->final_states;
					add_transition_to_arr(final_states, sub_start_transition, 0);
					free(prev_states);
					prev_states = current_states;
					current_states = get_state_array();
					add_state(current_states, new_state);
					i++;
					continue;
				}
			}
		}
		else if(token.type == LITERAL_CHAR) {
			struct State *new_state = create_state();
			add_state(nfa->states, new_state);
			struct Transition *transition = create_transition(token.value, new_state);
			append_transition_to_arr(current_states, transition);
			prev_states = replace_state_arr(prev_states, current_states);
			free(current_states->states);
			current_states->state_count = 0;
			add_state(current_states, new_state);
		}
		else if(token.type == QUESTION_MARK) {
			struct State *current_state = current_states->states[0];
			struct Transition *transition = create_transition(NULL, current_state);

			struct State *prev_state = prev_states->states[0];
			append_transition(prev_state, transition);
		}
		else if(token.type == PLUS) {
			struct State *prev_state = prev_states->states[0];
			struct Transition *transition = create_transition(prev_state->transitions->c, prev_state);
			// Transitions should be ordered by the importance
			add_transition(prev_state, transition, 0);
		}
		else if(token.type == STAR) {
			// Transition to itself is the most preferred transition.
			struct State *prev_state = prev_states->states[0];
			struct Transition *self_transition = create_transition(prev_state->transitions->c, prev_state);
			add_transition(prev_state, self_transition, 0);

			// NULL transitions are the least preferred.
			struct State *current_state = current_states->states[0];
			struct Transition *null_transition = create_transition(NULL, current_state);
			append_transition(prev_state, null_transition);
		}
	}
	int j;
	for(j = 0; j < current_states->state_count; j++) {
		   add_state(nfa->final_states, current_states->states[j]);
	}
	if(end == token_arr->token_count - 1) { // Last sub-NFA(or whole NFA) is being created
		free(token_arr->tokens);
		free(token_arr);
	}
	free(current_states->states);
	free(current_states);
	free(prev_states->states);
	free(prev_states);
	return nfa;
}

struct Nfa *create_nfa(char *regex) {
	struct TokenArray *token_arr = parse_regex(regex);
	if(token_arr == NULL) {
		return NULL;
	}
	struct Nfa *nfa = malloc(sizeof(struct Nfa));
	nfa->start = create_state();
	nfa->states = get_state_array();
	add_state(nfa->states, nfa->start);
	nfa->final_states = get_state_array();

	int current_tokens_start = 0;
	int i;
	for(i = 0; i < token_arr->token_count; i++) {
		if(token_arr->tokens[i].type == PIPE) {
			struct Nfa *sub_nfa = create_sub_nfa(token_arr, current_tokens_start, i - 1);
			int j;
			for(j = 0; j < sub_nfa->states->state_count; j++) {
				add_state(nfa->states, sub_nfa->states->states[j]);
			}
			for(j = 0; j < sub_nfa->final_states->state_count; j++) {
				add_state(nfa->final_states, sub_nfa->final_states->states[j]);
			}
			struct Transition *transition = create_transition(NULL, sub_nfa->start);
			append_transition(nfa->start, transition);
			current_tokens_start = i + 1;
		}
	}
	if(current_tokens_start == 0) {
		free_nfa(nfa);
		return create_sub_nfa(token_arr, 0, token_arr->token_count - 1);
	}
	else {
		struct Nfa *sub_nfa = create_sub_nfa(token_arr, current_tokens_start, token_arr->token_count - 1);
		int j;
	    for(j = 0; j < sub_nfa->states->state_count; j++) {
	    	add_state(nfa->states, sub_nfa->states->states[j]);
		}
		for(j = 0; j < sub_nfa->final_states->state_count; j++) {
			add_state(nfa->final_states, sub_nfa->final_states->states[j]);
		}
		struct Transition *transition = create_transition(NULL, sub_nfa->start);
		append_transition(nfa->start, transition);
	}
	return nfa;
}

void free_nfa(struct Nfa *nfa) {
	int i;
	for(i = 0; i < nfa->states->state_count; i++) {
		struct State *state = nfa->states->states[i];
		int j;
		for(j = 0; j < state->transition_count; j++) {
			free(state->transitions + j);
		}
		free(state);
	}
	free(nfa->final_states);
	free(nfa->states);
	free(nfa);
}

struct Match *create_match(int start, int end, char *str) {
	struct Match *match = malloc(sizeof(struct Match));
	match->start_pos = start;
	match->end_pos = end;
	char *subbuff = malloc(end - start + 1);
	memcpy(subbuff, str + start, end - start);
	subbuff[end - start] = '\0';
	match->string = subbuff;
	return match;
}

// Returns number of characters consumed to get to matching state.
int char_count_to_match(char *str, int char_pos, struct State *current_state,
		struct StateArray *final_states) {
	if(state_in_array(final_states, current_state)) {
		return 0;
	}
	int i;
	for(i = 0; i < current_state->transition_count; i++) {
		struct Transition transition = current_state->transitions[i];
		if(transition.c == str[char_pos]) {
			int char_count = char_count_to_match(str, char_pos + 1,
					transition.next_state, final_states);
			if(char_count != -1) {
				// There is a match after char_count characters.
				return 1 + char_count;
			}
		}
		else if(transition.c == NULL) {
			int char_count = char_count_to_match(str, char_pos,
					transition.next_state, final_states);
			if(char_count != -1) {
				// There is a match after char_count characters.
				return char_count;
			}
		}
	}
	return -1;
}

struct Match *nfa_find_match(char *str, struct Nfa *nfa) {
	int i;
	int start;
	int end = -1;
	for(i = 0; i < strlen(str); i++) {
		int char_count = char_count_to_match(str, i, nfa->start, nfa->final_states);
		if(char_count != -1) {// Found match
			start = i;
			end = i + char_count;
			return create_match(start, end, str);
		}
	}
	return NULL;
}
