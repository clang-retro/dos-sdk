/*
 * The unwinder for C++ exceptions on 16-bit DOS and Windows: setjmp/longjmp
 * style ("SjLj") function contexts, as LLVM's SjLjEHPrepare pass makes
 * them. A function with landing pads registers a context in its frame at
 * entry and unregisters it on return; the contexts form a list from the
 * innermost function out. To throw, libc++abi calls
 * _Unwind_SjLj_RaiseException, which asks each context's personality
 * routine (__gxx_personality_sj0) for a handler, then jumps to the
 * dispatch code of each function with cleanups or the handler, which picks
 * the landing pad by the call site the personality chose.
 *
 * The layout of a context is fixed by the compiler (X86ISelLowering.cpp):
 *
 *   0   near pointer to the previous context
 *   2   call site (32 bits): set before each call, then by the personality
 *   6   four 32-bit data words: the exception pointer and the selector
 *   22  the personality routine: a near or far code pointer
 *   +   near pointer to the LSDA (the exception table, in DGROUP)
 *   +   jump buffer: BP, the dispatch's offset, SP, the dispatch's segment
 *       (with far code), one spare word
 *
 * Contexts are on the stack, in DGROUP (SS = DS), so near pointers reach
 * them in every memory model. The landing pads restore their registers
 * themselves, so jumping to one only sets BP and SP.
 *
 * On 16-bit Windows, the unwinder takes each context pointer as an offset
 * in SS (SJLJ_STACK_CONTEXTS): a DLL runs on the stack of the task that
 * calls it, so SS is not DS, and the compiler keeps the context in the
 * stack frame in every memory model. Each task has a list of its own,
 * found by its SS, because one task may yield inside a DLL while another
 * task calls it. In a program, SS is DS and there is one task.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */
#include <stdint.h>
#include <stdlib.h>
#include <unwind.h>

#ifndef __USING_SJLJ_EXCEPTIONS__
#error "build with -fexceptions: the unwinder needs the SjLj exception model"
#endif

#if defined(__MEDIUM__) || defined(__LARGE__) || defined(__HUGE__)
#define FAR_CODE 1
#endif
/* A near pointer into DGROUP. The tiny model has only near pointers. */
#ifdef __TINY__
#define NEAR
#else
#define NEAR __near
#endif

#if defined(__WINDOWS__) && !defined(SJLJ_STACK_CONTEXTS)
#define SJLJ_STACK_CONTEXTS 1
#endif

/* The previous context is the 16-bit value that the compiler passes: a
 * near pointer, or with SJLJ_STACK_CONTEXTS an offset in SS. */
struct fc {
  uint16_t prev;
  uint32_t call_site;
  uint32_t data[4];
  _Unwind_Personality_Fn personality;
  const unsigned char NEAR *lsda;
  uint16_t jbuf[5];
};

#ifdef SJLJ_STACK_CONTEXTS
typedef struct fc __far *fc_t;

/* Returns the context at offset off in SS, or a null pointer for 0. */
static fc_t from_off(uint16_t off) {
  uint16_t ss;
  if (!off)
    return 0;
  __asm__("movw %%ss, %0" : "=r"(ss));
  return (fc_t)(((uint32_t)ss << 16) | off);
}

/* Returns the offset in SS of the context c. */
static uint16_t to_off(fc_t c) { return (uint16_t)(uint32_t)c; }

#define NTASKS 8

/* The innermost context of each task that is in the DLL. A slot whose top
 * is 0 is free. */
static struct {
  uint16_t ss;
  uint16_t top;
} tops[NTASKS];

/* Returns the slot of the current task (by its SS), or a free slot if the
 * task has none and alloc is set. Returns a null pointer if there is
 * none. */
static uint16_t *top_slot(int alloc) {
  uint16_t ss;
  __asm__("movw %%ss, %0" : "=r"(ss));
  for (int i = 0; i < NTASKS; i++)
    if (tops[i].top && tops[i].ss == ss)
      return &tops[i].top;
  if (!alloc)
    return 0;
  for (int i = 0; i < NTASKS; i++)
    if (!tops[i].top) {
      tops[i].ss = ss;
      return &tops[i].top;
    }
  abort();
}

/* Returns the innermost context of the current task. */
static fc_t get_top(void) {
  uint16_t *slot = top_slot(0);
  return slot ? from_off(*slot) : 0;
}

/* Makes c the innermost context of the current task. */
static void set_top(fc_t c) {
  uint16_t *slot = top_slot(c != 0);
  if (slot)
    *slot = to_off(c);
}
#else
typedef struct fc NEAR *fc_t;

/* Returns the context at the near pointer off. */
static fc_t from_off(uint16_t off) { return (fc_t)off; }

/* Returns the near pointer of the context c. */
static uint16_t to_off(fc_t c) { return (uint16_t)c; }

static fc_t top;

/* Returns the innermost context. */
static fc_t get_top(void) { return top; }

/* Makes c the innermost context. */
static void set_top(fc_t c) { top = c; }
#endif

/* Returns the context that the personality routine passes back. */
static fc_t to_fc(struct _Unwind_Context *context) {
  return from_off((uint16_t)(uintptr_t)context);
}

/* Returns the context to give to the personality routine: the 16-bit
 * value, as a default pointer. */
static struct _Unwind_Context *to_context(fc_t c) {
  return (struct _Unwind_Context *)(uintptr_t)to_off(c);
}

