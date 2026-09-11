#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x12, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x12, (from), (to), true)

static void read2_bytes(GB *gb, uint16_t dst, uint16_t src) {
  CYC(0x580d, 0x580f); uint8_t value = mem_rd(gb, src);
  CYC(0x580f, 0x5810); mem_wr(gb, dst, value);
  CYC(0x5810, 0x5812); value = mem_rd(gb, src + 1);
  CYC(0x5812, 0x5813); mem_wr(gb, dst + 1, value);
  CYC(0x5813, 0x5814);
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
  CYC(0x5814, 0x5816); uint8_t value = mem_rd(gb, src);
  CYC(0x5816, 0x5817); mem_wr(gb, dst, value); SET_HL(HL + 1);
  CYC(0x5817, 0x5818); L = alu_inc8(gb, L);
  CYC(0x5818, 0x581a); value = mem_rd(gb, src + 1);
  CYC(0x581a, 0x581b); mem_wr(gb, (uint16_t)((dst & 0xff00) | ((dst + 2) & 0xff)), value);
  CYC(0x581b, 0x581c);
}

void readCoordinates_hook(GB *gb) {
  uint16_t src = DE, dst = HL;
  read_coordinates(gb, dst, src);
  A = mem_rd(gb, src + 1);
  SET_DE(src + 2);
  ret_effect(gb);
}

void decEnemyCounterIfApplicable_hook(GB *gb) {
  CYC(0x581c, 0x581e); A = H8(hFF8B);
  alu_and(gb, 0x02);
  if (F & FZ) { CYCT(0x581e, 0x5821); ret_effect(gb); return; }
  CYC(0x581e, 0x5821); A = W8(wNumEnemies);
  CYC(0x5821, 0x5825); A = alu_dec8(gb, A);
  CYC(0x5825, 0x5829); W8(wNumEnemies) = A;
  ret_effect(gb);
}

void addPositionToPlacedEnemyPositions_hook(GB *gb) {
  uint16_t saved_hl = HL;
  CYC(0x5829, 0x582a); push_effect(gb, saved_hl);
  CYC(0x582a, 0x582d); A = W8(wEnemyPlacement_numEnemies);
  CYC(0x582d, 0x5830); SET_HL(wEnemyPlacement_placedEnemyPositions);
  CYC(0x5830, 0x5831);
  burn_rom(gb, 0, 0x0010, 0x0012, false); alu_add(gb, L); L = A;
  if (F & FC) { burn_rom(gb, 0, 0x0012, 0x0015, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 0x0012, 0x0013, true);
  CYC(0x5831, 0x5832); mem_wr(gb, HL, C);
  CYC(0x5832, 0x5835); A = W8(wEnemyPlacement_numEnemies);
  CYC(0x5835, 0x5836); A = alu_inc8(gb, A);
  CYC(0x5836, 0x5838); alu_and(gb, 0x0f);
  CYC(0x5838, 0x583b); W8(wEnemyPlacement_numEnemies) = A;
  CYC(0x583b, 0x583c); SET_HL(pop_effect(gb));
  CYC(0x583c, 0x583d);
  ret_effect(gb);
}

void assignRandomPositionToEnemy_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x583d, getRandomPositionForEnemy_b00_hook, 0x3219, 0x5840);
  if (F & FC) { CYCT(0x5840, 0x5841); ret_effect(gb); return; }
  CYC(0x5840, 0x5841);
  CYC(0x5841, 0x5845); C = W8(wEnemyPlacement_enemyPos);
  CALL_C(0x5845, addPositionToPlacedEnemyPositions_hook, 0x5829, 0x5848);
  CYC(0x5848, 0x584a); H = H8(hFF91);
  CYC(0x584a, 0x584d); L = 0x8b;
  CALL_C(0x584d, setShortPosition_paramC_hook, 0x20b9, 0x5850);
  CYC(0x5850, 0x5851); alu_xor(gb, A);
  ret_effect(gb);
}

void checkEnemyKilled_hook(GB *gb) {
  CYC(0x5852, 0x5855); A = W8(wEnemyPlacement_numKillableEnemies);
  CYC(0x5855, 0x5857); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(0x5857, 0x5870); goto done; }
  CYC(0x5857, 0x5859); A = alu_inc8(gb, A);
  CYC(0x5859, 0x585d); W8(wEnemyPlacement_numKillableEnemies) = A;
  CYC(0x585d, 0x5862); SET_HL(bitTable + A);
  CYC(0x5862, 0x5866); A = W8(wEnemyPlacement_killedEnemiesBitset); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5866, 0x5867); ret_effect(gb); return; }
  CYC(0x5866, 0x5867);
  CYC(0x5867, 0x586a); A = W8(wEnemyPlacement_numKillableEnemies);
  CYC(0x586a, 0x586c); alu_swap_a(gb);
  CYC(0x586c, 0x5870); alu_or(gb, 0x01); H8(hFF8D) = A;
