#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Returns the found substring (or NULL if not found) between 2 delimiters.
// This string is heap-allocated so must be freed
char* extract_substring_between_chars(const char* str, char delimiter) {
	const char* first_delim = strchr(str, delimiter);
	if (first_delim == NULL)
		return NULL;

	const char* second_delim = strchr(first_delim + 1, delimiter);
	if (second_delim == NULL)
		return NULL;

	ssize_t length = second_delim - first_delim - 1;

	char* substring = malloc(length + 1);
	if (substring == NULL)
		return NULL;

	strncpy(substring, first_delim + 1, length);
	substring[length] = '\0';

	return substring;
}

// Returns a boolean indicating if `str` is consisted of all whitespace characters
bool is_whitespace(const char* str) {
	for (int i = 0; i < strlen(str); i++) {
		if (!isspace(str[i]))
			return false;
	}

	return true;
}

// Returns a boolean indicating if string `a` starts with string `b`
bool starts_with(const char* a, const char* b) {
	if (strncmp(a, b, strlen(b)) == 0) {
		return true;
	}

	return false;
}
