#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(func_78dd);
  CYC(b_+2, b_+4); E = 0xf4;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  RET(b_+5); return;
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
  BASE(partCode49);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = bigBangBombSpawner_jump_table(gb);
    if (target == b_+16) goto state0;
    if (target == b_+48) goto state1;
    if (target == b_+83) goto state2;
    if (target == b_+109) goto state3;
    if (target == b_+154) goto state4;
    goto state5;
  }

state0:
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = 0xc2;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto func_7754; } // jr nz
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); L = 0xc4;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x05);
  CYC(b_+28, b_+31); func_77f0_hook(gb); return; // jp

func_7754:
  CYC(b_+31, b_+33); L = 0xc4;
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+34, func_78e3_hook, SYM(func_78e3), b_+37);
  CALL_C(b_+37, func_793b_hook, SYM(func_793b), b_+40);
  CYC(b_+40, b_+42); A = 0x98; // SND_POOF
  CALL_C(b_+42, playSound_b00_hook, SYM(playSound_b00), b_+45);
  CALL_C(b_+45, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+48);

state1:
  CALL_C(b_+48, objectApplySpeed_hook, SYM(objectApplySpeed), b_+51);
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+54); L = 0xf1;
  CYC(b_+54, b_+55); C = mem_rd(gb, HL);
  CALL_C(b_+55, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+58);
  if (F & FC) { CYCT(b_+58, b_+60); goto state1_noBounce; } // jr c
  CYC(b_+58, b_+60);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto state1_inAir; } // jr nz
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); E = 0xd0;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+67); A = alu_srl(gb, A);
  CYC(b_+67, b_+68); mem_wr(gb, DE, A);

state1_inAir:
  CYC(b_+68, b_+71); partAnimate_hook(gb); return; // jp

state1_noBounce:
  CYC(b_+71, b_+72); H = D;
  CYC(b_+72, b_+74); L = 0xc4;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x03);
  CYC(b_+76, b_+78); L = 0xc6;
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x14);
  CYC(b_+80, b_+83); partAnimate_hook(gb); return; // jp

state2:
  CYC(b_+83, b_+84); E = alu_inc8(gb, E);
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  {
    CYC(b_+85, b_+86); push_effect(gb, b_+86);
    uint16_t target = bigBangBombSpawner_jump_table(gb);
    if (target == b_+94) goto state2_substate0;
    if (target == b_+104) goto state2_substate3;
    goto state2_substateStub;
  }

state2_substate0:
  CYC(b_+94, b_+95); alu_xor(gb, A);
  CYC(b_+95, b_+98); mem_wr(gb, wLinkGrabState2, A); // wLinkGrabState2
  CYC(b_+98, b_+99); A = alu_inc8(gb, A);
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+103); objectSetVisiblec1_hook(gb); return; // jp

state2_substateStub:
  RET(b_+103); return; // ret

state2_substate3:
  CALL_C(b_+104, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+107);
  CYC(b_+107, b_+109); goto func_77b1; // jr

state3:
  CYC(b_+109, b_+110); H = D;
  CYC(b_+110, b_+112); L = 0xc6;
  CYC(b_+112, b_+113); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+113, b_+115); goto func_77b1; } // jr z
  CYC(b_+113, b_+115);
  CALL_C(b_+115, partAnimate_hook, SYM(partAnimate), b_+118);
  CALL_C(b_+118, func_79ab_hook, SYM(func_79ab), b_+121);
  CYC(b_+121, b_+124); objectAddToGrabbableObjectBuffer_hook(gb); return; // jp

func_77b1:
  CYC(b_+124, b_+125); H = D;
  CYC(b_+125, b_+127); L = 0xc4;
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0x04);
  CYC(b_+129, b_+131); L = 0xe4;
  CYC(b_+131, b_+133); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+133, b_+135); L = 0xdb;
  CYC(b_+135, b_+137); A = 0x0a;
  CYC(b_+137, b_+138); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+138, b_+139); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0x0c);
  CYC(b_+141, b_+143); A = 0x01;
  CALL_C(b_+143, partSetAnimation_hook, SYM(partSetAnimation), b_+146);
  CYC(b_+146, b_+148); A = 0x6f; // SND_EXPLOSION
  CALL_C(b_+148, playSound_b00_hook, SYM(playSound_b00), b_+151);
  CYC(b_+151, b_+154); objectSetVisible83_hook(gb); return; // jp

