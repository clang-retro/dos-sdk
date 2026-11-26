// threads.cpp - a test of C++ threads on 32-bit DOS (i386-pc-msdos): the
// cooperative threads of the C library under std::thread, std::mutex,
// std::condition_variable, thread_local, an exception in a thread, and
// the steady clock. It writes "PASS dos32 threads" when all checks pass.
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

static std::mutex m;
static std::condition_variable cv;
static bool go = false;
static int total = 0;
static thread_local int mine = 7;

// Waits for the start signal, then adds to the total under the mutex.
// Gives 1 when its thread-local copy kept its value.
static int work(int id) {
  {
    std::unique_lock<std::mutex> l(m);
    cv.wait(l, [] { return go; });
  }
  mine = id;
  for (int i = 0; i < 500; i++) {
    std::lock_guard<std::mutex> g(m);
    total++;
    if (i % 50 == 0) std::this_thread::yield();
  }
  try {
    throw std::runtime_error("in thread");
  } catch (const std::exception&) {
  }
  return mine == id;
}

// Starts four threads, checks their results and the clocks.
int main() {
  int bad = 0;
  std::vector<int> ok(4);
  std::vector<std::thread> t;
  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < 4; i++) t.emplace_back([&ok, i] { ok[i] = work(i + 1); });
  {
    std::lock_guard<std::mutex> g(m);
    go = true;
  }
  cv.notify_all();
  for (auto& th : t) th.join();
  for (int i = 0; i < 4; i++)
    if (!ok[i]) { std::printf("thread %d lost its thread_local\n", i); bad++; }
  if (total != 2000) { std::printf("total %d\n", total); bad++; }
  if (mine != 7) { std::printf("main thread_local %d\n", mine); bad++; }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
  if (ms < 50 || ms > 5000) { std::printf("steady clock %ld ms\n", (long)ms); bad++; }
  std::printf(bad ? "FAIL dos32 threads\n" : "PASS dos32 threads\n");
  return bad;
}
