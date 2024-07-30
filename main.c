#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/logging.h"
#include "cee/cmds.h"
#include "cee/types.h"

#define USAGE_TEXT "Usage:\n"\
"  cee <command> <arguments>\n"\
"\n"\
"Commands:\n"\
"  n, new <name>: Creates a new cee project\n"\
"  b, build: Builds the current project\n"\
"  r, run: Builds & executes the current project\n"\
"\n"\
"Arguments:\n"\
"  --release (-r): Builds the project with the release flags\n\n"\

void print_usage(FILE* stream) {
	fprintf(stream, USAGE_TEXT);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		cee_log(ERROR, "No arguments given\n");
		print_usage(stderr);
		return 1;
	}
	else if (argc == 2) {
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			print_usage(stdout);
			return 0;
		}
		else if (strcmp(argv[1], "build") == 0 || strcmp(argv[1], "b") == 0) {
			cee_build(BUILD_DEBUG);
			return 0;
		}
		else if (strcmp(argv[1], "run") == 0 || strcmp(argv[1], "r") == 0) {
			cee_run(BUILD_DEBUG);
			return 0;
		}
		else if (strcmp(argv[1], "new") == 0 || strcmp(argv[1], "n") == 0) {
			cee_log(ERROR, "Invalid command usage, expected project name\n");
			print_usage(stderr);
			return 1;
		}
		else {
			cee_log(ERROR, "Unexpected argument: %s\n", argv[1]);
			print_usage(stderr);
			return 1;
		}
	}
	else if (argc == 3) {
		if (strcmp(argv[1], "build") == 0 || strcmp(argv[1], "b") == 0) {
			if (strcmp(argv[2], "--release") == 0 || strcmp(argv[2], "-r") == 0) {
				cee_build(BUILD_RELEASE);
				return 0;
			}
			else {
				cee_log(ERROR, "Unexpected argument: %s\n", argv[2]);
				print_usage(stderr);
				return 1;
			}
		}
		else if (strcmp(argv[1], "run") == 0 || strcmp(argv[1], "r") == 0) {
			if (strcmp(argv[2], "--release") == 0 || strcmp(argv[2], "-r") == 0) {
				cee_run(BUILD_RELEASE);
				return 0;
			}
			else {
				cee_log(ERROR, "Unexpected argument: %s\n", argv[2]);
				print_usage(stderr);
				return 1;
			}
		}
		else if (strcmp(argv[1], "new") == 0) {
			cee_new(argv[2]);
			return 0;
		}
		else {
			cee_log(ERROR, "Unexpected argument: %s\n", argv[2]);
			print_usage(stderr);
			return 1;
		}
	}
	else {
		// Unexpected number of arguments
	}

	return 0;
}
