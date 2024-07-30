#ifndef UTILS_STRINGUTILS_H
#define UTILS_STRINGUTILS_H
#include <stdbool.h>

char* extract_substring_between_chars(const char* str, char delimiter);
bool is_whitespace(const char* str);
bool starts_with(const char* a, const char* b);

#endif
