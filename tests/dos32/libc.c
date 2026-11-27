/* libc.c - a test of the C library of 32-bit DOS (i386-pc-msdos).
 * Run it with an argument: dosbox-run.sh libc.exe hello */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <dpmi.h>
#include <dos.h>
#include <i86.h>

/* Checks the C library of 32-bit DOS: the command line, the
 * environment, a large malloc, files, seeks, stat, directories, the clock
 * and int386. Writes "PASS dos32 libc" when all checks pass, and gives
 * the number of failed checks. */
int main(int argc, char **argv) {
  int bad = 0;
  printf("argc=%d argv0=%s argv1=%s\n", argc, argv[0], argc > 1 ? argv[1] : "-");
  printf("PATH=%s\n", getenv("PATH") ? getenv("PATH") : "(none)");
  char *big = malloc(8u << 20);
  if (!big) { printf("FAIL malloc 8M\n"); bad++; } else { memset(big, 0x5a, 8u << 20); printf("malloc 8M ok %p\n", big); }
  FILE *f = fopen("T1.DAT", "wb");
  if (!f) { printf("FAIL fopen\n"); bad++; }
  else {
    for (int i = 0; i < 100000; i++) fwrite(&i, sizeof i, 1, f);
    fclose(f);
  }
  int fd = open("T1.DAT", O_RDONLY | O_BINARY);
  off_t end = lseek(fd, 0, SEEK_END);
  lseek(fd, 4 * 77777, SEEK_SET);
  int v = 0; read(fd, &v, sizeof v); close(fd);
  printf("size=%ld v=%d\n", (long)end, v);
  if (end != 400000 || v != 77777) { printf("FAIL lseek/read\n"); bad++; }
  struct stat st;
  if (stat("T1.DAT", &st) != 0 || st.st_size != 400000) { printf("FAIL stat\n"); bad++; }
  char cwd[128]; if (!getcwd(cwd, sizeof cwd)) { printf("FAIL getcwd\n"); bad++; } else printf("cwd=%s\n", cwd);
  char full[260]; if (!realpath("T1.DAT", full)) { printf("FAIL realpath\n"); bad++; } else printf("realpath=%s\n", full);
  DIR *d = opendir("."); int n = 0; struct dirent *e;
  while (d && (e = readdir(d))) n++;
  if (d) closedir(d);
  printf("dir entries=%d\n", n); if (n < 2) { printf("FAIL readdir\n"); bad++; }
  unlink("T1.DAT");
  time_t t = time(0); printf("time=%ld\n", (long)t);
  printf("dpmi version=%x psp=%x\n", __dpmi_get_version(), _psp);
  union REGS r; r.w.ax = 0x3000; int386(0x21, &r, &r); printf("dos version %d.%d\n", r.h.al, r.h.ah);
  unsigned short ticks = peek(0x40, 0x6c); printf("ticks=%u\n", ticks);
  printf(bad ? "FAIL\n" : "PASS dos32 libc\n");
  return bad;
}
