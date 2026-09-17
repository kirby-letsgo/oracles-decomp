#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t smallFairy_jump_table(GB *gb) {
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

static void smallFairy_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void func_56b6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56b6, 0x56b8); E = 0xc9; // Part.angle
  CYC(0x56b8, 0x56b9); A = mem_rd(gb, DE);
  CYC(0x56b9, 0x56bb); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(0x56bb); return; } // ret z
  CYC(0x56bb, 0x56bc);
  CYC(0x56bc, 0x56bd); A = mem_rd(gb, DE);
  CYC(0x56bd, 0x56bf); alu_cp(gb, 0x10);
  CYC(0x56bf, 0x56c1); A = 0x00;
  if (!(F & FC)) { CYCT(0x56c1, 0x56c3); goto L_56c4; } // jr nc
  CYC(0x56c1, 0x56c3);
  CYC(0x56c3, 0x56c4); A = alu_inc8(gb, A);

L_56c4:
  CYC(0x56c4, 0x56c5); H = D;
  CYC(0x56c5, 0x56c7); L = 0xc8;
  CYC(0x56c7, 0x56c8); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x56c8); return; } // ret z
  CYC(0x56c8, 0x56c9);
  CYC(0x56c9, 0x56ca); mem_wr(gb, HL, A);
  CYC(0x56ca, 0x56cd); partSetAnimation_hook(gb); return; // jp
}

void func_56cd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56cd, 0x56cf); E = 0xc9; // Part.angle
  CYC(0x56cf, 0x56d0); A = mem_rd(gb, DE);
  CYC(0x56d0, 0x56d2); alu_and(gb, 0x07);
  CYC(0x56d2, 0x56d3); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(0x56d3, 0x56d5); goto L_56d9; } // jr z
  CYC(0x56d3, 0x56d5);
  CYC(0x56d5, 0x56d7); alu_and(gb, 0x18);
  CYC(0x56d7, 0x56d9); alu_add(gb, 0x04);

L_56d9:
  CYC(0x56d9, 0x56db); alu_and(gb, 0x1c);
  CYC(0x56db, 0x56dc); alu_rrca(gb);
  CYC(0x56dc, 0x56df); SET_HL(0x56f5); // table_56f5
  CYC(0x56df, 0x56e0); smallFairy_addAToHl_from_rst(gb, 0x56e0);
  CYC(0x56e0, 0x56e2); E = 0xcb; // Part.yh
  CYC(0x56e2, 0x56e3); A = mem_rd(gb, DE);
  CYC(0x56e3, 0x56e4); alu_add(gb, mem_rd(gb, HL));
  CYC(0x56e4, 0x56e5); B = A;
  CYC(0x56e5, 0x56e7); E = 0xcd; // Part.xh
  CYC(0x56e7, 0x56e8); SET_HL(HL + 1);
  CYC(0x56e8, 0x56e9); A = mem_rd(gb, DE);
  CYC(0x56e9, 0x56ea); alu_add(gb, mem_rd(gb, HL));
  CYC(0x56ea, 0x56ec); alu_sub(gb, 0x38);
  CYC(0x56ec, 0x56ee); alu_cp(gb, 0x80);
  if (!(F & FC)) { RET_TAKEN(0x56ee); return; } // ret nc
  CYC(0x56ee, 0x56ef);
  CYC(0x56ef, 0x56f0); A = B;
  CYC(0x56f0, 0x56f2); alu_sub(gb, 0x18);
  CYC(0x56f2, 0x56f4); alu_cp(gb, 0x50);
  RET(0x56f4); return; // ret
}

void partCode28_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x560d, 0x560f); goto normalStatus; } // jr z
  CYC(0x560d, 0x560f);
  CYC(0x560f, 0x5611); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x5611, 0x5614); goto collected; } // jp z
  CYC(0x5611, 0x5614);
  CYC(0x5614, 0x5616); E = 0xc4; // Part.state
  CYC(0x5616, 0x5618); A = 0x02;
  CYC(0x5618, 0x5619); mem_wr(gb, DE, A);

normalStatus:
  CYC(0x5619, 0x561b); E = 0xc4; // Part.state
  CYC(0x561b, 0x561c); A = mem_rd(gb, DE);
  {
    CYC(0x561c, 0x561d); push_effect(gb, 0x561d);
    uint16_t target = smallFairy_jump_table(gb);
    if (target == 0x5623) goto state0;
    if (target == 0x5638) goto state1;
    goto state2;
  }

state0:
  CYC(0x5623, 0x5624); H = D;
  CYC(0x5624, 0x5626); L = 0xc4; // Part.state
  CYC(0x5626, 0x5627); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5627, 0x5629); L = 0xcf; // Part.zh
  CYC(0x5629, 0x562b); mem_wr(gb, HL, 0xfa);
  CYC(0x562b, 0x562d); L = 0xf1; // Part.var31
  CYC(0x562d, 0x562f); E = 0xcb; // Part.yh
  CYC(0x562f, 0x5630); A = mem_rd(gb, DE);
  CYC(0x5630, 0x5631); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5631, 0x5633); E = 0xcd; // Part.xh
  CYC(0x5633, 0x5634); A = mem_rd(gb, DE);
  CYC(0x5634, 0x5635); mem_wr(gb, HL, A);
  CYC(0x5635, 0x5638); objectSetVisiblec2_hook(gb); return; // jp

