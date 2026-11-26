/* <sys/time.h> with the declarations of libc99.a's functions that Mac OS X
 * before 10.2 lacks (src/posix.c). */
#include_next <sys/time.h>

#if !defined(__DOS_SDK_C99_SYS_TIME_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_SYS_TIME_H
#ifdef __cplusplus
extern "C" {
#endif
int futimes(int, const struct timeval *);
#ifdef __cplusplus
}
#endif
#endif
