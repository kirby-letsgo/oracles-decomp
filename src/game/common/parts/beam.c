#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(func_5758), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(func_5758), (from), (to), true)

static uint16_t beam_jump_table(GB *gb) {
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

static void beam_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void func_5758_hook(GB *gb) {
  BASE(func_5758);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplyComponentSpeed_hook, SYM(objectApplyComponentSpeed), b_+3);
  CYC(b_+3, b_+5); E = 0xc2; // Part.subid
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+10); A = mem_rd(gb, wFrameCounter);
  CYC(b_+10, b_+11); alu_and(gb, B);
  if (F & FZ) { CYCT(b_+11, b_+14); objectSetVisible81_hook(gb); return; } // jp z
  CYC(b_+11, b_+14);
  CYC(b_+14, SYM(partCode2a)); objectSetInvisible_hook(gb); return; // jp
}

void partCode29_hook(GB *gb) {
  BASE(partCode29);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x83);
  if (F & FZ) { CYCT(b_+7, b_+10); partDelete_hook(gb); return; } // jp z
  CYC(b_+7, b_+10);

normalStatus:
  CYC(b_+10, b_+12); E = 0xc4; // Part.state
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  {
    CYC(b_+13, b_+14); push_effect(gb, b_+14);
    uint16_t target = beam_jump_table(gb);
    if (target == b_+20) goto state0;
    if (target == b_+66) goto state1;
    goto state2;
  }

state0:
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+22); L = E;
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+23, b_+25); L = 0xc6; // Part.counter1
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x02);
  CYC(b_+27, b_+29); L = 0xc9; // Part.angle
  CYC(b_+29, b_+30); C = mem_rd(gb, HL);
  CYC(b_+30, b_+32); B = 0x50;
  CYC(b_+32, b_+34); A = 0x04;
  CALL_C(b_+34, objectSetComponentSpeedByScaledVelocity_hook, SYM(objectSetComponentSpeedByScaledVelocity), b_+37);
  CYC(b_+37, b_+39); E = 0xc9; // Part.angle
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_and(gb, 0x0f);
  CYC(b_+42, b_+45); SET_HL(b_+50); // table_5737
  CYC(b_+45, b_+46); beam_addAToHl_from_rst(gb, b_+46);
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+50); partSetAnimation_hook(gb); return; // jp

state1:
  CALL_C(b_+66, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+69);
  if (!(F & FZ)) { CYCT(b_+69, b_+71); func_5758_hook(gb); return; } // jr nz
  CYC(b_+69, b_+71);
  CYC(b_+71, b_+72); L = E;
  CYC(b_+72, b_+73); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state2:
  CALL_C(b_+73, func_5758_hook, SYM(func_5758), b_+76);
  CALL_C(b_+76, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+79);
  if (F & FC) { CYCT(b_+79, b_+82); partDelete_hook(gb); return; } // jp c
  CYC(b_+79, b_+82);
  RET(b_+82); return; // ret
}
