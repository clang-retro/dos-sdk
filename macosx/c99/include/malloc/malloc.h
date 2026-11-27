/* <malloc/malloc.h> for the Mac OS X 10.0 to 10.2 headers, which have none.
 * It declares malloc_size, which the system library has. Where the system
 * has the header, it is used. */
#if __has_include_next(<malloc/malloc.h>)
#include_next <malloc/malloc.h>
#elif !defined(__DOS_SDK_C99_MALLOC_H)
#define __DOS_SDK_C99_MALLOC_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
size_t malloc_size(const void *);
#ifdef __cplusplus
}
#endif
#endif
