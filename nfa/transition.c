#include "transition.h"

struct Transition *create_transition(char c, struct State *state) {
	struct Transition *transition = malloc(sizeof(struct Transition));
	transition->c = c;
	transition->next_state = state;
	return transition;
}
