#pragma once
#include <stddef.h>
#include <stdint.h>

// Reads a ROM image, either from a plain file or from a .zip or .gz holding one. Returns a malloc'd
// buffer the caller frees, or NULL with a short reason in `err` (suitable for the status line).
uint8_t *archive_read_rom(const char *path, size_t *size, char *err, size_t err_size);
