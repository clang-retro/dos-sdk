// Link check for weak inline variables of libc++ on 32-bit Mac OS X.
//
// At -O0 this file defines std::__itoa::__pow10_32 (an inline variable of
// <charconv>) as a global weak symbol. libc++.a (string.cpp.o, built with
// -fvisibility=hidden) defines a hidden copy of the same table, which
// std::to_string uses. ld64 merges the two copies into one global weak
// symbol. If the compiler accessed the hidden copy with a pointer
// difference from the PIC base, ld64 stops with "bad codegen, pointer diff
// in std::to_string to global weak symbol __pow10_32".
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

/// Converts the number in the first argument (or 12345) with std::to_chars
/// and std::to_string, prints both and returns 0 when they are equal.
int main(int argc, char **argv) {
  unsigned value = (unsigned)std::atoi(argc > 1 ? argv[1] : "12345");
  char buf[16];
  std::to_chars_result r = std::to_chars(buf, buf + sizeof buf - 1, value);
  *r.ptr = 0;
  std::string s = std::to_string(value);
  std::printf("%s %s\n", buf, s.c_str());
  return std::strcmp(buf, s.c_str()) == 0 ? 0 : 1;
}
