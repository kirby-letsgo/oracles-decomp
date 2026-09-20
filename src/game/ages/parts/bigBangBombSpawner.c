#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t bigBangBombSpawner_jump_table(GB *gb) {
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

static void bigBangBombSpawner_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void bigBangBombSpawner_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void bbbs_setHazardAndReturn(GB *gb) {
  CYC(0x78df, 0x78e1); E = 0xf4;
  CYC(0x78e1, 0x78e2); mem_wr(gb, DE, A);
  RET(0x78e2); return;
}

void partCode49_hook(GB *gb);
void func_77f0_hook(GB *gb);
void func_7805_hook(GB *gb);
void func_7858_hook(GB *gb);
void func_786f_hook(GB *gb);
void func_78bd_hook(GB *gb);
void func_78ce_hook(GB *gb);
void func_78d2_hook(GB *gb);
void func_78dd_hook(GB *gb);
void func_78e3_hook(GB *gb);
void func_793b_hook(GB *gb);
void func_79ab_hook(GB *gb);
void func_79c4_hook(GB *gb);
void func_79cb_hook(GB *gb);
void func_79d2_hook(GB *gb);
void func_79d9_hook(GB *gb);

void partCode49_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7735, 0x7737); E = 0xc4; // Part.state
  CYC(0x7737, 0x7738); A = mem_rd(gb, DE);
  {
    CYC(0x7738, 0x7739); push_effect(gb, 0x7739);
    uint16_t target = bigBangBombSpawner_jump_table(gb);
    if (target == 0x7745) goto state0;
    if (target == 0x7765) goto state1;
    if (target == 0x7788) goto state2;
    if (target == 0x77a2) goto state3;
    if (target == 0x77cf) goto state4;
    goto state5;
  }

state0:
  CYC(0x7745, 0x7746); H = D;
  CYC(0x7746, 0x7748); L = 0xc2;
  CYC(0x7748, 0x7749); A = mem_rd(gb, HL);
  CYC(0x7749, 0x774b); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x774b, 0x774d); goto func_7754; } // jr nz
  CYC(0x774b, 0x774d);
  CYC(0x774d, 0x774f); L = 0xc4;
  CYC(0x774f, 0x7751); mem_wr(gb, HL, 0x05);
  CYC(0x7751, 0x7754); func_77f0_hook(gb); return; // jp

func_7754:
  CYC(0x7754, 0x7756); L = 0xc4;
  CYC(0x7756, 0x7757); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x7757, func_78e3_hook, 0x78e3, 0x775a);
  CALL_C(0x775a, func_793b_hook, 0x793b, 0x775d);
  CYC(0x775d, 0x775f); A = 0x98; // SND_POOF
  CALL_C(0x775f, playSound_b00_hook, 0x0c98, 0x7762);
  CALL_C(0x7762, objectSetVisiblec1_hook, 0x1e3c, 0x7765);

state1:
  CALL_C(0x7765, objectApplySpeed_hook, 0x201d, 0x7768);
  CYC(0x7768, 0x7769); H = D;
  CYC(0x7769, 0x776b); L = 0xf1;
  CYC(0x776b, 0x776c); C = mem_rd(gb, HL);
  CALL_C(0x776c, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x776f);
  if (F & FC) { CYCT(0x776f, 0x7771); goto state1_noBounce; } // jr c
  CYC(0x776f, 0x7771);
  if (!(F & FZ)) { CYCT(0x7771, 0x7773); goto state1_inAir; } // jr nz
  CYC(0x7771, 0x7773);
  CYC(0x7773, 0x7775); E = 0xd0;
  CYC(0x7775, 0x7776); A = mem_rd(gb, DE);
  CYC(0x7776, 0x7778); A = alu_srl(gb, A);
  CYC(0x7778, 0x7779); mem_wr(gb, DE, A);

state1_inAir:
  CYC(0x7779, 0x777c); partAnimate_hook(gb); return; // jp

state1_noBounce:
  CYC(0x777c, 0x777d); H = D;
  CYC(0x777d, 0x777f); L = 0xc4;
  CYC(0x777f, 0x7781); mem_wr(gb, HL, 0x03);
  CYC(0x7781, 0x7783); L = 0xc6;
  CYC(0x7783, 0x7785); mem_wr(gb, HL, 0x14);
  CYC(0x7785, 0x7788); partAnimate_hook(gb); return; // jp

