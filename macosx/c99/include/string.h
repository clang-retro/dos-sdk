/* <string.h> with the declarations of libc99.a's functions that Mac OS X
 * before 10.3 lacks (src/posix.c). */
#include_next <string.h>

#if !defined(__DOS_SDK_C99_STRING_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1030
#define __DOS_SDK_C99_STRING_H
#ifdef __cplusplus
extern "C" {
#endif
int strerror_r(int, char *, size_t);
#ifdef __cplusplus
}
#endif
#endif
