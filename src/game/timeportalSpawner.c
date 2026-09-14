#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/ages/interactions/timeportalSpawner.s (INTERAC_TIMEPORTAL_SPAWNER), bank $10.

void markSpotDiscovered_hook(GB *gb);

static uint16_t timeportalSpawner_jump_table(GB *gb) {
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

void interactionCodee1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d02, 0x7d04); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7d04, 0x7d05); A = mem_rd(gb, DE);
  CYC(0x7d05, 0x7d06); push_effect(gb, 0x7d06);
  switch (timeportalSpawner_jump_table(gb)) {
    case 0x7d0e: goto state3;
    case 0x7d3d: goto state0;
    case 0x7d88: goto state1;
    case 0x7d96: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state3:
  CALL_C(0x7d0e, objectSetVisible83_hook, 0x1e72, 0x7d11);
  CYC(0x7d11, 0x7d13); B = 0x01;
  CALL_C(0x7d13, objectFlickerVisibility_hook, 0x24e6, 0x7d16);
  CALL_C(0x7d16, interactionAnimate_hook, 0x261b, 0x7d19);
  CYC(0x7d19, 0x7d1c); push_effect(gb, 0x7d1c);
  markSpotDiscovered_hook(gb);
  CYC(0x7d1c, 0x7d1f); A = mem_rd(gb, 0xcc2c);
  CYC(0x7d1f, 0x7d20); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x7d20); return; }
  CYC(0x7d20, 0x7d21);
  CALL_C(0x7d21, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x7d24);
  if (!(F & FC)) { RET_TAKEN(0x7d24); return; }
  CYC(0x7d24, 0x7d25);
  CALL_C(0x7d25, checkLinkCollisionsEnabled_hook, 0x1d32, 0x7d28);
  if (!(F & FC)) { RET_TAKEN(0x7d28); return; }
  CYC(0x7d28, 0x7d29);
  CYC(0x7d29, 0x7d2b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d2b, 0x7d2c); A = mem_rd(gb, DE);
  CYC(0x7d2c, 0x7d2e); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(0x7d2e, 0x7d30);
    goto interBankToBeginTimewarp;
  }
  CYC(0x7d2e, 0x7d30);
  CALL_C(0x7d30, getThisRoomFlags_hook, 0x197d, 0x7d33);
  CYC(0x7d33, 0x7d35); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));
interBankToBeginTimewarp:
  CYC(0x7d35, 0x7d38); SET_HL(0x7bce);
  CYC(0x7d38, 0x7d3a); E = 0x10;
  CYC(0x7d3a, 0x7d3d);
  interBankCall_hook(gb);
  return;
state0:
  CYC(0x7d3d, 0x7d3f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d3f, 0x7d40); A = mem_rd(gb, DE);
  CYC(0x7d40, 0x7d42); alu_and(gb, 0x0f);
  CYC(0x7d42, 0x7d43); push_effect(gb, 0x7d43);
  switch (timeportalSpawner_jump_table(gb)) {
    case 0x7d49: goto subid1Init;
    case 0x7d52: goto subid2Init;
    case 0x7d5e: goto commonInit;
    default: hook_continue(gb, HL, sp0_); return;
  }
subid1Init:
  CYC(0x7d49, 0x7d4b); A = 0x12;
  CALL_C(0x7d4b, checkGlobalFlag_hook, 0x31f3, 0x7d4e);
  if (!(F & FZ)) {
    CYCT(0x7d4e, 0x7d50);
    goto commonInit;
  }
  CYC(0x7d4e, 0x7d50);
  CYC(0x7d50, 0x7d52);
  goto setSubidBit7;
subid2Init:
  CYC(0x7d52, 0x7d54); A = 0x19;
  CALL_C(0x7d54, checkTreasureObtained_hook, 0x1748, 0x7d57);
  if (F & FC) {
    CYCT(0x7d57, 0x7d59);
    goto commonInit;
  }
  CYC(0x7d57, 0x7d59);
setSubidBit7:
  CYC(0x7d59, 0x7d5a); H = D;
  CYC(0x7d5a, 0x7d5c); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d5c, 0x7d5e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
commonInit:
  CALL_C(0x7d5e, objectGetTileAtPosition_hook, 0x1444, 0x7d61);
  CYC(0x7d61, 0x7d63); alu_cp(gb, 0xd7);
  if (!(F & FZ)) { RET_TAKEN(0x7d63); return; }
  CYC(0x7d63, 0x7d64);
  CALL_C(0x7d64, interactionInitGraphics_hook, 0x15fb, 0x7d67);
  CALL_C(0x7d67, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7d6a);
  CYC(0x7d6a, 0x7d6c); A = 0x02;
  CALL_C(0x7d6c, objectSetCollideRadius_hook, 0x24a1, 0x7d6f);
  CYC(0x7d6f, 0x7d71); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d71, 0x7d72); B = mem_rd(gb, HL);
  CYC(0x7d72, 0x7d74); alu_bit(gb, 6, B);
  if (F & FZ) {
    CYCT(0x7d74, 0x7d76);
    goto afterRoomFlagCheck;
  }
  CYC(0x7d74, 0x7d76);
  CALL_C(0x7d76, getThisRoomFlags_hook, 0x197d, 0x7d79);
  CYC(0x7d79, 0x7d7b); alu_and(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7d7b, 0x7d7d);
    goto afterRoomFlagCheck;
  }
  CYC(0x7d7b, 0x7d7d);
  CYC(0x7d7d, 0x7d7f); B = (uint8_t)(B | (1 << 7));
afterRoomFlagCheck:
  CALL_C(0x7d7f, interactionIncState_hook, 0x23e0, 0x7d82);
  CYC(0x7d82, 0x7d84); alu_bit(gb, 7, B);
  if (F & FZ) { RET_TAKEN(0x7d84); return; }
  CYC(0x7d84, 0x7d85);
  CYC(0x7d85, 0x7d87); mem_wr(gb, HL, 0x03);
  RET(0x7d87); return;
state1:
  CYC(0x7d88, 0x7d8b); A = mem_rd(gb, 0xcc8d);
  CYC(0x7d8b, 0x7d8c); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7d8c); return; }
  CYC(0x7d8c, 0x7d8d);
  CALL_C(0x7d8d, interactionIncState_hook, 0x23e0, 0x7d90);
  CALL_C(0x7d90, getThisRoomFlags_hook, 0x197d, 0x7d93);
  CYC(0x7d93, 0x7d95); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));
  RET(0x7d95); return;
state2:
  CYC(0x7d96, 0x7d99); A = mem_rd(gb, 0xcc8d);
  CYC(0x7d99, 0x7d9a); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7d9a); return; }
  CYC(0x7d9a, 0x7d9b);
  CYC(0x7d9b, 0x7d9d); A = 0xf1;
  CALL_C(0x7d9d, playSound_b00_hook, 0x0c98, 0x7da0);
  CYC(0x7da0, 0x7da2); A = 0x8d;
  CALL_C(0x7da2, playSound_b00_hook, 0x0c98, 0x7da5);
  CYC(0x7da5, 0x7da8);
  interactionIncState_hook(gb);
}

void markSpotDiscovered_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7d90, getThisRoomFlags_hook, 0x197d, 0x7d93);
  CYC(0x7d93, 0x7d95); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));
  RET(0x7d95); return;
}
