#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

// object_code/common/enemies/commonBossCode.s, bank $0f instance.

void enemyBoss_initializeRoomWithoutExtraGfx_b0f_hook(GB *gb);

void enemyBoss_beginBoss_common_b0f_hook(GB *gb) {
  CYC(0x4586, 0x4587); alu_xor(gb, A);
  CYC(0x4587, 0x458a); W8(wDisabledObjects) = A;
  CYC(0x458a, 0x458d); W8(wMenuDisabled) = A;
  CYC(0x458d, 0x458e); A = B;
  CYC(0x458e, 0x4591); W8(wActiveMusic) = A;
  CYC(0x4591, 0x4594);
  playSound_b00_hook(gb);
}

void enemyBoss_dead_b0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x44f0, 0x44f1); H = D;
  CYC(0x44f1, 0x44f3); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x44f3, 0x44f4); A = mem_rd(gb, HL);
  CYC(0x44f4, 0x44f5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x44f5, 0x44f7);
    goto alreadyPlayedDeathSound;
  }
  CYC(0x44f5, 0x44f7);
  CYC(0x44f7, 0x44f9); mem_wr(gb, HL, 0x00);
  CYC(0x44f9, 0x44fb); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x44fb, 0x44fd); mem_wr(gb, HL, 0x78);
  CYC(0x44fd, 0x44ff); A = 0x01;
  CYC(0x44ff, 0x4502); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x4502, 0x4504); A = 0x67;
  CALL_C(0x4504, playSound_b00_hook, 0x0c98, 0x4507);
alreadyPlayedDeathSound:
  CALL_C(0x4507, ecom_decCounter1_b0f_hook, 0x439a, 0x450a);
  if (!(F & FZ)) {
    CYCT(0x450a, 0x450d);
    ecom_flickerVisibility_b0f_hook(gb);
    return;
  }
  CYC(0x450a, 0x450d);
  CYC(0x450d, 0x450e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x450e, getFreePartSlot_hook, 0x3e8e, 0x4511);
  if (!(F & FZ)) { RET_TAKEN(0x4511); return; }
  CYC(0x4511, 0x4512);
  CYC(0x4512, 0x4514); mem_wr(gb, HL, 0x04);
  CYC(0x4514, 0x4515); L = alu_inc8(gb, L);
  CYC(0x4515, 0x4517); E = ENEMY_BASE + OBJ_ID;
  CYC(0x4517, 0x4518); A = mem_rd(gb, DE);
  CYC(0x4518, 0x4519); mem_wr(gb, HL, A);
  CALL_C(0x4519, objectCopyPosition_hook, 0x2242, 0x451c);
  CALL_C(0x451c, markEnemyAsKilledInRoom_b00_hook, 0x320d, 0x451f);
  CYC(0x451f, 0x4521); E = ENEMY_BASE + OBJ_ID;
  CYC(0x4521, 0x4522); A = mem_rd(gb, DE);
  CYC(0x4522, 0x4524); alu_sub(gb, 0x08);
  CYC(0x4524, 0x4526); alu_cp(gb, 0x68);
  if (F & FC) {
    CYCT(0x4526, 0x4528);
    goto finish;
  }
  CYC(0x4526, 0x4528);
  CYC(0x4528, 0x452b); A = W8(wActiveMusic2);
  CYC(0x452b, 0x452e); W8(wActiveMusic) = A;
  CALL_C(0x452e, playSound_b00_hook, 0x0c98, 0x4531);
finish:
  CYC(0x4531, 0x4534);
  enemyDelete_hook(gb);
}

void enemyBoss_spawnShadow_b0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4534, getFreePartSlot_hook, 0x3e8e, 0x4537);
  if (!(F & FZ)) { RET_TAKEN(0x4537); return; }
  CYC(0x4537, 0x4538);
  CYC(0x4538, 0x453a); mem_wr(gb, HL, 0x07);
  CYC(0x453a, 0x453b); L = alu_inc8(gb, L);
  CYC(0x453b, 0x453c); mem_wr(gb, HL, B);
  CYC(0x453c, 0x453d); L = alu_inc8(gb, L);
  CYC(0x453d, 0x453e); mem_wr(gb, HL, C);
  CYC(0x453e, 0x4540); L = PART_BASE + OBJ_RELATED1;
  CYC(0x4540, 0x4542); A = 0x80;
  CYC(0x4542, 0x4543); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4543, 0x4544); mem_wr(gb, HL, D);
  CYC(0x4544, 0x4545); alu_xor(gb, A);
  RET(0x4545); return;
}

void enemyBoss_initializeRoom_b0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4546, 0x4548); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x4548, 0x454a);
    goto skipScreenVarStore;
  }
  CYC(0x4548, 0x454a);
  CYC(0x454a, 0x454d); W8(wEnemyIDToLoadExtraGfx) = A;
skipScreenVarStore:
  CYC(0x454d, 0x454e); A = B;
  CYC(0x454e, 0x454f); alu_or(gb, A);
  if (F & FZ) {
    CYC(0x454f, 0x4552);
  } else {
    CALL_C_CC(0x454f, loadPaletteHeader_hook, 0x050b, 0x4552);
  }
  enemyBoss_initializeRoomWithoutExtraGfx_b0f_hook(gb);
}

void enemyBoss_initializeRoomWithoutExtraGfx_b0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4552, 0x4554); A = 0xf0;
  CALL_C(0x4554, playSound_b00_hook, 0x0c98, 0x4557);
  CYC(0x4557, 0x4558); alu_xor(gb, A);
  CYC(0x4558, 0x455b); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x455b, 0x455c); A = alu_dec8(gb, A);
  CYC(0x455c, 0x455f); W8(wActiveMusic) = A;
  CYC(0x455f, 0x4562); SET_HL(wcc93);
  CYC(0x4562, 0x4564); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x4564, 0x4567); A = W8(wScreenVariables);
  CYC(0x4567, 0x4569); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(0x4569); return; }
  CYC(0x4569, 0x456a);
  CYC(0x456a, 0x456c); A = 0x0b;
  CYC(0x456c, 0x456f); W8(wLinkForceState) = A;
  CYC(0x456f, 0x4571); A = 0x16;
  CYC(0x4571, 0x4574); W8(wLinkStateParameter) = A;
  CYC(0x4574, 0x4577); SET_HL(w1Link_direction);
  CYC(0x4577, 0x457a); A = W8(wScreenTransitionDirection);
  CYC(0x457a, 0x457b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x457b, 0x457d); A = alu_swap(gb, A);
  CYC(0x457d, 0x457e); alu_rrca(gb);
  CYC(0x457e, 0x457f); mem_wr(gb, HL, A);
  RET(0x457f); return;
}

void enemyBoss_beginMiniboss_b0f_hook(GB *gb) {
  CYC(0x4580, 0x4582); B = 0x2d;
  CYC(0x4582, 0x4584);
  enemyBoss_beginBoss_common_b0f_hook(gb);
}

void enemyBoss_beginBoss_b0f_hook(GB *gb) {
  CYC(0x4584, 0x4586); B = 0x2e;
  enemyBoss_beginBoss_common_b0f_hook(gb);
}
