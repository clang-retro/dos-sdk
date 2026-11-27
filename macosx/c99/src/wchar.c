/* The <wchar.h> and <wctype.h> functions that Mac OS X 10.0 to 10.2 do not
 * have. The build defines HAVE_name for each function that the system
 * library already has, and this file leaves those out.
 *
 * The multibyte encoding is UTF-8, the encoding of the file system. The
 * character classes are those of ASCII and Latin-1. The wide formatted
 * input and output functions convert the format and the text to narrow
 * strings and use the narrow functions of the system. */
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

/* The string and memory functions. */
#ifndef HAVE_wcslen
/* wcslen: gives the number of wide characters before the null. */
size_t wcslen(const wchar_t *s) {
  const wchar_t *p = s;
  while (*p) p++;
  return (size_t)(p - s);
}
#endif
#ifndef HAVE_wcscpy
/* wcscpy: copies a wide string with its null. */
wchar_t *wcscpy(wchar_t *d, const wchar_t *s) {
  wchar_t *r = d;
  while ((*d++ = *s++) != 0) {}
  return r;
}
#endif
#ifndef HAVE_wcsncpy
/* wcsncpy: copies at most n wide characters, and fills the rest with nulls. */
wchar_t *wcsncpy(wchar_t *d, const wchar_t *s, size_t n) {
  size_t i = 0;
  for (; i < n && s[i]; i++) d[i] = s[i];
  for (; i < n; i++) d[i] = 0;
  return d;
}
#endif
#ifndef HAVE_wcscat
/* wcscat: appends a wide string. */
wchar_t *wcscat(wchar_t *d, const wchar_t *s) {
  wcscpy(d + wcslen(d), s);
  return d;
}
#endif
#ifndef HAVE_wcsncat
/* wcsncat: appends at most n wide characters and a null. */
wchar_t *wcsncat(wchar_t *d, const wchar_t *s, size_t n) {
  wchar_t *p = d + wcslen(d);
  while (n-- && *s) *p++ = *s++;
  *p = 0;
  return d;
}
#endif
#ifndef HAVE_wcscmp
/* wcscmp: compares two wide strings by the values of their characters. */
int wcscmp(const wchar_t *a, const wchar_t *b) {
  while (*a && *a == *b) a++, b++;
  return *a < *b ? -1 : *a > *b;
}
#endif
#ifndef HAVE_wcsncmp
/* wcsncmp: compares at most n wide characters. */
int wcsncmp(const wchar_t *a, const wchar_t *b, size_t n) {
  for (; n; n--, a++, b++) {
    if (*a != *b) return *a < *b ? -1 : 1;
    if (!*a) return 0;
  }
  return 0;
}
#endif
#ifndef HAVE_wcscoll
/* wcscoll: compares two wide strings; the C locale orders by value. */
int wcscoll(const wchar_t *a, const wchar_t *b) { return wcscmp(a, b); }
#endif
#ifndef HAVE_wcsxfrm
/* wcsxfrm: copies the string, which in the C locale is its own key. */
size_t wcsxfrm(wchar_t *d, const wchar_t *s, size_t n) {
  size_t len = wcslen(s);
  if (len < n) wcscpy(d, s);
  return len;
}
#endif
#ifndef HAVE_wcschr
/* wcschr: finds the first c in a wide string (the null counts). */
wchar_t *wcschr(const wchar_t *s, wchar_t c) {
  for (;; s++) {
    if (*s == c) return (wchar_t *)s;
    if (!*s) return NULL;
  }
}
#endif
#ifndef HAVE_wcsrchr
/* wcsrchr: finds the last c in a wide string. */
wchar_t *wcsrchr(const wchar_t *s, wchar_t c) {
  const wchar_t *r = NULL;
  for (;; s++) {
    if (*s == c) r = s;
    if (!*s) return (wchar_t *)r;
  }
}
#endif
#ifndef HAVE_wcsspn
/* wcsspn: the length of the start of s made of characters in set. */
size_t wcsspn(const wchar_t *s, const wchar_t *set) {
  size_t n = 0;
  while (s[n] && wcschr(set, s[n])) n++;
  return n;
}
#endif
#ifndef HAVE_wcscspn
/* wcscspn: the length of the start of s made of characters not in set. */
size_t wcscspn(const wchar_t *s, const wchar_t *set) {
  size_t n = 0;
  while (s[n] && !wcschr(set, s[n])) n++;
  return n;
}
#endif
#ifndef HAVE_wcspbrk
/* wcspbrk: finds the first character of s that is in set. */
wchar_t *wcspbrk(const wchar_t *s, const wchar_t *set) {
  s += wcscspn(s, set);
  return *s ? (wchar_t *)s : NULL;
}
#endif
#ifndef HAVE_wcsstr
/* wcsstr: finds a wide string in another. */
wchar_t *wcsstr(const wchar_t *s, const wchar_t *find) {
  size_t n = wcslen(find);
  for (; *s; s++)
    if (!wcsncmp(s, find, n)) return (wchar_t *)s;
  return n ? NULL : (wchar_t *)s;
}
#endif
#ifndef HAVE_wcstok
/* wcstok: splits a wide string at the characters of sep. */
wchar_t *wcstok(wchar_t *s, const wchar_t *sep, wchar_t **last) {
  if (!s) s = *last;
  if (!s) return NULL;
  s += wcsspn(s, sep);
  if (!*s) {
    *last = NULL;
    return NULL;
  }
  wchar_t *end = s + wcscspn(s, sep);
  if (*end) *end++ = 0;
  else end = NULL;
  *last = end;
  return s;
}
#endif
#ifndef HAVE_wmemchr
/* wmemchr: finds c in the first n wide characters. */
wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n) {
  for (; n; n--, s++)
    if (*s == c) return (wchar_t *)s;
  return NULL;
}
#endif
#ifndef HAVE_wmemcmp
/* wmemcmp: compares n wide characters. */
int wmemcmp(const wchar_t *a, const wchar_t *b, size_t n) {
  for (; n; n--, a++, b++)
    if (*a != *b) return *a < *b ? -1 : 1;
  return 0;
}
#endif
#ifndef HAVE_wmemcpy
/* wmemcpy: copies n wide characters. */
wchar_t *wmemcpy(wchar_t *d, const wchar_t *s, size_t n) {
  return (wchar_t *)memcpy(d, s, n * sizeof(wchar_t));
}
#endif
#ifndef HAVE_wmemmove
/* wmemmove: copies n wide characters that may overlap. */
wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n) {
  return (wchar_t *)memmove(d, s, n * sizeof(wchar_t));
}
#endif
#ifndef HAVE_wmemset
/* wmemset: sets n wide characters to c. */
wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n) {
  for (size_t i = 0; i < n; i++) d[i] = c;
  return d;
}
#endif

