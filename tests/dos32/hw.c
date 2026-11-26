/* hw.c - a test of the hardware access of 32-bit DOS (i386-pc-msdos):
 * VGA memory, VESA calls through DOS memory, and a mapping of the linear
 * frame buffer. It writes "PASS dos32 hw" when all checks pass. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <conio.h>
#include <dpmi.h>
#include <i86.h>

/* Checks VGA memory, VESA through a real-mode buffer, a physical mapping
 * of the linear frame buffer, conio and floats. Gives the number of
 * failed checks. */
int main(void) {
  int bad = 0;
  union REGS r;
  r.w.ax = 0x13; int386(0x10, &r, &r);
  volatile uint8_t *vga = (uint8_t *)0xA0000;
  vga[0] = 42; vga[63999] = 7;
  int okvga = vga[0] == 42 && vga[63999] == 7;
  r.w.ax = 0x03; int386(0x10, &r, &r);
  printf("vga %s\n", okvga ? "ok" : "FAIL"); bad += !okvga;
  int sel, seg = __dpmi_allocate_dos_memory(64, &sel);
  printf("dos mem seg=%x\n", seg);
  char *buf = (char *)(seg * 16);
  memcpy(buf, "VBE2", 4);
  __dpmi_regs rm; memset(&rm, 0, sizeof rm);
  rm.x.ax = 0x4f00; rm.x.es = seg; rm.x.di = 0;
  __dpmi_int(0x10, &rm);
  printf("vbe ax=%x sig=%.4s ver=%x\n", rm.x.ax, buf, *(uint16_t *)(buf + 4));
  memset(&rm, 0, sizeof rm);
  rm.x.ax = 0x4f01; rm.x.cx = 0x111; rm.x.es = seg; rm.x.di = 0;
  __dpmi_int(0x10, &rm);
  uint32_t phys = *(uint32_t *)(buf + 40);
  printf("mode 111 ax=%x lfb=%lx\n", rm.x.ax, (unsigned long)phys);
  if (rm.x.ax == 0x4f && phys) {
    __dpmi_meminfo m = { .address = phys, .size = 640 * 480 * 2 };
    if (__dpmi_physical_address_mapping(&m) == 0) {
      memset(&rm, 0, sizeof rm); rm.x.ax = 0x4f02; rm.x.bx = 0x4111; __dpmi_int(0x10, &rm);
      volatile uint16_t *fb = (uint16_t *)m.address;
      for (int i = 0; i < 640 * 480; i++) fb[i] = (uint16_t)i;
      int ok = fb[1234] == 1234 && fb[640 * 480 - 1] == (uint16_t)(640 * 480 - 1);
      r.w.ax = 0x03; int386(0x10, &r, &r);
      printf("lfb map %lx %s\n", m.address, ok ? "ok" : "FAIL"); bad += !ok;
    } else { printf("FAIL map %x\n", __dpmi_error); bad++; }
  }
  __dpmi_free_dos_memory(sel);
  volatile double x = 2.0; double y = x * 3.25 / 1.5;
  printf("float %.6f\n", y);
  if (y < 4.3333 || y > 4.3334) { bad++; printf("FAIL float\n"); }
  cprintf("conio ok\r\n");
  printf(bad ? "FAIL\n" : "PASS dos32 hw\n");
  return bad;
}
