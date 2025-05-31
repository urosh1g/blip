#ifndef _blip_logger_h
#define _blip_logger_h

#include <stdio.h>

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
    LOG_NUMLEVELS,
} log_level_t;

typedef struct {
    FILE* sink;
    log_level_t level;
} log_sink_t;

#define log_debug(...) _log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) _log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) _log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) _log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) _log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void log_set_level(log_level_t level);
void log_add_sink(log_sink_t sink);
void _log(log_level_t level, const char* file, int line, const char* fmt, ...);

#endif