/* unwind.h declares these with a default pointer, which is far in the far
 * data models; the compiler passes a 16-bit one. */
void sjlj_register(uint16_t c) __asm__("__Unwind_SjLj_Register");
void sjlj_unregister(uint16_t c) __asm__("__Unwind_SjLj_Unregister");

/* Puts the context c at the head of the list, at the entry of a function
 * with landing pads. */
void sjlj_register(uint16_t c) {
  fc_t fc = from_off(c);
  fc->prev = to_off(get_top());
  set_top(fc);
}

/* Takes the context c off the list, when its function returns. */
void sjlj_unregister(uint16_t c) { set_top(from_off(from_off(c)->prev)); }

/* Jump to the dispatch code of the function that registered c. */
static __attribute__((noreturn)) void install(fc_t c) {
  /* BP, the dispatch's offset, SP and the dispatch's segment. */
  uint16_t bp = c->jbuf[0], ip = c->jbuf[1], sp = c->jbuf[2];
#ifdef FAR_CODE
  uint16_t cs = c->jbuf[3];
#endif
  set_top(c);
#ifdef FAR_CODE
  __asm__ volatile("movw %%si, %%bp\n\t"
                   "movw %%di, %%sp\n\t"
                   "pushw %%ax\n\t"
                   "pushw %%dx\n\t"
                   "lretw"
                   :
                   : "a"(cs), "d"(ip), "S"(bp), "D"(sp)
                   : "memory");
#else
  __asm__ volatile("movw %%si, %%bp\n\t"
                   "movw %%di, %%sp\n\t"
                   "jmpw *%%dx"
                   :
                   : "d"(ip), "S"(bp), "D"(sp)
                   : "memory");
#endif
  __builtin_unreachable();
}

static _Unwind_Reason_Code phase1(struct _Unwind_Exception *e) {
  for (fc_t c = get_top(); c; c = from_off(c->prev)) {
    if (!c->personality)
      continue;
    _Unwind_Reason_Code r = c->personality(1, _UA_SEARCH_PHASE,
                                           e->exception_class, e,
                                           to_context(c));
    if (r == _URC_HANDLER_FOUND) {
      e->private_2 = to_off(c);
      return _URC_NO_REASON;
    }
    if (r != _URC_CONTINUE_UNWIND)
      return _URC_FATAL_PHASE1_ERROR;
  }
  return _URC_END_OF_STACK;
}

static _Unwind_Reason_Code phase2(struct _Unwind_Exception *e) {
  for (fc_t c = get_top(); c; c = from_off(c->prev)) {
    if (!c->personality)
      continue;
    _Unwind_Action action = _UA_CLEANUP_PHASE;
    if (to_off(c) == e->private_2)
      action = (_Unwind_Action)(_UA_CLEANUP_PHASE | _UA_HANDLER_FRAME);
    _Unwind_Reason_Code r =
        c->personality(1, action, e->exception_class, e, to_context(c));
    if (r == _URC_INSTALL_CONTEXT)
      install(c);
    if (r != _URC_CONTINUE_UNWIND || (action & _UA_HANDLER_FRAME))
      return _URC_FATAL_PHASE2_ERROR;
  }
  return _URC_END_OF_STACK;
}

_Unwind_Reason_Code _Unwind_SjLj_RaiseException(struct _Unwind_Exception *e) {
  e->private_1 = 0;
  e->private_2 = 0;
  _Unwind_Reason_Code r = phase1(e);
  if (r != _URC_NO_REASON)
    return r;
  return phase2(e);
}

/* Called by a landing pad that only ran cleanups, to go on unwinding. */
void _Unwind_SjLj_Resume(struct _Unwind_Exception *e) {
  phase2(e);
  abort();
}

/* Called by __cxa_rethrow. */
_Unwind_Reason_Code
_Unwind_SjLj_Resume_or_Rethrow(struct _Unwind_Exception *e) {
  return _Unwind_SjLj_RaiseException(e);
}

void *_Unwind_GetLanguageSpecificData(struct _Unwind_Context *context) {
  /* A near pointer into DGROUP, made a default (far or huge) pointer. */
  return (void *)(const unsigned char *)to_fc(context)->lsda;
}

_Unwind_Word _Unwind_GetGR(struct _Unwind_Context *context, int index) {
  return to_fc(context)->data[index];
}

void _Unwind_SetGR(struct _Unwind_Context *context, int index,
                   _Unwind_Word value) {
  to_fc(context)->data[index] = value;
}

/* The "instruction pointer" is the call site number, plus one. */
_Unwind_Word _Unwind_GetIP(struct _Unwind_Context *context) {
  return to_fc(context)->call_site + 1;
}

_Unwind_Word _Unwind_GetIPInfo(struct _Unwind_Context *context, int *before) {
  *before = 0;
  return to_fc(context)->call_site + 1;
}

void _Unwind_SetIP(struct _Unwind_Context *context, _Unwind_Word value) {
  to_fc(context)->call_site = value - 1;
}

_Unwind_Ptr _Unwind_GetRegionStart(struct _Unwind_Context *context) {
  (void)context;
  return 0;
}

_Unwind_Word _Unwind_GetCFA(struct _Unwind_Context *context) {
  return context ? to_fc(context)->jbuf[2] : 0;
}

void _Unwind_DeleteException(struct _Unwind_Exception *e) {
  if (e->exception_cleanup)
    e->exception_cleanup(_URC_FOREIGN_EXCEPTION_CAUGHT, e);
}
