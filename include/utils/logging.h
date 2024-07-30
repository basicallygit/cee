#ifndef UTILS_LOGGING_H
#define UTILS_LOGGING_H

typedef enum {
	INFO,
	WARN,
	ERROR,
	EXEC,
} LogLevel;

void cee_log(LogLevel loglevel, const char* format_string, ...);

#endif
