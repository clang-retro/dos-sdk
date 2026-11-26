/* <time.h> with the declarations of libc99.a's functions that Mac OS X
 * before 10.2 lacks (src/posix.c), and of nanosleep, which the 10.0 and
 * 10.1 libSystem has but their time.h does not declare, and of tzname. */
#include_next <time.h>

#if !defined(__DOS_SDK_C99_TIME_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_TIME_H
#include <sys/time.h>
#ifdef __cplusplus
extern "C" {
#endif
struct tm *localtime_r(const time_t *, struct tm *);
struct tm *gmtime_r(const time_t *, struct tm *);
time_t timegm(struct tm *);
int nanosleep(const struct timespec *, struct timespec *);
extern char *tzname[2];
#ifdef __cplusplus
}
#endif
#endif
