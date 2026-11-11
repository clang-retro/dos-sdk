/* C99 <stdlib.h> and <inttypes.h> functions for Mac OS X 10.0 to 10.2,
 * built on the BSD functions those releases have. */
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

long long atoll(const char *s) { return strtoq(s, NULL, 10); }
long long llabs(long long n) { return n < 0 ? -n : n; }
lldiv_t lldiv(long long n, long long d) {
  lldiv_t r;
  r.quot = n / d;
  r.rem = n % d;
  return r;
}
long long strtoll(const char *s, char **end, int base) {
  return strtoq(s, end, base);
}
unsigned long long strtoull(const char *s, char **end, int base) {
  return strtouq(s, end, base);
}
float strtof(const char *s, char **end) { return (float)strtod(s, end); }
long double strtold(const char *s, char **end) { return strtod(s, end); }
void _Exit(int status) { _exit(status); }

intmax_t imaxabs(intmax_t n) { return n < 0 ? -n : n; }
imaxdiv_t imaxdiv(intmax_t n, intmax_t d) {
  imaxdiv_t r;
  r.quot = n / d;
  r.rem = n % d;
  return r;
}
intmax_t strtoimax(const char *s, char **end, int base) {
  return strtoq(s, end, base);
}
uintmax_t strtoumax(const char *s, char **end, int base) {
  return strtouq(s, end, base);
}
