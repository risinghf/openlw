/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
    (C)2016 RisingHF, all rights reserved.
*/
#ifndef __LOG_H
#define __LOG_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

#define LOG_BUF_LEN                 (260)

typedef enum{
    QUIET,
    PANIC,
    FATAL,
    ERR,
    WARN,
    INFO,
    DEBUG,
}log_t;

int log_puts(log_t level, const char *module, const char *fmt, ...);
void log_set_level(log_t level);
log_t log_get_level(void);

#define LOG(level, fmt...)              log_puts(level, (char *)NAME, fmt)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
