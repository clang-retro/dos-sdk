/* The C99 <stdlib.h> functions Mac OS X 10.0 to 10.2 lack, from libc99.a
 * (scripts/build-c99-macosx.sh). Link with -lc99. */
#include_next <stdlib.h>

#if !defined(__DOS_SDK_C99_STDLIB_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1030
#define __DOS_SDK_C99_STDLIB_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  long long quot;
  long long rem;
} lldiv_t;

long long atoll(const char *);
long long llabs(long long);
lldiv_t lldiv(long long, long long);
long long strtoll(const char *, char **, int);
unsigned long long strtoull(const char *, char **, int);
float strtof(const char *, char **);
long double strtold(const char *, char **);
void _Exit(int) __attribute__((__noreturn__));

#ifdef __cplusplus
}
#endif
#endif
