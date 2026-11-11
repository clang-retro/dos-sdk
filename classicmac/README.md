# Classic Mac OS

The classic Mac OS tools that were here (the 68k and PowerPC linkers
`codelink.py`, `peflink.py` and `fatlink.py`, `start.s`, the import
index, `sim68k.py` and the MAME and QEMU harnesses) moved to
[clang-retro/mac-sdk](https://github.com/clang-retro/mac-sdk), next to
the Toolbox headers and the runtime library:

- `tools/link/` has the linkers, `tools/macapp.py` compiles and links
  an application in one step, and `tools/sim68k.py`, `tools/mame/` and
  `tools/qemu/` are the emulator harnesses.
- See its README's "Building applications" section, also at
  <https://clang-retro.github.io/mac-sdk/building/>.
