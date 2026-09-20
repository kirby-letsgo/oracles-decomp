#pragma once
#include "core/gb.h"
#include <stdbool.h>

typedef void (*HookFn)(GB *gb);

typedef struct {
  uint16_t bank, addr;
  const char *name;
  HookFn fn;
  uint64_t calls;
  int flags;
} Hook;

enum { HOOK_NOVERIFY = 1, HOOK_LOCAL = 2 };

enum { HOOK_MODE_OFF = 0, HOOK_MODE_REPLACE = 1, HOOK_MODE_VERIFY = 2 };
extern int hook_mode;
extern int hook_in_verify;
extern int hook_suppress_interrupts;
extern uint64_t hook_verify_failures;
extern bool hook_verify_abort;

void hooks_init(void);
bool hook_dispatch(GB *gb);
bool hook_enabled_at(const GB *gb, uint16_t addr);
void gb_burn(GB *gb, int mcycles);
void gb_burn_nb(GB *gb, int mcycles);
void hooks_report(void);
void hook_handoff(GB *gb, uint16_t pc);
int hook_halt(GB *gb, uint16_t next);
void hook_continue(GB *gb, uint16_t pc, uint16_t sp0);
