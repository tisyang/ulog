#ifndef ULOG_UTILS_H
#define ULOG_UTILS_H

#include "ulog.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static int _ulog_default_log(void *userdata, int tag, const char *line)
{
    int rc = printf("%s", line);
    if (userdata) {
        FILE *fp = userdata;
        fputs(line, fp);
        fflush(fp);
    }
    return rc;
}

// init ulog with default configuration.
// will log to stdout and a log file in current work directory.
// app: argv[0] program name, using for log file name. 'app.log.date_time'
// using filter if ENV var ULOG_LEVEL set Trace/Debug/Info/Warn/Error.
static void ulog_init_default(const char *app)
{
    int lv = ULOG_LV_ALL;
    char *env = getenv("ULOG_LEVEL");
    if (env) {
        if (strcasecmp(env, "Trace") == 0) {
            lv = ULOG_LV_TRACE;
        } else if (strcasecmp(env, "Debug") == 0) {
            lv = ULOG_LV_DEBUG;
        } else if (strcasecmp(env, "Info") == 0) {
            lv = ULOG_LV_INFO;
        } else if (strcasecmp(env, "Warn") == 0) {
            lv = ULOG_LV_WARN;
        } else if (strcasecmp(env, "Error") == 0) {
            lv = ULOG_LV_ERROR;
        } else {
            printf("invalid ulog ENV '%s' = '%s\n', using default",
                     "ULOG_LEVEL", env);
            env = "Trace";
        }
        printf("ulog level=%s\n", env);
    }

    for (const char *p = app; *p; p++) {
        if (*p == '/' || *p == '\\') {
            app = p + 1;
        }
    }
    time_t now = time(NULL);
    char tim[32];
    strftime(tim, sizeof(tim), "%y%m%d_%H%M", localtime(&now));

    char buf[64];
    snprintf(buf, sizeof(buf), "%s.log.%s", app, tim);

    FILE* logf = fopen(buf, "w");
    if (logf) {
        printf("ulog create log file '%s' OK\n", buf);
        ulog_init(_ulog_default_log, logf, lv);
    } else {
        printf("ulog create log file '%s' FAILED, %s\n", buf, strerror(errno));
        ulog_init(NULL, NULL, lv);
    }
}

#ifdef __cplusplus
}
#endif
#endif // ULOG_UTILS_H
