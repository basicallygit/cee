#include <stdio.h>
#include <stdarg.h>
#include "utils/logging.h"

void cee_log(LogLevel loglevel, const char* format_string, ...) {
	FILE* stream;

	switch (loglevel) {
		case INFO:
			printf("[INFO] ");
			stream = stdout;
			break;
		case WARN:
			fprintf(stderr, "[WARN] ");
			stream = stderr;
			break;
		case ERROR:
			fprintf(stderr, "[ERROR] ");
			stream = stderr;
			break;
		case EXEC:
			fprintf(stdout, "[EXEC] ");
			stream = stdout;
			break;
	}

	va_list args;
	va_start(args, format_string);

	vfprintf(stream, format_string, args);

	va_end(args);
}
