#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void parseGivenObjectData_b12_hook(GB *gb) {
  BASE(parseGivenObjectData_b12);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_cp(gb, 0xfe);
  if (!(F & FZ)) CYCT(b_+3, b_+5);
  else {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+6); SET_DE(pop_effect(gb));
  }
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); SET_DE(DE + 1);
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  burn_rom(gb, 0, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0, 0x0004, 0x0006, false);
    burn_rom(gb, 0, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0, 0x000a, 0x000b, false);
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(objectDataOp0)) { objectDataOp0_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp1)) { objectDataOp1_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp2)) { objectDataOp2_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp3)) { objectDataOp3_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp4)) { objectDataOp4_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp5)) { objectDataOp5_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp6)) { objectDataOp6_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp7)) { objectDataOp7_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp8)) { objectDataOp8_hook(gb); return; }
    else if (jt_ == SYM(objectDataOp9)) { objectDataOp9_hook(gb); return; }
    else if (jt_ == SYM(objectDataOpA)) { objectDataOpA_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void read2_bytes(GB *gb, uint16_t dst, uint16_t src) {
  BASE(read2Bytes);
  CYC(b_+0, b_+2); uint8_t value = mem_rd(gb, src);
  CYC(b_+2, b_+3); mem_wr(gb, dst, value);
  CYC(b_+3, b_+5); value = mem_rd(gb, src + 1);
  CYC(b_+5, b_+6); mem_wr(gb, dst + 1, value);
  CYC(b_+6, b_+7);
}

void read2Bytes_hook(GB *gb) {
  uint16_t src = DE, dst = HL;
  read2_bytes(gb, dst, src);
  A = mem_rd(gb, src + 1);
  SET_DE(src + 2);
  SET_HL(dst + 1);
  ret_effect(gb);
}

static void read_coordinates(GB *gb, uint16_t dst, uint16_t src) {
  BASE(readCoordinates);
  CYC(b_+0, b_+2); uint8_t value = mem_rd(gb, src);
  CYC(b_+2, b_+3); mem_wr(gb, dst, value); SET_HL(HL + 1);
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);
  CYC(b_+4, b_+6); value = mem_rd(gb, src + 1);
  CYC(b_+6, b_+7); mem_wr(gb, (uint16_t)((dst & 0xff00) | ((dst + 2) & 0xff)), value);
  CYC(b_+7, b_+8);
}

void readCoordinates_hook(GB *gb) {
  uint16_t src = DE, dst = HL;
  read_coordinates(gb, dst, src);
  A = mem_rd(gb, src + 1);
  SET_DE(src + 2);
  ret_effect(gb);
}

void decEnemyCounterIfApplicable_hook(GB *gb) {
  BASE(decEnemyCounterIfApplicable);
  CYC(b_+0, b_+2); A = H8(hFF8B);
  alu_and(gb, 0x02);
  if (F & FZ) { CYCT(b_+2, b_+5); ret_effect(gb); return; }
  CYC(b_+2, b_+5); A = W8(wNumEnemies);
  CYC(b_+5, b_+9); A = alu_dec8(gb, A);
  CYC(b_+9, b_+13); W8(wNumEnemies) = A;
  ret_effect(gb);
}

void addPositionToPlacedEnemyPositions_hook(GB *gb) {
  BASE(addPositionToPlacedEnemyPositions);
  uint16_t saved_hl = HL;
  CYC(b_+0, b_+1); push_effect(gb, saved_hl);
  CYC(b_+1, b_+4); A = W8(wEnemyPlacement_numEnemies);
  CYC(b_+4, b_+7); SET_HL(wEnemyPlacement_placedEnemyPositions);
  CYC(b_+7, b_+8);
  burn_rom(gb, 0, 0x0010, 0x0012, false); alu_add(gb, L); L = A;
  if (F & FC) { burn_rom(gb, 0, 0x0012, 0x0015, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 0x0012, 0x0013, true);
  CYC(b_+8, b_+9); mem_wr(gb, HL, C);
  CYC(b_+9, b_+12); A = W8(wEnemyPlacement_numEnemies);
  CYC(b_+12, b_+13); A = alu_inc8(gb, A);
  CYC(b_+13, b_+15); alu_and(gb, 0x0f);
  CYC(b_+15, b_+18); W8(wEnemyPlacement_numEnemies) = A;
  CYC(b_+18, b_+19); SET_HL(pop_effect(gb));
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void assignRandomPositionToEnemy_hook(GB *gb) {
  BASE(assignRandomPositionToEnemy);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomPositionForEnemy_b00_hook, SYM(getRandomPositionForEnemy_b00), b_+3);
  if (F & FC) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+8); C = W8(wEnemyPlacement_enemyPos);
  CALL_C(b_+8, addPositionToPlacedEnemyPositions_hook, SYM(addPositionToPlacedEnemyPositions), b_+11);
  CYC(b_+11, b_+13); H = H8(hFF91);
  CYC(b_+13, b_+16); L = 0x8b;
  CALL_C(b_+16, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+19);
  CYC(b_+19, b_+20); alu_xor(gb, A);
  RET(b_+20);
}