state2:
  CYC(0x7788, 0x7789); E = alu_inc8(gb, E);
  CYC(0x7789, 0x778a); A = mem_rd(gb, DE);
  {
    CYC(0x778a, 0x778b); push_effect(gb, 0x778b);
    uint16_t target = bigBangBombSpawner_jump_table(gb);
    if (target == 0x7793) goto state2_substate0;
    if (target == 0x779d) goto state2_substate3;
    goto state2_substateStub;
  }

state2_substate0:
  CYC(0x7793, 0x7794); alu_xor(gb, A);
  CYC(0x7794, 0x7797); mem_wr(gb, 0xcc5b, A); // wLinkGrabState2
  CYC(0x7797, 0x7798); A = alu_inc8(gb, A);
  CYC(0x7798, 0x7799); mem_wr(gb, DE, A);
  CYC(0x7799, 0x779c); objectSetVisiblec1_hook(gb); return; // jp

state2_substateStub:
  RET(0x779c); return; // ret

state2_substate3:
  CALL_C(0x779d, objectSetVisiblec2_hook, 0x1e45, 0x77a0);
  CYC(0x77a0, 0x77a2); goto func_77b1; // jr

state3:
  CYC(0x77a2, 0x77a3); H = D;
  CYC(0x77a3, 0x77a5); L = 0xc6;
  CYC(0x77a5, 0x77a6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(0x77a6, 0x77a8); goto func_77b1; } // jr z
  CYC(0x77a6, 0x77a8);
  CALL_C(0x77a8, partAnimate_hook, 0x2978, 0x77ab);
  CALL_C(0x77ab, func_79ab_hook, 0x79ab, 0x77ae);
  CYC(0x77ae, 0x77b1); objectAddToGrabbableObjectBuffer_hook(gb); return; // jp

func_77b1:
  CYC(0x77b1, 0x77b2); H = D;
  CYC(0x77b2, 0x77b4); L = 0xc4;
  CYC(0x77b4, 0x77b6); mem_wr(gb, HL, 0x04);
  CYC(0x77b6, 0x77b8); L = 0xe4;
  CYC(0x77b8, 0x77ba); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x77ba, 0x77bc); L = 0xdb;
  CYC(0x77bc, 0x77be); A = 0x0a;
  CYC(0x77be, 0x77bf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x77bf, 0x77c0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x77c0, 0x77c2); mem_wr(gb, HL, 0x0c);
  CYC(0x77c2, 0x77c4); A = 0x01;
  CALL_C(0x77c4, partSetAnimation_hook, 0x2988, 0x77c7);
  CYC(0x77c7, 0x77c9); A = 0x6f; // SND_EXPLOSION
  CALL_C(0x77c9, playSound_b00_hook, 0x0c98, 0x77cc);
  CYC(0x77cc, 0x77cf); objectSetVisible83_hook(gb); return; // jp

state4:
  CALL_C(0x77cf, partAnimate_hook, 0x2978, 0x77d2);
  CYC(0x77d2, 0x77d4); E = 0xe1;
  CYC(0x77d4, 0x77d5); A = mem_rd(gb, DE);
  CYC(0x77d5, 0x77d6); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x77d6, 0x77d9); partDelete_hook(gb); return; } // jp z
  CYC(0x77d6, 0x77d9);
  CYC(0x77d9, 0x77da); A = alu_dec8(gb, A);
  CYC(0x77da, 0x77dc); E = 0xe6;
  CYC(0x77dc, 0x77dd); mem_wr(gb, DE, A);
  CYC(0x77dd, 0x77de); E = alu_inc8(gb, E);
  CYC(0x77de, 0x77df); mem_wr(gb, DE, A);
  RET(0x77df); return; // ret

state5:
  CYC(0x77e0, 0x77e1); H = D;
  CYC(0x77e1, 0x77e3); L = 0xf0;
  CYC(0x77e3, 0x77e4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x77e4); return; } // ret nz
  CYC(0x77e4, 0x77e5);
  CYC(0x77e5, 0x77e7); L = 0xc6;
  CYC(0x77e7, 0x77e8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x77e8, func_77f0_hook, 0x77f0, 0x77eb);
  if (F & FZ) { CYCT(0x77eb, 0x77ee); partDelete_hook(gb); return; } // jp z
  CYC(0x77eb, 0x77ee);
  CYC(0x77ee, 0x77f0); func_7858_hook(gb); return; // jr
}

