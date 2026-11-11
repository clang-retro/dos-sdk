#!/usr/bin/env python3
"""Pick the picolibc libm objects that libc99.a needs.

  select-objects.py NM SYSTEM-DYLIB OBJECT...

Prints two lines: the objects that define the C99 math functions the
system library lacks (with everything they need), and the long double
names those objects already define, for long-double.c to leave out.
An object that defines a wanted function and also a function the system
library has is refused, so a program doesn't get picolibc's sin where it
had Apple's. An object pulled in only for a picolibc internal (such as
__math_lgamma_r, which sits beside lgamma_r) may carry such a function;
those are listed on stderr.
"""
import subprocess
import sys

# The functions libc99.a provides from picolibc, when the system lacks them.
MATH = """acos acosh asin asinh atan atan2 atanh cbrt ceil copysign cos cosh erf
erfc exp exp2 expm1 fabs fdim floor fma fmax fmin fmod frexp hypot ilogb ldexp
lgamma llrint llround log log10 log1p log2 logb lrint lround modf nan nearbyint
nextafter nexttoward pow remainder remquo rint round scalbln scalbn sin sinh
sqrt tan tanh tgamma trunc""".split()

# And the <fenv.h> functions, taken as they are.
FENV = """feclearexcept fegetexceptflag feraiseexcept fesetexceptflag fetestexcept
fegetround fesetround fegetenv feholdexcept fesetenv feupdateenv""".split()


def symbols(nm, path):
    out = subprocess.run([nm, "-g", path], capture_output=True, text=True).stdout
    defs, undefs = set(), set()
    for line in out.splitlines():
        f = line.split()
        if len(f) == 3 and f[1] != "U":
            defs.add(f[2])
        elif len(f) == 2 and f[0] == "U":
            undefs.add(f[1])
    return defs, undefs


def main():
    nm, dylib, objs = sys.argv[1], sys.argv[2], sys.argv[3:]
    system = symbols(nm, dylib)[0]
    info = {o: symbols(nm, o) for o in objs}
    definer = {}
    for o in objs:
        for s in info[o][0]:
            definer.setdefault(s, o)
    wanted = ["_" + n + "f" for n in MATH] + ["_" + n for n in MATH] + ["_" + n for n in FENV]
    wanted = set(wanted)
    keep, work = set(), [s for s in wanted if s not in system]
    while work:
        s = work.pop()
        # Take Apple's public functions, but picolibc's own internals
        # (__kernel_sin and the like), even where libSystem exports one of
        # the same name: they need not match.
        if (s in system and not s.startswith("___")) or s not in definer:
            continue
        o = definer[s]
        if o in keep:
            continue
        clash = {c for c in info[o][0] & system if not c.startswith("___")}
        if clash and s in wanted:
            sys.exit("%s defines %s and also %s" % (o, s, " ".join(sorted(clash))))
        if clash:
            print("%s (for %s) also defines %s" % (o, s, " ".join(sorted(clash))),
                  file=sys.stderr)
        keep.add(o)
        work.extend(info[o][1])
    defined = set().union(*(info[o][0] for o in keep)) if keep else set()
    longs = sorted(n + "l" for n in MATH if "_" + n + "l" in defined | system)
    print(" ".join(sorted(keep)))
    print(" ".join(longs))


main()
