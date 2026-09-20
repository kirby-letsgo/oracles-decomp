#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t donkeyKongFlame_jump_table(GB *gb) {
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

static void donkeyKongFlame_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void func_6261_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6261, 0x6263); E = 0xc9; // Part.angle
  CYC(0x6263, 0x6264); A = mem_rd(gb, DE);
  CYC(0x6264, 0x6266); alu_xor(gb, 0x10);
  CYC(0x6266, 0x6267); mem_wr(gb, DE, A);
  CYC(0x6267, 0x6269); E = 0xc8;
  CYC(0x6269, 0x626a); A = mem_rd(gb, DE);
  CYC(0x626a, 0x626c); alu_xor(gb, 0x01);
  CYC(0x626c, 0x626d); mem_wr(gb, DE, A);
  CYC(0x626d, 0x6270); partSetAnimation_hook(gb); return; // jp
}

void func_6256_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6256, partCommon_getTileCollisionInFront_hook, 0x4000, 0x6259);
  if (!(F & FZ)) { CYCT(0x6259, 0x625b); func_6261_hook(gb); return; } // jr nz
  CYC(0x6259, 0x625b);
  CALL_C(0x625b, objectApplySpeed_hook, 0x201d, 0x625e);
  CYC(0x625e, 0x6261); partAnimate_hook(gb); return; // jp
}

void func_6248_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6248, objectGetShortPosition_hook, 0x2096, 0x624b);
  CYC(0x624b, 0x624d); alu_cp(gb, 0x91);
  if (!(F & FZ)) { CYCT(0x624d, 0x624f); func_6256_hook(gb); return; } // jr nz
  CYC(0x624d, 0x624f);
  CYC(0x624f, 0x6250); SET_HL(pop_effect(gb));
  CALL_C(0x6250, objectCreatePuff_hook, 0x24c1, 0x6253);
  CYC(0x6253, 0x6256); partDelete_hook(gb); return; // jp
}

void func_6270_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6270, 0x6272); A = 0x20;
  CALL_C(0x6272, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x6275);
  if (F & FC) { RET_TAKEN(0x6275); return; } // ret c
  CYC(0x6275, 0x6276);
  CYC(0x6276, 0x6277); A = mem_rd(gb, HL);
  CYC(0x6277, 0x6279); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(0x6279, 0x627b); goto L_6280; } // jr c
  CYC(0x6279, 0x627b);
  CYC(0x627b, 0x627d); mem_wr(gb, HL, 0x02);
  CYC(0x627d, 0x627e); L = alu_dec8(gb, L);
  CYC(0x627e, 0x6280); mem_wr(gb, HL, 0x00);

L_6280:
  CALL_C(0x6280, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6283);
  if (!(F & FZ)) { RET_TAKEN(0x6283); return; } // ret nz
  CYC(0x6283, 0x6284);
  CYC(0x6284, 0x6286); mem_wr(gb, HL, 0x10);
  CYC(0x6286, 0x6289); SET_BC(0x1000);
  CALL_C(0x6289, objectGetRelativeTile_hook, 0x1435, 0x628c);
  CYC(0x628c, 0x628e); alu_sub(gb, 0x19);
  CYC(0x628e, 0x628f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x628f); return; } // ret nz
  CYC(0x628f, 0x6290);
  CALL_C(0x6290, getRandomNumber_hook, 0x043e, 0x6293);
  CYC(0x6293, 0x6295); alu_and(gb, 0x07);
  CYC(0x6295, 0x6298); SET_HL(0x629f); // table_629f
  CYC(0x6298, 0x6299); donkeyKongFlame_addAToHl_from_rst(gb, 0x6299);
  CYC(0x6299, 0x629b); E = 0xc5; // Part.substate
  CYC(0x629b, 0x629c); A = mem_rd(gb, HL);
  CYC(0x629c, 0x629d); mem_wr(gb, DE, A);
  CYC(0x629d, 0x629e); alu_rrca(gb);
  RET(0x629e); return; // ret
}

void partCode2c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(0x615a, 0x615d); partDelete_hook(gb); return; } // jp nz
  CYC(0x615a, 0x615d);
  CYC(0x615d, 0x6160); A = mem_rd(gb, 0xcfd0); // wTmpcfc0 (context-dependent)
  CYC(0x6160, 0x6161); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6161, 0x6163); goto L_6169; } // jr z
  CYC(0x6161, 0x6163);
  CALL_C(0x6163, objectCreatePuff_hook, 0x24c1, 0x6166);
  CYC(0x6166, 0x6169); partDelete_hook(gb); return; // jp

