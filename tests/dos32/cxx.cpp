/* cxx.cpp - a test of C++ on 32-bit DOS (i386-pc-msdos): exceptions
 * through SjLj contexts, RTTI, the containers, strings and iostreams. It
 * writes "PASS dos32 cxx" when all checks pass. */
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

int destroyed = 0;

/* Counts its destruction, to check that a throw unwinds the stack. */
struct Guard {
  ~Guard() { destroyed++; }
};

/* A base class, for dynamic_cast. */
struct Shape {
  virtual ~Shape() = default;
  virtual int sides() const = 0;
};

/* A derived class, for dynamic_cast. */
struct Square : Shape {
  int sides() const override { return 4; }
};

/* Throws after it makes a Guard. */
void thrower(int n) {
  Guard g;
  if (n > 2) throw std::runtime_error("boom " + std::to_string(n));
  thrower(n + 1);
}

} // namespace

/* Runs the checks. Gives the number of failed checks. */
int main() {
  int bad = 0;
  try {
    thrower(0);
    bad++;
  } catch (const std::exception &e) {
    std::cout << "caught " << e.what() << ", " << destroyed << " destructors ran\n";
    if (destroyed != 4) bad++;
  }
  std::unique_ptr<Shape> s(new Square);
  if (!dynamic_cast<Square *>(s.get()) || s->sides() != 4) bad++;
  std::map<std::string, int> m;
  std::vector<int> v;
  for (int i = 0; i < 100000; i++) v.push_back(i * 3);
  m["three"] = v[1];
  std::ostringstream os;
  os << "map " << m["three"] << " vector " << v.size() << " " << v.back();
  std::cout << os.str() << "\n";
  if (os.str() != "map 3 vector 100000 299997") bad++;
  std::cout << (bad ? "FAIL\n" : "PASS dos32 cxx\n");
  return bad;
}
