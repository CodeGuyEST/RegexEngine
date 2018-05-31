struct Match {
	int start_pos;
	int end_pos;
	char *string;
};

struct Match *find_match(char *regex, char *string);

void free_match(struct Match *match);
