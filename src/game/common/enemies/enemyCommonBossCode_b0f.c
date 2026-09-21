#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/enemies/commonBossCode.s, bank $0f instance.

void enemyBoss_initializeRoomWithoutExtraGfx_b0f_hook(GB *gb);

void enemyBoss_beginBoss_common_b0f_hook(GB *gb) {
  BASE(enemyBoss_beginBoss_b0f);
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+6); W8(wDisabledObjects) = A;
  CYC(b_+6, b_+9); W8(wMenuDisabled) = A;
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+13); W8(wActiveMusic) = A;
  CYC(b_+13, b_+16);
  TAIL(playSound_b00);
}

void enemyBoss_dead_b0f_hook(GB *gb) {
  BASE(enemyBoss_dead_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto alreadyPlayedDeathSound;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x78);
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+18); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+18, b_+20); A = 0x67;
  CALL_C(b_+20, playSound_b00_hook, SYM(playSound_b00), b_+23);
alreadyPlayedDeathSound:
  CALL_C(b_+23, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+26);
  if (!(F & FZ)) {
    CYCT(b_+26, b_+29);
    TAIL(ecom_flickerVisibility_b0f);
  }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+30, getFreePartSlot_hook, SYM(getFreePartSlot), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x04);
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_ID;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CALL_C(b_+41, objectCopyPosition_hook, SYM(objectCopyPosition), b_+44);
  CALL_C(b_+44, markEnemyAsKilledInRoom_b00_hook, SYM(markEnemyAsKilledInRoom_b00), b_+47);
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_ID;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+52); alu_sub(gb, 0x08);
  CYC(b_+52, b_+54); alu_cp(gb, 0x68);
  if (F & FC) {
    CYCT(b_+54, b_+56);
    goto finish;
  }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+59); A = W8(wActiveMusic2);
  CYC(b_+59, b_+62); W8(wActiveMusic) = A;
  CALL_C(b_+62, playSound_b00_hook, SYM(playSound_b00), b_+65);
finish:
  CYC(b_+65, b_+68);
  TAIL(enemyDelete);
}

void enemyBoss_spawnShadow_b0f_hook(GB *gb) {
  BASE(enemyBoss_spawnShadow_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x07);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, B);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); mem_wr(gb, HL, C);
  CYC(b_+10, b_+12); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+12, b_+14); A = 0x80;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, HL, D);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  RET(b_+17); return;
}

void enemyBoss_initializeRoom_b0f_hook(GB *gb) {
  BASE(enemyBoss_initializeRoom_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+2, b_+4);
    goto skipScreenVarStore;
  }
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+7); W8(wEnemyIDToLoadExtraGfx) = A;
skipScreenVarStore:
  CYC(b_+7, b_+8); A = B;
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) {
    CYC(b_+9, b_+12);
  } else {
    CALL_C_CC(b_+9, loadPaletteHeader_hook, SYM(loadPaletteHeader), SYM(enemyBoss_initializeRoomWithoutExtraGfx_b0f));
  }
  TAIL(enemyBoss_initializeRoomWithoutExtraGfx_b0f);
}

void enemyBoss_initializeRoomWithoutExtraGfx_b0f_hook(GB *gb) {
  BASE(enemyBoss_initializeRoomWithoutExtraGfx_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (game_seasons) {
    CYC(b_+S(0), b_+S(2)); A = H8(hActiveObject);
    CYC(b_+S(2), b_+S(3)); D = A;
  }
  CYC(b_+O(0), b_+OE(2)); A = 0xf0;
  CALL_C(b_+O(2), playSound_b00_hook, SYM(playSound_b00), b_+OE(5));
  CYC(b_+O(5), b_+OE(6)); alu_xor(gb, A);
  CYC(b_+O(6), b_+OE(9)); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+O(9), b_+OE(10)); A = alu_dec8(gb, A);
  CYC(b_+O(10), b_+OE(13)); W8(wActiveMusic) = A;
  CYC(b_+O(13), b_+OE(16)); SET_HL(wcc93);
  CYC(b_+O(16), b_+OE(18)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+O(18), b_+OE(21)); A = W8(wScreenVariables);
  CYC(b_+O(21), b_+OE(23)); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+O(23)); return; }
  CYC(b_+O(23), b_+OE(24));
  CYC(b_+O(24), b_+OE(26)); A = 0x0b;
  CYC(b_+O(26), b_+OE(29)); W8(wLinkForceState) = A;
  CYC(b_+O(29), b_+OE(31)); A = GV(0x16, 0x1a);
  CYC(b_+O(31), b_+OE(34)); W8(wLinkStateParameter) = A;
  CYC(b_+O(34), b_+OE(37)); SET_HL(w1Link_direction);
  CYC(b_+O(37), b_+OE(40)); A = W8(wScreenTransitionDirection);
  CYC(b_+O(40), b_+OE(41)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(41), b_+OE(43)); A = alu_swap(gb, A);
  CYC(b_+O(43), b_+OE(44)); alu_rrca(gb);
  CYC(b_+O(44), b_+OE(45)); mem_wr(gb, HL, A);
  RET(b_+O(45)); return;
}

void enemyBoss_beginMiniboss_b0f_hook(GB *gb) {
  BASE(enemyBoss_beginMiniboss_b0f);
  CYC(b_+0, b_+2); B = 0x2d;
  CYC(b_+2, b_+4);
  enemyBoss_beginBoss_common_b0f_hook(gb);
}

void enemyBoss_beginBoss_b0f_hook(GB *gb) {
  BASE(enemyBoss_beginBoss_b0f);
  CYC(b_+0, b_+2); B = 0x2e;
  enemyBoss_beginBoss_common_b0f_hook(gb);
}