void func_77f0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x77f0, 0x77f1); H = D;
  CYC(0x77f1, 0x77f3); L = 0xc6;
  CYC(0x77f3, 0x77f4); A = mem_rd(gb, HL);
  CYC(0x77f4, 0x77f7); SET_BC(0x780f);
  CALL_C(0x77f7, addDoubleIndexToBc_hook, 0x007e, 0x77fa);
  CYC(0x77fa, 0x77fb); A = mem_rd(gb, BC);
  CYC(0x77fb, 0x77fd); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x77fd, 0x77ff); func_7805_hook(gb); return; } // jr nz
  CYC(0x77fd, 0x77ff);
  CYC(0x77ff, 0x7801); A = 0x01;
  CYC(0x7801, 0x7804); mem_wr(gb, 0xcfc0, A);
  RET(0x7804); return; // ret
}

void func_7805_hook(GB *gb) {
  CYC(0x7805, 0x7807); L = 0xf0;
  CYC(0x7807, 0x7808); mem_wr(gb, HL, A);
  CYC(0x7808, 0x7809); SET_BC(BC + 1);
  CYC(0x7809, 0x780a); A = mem_rd(gb, BC);
  CYC(0x780a, 0x780c); L = 0xf5;
  CYC(0x780c, 0x780d); mem_wr(gb, HL, A);
  CYC(0x780d, 0x780e); alu_or(gb, D);
  RET(0x780e); return; // ret
}

void func_7858_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7858, 0x7859); alu_xor(gb, A);
  CYC(0x7859, 0x785b); E = 0xf2;
  CYC(0x785b, 0x785c); mem_wr(gb, DE, A);
  CYC(0x785c, 0x785d); E = alu_inc8(gb, E);
  CYC(0x785d, 0x785e); mem_wr(gb, DE, A);
  CALL_C(0x785e, func_78bd_hook, 0x78bd, 0x7861);
  CYC(0x7861, 0x7863); E = 0xf5;
  CYC(0x7863, 0x7864); A = mem_rd(gb, DE);
loop:
  CYC(0x7864, 0x7866); hram_wr(gb, 0x92, A);
  CALL_C(0x7866, func_786f_hook, 0x786f, 0x7869);
  CYC(0x7869, 0x786b); A = hram_rd(gb, 0x92);
  CYC(0x786b, 0x786c); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x786c, 0x786e); goto loop; } // jr nz
  CYC(0x786c, 0x786e);
  RET(0x786e); return; // ret
}

void func_786f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
restart:
  CYC(0x786f, 0x7871); E = 0xf4;
  CYC(0x7871, 0x7872); A = mem_rd(gb, DE);
  CYC(0x7872, 0x7873); alu_add(gb, A);
  CYC(0x7873, 0x7874); alu_add(gb, A);
  CYC(0x7874, 0x7877); SET_BC(0x789d);
  CALL_C(0x7877, addDoubleIndexToBc_hook, 0x007e, 0x787a);
  CALL_C(0x787a, getRandomNumber_hook, 0x043e, 0x787d);
  CYC(0x787d, 0x787f); alu_and(gb, 0x07);
  CALL_C(0x787f, addAToBc_hook, 0x006d, 0x7882);
  CYC(0x7882, 0x7883); A = mem_rd(gb, BC);
  CYC(0x7883, 0x7885); hram_wr(gb, 0x8b, A);
  CYC(0x7885, 0x7886); H = D;
  CYC(0x7886, 0x7888); L = 0xf2;
  CALL_C(0x7888, checkFlag_hook, 0x0205, 0x788b);
  if (!(F & FZ)) { CYCT(0x788b, 0x788d); goto restart; } // jr nz
  CYC(0x788b, 0x788d);
  CALL_C(0x788d, getFreePartSlot_hook, 0x3e8e, 0x7890);
  if (!(F & FZ)) { RET_TAKEN(0x7890); return; } // ret nz
  CYC(0x7890, 0x7891);
  CYC(0x7891, 0x7893); mem_wr(gb, HL, 0x49); // PART_BIGBANG_BOMB_SPAWNER
  CYC(0x7893, 0x7894); L = alu_inc8(gb, L);
  CYC(0x7894, 0x7896); A = hram_rd(gb, 0x8b);
  CYC(0x7896, 0x7897); mem_wr(gb, HL, A);
  CYC(0x7897, 0x7898); H = D;
  CYC(0x7898, 0x789a); L = 0xf2;
  CYC(0x789a, 0x789d); setFlag_hook(gb); return; // jp
}

