#include "ulog.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// ulog default print function
static int ulog_default_print(void* userdata, int tag, const char* line);


// ulog global variables
static print_func m_print = ulog_default_print;
static void* m_userdata = NULL;
static int   m_filter = ULOG_LV_ALL;

// just print to stdout
static int ulog_default_print(void* userdata, int tag, const char* line)
{
    printf("%s", line);
}

static const char* const TAG_NAMES[] = {
    [ULOG_TAG_TRACE]     = "[TRACE]",
    [ULOG_TAG_DEBUG]     = "[DEBUG]",
    [ULOG_TAG_INFO]      = "[INFO ]",
    [ULOG_TAG_WARN]      = "[WARN ]",
    [ULOG_TAG_ERROR]     = "[ERROR]",
};

static const char* const TAG_COLORS[] = {
    [ULOG_TAG_TRACE]     = "\x1b[94m",
    [ULOG_TAG_DEBUG]     = "\x1b[36m",
    [ULOG_TAG_INFO]      = "\x1b[32m",
    [ULOG_TAG_WARN]      = "\x1b[33m",
    [ULOG_TAG_ERROR]     = "\x1b[31m",
};
#define COLOR_ADDR  "\x1b[90m"
#define COLOR_FUNC  COLOR_ADDR
#define COLOR_RESET "\x1b[0m"


void ulog_log(int tag, const char *file, int lineno, const char *func, const char *format, ...)
{
    // ignore log tag not in filters
    if (!(tag & m_filter)) {
        return;
    }

    // get file name from file path
    for (const char* p = file; *p; p++) {
        if (*p == '/' || *p == '\\') {
            file = p + 1;
        }
    }
    // get timestamp
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    struct tm *tminfo = localtime(&now.tv_sec);

    int idx = 0;
    char line[ULOG_LINE_MAXCHAR];
    // prcess timestamp, tag name, address, function
#if ULOG_ENABLE_TIME
# if ULOG_ENABLE_DATE
    idx += strftime(line + idx, sizeof(line) - idx, "%F %T", tminfo);
# else
    idx += strftime(line + idx, sizeof(line) - idx, "%T", tminfo);
# endif
# if ULOG_ENABLE_MILLISECOND
    idx += snprintf(line + idx, sizeof(line) - idx, ".%03d", now.tv_nsec / 1000000L);
# endif
#endif

    idx += snprintf(line + idx, sizeof(line) - idx,
#if ULOG_ENABLE_COLOR
                    " %s%s", TAG_COLORS[tag], TAG_NAMES[tag]);
#else
                    " %s", TAG_NAMES[tag]);
#endif

#if ULOG_ENABLE_ADDR
    idx += snprintf(line + idx, sizeof(line) - idx,
# if ULOG_ENABLE_COLOR
                    " %s%s:%d", COLOR_ADDR, file, lineno);
# else
                    " %s:%d", file, lineno);
# endif
#endif

#if ULOG_ENABLE_FUNC
    idx += snprintf(line + idx, sizeof(line) - idx,
# if ULOG_ENABLE_COLOR
                    " %s%s", COLOR_FUNC, func);
# else
                    " %s", func);
# endif
#endif

    idx += snprintf(line + idx, sizeof(line) - idx,
#if ULOG_ENABLE_COLOR
                    "%s: ", COLOR_RESET);
#else
                    ": ");
#endif

    va_list arglist;
    va_start(arglist, format);
    idx += vsnprintf(line + idx, sizeof(line) - idx, format, arglist);
    va_end(arglist);
    // add new line
    idx += snprintf(line + idx, sizeof(line) - idx, "\n");
    // output
    m_print(m_userdata, tag, line);
}


void ulog_init(print_func func, void *userdata, int filter)
{
    m_print = func ? func : ulog_default_print;
    m_userdata = userdata;
    m_filter = filter;
}

