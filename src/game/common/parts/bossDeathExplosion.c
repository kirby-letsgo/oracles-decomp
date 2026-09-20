#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x44cc, 0x44ce); E = 0xc4; // Part.state
  CYC(0x44ce, 0x44cf); A = mem_rd(gb, DE);
  CYC(0x44cf, 0x44d0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x44d0, 0x44d2); goto state0; } // jr z
  CYC(0x44d0, 0x44d2);

  CYC(0x44d2, 0x44d4); E = 0xe1; // Part.animParameter
  CYC(0x44d4, 0x44d5); A = mem_rd(gb, DE);
  CYC(0x44d5, 0x44d6); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x44d6, 0x44d9); partAnimate_hook(gb); return; } // jp nz
  CYC(0x44d6, 0x44d9);

  CALL_C(0x44d9, decNumEnemies_hook, 0x24b3, 0x44dc);
  if (!(F & FZ)) { CYCT(0x44dc, 0x44de); goto delete; } // jr nz
  CYC(0x44dc, 0x44de);
  CYC(0x44de, 0x44e0); E = 0xc2; // Part.subid
  CYC(0x44e0, 0x44e1); A = mem_rd(gb, DE);
  CYC(0x44e1, 0x44e2); alu_or(gb, A);
  if (F & FZ) { CYCT(0x44e2, 0x44e4); goto delete; } // jr z
  CYC(0x44e2, 0x44e4);
  CYC(0x44e4, 0x44e5); alu_xor(gb, A);
  CALL_C(0x44e5, decideItemDrop_hook, 0x16eb, 0x44e8);
  if (F & FZ) { CYCT(0x44e8, 0x44ea); goto delete; } // jr z
  CYC(0x44e8, 0x44ea);
  CYC(0x44ea, 0x44ec); B = 0x01; // PART_ITEM_DROP
  CYC(0x44ec, 0x44ef); objectReplaceWithID_hook(gb); return; // jp

delete:
  CYC(0x44ef, 0x44f2); partDelete_hook(gb); return; // jp

state0:
  CYC(0x44f2, 0x44f3); A = alu_inc8(gb, A);
  CYC(0x44f3, 0x44f4); mem_wr(gb, DE, A); // [state] = 1
  CYC(0x44f4, 0x44f6); E = 0xc2; // Part.subid
  CYC(0x44f6, 0x44f7); A = mem_rd(gb, DE);
  CYC(0x44f7, 0x44f8); alu_or(gb, A);
  CYC(0x44f8, 0x44fa); A = 0x79; // SND_BIG_EXPLOSION
  if (!(F & FZ)) {
    CALL_C_CC(0x44fa, playSound_b00_hook, 0x0c98, 0x44fd);
  } else {
    CYC(0x44fa, 0x44fd);
  }
  CYC(0x44fd, 0x4500); objectSetVisible80_hook(gb); return; // jp
}
