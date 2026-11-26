/* <sys/timeb.h> with the declarations of libc99.a's functions that Mac OS X
 * before 10.4 lacks (src/posix.c). */
#include_next <sys/timeb.h>

#if !defined(__DOS_SDK_C99_SYS_TIMEB_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1040
#define __DOS_SDK_C99_SYS_TIMEB_H
#ifdef __cplusplus
extern "C" {
#endif
int ftime(struct timeb *);
#ifdef __cplusplus
}
#endif
#endif
