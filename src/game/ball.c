#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  CYC(0x6b00, 0x6b01); alu_scf(gb);
  CYC(0x6b01, 0x6b02); push_effect(gb, (uint16_t)((A << 8) | F));
  CYC(0x6b02, 0x6b03); A = mem_rd(gb, HL);
  CYC(0x6b03, 0x6b05); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(0x6b05, 0x6b07); goto plus; } // jr z
  CYC(0x6b05, 0x6b07);
  CYC(0x6b07, 0x6b08);
  {
    uint16_t af = pop_effect(gb);
    A = (uint8_t)(af >> 8); F = (uint8_t)(af & 0xff);
  }
  CYC(0x6b08, 0x6b09); alu_ccf(gb);
  CYC(0x6b09, 0x6b0a);
  return; // ret
plus:
  CYC(0x6b0a, 0x6b0b);
  {
    uint16_t af = pop_effect(gb);
    A = (uint8_t)(af >> 8); F = (uint8_t)(af & 0xff);
  }
  CYC(0x6b0b, 0x6b0c);
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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x6a86, 0x6a88); goto normalStatus; } // jr z
  CYC(0x6a86, 0x6a88);
  CYC(0x6a88, 0x6a8a); E = 0xea; // Part.var2a
  CYC(0x6a8a, 0x6a8b); A = mem_rd(gb, DE);
  CYC(0x6a8b, 0x6a8d); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(0x6a8d, 0x6a90); partDelete_hook(gb); return; } // jp z
  CYC(0x6a8d, 0x6a90);
  CYC(0x6a90, 0x6a91); H = D;
  CYC(0x6a91, 0x6a93); L = 0xc4; // Part.state
  CYC(0x6a93, 0x6a94); A = mem_rd(gb, HL);
  CYC(0x6a94, 0x6a96); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x6a96, 0x6a98); goto normalStatus; } // jr nc
  CYC(0x6a96, 0x6a98);
  CYC(0x6a98, 0x6a9a); mem_wr(gb, HL, 0x02);

normalStatus:
  CYC(0x6a9a, 0x6a9b); H = D;
  CYC(0x6a9b, 0x6a9d); L = 0xc6; // Part.counter1
  CYC(0x6a9d, 0x6a9e); A = mem_rd(gb, HL);
  CYC(0x6a9e, 0x6a9f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6a9f, 0x6aa1); goto L_6aa3; } // jr z
  CYC(0x6a9f, 0x6aa1);
  CYC(0x6aa1, 0x6aa2); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x6aa2); return; // ret

L_6aa3:
  CYC(0x6aa3, 0x6aa5); E = 0xc4; // Part.state
  CYC(0x6aa5, 0x6aa6); A = mem_rd(gb, DE);
  {
    CYC(0x6aa6, 0x6aa7); push_effect(gb, 0x6aa7);
    uint16_t target = ball_jump_table(gb);
    if (target == 0x6aaf) goto state0;
    if (target == 0x6ae7) goto state1;
    if (target == 0x6b0c) goto state2;
    goto state3;
  }

state0:
  CYC(0x6aaf, 0x6ab0); H = D;
  CYC(0x6ab0, 0x6ab1); L = E; // Part.state
  CYC(0x6ab1, 0x6ab2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ab2, 0x6ab4); L = 0xc9; // Part.angle
  CYC(0x6ab4, 0x6ab6); mem_wr(gb, HL, 0x10);
  CALL_C(0x6ab6, objectSetVisible81_hook, 0x1e60, 0x6ab9);
  CALL_C(0x6ab9, getRandomNumber_hook, 0x043e, 0x6abc);
  CYC(0x6abc, 0x6abe); alu_and(gb, 0x0f);
  CYC(0x6abe, 0x6ac1); SET_HL(0x6ad7); // @table_6ad7
  CYC(0x6ac1, 0x6ac2); ball_addAToHl_from_rst(gb, 0x6ac2);
  CYC(0x6ac2, 0x6ac3); A = mem_rd(gb, HL);
  CYC(0x6ac3, 0x6ac4); H = D;
  CYC(0x6ac4, 0x6ac6); L = 0xd0; // Part.speed
  CYC(0x6ac6, 0x6ac7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6ac7, 0x6ac9); goto func_6ad0; } // jr nz
  CYC(0x6ac7, 0x6ac9);
  CYC(0x6ac9, 0x6acb); mem_wr(gb, HL, 0x64);
  CYC(0x6acb, 0x6acd); A = 0x51; // SND_THROW
  CYC(0x6acd, 0x6ad0); playSound_b00_hook(gb); return; // jp

func_6ad0:
  CYC(0x6ad0, 0x6ad2); mem_wr(gb, HL, 0x3c);
  CYC(0x6ad2, 0x6ad4); A = 0x59; // SND_FALLINHOLE
  CYC(0x6ad4, 0x6ad7); playSound_b00_hook(gb); return; // jp

