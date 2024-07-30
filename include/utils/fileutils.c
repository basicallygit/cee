#include <sys/stat.h>
#include <stdbool.h>

// Returns a boolean indicating if the specified path exists
bool path_exists(const char* path) {
	struct stat sb;
	if (!stat(path, &sb))
		return true;
	
	return false;
}

bool create_dir(const char* path) {
	if (mkdir(path, 0700) == 0)
		return true;
	
	return false;
}
