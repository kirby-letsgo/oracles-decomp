#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t ball_jump_table(GB *gb) {
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

static void ball_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

/* func_6b00 is a true local (no independent hook row), always called with a genuine return
   expected, and it never dispatches into another hook -- a plain helper with no push/ret_effect
   is correct and safe here, matching the state0func_6731 precedent. */
static void ball_func_6b00(GB *gb) {
  BASE(partCode38);
  CYC(b_+122, b_+123); alu_scf(gb);
  CYC(b_+123, b_+124); push_effect(gb, (uint16_t)((A << 8) | F));
  CYC(b_+124, b_+125); A = mem_rd(gb, HL);
  CYC(b_+125, b_+127); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(b_+127, b_+129); goto plus; } // jr z
  CYC(b_+127, b_+129);
  CYC(b_+129, b_+130);
  {
    uint16_t af = pop_effect(gb);
    A = (uint8_t)(af >> 8); F = (uint8_t)(af & 0xff);
  }
  CYC(b_+130, b_+131); alu_ccf(gb);
  CYC(b_+131, b_+132);
  return; // ret
plus:
  CYC(b_+132, b_+133);
  {
    uint16_t af = pop_effect(gb);
    A = (uint8_t)(af >> 8); F = (uint8_t)(af & 0xff);
  }
  CYC(b_+133, b_+134);
  return; // ret
}

void partCode38_hook(GB *gb);
void func_6b5f_hook(GB *gb);
void func_6b65_hook(GB *gb);
void func_6b71_hook(GB *gb);
void func_6b77_hook(GB *gb);
void func_6b9f_hook(GB *gb);
void func_6baf_hook(GB *gb);
void func_6bca_hook(GB *gb);
void func_6bf6_hook(GB *gb);
void func_6c02_hook(GB *gb);
void func_6c08_hook(GB *gb);
void func_6c0e_hook(GB *gb);
void func_6c17_hook(GB *gb);
void func_6c27_hook(GB *gb);

void partCode38_hook(GB *gb) {
  BASE(partCode38);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(b_+7, b_+10); partDelete_hook(gb); return; } // jp z
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = 0xc4; // Part.state
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto normalStatus; } // jr nc
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x02);

normalStatus:
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = 0xc6; // Part.counter1
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+25, b_+27); goto L_6aa3; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+28); return; // ret

L_6aa3:
  CYC(b_+29, b_+31); E = 0xc4; // Part.state
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  {
    CYC(b_+32, b_+33); push_effect(gb, b_+33);
    uint16_t target = ball_jump_table(gb);
    if (target == b_+41) goto state0;
    if (target == b_+97) goto state1;
    if (target == b_+134) goto state2;
    goto state3;
  }

state0:
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+43); L = E; // Part.state
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+44, b_+46); L = 0xc9; // Part.angle
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x10);
  CALL_C(b_+48, objectSetVisible81_hook, SYM(objectSetVisible81), b_+51);
  CALL_C(b_+51, getRandomNumber_hook, SYM(getRandomNumber), b_+54);
  CYC(b_+54, b_+56); alu_and(gb, 0x0f);
  CYC(b_+56, b_+59); SET_HL(b_+81); // @table_6ad7
  CYC(b_+59, b_+60); ball_addAToHl_from_rst(gb, b_+60);
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+62); H = D;
  CYC(b_+62, b_+64); L = 0xd0; // Part.speed
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto func_6ad0; } // jr nz
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x64);
  CYC(b_+69, b_+71); A = 0x51; // SND_THROW
  CYC(b_+71, b_+74); TAIL(playSound_b00); // jp

func_6ad0:
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x3c);
  CYC(b_+76, b_+78); A = 0x59; // SND_FALLINHOLE
  CYC(b_+78, b_+81); TAIL(playSound_b00); // jp

state1:
  CALL_C(b_+97, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+100);
  if (!(F & FC)) { CYCT(b_+100, b_+103); func_6c17_hook(gb); return; } // jp nc
  CYC(b_+100, b_+103);
  CALL_C(b_+103, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+106);
  if (!(F & FC)) { CYCT(b_+106, b_+108); goto objectApplySpeedTail; } // jr nc
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+111); ball_func_6b00(gb);
  if (!(F & FC)) { CYCT(b_+111, b_+113); goto objectApplySpeedTail; } // jr nc
  CYC(b_+111, b_+113);
  if (F & FZ) { CYCT(b_+113, b_+116); func_6c17_hook(gb); return; } // jp z
  CYC(b_+113, b_+116);
  CYC(b_+116, b_+119); TAIL(func_6bf6); // jp

