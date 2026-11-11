/* C99 library functions: float and long double math, the classification
 * macros, <fenv.h> rounding, and the long long and intmax_t functions.
 * Mac OS X 10.0 to 10.2 get them from libc99.a (scripts/build-c99-macosx.sh,
 * link with -lc99); 10.3 and later have them in libSystem. */
#include <fenv.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fails;
#define CHECK(c)                                                               \
  do {                                                                         \
    if (!(c)) {                                                                \
      printf("FAIL line %d: %s\n", __LINE__, #c);                              \
      fails++;                                                                 \
    }                                                                          \
  } while (0)

/* volatile keeps the compiler from working the answers out itself. */
static volatile float f2 = 2.0f, fh = 2.5f, fneg = -2.5f;
static volatile double d2 = 2.0, dh = 2.5;
static volatile long double l2 = 2.0L, lh = 2.5L;

static int near(double a, double b) { return fabs(a - b) <= 1e-6 * fabs(b); }

int main(void) {
  char *end;
  lldiv_t q;
  imaxdiv_t iq;
  volatile double x;

  /* float */
  CHECK(sqrtf(f2) * sqrtf(f2) > 1.9999f && sqrtf(f2) * sqrtf(f2) < 2.0001f);
  CHECK(near(sinf(f2), 0.9092974));
  CHECK(near(expf(f2), 7.3890561));
  CHECK(near(logf(f2), 0.6931472));
  CHECK(near(powf(f2, 10.0f), 1024.0));
  CHECK(near(atan2f(f2, f2), 0.7853982));
  CHECK(roundf(fh) == 3.0f && roundf(fneg) == -3.0f && truncf(fneg) == -2.0f);
  CHECK(floorf(fh) == 2.0f && ceilf(fh) == 3.0f && fabsf(fneg) == 2.5f);
  CHECK(lroundf(fh) == 3 && llrintf(fh) == 2); /* rint rounds to even */
  CHECK(fmaxf(f2, fh) == 2.5f && fminf(f2, fh) == 2.0f);
  CHECK(fmaf(f2, fh, 1.0f) == 6.0f);
  CHECK(near(cbrtf(8.0f * f2 / 2.0f), 2.0));
  CHECK(near(tgammaf(5.0f * f2 / 2.0f), 24.0));
  CHECK(ilogbf(8.0f * f2) == 4 && scalbnf(f2, 3) == 16.0f);

  /* double functions new in C99 */
  CHECK(round(dh) == 3.0 && trunc(-dh) == -2.0 && lround(dh) == 3);
  CHECK(near(exp2(d2 + 1), 8.0) && near(log2(d2 * 4), 3.0));
  CHECK(fma(d2, dh, 1.0) == 6.0 && fdim(dh, d2) == 0.5);
  CHECK(near(tgamma(d2 + 3), 24.0));
  CHECK(isnan(nan("")) && nextafter(1.0, 2.0) > 1.0);

  /* long double */
  CHECK(sizeof(long double) == sizeof(double) || __LDBL_MANT_DIG__ == 106);
  CHECK(near((double)sqrtl(l2 * l2), 2.0) && near((double)sinl(l2), 0.9092974));
  CHECK(roundl(lh) == 3.0L && truncl(-lh) == -2.0L && llroundl(lh) == 3);

  /* classification */
  x = 0.0;
  CHECK(isinf(1.0 / x) && !isfinite(1.0 / x) && signbit(-1.0 / x));
  CHECK(isnan(x / x) && fpclassify(x) == FP_ZERO && isnormal(d2));
  CHECK(isunordered(x / x, 1.0) && isgreater(d2, x) && isless(x, d2));
  CHECK(isinf(INFINITY) && isnan(NAN));

  /* rounding modes */
  CHECK(fegetround() == FE_TONEAREST);
  CHECK(fesetround(FE_UPWARD) == 0 && fegetround() == FE_UPWARD);
  CHECK(rintf(fh) == 3.0f && rint(-dh) == -2.0);
  CHECK(fesetround(FE_DOWNWARD) == 0 && rint(dh) == 2.0 && rintf(fneg) == -3.0f);
  CHECK(fesetround(FE_TONEAREST) == 0 && rint(dh) == 2.0);
  feclearexcept(FE_ALL_EXCEPT);
  x = d2 / x;
  CHECK(fetestexcept(FE_DIVBYZERO) && !fetestexcept(FE_OVERFLOW));

  /* long long and intmax_t */
  CHECK(strtoll("-123456789012", &end, 10) == -123456789012LL && *end == 0);
  CHECK(strtoull("ffffffffffff", NULL, 16) == 0xffffffffffffULL);
  CHECK(atoll("9000000000") == 9000000000LL && llabs(-5000000000LL) == 5000000000LL);
  q = lldiv(-7000000001LL, 1000000000LL);
  CHECK(q.quot == -7 && q.rem == -1);
  iq = imaxdiv(7, 2);
  CHECK(iq.quot == 3 && iq.rem == 1 && imaxabs(-3) == 3);
  CHECK(strtoimax("-42", NULL, 0) == -42 && strtoumax("0x10", NULL, 0) == 16);
  CHECK(strtof("0.5", NULL) == 0.5f && strtold("2.5", NULL) == 2.5L);

  printf(fails ? "%d FAILED\n" : "ALL PASSED\n", fails);
  return fails != 0;
}
