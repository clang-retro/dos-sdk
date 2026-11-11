/* Mac OS X 10.0's <ctype.h> has isdigit and the like only as macros. C++
 * headers remove the macros and need the functions, which libSystem has, so
 * declare them (in parentheses, past the macros). */
#include_next <ctype.h>

#if !defined(__DOS_SDK_C99_CTYPE_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1030
#define __DOS_SDK_C99_CTYPE_H
#ifdef __cplusplus
extern "C" {
#endif
int(isalnum)(int);
int(isalpha)(int);
int(isblank)(int);
int(iscntrl)(int);
int(isdigit)(int);
int(isgraph)(int);
int(islower)(int);
int(isprint)(int);
int(ispunct)(int);
int(isspace)(int);
int(isupper)(int);
int(isxdigit)(int);
#ifdef __cplusplus
}
#endif
#endif