L_6169:
  CYC(0x6169, 0x616b); E = 0xc4; // Part.state
  CYC(0x616b, 0x616c); A = mem_rd(gb, DE);
  {
    CYC(0x616c, 0x616d); push_effect(gb, 0x616d);
    uint16_t target = donkeyKongFlame_jump_table(gb);
    if (target == 0x6177) goto state0;
    if (target == 0x61ac) goto state1;
    if (target == 0x61c2) goto state2;
    if (target == 0x61d2) goto state3;
    goto state4;
  }

state0:
  CYC(0x6177, 0x6178); H = D;
  CYC(0x6178, 0x6179); L = E;
  CYC(0x6179, 0x617a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x617a, 0x617c); L = 0xd0; // Part.speed
  CYC(0x617c, 0x617e); mem_wr(gb, HL, 0x1e);
  CYC(0x617e, 0x6180); E = 0xc2; // Part.subid
  CYC(0x6180, 0x6181); A = mem_rd(gb, DE);
  CYC(0x6181, 0x6183); A = alu_swap(gb, A);
  CYC(0x6183, 0x6185); alu_add(gb, 0x08);
  CYC(0x6185, 0x6187); L = 0xc9; // Part.angle
  CYC(0x6187, 0x6188); mem_wr(gb, HL, A);
  CYC(0x6188, 0x618a); alu_bit(gb, 4, A);
  if (F & FZ) { CYCT(0x618a, 0x618c); goto L_61a7; } // jr z
  CYC(0x618a, 0x618c);
  CYC(0x618c, 0x618e); L = 0xcb; // Part.yh
  CYC(0x618e, 0x6190); mem_wr(gb, HL, 0xfe);
  CALL_C(0x6190, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6193);
  CYC(0x6193, 0x6195); alu_and(gb, 0x07);
  CYC(0x6195, 0x6198); SET_HL(0x629f); // table_629f
  CYC(0x6198, 0x6199); donkeyKongFlame_addAToHl_from_rst(gb, 0x6199);
  CYC(0x6199, 0x619a); A = mem_rd(gb, HL);
  CYC(0x619a, 0x619d); SET_HL(0x61aa); // table_61aa
  CYC(0x619d, 0x619e); donkeyKongFlame_addAToHl_from_rst(gb, 0x619e);
  CYC(0x619e, 0x61a0); E = 0xcd; // Part.xh
  CYC(0x61a0, 0x61a1); A = mem_rd(gb, HL);
  CYC(0x61a1, 0x61a2); mem_wr(gb, DE, A);
  CYC(0x61a2, 0x61a4); A = 0x01;
  CALL_C(0x61a4, partSetAnimation_hook, 0x2988, 0x61a7);

L_61a7:
  CYC(0x61a7, 0x61ac); objectSetVisible82_hook(gb); return; // jp

state1:
  CYC(0x61ac, 0x61ae); A = 0x20;
  CALL_C(0x61ae, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x61b1);
  if (!(F & FC)) { CYCT(0x61b1, 0x61b3); goto animate; } // jr nc
  CYC(0x61b1, 0x61b3);
  CYC(0x61b3, 0x61b4); H = D;
  CYC(0x61b4, 0x61b6); L = 0xc4; // Part.state
  CYC(0x61b6, 0x61b7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x61b7, 0x61b9); L = 0xf1;
  CYC(0x61b9, 0x61bb); mem_wr(gb, HL, 0x04);
  CYC(0x61bb, 0x61bd); L = 0xd4; // Part.speedZ
  CYC(0x61bd, 0x61be); alu_xor(gb, A);
  CYC(0x61be, 0x61bf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x61bf, 0x61c0); mem_wr(gb, HL, A);
  CYC(0x61c0, 0x61c2); goto animate; // jr

state2:
  CYC(0x61c2, 0x61c3); H = D;
  CYC(0x61c3, 0x61c5); L = 0xf1;
  CYC(0x61c5, 0x61c6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x61c6, 0x61c8); goto animate; } // jr nz
  CYC(0x61c6, 0x61c8);
  CYC(0x61c8, 0x61ca); mem_wr(gb, HL, 0x04);
  CYC(0x61ca, 0x61cb); L = E;
  CYC(0x61cb, 0x61cc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x61cc, 0x61cd); L = alu_inc8(gb, L);
  CYC(0x61cd, 0x61cf); mem_wr(gb, HL, 0x00);

animate:
  CYC(0x61cf, 0x61d2); partAnimate_hook(gb); return; // jp

