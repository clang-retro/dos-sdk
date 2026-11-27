/* <alloca.h> for the Mac OS X 10.0 and 10.1 headers, which have none. The
 * compiler gives alloca. Where the system has the header, it is used. */
#if __has_include_next(<alloca.h>)
#include_next <alloca.h>
#elif !defined(__DOS_SDK_C99_ALLOCA_H)
#define __DOS_SDK_C99_ALLOCA_H
#include <stddef.h>
#undef alloca
#define alloca(size) __builtin_alloca(size)
#endif