done:
  CYC(0x5870, 0x5871); alu_scf(gb);
  ret_effect(gb);
}

void parseObjectData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55b7, 0x55b8); alu_xor(gb, A);
  CYC(0x55b8, 0x55bb); W8(wNumEnemies) = A;
  CYC(0x55bb, 0x55be); W8(wTmpcfc0) = A;
  CYC(0x55be, 0x55c1); SET_HL(0xcec0);
  CYC(0x55c1, 0x55c3); B = 0x20;
  CALL_C(0x55c3, clearMemory_hook, 0x046f, 0x55c6);
  CALL_C(0x55c6, addRoomToEnemiesKilledList_b00_hook, 0x3209, 0x55c9);
  CALL_C(0x55c9, generateRandomBuffer_b00_hook, 0x3215, 0x55cc);
  CYC(0x55cc, 0x55cf); SET_HL(0x4315);
  CYC(0x55cf, 0x55d1); E = 0x15;
  CALL_C(0x55d1, interBankCall_hook, 0x008a, 0x55d4);
  parseGivenObjectData_b12(gb);
}

void func_55f8_hook(GB *gb) {
  CYC(0x55f8, 0x55fb);
  parseGivenObjectData_b12(gb);
}

void parseGivenObjectData_hl_hook(GB *gb) {
  CYC(0x55fa, 0x55fb); E = L;
  CYC(0x55fb, 0x55fc); D = H;
  CYC(0x55fc, 0x55ff);
  parseGivenObjectData_b12(gb);
}

void skipToOpEnd_2byte_hook(GB *gb) {
  for (;;) {
    CYC(0x5660, 0x5662); SET_DE(DE + 2);
    CYC(0x5662, 0x5663); A = mem_rd(gb, DE);
    CYC(0x5663, 0x5665); alu_cp(gb, 0xf0);
    if (F & FC) { CYCT(0x5665, 0x5668); continue; }
    CYC(0x5665, 0x5668);
    CYC(0x5668, 0x566b);
    parseGivenObjectData_b12(gb);
    return;
  }
}

void skipToOpEnd_4byte_hook(GB *gb) {
  for (;;) {
    CYC(0x567d, 0x5681); SET_DE(DE + 4);
    CYC(0x5681, 0x5682); A = mem_rd(gb, DE);
    CYC(0x5682, 0x5684); alu_cp(gb, 0xf0);
    if (F & FC) { CYCT(0x5684, 0x5687); continue; }
    CYC(0x5684, 0x5687);
    CYC(0x5687, 0x568a);
    parseGivenObjectData_b12(gb);
    return;
  }
}

void checkSkipPointer_hook(GB *gb) {
  CYC(0x568a, 0x568d); A = W8(wcc05);
  CYC(0x568d, 0x568f); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(0x568f, 0x5690); ret_effect(gb); return; }
  CYC(0x568f, 0x5690);
  CYC(0x5690, 0x5693); A = W8(wcc85);
  CYC(0x5693, 0x5695); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x5695, 0x5697);
    CYC(0x56a5, 0x56a6); alu_or(gb, A);
    if (F & FZ) { CYCT(0x56a6, 0x56a8); goto done; }
    CYC(0x56a6, 0x56a8);
    CYC(0x56a8, 0x56a9); alu_xor(gb, A);
    CYC(0x56a9, 0x56aa); ret_effect(gb);
    return;
  }
  CYC(0x5695, 0x5697);
  CYC(0x5697, 0x5699); alu_and(gb, 0x03);
  CYC(0x5699, 0x569a); B = A;
  CYC(0x569a, 0x569b); alu_xor(gb, A);
  CYC(0x569b, 0x569e); W8(wcc85) = A;
  CYC(0x569e, 0x56a1); A = W8(wScreenTransitionDirection);
  CYC(0x56a1, 0x56a2); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x56a2, 0x56a3); ret_effect(gb); return; }
  CYC(0x56a2, 0x56a3);
done:
  CYC(0x56a3, 0x56a4); alu_or(gb, D);
  CYC(0x56a4, 0x56a5);
  ret_effect(gb);
}

void skipPointer_hook(GB *gb) {
  CYC(0x56aa, 0x56ac); SET_DE(DE + 2);
  CYC(0x56ac, 0x56af);
  parseGivenObjectData_b12(gb);
}

void parsePointer_hook(GB *gb) {
  CYC(0x56af, 0x56b1); L = E; H = D;
  CYC(0x56b1, 0x56b3); SET_DE(DE + 2);
  CYC(0x56b3, 0x56b4); push_effect(gb, DE);
  CYC(0x56b4, 0x56b5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x56b5, 0x56b6); D = mem_rd(gb, HL);
  CYC(0x56b6, 0x56b7); E = A;
  CYC(0x56b7, 0x56ba);
  parseGivenObjectData_b12(gb);
}

