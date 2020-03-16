#ifndef ULOG_H
#define ULOG_H

// tiny c log library for debugging
// author by TyK
// Github: https://github.com/lazytinker/ulog

#ifdef __cplusplus
extern "C" {
#endif

// configuration macros
// max chars in one line, including terminal null char
#ifndef ULOG_LINE_MAXCHAR
# define ULOG_LINE_MAXCHAR   512
#endif

// enable color by default
#ifndef ULOG_DISABLE_COLOR
# define ULOG_ENABLE_COLOR 1
#endif

// enable log address like file:linenumber by default
#ifndef ULOG_DISABLE_ADDR
# define ULOG_ENABLE_ADDR 1
#endif

// enable timestamp by default
#ifndef ULOG_DISABLE_TIME
# define ULOG_ENABLE_TIME 1

// enable millisecond in timestamp by default
# ifndef ULOG_DISABLE_MILLISECOND
#  define ULOG_ENABLE_MILLISECOND 1
# endif

// enable date in timestamp by default
# ifndef ULOG_DISABLE_DATE
#  define ULOG_ENABLE_DATE 1
# endif
#endif

// enable function name by default
#ifndef ULOG_DISABLE_FUNC
# define ULOG_ENABLE_FUNC 1
#endif

// log tags
#define ULOG_TAG_TRACE   (1 << 0)
#define ULOG_TAG_DEBUG   (1 << 1)
#define ULOG_TAG_INFO    (1 << 2)
#define ULOG_TAG_WARN    (1 << 3)
#define ULOG_TAG_ERROR   (1 << 4)


#define ULOG_LV_NONE    (0)
#define ULOG_LV_ERROR   (ULOG_TAG_ERROR)
#define ULOG_LV_WARN    (ULOG_TAG_WARN | ULOG_LV_ERROR)
#define ULOG_LV_INFO    (ULOG_TAG_INFO | ULOG_LV_WARN)
#define ULOG_LV_DEBUG   (ULOG_TAG_DEBUG | ULOG_LV_INFO)
#define ULOG_LV_TRACE   (ULOG_TAG_TRACE | ULOG_LV_DEBUG)
#define ULOG_LV_ALL     (ULOG_LV_TRACE)

// print function
// tag: log tag
// line: log message line with newline
typedef int (*print_func)(void* userdata, int tag, const char* line);


// init ulog, user can use default if not call this.
// func: user defined print function, if NULL, using default
// userdata: userdata pointer pass to print function
// filter: tag filter allow what tag log would be print
void ulog_init(print_func func, void *userdata, int filter);


// ulog log process function
void ulog_log(int tag, const char *file, int lineno, const char *func, const char *format, ...);


// log macros, use these macros to print log
#define	LOG_TRACE(...)  ulog_log(ULOG_TAG_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define	LOG_DEBUG(...)  ulog_log(ULOG_TAG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define	LOG_INFO(...)   ulog_log(ULOG_TAG_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define	LOG_WARN(...)   ulog_log(ULOG_TAG_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define	LOG_ERROR(...)  ulog_log(ULOG_TAG_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)


// helper function, like clock_gettime(CLOCK_REALTIME, ...)
#if ULOG_ENABLE_TIME
# include <time.h>
void ulog_gettime(time_t *tv_sec, long *tv_nsec);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ULOG_H */

