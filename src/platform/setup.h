#pragma once
#include "core/gb.h"
#include <stddef.h>
#include <stdbool.h>

uint8_t *oracles_read_file(const char *path, size_t *size);
void oracles_apply_agb_boot_patch(uint8_t *boot, size_t size);
bool oracles_load_init_ram(GB *gb, const char *path);
bool oracles_save_boot_state(const GB *gb, const char *path);
int oracles_state_format(void);
bool oracles_load_boot_state(GB *gb, const char *path);
void oracles_copy_state(GB *gb, const GB *src);
