#pragma once
#include <stdbool.h>

// Recent text from all four games' crash.log files, including inactive ROM regions.
// Reading only the tail keeps repeated failures from filling the clipboard indefinitely.
enum { ERROR_LOG_TAIL = 4096, ERROR_LOG_SIZE = 4 * (ERROR_LOG_TAIL + 128) + 1 };
bool error_log_read(const char *cache, char out[ERROR_LOG_SIZE]);
