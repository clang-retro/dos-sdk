/* <unistd.h> with the declarations of libc99.a's functions that Mac OS X
 * before 10.2 lacks (src/posix.c). */
#include_next <unistd.h>

#if !defined(__DOS_SDK_C99_UNISTD_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_UNISTD_H
#ifdef __cplusplus
extern "C" {
#endif
ssize_t pread(int, void *, size_t, off_t);
ssize_t pwrite(int, const void *, size_t, off_t);
pid_t getsid(pid_t);
#ifdef __cplusplus
}
#endif
#endif
