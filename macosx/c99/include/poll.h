/* <poll.h> for Mac OS X 10.0 to 10.2, which have none: poll of libc99.a
 * (src/posix.c), over select. Where the system has a <poll.h>, it is
 * used. */
#if __has_include_next(<poll.h>)
#include_next <poll.h>
#elif !defined(__DOS_SDK_C99_POLL_H)
#define __DOS_SDK_C99_POLL_H

struct pollfd {
  int fd;
  short events;
  short revents;
};
typedef unsigned int nfds_t;

#define POLLIN 0x0001
#define POLLPRI 0x0002
#define POLLOUT 0x0004
#define POLLRDNORM 0x0040
#define POLLWRNORM POLLOUT
#define POLLRDBAND 0x0080
#define POLLWRBAND 0x0100
#define POLLERR 0x0008
#define POLLHUP 0x0010
#define POLLNVAL 0x0020

#ifdef __cplusplus
extern "C" {
#endif
int poll(struct pollfd *, nfds_t, int);
#ifdef __cplusplus
}
#endif
#endif