void checkEnemyKilled_hook(GB *gb) {
  BASE(checkEnemyKilled);
  CYC(b_+0, b_+3); A = W8(wEnemyPlacement_numKillableEnemies);
  CYC(b_+3, b_+5); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto done; }
  CYC(b_+5, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+11); W8(wEnemyPlacement_numKillableEnemies) = A;
  CYC(b_+11, b_+16); SET_HL(bitTable + A);
  CYC(b_+16, b_+20); A = W8(wEnemyPlacement_killedEnemiesBitset); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+20, b_+21); ret_effect(gb); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+24); A = W8(wEnemyPlacement_numKillableEnemies);
  CYC(b_+24, b_+26); alu_swap_a(gb);
  CYC(b_+26, b_+30); alu_or(gb, 0x01); H8(hFF8D) = A;
done:
  CYC(b_+30, b_+31); alu_scf(gb);
  CYC(b_+31, b_+32); ret_effect(gb);
}

void parseObjectData_hook(GB *gb) {
  BASE(parseObjectData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wNumEnemies) = A;
  CYC(b_+4, b_+7); W8(wTmpcfc0) = A;
  CYC(b_+7, b_+10); SET_HL(wEnemyPlacement);
  CYC(b_+10, b_+12); B = 0x20;
  CALL_C(b_+12, clearMemory_hook, SYM(clearMemory), b_+15);
  CALL_C(b_+15, addRoomToEnemiesKilledList_b00_hook, SYM(addRoomToEnemiesKilledList_b00), b_+18);
  CALL_C(b_+18, generateRandomBuffer_b00_hook, SYM(generateRandomBuffer_b00), b_+21);
  CYC(b_+21, b_+24); SET_HL((SYM(group5Mapd3EnemyObjectData) + 6));
  CYC(b_+24, b_+26); E = 0x15;
  CALL_C(b_+26, interBankCall_hook, 0x008a, SYM(parseGivenObjectData_b12));
  parseGivenObjectData_b12_hook(gb);
}

void func_55f8_hook(GB *gb) {
  BASE(func_55f8);
  CYC(b_+0, b_+2);
  parseGivenObjectData_b12_hook(gb);
}

void parseGivenObjectData_hl_hook(GB *gb) {
  BASE(parseGivenObjectData_hl);
  CYC(b_+0, b_+1); E = L;
  CYC(b_+1, b_+2); D = H;
  CYC(b_+2, (SYM(objectDataOpcodeSizes) + 1));
  parseGivenObjectData_b12_hook(gb);
}

void skipToOpEnd_2byte_hook(GB *gb) {
  BASE(skipToOpEnd_2byte);
  for (;;) {
    CYC(b_+0, b_+2); SET_DE(DE + 2);
    CYC(b_+2, b_+3); A = mem_rd(gb, DE);
    CYC(b_+3, b_+5); alu_cp(gb, 0xf0);
    if (F & FC) { CYCT(b_+5, b_+8); continue; }
    CYC(b_+5, b_+8);
    CYC(b_+8, b_+11);
    parseGivenObjectData_b12_hook(gb);
    return;
  }
}

void skipToOpEnd_4byte_hook(GB *gb) {
  BASE(skipToOpEnd_4byte);
  for (;;) {
    CYC(b_+0, b_+4); SET_DE(DE + 4);
    CYC(b_+4, b_+5); A = mem_rd(gb, DE);
    CYC(b_+5, b_+7); alu_cp(gb, 0xf0);
    if (F & FC) { CYCT(b_+7, b_+10); continue; }
    CYC(b_+7, b_+10);
    CYC(b_+10, b_+13);
    parseGivenObjectData_b12_hook(gb);
    return;
  }
}

