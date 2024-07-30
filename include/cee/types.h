#ifndef CEE_TYPES_H
#define CEE_TYPES_H

typedef enum {
	BUILD_DEBUG,
	BUILD_RELEASE,
} CeeBuildMode;

typedef struct {
	char* name;
	char* compiler;
	char* debug_flags;
	char* release_flags;
	char* source_files;
	char* bin_directory;
	char* version;
} CeeConfig;

void CeeConfigInit(CeeConfig* config);
void CeeConfigFree(CeeConfig* config);

#endif
