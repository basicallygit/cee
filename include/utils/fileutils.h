#ifndef UTILS_FILEUTILS_H
#define UTILS_FILEUTILS_H
#include <stdbool.h>

bool path_exists(const char* path);
int create_dir(const char* path);

#endif
