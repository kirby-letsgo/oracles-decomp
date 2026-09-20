#include "game/game.h"

void itemCode0c_hook(GB *gb);

void itemCode1e_hook(GB *gb) {
  itemCode0c_hook(gb);
}
