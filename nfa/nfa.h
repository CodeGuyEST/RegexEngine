struct Nfa {
	struct State *start;
	struct StateArray *states;
	struct StateArray *final_states;
};

struct Nfa *create_nfa(char *regex);
void free_nfa(struct Nfa *nfa);
struct Match *nfa_find_match(char *str, struct Nfa *nfa);
