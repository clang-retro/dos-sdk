/* POSIX and system functions that Mac OS X 10.0 to 10.3 lack, over the
 * ones they have. The build defines HAVE_name for each function that the
 * system library already has, and this file leaves those out. */
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <mach-o/dyld.h>
#include <pthread.h>
#include <pwd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>

/* The functions that copy a static result take this lock. */
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

#ifndef HAVE_localtime_r
/* localtime_r: the local time, into the caller's structure. */
struct tm *localtime_r(const time_t *t, struct tm *out) {
  pthread_mutex_lock(&g_lock);
  struct tm *r = localtime(t);
  if (r) *out = *r;
  pthread_mutex_unlock(&g_lock);
  return r ? out : NULL;
}
#endif
#ifndef HAVE_gmtime_r
/* gmtime_r: the universal time, into the caller's structure. */
struct tm *gmtime_r(const time_t *t, struct tm *out) {
  pthread_mutex_lock(&g_lock);
  struct tm *r = gmtime(t);
  if (r) *out = *r;
  pthread_mutex_unlock(&g_lock);
  return r ? out : NULL;
}
#endif
#ifndef HAVE_timegm
/* timegm: the time of a broken-down universal time. */
time_t timegm(struct tm *tm) {
  static const int days[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  long year = tm->tm_year + 1900L, mon = tm->tm_mon;
  year += mon / 12;
  mon %= 12;
  if (mon < 0) mon += 12, year--;
  long leaps = (year - 1969) / 4 - (year - 1901) / 100 + (year - 1601) / 400;
  long d = (year - 1970) * 365 + leaps + days[mon] + tm->tm_mday - 1;
  if (mon > 1 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) d++;
  return (time_t)(((d * 24 + tm->tm_hour) * 60 + tm->tm_min) * 60 + tm->tm_sec);
}
#endif
#ifndef HAVE_ftime
/* ftime: the time in seconds and milliseconds. */
int ftime(struct timeb *tb) {
  struct timeval tv;
  struct timezone tz;
  if (gettimeofday(&tv, &tz) != 0) return -1;
  tb->time = tv.tv_sec;
  tb->millitm = (unsigned short)(tv.tv_usec / 1000);
  tb->timezone = (short)tz.tz_minuteswest;
  tb->dstflag = (short)tz.tz_dsttime;
  return 0;
}
#endif
#ifndef HAVE_strerror_r
/* strerror_r: the message of an error number, into the caller's buffer. */
int strerror_r(int e, char *buf, size_t n) {
  pthread_mutex_lock(&g_lock);
  const char *s = strerror(e);
  size_t len = strlen(s);
  int r = len < n ? 0 : ERANGE;
  if (n) {
    memcpy(buf, s, len < n ? len + 1 : n - 1);
    if (len >= n) buf[n - 1] = 0;
  }
  pthread_mutex_unlock(&g_lock);
  return r;
}
#endif

/* CopyString: copies s into the caller's buffer; NULL when it is full. */
static char *CopyString(const char *s, char **buf, size_t *left) {
  if (!s) return NULL;
  size_t n = strlen(s) + 1;
  if (n > *left) return NULL;
  char *r = *buf;
  memcpy(r, s, n);
  *buf += n;
  *left -= n;
  return r;
}

/* CopyPasswd: copies a password entry and its strings to the caller's. */
static int CopyPasswd(struct passwd *p, struct passwd *out, char *buf, size_t n, struct passwd **result) {
  *result = NULL;
  if (!p) return 0;
  *out = *p;
  if ((p->pw_name && !(out->pw_name = CopyString(p->pw_name, &buf, &n))) ||
      (p->pw_passwd && !(out->pw_passwd = CopyString(p->pw_passwd, &buf, &n))) ||
      (p->pw_class && !(out->pw_class = CopyString(p->pw_class, &buf, &n))) ||
      (p->pw_gecos && !(out->pw_gecos = CopyString(p->pw_gecos, &buf, &n))) ||
      (p->pw_dir && !(out->pw_dir = CopyString(p->pw_dir, &buf, &n))) ||
      (p->pw_shell && !(out->pw_shell = CopyString(p->pw_shell, &buf, &n))))
    return ERANGE;
  *result = out;
  return 0;
}
#ifndef HAVE_getpwuid_r
/* getpwuid_r: the password entry of a user number. */
int getpwuid_r(uid_t uid, struct passwd *out, char *buf, size_t n, struct passwd **result) {
  pthread_mutex_lock(&g_lock);
  int r = CopyPasswd(getpwuid(uid), out, buf, n, result);
  pthread_mutex_unlock(&g_lock);
  return r;
}
#endif
#ifndef HAVE_getpwnam_r
/* getpwnam_r: the password entry of a user name. */
int getpwnam_r(const char *name, struct passwd *out, char *buf, size_t n, struct passwd **result) {
  pthread_mutex_lock(&g_lock);
  int r = CopyPasswd(getpwnam(name), out, buf, n, result);
  pthread_mutex_unlock(&g_lock);
  return r;
}
#endif

/* CopyGroup: copies a group entry, its strings and its members. */
static int CopyGroup(struct group *g, struct group *out, char *buf, size_t n, struct group **result) {
  *result = NULL;
  if (!g) return 0;
  *out = *g;
  size_t members = 0;
  while (g->gr_mem && g->gr_mem[members]) members++;
  size_t pad = (sizeof(char *) - ((uintptr_t)buf % sizeof(char *))) % sizeof(char *);
  if (pad + (members + 1) * sizeof(char *) > n) return ERANGE;
  buf += pad;
  n -= pad;
  char **mem = (char **)buf;
  buf += (members + 1) * sizeof(char *);
  n -= (members + 1) * sizeof(char *);
  for (size_t i = 0; i < members; i++)
    if (!(mem[i] = CopyString(g->gr_mem[i], &buf, &n))) return ERANGE;
  mem[members] = NULL;
  out->gr_mem = mem;
  if ((g->gr_name && !(out->gr_name = CopyString(g->gr_name, &buf, &n))) ||
      (g->gr_passwd && !(out->gr_passwd = CopyString(g->gr_passwd, &buf, &n))))
    return ERANGE;
  *result = out;
  return 0;
}
#ifndef HAVE_getgrgid_r
/* getgrgid_r: the group entry of a group number. */
int getgrgid_r(gid_t gid, struct group *out, char *buf, size_t n, struct group **result) {
  pthread_mutex_lock(&g_lock);
  int r = CopyGroup(getgrgid(gid), out, buf, n, result);
  pthread_mutex_unlock(&g_lock);
  return r;
}
#endif
#ifndef HAVE_getgrnam_r
/* getgrnam_r: the group entry of a group name. */
int getgrnam_r(const char *name, struct group *out, char *buf, size_t n, struct group **result) {
  pthread_mutex_lock(&g_lock);
  int r = CopyGroup(getgrnam(name), out, buf, n, result);
  pthread_mutex_unlock(&g_lock);
  return r;
}
#endif
#ifndef HAVE_getsid
/* getsid: the session of a process, by the system call. The 10.0 headers
 * do not name it; 310 is its number in all Darwin kernels. */
#ifndef SYS_getsid
#define SYS_getsid 310
#endif
pid_t getsid(pid_t pid) { return (pid_t)syscall(SYS_getsid, pid); }
#endif
#ifndef HAVE_pread
/* pread: reads at an offset; the file's offset is kept. Other threads must
 * not read the same descriptor at the same time. */
ssize_t pread(int fd, void *buf, size_t n, off_t at) {
  off_t was = lseek(fd, 0, SEEK_CUR);
  if (was < 0 || lseek(fd, at, SEEK_SET) < 0) return -1;
  ssize_t r = read(fd, buf, n);
  int e = errno;
  lseek(fd, was, SEEK_SET);
  errno = e;
  return r;
}
#endif
#ifndef HAVE_pwrite
/* pwrite: writes at an offset; the file's offset is kept. */
ssize_t pwrite(int fd, const void *buf, size_t n, off_t at) {
  off_t was = lseek(fd, 0, SEEK_CUR);
  if (was < 0 || lseek(fd, at, SEEK_SET) < 0) return -1;
  ssize_t r = write(fd, buf, n);
  int e = errno;
  lseek(fd, was, SEEK_SET);
  errno = e;
  return r;
}
#endif
#ifndef HAVE_futimes
/* futimes: the system has no call that sets the times of an open file. */
int futimes(int fd, const struct timeval *tv) {
  (void)fd;
  (void)tv;
  errno = ENOSYS;
  return -1;
}
#endif
#ifndef HAVE_poll
/* poll: waits for the descriptors with select. */
int poll(struct pollfd *fds, nfds_t n, int timeout) {
  fd_set r, w, x;
  FD_ZERO(&r);
  FD_ZERO(&w);
  FD_ZERO(&x);
  int top = -1;
  for (nfds_t i = 0; i < n; i++) {
    fds[i].revents = 0;
    if (fds[i].fd < 0) continue;
    if (fds[i].fd >= FD_SETSIZE) {
      errno = EINVAL;
      return -1;
    }
    if (fds[i].events & (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI)) FD_SET(fds[i].fd, &r);
    if (fds[i].events & (POLLOUT | POLLWRBAND)) FD_SET(fds[i].fd, &w);
    FD_SET(fds[i].fd, &x);
    if (fds[i].fd > top) top = fds[i].fd;
  }
  struct timeval tv, *ptv = NULL;
  if (timeout >= 0) {
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    ptv = &tv;
  }
  int got = select(top + 1, &r, &w, &x, ptv);
  if (got <= 0) return got;
  int count = 0;
  for (nfds_t i = 0; i < n; i++) {
    int fd = fds[i].fd;
    if (fd < 0) continue;
    if (FD_ISSET(fd, &r)) fds[i].revents |= fds[i].events & (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI);
    if (FD_ISSET(fd, &w)) fds[i].revents |= fds[i].events & (POLLOUT | POLLWRBAND);
    if (FD_ISSET(fd, &x)) fds[i].revents |= POLLERR;
    if (fds[i].revents) count++;
  }
  return count;
}
#endif

/* The spin locks of <libkern/OSAtomic.h> (10.4), which compiler-rt's
 * atomic functions use: an int that is 0 when free. */
#ifndef HAVE_OSSpinLockTry
/* OSSpinLockTry: takes the lock if it is free. */
int OSSpinLockTry(volatile int *lock) { return __sync_lock_test_and_set(lock, 1) == 0; }
#endif
#ifndef HAVE_OSSpinLockLock
/* OSSpinLockLock: takes the lock, and gives the processor away while it
 * waits. */
void OSSpinLockLock(volatile int *lock) {
  while (__sync_lock_test_and_set(lock, 1) != 0) sched_yield();
}
#endif
#ifndef HAVE_OSSpinLockUnlock
/* OSSpinLockUnlock: lets the lock go. */
void OSSpinLockUnlock(volatile int *lock) { __sync_lock_release(lock); }
#endif

#ifndef HAVE___cxa_atexit
/* The destructors that __cxa_atexit registers, run at exit, last first. A
 * bundle is never unloaded, so the image (dso) is not looked at. */
struct exit_fn {
  void (*fn)(void *);
  void *arg;
  struct exit_fn *next;
};
static struct exit_fn *g_exitFns;

/* RunExitFns: runs the registered destructors. */
static void RunExitFns(void) {
  for (;;) {
    pthread_mutex_lock(&g_lock);
    struct exit_fn *f = g_exitFns;
    if (f) g_exitFns = f->next;
    pthread_mutex_unlock(&g_lock);
    if (!f) return;
    f->fn(f->arg);
    free(f);
  }
}

/* __cxa_atexit: registers the destructor of a static object. */
int __cxa_atexit(void (*fn)(void *), void *arg, void *dso) {
  (void)dso;
  static int registered;
  struct exit_fn *f = (struct exit_fn *)malloc(sizeof *f);
  if (!f) return -1;
  f->fn = fn;
  f->arg = arg;
  pthread_mutex_lock(&g_lock);
  f->next = g_exitFns;
  g_exitFns = f;
  int first = !registered;
  registered = 1;
  pthread_mutex_unlock(&g_lock);
  if (first) atexit(RunExitFns);
  return 0;
}
#endif

#ifndef HAVE__NSGetExecutablePath
/* _NSGetExecutablePath: the path of the program, from dyld's first image. */
int _NSGetExecutablePath(char *buf, uint32_t *size) {
  const char *p = _dyld_get_image_name(0);
  if (!p) p = "";
  size_t n = strlen(p) + 1;
  if (n > *size) {
    *size = (uint32_t)n;
    return -1;
  }
  memcpy(buf, p, n);
  return 0;
}
#endif
