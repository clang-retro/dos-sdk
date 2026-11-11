/* The C99 <math.h> functions Mac OS X 10.0 to 10.2 lack, from libc99.a
 * (scripts/build-c99-macosx.sh). Link with -lc99. */
#include_next <math.h>

#if !defined(__DOS_SDK_C99_MATH_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1030
#define __DOS_SDK_C99_MATH_H
#ifdef __cplusplus
extern "C" {
#endif

typedef float float_t;
typedef double double_t;

/* The C99 macros, on clang's builtins, with the values of the 10.3 SDK,
 * except FP_ILOGB0 and FP_ILOGBNAN, which are what picolibc's ilogbf
 * returns. */
#define FP_SNAN 0 /* as in CarbonCore's fp.h */
#define FP_QNAN 1
#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5
#define FP_ILOGB0 (-2147483647)
#define FP_ILOGBNAN (-2147483647 - 1)
#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2
#define math_errhandling MATH_ERREXCEPT
#define HUGE_VALF __builtin_huge_valf()
#define HUGE_VALL __builtin_huge_vall()
#define INFINITY __builtin_inff()
#define NAN __builtin_nanf("")
#define fpclassify(x) __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, (x))
#define isfinite(x) __builtin_isfinite(x)
#define isinf(x) __builtin_isinf(x)
#define isnan(x) __builtin_isnan(x)
#define isnormal(x) __builtin_isnormal(x)
#define signbit(x) __builtin_signbit(x)
#define isgreater(x, y) __builtin_isgreater((x), (y))
#define isgreaterequal(x, y) __builtin_isgreaterequal((x), (y))
#define isless(x, y) __builtin_isless((x), (y))
#define islessequal(x, y) __builtin_islessequal((x), (y))
#define islessgreater(x, y) __builtin_islessgreater((x), (y))
#define isunordered(x, y) __builtin_isunordered((x), (y))

/* The names are in parentheses: CarbonCore's fp.h defines some of them
 * (acosl and the like) as macros. */

/* double */
double (exp2)(double);
double (fdim)(double, double);
double (fma)(double, double, double);
double (fmax)(double, double);
double (fmin)(double, double);
long long (llrint)(double);
long long (llround)(double);
double (log2)(double);
long (lrint)(double);
long (lround)(double);
double (nan)(const char *);
double (nearbyint)(double);
double (nexttoward)(double, long double);
double (remquo)(double, double, int *);
double (round)(double);
double (scalbln)(double, long);
double (tgamma)(double);
double (trunc)(double);

/* float */
float (acosf)(float);
float (acoshf)(float);
float (asinf)(float);
float (asinhf)(float);
float (atanf)(float);
float (atan2f)(float, float);
float (atanhf)(float);
float (cbrtf)(float);
float (ceilf)(float);
float (copysignf)(float, float);
float (cosf)(float);
float (coshf)(float);
float (erff)(float);
float (erfcf)(float);
float (expf)(float);
float (exp2f)(float);
float (expm1f)(float);
float (fabsf)(float);
float (fdimf)(float, float);
float (floorf)(float);
float (fmaf)(float, float, float);
float (fmaxf)(float, float);
float (fminf)(float, float);
float (fmodf)(float, float);
float (frexpf)(float, int *);
float (hypotf)(float, float);
int (ilogbf)(float);
float (ldexpf)(float, int);
float (lgammaf)(float);
long long (llrintf)(float);
long long (llroundf)(float);
float (logf)(float);
float (log10f)(float);
float (log1pf)(float);
float (log2f)(float);
float (logbf)(float);
long (lrintf)(float);
long (lroundf)(float);
float (modff)(float, float *);
float (nanf)(const char *);
float (nearbyintf)(float);
float (nextafterf)(float, float);
float (nexttowardf)(float, long double);
float (powf)(float, float);
float (remainderf)(float, float);
float (remquof)(float, float, int *);
float (rintf)(float);
float (roundf)(float);
float (scalblnf)(float, long);
float (scalbnf)(float, int);
float (sinf)(float);
float (sinhf)(float);
float (sqrtf)(float);
float (tanf)(float);
float (tanhf)(float);
float (tgammaf)(float);
float (truncf)(float);

/* long double, which is double before 10.4 */
long double (acosl)(long double);
long double (acoshl)(long double);
long double (asinl)(long double);
long double (asinhl)(long double);
long double (atanl)(long double);
long double (atan2l)(long double, long double);
long double (atanhl)(long double);
long double (cbrtl)(long double);
long double (ceill)(long double);
long double (copysignl)(long double, long double);
long double (cosl)(long double);
long double (coshl)(long double);
long double (erfl)(long double);
long double (erfcl)(long double);
long double (expl)(long double);
long double (exp2l)(long double);
long double (expm1l)(long double);
long double (fabsl)(long double);
long double (fdiml)(long double, long double);
long double (floorl)(long double);
long double (fmal)(long double, long double, long double);
long double (fmaxl)(long double, long double);
long double (fminl)(long double, long double);
long double (fmodl)(long double, long double);
long double (frexpl)(long double, int *);
long double (hypotl)(long double, long double);
int (ilogbl)(long double);
long double (ldexpl)(long double, int);
long double (lgammal)(long double);
long long (llrintl)(long double);
long long (llroundl)(long double);
long double (logl)(long double);
long double (log10l)(long double);
long double (log1pl)(long double);
long double (log2l)(long double);
long double (logbl)(long double);
long (lrintl)(long double);
long (lroundl)(long double);
long double (modfl)(long double, long double *);
long double (nanl)(const char *);
long double (nearbyintl)(long double);
long double (nextafterl)(long double, long double);
long double (nexttowardl)(long double, long double);
long double (powl)(long double, long double);
long double (remainderl)(long double, long double);
long double (remquol)(long double, long double, int *);
long double (rintl)(long double);
long double (roundl)(long double);
long double (scalblnl)(long double, long);
long double (scalbnl)(long double, int);
long double (sinl)(long double);
long double (sinhl)(long double);
long double (sqrtl)(long double);
long double (tanl)(long double);
long double (tanhl)(long double);
long double (tgammal)(long double);
long double (truncl)(long double);

#ifdef __cplusplus
}
#endif
#endif
