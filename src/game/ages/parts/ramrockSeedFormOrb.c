#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode4f), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode4f), (from), (to), true)

static uint16_t ramrockSeedFormOrb_jump_table(GB *gb) {
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

// PART_RAMROCK_SEED_FORM_ORB
void partCode4f_hook(GB *gb) {
  BASE(partCode4f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = ramrockSeedFormOrb_jump_table(gb);
    if (target == b_+25) goto state1;
    if (target == b_+58) goto state2;
    goto state0;
  }

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A); // Part.state
  CYC(b_+13, b_+14); A = alu_inc8(gb, A);
  CALL_C(b_+14, partSetAnimation_hook, SYM(partSetAnimation), b_+17);
  CYC(b_+17, b_+19); E = 0xc6; // Part.counter1
  CYC(b_+19, b_+21); A = 0x28;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+25); objectSetVisible80_hook(gb); return; // jp

state1:
  CALL_C(b_+25, partAnimate_hook, SYM(partAnimate), b_+28);
  CYC(b_+28, b_+30); A = 0x02; // Object.state
  CALL_C(b_+30, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+33);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto delete; } // jr nz
  CYC(b_+36, b_+38);
  CALL_C(b_+38, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+41);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; } // ret nz
  CYC(b_+41, b_+42);
  CALL_C(b_+42, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+45);
  CYC(b_+45, b_+47); E = 0xc9; // Part.angle
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); A = 0x50;
  CYC(b_+50, b_+52); E = 0xd0; // Part.speed
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+55); E = 0xc4; // Part.state
  CYC(b_+55, b_+57); A = 0x02;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);

state2:
  CALL_C(b_+58, partAnimate_hook, SYM(partAnimate), b_+61);
  CALL_C(b_+61, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+64);
  if (!(F & FZ)) { CYCT(b_+64, b_+66); goto func_7aa9; } // jr nz
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x0a);
  CALL_C(b_+68, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+71);
  CYC(b_+71, b_+74); objectNudgeAngleTowards_hook(gb); return; // jp

func_7aa9:
  CALL_C(b_+74, objectApplySpeed_hook, SYM(objectApplySpeed), b_+77);
  CALL_C(b_+77, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+80);
  if (F & FC) { RET_TAKEN(b_+80); return; } // ret c
  CYC(b_+80, b_+81);

delete:
  CYC(b_+81, SYM(partCode54)); partDelete_hook(gb); return; // jp
}
