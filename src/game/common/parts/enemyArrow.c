#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode1a), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode1a), (from), (to), true)

static uint16_t enemyArrow_jump_table(GB *gb) {
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

void partCode1a_hook(GB *gb) {
  BASE(partCode1a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(b_+7, b_+9); goto partDelete; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); goto func_11_513a; // jr

normalStatus:
  CYC(b_+11, b_+13); E = 0xc2; // Part.subid
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  {
    CYC(b_+14, b_+15); push_effect(gb, b_+15);
    uint16_t target = enemyArrow_jump_table(gb);
    if (target == b_+19) goto subid0;
    goto subid1;
  }

subid0:
  CYC(b_+19, b_+21); E = 0xc4; // Part.state
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  {
    CYC(b_+22, b_+23); push_effect(gb, b_+23);
    uint16_t target = enemyArrow_jump_table(gb);
    if (target == b_+29) goto subid0_state0;
    if (target == b_+57) goto subid0_state1;
    partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(gb); return;
  }

subid0_state0:
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+31); L = E;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+32, b_+34); L = 0xd0; // Part.speed
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x50);
  CYC(b_+36, b_+38); L = 0xcb; // Part.yh
  CYC(b_+38, b_+39); B = mem_rd(gb, HL);
  CYC(b_+39, b_+41); L = 0xcd; // Part.xh
  CYC(b_+41, b_+42); C = mem_rd(gb, HL);
  CALL_C(b_+42, partCommon_setPositionOffsetAndRadiusFromAngle_hook, SYM(partCommon_setPositionOffsetAndRadiusFromAngle), b_+45);
  CYC(b_+45, b_+47); E = 0xc9; // Part.angle
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+50); A = alu_swap(gb, A);
  CYC(b_+50, b_+51); alu_rlca(gb);
  CALL_C(b_+51, partSetAnimation_hook, SYM(partSetAnimation), b_+54);
  CYC(b_+54, b_+57); objectSetVisible81_hook(gb); return; // jp

subid0_state1:
  CALL_C(b_+57, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+60);
  if (!(F & FC)) { CYCT(b_+60, b_+62); goto objectApplySpeed; } // jr nc
  CYC(b_+60, b_+62);
  if (F & FZ) { CYCT(b_+62, b_+64); goto partDelete; } // jr z
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+66); goto func_11_513a; // jr

subid1:
  CYC(b_+66, b_+68); E = 0xc4; // Part.state
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  {
    CYC(b_+69, b_+70); push_effect(gb, b_+70);
    uint16_t target = enemyArrow_jump_table(gb);
    if (target == b_+78) goto subid1_state0;
    if (target == b_+101) goto subid1_state1;
    if (target == b_+57) goto subid0_state1;
    partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(gb); return;
  }

subid1_state0:
  CYC(b_+78, b_+79); H = D;
  CYC(b_+79, b_+80); L = E;
  CYC(b_+80, b_+81); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+81, b_+83); L = 0xc6; // Part.counter1
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x08);
  CYC(b_+85, b_+87); L = 0xd0; // Part.speed
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x50);
  CYC(b_+89, b_+91); E = 0xc9; // Part.angle
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+94); A = alu_swap(gb, A);
  CYC(b_+94, b_+95); alu_rlca(gb);
  CALL_C(b_+95, partSetAnimation_hook, SYM(partSetAnimation), b_+98);
  CYC(b_+98, b_+101); objectSetVisible81_hook(gb); return; // jp

subid1_state1:
  CALL_C(b_+101, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+104);
  if (!(F & FZ)) { CYCT(b_+104, b_+106); goto L_512f; } // jr nz
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+107); L = E;
  CYC(b_+107, b_+108); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+108, b_+110); goto subid0_state1; // jr

L_512f:
  CALL_C(b_+110, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+113);
  if (F & FZ) { CYCT(b_+113, b_+115); goto partDelete; } // jr z
  CYC(b_+113, b_+115);

objectApplySpeed:
  CYC(b_+115, b_+118); objectApplySpeed_hook(gb); return; // jp

partDelete:
  CYC(b_+118, b_+121); partDelete_hook(gb); return; // jp

func_11_513a:
  CYC(b_+121, b_+123); E = 0xc2; // Part.subid
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+125); alu_or(gb, A);
  CYC(b_+125, b_+127); A = 0x02;
  if (F & FZ) { CYCT(b_+127, b_+129); goto L_5144; } // jr z
  CYC(b_+127, b_+129);
  CYC(b_+129, b_+131); A = 0x03;

L_5144:
  CYC(b_+131, b_+133); E = 0xc4; // Part.state
  CYC(b_+133, b_+134); mem_wr(gb, DE, A);
  CYC(b_+134, b_+136); A = 0x04;
  CYC(b_+136, SYM(partCode1b)); partCommon_bounceWhenCollisionsEnabled_hook(gb); return; // jp
}
