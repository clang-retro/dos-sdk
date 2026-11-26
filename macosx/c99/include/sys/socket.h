/* <sys/socket.h> with socklen_t, which the Mac OS X headers before 10.2 do
 * not define. Their functions take int lengths, so socklen_t is int. */
#include <sys/types.h> /* the 10.0 header uses its types without it */
#include_next <sys/socket.h>

#if !defined(__DOS_SDK_C99_SYS_SOCKET_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_SYS_SOCKET_H
typedef int socklen_t;
#endif
