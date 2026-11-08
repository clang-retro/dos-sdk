---
hide:
  - navigation
---

# clang-retro projects

clang-retro brings a current clang and LLVM to 16 and 32-bit systems that
upstream LLVM does not support, with a C library, C++ and the system's
own API for each. Every repository has its own documentation site.

## Getting started

Most people only need the toolchain package, which has the compiler, the
linkers and a sysroot for every target below: see
[the toolchain package](toolchain.md).

| Platform | Target triple | Documentation |
| --- | --- | --- |
| MS-DOS (8086 and up) | `i8086-pc-msdos` | [dos-sdk](https://clang-retro.github.io/dos-sdk/) |
| Windows 1.x, 2.x and 3.x | `i8086-pc-win16` | [win16-sdk](https://clang-retro.github.io/win16-sdk/) |
| Win32s, Windows NT 3.1 to 11, 95/98/ME | `i386-pc-windows*-gnu`, `x86_64-w64-mingw32` | [win32-sdk](https://clang-retro.github.io/win32-sdk/) |
| MFC programs on Win32 | (any Windows target) | [mfc](https://clang-retro.github.io/mfc/) |
| OS/2 1.x, 2.x to 4.x, ArcaOS | `i8086-pc-os2`, `i386-pc-os2` | [os2-sdk](https://clang-retro.github.io/os2-sdk/) |
| Classic Mac OS, System 0.1 to 9 | `m68k-apple-macos`, `powerpc-apple-macos9` | [mac-sdk](https://clang-retro.github.io/mac-sdk/) |
| AmigaOS, Kickstart 1.3 to 3.x | `m68k-amigaos` | [amiga-sdk](https://clang-retro.github.io/amiga-sdk/) |
| Atari ST, STe, TT and Falcon | `m68k-atari-tos` | [atari-sdk](https://clang-retro.github.io/atari-sdk/) |

## The toolchain

| Repository | What it is | Documentation |
| --- | --- | --- |
| [llvm](https://github.com/clang-retro/llvm) | LLVM and clang with the 8086 backend, the retro targets and their drivers | [llvm](https://clang-retro.github.io/llvm/) |
| [picolibc](https://github.com/clang-retro/picolibc) | The C library, with a layer for each system | [picolibc](https://clang-retro.github.io/picolibc/) |
| [JWlink](https://github.com/clang-retro/JWlink) | The linker for DOS, Win16 and OS/2 programs | [JWlink](https://clang-retro.github.io/JWlink/) |
| [dos-sdk](https://github.com/clang-retro/dos-sdk) | The DOS sysroot, the toolchain packages and the CI that ties it all together | [this site](index.md) |

## License

The SDKs are free software under the GNU Affero General Public License,
version 3 only. The forks of LLVM, picolibc and JWlink keep their
upstream licenses. Contributors keep the copyright in their own work and
sign off their commits under the Developer Certificate of Origin.
