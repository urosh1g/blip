#include <stdarg.h>

#include <logger/logger.h>

#define LOG_MAX_SINKS 8

#ifdef __unix__
#define LOG_STYLE_RESET "\x1b[0m"
#define LOG_STYLE_BOLD "\x1b[1m"
#define LOG_COLOR_BLUE "\x1b[34m"
#define LOG_COLOR_MAGENTA "\x1b[35m"
#define LOG_COLOR_RED "\x1b[31m"
#define LOG_COLOR_YELLOW "\x1b[33m"
#define LOG_BG_WHITE "\x1b[47m"
#endif

static log_sink_t sinks[LOG_MAX_SINKS] = {(log_sink_t){
                                              .level = LOG_DEBUG,
                                              .sink = NULL,
                                          },
                                          {0, 0}};
static size_t sinks_len = 1;

static const char* log_levels[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static const char* log_colors[] = {LOG_COLOR_MAGENTA, LOG_COLOR_BLUE, LOG_COLOR_YELLOW, LOG_COLOR_RED, LOG_COLOR_RED LOG_BG_WHITE};

static inline const char* str_log_level(log_level_t level) {
    if (level >= LOG_NUMLEVELS) {
        return log_levels[LOG_NUMLEVELS - 1];
    }
    return log_levels[level];
}

void log_add_sink(log_sink_t sink) {
    if (sinks_len >= LOG_MAX_SINKS) {
        sinks_len = 1;
    }
    sinks[sinks_len++] = sink;
}

void _log(log_level_t level, const char* file, int line, const char* fmt, ...) {
    va_list args;

    for (size_t i = 0; i < sinks_len; i++) {
        FILE* sink = sinks[i].sink;
        if (level < sinks[i].level) {
            continue;
        }
        if (sink == NULL) {
            sink = stdout;
        }
#ifdef __unix__
        fprintf(sink, "[" "%s" LOG_STYLE_BOLD "%s" LOG_STYLE_RESET "] %s:%d ",
                log_colors[level], str_log_level(level), file, line);
        va_start(args, fmt);
        vfprintf(sink, fmt, args);
        fprintf(sink, "\n");
        va_end(args);
        fflush(sink);
#else
        fprintf(sink, "[%s] %s:%d ", str_log_level(level), file, line);
        va_start(args, fmt);
        vfprintf(sink, fmt, args);
        fprintf(sink, "\n");
        va_end(args);
        fflush(sink);
#endif
    }
}
