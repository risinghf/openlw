/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
    (C)2016 RisingHF, all rights reserved.
*/
#include "log.h"
#include "timer.h"
#include "utilities.h"

static log_t log_level = WARN;

const char *log_str_tab[] = {
    "QUIET",
    "PANIC",
    "FATAL",
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
};

void log_set_level(log_t level)
{
    log_level = level;
}

log_t log_get_level(void)
{
    return log_level;
}

int log_puts(log_t level, const char *module, const char *fmt, ...)
{
    int i = 0;
    va_list ap;
    FILE *fp = stdout;

    if(level > log_level){
        return 0;
    }

    if(fmt == NULL){
        return 0;
    }

    fprintf(fp, "+LOG: %5s %8lu %-10s", log_str_tab[level], TimerGetCurrentTime(), module);

    va_start(ap, fmt);

    i = vfprintf1(fp, fmt, ap);

    va_end(ap);

    fprintf(fp, "\r\n");
    i += 2;

    return i;
}