state4:
  CALL_C(b_+154, partAnimate_hook, SYM(partAnimate), b_+157);
  CYC(b_+157, b_+159); E = 0xe1;
  CYC(b_+159, b_+160); A = mem_rd(gb, DE);
  CYC(b_+160, b_+161); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+161, b_+164); partDelete_hook(gb); return; } // jp z
  CYC(b_+161, b_+164);
  CYC(b_+164, b_+165); A = alu_dec8(gb, A);
  CYC(b_+165, b_+167); E = 0xe6;
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  CYC(b_+168, b_+169); E = alu_inc8(gb, E);
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);
  RET(b_+170); return; // ret

state5:
  CYC(b_+171, b_+172); H = D;
  CYC(b_+172, b_+174); L = 0xf0;
  CYC(b_+174, b_+175); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+175); return; } // ret nz
  CYC(b_+175, b_+176);
  CYC(b_+176, b_+178); L = 0xc6;
  CYC(b_+178, b_+179); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+179, func_77f0_hook, SYM(func_77f0), b_+182);
  if (F & FZ) { CYCT(b_+182, b_+185); partDelete_hook(gb); return; } // jp z
  CYC(b_+182, b_+185);
  CYC(b_+185, b_+187); func_7858_hook(gb); return; // jr
}

void func_77f0_hook(GB *gb) {
  BASE(func_77f0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc6;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+7); SET_BC(SYM(table_780f));
  CALL_C(b_+7, addDoubleIndexToBc_hook, 0x007e, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, BC);
  CYC(b_+11, b_+13); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); func_7805_hook(gb); return; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, wRoomLayoutEnd, A);
  RET(b_+20); return; // ret
}

void func_7805_hook(GB *gb) {
  BASE(func_7805);
  CYC(b_+0, b_+2); L = 0xf0;
  CYC(b_+2, b_+3); mem_wr(gb, HL, A);
  CYC(b_+3, b_+4); SET_BC(BC + 1);
  CYC(b_+4, b_+5); A = mem_rd(gb, BC);
  CYC(b_+5, b_+7); L = 0xf5;
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+9); alu_or(gb, D);
  RET(b_+9); return; // ret
}

void func_7858_hook(GB *gb) {
  BASE(func_7858);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = 0xf2;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CALL_C(b_+6, func_78bd_hook, SYM(func_78bd), b_+9);
  CYC(b_+9, b_+11); E = 0xf5;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
loop:
  CYC(b_+12, b_+14); mem_wr(gb, hFF92, A);
  CALL_C(b_+14, func_786f_hook, SYM(func_786f), b_+17);
  CYC(b_+17, b_+19); A = mem_rd(gb, hFF92);
  CYC(b_+19, b_+20); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto loop; } // jr nz
  CYC(b_+20, b_+22);
  RET(b_+22); return; // ret
}

void func_786f_hook(GB *gb) {
  BASE(func_786f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
restart:
  CYC(b_+0, b_+2); E = 0xf4;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+8); SET_BC(SYM(table_789d));
  CALL_C(b_+8, addDoubleIndexToBc_hook, 0x007e, b_+11);
  CALL_C(b_+11, getRandomNumber_hook, SYM(getRandomNumber), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x07);
  CALL_C(b_+16, addAToBc_hook, 0x006d, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, BC);
  CYC(b_+20, b_+22); mem_wr(gb, hFF8B, A);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = 0xf2;
  CALL_C(b_+25, checkFlag_hook, SYM(checkFlag), b_+28);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto restart; } // jr nz
  CYC(b_+28, b_+30);
  CALL_C(b_+30, getFreePartSlot_hook, SYM(getFreePartSlot), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x49); // PART_BIGBANG_BOMB_SPAWNER
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+39); A = mem_rd(gb, hFF8B);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); L = 0xf2;
  CYC(b_+43, b_+46); setFlag_hook(gb); return; // jp
}

void func_78bd_hook(GB *gb) {
  BASE(func_78bd);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_xh); // w1Link.xh
  CYC(b_+3, b_+5); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+5, b_+7); func_78d2_hook(gb); return; } // jr nc
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, w1Link_yh); // w1Link.yh
  CYC(b_+10, b_+12); alu_cp(gb, 0x40);
  if (!(F & FC)) { CYCT(b_+12, b_+14); func_78ce_hook(gb); return; } // jr nc
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+17); bbbs_setHazardAndReturn(gb); return; // jr
}

void func_78ce_hook(GB *gb) {
  BASE(func_78ce);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4); bbbs_setHazardAndReturn(gb); return; // jr
}

void func_78d2_hook(GB *gb) {
  BASE(func_78d2);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh); // w1Link.yh
  CYC(b_+3, b_+5); alu_cp(gb, 0x40);
  if (!(F & FC)) { CYCT(b_+5, b_+7); func_78dd_hook(gb); return; } // jr nc
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x02;
  CYC(b_+9, b_+11); bbbs_setHazardAndReturn(gb); return; // jr
}

