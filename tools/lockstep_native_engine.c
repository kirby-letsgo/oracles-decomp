// The native engine compiled into the lockstep tool next to the interpreter: its gb_step gets a
// second name so both can be linked, and the tool installs it through GB.step.
#define gb_step native_gb_step
#include "../src/rt/native.c"