state1:
  CALL_C(0x6ae7, objectCheckWithinScreenBoundary_hook, 0x2184, 0x6aea);
  if (!(F & FC)) { CYCT(0x6aea, 0x6aed); func_6c17_hook(gb); return; } // jp nc
  CYC(0x6aea, 0x6aed);
  CALL_C(0x6aed, partCommon_checkTileCollisionOrOutOfBounds, 0x4072, 0x6af0);
  if (!(F & FC)) { CYCT(0x6af0, 0x6af2); goto objectApplySpeedTail; } // jr nc
  CYC(0x6af0, 0x6af2);
  CYC(0x6af2, 0x6af5); ball_func_6b00(gb);
  if (!(F & FC)) { CYCT(0x6af5, 0x6af7); goto objectApplySpeedTail; } // jr nc
  CYC(0x6af5, 0x6af7);
  if (F & FZ) { CYCT(0x6af7, 0x6afa); func_6c17_hook(gb); return; } // jp z
  CYC(0x6af7, 0x6afa);
  CYC(0x6afa, 0x6afd); func_6bf6_hook(gb); return; // jp

objectApplySpeedTail:
  CYC(0x6afd, 0x6b00); objectApplySpeed_hook(gb); return; // jp

state2:
  CYC(0x6b0c, 0x6b0e); A = 0x03;
  CYC(0x6b0e, 0x6b0f); mem_wr(gb, DE, A);
  CYC(0x6b0f, 0x6b11); A = 0x50; // SND_CLINK
  CALL_C(0x6b11, playSound_b00_hook, 0x0c98, 0x6b14);
  CYC(0x6b14, 0x6b15); H = D;
  CYC(0x6b15, 0x6b17); L = 0xc6; // Part.counter1
  CYC(0x6b17, 0x6b19); mem_wr(gb, HL, 0x00);
  CYC(0x6b19, 0x6b1b); L = 0xd0; // Part.speed
  CYC(0x6b1b, 0x6b1d); mem_wr(gb, HL, 0x78);
  CYC(0x6b1d, 0x6b1f); L = 0xec;
  CYC(0x6b1f, 0x6b20); A = mem_rd(gb, HL);
  CYC(0x6b20, 0x6b22); L = 0xc9; // Part.angle
  CYC(0x6b22, 0x6b23); mem_wr(gb, HL, A);
  RET(0x6b23); return; // ret

state3:
  CALL_C(0x6b24, objectCheckWithinScreenBoundary_hook, 0x2184, 0x6b27);
  if (!(F & FC)) { CYCT(0x6b27, 0x6b2a); func_6c17_hook(gb); return; } // jp nc
  CYC(0x6b27, 0x6b2a);
  CYC(0x6b2a, 0x6b2c); B = 0xff;
  CALL_C(0x6b2c, func_6b5f_hook, 0x6b5f, 0x6b2f);
  CALL_C(0x6b2f, partCommon_checkTileCollisionOrOutOfBounds, 0x4072, 0x6b32);
  if (!(F & FC)) { CYCT(0x6b32, 0x6b34); goto L_6b3f; } // jr nc
  CYC(0x6b32, 0x6b34);
  CYC(0x6b34, 0x6b37); ball_func_6b00(gb);
  if (!(F & FC)) { CYCT(0x6b37, 0x6b39); goto L_6b3f; } // jr nc
  CYC(0x6b37, 0x6b39);
  if (F & FZ) { CYCT(0x6b39, 0x6b3c); func_6c17_hook(gb); return; } // jp z
  CYC(0x6b39, 0x6b3c);
  CALL_C(0x6b3c, func_6c02_hook, 0x6c02, 0x6b3f);
L_6b3f:
  CYC(0x6b3f, 0x6b41); B = 0x02;
  CALL_C(0x6b41, func_6b5f_hook, 0x6b5f, 0x6b44);
  CALL_C(0x6b44, partCommon_checkTileCollisionOrOutOfBounds, 0x4072, 0x6b47);
  if (!(F & FC)) { CYCT(0x6b47, 0x6b49); goto L_6b54; } // jr nc
  CYC(0x6b47, 0x6b49);
  CYC(0x6b49, 0x6b4c); ball_func_6b00(gb);
  if (!(F & FC)) { CYCT(0x6b4c, 0x6b4e); goto L_6b54; } // jr nc
  CYC(0x6b4c, 0x6b4e);
  if (F & FZ) { CYCT(0x6b4e, 0x6b51); func_6c17_hook(gb); return; } // jp z
  CYC(0x6b4e, 0x6b51);
  CALL_C(0x6b51, func_6c08_hook, 0x6c08, 0x6b54);
