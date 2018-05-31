#include <stdio.h>
#include "match.h"
#include "nfa/nfa.h"
#include "parser/parser.h"
#include "nfa/state.h"
#include "nfa/transition.h"

struct Match *find_match(char *regex, char *str) {
	struct Nfa *nfa = create_nfa(regex);
	if(nfa == NULL) {
		return NULL;
	}
	struct Match *match = nfa_find_match(str, nfa);
	free_nfa(nfa);
	return match;
}

void free_match(struct Match *match) {
	free(match->string);
	free(match);
}