void func_78bd_hook(GB *gb) {
  CYC(0x78bd, 0x78c0); A = mem_rd(gb, 0xd00d); // w1Link.xh
  CYC(0x78c0, 0x78c2); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(0x78c2, 0x78c4); func_78d2_hook(gb); return; } // jr nc
  CYC(0x78c2, 0x78c4);
  CYC(0x78c4, 0x78c7); A = mem_rd(gb, 0xd00b); // w1Link.yh
  CYC(0x78c7, 0x78c9); alu_cp(gb, 0x40);
  if (!(F & FC)) { CYCT(0x78c9, 0x78cb); func_78ce_hook(gb); return; } // jr nc
  CYC(0x78c9, 0x78cb);
  CYC(0x78cb, 0x78cc); alu_xor(gb, A);
  CYC(0x78cc, 0x78ce); bbbs_setHazardAndReturn(gb); return; // jr
}

void func_78ce_hook(GB *gb) {
  CYC(0x78ce, 0x78d0); A = 0x01;
  CYC(0x78d0, 0x78d2); bbbs_setHazardAndReturn(gb); return; // jr
}

void func_78d2_hook(GB *gb) {
  CYC(0x78d2, 0x78d5); A = mem_rd(gb, 0xd00b); // w1Link.yh
  CYC(0x78d5, 0x78d7); alu_cp(gb, 0x40);
  if (!(F & FC)) { CYCT(0x78d7, 0x78d9); func_78dd_hook(gb); return; } // jr nc
  CYC(0x78d7, 0x78d9);
  CYC(0x78d9, 0x78db); A = 0x02;
  CYC(0x78db, 0x78dd); bbbs_setHazardAndReturn(gb); return; // jr
}

void func_78dd_hook(GB *gb) {
  CYC(0x78dd, 0x78df); A = 0x03;
  bbbs_setHazardAndReturn(gb); return;
}

void func_78e3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x78e3, 0x78e4); H = D;
  CYC(0x78e4, 0x78e6); L = 0xc2;
  CYC(0x78e6, 0x78e7); A = mem_rd(gb, HL);
  CYC(0x78e7, 0x78ea); SET_HL(0x791b);
  CYC(0x78ea, 0x78eb); bigBangBombSpawner_addDoubleIndexToHl_from_rst(gb, 0x78eb);
  CYC(0x78eb, 0x78ed); E = 0xcb;
  CYC(0x78ed, 0x78ee); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x78ee, 0x78ef); mem_wr(gb, DE, A);
  CYC(0x78ef, 0x78f1); E = 0xcd;
  CYC(0x78f1, 0x78f2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x78f2, 0x78f3); mem_wr(gb, DE, A);
  CALL_C(0x78f3, objectGetAngleTowardLink_hook, 0x1e9c, 0x78f6);
  CYC(0x78f6, 0x78f8); E = 0xc9;
  CYC(0x78f8, 0x78f9); mem_wr(gb, DE, A);
  CALL_C(0x78f9, getRandomNumber_hook, 0x043e, 0x78fc);
  CYC(0x78fc, 0x78fe); alu_and(gb, 0x0f);
  CYC(0x78fe, 0x7901); SET_HL(0x790b);
  CYC(0x7901, 0x7902); bigBangBombSpawner_addAToHl_from_rst(gb, 0x7902);
  CYC(0x7902, 0x7903); B = mem_rd(gb, HL);
  CYC(0x7903, 0x7905); E = 0xc9;
  CYC(0x7905, 0x7906); A = mem_rd(gb, DE);
  CYC(0x7906, 0x7907); alu_add(gb, B);
  CYC(0x7907, 0x7909); alu_and(gb, 0x1f);
  CYC(0x7909, 0x790a); mem_wr(gb, DE, A);
  RET(0x790a); return; // ret
}