state3:
  CYC(0x61d2, 0x61d4); E = 0xc5; // Part.substate
  CYC(0x61d4, 0x61d5); A = mem_rd(gb, DE);
  {
    CYC(0x61d5, 0x61d6); push_effect(gb, 0x61d6);
    uint16_t target = donkeyKongFlame_jump_table(gb);
    if (target == 0x61da) goto substate0;
    goto substate1;
  }

substate0:
  CALL_C(0x61da, func_6248_hook, 0x6248, 0x61dd);
  CALL_C(0x61dd, func_6270_hook, 0x6270, 0x61e0);
  if (F & FC) { RET_TAKEN(0x61e0); return; } // ret c
  CYC(0x61e0, 0x61e1);
  CYC(0x61e1, 0x61e2); H = D;
  CYC(0x61e2, 0x61e4); L = 0xc4; // Part.state
  CYC(0x61e4, 0x61e5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x61e5); return; // ret

substate1:
  CYC(0x61e6, 0x61e9); SET_BC(0x1000);
  CALL_C(0x61e9, objectGetRelativeTile_hook, 0x1435, 0x61ec);
  CYC(0x61ec, 0x61ee); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(0x61ee, 0x61f1); func_6248_hook(gb); return; } // jp z
  CYC(0x61ee, 0x61f1);
  CYC(0x61f1, 0x61f2); H = D;
  CYC(0x61f2, 0x61f4); L = 0xc5; // Part.substate
  CYC(0x61f4, 0x61f5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x61f5, 0x61f7); L = 0xc6; // Part.counter1
  CYC(0x61f7, 0x61f8); alu_xor(gb, A);
  CYC(0x61f8, 0x61f9); mem_wr(gb, HL, A);
  CYC(0x61f9, 0x61fb); L = 0xd4; // Part.speedZ
  CYC(0x61fb, 0x61fc); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x61fc, 0x61fd); mem_wr(gb, HL, A);
  CYC(0x61fd, 0x61ff); goto substate0; // jr

state4:
  CYC(0x61ff, 0x6201); E = 0xcb; // Part.yh
  CYC(0x6201, 0x6202); A = mem_rd(gb, DE);
  CYC(0x6202, 0x6204); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(0x6204, 0x6207); partDelete_hook(gb); return; } // jp nc
  CYC(0x6204, 0x6207);
  CALL_C(0x6207, func_6248_hook, 0x6248, 0x620a);
  CALL_C(0x620a, func_6270_hook, 0x6270, 0x620d);
  if (!(F & FC)) { RET_TAKEN(0x620d); return; } // ret nc
  CYC(0x620d, 0x620e);
  CYC(0x620e, 0x620f); H = D;
  CYC(0x620f, 0x6211); L = 0xc4; // Part.state
  CYC(0x6211, 0x6213); mem_wr(gb, HL, 0x02);
  CYC(0x6213, 0x6214); alu_xor(gb, A);
  CYC(0x6214, 0x6216); L = 0xd4; // Part.speedZ
  CYC(0x6216, 0x6217); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6217, 0x6218); mem_wr(gb, HL, A);
  CYC(0x6218, 0x621a); L = 0xc6; // Part.counter1
  CYC(0x621a, 0x621b); mem_wr(gb, HL, A);
  CYC(0x621b, 0x621d); E = 0xc2; // Part.subid
  CYC(0x621d, 0x621e); A = mem_rd(gb, DE);
  CYC(0x621e, 0x6220); A = alu_swap(gb, A);
  CYC(0x6220, 0x6221); alu_rrca(gb);
  CYC(0x6221, 0x6222); L = alu_inc8(gb, L);
  CYC(0x6222, 0x6223); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6223, 0x6224); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6224, 0x6227); SET_BC(0x6238); // table_6238
  CALL_C(0x6227, addAToBc_hook, 0x006d, 0x622a);
  CYC(0x622a, 0x622c); L = 0xc9; // Part.angle
  CYC(0x622c, 0x622d); A = mem_rd(gb, BC);
  CYC(0x622d, 0x622e); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x622e, 0x6230); alu_and(gb, 0x10);
  CYC(0x6230, 0x6232); A = alu_swap(gb, A);
  CYC(0x6232, 0x6233); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6233); return; } // ret z
  CYC(0x6233, 0x6234);
  CYC(0x6234, 0x6235); mem_wr(gb, HL, A);
  CYC(0x6235, 0x6238); partSetAnimation_hook(gb); return; // jp
}