objectApplySpeedTail:
  CYC(b_+119, b_+122); TAIL(objectApplySpeed); // jp

state2:
  CYC(b_+134, b_+136); A = 0x03;
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  CYC(b_+137, b_+139); A = 0x50; // SND_CLINK
  CALL_C(b_+139, playSound_b00_hook, SYM(playSound_b00), b_+142);
  CYC(b_+142, b_+143); H = D;
  CYC(b_+143, b_+145); L = 0xc6; // Part.counter1
  CYC(b_+145, b_+147); mem_wr(gb, HL, 0x00);
  CYC(b_+147, b_+149); L = 0xd0; // Part.speed
  CYC(b_+149, b_+151); mem_wr(gb, HL, 0x78);
  CYC(b_+151, b_+153); L = 0xec;
  CYC(b_+153, b_+154); A = mem_rd(gb, HL);
  CYC(b_+154, b_+156); L = 0xc9; // Part.angle
  CYC(b_+156, b_+157); mem_wr(gb, HL, A);
  RET(b_+157); return; // ret

state3:
  CALL_C(b_+158, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+161);
  if (!(F & FC)) { CYCT(b_+161, b_+164); func_6c17_hook(gb); return; } // jp nc
  CYC(b_+161, b_+164);
  CYC(b_+164, b_+166); B = 0xff;
  CALL_C(b_+166, func_6b5f_hook, SYM(func_6b5f), b_+169);
  CALL_C(b_+169, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+172);
  if (!(F & FC)) { CYCT(b_+172, b_+174); goto L_6b3f; } // jr nc
  CYC(b_+172, b_+174);
  CYC(b_+174, b_+177); ball_func_6b00(gb);
  if (!(F & FC)) { CYCT(b_+177, b_+179); goto L_6b3f; } // jr nc
  CYC(b_+177, b_+179);
  if (F & FZ) { CYCT(b_+179, b_+182); func_6c17_hook(gb); return; } // jp z
  CYC(b_+179, b_+182);
  CALL_C(b_+182, func_6c02_hook, SYM(func_6c02), b_+185);
L_6b3f:
  CYC(b_+185, b_+187); B = 0x02;
  CALL_C(b_+187, func_6b5f_hook, SYM(func_6b5f), b_+190);
  CALL_C(b_+190, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+193);
  if (!(F & FC)) { CYCT(b_+193, b_+195); goto L_6b54; } // jr nc
  CYC(b_+193, b_+195);
  CYC(b_+195, b_+198); ball_func_6b00(gb);
  if (!(F & FC)) { CYCT(b_+198, b_+200); goto L_6b54; } // jr nc
  CYC(b_+198, b_+200);
  if (F & FZ) { CYCT(b_+200, b_+203); func_6c17_hook(gb); return; } // jp z
  CYC(b_+200, b_+203);
  CALL_C(b_+203, func_6c08_hook, SYM(func_6c08), b_+206);
L_6b54:
  CYC(b_+206, b_+208); B = 0xff;
  CALL_C(b_+208, func_6b5f_hook, SYM(func_6b5f), b_+211);
  CALL_C(b_+211, partAnimate_hook, SYM(partAnimate), b_+214);
  CYC(b_+214, b_+217); TAIL(objectApplySpeed); // jp
}

void func_6b5f_hook(GB *gb) {
  BASE(func_6b5f);
  CYC(b_+0, b_+2); E = 0xcd; // Part.xh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, B);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  RET(b_+5); return; // ret
}

void func_6b65_hook(GB *gb) {
  BASE(func_6b65);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+4); A = L;
  CYC(b_+4, b_+6); mem_wr(gb, hFF8C, A);
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CALL_C(b_+7, func_6b71_hook, SYM(func_6b71), b_+10);
  CYC(b_+10, b_+12); TAIL(func_6bca); // jr
}

void func_6b71_hook(GB *gb) {
  BASE(func_6b71);
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); mem_wr(gb, wTmpcfc0_genericCutscene_cfd5, A);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  TAIL(func_6b77);// falls through
}

void func_6b77_hook(GB *gb) {
  BASE(func_6b77);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, hFF8B, A);
  CYC(b_+2, b_+5); SET_HL(SYM(table_6bab)); // table_6bab
  CYC(b_+5, b_+6); ball_addAToHl_from_rst(gb, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); func_6b9f_hook(gb); return; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wShootingGalleryBallStatus);
  CYC(b_+13, b_+15); alu_and(gb, 0x7f);
  CYC(b_+15, b_+17); alu_cp(gb, 0x01);
  CYC(b_+17, b_+19); A = mem_rd(gb, hFF8B);
  CYC(b_+19, b_+22); mem_wr(gb, wTmpcfc0_genericCutscene_cfd5, A);
  if (F & FZ) { CYCT(b_+22, b_+24); goto L_6b91; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); alu_add(gb, 0x04);
