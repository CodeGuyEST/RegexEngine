struct Transition {
	char c;
	struct State *next_state;
};

struct Transition *create_transition(char c, struct State *state);
