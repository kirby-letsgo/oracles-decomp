# hqx

The hqx pixel-art scaler by Maxim Stepin, in Cameron Zemek's C implementation
(https://github.com/grom358/hqx, commit `a1c7d415`), licensed under the GNU Lesser General Public
License v2.1 (see `COPYING` and the file headers).

Local changes:

- `common.h`: `rgb_to_yuv` computes the value instead of reading it out of `RGBtoYUV`, a
  16777216-entry (64 MB) table that `hqxInit` filled at startup. It was reached through this one
  function, and a table that size has no place in a self-contained app that scales a 160x144
  picture. The intermediate is now `int`: upstream casts a negative `double` to `uint32_t`, which is
  undefined behaviour and does not agree between machines -- x86-64 wraps (and the `+ 128` brings
  the value back into range) while arm64 saturates to 0, which left 12450786 of the 16777216 colours
  with the wrong U and V. The computed version matches the x86-64 result for every colour.
- `common.h`: `yuv_diff` subtracts in `int32_t`. Upstream subtracts `uint32_t`, so a negative
  difference wraps and is only rescued by the implementation-defined conversion to `int` at the
  `abs()` call. Every real compiler does what was meant, but it warns under `-Wall`; each masked
  field is at most `0x00FF0000`, so the signed subtraction cannot overflow and the result is the
  same.
- `hqx.h`: `hqxInit` is gone with that table. `HQX_CALLCONV` and `HQX_API` are empty, because this
  copy is compiled into the app rather than built as a Windows DLL, and upstream's
  `__declspec(dllimport)` on a definition is an error in the MinGW cross-build.
- `init.c` and `hq4x.c` are not vendored: the first only built the table, and the app scales by 2
  or 3 (`src/ui/upscale.cpp`).
