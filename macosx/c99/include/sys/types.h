/* <sys/types.h> with the types that the Mac OS X headers before 10.3 do not
 * define, laid out as in the 10.4 SDK. */
#include_next <sys/types.h>

#if !defined(__DOS_SDK_C99_SYS_TYPES_H) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1030
#define __DOS_SDK_C99_SYS_TYPES_H
typedef u_int32_t useconds_t;
typedef int32_t suseconds_t;
#endif
