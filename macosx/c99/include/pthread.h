/* <pthread.h> for Mac OS X 10.0 and 10.1, whose mutexes cannot be
 * recursive (PTHREAD_MUTEX_RECURSIVE arrived in 10.2). Programs compiled
 * with this header use a mutex type of libc99.a (src/pthread.c): the
 * system's mutex, with an owner and a count. The names of the mutex
 * functions are macros for libc99.a's functions, which also take the
 * condition variables' waits. The system's libraries never see this type.
 * From 10.2, the system's header is used as it is. */
#include_next <pthread.h>

#if !defined(__DOS_SDK_C99_PTHREAD_H) && !defined(PTHREAD_MUTEX_RECURSIVE) && \
    defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1020
#define __DOS_SDK_C99_PTHREAD_H

#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_ERRORCHECK 1
#define PTHREAD_MUTEX_RECURSIVE 2
#define PTHREAD_MUTEX_DEFAULT PTHREAD_MUTEX_NORMAL

typedef struct {
  pthread_mutex_t __m;
  pthread_t __owner;
  unsigned __count;
  int __type;
} __dos_sdk_mutex_t;
typedef struct {
  pthread_mutexattr_t __a;
  int __type;
} __dos_sdk_mutexattr_t;

#ifdef __cplusplus
extern "C" {
#endif
int __dos_sdk_mutex_init(__dos_sdk_mutex_t *, const __dos_sdk_mutexattr_t *);
int __dos_sdk_mutex_destroy(__dos_sdk_mutex_t *);
int __dos_sdk_mutex_lock(__dos_sdk_mutex_t *);
int __dos_sdk_mutex_trylock(__dos_sdk_mutex_t *);
int __dos_sdk_mutex_unlock(__dos_sdk_mutex_t *);
int __dos_sdk_mutexattr_init(__dos_sdk_mutexattr_t *);
int __dos_sdk_mutexattr_destroy(__dos_sdk_mutexattr_t *);
int __dos_sdk_mutexattr_settype(__dos_sdk_mutexattr_t *, int);
int __dos_sdk_mutexattr_gettype(const __dos_sdk_mutexattr_t *, int *);
int __dos_sdk_cond_wait(pthread_cond_t *, __dos_sdk_mutex_t *);
int __dos_sdk_cond_timedwait(pthread_cond_t *, __dos_sdk_mutex_t *, const struct timespec *);
#ifdef __cplusplus
}
#endif

#ifndef __DOS_SDK_C99_NO_MUTEX_MACROS
#undef PTHREAD_MUTEX_INITIALIZER
#define PTHREAD_MUTEX_INITIALIZER {{_PTHREAD_MUTEX_SIG_init}, 0, 0, 0}
#define pthread_mutex_t __dos_sdk_mutex_t
#define pthread_mutexattr_t __dos_sdk_mutexattr_t
#define pthread_mutex_init __dos_sdk_mutex_init
#define pthread_mutex_destroy __dos_sdk_mutex_destroy
#define pthread_mutex_lock __dos_sdk_mutex_lock
#define pthread_mutex_trylock __dos_sdk_mutex_trylock
#define pthread_mutex_unlock __dos_sdk_mutex_unlock
#define pthread_mutexattr_init __dos_sdk_mutexattr_init
#define pthread_mutexattr_destroy __dos_sdk_mutexattr_destroy
#define pthread_mutexattr_settype __dos_sdk_mutexattr_settype
#define pthread_mutexattr_gettype __dos_sdk_mutexattr_gettype
#define pthread_cond_wait __dos_sdk_cond_wait
#define pthread_cond_timedwait __dos_sdk_cond_timedwait
#endif
#endif
