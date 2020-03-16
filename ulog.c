#include "ulog.h"

#include <stdio.h>
#include <stdarg.h>

#if ULOG_ENABLE_TIME
# ifdef _WIN32
# include "Windows.h"
# endif

void ulog_gettime(time_t *tv_sec, long *tv_nsec)
{
# ifdef _WIN32
    __int64 wintime;
    GetSystemTimeAsFileTime((FILETIME*)&wintime);
    wintime -= 116444736000000000ll;        //1jan1601 to 1jan1970
    *tv_sec  = wintime / 10000000ll;        //seconds
    *tv_nsec = wintime % 10000000ll * 100;  //nano-seconds
# else
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    *tv_sec = now.tv_sec;
    *tv_nsec = now.tv_nsec;
# endif
}
#endif

// ulog default print function
static int ulog_default_print(void* userdata, int tag, const char* line);


// ulog global variables
static print_func m_print = ulog_default_print;
static void* m_userdata = NULL;
static int   m_filter = ULOG_LV_ALL;

// just print to stdout
static int ulog_default_print(void* userdata, int tag, const char* line)
{
    return printf("%s", line);
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
#define COLOR_TIME  "\x1b[92m"
#define COLOR_RESET "\x1b[0m"


void ulog_log(int tag, const char *file, int lineno, const char *func, const char *format, ...)
{
    // ignore log tag not in filters
    if (!(tag & m_filter)) {
        return;
    }

    int idx = 0;
    char line[ULOG_LINE_MAXCHAR];

#if ULOG_ENABLE_ADDR
    // get file name from file path
    for (const char* p = file; *p; p++) {
        if (*p == '/' || *p == '\\') {
            file = p + 1;
        }
    }
#endif // ULOG_ENABLE_ADDR


#if ULOG_ENABLE_TIME
    // prcess timestamp, tag name, address, function
    // get timestamp
    time_t tv_sec;
    long tv_nsec;
    ulog_gettime(&tv_sec, &tv_nsec);
    struct tm *tminfo = localtime(&tv_sec);
# if ULOG_ENABLE_COLOR
    idx += snprintf(line + idx, sizeof(line) - idx, "%s", COLOR_TIME);
# endif
# if ULOG_ENABLE_DATE
    idx += strftime(line + idx, sizeof(line) - idx, "%Y-%m-%d %H:%M:%S", tminfo);
# else
    idx += strftime(line + idx, sizeof(line) - idx, "%H:%M:%S", tminfo);
# endif
# if ULOG_ENABLE_MILLISECOND
    idx += snprintf(line + idx, sizeof(line) - idx, ".%03d", tv_nsec / 1000000L);
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

