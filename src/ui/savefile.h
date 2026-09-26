#pragma once
// Reads the game's own save files (the SRAM image) for the launcher: name, hearts, essences.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UI_FILES 3

typedef struct {
  bool valid;
  char name[6];
  uint8_t health, max_health;   // in quarter hearts
  uint8_t essences;             // bit per essence
  int essence_count;
  bool linked, hero, completed;
} UiFileInfo;

// Fills out[0..2]; a file counts when its checksum and verification string match (copy 1, else the
// backup copy), as the game's verifyFileCopies does. Returns the number of valid files.
int ui_read_files(const uint8_t *sram, size_t size, bool ages, UiFileInfo out[UI_FILES]);
// The checksum the game stores in a file's first two bytes.
uint16_t ui_file_checksum(const uint8_t *file);
