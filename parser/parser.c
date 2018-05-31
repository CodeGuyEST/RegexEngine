#include <stdio.h>
#include "parser.h"

void throw_unexpected_char(int position) {
	fprintf(stderr, "Unexpected character at position %d\n", position);
}

void throw_missing_parenthesis(int position) {
	fprintf(stderr, "Expected ')' after position %d\n", position);
}

// Checks if parentheses are balanced
int check_parentheses(char *regex, int start_pos, int end_pos) {
	int counter = 0;
	int last_left_par_pos = -1;
	int i = start_pos;
	while(i <= end_pos) {
		if(regex[i] == '\\') {
			i += 2;
			continue;
		}
		if(regex[i] == '(') {
			last_left_par_pos = i;
			counter++;
		}
		if(regex[i] == ')') {
			counter--;
		}
		if(counter < 0) {
			throw_unexpected_char(i);
			return 1;
		}
		i++;
	}
	if(counter > 0) { // Missing at least 1 ')' in the end of the regex
		throw_missing_parenthesis(last_left_par_pos);
	}
	return counter;
}

// Finds the end position of capturing group starting from start_pos
int capt_group_end(char *regex, int start_pos) {
	int count = 1;
	int i = start_pos;
	while(i < strlen(regex)) {
		if(regex[i] == '\\') {
			i += 2;
			continue;
		}
		if(regex[i] == '(') {
			count++;
		}
		else if(regex[i] == ')') {
			count--;
		}
		if(count == 0) {
			return i == start_pos ? i : i - 1;
		}
		i++;
	}
	return -1;
}

int validate(char *regex, int start_pos, int end_pos) {
	if(start_pos == end_pos) {
		return 0;
	}
	if(regex[start_pos] == '*' || regex[start_pos] == '?' || regex[start_pos] == '+' ||
			regex[start_pos] == '|') {
		throw_unexpected_char(start_pos);
		return 1;
	}
	if(check_parentheses(regex, start_pos, end_pos) != 0) {
		return 1;
	}
	int i = start_pos;
	while(i <= end_pos) {
		if(regex[i] == '\\') {
			if(i == end_pos) {
				throw_unexpected_char(i);
				return 1;
			}
			i += 2;
			continue;
		}
		if(regex[i] == '(') {
			if(validate(regex, i + 1, capt_group_end(regex, i + 1)) != 0) {
				return 1;
			}
		}
		if(i < end_pos && (regex[i] == '*' || regex[i] == '?' || regex[i] == '+')) {
			if(regex[i + 1] == '*' || regex[i + 1] == '?' || regex[i + 1] == '+') {
				throw_unexpected_char(i + 1);
				return 1;
			}
		}
		if(i == end_pos && regex[i] == '|') {
			throw_unexpected_char(i);
			return 1;
		}
		if(i < end_pos && regex[i] == '|') {
			if(regex[i + 1] == '|') {
				throw_unexpected_char(i + 1);
				return 1;
			}
		}
		i++;
	}
	return 0;
}

struct TokenArray *parse_regex(char *regex) {
	if(validate(regex, 0, strlen(regex) - 1) != 0) {
		return NULL;
	}
	struct TokenArray *token_array = malloc(sizeof(struct TokenArray));
	token_array->token_count = 0;
	int i = 0;
	while(i < strlen(regex)) {
		struct Token token;
		if(regex[i] == '\\') {
			token.value = regex[i + 1];
			token.type = LITERAL_CHAR;
			if(token_array->token_count == 0) {
				token_array->tokens = malloc(sizeof(struct Token));
			}
			else {
				token_array->tokens = realloc(token_array->tokens, (token_array->token_count + 1) * sizeof(struct Token));
			}
			token_array->tokens[token_array->token_count] = token;
			token_array->token_count++;
			i += 2;
			continue;
		}
		if(regex[i] == '(') {
			token.type = CAPT_GROUP;
			int start = i + 1;
			int end = capt_group_end(regex, i + 1);

			if(start == end) {
				char *subbuff = malloc(1);
				subbuff[0] = '\0';
				token.string_value = subbuff;
			}
			else {
				char *subbuff = malloc(end - start + 2);
				memcpy(subbuff, regex + start, end - start + 1);
				subbuff[end - start + 1] = '\0';
				token.string_value = subbuff;
			}

			if(token_array->token_count == 0) {
				token_array->tokens = malloc(sizeof(struct Token));
			}
			else {
				token_array->tokens = realloc(token_array->tokens, (token_array->token_count + 1) * sizeof(struct Token));
			}
			token_array->tokens[token_array->token_count] = token;
			token_array->token_count++;
			i = end + 2; // Don't take ending ')' into account.
			continue;
		}
		token.value = regex[i];
		switch(regex[i]) {
		case '*':
			token.type = STAR;
			break;
		case '?':
			token.type = QUESTION_MARK;
			break;
		case '+':
			token.type = PLUS;
			break;
		case '|':
			token.type = PIPE;
			break;
		default:
			token.type = LITERAL_CHAR;
			break;
		}
		if(token_array->token_count == 0) {
			token_array->tokens = malloc(sizeof(struct Token));
		}
	    else {
	    	token_array->tokens = realloc(token_array->tokens, (token_array->token_count + 1) * sizeof(struct Token));
	    }
		token_array->tokens[token_array->token_count] = token;
		token_array->token_count++;
		i++;
	}
	return token_array;
}
