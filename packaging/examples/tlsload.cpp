// Loads tlsdll.dll with LoadLibrary and uses its thread_local variables
// from two threads. Prints "tls dll ok" when each thread has its own.
#include <cstdio>
#include <thread>
#include <windows.h>

// Loads the DLL, calls tls_bump in the main thread and in a second
// thread, and checks that each thread starts from the initial values.
int main() {
  HMODULE m = LoadLibraryA("tlsdll.dll");
  if (!m) {
    std::printf("LoadLibrary failed\n");
    return 1;
  }
  auto bump = reinterpret_cast<int (*)(int)>(GetProcAddress(m, "tls_bump"));
  if (!bump)
    return 1;
  // 10 + 1 + 8, then 12 + 9.
  int a = bump(1), b = bump(1), c = 0;
  std::thread t([&] { c = bump(5); });  // 15 + 8: a new copy
  t.join();
  int d = bump(0);  // 12 + 10: the main thread's copy
  FreeLibrary(m);
  bool good = a == 19 && b == 21 && c == 23 && d == 22;
  std::printf("tls dll %s %d %d %d %d\n", good ? "ok" : "BAD", a, b, c, d);
  return !good;
}