L_6b54:
  CYC(0x6b54, 0x6b56); B = 0xff;
  CALL_C(0x6b56, func_6b5f_hook, 0x6b5f, 0x6b59);
  CALL_C(0x6b59, partAnimate_hook, 0x2978, 0x6b5c);
  CYC(0x6b5c, 0x6b5f); objectApplySpeed_hook(gb); return; // jp
}

void func_6b5f_hook(GB *gb) {
  CYC(0x6b5f, 0x6b61); E = 0xcd; // Part.xh
  CYC(0x6b61, 0x6b62); A = mem_rd(gb, DE);
  CYC(0x6b62, 0x6b63); alu_add(gb, B);
  CYC(0x6b63, 0x6b64); mem_wr(gb, DE, A);
  RET(0x6b64); return; // ret
}

void func_6b65_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6b65, objectGetTileAtPosition_hook, 0x1444, 0x6b68);
  CYC(0x6b68, 0x6b69); A = L;
  CYC(0x6b69, 0x6b6b); hram_wr(gb, 0x8c, A);
  CYC(0x6b6b, 0x6b6c); C = mem_rd(gb, HL);
  CALL_C(0x6b6c, func_6b71_hook, 0x6b71, 0x6b6f);
  CYC(0x6b6f, 0x6b71); func_6bca_hook(gb); return; // jr
}

void func_6b71_hook(GB *gb) {
  CYC(0x6b71, 0x6b73); A = 0xff;
  CYC(0x6b73, 0x6b76); mem_wr(gb, 0xcfd5, A);
  CYC(0x6b76, 0x6b77); alu_xor(gb, A);
  func_6b77_hook(gb); // falls through
}

void func_6b77_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b77, 0x6b79); hram_wr(gb, 0x8b, A);
  CYC(0x6b79, 0x6b7c); SET_HL(0x6bab); // table_6bab
  CYC(0x6b7c, 0x6b7d); ball_addAToHl_from_rst(gb, 0x6b7d);
  CYC(0x6b7d, 0x6b7e); A = mem_rd(gb, HL);
  CYC(0x6b7e, 0x6b7f); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x6b7f, 0x6b81); func_6b9f_hook(gb); return; } // jr nz
  CYC(0x6b7f, 0x6b81);
  CYC(0x6b81, 0x6b84); A = mem_rd(gb, 0xccd6);
  CYC(0x6b84, 0x6b86); alu_and(gb, 0x7f);
  CYC(0x6b86, 0x6b88); alu_cp(gb, 0x01);
  CYC(0x6b88, 0x6b8a); A = hram_rd(gb, 0x8b);
  CYC(0x6b8a, 0x6b8d); mem_wr(gb, 0xcfd5, A);
  if (F & FZ) { CYCT(0x6b8d, 0x6b8f); goto L_6b91; } // jr z
  CYC(0x6b8d, 0x6b8f);
  CYC(0x6b8f, 0x6b91); alu_add(gb, 0x04);
L_6b91:
  CYC(0x6b91, 0x6b94); SET_HL(0x00f8);
  CYC(0x6b94, 0x6b95); alu_add(gb, L);
  CYC(0x6b95, 0x6b96); L = A;
  CYC(0x6b96, 0x6b99); A = mem_rd(gb, 0xccd4);
  CYC(0x6b99, 0x6b9a); alu_or(gb, mem_rd(gb, HL));
  CYC(0x6b9a, 0x6b9d); mem_wr(gb, 0xccd4, A);
  CYC(0x6b9d, 0x6b9f); func_6baf_hook(gb); return; // jr
}

void func_6b9f_hook(GB *gb) {
  CYC(0x6b9f, 0x6ba1); A = hram_rd(gb, 0x8b);
  CYC(0x6ba1, 0x6ba2); A = alu_inc8(gb, A);
  CYC(0x6ba2, 0x6ba4); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x6ba4, 0x6ba6); func_6b77_hook(gb); return; } // jr nz
  CYC(0x6ba4, 0x6ba6);
  CYC(0x6ba6, 0x6ba9); SET_HL(0xccd6);
  CYC(0x6ba9, 0x6baa); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x6baa); return; // ret
}

void func_6baf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6baf, objectGetShortPosition_hook, 0x2096, 0x6bb2);
  CYC(0x6bb2, 0x6bb3); C = A;
  CYC(0x6bb3, 0x6bb5); A = 0xa0;
  CALL_C(0x6bb5, setTile_hook, 0x3a9c, 0x6bb8);
  CYC(0x6bb8, 0x6bb9); H = D;
  CYC(0x6bb9, 0x6bbb); L = 0xc6; // Part.counter1
  CYC(0x6bbb, 0x6bbd); mem_wr(gb, HL, 0x03);
  CYC(0x6bbd, 0x6bc0); A = mem_rd(gb, 0xccd6);
  CYC(0x6bc0, 0x6bc2); alu_and(gb, 0x7f);
  CYC(0x6bc2, 0x6bc4); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(0x6bc4); return; } // ret nz
  CYC(0x6bc4, 0x6bc5);
  CYC(0x6bc5, 0x6bc7); A = 0x7e; // SND_SWITCH
  CYC(0x6bc7, 0x6bca); playSound_b00_hook(gb); return; // jp
}

