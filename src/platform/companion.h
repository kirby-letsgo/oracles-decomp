#pragma once
#include <stdbool.h>
struct GB;
// Consumes the UI's latest request and publishes a snapshot on the game thread.
bool companion_update(struct GB *gb, bool seasons, bool playing);
void companion_clear(void);
