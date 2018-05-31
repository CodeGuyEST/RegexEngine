#include <stdio.h>
#include "state.h"
#include "transition.h"

struct State *create_state() {
	struct State *state = malloc(sizeof(struct State));
	state->transition_count = 0;
	state->max_transitions = 4;
	state->transitions = malloc(state->max_transitions * sizeof(struct Transition));
	return state;
}

void free_state(struct State *state, struct StateArray *dealloc_states) {
	if(dealloc_states == NULL) {
		struct StateArray *dealloc_state_arr = malloc(sizeof(struct StateArray));
		dealloc_state_arr->state_count = 0;
		free_state(state, dealloc_state_arr);
		free(dealloc_state_arr->states);
		free(dealloc_state_arr);
		return;
	}
	if(state_in_array(dealloc_states, state)) {
		return;
	}
	int i;
	for(i = 0; i < state->transition_count; i++) {
		struct Transition *transition = state->transitions + i;
		if(transition->next_state != state) {
			free_state(transition->next_state, dealloc_states);
		}
	}
	free(state->transitions);
	free(state);
	add_state(dealloc_states, state);
}

void add_state(struct StateArray *array, struct State *state) {
	if(array->state_count == 0) {
		array->states = malloc(sizeof(struct State*));
	}
	else {
		array->states = realloc(array->states, (array->state_count + 1) * sizeof(struct State*));
	}
	array->states[array->state_count] = state;
	array->state_count++;
}

void add_transition(struct State *state, struct Transition *transition, int index) {
	if(state->transition_count >= state->max_transitions) {
			state->max_transitions *= 2;
			realloc(state->transitions, (state->max_transitions) * sizeof(struct Transition));
	}
	int i;
	for(i = state->transition_count; i > index; i--) {
		state->transitions[i] = state->transitions[i - 1];
	}
	state->transitions[index] = *transition;
	state->transition_count++;
}

void append_transition(struct State *state, struct Transition *transition) {
	add_transition(state, transition, state->transition_count);
}

void append_transition_to_arr(struct StateArray *arr, struct Transition *transition) {
	int i;
	for(i = 0; i < arr->state_count; i++) {
		append_transition(arr->states[i], transition);
	}
}

void add_transition_to_arr(struct StateArray *arr, struct Transition *transition, int index) {
	int i;
	for(i = 0; i < arr->state_count; i++) {
		add_transition(arr->states[i], transition, index);
	}
}

struct StateArray *get_state_array() {
	struct StateArray *arr = malloc(sizeof(struct StateArray));
	arr->state_count = 0;
	return arr;
}

struct StateArray *get_next_states(struct StateArray *state_arr, char c) {
	struct StateArray *next_states = malloc(sizeof(struct StateArray));
	next_states->state_count = 0;
	int i;
	for(i = 0; i < state_arr->state_count; i++) {
		struct State *state = state_arr->states[i];
		int j;
		for(j = 0; j < state->transition_count; j++) {
			struct Transition transition = state->transitions[j];
			if(transition.c == c || transition.c == NULL) {
				add_state(next_states, transition.next_state);
			}
		}
	}
	if(next_states->state_count > 0) {
		struct StateArray *null_states = get_next_states(next_states, NULL);
		for(i = 0; i < null_states->state_count; i++) {
			add_state(next_states, null_states->states[i]);
		}
	}
	return next_states;
}

int state_in_array(struct StateArray *arr, struct State *state) {
	int i;
	for(i = 0; i < arr->state_count; i++) {
		if((arr->states)[i] == state) {
			return 1;
		}
	}
	return 0;
}

struct StateArray *replace_state_arr(struct StateArray *old, struct StateArray *new) {
	if(old->state_count > 0) {
		free(old->states);
	}
	free(old);
	old = malloc(sizeof(struct StateArray));
	old->state_count = 0;
	int i;
	for(i = 0; i < new->state_count; i++) {
		add_state(old, new->states[i]);
		old->state_count++;
	}
	return old;
}
