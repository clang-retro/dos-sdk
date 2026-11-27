/* <wchar.h> for Mac OS X 10.0 to 10.2, which have none. It gives the types,
 * laid out as in the 10.4 SDK, and the functions of libc99.a
 * (src/wchar.c). Where the system has a <wchar.h>, it is used. */
#if __has_include_next(<wchar.h>)
#include_next <wchar.h>
#elif !defined(__DOS_SDK_C99_WCHAR_H)
#define __DOS_SDK_C99_WCHAR_H
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h> /* struct tm, which C99 says <wchar.h> declares */

typedef union {
  char __mbstate8[128];
  long long _mbstateL; /* for alignment */
} __mbstate_t;
typedef __mbstate_t mbstate_t;
#ifndef _WINT_T
#define _WINT_T
typedef int wint_t;
#endif

#define WEOF ((wint_t)-1)
#ifndef WCHAR_MIN
#define WCHAR_MIN __WCHAR_MIN__
#define WCHAR_MAX __WCHAR_MAX__
#endif

#ifdef __cplusplus
extern "C" {
#endif
size_t wcslen(const wchar_t *);
wchar_t *wcscpy(wchar_t *, const wchar_t *);
wchar_t *wcsncpy(wchar_t *, const wchar_t *, size_t);
wchar_t *wcscat(wchar_t *, const wchar_t *);
wchar_t *wcsncat(wchar_t *, const wchar_t *, size_t);
int wcscmp(const wchar_t *, const wchar_t *);
int wcsncmp(const wchar_t *, const wchar_t *, size_t);
int wcscoll(const wchar_t *, const wchar_t *);
size_t wcsxfrm(wchar_t *, const wchar_t *, size_t);
wchar_t *wcschr(const wchar_t *, wchar_t);
wchar_t *wcsrchr(const wchar_t *, wchar_t);
size_t wcsspn(const wchar_t *, const wchar_t *);
size_t wcscspn(const wchar_t *, const wchar_t *);
wchar_t *wcspbrk(const wchar_t *, const wchar_t *);
wchar_t *wcsstr(const wchar_t *, const wchar_t *);
wchar_t *wcstok(wchar_t *, const wchar_t *, wchar_t **);
wchar_t *wmemchr(const wchar_t *, wchar_t, size_t);
int wmemcmp(const wchar_t *, const wchar_t *, size_t);
wchar_t *wmemcpy(wchar_t *, const wchar_t *, size_t);
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t);
wchar_t *wmemset(wchar_t *, wchar_t, size_t);
size_t mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
size_t mbrlen(const char *, size_t, mbstate_t *);
int mbsinit(const mbstate_t *);
size_t wcrtomb(char *, wchar_t, mbstate_t *);
wint_t btowc(int);
int wctob(wint_t);
size_t mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *);
size_t wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *);
long wcstol(const wchar_t *, wchar_t **, int);
unsigned long wcstoul(const wchar_t *, wchar_t **, int);
long long wcstoll(const wchar_t *, wchar_t **, int);
unsigned long long wcstoull(const wchar_t *, wchar_t **, int);
double wcstod(const wchar_t *, wchar_t **);
float wcstof(const wchar_t *, wchar_t **);
long double wcstold(const wchar_t *, wchar_t **);
int swprintf(wchar_t *, size_t, const wchar_t *, ...);
int vswprintf(wchar_t *, size_t, const wchar_t *, va_list);
int swscanf(const wchar_t *, const wchar_t *, ...);
int vswscanf(const wchar_t *, const wchar_t *, va_list);
#ifdef __cplusplus
}
#endif
#endif
