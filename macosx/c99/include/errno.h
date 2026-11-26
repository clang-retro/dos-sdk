/* <errno.h> with the error numbers that Mac OS X 10.0 to 10.3 do not name,
 * with the values of 10.4 and later. The system never gives them before
 * 10.4; the C++ library and the functions of libc99.a use the names. */
#include_next <errno.h>

#ifndef __DOS_SDK_C99_ERRNO_H
#define __DOS_SDK_C99_ERRNO_H
#ifndef ECANCELED
#define ECANCELED 89
#endif
#ifndef EIDRM
#define EIDRM 90
#endif
#ifndef ENOMSG
#define ENOMSG 91
#endif
#ifndef EILSEQ
#define EILSEQ 92
#endif
#ifndef EBADMSG
#define EBADMSG 94
#endif
#ifndef EMULTIHOP
#define EMULTIHOP 95
#endif
#ifndef ENODATA
#define ENODATA 96
#endif
#ifndef ENOLINK
#define ENOLINK 97
#endif
#ifndef ENOSR
#define ENOSR 98
#endif
#ifndef ENOSTR
#define ENOSTR 99
#endif
#ifndef EPROTO
#define EPROTO 100
#endif
#ifndef ETIME
#define ETIME 101
#endif
#ifndef ENOTRECOVERABLE
#define ENOTRECOVERABLE 104
#endif
#ifndef EOWNERDEAD
#define EOWNERDEAD 105
#endif
#endif
