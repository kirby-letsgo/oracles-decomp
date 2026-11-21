#pragma once
#include "ui/labels.h"
struct GB;
// Consumes the UI's latest request and publishes a snapshot on the game thread.
bool companion_update(struct GB *gb, bool seasons, bool playing, const uint8_t glyphs[HUD_SLOTS]);
void companion_clear(void);
void companion_set_enabled(bool enabled);
