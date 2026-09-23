#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_MISCELLANEOUS_1, INTERAC_ROSA_HIDING,
// INTERAC_STRANGE_BROTHERS_HIDING), bank 0x15: the floodgate keeper, D4 keyhole, hiding-game and
// strange brothers script helpers.

// wTmpcfc0+1 = the floodgate stage: 3 opened (this room's bit 7), 2 D3 entrance drained (room
// $81's bit 7), 1 keeper talked to (bit 5), else 0.
void s_floodgateKeeper_checkStage_hook(GB *gb) {
  BASE(floodgateKeeper_checkStage);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+5, b_+7); A = 0x03;
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto store; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(wGroup0RoomFlags + 0x81); // ROOM_SEASONS_081
  CYC(b_+12, b_+14); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+14, b_+16); A = 0x02;
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto store; }
  CYC(b_+16, b_+18);
  CALL_C(b_+18, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+23); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(b_+23, b_+25); A = 0x01;
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto store; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); A = alu_dec8(gb, A);
store:
  CYC(b_+28, b_+31); mem_wr(gb, wTmpcfc0 + 0x01, A);
  RET(b_+31); return;
}

void s_d4Keyhole_setState0eDisableAllSorts_hook(GB *gb) {
  BASE(d4Keyhole_setState0eDisableAllSorts);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0e;
  CYC(b_+2, b_+5); mem_wr(gb, wLinkForceState, A);
  TAIL_S(d4KeyHolw_disableAllSorts);
}

// Disables the menu, wcc8b and every object, and keeps this object updating.
void s_d4KeyHolw_disableAllSorts_hook(GB *gb) {
  BASE(d4KeyHolw_disableAllSorts);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+5, b_+8); mem_wr(gb, wcc8b, A);
  CYC(b_+8, b_+10); A = 0xff;
  CYC(b_+10, b_+13); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+13, b_+16);
  TAIL(interactionSetAlwaysUpdateBit);
}

void s_floodgate_disableObjectsScreenTransition_hook(GB *gb) {
  BASE(floodgate_disableObjectsScreenTransition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x11;
  CYC(b_+2, b_+5); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+5, b_+8); mem_wr(gb, wDisabledObjects, A);
  RET(b_+8); return;
}

void s_floodgate_enableObjects_hook(GB *gb) {
  BASE(floodgate_enableObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+4, b_+7); mem_wr(gb, wSwitchState, A);
  RET(b_+7); return;
}

// Hides every bomb item and marks it (var2f bit 5).
void s_strangeBrothersFunc_15_5d9a_hook(GB *gb) {
  BASE(strangeBrothersFunc_15_5d9a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); H = 0xd7; // FIRST_DYNAMIC_ITEM_INDEX
  for (;;) {
    CYC(b_+2, b_+4); L = 0x01; // Item.id
    CYC(b_+4, b_+5); A = mem_rd(gb, HL);
    CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ITEM_BOMB
    if (!(F & FZ)) { CYCT(b_+7, b_+9); goto next; }
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+11); L = 0x1a; // Item.visible
    CYC(b_+11, b_+12); mem_wr(gb, HL, A);
    CYC(b_+12, b_+14); L = 0x2f; // Item.var2f
    CYC(b_+14, b_+16); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
next:
    CYC(b_+16, b_+17); H = alu_inc8(gb, H);
    CYC(b_+17, b_+18); A = H;
    CYC(b_+18, b_+20); alu_cp(gb, 0xdc); // LAST_DYNAMIC_ITEM_INDEX+1
    if (F & FC) { CYCT(b_+20, b_+22); continue; }
    CYC(b_+20, b_+22);
    break;
  }
  RET(b_+22); return;
}

void s_subrosianHiding_store02Intocc9e_hook(GB *gb) {
  BASE(subrosianHiding_store02Intocc9e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+5); mem_wr(gb, wcc84, A);
  RET(b_+5); return;
}

void s_rosaHiding_hidingFinishedSetInitialRoomsFlags_hook(GB *gb) {
  BASE(rosaHiding_hidingFinishedSetInitialRoomsFlags);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wGroup0RoomFlags + 0xcb); // ROOM_SEASONS_0cb
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wcc84, A);
  CYC(b_+9, b_+12); mem_wr(gb, wcc85, A);
  RET(b_+12); return;
}

// The strange brothers' game reward: the feather the first time (room flag bit 5 clear), then
// gasha seeds, a random ring at the eighth play, or an ore chunk.
void s_strangeBrothersFunc_15_5dc4_hook(GB *gb) {
  BASE(strangeBrothersFunc_15_5dc4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x1e;
  CALL_C(b_+2, addToGashaMaturity_hook, SYM(addToGashaMaturity), b_+5);
  CYC(b_+5, b_+8); SET_HL(wNumTimesPlayedStrangeBrothersGame);
  CALL_C(b_+8, incHlRefWithCap_hook, SYM(incHlRefWithCap), b_+11);
  CALL_C(b_+11, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+14);
  CYC(b_+14, b_+16); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+16, b_+18); TAIL_S(strangeBrothersFunc_15_5ddb); }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); SET_BC(0x1702); // TREASURE_FEATHER, subid 2
  CYCT(b_+21, b_+23);
  TAIL_S(strangeBrothersFunc_15_5e00);
}

