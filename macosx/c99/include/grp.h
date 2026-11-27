/* <grp.h> with the declarations of libc99.a's functions that Mac OS X
 * before 10.2 lacks (src/posix.c). */
#include <sys/types.h> /* the 10.0 header uses its types without it */
#include_next <grp.h>

#if !defined(__DOS_SDK_C99_GRP_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_GRP_H
#ifdef __cplusplus
extern "C" {
#endif
int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **);
int getgrnam_r(const char *, struct group *, char *, size_t, struct group **);
#ifdef __cplusplus
}
#endif
#endif