void continueObjectLoopIfOpDone_hook(GB *gb) {
  CYC(0x5805, 0x5806); A = mem_rd(gb, DE);
  CYC(0x5806, 0x5808); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(0x5808, 0x5809); ret_effect(gb); return; }
  CYC(0x5808, 0x5809);
  CYC(0x5809, 0x580a); SET_BC(pop_effect(gb));
  CYC(0x580a, 0x580d);
  parseGivenObjectData_b12(gb);
}

void objectDataOp1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(0x5653, continueObjectLoopIfOpDone_hook, 0x5805, 0x5656);
    CALL_C(0x5656, getFreeInteractionSlot_hook, 0x3aef, 0x5659);
    if (!(F & FZ)) { CYCT(0x5659, 0x565b); skipToOpEnd_2byte_hook(gb); return; }
    CYC(0x5659, 0x565b);
    CALL_C(0x565b, read2Bytes_hook, 0x580d, 0x565e);
    CYCT(0x565e, 0x5660);
  }
}

void objectDataOp2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(0x566b, continueObjectLoopIfOpDone_hook, 0x5805, 0x566e);
    CALL_C(0x566e, getFreeInteractionSlot_hook, 0x3aef, 0x5671);
    if (!(F & FZ)) { CYCT(0x5671, 0x5673); skipToOpEnd_4byte_hook(gb); return; }
    CYC(0x5671, 0x5673);
    CALL_C(0x5673, read2Bytes_hook, 0x580d, 0x5676);
    CYC(0x5676, 0x5678); L = 0x4b;
    CALL_C(0x5678, readCoordinates_hook, 0x5814, 0x567b);
    CYCT(0x567b, 0x567d);
  }
}

void objectDataOp3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x56ba, checkSkipPointer_hook, 0x568a, 0x56bd);
  if (F & FZ) { CYCT(0x56bd, 0x56bf); skipPointer_hook(gb); return; }
  CYC(0x56bd, 0x56bf);
  CYC(0x56bf, 0x56c1);
  parsePointer_hook(gb);
}

void objectDataOp4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x56c1, checkSkipPointer_hook, 0x568a, 0x56c4);
  if (F & FZ) { CYCT(0x56c4, 0x56c6); skipPointer_hook(gb); return; }
  CYC(0x56c4, 0x56c6);
  CALL_C(0x56c6, getThisRoomFlags_hook, 0x197d, 0x56c9);
  CYC(0x56c9, 0x56cb); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x56cb, 0x56cd); skipPointer_hook(gb); return; }
  CYC(0x56cb, 0x56cd);
  CYC(0x56cd, 0x56cf);
  parsePointer_hook(gb);
}

void objectDataOp5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x56cf, checkSkipPointer_hook, 0x568a, 0x56d2);
  if (F & FZ) { CYCT(0x56d2, 0x56d4); skipPointer_hook(gb); return; }
  CYC(0x56d2, 0x56d4);
  CALL_C(0x56d4, getThisRoomFlags_hook, 0x197d, 0x56d7);
  CYC(0x56d7, 0x56d9); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x56d9, 0x56db); skipPointer_hook(gb); return; }
  CYC(0x56d9, 0x56db);
  CYC(0x56db, 0x56dd);
  parsePointer_hook(gb);
}

void objectDataOp8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x577a, 0x577b); A = mem_rd(gb, DE);
    CYC(0x577b, 0x577d); alu_bit(gb, 7, A);
    if (!(F & FZ)) { CYCT(0x577d, 0x5780); parseGivenObjectData_b12(gb); return; }
    CYC(0x577d, 0x5780);
    CALL_C(0x5780, getFreePartSlot_hook, 0x3e8e, 0x5783);
    if (!(F & FZ)) { CYCT(0x5783, 0x5786); goto skip; }
    CYC(0x5783, 0x5786);
    CALL_C(0x5786, read2Bytes_hook, 0x580d, 0x5789);
    CYC(0x5789, 0x578a); A = mem_rd(gb, DE);
    CYC(0x578a, 0x578b); C = A;
    CYC(0x578b, 0x578c); SET_DE(DE + 1);
    CYC(0x578c, 0x578e); L = 0xcb;
    CALL_C(0x578e, setShortPosition_hook, 0x20b8, 0x5791);
    CALL_C(0x5791, addPositionToPlacedEnemyPositions_hook, 0x5829, 0x5794);
    CYCT(0x5794, 0x5796);
    continue;
skip:
    CYC(0x5796, 0x5799); SET_DE(DE + 3);
    CYCT(0x5799, 0x579b);
  }
}