// strangeBrothersFunc_15_5ddb from its +14: a gasha seed treasure.
static void brothers_gasha_seed(GB *gb) {
  BASE(strangeBrothersFunc_15_5ddb);
  CYC(b_+14, b_+17); SET_BC(0x3402); // TREASURE_GASHA_SEED, subid 2
  CYCT(b_+17, b_+19);
  TAIL_S(strangeBrothersFunc_15_5e00);
}

void s_strangeBrothersFunc_15_5ddb_hook(GB *gb) {
  BASE(strangeBrothersFunc_15_5ddb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wNumTimesPlayedStrangeBrothersGame);
  CYC(b_+3, b_+5); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+5, b_+7); TAIL_S(strangeBrothersFunc_15_5dee); }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, getRandomNumber_hook, SYM(getRandomNumber), b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x60);
  if (!(F & FC)) { CYCT(b_+12, b_+14); TAIL_S(strangeBrothersFunc_15_5e13); }
  CYC(b_+12, b_+14);
  brothers_gasha_seed(gb); return;
}

// The eighth play: a ring not yet obtained (or a gasha seed when all four are), and the
// global flag.
void s_strangeBrothersFunc_15_5dee_hook(GB *gb) {
  BASE(strangeBrothersFunc_15_5dee);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_15_5e20_hook, SYM(seasonsFunc_15_5e20), b_+3);
  if (F & FC) { CYCT(b_+3, b_+5); brothers_gasha_seed(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); C = 0x03;
  CALL_C(b_+7, createRingTreasure_hook, SYM(createRingTreasure), b_+10);
  CALL_C(b_+10, s_strangeBrothersFunc_15_5e0a_hook, SYM(strangeBrothersFunc_15_5e0a), b_+13);
  CYC(b_+13, b_+15); A = 0x14; // GLOBALFLAG_S_14
  CYC(b_+15, b_+18);
  TAIL(setGlobalFlag);
}

// A treasure b (subid c) at ($48,$28).
void s_strangeBrothersFunc_15_5e00_hook(GB *gb) {
  BASE(strangeBrothersFunc_15_5e00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, B);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); mem_wr(gb, HL, C);
  TAIL_S(strangeBrothersFunc_15_5e0a);
}

void s_strangeBrothersFunc_15_5e0a_hook(GB *gb) {
  BASE(strangeBrothersFunc_15_5e0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0x4b; // Interaction.yh
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x48);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x28);
  RET(b_+8); return;
}

// An ore chunk dig spot (INTERAC_MISCELLANEOUS_1 subid 9, var03 set) at ($48,$28).
void s_strangeBrothersFunc_15_5e13_hook(GB *gb) {
  BASE(strangeBrothersFunc_15_5e13);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x6b); // INTERAC_MISCELLANEOUS_1
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x09);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYCT(b_+11, b_+13);
  TAIL_S(strangeBrothersFunc_15_5e0a);
}

// b = a ring from @table_5e4a that Link does not have yet, starting at a random one; carry
// set (b = $80) when he has all four.
void s_seasonsFunc_15_5e20_hook(GB *gb) {
  BASE(seasonsFunc_15_5e20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+8); B = 0x04;
  for (;;) {
    CYC(b_+8, b_+9); push_effect(gb, BC);
    CYC(b_+9, b_+10); A = C;
    CYC(b_+10, b_+13); SET_BC(b_+42); // @table_5e4a
    CALL_C(b_+13, addAToBc_hook, SYM(addAToBc), b_+16);
    CYC(b_+16, b_+17); A = mem_rd(gb, BC);
    CYC(b_+17, b_+20); SET_HL(wRingsObtained);
    CALL_C(b_+20, checkFlag_hook, SYM(checkFlag), b_+23);
    if (F & FZ) { CYCT(b_+23, b_+25); goto found; }
    CYC(b_+23, b_+25);
    CYC(b_+25, b_+26); SET_BC(pop_effect(gb));
    CYC(b_+26, b_+27); A = C;
    CYC(b_+27, b_+28); A = alu_inc8(gb, A);
    CYC(b_+28, b_+30); alu_and(gb, 0x03);
    CYC(b_+30, b_+31); C = A;
    CYC(b_+31, b_+32); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+32, b_+34); continue; }
    CYC(b_+32, b_+34);
    break;
  }
  CYC(b_+34, b_+36); B = 0x80;
  CYC(b_+36, b_+37); alu_scf(gb);
  RET(b_+37); return;
found:
  CYC(b_+38, b_+39); A = mem_rd(gb, BC);
  CYC(b_+39, b_+40); SET_BC(pop_effect(gb));
  CYC(b_+40, b_+41); B = A;
  RET(b_+41); return;
}

// A detection helper part tied to this interaction, at its position.
void s_subrosianHiding_createDetectionHelper_hook(GB *gb) {
  BASE(subrosianHiding_createDetectionHelper);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x0e); // PART_DETECTION_HELPER
  CYC(b_+6, b_+8); L = 0xd6; // Part.relatedObj1
  CYC(b_+8, b_+10); A = 0x40; // Interaction.start
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, HL, D);
  CYC(b_+12, b_+15);
  TAIL(objectCopyPosition);
}
