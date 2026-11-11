/* long double versions of the <math.h> functions. Before 10.4 long double
 * is double, so each one calls the double function. The build leaves out,
 * with -DNO_name, those the picolibc objects in libc99.a already define. */
#include <math.h>

#if __LDBL_MANT_DIG__ != __DBL_MANT_DIG__
#error long double must be double here
#endif

#ifndef NO_acosl
long double acosl(long double a) { return acos(a); }
#endif
#ifndef NO_acoshl
long double acoshl(long double a) { return acosh(a); }
#endif
#ifndef NO_asinl
long double asinl(long double a) { return asin(a); }
#endif
#ifndef NO_asinhl
long double asinhl(long double a) { return asinh(a); }
#endif
#ifndef NO_atanl
long double atanl(long double a) { return atan(a); }
#endif
#ifndef NO_atan2l
long double atan2l(long double a, long double b) { return atan2(a, b); }
#endif
#ifndef NO_atanhl
long double atanhl(long double a) { return atanh(a); }
#endif
#ifndef NO_cbrtl
long double cbrtl(long double a) { return cbrt(a); }
#endif
#ifndef NO_ceill
long double ceill(long double a) { return ceil(a); }
#endif
#ifndef NO_copysignl
long double copysignl(long double a, long double b) { return copysign(a, b); }
#endif
#ifndef NO_cosl
long double cosl(long double a) { return cos(a); }
#endif
#ifndef NO_coshl
long double coshl(long double a) { return cosh(a); }
#endif
#ifndef NO_erfl
long double erfl(long double a) { return erf(a); }
#endif
#ifndef NO_erfcl
long double erfcl(long double a) { return erfc(a); }
#endif
#ifndef NO_expl
long double expl(long double a) { return exp(a); }
#endif
#ifndef NO_exp2l
long double exp2l(long double a) { return exp2(a); }
#endif
#ifndef NO_expm1l
long double expm1l(long double a) { return expm1(a); }
#endif
#ifndef NO_fabsl
long double fabsl(long double a) { return fabs(a); }
#endif
#ifndef NO_fdiml
long double fdiml(long double a, long double b) { return fdim(a, b); }
#endif
#ifndef NO_floorl
long double floorl(long double a) { return floor(a); }
#endif
#ifndef NO_fmal
long double fmal(long double a, long double b, long double c) { return fma(a, b, c); }
#endif
#ifndef NO_fmaxl
long double fmaxl(long double a, long double b) { return fmax(a, b); }
#endif
#ifndef NO_fminl
long double fminl(long double a, long double b) { return fmin(a, b); }
#endif
#ifndef NO_fmodl
long double fmodl(long double a, long double b) { return fmod(a, b); }
#endif
#ifndef NO_frexpl
long double frexpl(long double a, int * b) { return frexp(a, b); }
#endif
#ifndef NO_hypotl
long double hypotl(long double a, long double b) { return hypot(a, b); }
#endif
#ifndef NO_ilogbl
int ilogbl(long double a) { return ilogb(a); }
#endif
#ifndef NO_ldexpl
long double ldexpl(long double a, int b) { return ldexp(a, b); }
#endif
#ifndef NO_lgammal
long double lgammal(long double a) { return lgamma(a); }
#endif
#ifndef NO_llrintl
long long llrintl(long double a) { return llrint(a); }
#endif
#ifndef NO_llroundl
long long llroundl(long double a) { return llround(a); }
#endif
#ifndef NO_logl
long double logl(long double a) { return log(a); }
#endif
#ifndef NO_log10l
long double log10l(long double a) { return log10(a); }
#endif
#ifndef NO_log1pl
long double log1pl(long double a) { return log1p(a); }
#endif
#ifndef NO_log2l
long double log2l(long double a) { return log2(a); }
#endif
#ifndef NO_logbl
long double logbl(long double a) { return logb(a); }
#endif
#ifndef NO_lrintl
long lrintl(long double a) { return lrint(a); }
#endif
#ifndef NO_lroundl
long lroundl(long double a) { return lround(a); }
#endif
#ifndef NO_modfl
long double modfl(long double a, long double * b) { return modf(a, (double *)b); }
#endif
#ifndef NO_nanl
long double nanl(const char * a) { return nan(a); }
#endif
#ifndef NO_nearbyintl
long double nearbyintl(long double a) { return nearbyint(a); }
#endif
#ifndef NO_nextafterl
long double nextafterl(long double a, long double b) { return nextafter(a, b); }
#endif
#ifndef NO_nexttowardl
long double nexttowardl(long double a, long double b) { return nexttoward(a, b); }
#endif
#ifndef NO_powl
long double powl(long double a, long double b) { return pow(a, b); }
#endif
#ifndef NO_remainderl
long double remainderl(long double a, long double b) { return remainder(a, b); }
#endif
#ifndef NO_remquol
long double remquol(long double a, long double b, int * c) { return remquo(a, b, c); }
#endif
#ifndef NO_rintl
long double rintl(long double a) { return rint(a); }
#endif
#ifndef NO_roundl
long double roundl(long double a) { return round(a); }
#endif
#ifndef NO_scalblnl
long double scalblnl(long double a, long b) { return scalbln(a, b); }
#endif
#ifndef NO_scalbnl
long double scalbnl(long double a, int b) { return scalbn(a, b); }
#endif
#ifndef NO_sinl
long double sinl(long double a) { return sin(a); }
#endif
#ifndef NO_sinhl
long double sinhl(long double a) { return sinh(a); }
#endif
#ifndef NO_sqrtl
long double sqrtl(long double a) { return sqrt(a); }
#endif
#ifndef NO_tanl
long double tanl(long double a) { return tan(a); }
#endif
#ifndef NO_tanhl
long double tanhl(long double a) { return tanh(a); }
#endif
#ifndef NO_tgammal
long double tgammal(long double a) { return tgamma(a); }
#endif
#ifndef NO_truncl
long double truncl(long double a) { return trunc(a); }
#endif