void checkSkipPointer_hook(GB *gb) {
  BASE(checkSkipPointer);
  CYC(b_+0, b_+3); A = W8(wcc05);
  CYC(b_+3, b_+5); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wcc85);
  CYC(b_+9, b_+11); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+11, b_+13);
    CYC(b_+27, b_+28); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+28, b_+30); goto done; }
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+31); alu_xor(gb, A);
    CYC(b_+31, b_+32); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x03);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+20); W8(wcc85) = A;
  CYC(b_+20, b_+23); A = W8(wScreenTransitionDirection);
  CYC(b_+23, b_+24); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+24, b_+25); ret_effect(gb); return; }
  CYC(b_+24, b_+25);
done:
  CYC(b_+25, b_+26); alu_or(gb, D);
  CYC(b_+26, b_+27);
  ret_effect(gb);
}

void skipPointer_hook(GB *gb) {
  BASE(skipPointer);
  CYC(b_+0, b_+2); SET_DE(DE + 2);
  CYC(b_+2, b_+5);
  parseGivenObjectData_b12_hook(gb);
}

void parsePointer_hook(GB *gb) {
  BASE(parsePointer);
  CYC(b_+0, b_+2); L = E; H = D;
  CYC(b_+2, b_+4); SET_DE(DE + 2);
  CYC(b_+4, b_+5); push_effect(gb, DE);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); D = mem_rd(gb, HL);
  CYC(b_+7, b_+8); E = A;
  CYC(b_+8, b_+11);
  parseGivenObjectData_b12_hook(gb);
}

void continueObjectLoopIfOpDone_hook(GB *gb) {
  BASE(continueObjectLoopIfOpDone);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); SET_BC(pop_effect(gb));
  CYC(b_+5, b_+8);
  parseGivenObjectData_b12_hook(gb);
}

void objectDataOp1_hook(GB *gb) {
  BASE(objectDataOp1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(b_+0, continueObjectLoopIfOpDone_hook, SYM(continueObjectLoopIfOpDone), b_+3);
    CALL_C(b_+3, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+6);
    if (!(F & FZ)) { CYCT(b_+6, b_+8); skipToOpEnd_2byte_hook(gb); return; }
    CYC(b_+6, b_+8);
    CALL_C(b_+8, read2Bytes_hook, SYM(read2Bytes), b_+11);
    CYCT(b_+11, b_+13);
  }
}

void objectDataOp2_hook(GB *gb) {
  BASE(objectDataOp2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(b_+0, continueObjectLoopIfOpDone_hook, SYM(continueObjectLoopIfOpDone), b_+3);
    CALL_C(b_+3, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+6);
    if (!(F & FZ)) { CYCT(b_+6, b_+8); skipToOpEnd_4byte_hook(gb); return; }
    CYC(b_+6, b_+8);
    CALL_C(b_+8, read2Bytes_hook, SYM(read2Bytes), b_+11);
    CYC(b_+11, b_+13); L = 0x4b;
    CALL_C(b_+13, readCoordinates_hook, SYM(readCoordinates), b_+16);
    CYCT(b_+16, b_+18);
  }
}

void objectDataOp3_hook(GB *gb) {
  BASE(objectDataOp3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkSkipPointer_hook, SYM(checkSkipPointer), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); skipPointer_hook(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7);
  parsePointer_hook(gb);
}

void objectDataOp4_hook(GB *gb) {
  BASE(objectDataOp4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkSkipPointer_hook, SYM(checkSkipPointer), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); skipPointer_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); skipPointer_hook(gb); return; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14);
  parsePointer_hook(gb);
}

void objectDataOp5_hook(GB *gb) {
  BASE(objectDataOp5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkSkipPointer_hook, SYM(checkSkipPointer), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); skipPointer_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+10, b_+12); skipPointer_hook(gb); return; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14);
  parsePointer_hook(gb);
}

