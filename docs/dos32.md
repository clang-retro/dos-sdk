# 32-bit DOS programs

The `i386-pc-msdos` target makes 32-bit protected-mode programs for
MS-DOS. A program is a flat 32-bit LE executable. The DOS/32A extender
is the stub of each program, so the program needs no other file. Type
the name of the program at the DOS prompt to start it.

```sh
clang --config=dos32.cfg -O2 hello.c -o hello.exe
clang++ --config=dos32.cfg -O2 app.cpp -o app.exe
```

## What a program gets

- A flat memory model. CS, DS, ES and SS have a base of 0 and a limit of
  4 GiB. A pointer is 32 bits. The first megabyte of memory is at its
  physical address, so the VGA memory is at `(char *)0xA0000`.
- picolibc, with the DOS layer of the 16-bit target. File, directory,
  time and console functions use INT 21h through DOS/32A.
- The heap is one block of DPMI memory. At startup, the C library takes
  the largest free block, less 1/8 of the free memory (at most 256 KiB).
  The block is at most 256 MiB.
- `argv` from the command line, and `environ` from the DOS environment.
- C++ with libc++ and libc++abi: exceptions (SjLj), RTTI, the containers,
  streams, `<filesystem>` and threads.
- Threads (`<pthread.h>`, `std::thread`, `std::mutex` and the others).
  See [Threads](#threads).

## The headers

| Header | Contents |
| --- | --- |
| `<dpmi.h>` | DPMI calls with the DJGPP names: `__dpmi_int`, `__dpmi_allocate_dos_memory`, `__dpmi_physical_address_mapping` and others. |
| `<i86.h>` | `union REGS`, `struct SREGS`, `int386`, `int386x`, `intdos`, `intdosx`, `segread`. `int86` is the same as `int386`. |
| `<dos.h>` | Port I/O (`inportb`, `outportb`, `inportl`, `outportl` and others), `peek`, `poke`, `delay`, `sound`, `_psp`. |
| `<conio.h>` | The console functions of the 16-bit target. |

`int386` does a protected-mode interrupt. DOS/32A gives the interrupt to
the real-mode handler. It translates the pointers of most INT 21h
functions. For a BIOS call that uses a buffer (for example the VESA
calls of INT 10h), do these steps:

1. Get DOS memory with `__dpmi_allocate_dos_memory`. The function gives
   a real-mode segment. The flat address of the buffer is
   `segment * 16`.
2. Fill a `__dpmi_regs`. Put the segment in `es` (or `ds`).
3. Call `__dpmi_int`.

To get to a linear frame buffer, give its physical address to
`__dpmi_physical_address_mapping`. Use the linear address that it gives
as a pointer. `tests/dos32/hw.c` shows these steps.

## Threads

DOS has no threads. The C library switches the threads itself. The
threads are cooperative: one thread runs at a time. A thread runs until
it calls a function that waits. Then the next thread runs.

These functions let other threads run:

| Function | When it lets other threads run |
| --- | --- |
| `pthread_join`, `std::thread::join` | Until the thread ends |
| `pthread_mutex_lock`, `std::mutex::lock` | While another thread has the mutex |
| `pthread_cond_wait`, `std::condition_variable::wait` | Until a signal |
| `sched_yield`, `std::this_thread::yield` | One time |
| `nanosleep`, `usleep`, `sleep`, `std::this_thread::sleep_for` | Until the time passes |

A thread that does not call these functions stops all other threads. In
a long loop, call `sched_yield`. A new thread starts when the thread that
made it waits.

- Each thread has a stack of 64 KiB from the heap. Use
  `pthread_attr_setstacksize` for a different size.
- Thread-local variables (`thread_local`, `_Thread_local`) use emulated
  TLS. Each access is a function call.
- When no other thread can run, a wait gives the time to the DPMI host
  (INT 2Fh, AX=1680h). This makes DOSBox, Windows and OS/2 faster.
- Do not call a function that waits from an interrupt handler.

## Limits

- **An FPU is necessary for `float` and `double`.** The code uses x87
  instructions. On a 386 or 486SX without an FPU, compile with
  `-msoft-float`.
- **The CPU is a 386 by default.** Use `--target=i686-pc-msdos` (or
  `-march=`) for code that uses later instructions.
- **The threads are cooperative.** See [Threads](#threads). C11
  `<threads.h>` is not available. Use `<pthread.h>` or `std::thread`.
- **INT 21h function 60h** (`_fullpath`) goes through a buffer in DOS
  memory, because DOS/32A does not translate it.
- **No hardware tests.** DOSBox is the only test. A test on a real
  machine is necessary.

## A program without the stub

When `lib/dos32a.exe` is not in the sysroot, the linker writes a plain LE
file. Then start the program with an external extender:
`dos32a prog.exe`.

## Tests

`tests/run-dos32-tests.sh` builds `tests/dos32/libc.c`, `hw.c` and
`cxx.cpp` and runs them in DOSBox with `packaging/dosbox-run.sh`. Each
test writes `PASS dos32 NAME`. The smoke test of the toolchain package
runs them when `DOSBOX` names DOSBox.

## License of DOS/32A

DOS/32A 7.35 has a BSD-style license. The sysroot has its text in
`share/licenses/DOS32A-license.txt`. When you distribute a program,
include this acknowledgment with it:

> This product uses DOS/32 Advanced DOS Extender technology.
