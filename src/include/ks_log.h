#ifndef KS_LOG_H
#define KS_LOG_H

#define KS_LOG_LEVEL_DEBUG 0
#define KS_LOG_LEVEL_WARNING 1
#define KS_LOG_LEVEL_ERROR  2

#define MAX_STRING_LEN 8192

static int ks_default_log_level = KS_LOG_LEVEL_DEBUG;
static FILE *logf = NULL;
static const char *log_file_path = "/tmp/ks.log";

void ks_log_error(int level, const char *fmt, ...);

#endif
