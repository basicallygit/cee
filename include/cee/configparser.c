#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils/logging.h"
#include "utils/stringutils.h"
#include "cee/types.h"

#define MAX_CONFIG_LINE_SIZE 512

// Extracts the value between 2 quotation marks in a config line
char* get_value(const char* line) {
	return extract_substring_between_chars(line, '"');
}

// Log that the config file has declared an option but not set a value, free
// the config, and exit the program
void error_missing_value(CeeConfig* config, const char* config_filename, const char* option) {
	cee_log(ERROR, "%s: Option \"%s\" declared but no value set.\n", config_filename, option);
	CeeConfigFree(config);
	exit(1);
}

// Log that the config file is missing an option, free the config and exit the program
void error_missing_option(CeeConfig* config, const char* config_filename, const char* option) {
	cee_log(ERROR, "%s: Missing option \"%s\".\n", config_filename, option);
	CeeConfigFree(config);
	exit(1);
}

// Tries to open the file `config_filename` and fill the config struct
// with the key-value pairs found in `config_filename`
void parse_cee_config(CeeConfig* config, const char* config_filename) {
	FILE* config_file = fopen(config_filename, "r");
	
	if (config_file == NULL) {
		cee_log(ERROR, "Failed to open %s: ", config_filename);
		perror(NULL);
		exit(1);
	}

	char line[MAX_CONFIG_LINE_SIZE];
	
	// Loop over lines of the file
	while (fgets(line, sizeof(line), config_file)) {
		if (starts_with(line, "#")) {} // Ignore comment lines (starting with #)
		else if (starts_with(line, "name =") || starts_with(line, "name=")) {
			config->name = get_value(line);
			if (!config->name) {
				error_missing_value(config, config_filename, "name");
    		}
		}
		else if (starts_with(line, "compiler =") || starts_with(line, "compiler=")) {
			config->compiler = get_value(line);
			if (!config->compiler) {
				error_missing_value(config, config_filename, "compiler");
			}
		}
		else if (starts_with(line, "debug_flags =") || starts_with(line, "debug_flags=")) {
    		config->debug_flags = get_value(line);
    		if (!config->debug_flags) {
        		error_missing_value(config, config_filename, "debug_flags");
    		}
		}
		else if (starts_with(line, "release_flags =") || starts_with(line, "release_flags=")) {
    		config->release_flags = get_value(line);
    		if (!config->release_flags) {
        		error_missing_value(config, config_filename, "release_flags");
    		}
		}
		else if (starts_with(line, "source_files =") || starts_with(line, "source_files=")) {
    		config->source_files = get_value(line);
    		if (!config->source_files) {
        		error_missing_value(config, config_filename, "source_files");
    		}
		}
		else if (starts_with(line, "bin_directory =") || starts_with(line, "bin_directory=")) {
    		config->bin_directory = get_value(line);
    		if (!config->bin_directory) {
        		error_missing_value(config, config_filename, "bin_directory");
    		}
		}
		else if (starts_with(line, "version =") || starts_with(line, "version=")) {
    		config->version = get_value(line);
    		if (!config->version) {
        		error_missing_value(config, config_filename, "version");
    		}
		}
		else {
			if (!is_whitespace(line)) {
				cee_log(ERROR, "%s: Unexpected line: %s\n", config_filename, line);
				CeeConfigFree(config);
				exit(1);
			}
		}
	}

	// Check for any missing options
	if (!config->name)
		error_missing_option(config, config_filename, "name");

	if (!config->compiler)
		error_missing_option(config, config_filename, "compiler");
	
	if (!config->debug_flags)
		error_missing_option(config, config_filename, "debug_flags");
	
	if (!config->release_flags)
		error_missing_option(config, config_filename, "release_flags");
	
	if (!config->source_files)
		error_missing_option(config, config_filename, "source_files");
	
	if (!config->bin_directory)
		error_missing_option(config, config_filename, "bin_directory");
	
	if (!config->version)
		error_missing_option(config, config_filename, "version");
	
	fclose(config_file);
}