void objectDataOp8_hook(GB *gb) {
  BASE(objectDataOp8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, DE);
    CYC(b_+1, b_+3); alu_bit(gb, 7, A);
    if (!(F & FZ)) { CYCT(b_+3, b_+6); parseGivenObjectData_b12_hook(gb); return; }
    CYC(b_+3, b_+6);
    CALL_C(b_+6, getFreePartSlot_hook, SYM(getFreePartSlot), b_+9);
    if (!(F & FZ)) { CYCT(b_+9, b_+12); goto skip; }
    CYC(b_+9, b_+12);
    CALL_C(b_+12, read2Bytes_hook, SYM(read2Bytes), b_+15);
    CYC(b_+15, b_+16); A = mem_rd(gb, DE);
    CYC(b_+16, b_+17); C = A;
    CYC(b_+17, b_+18); SET_DE(DE + 1);
    CYC(b_+18, b_+20); L = 0xcb;
    CALL_C(b_+20, setShortPosition_hook, SYM(setShortPosition), b_+23);
    CALL_C(b_+23, addPositionToPlacedEnemyPositions_hook, SYM(addPositionToPlacedEnemyPositions), b_+26);
    CYCT(b_+26, b_+28);
    continue;
skip:
    CYC(b_+28, b_+31); SET_DE(DE + 3);
    CYCT(b_+31, b_+33);
  }
}

void objectDataOp0_hook(GB *gb) {
  BASE(objectDataOp0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wRoomStateModifier);
  CYC(b_+3, b_+6); SET_HL(bitTable);
  CYC(b_+6, b_+7); alu_add(gb, L);
  CYC(b_+7, b_+8); L = A;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); SET_DE(DE + 1);
  CYC(b_+12, b_+13); alu_and(gb, B);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); parseGivenObjectData_b12_hook(gb); return; }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); B = 0;
  CYC(b_+18, b_+20); L = E; H = D;
  for (;;) {
    CYC(b_+20, b_+21); A = mem_rd(gb, HL);
    CYC(b_+21, b_+23); alu_cp(gb, 0xf0);
    if (F & FZ) { CYCT(b_+23, b_+25); parseGivenObjectData_hl_hook(gb); return; }
    CYC(b_+23, b_+25);
    CYC(b_+25, b_+27); alu_cp(gb, 0xfe);
    if (F & FZ) { CYCT(b_+27, b_+29); parseGivenObjectData_hl_hook(gb); return; }
    CYC(b_+27, b_+29);
    CYC(b_+29, b_+31); alu_cp(gb, 0xff);
    if (F & FZ) { CYCT(b_+31, b_+32); ret_effect(gb); return; }
    CYC(b_+31, b_+32);
    CYC(b_+32, b_+34); alu_and(gb, 0x0f);
    CYC(b_+34, b_+37); SET_DE(SYM(objectDataOpcodeSizes));
    CALL_C(b_+37, addDoubleIndexToDe_hook, 0x0072, b_+40);
    CYC(b_+40, b_+41); A = mem_rd(gb, DE);
    CYC(b_+41, b_+42); C = A;
    CYC(b_+42, b_+43); alu_add_hl(gb, BC);
    CYC(b_+43, b_+44); SET_DE(DE + 1);
    CYC(b_+44, b_+45); A = mem_rd(gb, DE);
    CYC(b_+45, b_+46); C = A;
    for (;;) {
      CYC(b_+46, b_+47); alu_add_hl(gb, BC);
      CYC(b_+47, b_+49); alu_bit(gb, 7, mem_rd(gb, HL));
      if (!(F & FZ)) { CYCT(b_+49, b_+51); break; }
      CYCT(b_+51, b_+53);
    }
  }
}

