/* Recursive mutexes for Mac OS X 10.0 and 10.1 (see include/pthread.h):
 * the system's mutex, with the thread that holds it and how many times. */
#define __DOS_SDK_C99_NO_MUTEX_MACROS
#include <errno.h>
#include <pthread.h>

#ifdef __DOS_SDK_C99_PTHREAD_H

/* __dos_sdk_mutex_init: makes a mutex of the attributes' type. */
int __dos_sdk_mutex_init(__dos_sdk_mutex_t *m, const __dos_sdk_mutexattr_t *a) {
  m->__owner = 0;
  m->__count = 0;
  m->__type = a ? a->__type : PTHREAD_MUTEX_NORMAL;
  return pthread_mutex_init(&m->__m, a ? &a->__a : NULL);
}

/* __dos_sdk_mutex_destroy: frees the system's mutex. */
int __dos_sdk_mutex_destroy(__dos_sdk_mutex_t *m) { return pthread_mutex_destroy(&m->__m); }

/* Holds: whether this thread holds the mutex. */
static int Holds(__dos_sdk_mutex_t *m) { return m->__count && pthread_equal(m->__owner, pthread_self()); }

/* __dos_sdk_mutex_lock: takes the mutex; a recursive one again when this
 * thread holds it. */
int __dos_sdk_mutex_lock(__dos_sdk_mutex_t *m) {
  if (m->__type != PTHREAD_MUTEX_NORMAL && Holds(m)) {
    if (m->__type == PTHREAD_MUTEX_ERRORCHECK) return EDEADLK;
    m->__count++;
    return 0;
  }
  int r = pthread_mutex_lock(&m->__m);
  if (r == 0) {
    m->__owner = pthread_self();
    m->__count = 1;
  }
  return r;
}

/* __dos_sdk_mutex_trylock: takes the mutex if no other thread holds it. */
int __dos_sdk_mutex_trylock(__dos_sdk_mutex_t *m) {
  if (m->__type == PTHREAD_MUTEX_RECURSIVE && Holds(m)) {
    m->__count++;
    return 0;
  }
  int r = pthread_mutex_trylock(&m->__m);
  if (r == 0) {
    m->__owner = pthread_self();
    m->__count = 1;
  }
  return r;
}

/* __dos_sdk_mutex_unlock: lets the mutex go when the count reaches 0. */
int __dos_sdk_mutex_unlock(__dos_sdk_mutex_t *m) {
  if (m->__type != PTHREAD_MUTEX_NORMAL) {
    if (!Holds(m)) return EPERM;
    if (--m->__count) return 0;
  }
  m->__count = 0;
  m->__owner = 0;
  return pthread_mutex_unlock(&m->__m);
}

/* __dos_sdk_mutexattr_init: attributes of a normal mutex. */
int __dos_sdk_mutexattr_init(__dos_sdk_mutexattr_t *a) {
  a->__type = PTHREAD_MUTEX_NORMAL;
  return pthread_mutexattr_init(&a->__a);
}

/* __dos_sdk_mutexattr_destroy: frees the system's attributes. */
int __dos_sdk_mutexattr_destroy(__dos_sdk_mutexattr_t *a) { return pthread_mutexattr_destroy(&a->__a); }

/* __dos_sdk_mutexattr_settype: sets the type of the mutex. */
int __dos_sdk_mutexattr_settype(__dos_sdk_mutexattr_t *a, int type) {
  if (type < PTHREAD_MUTEX_NORMAL || type > PTHREAD_MUTEX_RECURSIVE) return EINVAL;
  a->__type = type;
  return 0;
}

/* __dos_sdk_mutexattr_gettype: gives the type of the mutex. */
int __dos_sdk_mutexattr_gettype(const __dos_sdk_mutexattr_t *a, int *type) {
  *type = a->__type;
  return 0;
}

/* __dos_sdk_cond_wait: waits on the condition. The mutex is let go whole
 * while the thread waits, and held again as before. */
int __dos_sdk_cond_wait(pthread_cond_t *c, __dos_sdk_mutex_t *m) {
  unsigned count = m->__count;
  m->__count = 0;
  m->__owner = 0;
  int r = pthread_cond_wait(c, &m->__m);
  m->__owner = pthread_self();
  m->__count = count ? count : 1;
  return r;
}

/* __dos_sdk_cond_timedwait: waits on the condition until the time. */
int __dos_sdk_cond_timedwait(pthread_cond_t *c, __dos_sdk_mutex_t *m, const struct timespec *t) {
  unsigned count = m->__count;
  m->__count = 0;
  m->__owner = 0;
  int r = pthread_cond_timedwait(c, &m->__m, t);
  m->__owner = pthread_self();
  m->__count = count ? count : 1;
  return r;
}

#endif
