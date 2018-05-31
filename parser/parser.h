typedef enum {LITERAL_CHAR, CAPT_GROUP, PIPE, QUESTION_MARK, PLUS, STAR} token_type;

struct Token {
	token_type type;
	char value;
	char *string_value; // For capturing groups
};

struct TokenArray {
	struct Token *tokens;
	int token_count;
};

struct TokenArray *parse_regex(char *regex);