void objectDataOp6_hook(GB *gb) {
  BASE(objectDataOp6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, DE); SET_DE(DE + 1);
  CYC(b_+2, b_+3); B = A;
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  CYC(b_+5, b_+7); H8(hFF8B) = A;
  CYC(b_+7, b_+8); A = B;
  CYC(b_+8, b_+10); A = alu_swap(gb, A);
  CYC(b_+10, b_+11); alu_rrca(gb);
  CYC(b_+11, b_+13); alu_and(gb, 0x07);
  CYC(b_+13, b_+15); H8(hFF8C) = A;
  CYC(b_+15, b_+17); A = mem_rd(gb, DE); SET_DE(DE + 1);
  CYC(b_+17, b_+19); H8(hFF8F) = A;
  CYC(b_+19, b_+21); A = mem_rd(gb, DE); SET_DE(DE + 1);
  CYC(b_+21, b_+23); H8(hFF8E) = A;
  for (;;) {
    CYC(b_+23, b_+25); A = 1;
    CYC(b_+25, b_+27); H8(hFF8D) = A;
    CYC(b_+27, b_+29); A = H8(hFF8B);
    CYC(b_+29, b_+31); alu_and(gb, 1);
    if (!(F & FZ)) { CYCT(b_+31, b_+33); goto allocate; }
    CYC(b_+31, b_+33);
    CALL_C(b_+33, checkEnemyKilled_hook, SYM(checkEnemyKilled), b_+36);
    if (!(F & FC)) { CYCT(b_+36, b_+38); goto next; }
    CYC(b_+36, b_+38);
allocate:
    CALL_C(b_+38, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+41);
    if (!(F & FZ)) { CYCT(b_+41, b_+44); parseGivenObjectData_b12_hook(gb); return; }
    CYC(b_+41, b_+44);
    CALL_C(b_+44, decEnemyCounterIfApplicable_hook, SYM(decEnemyCounterIfApplicable), b_+47);
    CYC(b_+47, b_+49); A = H8(hFF8F);
    CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+50, b_+52); A = H8(hFF8E);
    CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+53, b_+54); A = H;
    CYC(b_+54, b_+56); H8(hFF91) = A;
    CYC(b_+56, b_+57); push_effect(gb, DE);
    CALL_C(b_+57, assignRandomPositionToEnemy_hook, SYM(assignRandomPositionToEnemy), b_+60);
    CYC(b_+60, b_+61); SET_DE(pop_effect(gb));
    CYC(b_+61, b_+63); A = H8(hFF91);
    CYC(b_+63, b_+64); H = A;
    if (!(F & FC)) { CYCT(b_+64, b_+66); goto placed; }
    CYC(b_+64, b_+66);
    CYC(b_+66, b_+68); L = 0x80;
    CYC(b_+68, b_+70); mem_wr(gb, HL, 0);
    CYC(b_+70, b_+72);
    goto next;
placed:
    CYC(b_+72, b_+74); L = 0x80;
    CYC(b_+74, b_+76); A = H8(hFF8D);
    CYC(b_+76, b_+77); mem_wr(gb, HL, A);
next:
    CYC(b_+77, b_+79); A = H8(hFF8C);
    CYC(b_+79, b_+80); A = alu_dec8(gb, A);
    CYC(b_+80, b_+82); H8(hFF8C) = A;
    if (!(F & FZ)) { CYCT(b_+82, b_+84); continue; }
    CYC(b_+82, b_+84);
    CYC(b_+84, b_+87);
    parseGivenObjectData_b12_hook(gb);
    return;
  }
}

void objectDataOp7_hook(GB *gb) {
  BASE(objectDataOp7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+4); A = mem_rd(gb, DE); SET_DE(DE + 1); H8(hFF8B) = A;
  for (;;) {
    CYC(b_+4, b_+5); A = mem_rd(gb, DE);
    CYC(b_+5, b_+7); alu_bit(gb, 7, A);
    if (!(F & FZ)) { CYCT(b_+7, b_+10); parseGivenObjectData_b12_hook(gb); return; }
    CYC(b_+7, b_+10);
    CYC(b_+10, b_+12); A = 1;
    CYC(b_+12, b_+14); H8(hFF8D) = A;
    CYC(b_+14, b_+16); A = H8(hFF8B);
    CYC(b_+16, b_+18); alu_and(gb, 1);
    if (!(F & FZ)) { CYCT(b_+18, b_+20); goto allocate; }
    CYC(b_+18, b_+20);
    CALL_C(b_+20, checkEnemyKilled_hook, SYM(checkEnemyKilled), b_+23);
    if (F & FC) { CYCT(b_+23, b_+25); goto allocate; }
    CYC(b_+23, b_+25);
    CYC(b_+25, b_+29); SET_DE(DE + 4);
    CYCT(b_+29, b_+31);
    continue;
allocate:
    CALL_C(b_+31, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+34);
    if (!(F & FZ)) { CYCT(b_+34, b_+37); skipToOpEnd_4byte_hook(gb); return; }
    CYC(b_+34, b_+37);
    CALL_C(b_+37, decEnemyCounterIfApplicable_hook, SYM(decEnemyCounterIfApplicable), b_+40);
    CALL_C(b_+40, read2Bytes_hook, SYM(read2Bytes), b_+43);
    CYC(b_+43, b_+45); L = 0x8b;
    CALL_C(b_+45, readCoordinates_hook, SYM(readCoordinates), b_+48);
    CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+49, b_+51); alu_and(gb, 0xf0);
    CYC(b_+51, b_+53); A = alu_swap(gb, A);
    CYC(b_+53, b_+54); C = A;
    CYC(b_+54, b_+55); L = alu_dec8(gb, L);
    CYC(b_+55, b_+56); A = mem_rd(gb, HL);
    CYC(b_+56, b_+58); alu_and(gb, 0xf0);
    CYC(b_+58, b_+59); alu_or(gb, C);
    CYC(b_+59, b_+60); C = A;
    CALL_C(b_+60, addPositionToPlacedEnemyPositions_hook, SYM(addPositionToPlacedEnemyPositions), b_+63);
    CYC(b_+63, b_+65); L = 0x80;
    CYC(b_+65, b_+68); A = H8(hFF8D); mem_wr(gb, HL, A);
    CYCT(b_+68, b_+70);
  }
}

