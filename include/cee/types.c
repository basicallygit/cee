#include <stdlib.h>
#include <stddef.h>
#include "cee/types.h"

void CeeConfigInit(CeeConfig* config) {
	// Initialize all strings to NULL to allow for
	// easy free-ing of any malloc'd strings with `CeeConfigFree`.
	config->name = NULL;
	config->compiler = NULL;
	config->debug_flags = NULL;
	config->release_flags = NULL;
	config->source_files = NULL;
	config->bin_directory = NULL;
	config->version = NULL;
}

void CeeConfigFree(CeeConfig* config) {
	// free() any allocated strings in the config.
	// All config strings are bound to be heap-allocated
	// due to `extract_substring_between_chars` returning
	// a heap-allocated string.
	if (config->name)
		free(config->name);

	if (config->compiler)
		free(config->compiler);
	
	if (config->debug_flags)
		free(config->debug_flags);
	
	if (config->release_flags)
		free(config->release_flags);
	
	if (config->source_files)
		free(config->source_files);
	
	if (config->bin_directory)
		free(config->bin_directory);
	
	if (config->version)
		free(config->version);
}