state1:
  CALL_C(0x5638, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x563b);
  if (F & FZ) { CYCT(0x563b, 0x563d); goto L_5643; } // jr z
  CYC(0x563b, 0x563d);
  CALL_C(0x563d, func_56cd_hook, 0x56cd, 0x5640);
  if (F & FC) { CYCT(0x5640, 0x5643); objectApplySpeed_hook(gb); return; } // jp c
  CYC(0x5640, 0x5643);

L_5643:
  CALL_C(0x5643, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5646);
  CYC(0x5646, 0x5648); alu_and(gb, 0x3e);
  CYC(0x5648, 0x564a); alu_add(gb, 0x08);
  CYC(0x564a, 0x564c); E = 0xc6; // Part.counter1
  CYC(0x564c, 0x564d); mem_wr(gb, DE, A);
  CALL_C(0x564d, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5650);
  CYC(0x5650, 0x5652); alu_and(gb, 0x03);
  CYC(0x5652, 0x5655); SET_HL(0x5666); // table_5666
  CYC(0x5655, 0x5656); smallFairy_addAToHl_from_rst(gb, 0x5656);
  CYC(0x5656, 0x5658); E = 0xd0; // Part.speed
  CYC(0x5658, 0x5659); A = mem_rd(gb, HL);
  CYC(0x5659, 0x565a); mem_wr(gb, DE, A);
  CALL_C(0x565a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x565d);
  CYC(0x565d, 0x565f); alu_and(gb, 0x1e);
  CYC(0x565f, 0x5660); H = D;
  CYC(0x5660, 0x5662); L = 0xc9; // Part.angle
  CYC(0x5662, 0x5663); mem_wr(gb, HL, A);
  CYC(0x5663, 0x5666); func_56b6_hook(gb); return; // jp

state2:
  CYC(0x566a, 0x566c); E = 0xc5; // Part.substate
  CYC(0x566c, 0x566d); A = mem_rd(gb, DE);
  CYC(0x566d, 0x566e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x566e, 0x5670); goto L_5683; } // jr nz
  CYC(0x566e, 0x5670);
  CYC(0x5670, 0x5671); H = D;
  CYC(0x5671, 0x5672); L = E;
  CYC(0x5672, 0x5673); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5673, 0x5675); L = 0xcf; // Part.zh
  CYC(0x5675, 0x5677); mem_wr(gb, HL, 0x00);
  CYC(0x5677, 0x5679); A = 0x01;
  CALL_C(0x5679, objectGetRelatedObject1Var_hook, 0x2160, 0x567c);
  CYC(0x567c, 0x567d); A = mem_rd(gb, HL);
  CYC(0x567d, 0x567f); E = 0xf0; // Part.var30
  CYC(0x567f, 0x5680); mem_wr(gb, DE, A);
  CALL_C(0x5680, objectSetVisible80_hook, 0x1e57, 0x5683);

L_5683:
  CALL_C(0x5683, objectCheckCollidedWithLink_hook, 0x1c41, 0x5686);
  if (F & FC) { CYCT(0x5686, 0x5689); goto collected; } // jp c
  CYC(0x5686, 0x5689);
  CYC(0x5689, 0x568b); A = 0x00;
  CALL_C(0x568b, objectGetRelatedObject1Var_hook, 0x2160, 0x568e);
  CYC(0x568e, 0x568f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x568f, 0x5690); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5690, 0x5692); goto L_5699; } // jr z
  CYC(0x5690, 0x5692);
  CYC(0x5692, 0x5694); E = 0xf0; // Part.var30
  CYC(0x5694, 0x5695); A = mem_rd(gb, DE);
  CYC(0x5695, 0x5696); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5696, 0x5699); objectTakePosition_hook(gb); return; } // jp z
  CYC(0x5696, 0x5699);

L_5699:
  CYC(0x5699, 0x569c); partDelete_hook(gb); return; // jp

collected:
  CYC(0x569c, 0x569e); A = 0x26;
  CALL_C(0x569e, cpActiveRing_hook, 0x23b0, 0x56a1);
  CYC(0x56a1, 0x56a3); C = 0x18;
  if (F & FZ) { CYCT(0x56a3, 0x56a5); goto L_56ac; } // jr z
  CYC(0x56a3, 0x56a5);
  CYC(0x56a5, 0x56a7); A = 0x25;
  CALL_C(0x56a7, cpActiveRing_hook, 0x23b0, 0x56aa);
  if (!(F & FZ)) { CYCT(0x56aa, 0x56ac); goto L_56ae; } // jr nz
  CYC(0x56aa, 0x56ac);

L_56ac:
  CYC(0x56ac, 0x56ae); C = 0x30;

L_56ae:
  CYC(0x56ae, 0x56b0); A = 0x29;
  CALL_C(0x56b0, giveTreasure_hook, 0x171c, 0x56b3);
  CYC(0x56b3, 0x56b6); partDelete_hook(gb); return; // jp
}
