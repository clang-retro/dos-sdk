/* Mac OS X 10.0 to 10.2 have no <wchar.h>. C++ headers need its types even
 * without wide character functions, so this one gives the types, laid out
 * as in the 10.4 SDK. Where the system has a <wchar.h>, it is used. */
#if __has_include_next(<wchar.h>)
#include_next <wchar.h>
#elif !defined(__DOS_SDK_C99_WCHAR_H)
#define __DOS_SDK_C99_WCHAR_H
#include <stddef.h>

typedef union {
  char __mbstate8[128];
  long long _mbstateL; /* for alignment */
} __mbstate_t;
typedef __mbstate_t mbstate_t;
typedef int wint_t;

#define WEOF ((wint_t)-1)
#ifndef WCHAR_MIN
#define WCHAR_MIN __WCHAR_MIN__
#define WCHAR_MAX __WCHAR_MAX__
#endif
#endif
