#include "ks_global.h"
#include "ks_log.h"
#include <stdarg.h>

void
ks_log_error(int level, const char *fmt, ...)
{
	if (level >= ks_default_log_level) {
		if (logf == NULL) {
			logf = fopen(log_file_path, "a+");
		}
		char log_line[MAX_STRING_LEN];
		va_list args;
		va_start(args, fmt);
		vsnprintf(log_line, MAX_STRING_LEN, fmt, args);
		va_end(args);
		fputs(log_line, logf);
	} else {
		return;
	}
}