void func_78dd_hook(GB *gb) {
  BASE(func_78dd);
  CYC(b_+0, b_+2); A = 0x03;
  bbbs_setHazardAndReturn(gb); return;
}

void func_78e3_hook(GB *gb) {
  BASE(func_78e3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc2;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+7); SET_HL(SYM(table_791b));
  CYC(b_+7, b_+8); bigBangBombSpawner_addDoubleIndexToHl_from_rst(gb, b_+8);
  CYC(b_+8, b_+10); E = 0xcb;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = 0xcd;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CALL_C(b_+16, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+19);
  CYC(b_+19, b_+21); E = 0xc9;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, getRandomNumber_hook, SYM(getRandomNumber), b_+25);
  CYC(b_+25, b_+27); alu_and(gb, 0x0f);
  CYC(b_+27, b_+30); SET_HL(SYM(table_790b));
  CYC(b_+30, b_+31); bigBangBombSpawner_addAToHl_from_rst(gb, b_+31);
  CYC(b_+31, b_+32); B = mem_rd(gb, HL);
  CYC(b_+32, b_+34); E = 0xc9;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_add(gb, B);
  CYC(b_+36, b_+38); alu_and(gb, 0x1f);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  RET(b_+39); return; // ret
}

void func_793b_hook(GB *gb) {
  BASE(func_793b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, func_78bd_hook, SYM(func_78bd), b_+3);
  CYC(b_+3, b_+5); E = 0xc2; // Part.subid
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_add(gb, A);
  CYC(b_+7, b_+10); SET_HL(SYM(table_7962));
  CYC(b_+10, b_+11); bigBangBombSpawner_addDoubleIndexToHl_from_rst(gb, b_+11);
  CYC(b_+11, b_+13); E = 0xf4;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); bigBangBombSpawner_addAToHl_from_rst(gb, b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+19); SET_BC(SYM(table_79a2));
  CALL_C(b_+19, addAToBc_hook, 0x006d, b_+22);
  CYC(b_+22, b_+23); A = mem_rd(gb, BC);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = 0xd0;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = 0xf1;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x20);
  CYC(b_+31, b_+33); L = 0xd4;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x80);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0xfd);
  RET(b_+38); return; // ret
}

void func_79ab_hook(GB *gb) {
  BASE(func_79ab);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+3);
  CYC(b_+3, b_+6); SET_HL(wRoomLayout); // wRoomLayout
  CYC(b_+6, b_+7); bigBangBombSpawner_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0x54);
  if (F & FZ) { CYCT(b_+10, b_+12); func_79c4_hook(gb); return; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); alu_cp(gb, 0x55);
  if (F & FZ) { CYCT(b_+14, b_+16); func_79cb_hook(gb); return; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); alu_cp(gb, 0x56);
  if (F & FZ) { CYCT(b_+18, b_+20); func_79d2_hook(gb); return; } // jr z
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_cp(gb, 0x57);
  if (F & FZ) { CYCT(b_+22, b_+24); func_79d9_hook(gb); return; } // jr z
  CYC(b_+22, b_+24);
  RET(b_+24); return; // ret
}

void func_79c4_hook(GB *gb) {
  BASE(func_79c4);
  CYC(b_+0, b_+3); SET_HL(SYM(table_79e3));
  CYC(b_+3, b_+5); E = 0xca;
  CYC(b_+5, b_+7); // jr $79de
  CYC((SYM(func_79d9) + 5), (SYM(func_79d9) + 8)); add16BitRefs_hook(gb); return; // jp
}

void func_79cb_hook(GB *gb) {
  BASE(func_79cb);
  CYC(b_+0, b_+3); SET_HL(SYM(table_79e1));
  CYC(b_+3, b_+5); E = 0xcc;
  CYC(b_+5, b_+7); // jr $79de
  CYC((SYM(func_79d9) + 5), (SYM(func_79d9) + 8)); add16BitRefs_hook(gb); return; // jp
}

void func_79d2_hook(GB *gb) {
  BASE(func_79d2);
  CYC(b_+0, b_+3); SET_HL(SYM(table_79e1));
  CYC(b_+3, b_+5); E = 0xca;
  CYC(b_+5, b_+7); // jr $79de
  CYC((SYM(func_79d9) + 5), (SYM(func_79d9) + 8)); add16BitRefs_hook(gb); return; // jp
}

void func_79d9_hook(GB *gb) {
  BASE(func_79d9);
  CYC(b_+0, b_+3); SET_HL(SYM(table_79e3));
  CYC(b_+3, b_+5); E = 0xcc;
  CYC(b_+5, b_+8); add16BitRefs_hook(gb); return; // jp
}
