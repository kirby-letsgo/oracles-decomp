#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(hardhatWorker_moveLinkAway), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(hardhatWorker_moveLinkAway), (from), (to), true)

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
  BASE(hardhatWorker_moveLinkAway);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CALL_C(b_+3, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+6);
  CYC(b_+6, b_+8); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_HL(w1Link_yh);
  CYC(b_+12, b_+13); B = mem_rd(gb, HL);
  CYC(b_+13, b_+15); A = 0x48;
  CYC(b_+15, b_+16); alu_sub(gb, B);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+20); SET_HL(b_+41); // @simulatedInput
  CYC(b_+20, b_+22); A = 0x15; // :@simulatedInput bank
  CYC(b_+22, b_+23); push_effect(gb, DE);
  CALL_C(b_+23, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+26);
  CYC(b_+26, b_+27); SET_DE(pop_effect(gb));
  CYC(b_+27, b_+28); A = B;
  CYC(b_+28, b_+31); mem_wr(gb, wSimulatedInputCounter, A);
  CYC(b_+31, b_+33); A = 0x80; // BTN_DOWN
  CYC(b_+33, b_+36); mem_wr(gb, wSimulatedInputValue, A);
  CYC(b_+36, b_+37); alu_xor(gb, A);
  CYC(b_+37, b_+40); mem_wr(gb, wDisabledObjects, A);
  RET(b_+40); return;
}

void hardhatWorker_storeLinkVarsSomewhere_hook(GB *gb) {
  BASE(hardhatWorker_storeLinkVarsSomewhere);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(w1Link_yh);
  CALL_C(b_+3, getShortPositionFromDE_hook, SYM(getShortPositionFromDE), b_+6);
  CYC(b_+6, b_+9); mem_wr(gb, wTmpcfc0_genericCutscene_cfd3, A);
  CYC(b_+9, b_+11); E = (uint8_t)w1Link_direction;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+15); mem_wr(gb, wTmpcfc0_genericCutscene_cfd4, A);
  RET(b_+15); return;
}

void soldierSetSpeed80AndVar3fTo01_hook(GB *gb) {
  BASE(soldierSetSpeed80AndVar3fTo01);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+5, b_+7); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x01);
  RET(b_+9); return;
}

void hardhatWorker_setPatrolDirection_hook(GB *gb) {
  BASE(hardhatWorker_setPatrolDirection);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); A = alu_swap(gb, A);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+12); A = B;
  CYC(b_+12, SYM(hardhatWorker_setPatrolCounter)); interactionSetAnimation_hook(gb); return; // jp
}

void hardhatWorker_setPatrolCounter_hook(GB *gb) {
  BASE(hardhatWorker_setPatrolCounter);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  RET(b_+3); return;
}

void hardhatWorker_updatePatrolAnimation_hook(GB *gb) {
  BASE(hardhatWorker_updatePatrolAnimation);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, SYM(hardhatWorker_decPatrolCounter)); interactionSetAnimation_hook(gb); return; // jp
}

void hardhatWorker_decPatrolCounter_hook(GB *gb) {
  BASE(hardhatWorker_decPatrolCounter);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, SYM(hardhatWorker_chooseTextForPatroller)); writeFlagsTocddb_hook(gb); return; // jp
}

void hardhatWorker_chooseTextForPatroller_hook(GB *gb) {
  BASE(hardhatWorker_chooseTextForPatroller);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+5, b_+7); goto have_index; } // jr z
  CYC(b_+5, b_+7);
  CALL_C(b_+7, getRandomNumber_hook, SYM(getRandomNumber), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0x03);

have_index:
  CYC(b_+12, b_+15); SET_HL(b_+25); // @textIDs
  CYC(b_+15, b_+16); hardhatWorker15_addAToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); A = 0x10; // >TX_1000
  CYC(b_+22, b_+23); E = alu_inc8(gb, E);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  RET(b_+24); return;
}

void hardhatWorker_checkBlackTowerProgressIs00_hook(GB *gb) {
  BASE(hardhatWorker_checkBlackTowerProgressIs00);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(b_+0, getBlackTowerProgress_hook, SYM(getBlackTowerProgress), b_+3);
  CYC(b_+3, SYM(hardhatWorker_checkBlackTowerProgressIs01)); writeFlagsTocddb_hook(gb); return; // jp
}

void hardhatWorker_checkBlackTowerProgressIs01_hook(GB *gb) {
  BASE(hardhatWorker_checkBlackTowerProgressIs01);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(b_+0, getBlackTowerProgress_hook, SYM(getBlackTowerProgress), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x01);
  CYC(b_+5, SYM(hardhatWorkerSubid02Script_b15)); writeFlagsTocddb_hook(gb); return; // jp
}
