#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode18), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode18), (from), (to), true)

static uint16_t octorokProjectile_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void partCode18_hook(GB *gb) {
  BASE(partCode18);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(b_+7, b_+10); partDelete_hook(gb); return; } // jp z
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = 0xc4; // Part.state
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto normalStatus; } // jr nc
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x02);

normalStatus:
  CYC(b_+20, b_+22); E = 0xc4; // Part.state
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  {
    CYC(b_+23, b_+24); push_effect(gb, b_+24);
    uint16_t target = octorokProjectile_jump_table(gb);
    if (target == b_+32) goto state0;
    if (target == b_+42) goto state1;
    if (target == b_+64) goto state2;
    partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(gb); return;
  }

state0:
  CYC(b_+32, b_+33); H = D;
  CYC(b_+33, b_+34); L = E;
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+35, b_+37); L = 0xd0; // Part.speed
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x50);
  CYC(b_+39, b_+42); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(b_+42, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+45);
  if (!(F & FC)) { CYCT(b_+45, b_+48); partDelete_hook(gb); return; } // jp nc
  CYC(b_+45, b_+48);
  CALL_C(b_+48, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+51);
  if (!(F & FC)) { CYCT(b_+51, b_+53); goto L_5063; } // jr nc
  CYC(b_+51, b_+53);
  if (F & FZ) { CYCT(b_+53, b_+56); partDelete_hook(gb); return; } // jp z
  CYC(b_+53, b_+56);
  CYC(b_+56, b_+58); E = 0xc4; // Part.state
  CYC(b_+58, b_+60); A = 0x02;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);

L_5063:
  CYC(b_+61, b_+64); objectApplySpeed_hook(gb); return; // jp

state2:
  CYC(b_+64, b_+66); A = 0x03;
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+68); alu_xor(gb, A);
  CYC(b_+68, SYM(partCode19)); partCommon_bounceWhenCollisionsEnabled_hook(gb); return; // jp
}
