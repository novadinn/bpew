#ifndef LOG_H
#define LOG_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdarg.h>

static void logFmtError(const char *err, ...) {
  char *errc;
  va_list va;
  va_start(va, err);
  errc = (char *)malloc(512);
  vsnprintf(errc, 512 - 1, err, va);
  errc = (char *)realloc(errc, strlen(errc) + 1);
  va_end(va);

  printf("%s\n", errc);
  free(errc);
}

#define LOG_ERROR(fmt, ...)                                                    \
  { logFmtError(fmt, ##__VA_ARGS__); }

#define ASSERT(condition) assert(condition);

#endif // LOG_H
