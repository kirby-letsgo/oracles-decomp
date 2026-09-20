#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode2f), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode2f), (from), (to), true)

static uint16_t babyBall_jump_table(GB *gb) {
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

// PART_BABY_BALL
// Turns Link into a baby
void partCode2f_hook(GB *gb) {
  BASE(partCode2f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); partDelete_hook(gb); return; } // jp nz
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); A = 0x29; // Object.health
  CALL_C(b_+5, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto veranFairyBeat; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); B = H;
  CYC(b_+13, b_+15); E = 0xc4; // Part.state
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  {
    CYC(b_+16, b_+17); push_effect(gb, b_+17);
    uint16_t target = babyBall_jump_table(gb);
    if (target == b_+23) goto state0;
    if (target == b_+42) goto state1;
    goto state2;
  }

state0:
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+25); L = E;
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+28); L = 0xd0; // Part.speed
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x14);
  CYC(b_+30, b_+32); L = 0xc6; // Part.counter1
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x1e);
  CYC(b_+34, b_+36); A = 0xbf; // SND_BLUE_STALFOS_CHARGE
  CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39);
  CYC(b_+39, b_+42); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(b_+42, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+45);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto animate; } // jr nz
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+48); L = E;
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+49, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+52);
  CYC(b_+52, b_+54); E = 0xc9; // Part.angle
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57); A = 0xbb; // SND_BEAM2
  CALL_C(b_+57, playSound_b00_hook, SYM(playSound_b00), b_+60);
  CYC(b_+60, b_+62); goto animate; // jr

state2:
  CYC(b_+62, b_+64); C = 0x84; // Enemy.state
  CYC(b_+64, b_+65); A = mem_rd(gb, BC);
  CYC(b_+65, b_+67); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+67, b_+69); goto applySpeed; } // jr nz
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); C = 0x83; // Enemy.var03
  CYC(b_+71, b_+72); A = mem_rd(gb, BC);
  CYC(b_+72, b_+74); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+74, b_+76); goto applySpeed; } // jr nz
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+79); A = mem_rd(gb, wFrameCounter);
  CYC(b_+79, b_+81); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+81, b_+83); goto applySpeed; } // jr nz
  CYC(b_+81, b_+83);
  CALL_C(b_+83, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+86);
  CALL_C(b_+86, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+89);

applySpeed:
  CALL_C(b_+89, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+92);
  if (F & FZ) { CYCT(b_+92, b_+94); goto delete; } // jr z
  CYC(b_+92, b_+94);
  CALL_C(b_+94, objectApplySpeed_hook, SYM(objectApplySpeed), b_+97);

animate:
  CYC(b_+97, b_+100); partAnimate_hook(gb); return; // jp

veranFairyBeat:
  CALL_C(b_+100, objectCreatePuff_hook, SYM(objectCreatePuff), b_+103);

delete:
  CYC(b_+103, SYM(partCode32)); partDelete_hook(gb); return; // jp
}
