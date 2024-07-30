#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils/logging.h"
#include "utils/fileutils.h"
#include "cee/types.h"
#include "cee/configparser.h"

#define MAX_BUILD_COMMAND_SIZE 2048
#define CEE_CONF_FILE "cee.conf"

void cee_build(CeeBuildMode build_mode) {
	// Collect the config from cee.conf and build accordingly
	CeeConfig config;
	CeeConfigInit(&config);

	parse_cee_config(&config, CEE_CONF_FILE);

	if (!path_exists(config.bin_directory)) {
		if (!create_dir(config.bin_directory)) {
			cee_log(ERROR, "Failed to create directory '%s': ", config.bin_directory);
			perror(NULL);
			exit(1);
		}
	}

	cee_log(INFO, "Compiling `%s` version %s (%s)\n", config.name, config.version, build_mode == BUILD_DEBUG ? "debug" : "release");

	char command[MAX_BUILD_COMMAND_SIZE];

	if (build_mode == BUILD_DEBUG)
		snprintf(command, sizeof(command), "%s %s %s -o %s/%s-debug", config.compiler, config.source_files, config.debug_flags, config.bin_directory, config.name);
	else
		snprintf(command, sizeof(command), "%s %s %s -o %s/%s-release", config.compiler, config.source_files, config.release_flags, config.bin_directory, config.name);
	
	cee_log(EXEC, "%s\n", command);
	system(command);

	CeeConfigFree(&config);
}

void cee_run(CeeBuildMode build_mode) {
	cee_build(build_mode); // Build the project before running

	CeeConfig config;
	CeeConfigInit(&config);

	parse_cee_config(&config, CEE_CONF_FILE);

	cee_log(INFO, "Running `%s` version %s (%s)\n", config.name, config.version, build_mode == BUILD_DEBUG ? "debug" : "release");

	char binarypath[256];

	if (build_mode == BUILD_DEBUG)
		snprintf(binarypath, sizeof(binarypath), "./%s/%s-debug", config.bin_directory, config.name);
	else
		snprintf(binarypath, sizeof(binarypath), "./%s/%s-release", config.bin_directory, config.name);
	
	cee_log(EXEC, "%s\n", binarypath);
	system(binarypath);

	CeeConfigFree(&config);
}

void cee_new(const char* project_name) {
	if (path_exists(project_name)) {
		cee_log(ERROR, "%s: Directory already exists\n", project_name);
		exit(1);
	}
	
	if (!create_dir(project_name)) {
		cee_log(ERROR, "Failed to create directory '%s': ", project_name);
		perror(NULL);
		exit(1);
	}

	if (chdir(project_name) != 0) {
		cee_log(ERROR, "chdir(%s): ", project_name);
		perror(NULL);
		exit(1);
	}

	FILE* cee_conf_ptr = fopen(CEE_CONF_FILE, "w");
	if (!cee_conf_ptr) {
		cee_log(ERROR, "Failed to open %s: ", CEE_CONF_FILE);
		perror(NULL);
		exit(1);
	}

	fprintf(cee_conf_ptr, "name = \"%s\"\n", project_name);
	fprintf(cee_conf_ptr, "compiler = \"cc\"\ndebug_flags = \"-Wall -Wpedantic\"\nrelease_flags = \"-Wall -Wpedantic -O2\"\n");
	fprintf(cee_conf_ptr, "source_files = \"main.c\"\nbin_directory = \"bin\"\nversion = \"0.1.0\"\n");

	fclose(cee_conf_ptr);

	FILE* main_c_ptr = fopen("main.c", "w");
	if (!main_c_ptr) {
		cee_log(ERROR, "Failed to open main.c: ");
		perror(NULL);
		exit(1);
	}

	fprintf(main_c_ptr, "#include <stdio.h>\n\nint main(int argc, char** argv) {\n\tputs(\"Hello, World!\\n\");\n\treturn 0;\n}\n");

	fclose(main_c_ptr);

	cee_log(INFO, "New cee project `%s` created\n", project_name);
}
