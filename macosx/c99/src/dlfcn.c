/* dlopen and the others for Mac OS X 10.0 to 10.2, which have none (see
 * include/dlfcn.h). They use the functions of dyld that 10.0 has: bundles
 * load with NSLinkModule, libraries with NSAddLibrary, and symbols are
 * looked up in the module or in the whole program. */
#include <dlfcn.h>

#ifndef HAVE_dlopen
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A handle: a bundle linked with NSLinkModule, or a loaded image. */
struct handle {
  NSModule module;              /* a bundle's module, or NULL */
  const struct mach_header *mh; /* the image, or NULL for the program */
};

static struct handle g_main;    /* dlopen(NULL) */
static char g_error[512];
static int g_haveError;

/* SetError: keeps the message that dlerror gives next. */
static void SetError(const char *what, const char *name) {
  snprintf(g_error, sizeof g_error, "%s: %s", what, name ? name : "(null)");
  g_haveError = 1;
}

/* LoadedImage: the header of the loaded image whose path is path. */
static const struct mach_header *LoadedImage(const char *path) {
  char want[1024], have[1024];
  if (!realpath(path, want)) strncpy(want, path, sizeof want - 1), want[sizeof want - 1] = 0;
  unsigned long n = _dyld_image_count();
  for (unsigned long i = 0; i < n; i++) {
    const char *name = _dyld_get_image_name(i);
    if (!name) continue;
    if (!strcmp(name, path) || !strcmp(name, want) || (realpath(name, have) && !strcmp(have, want)))
      return _dyld_get_image_header(i);
  }
  return NULL;
}

/* dlopen: loads a bundle or a library, or gives the program (NULL). */
void *dlopen(const char *path, int mode) {
  if (!path) return &g_main;
  const struct mach_header *mh = LoadedImage(path);
  if (!mh && (mode & RTLD_NOLOAD)) return NULL;
  struct handle *h = (struct handle *)calloc(1, sizeof *h);
  if (!h) return NULL;
  if (mh) {
    h->mh = mh;
    return h;
  }
  NSObjectFileImage image;
  NSObjectFileImageReturnCode rc = NSCreateObjectFileImageFromFile(path, &image);
  if (rc == NSObjectFileImageSuccess) {
    h->module = NSLinkModule(image, path, NSLINKMODULE_OPTION_RETURN_ON_ERROR);
    NSDestroyObjectFileImage(image);
    if (!h->module) {
      SetError("cannot link", path);
      free(h);
      return NULL;
    }
    return h;
  }
  if (rc == NSObjectFileImageInappropriateFile && NSAddLibrary(path)) {
    h->mh = LoadedImage(path);
    if (h->mh) return h;
  }
  SetError("cannot load", path);
  free(h);
  return NULL;
}

/* dlsym: the address of a symbol (its C name, without the underscore). */
void *dlsym(void *handle, const char *name) {
  char sym[512];
  snprintf(sym, sizeof sym, "_%s", name);
  struct handle *h = (struct handle *)handle;
  NSSymbol s = NULL;
  if (h && h != (struct handle *)RTLD_DEFAULT && h != (struct handle *)RTLD_NEXT && h->module)
    s = NSLookupSymbolInModule(h->module, sym);
  else if (NSIsSymbolNameDefined(sym))
    s = NSLookupAndBindSymbol(sym);
  if (!s) {
    SetError("symbol not found", name);
    return NULL;
  }
  return NSAddressOfSymbol(s);
}

/* dlclose: unlinks a bundle; a library stays loaded. */
int dlclose(void *handle) {
  struct handle *h = (struct handle *)handle;
  if (!h || h == &g_main) return 0;
  if (h->module) NSUnLinkModule(h->module, NSUNLINKMODULE_OPTION_NONE);
  free(h);
  return 0;
}

/* dlerror: the last error, once, or NULL. */
const char *dlerror(void) {
  if (!g_haveError) return NULL;
  g_haveError = 0;
  return g_error;
}

/* dladdr: the image that holds an address. The symbol is not looked up. */
int dladdr(const void *addr, Dl_info *info) {
  unsigned long n = _dyld_image_count();
  for (unsigned long i = 0; i < n; i++) {
    const struct mach_header *mh = _dyld_get_image_header(i);
    unsigned long slide = (unsigned long)_dyld_get_image_vmaddr_slide(i);
    const struct load_command *lc = (const struct load_command *)(mh + 1);
    for (unsigned long k = 0; mh && k < mh->ncmds; k++) {
      if (lc->cmd == LC_SEGMENT) {
        const struct segment_command *seg = (const struct segment_command *)lc;
        unsigned long start = seg->vmaddr + slide;
        if ((unsigned long)addr >= start && (unsigned long)addr < start + seg->vmsize && seg->vmsize) {
          info->dli_fname = _dyld_get_image_name(i);
          info->dli_fbase = (void *)mh;
          info->dli_sname = NULL;
          info->dli_saddr = NULL;
          return 1;
        }
      }
      lc = (const struct load_command *)((const char *)lc + lc->cmdsize);
    }
  }
  return 0;
}

#endif
