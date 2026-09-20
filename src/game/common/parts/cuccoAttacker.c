#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t cuccoAttacker_jump_table(GB *gb) {
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

static void cuccoAttacker_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void partCode22_hook(GB *gb) {
  BASE(partCode22);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = cuccoAttacker_jump_table(gb);
    if (target == b_+10) goto state0;
    if (target == b_+99) goto state1;
    goto state2;
  }

state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+13, b_+15); L = 0xc6; // Part.counter1
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x18);
  CYC(b_+17, b_+19); L = 0xcf; // Part.zh
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0xfa);
  CYC(b_+21, b_+23); A = 0x30; // Object.var30
  CALL_C(b_+23, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_sub(gb, 0x10);
  CYC(b_+29, b_+31); alu_and(gb, 0x1e);
  CYC(b_+31, b_+32); alu_rrca(gb);
  CYC(b_+32, b_+35); SET_HL(b_+156); // @speedVals
  CYC(b_+35, b_+36); cuccoAttacker_addAToHl_from_rst(gb, b_+36);
  CYC(b_+36, b_+38); E = 0xd0; // Part.speed
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CALL_C(b_+40, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+43);
  CALL_C(b_+43, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+46);
  CYC(b_+46, b_+47); C = A;
  CYC(b_+47, b_+49); alu_and(gb, 0x30);
  CYC(b_+49, b_+50); B = A;
  CYC(b_+50, b_+52); B = alu_swap(gb, B);
  CYC(b_+52, b_+54); alu_and(gb, 0x10);
  CYC(b_+54, b_+57); SET_HL(b_+124); // @xOrYVals
  CYC(b_+57, b_+58); cuccoAttacker_addAToHl_from_rst(gb, b_+58);
  CYC(b_+58, b_+59); A = C;
  CYC(b_+59, b_+61); alu_and(gb, 0x0f);
  CYC(b_+61, b_+62); cuccoAttacker_addAToHl_from_rst(gb, b_+62);
  CYC(b_+62, b_+64); alu_bit(gb, 0, B);
  CYC(b_+64, b_+66); E = 0xcb; // Part.yh
  CYC(b_+66, b_+68); C = 0xcd; // Part.xh
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto L_5482; } // jr nz
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); E = C;
  CYC(b_+71, b_+73); C = 0xcb; // Part.yh

L_5482:
  CYC(b_+73, b_+74); A = mem_rd(gb, HL);
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+76); A = B;
  CYC(b_+76, b_+79); SET_HL(b_+120); // @screenEdgePositions
  CYC(b_+79, b_+80); cuccoAttacker_addAToHl_from_rst(gb, b_+80);
  CYC(b_+80, b_+81); E = C;
  CYC(b_+81, b_+82); A = mem_rd(gb, HL);
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CALL_C(b_+83, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+86);
  CYC(b_+86, b_+88); E = 0xc9; // Part.angle
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+91); alu_cp(gb, 0x11);
  CYC(b_+91, b_+93); A = 0x00;
  if (!(F & FC)) { CYCT(b_+93, b_+95); goto L_5499; } // jr nc
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+96); A = alu_inc8(gb, A);

L_5499:
  CYC(b_+96, b_+99); partSetAnimation_hook(gb); return; // jp

state1:
  CALL_C(b_+99, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+102);
  if (!(F & FZ)) { CYCT(b_+102, b_+104); goto applySpeedAndAnimate; } // jr nz
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+105); L = E;
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+108); goto applySpeedAndAnimate; // jr

state2:
  CALL_C(b_+108, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+111);
  if (!(F & FC)) { CYCT(b_+111, b_+114); partDelete_hook(gb); return; } // jp nc
  CYC(b_+111, b_+114);

applySpeedAndAnimate:
  CALL_C(b_+114, objectApplySpeed_hook, SYM(objectApplySpeed), b_+117);
  CYC(b_+117, b_+120); partAnimate_hook(gb); return; // jp
}
