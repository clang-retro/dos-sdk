/* <arpa/inet.h> with inet_pton and inet_ntop, which Mac OS X before 10.2
 * lacks (src/inet.c). */
#include <sys/types.h> /* the 10.0 header uses its types without it */
#include_next <arpa/inet.h>

#if !defined(__DOS_SDK_C99_ARPA_INET_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_ARPA_INET_H
#include <sys/socket.h>
#ifdef __cplusplus
extern "C" {
#endif
int inet_pton(int, const char *, void *);
const char *inet_ntop(int, const void *, char *, socklen_t);
#ifdef __cplusplus
}
#endif
#endif
