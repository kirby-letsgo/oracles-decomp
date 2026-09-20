#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(func_56b6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc9; // Part.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0x10);
  CYC(b_+9, b_+11); A = 0x00;
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto L_56c4; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = alu_inc8(gb, A);

L_56c4:
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = 0xc8;
  CYC(b_+17, b_+18); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+18); return; } // ret z
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+23); partSetAnimation_hook(gb); return; // jp
}

void func_56cd_hook(GB *gb) {
  BASE(func_56cd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc9; // Part.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(b_+6, b_+8); goto L_56d9; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x18);
  CYC(b_+10, b_+12); alu_add(gb, 0x04);

L_56d9:
  CYC(b_+12, b_+14); alu_and(gb, 0x1c);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+18); SET_HL(SYM(table_56f5)); // table_56f5
  CYC(b_+18, b_+19); smallFairy_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+21); E = 0xcb; // Part.yh
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); B = A;
  CYC(b_+24, b_+26); E = 0xcd; // Part.xh
  CYC(b_+26, b_+27); SET_HL(HL + 1);
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+31); alu_sub(gb, 0x38);
  CYC(b_+31, b_+33); alu_cp(gb, 0x80);
  if (!(F & FC)) { RET_TAKEN(b_+33); return; } // ret nc
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+37); alu_sub(gb, 0x18);
  CYC(b_+37, b_+39); alu_cp(gb, 0x50);
  RET(b_+39); return; // ret
}

void partCode28_hook(GB *gb) {
  BASE(partCode28);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+4, b_+7); goto collected; } // jp z
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); E = 0xc4; // Part.state
  CYC(b_+9, b_+11); A = 0x02;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);

normalStatus:
  CYC(b_+12, b_+14); E = 0xc4; // Part.state
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  {
    CYC(b_+15, b_+16); push_effect(gb, b_+16);
    uint16_t target = smallFairy_jump_table(gb);
    if (target == b_+22) goto state0;
    if (target == b_+43) goto state1;
    goto state2;
  }

state0:
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = 0xc4; // Part.state
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+28); L = 0xcf; // Part.zh
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0xfa);
  CYC(b_+30, b_+32); L = 0xf1; // Part.var31
  CYC(b_+32, b_+34); E = 0xcb; // Part.yh
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+36, b_+38); E = 0xcd; // Part.xh
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+43); objectSetVisiblec2_hook(gb); return; // jp

state1:
  CALL_C(b_+43, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+46);
  if (F & FZ) { CYCT(b_+46, b_+48); goto L_5643; } // jr z
  CYC(b_+46, b_+48);
  CALL_C(b_+48, func_56cd_hook, SYM(func_56cd), b_+51);
  if (F & FC) { CYCT(b_+51, b_+54); objectApplySpeed_hook(gb); return; } // jp c
  CYC(b_+51, b_+54);

L_5643:
  CALL_C(b_+54, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+57);
  CYC(b_+57, b_+59); alu_and(gb, 0x3e);
  CYC(b_+59, b_+61); alu_add(gb, 0x08);
  CYC(b_+61, b_+63); E = 0xc6; // Part.counter1
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CALL_C(b_+64, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+67);
  CYC(b_+67, b_+69); alu_and(gb, 0x03);
  CYC(b_+69, b_+72); SET_HL(b_+89); // table_5666
  CYC(b_+72, b_+73); smallFairy_addAToHl_from_rst(gb, b_+73);
  CYC(b_+73, b_+75); E = 0xd0; // Part.speed
  CYC(b_+75, b_+76); A = mem_rd(gb, HL);
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  CALL_C(b_+77, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+80);
  CYC(b_+80, b_+82); alu_and(gb, 0x1e);
  CYC(b_+82, b_+83); H = D;
  CYC(b_+83, b_+85); L = 0xc9; // Part.angle
  CYC(b_+85, b_+86); mem_wr(gb, HL, A);
  CYC(b_+86, b_+89); func_56b6_hook(gb); return; // jp

state2:
  CYC(b_+93, b_+95); E = 0xc5; // Part.substate
  CYC(b_+95, b_+96); A = mem_rd(gb, DE);
  CYC(b_+96, b_+97); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+97, b_+99); goto L_5683; } // jr nz
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+100); H = D;
  CYC(b_+100, b_+101); L = E;
  CYC(b_+101, b_+102); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+102, b_+104); L = 0xcf; // Part.zh
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x00);
  CYC(b_+106, b_+108); A = 0x01;
  CALL_C(b_+108, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+111);
  CYC(b_+111, b_+112); A = mem_rd(gb, HL);
  CYC(b_+112, b_+114); E = 0xf0; // Part.var30
  CYC(b_+114, b_+115); mem_wr(gb, DE, A);
  CALL_C(b_+115, objectSetVisible80_hook, SYM(objectSetVisible80), b_+118);

L_5683:
  CALL_C(b_+118, objectCheckCollidedWithLink_hook, SYM(objectCheckCollidedWithLink), b_+121);
  if (F & FC) { CYCT(b_+121, b_+124); goto collected; } // jp c
  CYC(b_+121, b_+124);
  CYC(b_+124, b_+126); A = 0x00;
  CALL_C(b_+126, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+129);
  CYC(b_+129, b_+130); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+130, b_+131); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+131, b_+133); goto L_5699; } // jr z
  CYC(b_+131, b_+133);
  CYC(b_+133, b_+135); E = 0xf0; // Part.var30
  CYC(b_+135, b_+136); A = mem_rd(gb, DE);
  CYC(b_+136, b_+137); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+137, b_+140); objectTakePosition_hook(gb); return; } // jp z
  CYC(b_+137, b_+140);

L_5699:
  CYC(b_+140, b_+143); partDelete_hook(gb); return; // jp

collected:
  CYC(b_+143, b_+145); A = 0x26;
  CALL_C(b_+145, cpActiveRing_hook, SYM(cpActiveRing), b_+148);
  CYC(b_+148, b_+150); C = 0x18;
  if (F & FZ) { CYCT(b_+150, b_+152); goto L_56ac; } // jr z
  CYC(b_+150, b_+152);
  CYC(b_+152, b_+154); A = 0x25;
  CALL_C(b_+154, cpActiveRing_hook, SYM(cpActiveRing), b_+157);
  if (!(F & FZ)) { CYCT(b_+157, b_+159); goto L_56ae; } // jr nz
  CYC(b_+157, b_+159);

L_56ac:
  CYC(b_+159, b_+161); C = 0x30;

L_56ae:
  CYC(b_+161, b_+163); A = 0x29;
  CALL_C(b_+163, giveTreasure_hook, SYM(giveTreasure), b_+166);
  CYC(b_+166, b_+169); partDelete_hook(gb); return; // jp
}
