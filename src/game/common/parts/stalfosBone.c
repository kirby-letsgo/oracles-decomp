#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t stalfosBone_jump_table(GB *gb) {
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

void partCode1c_hook(GB *gb) {
  BASE(partCode1c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(b_+7, b_+9); goto partDelete; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); goto func_11_51dd; // jr

normalStatus:
  CYC(b_+11, b_+13); E = 0xc4; // Part.state
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  {
    CYC(b_+14, b_+15); push_effect(gb, b_+15);
    uint16_t target = stalfosBone_jump_table(gb);
    if (target == b_+21) goto state0;
    if (target == b_+37) goto state1;
    goto state2;
  }

state0:
  CYC(b_+21, b_+22); H = D;
  CYC(b_+22, b_+23); L = E;
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+24, b_+26); L = 0xd0; // Part.speed
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+28, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+31);
  CYC(b_+31, b_+33); E = 0xc9; // Part.angle
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+37); TAIL(objectSetVisible81); // jp

state1:
  CALL_C(b_+37, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+40);
  if (F & FC) { CYCT(b_+40, b_+42); goto L_51db; } // jr c
  CYC(b_+40, b_+42);
  CALL_C(b_+42, objectApplySpeed_hook, SYM(objectApplySpeed), b_+45);
  CALL_C(b_+45, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+48);
  if (F & FC) { CYCT(b_+48, b_+51); partAnimate_hook(gb); return; } // jp c
  CYC(b_+48, b_+51);

partDelete:
  CYC(b_+51, b_+54); TAIL(partDelete); // jp

state2:
  CALL_C(b_+54, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+57);
  if (F & FZ) { CYCT(b_+57, b_+59); goto partDelete; } // jr z
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); C = 0x0e;
  CALL_C(b_+61, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+64);
  CALL_C(b_+64, objectApplySpeed_hook, SYM(objectApplySpeed), b_+67);
  CYC(b_+67, b_+70); A = mem_rd(gb, wFrameCounter); // wFrameCounter
  CYC(b_+70, b_+71); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+71); return; } // ret c
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+75); TAIL(partAnimate); // jp

L_51db:
  if (F & FZ) { CYCT(b_+75, b_+77); goto partDelete; } // jr z
  CYC(b_+75, b_+77);

func_11_51dd:
  CYC(b_+77, b_+79); E = 0xc4; // Part.state
  CYC(b_+79, b_+81); A = 0x02;
  CYC(b_+81, b_+82); mem_wr(gb, DE, A);
  CYC(b_+82, b_+83); alu_xor(gb, A);
  CYC(b_+83, b_+86); TAIL(partCommon_bounceWhenCollisionsEnabled); // jp
}
