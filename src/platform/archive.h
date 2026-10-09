#pragma once
#include <stddef.h>
#include <stdint.h>

// A ROM image out of bytes that are either the ROM itself or a .zip or .gz holding one. Returns a
// malloc'd buffer the caller frees, or NULL with a short reason in `err` (fits the status line).
// Takes bytes rather than a path because the apps must read the file SDL's way: on Android the
// picker hands back a content:// URI, which only SDL_IOFromFile knows how to open.
uint8_t *archive_rom_from_memory(const uint8_t *data, size_t size, size_t *out_size, char *err, size_t err_size);

// The same, reading the file with stdio: for the headless tools and the tests, which are given real
// paths.
uint8_t *archive_read_rom(const char *path, size_t *size, char *err, size_t err_size);
