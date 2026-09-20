#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t blackTowerMovingFlames_jump_table(GB *gb) {
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

static void blackTowerMovingFlames_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_BLACK_TOWER_MOVING_FLAMES
void partCode59_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7e9a, 0x7e9c); E = 0xc4; // Part.state
  CYC(0x7e9c, 0x7e9d); A = mem_rd(gb, DE);
  {
    CYC(0x7e9d, 0x7e9e); push_effect(gb, 0x7e9e);
    uint16_t target = blackTowerMovingFlames_jump_table(gb);
    if (target == 0x7ec1) goto state1;
    if (target == 0x7edc) goto state2;
    if (target == 0x7ee7) goto state3;
    if (target == 0x7f03) goto state4;
    if (target == 0x7f30) goto state5;
    goto state0;
  }

state0:
  CYC(0x7eaa, 0x7eab); H = D;
  CYC(0x7eab, 0x7eac); L = E;
  CYC(0x7eac, 0x7ead); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7ead, 0x7eaf); L = 0xd0; // Part.speed
  CYC(0x7eaf, 0x7eb1); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x7eb1, 0x7eb3); L = 0xc2; // Part.subid
  CYC(0x7eb3, 0x7eb4); A = mem_rd(gb, HL);
  CYC(0x7eb4, 0x7eb7); SET_HL(0x7ebd); // table_7ebd
  CYC(0x7eb7, 0x7eb8); blackTowerMovingFlames_addAToHl_from_rst(gb, 0x7eb8);
  CYC(0x7eb8, 0x7eba); E = 0xc6; // Part.counter1
  CYC(0x7eba, 0x7ebb); A = mem_rd(gb, HL);
  CYC(0x7ebb, 0x7ebc); mem_wr(gb, DE, A);
  RET(0x7ebc); return; // ret

state1:
  CALL_C(0x7ec1, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7ec4);
  if (!(F & FZ)) { RET_TAKEN(0x7ec4); return; } // ret nz
  CYC(0x7ec4, 0x7ec5);
  CYC(0x7ec5, 0x7ec6); L = E;
  CYC(0x7ec6, 0x7ec7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7ec7, 0x7ec9); L = 0xc2; // Part.subid
  CYC(0x7ec9, 0x7eca); A = mem_rd(gb, HL);
  CYC(0x7eca, 0x7ecc); alu_xor(gb, 0x03);
  CYC(0x7ecc, 0x7ecf); SET_HL(0x7ebd); // table_7ebd
  CYC(0x7ecf, 0x7ed0); blackTowerMovingFlames_addAToHl_from_rst(gb, 0x7ed0);
  CYC(0x7ed0, 0x7ed2); E = 0xc6; // Part.counter1
  CYC(0x7ed2, 0x7ed3); A = mem_rd(gb, HL);
  CYC(0x7ed3, 0x7ed4); mem_wr(gb, DE, A);
  CYC(0x7ed4, 0x7ed6); A = 0x72; // SND_LIGHTTORCH
  CALL_C(0x7ed6, playSound_b00_hook, 0x0c98, 0x7ed9);
  CYC(0x7ed9, 0x7edc); objectSetVisible83_hook(gb); return; // jp

state2:
  CALL_C(0x7edc, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7edf);
  if (!(F & FZ)) { CYCT(0x7edf, 0x7ee1); goto animate; } // jr nz
  CYC(0x7edf, 0x7ee1);
  CYC(0x7ee1, 0x7ee3); mem_wr(gb, HL, 0x14);
  CYC(0x7ee3, 0x7ee4); L = E;
  CYC(0x7ee4, 0x7ee5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7ee5, 0x7ee7); goto animate; // jr

state3:
  CALL_C(0x7ee7, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7eea);
  if (!(F & FZ)) { CYCT(0x7eea, 0x7eec); goto animate; } // jr nz
  CYC(0x7eea, 0x7eec);
  CYC(0x7eec, 0x7eef); SET_HL(0x6dbc); // blackTower_getMovingFlamesNextTileCoords
  CYC(0x7eef, 0x7ef1); E = 0x10; // bank 0x10
  CALL_C(0x7ef1, interBankCall_hook, 0x008a, 0x7ef4);
  CYC(0x7ef4, 0x7ef5); H = D;
  CYC(0x7ef5, 0x7ef7); L = 0xc4; // Part.state
  CYC(0x7ef7, 0x7ef8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7ef8, 0x7ef9); A = B;
  CYC(0x7ef9, 0x7efa); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7efa, 0x7efc); goto animate; } // jr nz
  CYC(0x7efa, 0x7efc);
  CYC(0x7efc, 0x7efd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7efd, 0x7eff); L = 0xc6; // Part.counter1
  CYC(0x7eff, 0x7f01); mem_wr(gb, HL, 0x10);
  CYC(0x7f01, 0x7f03); goto animate; // jr

state4:
  CYC(0x7f03, 0x7f04); H = D;
  CYC(0x7f04, 0x7f06); L = 0xf0; // Part.var30
  CYC(0x7f06, 0x7f07); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7f07, 0x7f08); B = A;
  CYC(0x7f08, 0x7f09); C = mem_rd(gb, HL);
  CYC(0x7f09, 0x7f0b); L = 0xcb; // Part.yh
  CYC(0x7f0b, 0x7f0c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7f0c, 0x7f0e); mem_wr(gb, 0xff8f, A);
  CYC(0x7f0e, 0x7f0f); L = alu_inc8(gb, L);
  CYC(0x7f0f, 0x7f10); A = mem_rd(gb, HL);
  CYC(0x7f10, 0x7f12); mem_wr(gb, 0xff8e, A);
  CYC(0x7f12, 0x7f13); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x7f13, 0x7f15); goto moveToBC; } // jr nz
  CYC(0x7f13, 0x7f15);
  CYC(0x7f15, 0x7f17); A = mem_rd(gb, 0xff8f);
  CYC(0x7f17, 0x7f18); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x7f18, 0x7f1a); goto moveToBC; } // jr nz
  CYC(0x7f18, 0x7f1a);
  CYC(0x7f1a, 0x7f1b); L = E;
  CYC(0x7f1b, 0x7f1c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7f1c, 0x7f1e); L = 0xc6; // Part.counter1
  CYC(0x7f1e, 0x7f20); mem_wr(gb, HL, 0x10);
  CYC(0x7f20, 0x7f21); L = alu_inc8(gb, L);
  CYC(0x7f21, 0x7f22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7f22, 0x7f24); goto animate; // jr

moveToBC:
  CALL_C(0x7f24, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x7f27);
  CYC(0x7f27, 0x7f29); E = 0xc9; // Part.angle
  CYC(0x7f29, 0x7f2a); mem_wr(gb, DE, A);
  CALL_C(0x7f2a, objectApplySpeed_hook, 0x201d, 0x7f2d);

animate:
  CYC(0x7f2d, 0x7f30); partAnimate_hook(gb); return; // jp

state5:
  CALL_C(0x7f30, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7f33);
  if (!(F & FZ)) { CYCT(0x7f33, 0x7f35); goto animate; } // jr nz
  CYC(0x7f33, 0x7f35);
  CALL_C(0x7f35, objectCreatePuff_hook, 0x24c1, 0x7f38);
  CYC(0x7f38, 0x7f3b); partDelete_hook(gb); return; // jp
}
