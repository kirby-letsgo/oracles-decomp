#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void func_7ad3_hook(GB *gb);
void func_7adb_hook(GB *gb);
void func_7b0a_hook(GB *gb);

// PART_ROOM_OF_RITES_FALLING_BOULDER
void partCode54_hook(GB *gb) {
  BASE(partCode54);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+6); E = 0xc4; // Part.state
  if (!(F & FZ)) { CYCT(b_+6, b_+9); func_7adb_hook(gb); return; } // jp nz
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); func_7ad3_hook(gb); return; } // jr z
  CYC(b_+11, b_+13);
  CALL_C(b_+13, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+16);
  if (F & FZ) { CYCT(b_+16, b_+19); partDelete_hook(gb); return; } // jp z
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; } // ret nz
  CYC(b_+22, b_+23);
  CALL_C(b_+23, getFreePartSlot_hook, SYM(getFreePartSlot), b_+26);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } // ret nz
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x54); // PART_ROOM_OF_RITES_FALLING_BOULDER
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+31); return; // ret
}

void func_7ad3_hook(GB *gb) {
  BASE(func_7ad3);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xc6; // Part.counter1
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x96); // 150
  RET(b_+7); return; // ret
}

void func_7adb_hook(GB *gb) {
  BASE(func_7adb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); func_7b0a_hook(gb); return; } // jr nz
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); A = mem_rd(gb, hCameraY); // hCameraY
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+11); A = mem_rd(gb, hCameraX); // hCameraX
  CYC(b_+11, b_+12); C = A;
  CALL_C(b_+12, getRandomNumber_hook, SYM(getRandomNumber), b_+15);
  CYC(b_+15, b_+16); L = A;
  CYC(b_+16, b_+18); alu_and(gb, 0x07);
  CYC(b_+18, b_+20); A = alu_swap(gb, A);
  CYC(b_+20, b_+22); alu_add(gb, 0x28);
  CYC(b_+22, b_+23); alu_add(gb, C);
  CYC(b_+23, b_+25); E = 0xcd; // Part.xh
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); A = L;
  CYC(b_+27, b_+29); alu_and(gb, 0x70);
  CYC(b_+29, b_+31); alu_add(gb, 0x08);
  CYC(b_+31, b_+32); L = A;
  CYC(b_+32, b_+33); alu_add(gb, B);
  CYC(b_+33, b_+35); E = 0xcb; // Part.yh
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+37); A = L;
  CYC(b_+37, b_+38); alu_cpl(gb);
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  CYC(b_+39, b_+41); alu_sub(gb, 0x07);
  CYC(b_+41, b_+43); E = 0xcf; // Part.zh
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+47); objectSetVisiblec1_hook(gb); return; // jp
}

void func_7b0a_hook(GB *gb) {
  BASE(func_7b0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CALL_C(b_+8, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+11);
  if (F & FC) { CYCT(b_+11, b_+13); goto delete; } // jr c
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(b_+15, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+18);

delete:
  CYC(b_+18, b_+21); partDelete_hook(gb); return; // jp
}