void objectDataOp9_hook(GB *gb) {
  BASE(objectDataOp9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(b_+0, continueObjectLoopIfOpDone_hook, SYM(continueObjectLoopIfOpDone), b_+3);
    CALL_ROM(b_+3, b_+40);
    if (!(F & FZ)) { CYCT(b_+6, b_+8); goto allocation_failure; }
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+11); SET_DE(DE + 1); A = mem_rd(gb, DE); SET_DE(DE + 1);
    CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+12, b_+15); A = mem_rd(gb, DE); SET_DE(DE + 1); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+15, b_+18); A = mem_rd(gb, DE); SET_DE(DE + 1); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+18, b_+24); A = L; alu_and(gb, 0xc0); alu_add(gb, 0x0b); L = A;
    CYC(b_+24, b_+27); A = mem_rd(gb, DE); SET_DE(DE + 1); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+27, b_+28); L = alu_inc8(gb, L);
    CYC(b_+28, b_+31); A = mem_rd(gb, DE); SET_DE(DE + 1); mem_wr(gb, HL, A);
    CYCT(b_+31, b_+33);
    continue;
allocation_failure:
    CYC(b_+33, b_+35); A = 6;
    CALL_C(b_+35, addAToDe_hook, 0x0068, b_+38);
    CYCT(b_+38, b_+40);
  }
}

void objectDataOpA_hook(GB *gb) {
  BASE(objectDataOpA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+4); A = mem_rd(gb, DE); SET_DE(DE + 1); H8(hFF8B) = A;
  for (;;) {
    CYC(b_+4, b_+5); A = mem_rd(gb, DE);
    CYC(b_+5, b_+7); alu_bit(gb, 7, A);
    if (!(F & FZ)) { CYCT(b_+7, b_+10); parseGivenObjectData_b12_hook(gb); return; }
    CYC(b_+7, b_+10);
    CYC(b_+10, b_+12); A = 1;
    CYC(b_+12, b_+14); H8(hFF8D) = A;
    CYC(b_+14, b_+16); A = H8(hFF8B);
    CYC(b_+16, b_+18); alu_and(gb, 1);
    if (!(F & FZ)) { CYCT(b_+18, b_+20); goto allocate; }
    CYC(b_+18, b_+20);
    CALL_C(b_+20, checkEnemyKilled_hook, SYM(checkEnemyKilled), b_+23);
    if (F & FC) { CYCT(b_+23, b_+25); goto allocate; }
    CYC(b_+23, b_+25);
    CYC(b_+25, b_+29); SET_DE(DE + 2);
    CYCT(b_+29, b_+32);
    continue;
allocate:
    CALL_C(b_+29, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+32);
    if (!(F & FZ)) { CYCT(b_+32, b_+35); skipToOpEnd_2byte_hook(gb); return; }
    CYC(b_+32, b_+35);
    CYC(b_+35, b_+37); mem_wr(gb, HL, 0x59);
    CYC(b_+37, b_+38); L = alu_inc8(gb, L);
    CYC(b_+38, b_+41); A = mem_rd(gb, DE); SET_DE(DE + 1); mem_wr(gb, HL, A);
    CYC(b_+41, b_+45); L = 0x8b; A = mem_rd(gb, DE); SET_DE(DE + 1);
    CALL_C(b_+45, setShortPosition_hook, SYM(setShortPosition), b_+48);
    CALL_C(b_+48, addPositionToPlacedEnemyPositions_hook, SYM(addPositionToPlacedEnemyPositions), b_+51);
    CYC(b_+51, b_+56); L = 0x80; A = H8(hFF8D); mem_wr(gb, HL, A);
    CYCT(b_+56, b_+58);
  }
}
