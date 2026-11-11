/* Mac OS X 10.0 to 10.2 have no <fenv.h>. This one has the C99 floating
 * point environment with the types and values of the 10.3 SDK; the
 * functions are picolibc's, in libc99.a. Link with -lc99. Where the system
 * has a <fenv.h>, it is used. */
#if __has_include_next(<fenv.h>)
#include_next <fenv.h>
#elif !defined(__DOS_SDK_C99_FENV_H)
#define __DOS_SDK_C99_FENV_H
#ifdef __cplusplus
extern "C" {
#endif

typedef long fenv_t;
typedef long fexcept_t;

#define FE_INEXACT 0x02000000
#define FE_DIVBYZERO 0x04000000
#define FE_UNDERFLOW 0x08000000
#define FE_OVERFLOW 0x10000000
#define FE_INVALID 0x20000000
#define FE_ALL_EXCEPT 0x3E000000

#define FE_TONEAREST 0x00000000
#define FE_TOWARDZERO 0x00000001
#define FE_UPWARD 0x00000002
#define FE_DOWNWARD 0x00000003

extern const fenv_t _FE_DFL_ENV;
#define FE_DFL_ENV (&_FE_DFL_ENV)

int feclearexcept(int);
int fegetexceptflag(fexcept_t *, int);
int feraiseexcept(int);
int fesetexceptflag(const fexcept_t *, int);
int fetestexcept(int);
int fegetround(void);
int fesetround(int);
int fegetenv(fenv_t *);
int feholdexcept(fenv_t *);
int fesetenv(const fenv_t *);
int feupdateenv(const fenv_t *);

#ifdef __cplusplus
}
#endif
#endif
