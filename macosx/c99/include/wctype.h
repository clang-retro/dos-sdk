/* <wctype.h> for Mac OS X 10.0 to 10.2, which have none: the character
 * classes of libc99.a (src/wchar.c), for ASCII and Latin-1. Where the
 * system has a <wctype.h>, it is used. */
#if __has_include_next(<wctype.h>)
#include_next <wctype.h>
#elif !defined(__DOS_SDK_C99_WCTYPE_H)
#define __DOS_SDK_C99_WCTYPE_H
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif
int iswalnum(wint_t);
int iswalpha(wint_t);
int iswblank(wint_t);
int iswcntrl(wint_t);
int iswdigit(wint_t);
int iswgraph(wint_t);
int iswlower(wint_t);
int iswprint(wint_t);
int iswpunct(wint_t);
int iswspace(wint_t);
int iswupper(wint_t);
int iswxdigit(wint_t);
wint_t towlower(wint_t);
wint_t towupper(wint_t);
#ifdef __cplusplus
}
#endif
#endif
