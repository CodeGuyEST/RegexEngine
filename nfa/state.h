struct State {
	int transition_count;
	int max_transitions;
	struct Transition *transitions;
};

struct StateArray {
	int state_count;
	struct State **states;
};

struct State *create_state();
void free_state(struct State* state, struct StateArray *dealloc_states);
void add_state(struct StateArray *array, struct State *state);
void add_transition(struct State *state, struct Transition *transition, int index);
void append_transition(struct State *state, struct Transition *transition);
void append_transition_to_arr(struct StateArray *arr, struct Transition *transition);
void add_transition_to_arr(struct StateArray *arr, struct Transition *transition, int index);
struct StateArray *get_state_array();
struct StateArray *get_next_states(struct StateArray *state_arr, char c);
struct StateArray *replace_state_arr(struct StateArray *old, struct StateArray *new);
int state_in_array(struct StateArray *state_arr, struct State *state);
