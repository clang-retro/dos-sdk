/* <pwd.h> with the declarations of libc99.a's functions that Mac OS X
 * before 10.2 lacks (src/posix.c). */
#include <sys/types.h> /* the 10.0 header uses its types without it */
#include_next <pwd.h>

#if !defined(__DOS_SDK_C99_PWD_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_PWD_H
#ifdef __cplusplus
extern "C" {
#endif
int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **);
int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **);
#ifdef __cplusplus
}
#endif
#endif
