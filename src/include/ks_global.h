#ifndef KS_GLOBAL_H
#define KS_GLOBAL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define KS_POOL_ALIGNMENT 16
#define KS_MAX_ALLOC_FROM_POOL ( 32 * 1024 )  // TODO: need change depend on os
#define KS_DEFAULT_POOL_SIZE ( 16 * 1024 )

#define KS_LOG_LEVEL_DEBUG 0
#define KS_LOG_LEVEL_WARNING 1
#define KS_LOG_LEVEL_ERROR  2

static int ks_default_log_level = KS_LOG_LEVEL_DEBUG;
static char *ks_log_file_path = "./log/ks.log";

#endif
