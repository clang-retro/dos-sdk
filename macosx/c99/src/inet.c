/* inet_pton and inet_ntop for Mac OS X before 10.2, for IPv4 (AF_INET) and
 * IPv6 (AF_INET6) addresses, in network byte order. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#ifndef HAVE_inet_pton
/* Pton4 reads a dotted IPv4 address of four decimal parts into out[4].
 * Returns 1, or 0 when src is not such an address. */
static int Pton4(const char *src, unsigned char *out) {
  int part = 0, digits = 0;
  unsigned value = 0;
  for (;; src++) {
    if (*src >= '0' && *src <= '9') {
      if (digits && value == 0) return 0; /* no leading zeros */
      value = value * 10 + (unsigned)(*src - '0');
      if (value > 255 || ++digits > 3) return 0;
    } else if ((*src == '.' || *src == '\0') && digits) {
      out[part++] = (unsigned char)value;
      if (*src == '\0') return part == 4;
      if (part == 4) return 0;
      value = 0;
      digits = 0;
    } else {
      return 0;
    }
  }
}

/* HexValue gives the value of a hexadecimal digit, or -1. */
static int HexValue(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

/* Pton6 reads an IPv6 address (with "::" and an IPv4 tail) into out[16].
 * Returns 1, or 0 when src is not such an address. */
static int Pton6(const char *src, unsigned char *out) {
  unsigned char buf[16];
  int n = 0, gap = -1;
  memset(buf, 0, sizeof buf);
  if (src[0] == ':') {
    if (src[1] != ':') return 0;
    src++;
  }
  while (*src) {
    if (*src == ':') {
      if (gap >= 0) return 0;
      gap = n;
      src++;
      continue;
    }
    const char *start = src;
    unsigned value = 0;
    int digits = 0, d;
    while ((d = HexValue(*src)) >= 0) {
      value = value * 16 + (unsigned)d;
      if (++digits > 4) return 0;
      src++;
    }
    if (*src == '.') {
      if (n > 12 || !Pton4(start, buf + n)) return 0;
      n += 4;
      break;
    }
    if (!digits || n > 14) return 0;
    buf[n++] = (unsigned char)(value >> 8);
    buf[n++] = (unsigned char)value;
    if (*src == ':') {
      src++;
      if (*src == '\0') return 0;
    } else if (*src != '\0') {
      return 0;
    }
  }
  if (gap >= 0) {
    if (n == 16) return 0;
    memmove(buf + 16 - (n - gap), buf + gap, (size_t)(n - gap));
    memset(buf + gap, 0, (size_t)(16 - n));
  } else if (n != 16) {
    return 0;
  }
  memcpy(out, buf, 16);
  return 1;
}

/* inet_pton: the binary address of a text address. Returns 1, 0 when src
 * is not an address of the family, or -1 for a family it does not know. */
int inet_pton(int af, const char *src, void *dst) {
  if (af == AF_INET) return Pton4(src, (unsigned char *)dst);
  if (af == AF_INET6) return Pton6(src, (unsigned char *)dst);
  errno = EAFNOSUPPORT;
  return -1;
}
#endif

#ifndef HAVE_inet_ntop
/* inet_ntop: the text of a binary address, in dst of size bytes. Returns
 * dst, or NULL (errno ENOSPC or EAFNOSUPPORT). IPv6 addresses have the
 * longest run of zero groups as "::". */
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
  const unsigned char *a = (const unsigned char *)src;
  char text[64];
  if (af == AF_INET) {
    snprintf(text, sizeof text, "%u.%u.%u.%u", a[0], a[1], a[2], a[3]);
  } else if (af == AF_INET6) {
    unsigned group[8];
    int i, best = -1, bestLen = 0, run = -1;
    for (i = 0; i < 8; i++) group[i] = (unsigned)a[2 * i] << 8 | a[2 * i + 1];
    for (i = 0; i <= 8; i++) {
      if (i < 8 && group[i] == 0) {
        if (run < 0) run = i;
      } else if (run >= 0) {
        if (i - run > bestLen && i - run >= 2) best = run, bestLen = i - run;
        run = -1;
      }
    }
    char *p = text;
    for (i = 0; i < 8; i++) {
      if (i == best) {
        *p++ = ':';
        if (i == 0) *p++ = ':';
        i += bestLen - 1;
        continue;
      }
      p += sprintf(p, "%x", group[i]);
      if (i < 7) *p++ = ':';
    }
    *p = '\0';
  } else {
    errno = EAFNOSUPPORT;
    return NULL;
  }
  if (strlen(text) >= (size_t)size) {
    errno = ENOSPC;
    return NULL;
  }
  strcpy(dst, text);
  return dst;
}
#endif
