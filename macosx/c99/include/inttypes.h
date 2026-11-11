/* The C99 <inttypes.h> functions Mac OS X 10.0 to 10.2 lack, from libc99.a
 * (scripts/build-c99-macosx.sh). Link with -lc99. */
#include_next <inttypes.h>

#if !defined(__DOS_SDK_C99_INTTYPES_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1030
#define __DOS_SDK_C99_INTTYPES_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  intmax_t quot;
  intmax_t rem;
} imaxdiv_t;

intmax_t imaxabs(intmax_t);
imaxdiv_t imaxdiv(intmax_t, intmax_t);
intmax_t strtoimax(const char *, char **, int);
uintmax_t strtoumax(const char *, char **, int);

#ifdef __cplusplus
}
#endif
#endif