void func_6bca_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6bca, 0x6bcd); A = mem_rd(gb, 0xcfd5);
  CYC(0x6bcd, 0x6bcf); alu_cp(gb, 0xff);
  if (F & FZ) { RET_TAKEN(0x6bcf); return; } // ret z
  CYC(0x6bcf, 0x6bd0);
  CYC(0x6bd0, 0x6bd2); A = 0x04;
L_6bd2:
  CYC(0x6bd2, 0x6bd4); hram_wr(gb, 0x8b, A);
  CYC(0x6bd4, 0x6bd7); SET_BC(0x9204); // INTERAC_FALLING_ROCK, $04
  CYC(0x6bd7, 0x6bda); A = mem_rd(gb, 0xcfd5);
  CYC(0x6bda, 0x6bdc); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(0x6bdc, 0x6bde); goto plus1; } // jr c
  CYC(0x6bdc, 0x6bde);
  CYC(0x6bde, 0x6be1); SET_BC(0x9205); // INTERAC_FALLING_ROCK, $05
plus1:
  CALL_C(0x6be1, objectCreateInteraction_hook, 0x24c5, 0x6be4);
  if (!(F & FZ)) { CYCT(0x6be4, 0x6be6); goto plus2; } // jr nz
  CYC(0x6be4, 0x6be6);
  CYC(0x6be6, 0x6be8); L = 0x4b;
  CYC(0x6be8, 0x6bea); A = hram_rd(gb, 0x8c);
  CALL_C(0x6bea, setShortPosition_hook, 0x20b8, 0x6bed);
  CYC(0x6bed, 0x6bef); L = 0x49;
  CYC(0x6bef, 0x6bf1); A = hram_rd(gb, 0x8b);
  CYC(0x6bf1, 0x6bf2); A = alu_dec8(gb, A);
  CYC(0x6bf2, 0x6bf3); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(0x6bf3, 0x6bf5); goto L_6bd2; } // jr nz
  CYC(0x6bf3, 0x6bf5);
plus2:
  RET(0x6bf5); return; // ret
}

void func_6bf6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6bf6, 0x6bf8); A = 0xa6; // SND_STRIKE
  CALL_C(0x6bf8, playSound_b00_hook, 0x0c98, 0x6bfb);
  CYC(0x6bfb, 0x6bfd); A = 0x01;
  CYC(0x6bfd, 0x6c00); mem_wr(gb, 0xcfd6, A);
  CYC(0x6c00, 0x6c02); func_6c27_hook(gb); return; // jr
}

void func_6c02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6c02, func_6c0e_hook, 0x6c0e, 0x6c05);
  CYC(0x6c05, 0x6c08); func_6b65_hook(gb); return; // jp
}

void func_6c08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6c08, func_6c0e_hook, 0x6c0e, 0x6c0b);
  CYC(0x6c0b, 0x6c0e); func_6b65_hook(gb); return; // jp
}

void func_6c0e_hook(GB *gb) {
  CYC(0x6c0e, 0x6c0f); alu_xor(gb, A);
  CYC(0x6c0f, 0x6c12); mem_wr(gb, 0xcfd6, A);
  CYC(0x6c12, 0x6c15); SET_HL(0xccd6);
  CYC(0x6c15, 0x6c16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x6c16); return; // ret
}

void func_6c17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c17, 0x6c18); alu_xor(gb, A);
  CYC(0x6c18, 0x6c1b); mem_wr(gb, 0xcfd6, A);
  CYC(0x6c1b, 0x6c1e); A = mem_rd(gb, 0xccd6);
  CYC(0x6c1e, 0x6c20); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(0x6c20, 0x6c22); func_6c27_hook(gb); return; } // jr nz
  CYC(0x6c20, 0x6c22);
  CYC(0x6c22, 0x6c24); A = 0x5a; // SND_ERROR
  CALL_C(0x6c24, playSound_b00_hook, 0x0c98, 0x6c27);
  func_6c27_hook(gb); // falls through
}

void func_6c27_hook(GB *gb) {
  CYC(0x6c27, 0x6c2a); SET_HL(0xccd6);
  CYC(0x6c2a, 0x6c2c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x6c2c, 0x6c2f); partDelete_hook(gb); return; // jp
}