/* Conversions between UTF-8 and wide characters. */
#ifndef HAVE_mbrtowc
/* mbrtowc: reads one UTF-8 character. The state is not used: an
 * incomplete sequence gives (size_t)-2 and is read again whole. */
size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) {
  (void)ps;
  if (!s) return 0;
  if (!n) return (size_t)-2;
  unsigned char c = (unsigned char)*s;
  int len = c < 0x80 ? 1 : (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : (c & 0xF8) == 0xF0 ? 4 : 0;
  if (!len) {
    errno = EILSEQ;
    return (size_t)-1;
  }
  if (n < (size_t)len) return (size_t)-2;
  wchar_t w = len == 1 ? c : len == 2 ? (c & 0x1F) : len == 3 ? (c & 0x0F) : (c & 0x07);
  for (int i = 1; i < len; i++) {
    unsigned char k = (unsigned char)s[i];
    if ((k & 0xC0) != 0x80) {
      errno = EILSEQ;
      return (size_t)-1;
    }
    w = (w << 6) | (k & 0x3F);
  }
  if (pwc) *pwc = w;
  return w ? (size_t)len : 0;
}
#endif
#ifndef HAVE_mbrlen
/* mbrlen: the length of the next UTF-8 character. */
size_t mbrlen(const char *s, size_t n, mbstate_t *ps) { return mbrtowc(NULL, s, n, ps); }
#endif
#ifndef HAVE_mbsinit
/* mbsinit: the state is always the initial one. */
int mbsinit(const mbstate_t *ps) {
  (void)ps;
  return 1;
}
#endif
#ifndef HAVE_wcrtomb
/* wcrtomb: writes one wide character as UTF-8. */
size_t wcrtomb(char *s, wchar_t w, mbstate_t *ps) {
  (void)ps;
  char buf[4];
  if (!s) s = buf, w = 0;
  unsigned long u = (unsigned long)w;
  if (u < 0x80) {
    s[0] = (char)u;
    return 1;
  }
  if (u < 0x800) {
    s[0] = (char)(0xC0 | (u >> 6));
    s[1] = (char)(0x80 | (u & 0x3F));
    return 2;
  }
  if (u < 0x10000) {
    s[0] = (char)(0xE0 | (u >> 12));
    s[1] = (char)(0x80 | ((u >> 6) & 0x3F));
    s[2] = (char)(0x80 | (u & 0x3F));
    return 3;
  }
  if (u < 0x110000) {
    s[0] = (char)(0xF0 | (u >> 18));
    s[1] = (char)(0x80 | ((u >> 12) & 0x3F));
    s[2] = (char)(0x80 | ((u >> 6) & 0x3F));
    s[3] = (char)(0x80 | (u & 0x3F));
    return 4;
  }
  errno = EILSEQ;
  return (size_t)-1;
}
#endif
#ifndef HAVE_btowc
/* btowc: the wide character of a single byte, or WEOF. */
wint_t btowc(int c) { return c == EOF || (c & 0x80) ? WEOF : (wint_t)c; }
#endif
#ifndef HAVE_wctob
/* wctob: the single byte of a wide character, or EOF. */
int wctob(wint_t c) { return c < 0x80 ? (int)c : EOF; }
#endif
#ifndef HAVE_mbsrtowcs
/* mbsrtowcs: converts a UTF-8 string to at most n wide characters. */
size_t mbsrtowcs(wchar_t *d, const char **src, size_t n, mbstate_t *ps) {
  const char *s = *src;
  size_t count = 0;
  while (!d || count < n) {
    wchar_t w;
    size_t k = mbrtowc(&w, s, MB_LEN_MAX, ps);
    if (k == (size_t)-1 || k == (size_t)-2) {
      errno = EILSEQ;
      return (size_t)-1;
    }
    if (d) d[count] = w;
    if (!k) {
      if (d) *src = NULL;
      return count;
    }
    s += k;
    count++;
  }
  *src = s;
  return count;
}
#endif
#ifndef HAVE_wcsrtombs
/* wcsrtombs: converts a wide string to at most n bytes of UTF-8. */
size_t wcsrtombs(char *d, const wchar_t **src, size_t n, mbstate_t *ps) {
  const wchar_t *s = *src;
  size_t count = 0;
  for (;; s++) {
    char buf[4];
    size_t k = wcrtomb(buf, *s, ps);
    if (k == (size_t)-1) return k;
    if (d && count + k > n) break;
    if (d) memcpy(d + count, buf, k);
    if (!*s) {
      if (d) *src = NULL;
      return count;
    }
    count += k;
  }
  *src = s;
  return count;
}
#endif

/* The character classes: ASCII, then Latin-1 for the letters. */
#ifndef HAVE_iswalpha
/* iswalpha: letters of ASCII and Latin-1 (and all characters above Latin-1). */
int iswalpha(wint_t c) {
  if (c < 0x80) return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  if (c < 0xC0) return c == 0xAA || c == 0xB5 || c == 0xBA;
  if (c < 0x100) return c != 0xD7 && c != 0xF7;
  return c != WEOF;
}
#endif
#ifndef HAVE_iswdigit
/* iswdigit: the decimal digits. */
int iswdigit(wint_t c) { return c >= '0' && c <= '9'; }
#endif
#ifndef HAVE_iswalnum
/* iswalnum: letters and digits. */
int iswalnum(wint_t c) { return iswalpha(c) || iswdigit(c); }
#endif
#ifndef HAVE_iswxdigit
/* iswxdigit: the hexadecimal digits. */
int iswxdigit(wint_t c) { return iswdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
#endif
#ifndef HAVE_iswspace
/* iswspace: the white space characters. */
int iswspace(wint_t c) { return c == ' ' || (c >= 9 && c <= 13) || c == 0x85 || c == 0xA0 || c == 0x3000; }
#endif
#ifndef HAVE_iswblank
/* iswblank: space and tab. */
int iswblank(wint_t c) { return c == ' ' || c == '\t'; }
#endif
#ifndef HAVE_iswcntrl
/* iswcntrl: the control characters. */
int iswcntrl(wint_t c) { return c < 0x20 || (c >= 0x7F && c < 0xA0); }
#endif
#ifndef HAVE_iswupper
/* iswupper: the capital letters. */
int iswupper(wint_t c) { return (c >= 'A' && c <= 'Z') || (c >= 0xC0 && c <= 0xDE && c != 0xD7); }
#endif
#ifndef HAVE_iswlower
/* iswlower: the small letters. */
int iswlower(wint_t c) { return (c >= 'a' && c <= 'z') || (c >= 0xDF && c <= 0xFF && c != 0xF7); }
#endif
#ifndef HAVE_iswprint
/* iswprint: the characters that print, with the space. */
int iswprint(wint_t c) { return c != WEOF && !iswcntrl(c); }
#endif
#ifndef HAVE_iswgraph
/* iswgraph: the characters that print, without the space. */
int iswgraph(wint_t c) { return iswprint(c) && !iswspace(c); }
#endif
#ifndef HAVE_iswpunct
/* iswpunct: the characters that print and are not letters or digits. */
int iswpunct(wint_t c) { return iswgraph(c) && !iswalnum(c); }
#endif
#ifndef HAVE_towupper
/* towupper: the capital of a small letter of ASCII or Latin-1. */
wint_t towupper(wint_t c) {
  if ((c >= 'a' && c <= 'z') || (c >= 0xE0 && c <= 0xFE && c != 0xF7)) return c - 0x20;
  return c;
}
#endif
#ifndef HAVE_towlower
/* towlower: the small letter of a capital of ASCII or Latin-1. */
wint_t towlower(wint_t c) {
  if ((c >= 'A' && c <= 'Z') || (c >= 0xC0 && c <= 0xDE && c != 0xD7)) return c + 0x20;
  return c;
}
#endif

/* Numbers from wide strings: the digits are ASCII, so the narrow
 * functions read a narrow copy. */

/* Narrow: copies the ASCII start of a wide string into buf (n bytes). */
static const char *Narrow(const wchar_t *s, char *buf, size_t n) {
  size_t i = 0;
  while (i + 1 < n && s[i] > 0 && s[i] < 0x80) {
    buf[i] = (char)s[i];
    i++;
  }
  buf[i] = 0;
  return buf;
}
#define WIDE_TO_NUMBER(type, name, narrow_call)                         \
  type name {                                                           \
    char buf[128];                                                      \
    char *e;                                                            \
    const char *b = Narrow(s, buf, sizeof buf);                         \
    type r = narrow_call;                                               \
    if (end) *end = (wchar_t *)(s + (e - b));                           \
    return r;                                                           \
  }
#ifndef HAVE_wcstol
/* wcstol: a long from a wide string. */
WIDE_TO_NUMBER(long, wcstol(const wchar_t *s, wchar_t **end, int base), strtol(b, &e, base))
#endif
#ifndef HAVE_wcstoul
/* wcstoul: an unsigned long from a wide string. */
WIDE_TO_NUMBER(unsigned long, wcstoul(const wchar_t *s, wchar_t **end, int base), strtoul(b, &e, base))
#endif
#ifndef HAVE_wcstoll
/* wcstoll: a long long from a wide string. */
WIDE_TO_NUMBER(long long, wcstoll(const wchar_t *s, wchar_t **end, int base), strtoq(b, &e, base))
#endif
#ifndef HAVE_wcstoull
/* wcstoull: an unsigned long long from a wide string. */
WIDE_TO_NUMBER(unsigned long long, wcstoull(const wchar_t *s, wchar_t **end, int base), strtouq(b, &e, base))
#endif
#ifndef HAVE_wcstod
/* wcstod: a double from a wide string. */
WIDE_TO_NUMBER(double, wcstod(const wchar_t *s, wchar_t **end), strtod(b, &e))
#endif
#ifndef HAVE_wcstof
/* wcstof: a float from a wide string. */
WIDE_TO_NUMBER(float, wcstof(const wchar_t *s, wchar_t **end), (float)strtod(b, &e))
#endif
#ifndef HAVE_wcstold
/* wcstold: a long double (a double before 10.4) from a wide string. */
WIDE_TO_NUMBER(long double, wcstold(const wchar_t *s, wchar_t **end), strtod(b, &e))
#endif

/* Formatted input and output of wide strings. */

/* NarrowString: makes a UTF-8 copy of a wide string, which the caller
 * frees. */
static char *NarrowString(const wchar_t *s) {
  size_t n = 0;
  for (const wchar_t *p = s; *p; p++) n += 4;
  char *r = (char *)malloc(n + 1), *q = r;
  if (!r) return NULL;
  for (; *s; s++) {
    size_t k = wcrtomb(q, *s, NULL);
    if (k != (size_t)-1) q += k;
  }
  *q = 0;
  return r;
}

#ifndef HAVE_vswprintf
/* vswprintf: formats into a wide buffer of n characters. The format is
 * converted to UTF-8 (so %s takes a narrow string, as with sprintf), and
 * the result back to wide characters. */
int vswprintf(wchar_t *d, size_t n, const wchar_t *fmt, va_list ap) {
  char *f = NarrowString(fmt);
  if (!f) return -1;
  va_list copy;
  va_copy(copy, ap);
  int len = vsnprintf(NULL, 0, f, copy);
  va_end(copy);
  if (len < 0) {
    free(f);
    return -1;
  }
  char *out = (char *)malloc((size_t)len + 1);
  if (!out) {
    free(f);
    return -1;
  }
  vsnprintf(out, (size_t)len + 1, f, ap);
  free(f);
  const char *p = out;
  size_t count = 0;
  while (*p && count + 1 < n) {
    wchar_t w;
    size_t k = mbrtowc(&w, p, 4, NULL);
    if (k == (size_t)-1 || k == (size_t)-2 || k == 0) break;
    d[count++] = w;
    p += k;
  }
  int fits = *p == 0;
  if (n) d[count] = 0;
  free(out);
  return fits ? (int)count : -1;
}
#endif
#ifndef HAVE_swprintf
/* swprintf: formats into a wide buffer of n characters. */
int swprintf(wchar_t *d, size_t n, const wchar_t *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int r = vswprintf(d, n, fmt, ap);
  va_end(ap);
  return r;
}
#endif
#ifndef HAVE_vswscanf
/* vswscanf: reads from a wide string, through narrow copies of the string
 * and the format. %s and %c store narrow characters. */
int vswscanf(const wchar_t *s, const wchar_t *fmt, va_list ap) {
  char *in = NarrowString(s), *f = NarrowString(fmt);
  int r = in && f ? vsscanf(in, f, ap) : EOF;
  free(in);
  free(f);
  return r;
}
#endif
#ifndef HAVE_swscanf
/* swscanf: reads from a wide string. */
int swscanf(const wchar_t *s, const wchar_t *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int r = vswscanf(s, fmt, ap);
  va_end(ap);
  return r;
}
#endif
