#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_HARDHAT_WORKER), bank 0x15.
// soldierSetSpeed80AndVar3fTo01 (15:5fb9) sits between hardhatWorker_storeLinkVarsSomewhere and
// hardhatWorker_setPatrolDirection in the disassembly and is only ever used by the hardhat
// worker's patrol script, so it lives in this file despite its name.

static void hardhatWorker15_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

// Move Link away to make way for the hardhat worker to move right, if necessary.
void hardhatWorker_moveLinkAway_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f75, objectGetAngleTowardLink_hook, 0x1e9c, 0x5f78);
  CALL_C(0x5f78, convertAngleToDirection_hook, 0x26f9, 0x5f7b);
  CYC(0x5f7b, 0x5f7d); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(0x5f7d); return; }
  CYC(0x5f7d, 0x5f7e);
  CYC(0x5f7e, 0x5f81); SET_HL(w1Link_yh);
  CYC(0x5f81, 0x5f82); B = mem_rd(gb, HL);
  CYC(0x5f82, 0x5f84); A = 0x48;
  CYC(0x5f84, 0x5f85); alu_sub(gb, B);
  CYC(0x5f85, 0x5f86); B = A;
  CYC(0x5f86, 0x5f89); SET_HL(0x5f9e); // @simulatedInput
  CYC(0x5f89, 0x5f8b); A = 0x15; // :@simulatedInput bank
  CYC(0x5f8b, 0x5f8c); push_effect(gb, DE);
  CALL_C(0x5f8c, setSimulatedInputAddress_hook, 0x2a1d, 0x5f8f);
  CYC(0x5f8f, 0x5f90); SET_DE(pop_effect(gb));
  CYC(0x5f90, 0x5f91); A = B;
  CYC(0x5f91, 0x5f94); mem_wr(gb, wSimulatedInputCounter, A);
  CYC(0x5f94, 0x5f96); A = 0x80; // BTN_DOWN
  CYC(0x5f96, 0x5f99); mem_wr(gb, wSimulatedInputValue, A);
  CYC(0x5f99, 0x5f9a); alu_xor(gb, A);
  CYC(0x5f9a, 0x5f9d); mem_wr(gb, wDisabledObjects, A);
  RET(0x5f9d); return;
}

void hardhatWorker_storeLinkVarsSomewhere_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5fa9, 0x5fac); SET_DE(w1Link_yh);
  CALL_C(0x5fac, getShortPositionFromDE_hook, 0x209b, 0x5faf);
  CYC(0x5faf, 0x5fb2); mem_wr(gb, wTmpcfc0_genericCutscene_cfd3, A);
  CYC(0x5fb2, 0x5fb4); E = (uint8_t)w1Link_direction;
  CYC(0x5fb4, 0x5fb5); A = mem_rd(gb, DE);
  CYC(0x5fb5, 0x5fb8); mem_wr(gb, wTmpcfc0_genericCutscene_cfd4, A);
  RET(0x5fb8); return;
}

void soldierSetSpeed80AndVar3fTo01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5fb9, 0x5fba); H = D;
  CYC(0x5fba, 0x5fbc); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5fbc, 0x5fbe); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x5fbe, 0x5fc0); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x5fc0, 0x5fc2); mem_wr(gb, HL, 0x01);
  RET(0x5fc2); return;
}

void hardhatWorker_setPatrolDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5fc3, 0x5fc4); H = D;
  CYC(0x5fc4, 0x5fc6); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x5fc6, 0x5fc7); mem_wr(gb, HL, A);
  CYC(0x5fc7, 0x5fc8); B = A;
  CYC(0x5fc8, 0x5fca); A = alu_swap(gb, A);
  CYC(0x5fca, 0x5fcb); alu_rrca(gb);
  CYC(0x5fcb, 0x5fcd); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5fcd, 0x5fce); mem_wr(gb, HL, A);
  CYC(0x5fce, 0x5fcf); A = B;
  CYC(0x5fcf, 0x5fd2); interactionSetAnimation_hook(gb); return; // jp
}

void hardhatWorker_setPatrolCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5fd2, 0x5fd4); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x5fd4, 0x5fd5); mem_wr(gb, DE, A);
  RET(0x5fd5); return;
}

void hardhatWorker_updatePatrolAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5fd6, 0x5fd8); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x5fd8, 0x5fd9); A = mem_rd(gb, DE);
  CYC(0x5fd9, 0x5fdc); interactionSetAnimation_hook(gb); return; // jp
}

void hardhatWorker_decPatrolCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5fdc, 0x5fdd); H = D;
  CYC(0x5fdd, 0x5fdf); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x5fdf, 0x5fe0); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5fe0, 0x5fe3); writeFlagsTocddb_hook(gb); return; // jp
}

void hardhatWorker_chooseTextForPatroller_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5fe3, 0x5fe5); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5fe5, 0x5fe6); A = mem_rd(gb, DE);
  CYC(0x5fe6, 0x5fe8); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x5fe8, 0x5fea); goto have_index; } // jr z
  CYC(0x5fe8, 0x5fea);
  CALL_C(0x5fea, getRandomNumber_hook, 0x043e, 0x5fed);
  CYC(0x5fed, 0x5fef); alu_and(gb, 0x03);

have_index:
  CYC(0x5fef, 0x5ff2); SET_HL(0x5ffc); // @textIDs
  CYC(0x5ff2, 0x5ff3); hardhatWorker15_addAToHl_from_rst(gb, 0x5ff3);
  CYC(0x5ff3, 0x5ff4); A = mem_rd(gb, HL);
  CYC(0x5ff4, 0x5ff6); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x5ff6, 0x5ff7); mem_wr(gb, DE, A);
  CYC(0x5ff7, 0x5ff9); A = 0x10; // >TX_1000
  CYC(0x5ff9, 0x5ffa); E = alu_inc8(gb, E);
  CYC(0x5ffa, 0x5ffb); mem_wr(gb, DE, A);
  RET(0x5ffb); return;
}

void hardhatWorker_checkBlackTowerProgressIs00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(0x6001, getBlackTowerProgress_hook, 0x36c0, 0x6004);
  CYC(0x6004, 0x6007); writeFlagsTocddb_hook(gb); return; // jp
}

void hardhatWorker_checkBlackTowerProgressIs01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(0x6007, getBlackTowerProgress_hook, 0x36c0, 0x600a);
  CYC(0x600a, 0x600c); alu_cp(gb, 0x01);
  CYC(0x600c, 0x600f); writeFlagsTocddb_hook(gb); return; // jp
}
