#include "game/game.h"

void itemCode0c_hook(GB *gb);

void itemCode1e_hook(GB *gb) {
  // In Ages both item labels share this entry point.
  itemCode0c_hook(gb);
}
