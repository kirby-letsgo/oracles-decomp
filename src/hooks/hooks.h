#pragma once
#include "core/gb.h"
#include <stdbool.h>

typedef void (*HookFn)(GB *gb);

typedef struct {
  uint16_t bank, addr;
  const char *name;
  HookFn fn;
  uint64_t calls;
} Hook;

enum { HOOK_MODE_OFF = 0, HOOK_MODE_REPLACE = 1, HOOK_MODE_VERIFY = 2 };
extern int hook_mode;
extern uint64_t hook_verify_failures;
extern bool hook_verify_abort;

void hooks_init(void);
bool hook_dispatch(GB *gb);
void gb_burn(GB *gb, int mcycles);
void hooks_report(void);
