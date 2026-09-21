#pragma once
#include "core/gb.h"

// One native stack per game thread. The kernel (the ROM's main loop) runs on the primary stack
// inside the ordinary dispatch chain; a thread yields by switching back to it and continues in
// the same C frame when the kernel resumes it. Under HOOK_MODE_VERIFY the primitives fall back
// to the trampoline (hook_handoff), because verify mode re-runs hooks in the interpreter.
enum { FIBER_YIELD = 0, FIBER_EXIT = 1, FIBER_RESET = 2 };

// Kernel side: run thread n until it switches back. `start` non-NULL (re)creates the fiber and
// runs start(gb) on it; NULL resumes it. Returns the thread's request. In verify mode this
// hands off to `fallback_pc` instead and never returns.
int fiber_run(GB *gb, int n, void (*start)(GB *), uint16_t fallback_pc);
// Thread side: switch to the kernel with a request. FIBER_YIELD returns when resumed; the others
// never return. In verify mode this is hook_handoff(gb, gb->pc).
void fiber_back(GB *gb, int request);
bool fiber_in_thread(const GB *gb);
// Drop every thread fiber (reset paths); the kernel is current afterwards.
void fibers_reset(GB *gb);
