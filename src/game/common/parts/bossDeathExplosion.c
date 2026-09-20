#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode04_hook(GB *gb) {
  BASE(partCode04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto state0; } // jr z
  CYC(b_+4, b_+6);

  CYC(b_+6, b_+8); E = 0xe1; // Part.animParameter
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);

  CALL_C(b_+13, decNumEnemies_hook, SYM(decNumEnemies), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto delete; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); E = 0xc2; // Part.subid
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+22, b_+24); goto delete; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CALL_C(b_+25, decideItemDrop_hook, SYM(decideItemDrop), b_+28);
  if (F & FZ) { CYCT(b_+28, b_+30); goto delete; } // jr z
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); B = 0x01; // PART_ITEM_DROP
  CYC(b_+32, b_+35); objectReplaceWithID_hook(gb); return; // jp

delete:
  CYC(b_+35, b_+38); partDelete_hook(gb); return; // jp

state0:
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  CYC(b_+39, b_+40); mem_wr(gb, DE, A); // [state] = 1
  CYC(b_+40, b_+42); E = 0xc2; // Part.subid
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_or(gb, A);
  CYC(b_+44, b_+46); A = 0x79; // SND_BIG_EXPLOSION
  if (!(F & FZ)) {
    CALL_C_CC(b_+46, playSound_b00_hook, SYM(playSound_b00), b_+49);
  } else {
    CYC(b_+46, b_+49);
  }
  CYC(b_+49, b_+52); objectSetVisible80_hook(gb); return; // jp
}
