/* <dlfcn.h> for Mac OS X 10.0 to 10.2, which have none: dlopen and the
 * others of libc99.a (src/dlfcn.c), over the NS... functions of dyld.
 * Where the system has a <dlfcn.h>, it is used. */
#if __has_include_next(<dlfcn.h>)
#include_next <dlfcn.h>
#elif !defined(__DOS_SDK_C99_DLFCN_H)
#define __DOS_SDK_C99_DLFCN_H

typedef struct dl_info {
  const char *dli_fname; /* the file of the image */
  void *dli_fbase;       /* where the image starts */
  const char *dli_sname; /* the nearest symbol, or NULL */
  void *dli_saddr;       /* its address */
} Dl_info;

#define RTLD_LAZY 0x1
#define RTLD_NOW 0x2
#define RTLD_LOCAL 0x4
#define RTLD_GLOBAL 0x8
#define RTLD_NOLOAD 0x10
#define RTLD_NODELETE 0x80
#define RTLD_NEXT ((void *)-1)
#define RTLD_DEFAULT ((void *)-2)

#ifdef __cplusplus
extern "C" {
#endif
void *dlopen(const char *, int);
void *dlsym(void *, const char *);
int dlclose(void *);
const char *dlerror(void);
int dladdr(const void *, Dl_info *);
#ifdef __cplusplus
}
#endif
#endif
