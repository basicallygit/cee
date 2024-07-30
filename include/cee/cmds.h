#ifndef CEE_CMDS_H
#define CEE_CMDS_H
#include "cee/types.h"

void cee_build(CeeBuildMode build_mode);
void cee_run(CeeBuildMode build_mode);
void cee_new(const char* project_name);

#endif
