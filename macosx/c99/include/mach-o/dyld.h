/* <mach-o/dyld.h> with the declarations of libc99.a's functions that Mac OS X
 * before 10.2 lacks (src/posix.c). Before 10.2, the system header does not
 * declare its functions extern "C", so it is included in such a block. */
#ifdef __cplusplus
extern "C" {
#endif
#include_next <mach-o/dyld.h>
#ifdef __cplusplus
}
#endif

#if !defined(__DOS_SDK_C99_DYLD_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_DYLD_H
#ifdef __cplusplus
extern "C" {
#endif
int _NSGetExecutablePath(char *, unsigned int *);
#ifdef __cplusplus
}
#endif
#endif
