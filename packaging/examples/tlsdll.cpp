// A DLL with thread_local variables that tlsload.cpp loads with
// LoadLibrary. Built with win32-picolibc-emutls.cfg, it uses emulated TLS
// and has no TLS directory, so it works on Windows before Vista too.
#include <string>

static thread_local int counter = 10;
static thread_local std::string name = "initial";

// Adds by to the calling thread's counter and a character to its name.
// Returns the counter plus the length of the name.
extern "C" __declspec(dllexport) int tls_bump(int by) {
  counter += by;
  name += "+";
  return counter + static_cast<int>(name.size());
}
