#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void func_7ad3_hook(GB *gb);
void func_7adb_hook(GB *gb);
void func_7b0a_hook(GB *gb);

// PART_ROOM_OF_RITES_FALLING_BOULDER
void partCode54_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7ab3, 0x7ab5); E = 0xc2; // Part.subid
  CYC(0x7ab5, 0x7ab6); A = mem_rd(gb, DE);
  CYC(0x7ab6, 0x7ab7); alu_or(gb, A);
  CYC(0x7ab7, 0x7ab9); E = 0xc4; // Part.state
  if (!(F & FZ)) { CYCT(0x7ab9, 0x7abc); func_7adb_hook(gb); return; } // jp nz
  CYC(0x7ab9, 0x7abc);
  CYC(0x7abc, 0x7abd); A = mem_rd(gb, DE);
  CYC(0x7abd, 0x7abe); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7abe, 0x7ac0); func_7ad3_hook(gb); return; } // jr z
  CYC(0x7abe, 0x7ac0);
  CALL_C(0x7ac0, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7ac3);
  if (F & FZ) { CYCT(0x7ac3, 0x7ac6); partDelete_hook(gb); return; } // jp z
  CYC(0x7ac3, 0x7ac6);
  CYC(0x7ac6, 0x7ac7); A = mem_rd(gb, HL);
  CYC(0x7ac7, 0x7ac9); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(0x7ac9); return; } // ret nz
  CYC(0x7ac9, 0x7aca);
  CALL_C(0x7aca, getFreePartSlot_hook, 0x3e8e, 0x7acd);
  if (!(F & FZ)) { RET_TAKEN(0x7acd); return; } // ret nz
  CYC(0x7acd, 0x7ace);
  CYC(0x7ace, 0x7ad0); mem_wr(gb, HL, 0x54); // PART_ROOM_OF_RITES_FALLING_BOULDER
  CYC(0x7ad0, 0x7ad1); L = alu_inc8(gb, L);
  CYC(0x7ad1, 0x7ad2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x7ad2); return; // ret
}

void func_7ad3_hook(GB *gb) {
  CYC(0x7ad3, 0x7ad4); H = D;
  CYC(0x7ad4, 0x7ad5); L = E;
  CYC(0x7ad5, 0x7ad6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7ad6, 0x7ad8); L = 0xc6; // Part.counter1
  CYC(0x7ad8, 0x7ada); mem_wr(gb, HL, 0x96); // 150
  RET(0x7ada); return; // ret
}

void func_7adb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7adb, 0x7adc); A = mem_rd(gb, DE);
  CYC(0x7adc, 0x7add); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7add, 0x7adf); func_7b0a_hook(gb); return; } // jr nz
  CYC(0x7add, 0x7adf);
  CYC(0x7adf, 0x7ae0); A = alu_inc8(gb, A);
  CYC(0x7ae0, 0x7ae1); mem_wr(gb, DE, A);
  CYC(0x7ae1, 0x7ae3); A = mem_rd(gb, 0xffaa); // hCameraY
  CYC(0x7ae3, 0x7ae4); B = A;
  CYC(0x7ae4, 0x7ae6); A = mem_rd(gb, 0xffac); // hCameraX
  CYC(0x7ae6, 0x7ae7); C = A;
  CALL_C(0x7ae7, getRandomNumber_hook, 0x043e, 0x7aea);
  CYC(0x7aea, 0x7aeb); L = A;
  CYC(0x7aeb, 0x7aed); alu_and(gb, 0x07);
  CYC(0x7aed, 0x7aef); A = alu_swap(gb, A);
  CYC(0x7aef, 0x7af1); alu_add(gb, 0x28);
  CYC(0x7af1, 0x7af2); alu_add(gb, C);
  CYC(0x7af2, 0x7af4); E = 0xcd; // Part.xh
  CYC(0x7af4, 0x7af5); mem_wr(gb, DE, A);
  CYC(0x7af5, 0x7af6); A = L;
  CYC(0x7af6, 0x7af8); alu_and(gb, 0x70);
  CYC(0x7af8, 0x7afa); alu_add(gb, 0x08);
  CYC(0x7afa, 0x7afb); L = A;
  CYC(0x7afb, 0x7afc); alu_add(gb, B);
  CYC(0x7afc, 0x7afe); E = 0xcb; // Part.yh
  CYC(0x7afe, 0x7aff); mem_wr(gb, DE, A);
  CYC(0x7aff, 0x7b00); A = L;
  CYC(0x7b00, 0x7b01); alu_cpl(gb);
  CYC(0x7b01, 0x7b02); A = alu_inc8(gb, A);
  CYC(0x7b02, 0x7b04); alu_sub(gb, 0x07);
  CYC(0x7b04, 0x7b06); E = 0xcf; // Part.zh
  CYC(0x7b06, 0x7b07); mem_wr(gb, DE, A);
  CYC(0x7b07, 0x7b0a); objectSetVisiblec1_hook(gb); return; // jp
}

void func_7b0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b0a, 0x7b0c); C = 0x20;
  CALL_C(0x7b0c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7b0f);
  if (!(F & FZ)) { CYCT(0x7b0f, 0x7b12); partAnimate_hook(gb); return; } // jp nz
  CYC(0x7b0f, 0x7b12);
  CALL_C(0x7b12, objectReplaceWithAnimationIfOnHazard_hook, 0x2225, 0x7b15);
  if (F & FC) { CYCT(0x7b15, 0x7b17); goto delete; } // jr c
  CYC(0x7b15, 0x7b17);
  CYC(0x7b17, 0x7b19); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(0x7b19, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x7b1c);

delete:
  CYC(0x7b1c, 0x7b1f); partDelete_hook(gb); return; // jp
}