void func_793b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x793b, func_78bd_hook, 0x78bd, 0x793e);
  CYC(0x793e, 0x7940); E = 0xc2; // Part.subid
  CYC(0x7940, 0x7941); A = mem_rd(gb, DE);
  CYC(0x7941, 0x7942); alu_add(gb, A);
  CYC(0x7942, 0x7945); SET_HL(0x7962);
  CYC(0x7945, 0x7946); bigBangBombSpawner_addDoubleIndexToHl_from_rst(gb, 0x7946);
  CYC(0x7946, 0x7948); E = 0xf4;
  CYC(0x7948, 0x7949); A = mem_rd(gb, DE);
  CYC(0x7949, 0x794a); bigBangBombSpawner_addAToHl_from_rst(gb, 0x794a);
  CYC(0x794a, 0x794b); A = mem_rd(gb, HL);
  CYC(0x794b, 0x794e); SET_BC(0x79a2);
  CALL_C(0x794e, addAToBc_hook, 0x006d, 0x7951);
  CYC(0x7951, 0x7952); A = mem_rd(gb, BC);
  CYC(0x7952, 0x7953); H = D;
  CYC(0x7953, 0x7955); L = 0xd0;
  CYC(0x7955, 0x7956); mem_wr(gb, HL, A);
  CYC(0x7956, 0x7958); L = 0xf1;
  CYC(0x7958, 0x795a); mem_wr(gb, HL, 0x20);
  CYC(0x795a, 0x795c); L = 0xd4;
  CYC(0x795c, 0x795e); mem_wr(gb, HL, 0x80);
  CYC(0x795e, 0x795f); L = alu_inc8(gb, L);
  CYC(0x795f, 0x7961); mem_wr(gb, HL, 0xfd);
  RET(0x7961); return; // ret
}

void func_79ab_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x79ab, objectGetShortPosition_hook, 0x2096, 0x79ae);
  CYC(0x79ae, 0x79b1); SET_HL(0xcf00); // wRoomLayout
  CYC(0x79b1, 0x79b2); bigBangBombSpawner_addAToHl_from_rst(gb, 0x79b2);
  CYC(0x79b2, 0x79b3); A = mem_rd(gb, HL);
  CYC(0x79b3, 0x79b5); alu_cp(gb, 0x54);
  if (F & FZ) { CYCT(0x79b5, 0x79b7); func_79c4_hook(gb); return; } // jr z
  CYC(0x79b5, 0x79b7);
  CYC(0x79b7, 0x79b9); alu_cp(gb, 0x55);
  if (F & FZ) { CYCT(0x79b9, 0x79bb); func_79cb_hook(gb); return; } // jr z
  CYC(0x79b9, 0x79bb);
  CYC(0x79bb, 0x79bd); alu_cp(gb, 0x56);
  if (F & FZ) { CYCT(0x79bd, 0x79bf); func_79d2_hook(gb); return; } // jr z
  CYC(0x79bd, 0x79bf);
  CYC(0x79bf, 0x79c1); alu_cp(gb, 0x57);
  if (F & FZ) { CYCT(0x79c1, 0x79c3); func_79d9_hook(gb); return; } // jr z
  CYC(0x79c1, 0x79c3);
  RET(0x79c3); return; // ret
}

void func_79c4_hook(GB *gb) {
  CYC(0x79c4, 0x79c7); SET_HL(0x79e3);
  CYC(0x79c7, 0x79c9); E = 0xca;
  CYC(0x79c9, 0x79cb); // jr $79de
  CYC(0x79de, 0x79e1); add16BitRefs_hook(gb); return; // jp
}

void func_79cb_hook(GB *gb) {
  CYC(0x79cb, 0x79ce); SET_HL(0x79e1);
  CYC(0x79ce, 0x79d0); E = 0xcc;
  CYC(0x79d0, 0x79d2); // jr $79de
  CYC(0x79de, 0x79e1); add16BitRefs_hook(gb); return; // jp
}

void func_79d2_hook(GB *gb) {
  CYC(0x79d2, 0x79d5); SET_HL(0x79e1);
  CYC(0x79d5, 0x79d7); E = 0xca;
  CYC(0x79d7, 0x79d9); // jr $79de
  CYC(0x79de, 0x79e1); add16BitRefs_hook(gb); return; // jp
}

void func_79d9_hook(GB *gb) {
  CYC(0x79d9, 0x79dc); SET_HL(0x79e3);
  CYC(0x79dc, 0x79de); E = 0xcc;
  CYC(0x79de, 0x79e1); add16BitRefs_hook(gb); return; // jp
}