L_6b91:
  CYC(b_+26, b_+29); SET_HL(0x00f8);
  CYC(b_+29, b_+30); alu_add(gb, L);
  CYC(b_+30, b_+31); L = A;
  CYC(b_+31, b_+34); A = mem_rd(gb, wLinkPushingAgainstBedCounter);
  CYC(b_+34, b_+35); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+35, b_+38); mem_wr(gb, wLinkPushingAgainstBedCounter, A);
  CYC(b_+38, b_+40); TAIL(func_6baf); // jr
}

void func_6b9f_hook(GB *gb) {
  BASE(func_6b9f);
  CYC(b_+0, b_+2); A = mem_rd(gb, hFF8B);
  CYC(b_+2, b_+3); A = alu_inc8(gb, A);
  CYC(b_+3, b_+5); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); func_6b77_hook(gb); return; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(wShootingGalleryBallStatus);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+11); return; // ret
}

void func_6baf_hook(GB *gb) {
  BASE(func_6baf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+3);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); A = 0xa0;
  CALL_C(b_+6, setTile_hook, SYM(setTile), b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0xc6; // Part.counter1
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x03);
  CYC(b_+14, b_+17); A = mem_rd(gb, wShootingGalleryBallStatus);
  CYC(b_+17, b_+19); alu_and(gb, 0x7f);
  CYC(b_+19, b_+21); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; } // ret nz
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); A = 0x7e; // SND_SWITCH
  CYC(b_+24, b_+27); TAIL(playSound_b00); // jp
}

void func_6bca_hook(GB *gb) {
  BASE(func_6bca);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd5);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x04;
L_6bd2:
  CYC(b_+8, b_+10); mem_wr(gb, hFF8B, A);
  CYC(b_+10, b_+13); SET_BC(0x9204); // INTERAC_FALLING_ROCK, $04
  CYC(b_+13, b_+16); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd5);
  CYC(b_+16, b_+18); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+18, b_+20); goto plus1; } // jr c
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+23); SET_BC(0x9205); // INTERAC_FALLING_ROCK, $05
plus1:
  CALL_C(b_+23, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+26);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto plus2; } // jr nz
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); L = 0x4b;
  CYC(b_+30, b_+32); A = mem_rd(gb, hFF8C);
  CALL_C(b_+32, setShortPosition_hook, SYM(setShortPosition), b_+35);
  CYC(b_+35, b_+37); L = 0x49;
  CYC(b_+37, b_+39); A = mem_rd(gb, hFF8B);
  CYC(b_+39, b_+40); A = alu_dec8(gb, A);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto L_6bd2; } // jr nz
  CYC(b_+41, b_+43);
plus2:
  RET(b_+43); return; // ret
}

void func_6bf6_hook(GB *gb) {
  BASE(func_6bf6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xa6; // SND_STRIKE
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+10); mem_wr(gb, wTmpcfc0_bigBangGame_prizeIndex, A);
  CYC(b_+10, b_+12); TAIL(func_6c27); // jr
}

void func_6c02_hook(GB *gb) {
  BASE(func_6c02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, func_6c0e_hook, SYM(func_6c0e), b_+3);
  CYC(b_+3, b_+6); TAIL(func_6b65); // jp
}

void func_6c08_hook(GB *gb) {
  BASE(func_6c08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, func_6c0e_hook, SYM(func_6c0e), b_+3);
  CYC(b_+3, b_+6); TAIL(func_6b65); // jp
}

void func_6c0e_hook(GB *gb) {
  BASE(func_6c0e);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0_bigBangGame_prizeIndex, A);
  CYC(b_+4, b_+7); SET_HL(wShootingGalleryBallStatus);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+8); return; // ret
}

void func_6c17_hook(GB *gb) {
  BASE(func_6c17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0_bigBangGame_prizeIndex, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wShootingGalleryBallStatus);
  CYC(b_+7, b_+9); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); func_6c27_hook(gb); return; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x5a; // SND_ERROR
  CALL_C(b_+13, playSound_b00_hook, SYM(playSound_b00), SYM(func_6c27));
  TAIL(func_6c27);// falls through
}

void func_6c27_hook(GB *gb) {
  BASE(func_6c27);
  CYC(b_+0, b_+3); SET_HL(wShootingGalleryBallStatus);
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+5, b_+8); TAIL(partDelete); // jp
}
