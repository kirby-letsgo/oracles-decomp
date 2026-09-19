#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t miscPuzzles_jump_table(GB *gb) {
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

static void miscPuzzles_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void miscPuzzles_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void miscPuzzles_subid00_hook(GB *gb);
void miscPuzzles_subid01_hook(GB *gb);
void miscPuzzles_subid02_hook(GB *gb);
void miscPuzzles_subid03_hook(GB *gb);
void miscPuzzles_verifyTilesAtPositions_hook(GB *gb);
void miscPuzzles_subid04_hook(GB *gb);
void miscPuzzles_subid05_hook(GB *gb);
void miscPuzzles_subid06_hook(GB *gb);
void miscPuzzles_subid07_hook(GB *gb);
void miscPuzzles_subid08_hook(GB *gb);
void miscPuzzles_subid09_hook(GB *gb);
void miscPuzzles_subid0a_hook(GB *gb);
void miscPuzzles_subid0b_hook(GB *gb);
void miscPuzzles_dropSmallKeyHere_hook(GB *gb);
void miscPuzzles_subid0c_hook(GB *gb);
void miscPuzzles_subid0d_hook(GB *gb);
void miscPuzzles_subid0e_hook(GB *gb);
void miscPuzzles_subid0f_hook(GB *gb);
void miscPuzzles_subid10_hook(GB *gb);
void miscPuzzles_subid11_hook(GB *gb);
void miscPuzzles_setScriptAndIncState_hook(GB *gb);
void miscPuzzles_subid12_hook(GB *gb);
void miscPuzzles_subid13_hook(GB *gb);
void miscPuzzles_subid14_hook(GB *gb);
void miscPuzzles_subid15_hook(GB *gb);
void miscPuzzles_subid16_hook(GB *gb);
void miscPuzzles_subid17_hook(GB *gb);
void miscPuzzles_subid18_hook(GB *gb);
void miscPuzzles_subid19_hook(GB *gb);
void miscPuzzles_subid1a_hook(GB *gb);
void miscPuzzles_subid1b_hook(GB *gb);
void miscPuzzles_subid1c_hook(GB *gb);
void miscPuzzles_subid1d_hook(GB *gb);
void miscPuzzles_subid1e_hook(GB *gb);
void miscPuzzles_subid1f_hook(GB *gb);
void miscPuzzles_subid20_hook(GB *gb);
void miscPuzzles_subid21_hook(GB *gb);
void miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook(GB *gb);
void miscPuzzles_deleteSelfOrIncStateIfItemFlagSet_hook(GB *gb);
void miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(GB *gb);
void miscPuzzles_deleteSelfOrIncStateIfRoomFlag6Set_hook(GB *gb);

// INTERAC_MISC_PUZZLES
void interactionCode90_hook(GB *gb) {
  CYC(0x6cae, 0x6cb0); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6cb0, 0x6cb1); A = mem_rd(gb, DE);
  {
    CYC(0x6cb1, 0x6cb2); push_effect(gb, 0x6cb2);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == 0x6cf6) { miscPuzzles_subid00_hook(gb); return; }
    if (target == 0x6d6d) { miscPuzzles_subid01_hook(gb); return; }
    if (target == 0x6d8a) { miscPuzzles_subid02_hook(gb); return; }
    if (target == 0x6db2) { miscPuzzles_subid03_hook(gb); return; }
    if (target == 0x6dde) { miscPuzzles_subid04_hook(gb); return; }
    if (target == 0x6e25) { miscPuzzles_subid05_hook(gb); return; }
    if (target == 0x6e8c) { miscPuzzles_subid07_hook(gb); return; }
    if (target == 0x6f48) { miscPuzzles_subid08_hook(gb); return; }
    if (target == 0x6f58) { miscPuzzles_subid09_hook(gb); return; }
    if (target == 0x6f68) { miscPuzzles_subid0a_hook(gb); return; }
    if (target == 0x700c) { miscPuzzles_subid0b_hook(gb); return; }
    if (target == 0x7068) { miscPuzzles_subid0c_hook(gb); return; }
    if (target == 0x7088) { miscPuzzles_subid0d_hook(gb); return; }
    if (target == 0x70eb) { miscPuzzles_subid0e_hook(gb); return; }
    if (target == 0x7123) { miscPuzzles_subid0f_hook(gb); return; }
    if (target == 0x7138) { miscPuzzles_subid10_hook(gb); return; }
    if (target == 0x7143) { miscPuzzles_subid11_hook(gb); return; }
    if (target == 0x7165) { miscPuzzles_subid12_hook(gb); return; }
    if (target == 0x7178) { miscPuzzles_subid13_hook(gb); return; }
    if (target == 0x718b) { miscPuzzles_subid14_hook(gb); return; }
    if (target == 0x71bd) { miscPuzzles_subid15_hook(gb); return; }
    if (target == 0x71dd) { miscPuzzles_subid16_hook(gb); return; }
    if (target == 0x7209) { miscPuzzles_subid17_hook(gb); return; }
    if (target == 0x7219) { miscPuzzles_subid18_hook(gb); return; }
    if (target == 0x722f) { miscPuzzles_subid19_hook(gb); return; }
    if (target == 0x72b7) { miscPuzzles_subid1a_hook(gb); return; }
    if (target == 0x72d9) { miscPuzzles_subid1b_hook(gb); return; }
    if (target == 0x72de) { miscPuzzles_subid1c_hook(gb); return; }
    if (target == 0x72e3) { miscPuzzles_subid1d_hook(gb); return; }
    if (target == 0x7308) { miscPuzzles_subid1e_hook(gb); return; }
    if (target == 0x731a) { miscPuzzles_subid1f_hook(gb); return; }
    if (target == 0x7392) { miscPuzzles_subid20_hook(gb); return; }
    miscPuzzles_subid21_hook(gb); return; // target == 0x73a8
  }
}

// Boss key puzzle in D6
void miscPuzzles_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6cf6, 0x6cf8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6cf8, 0x6cf9); A = mem_rd(gb, DE);
  {
    CYC(0x6cf9, 0x6cfa); push_effect(gb, 0x6cfa);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == 0x6d4d) goto state2;
    if (target == 0x6d58) goto state3;
  }

  // miscPuzzles_subid00@state0
  CALL_C(0x6d02, interactionIncState_hook, 0x23e0, 0x6d05);

  // miscPuzzles_subid00@state1
  CYC(0x6d05, 0x6d08); SET_HL(0xccab); // wLever1PullDistance
  CYC(0x6d08, 0x6d0a); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6d0a, 0x6d0c); goto checkAlreadyOpened; } // jr nz
  CYC(0x6d0a, 0x6d0c);
  CYC(0x6d0c, 0x6d0d); L = alu_inc8(gb, L);
  CYC(0x6d0d, 0x6d0f); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6d0f); return; } // ret z
  CYC(0x6d0f, 0x6d10);

checkAlreadyOpened:
  CALL_C(0x6d10, getThisRoomFlags_hook, 0x197d, 0x6d13);
  CYC(0x6d13, 0x6d15); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(0x6d15, 0x6d17); goto alreadyOpened; } // jr nz
  CYC(0x6d15, 0x6d17);
  CALL_C(0x6d17, interactionIncState_hook, 0x23e0, 0x6d1a);
  CYC(0x6d1a, 0x6d1c); L = 0x47; // Interaction.counter2
  CYC(0x6d1c, 0x6d1d); A = mem_rd(gb, HL);
  CYC(0x6d1d, 0x6d1e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d1e, 0x6d20); goto checkRng; } // jr nz
  CYC(0x6d1e, 0x6d20);
  CYC(0x6d20, 0x6d22); mem_wr(gb, HL, 0x01);
  CYC(0x6d22, 0x6d24); goto error; // jr

checkRng:
  CALL_C(0x6d24, getRandomNumber_hook, 0x043e, 0x6d27);
  CYC(0x6d27, 0x6d29); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(0x6d29, 0x6d2c); interactionIncState_hook(gb); return; } // jp z
  CYC(0x6d29, 0x6d2c);

error:
  CYC(0x6d2c, 0x6d2e); A = 0x5a; // SND_ERROR
  CALL_C(0x6d2e, playSound_b00_hook, 0x0c98, 0x6d31);
  CYC(0x6d31, 0x6d34); A = W8(wActiveTilePos);
  CYC(0x6d34, 0x6d37); W8(wWarpDestPos) = A;
  CYC(0x6d37, 0x6d3a); SET_HL(0xcec0); // wTmpcec0
  CYC(0x6d3a, 0x6d3c); B = 0x20;
  CALL_C(0x6d3c, clearMemory_hook, 0x046f, 0x6d3f);
  CYC(0x6d3f, 0x6d42); SET_HL(0x7823); // roomInitialization.generateRandomBuffer
  CYC(0x6d42, 0x6d44); E = 0x02; // bank of roomInitialization
  CALL_C(0x6d44, interBankCall_hook, 0x008a, 0x6d47);
  CYC(0x6d47, 0x6d4a); SET_HL(0x78db); // objectData.objectData78db
  CYC(0x6d4a, 0x6d4d); parseGivenObjectData_b00_hook(gb); return; // jp

state2:
  CYC(0x6d4d, 0x6d50); A = W8(wNumEnemies);
  CYC(0x6d50, 0x6d51); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6d51); return; } // ret nz
  CYC(0x6d51, 0x6d52);
  CYC(0x6d52, 0x6d54); A = 0x01;
  CYC(0x6d54, 0x6d56); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6d56, 0x6d57); mem_wr(gb, DE, A);
  RET(0x6d57); return; // ret

state3:
  CYC(0x6d58, 0x6d5a); A = 0x01;
  CYC(0x6d5a, 0x6d5d); W8(wActiveTriggers) = A;
  CYC(0x6d5d, 0x6d60); SET_HL(0x4f1d); // agesInteractionsBank08.spawnChestAndDeleteSelf
  CYC(0x6d60, 0x6d62); E = 0x08; // bank of agesInteractionsBank08
  CYC(0x6d62, 0x6d65); interBankCall_hook(gb); return; // jp

alreadyOpened:
  CYC(0x6d65, 0x6d67); A = 0x01;
  CYC(0x6d67, 0x6d6a); W8(wActiveTriggers) = A;
  CYC(0x6d6a, 0x6d6d); interactionDelete_hook(gb); return; // jp
}

// Underwater switch hook puzzle in past d6
void miscPuzzles_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d6d, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x6d70);
  CALL_C(0x6d70, miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook, 0x73e8, 0x6d73);
  CYC(0x6d73, 0x6d76); SET_HL(0x6d82); // @diamondPositions
  CALL_C(0x6d76, miscPuzzles_verifyTilesAtPositions_hook, 0x6dcc, 0x6d79);
  if (!(F & FZ)) { RET_TAKEN(0x6d79); return; } // ret nz
  CYC(0x6d79, 0x6d7a);
  CYC(0x6d7a, 0x6d7d); SET_HL(0x4f1d); // agesInteractionsBank08.spawnChestAndDeleteSelf
  CYC(0x6d7d, 0x6d7f); E = 0x08; // bank of agesInteractionsBank08
  CYC(0x6d7f, 0x6d82); interBankCall_hook(gb); return; // jp
}

// Spot to put a rolling colored block on in present d6
void miscPuzzles_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d8a, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x6d8d);
  CALL_C(0x6d8d, objectGetTileAtPosition_hook, 0x1444, 0x6d90);
  CYC(0x6d90, 0x6d92); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(0x6d92, 0x6d93); B = A;
  CYC(0x6d93, 0x6d96); A = W8(wRotatingCubePos);
  CYC(0x6d96, 0x6d97); alu_cp(gb, L);
  if (!(F & FZ)) { RET_TAKEN(0x6d97); return; } // ret nz
  CYC(0x6d97, 0x6d98);
  CYC(0x6d98, 0x6d9b); A = W8(wRotatingCubeColor);
  CYC(0x6d9b, 0x6d9d); alu_and(gb, 0x03);
  CYC(0x6d9d, 0x6d9e); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(0x6d9e); return; } // ret nz
  CYC(0x6d9e, 0x6d9f);
  CYC(0x6d9f, 0x6da0); C = L;
  CYC(0x6da0, 0x6da2); A = 0xa0; // TILEINDEX_STANDARD_FLOOR
  CALL_C(0x6da2, setTile_hook, 0x3a9c, 0x6da5);
  CYC(0x6da5, 0x6da7); B = 0xce; // >wRoomCollisions
  CYC(0x6da7, 0x6da9); A = 0x0f;
  CYC(0x6da9, 0x6daa); mem_wr(gb, BC, A);
  CYC(0x6daa, 0x6dac); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x6dac, playSound_b00_hook, 0x0c98, 0x6daf);
  CYC(0x6daf, 0x6db2); interactionDelete_hook(gb); return; // jp
}

// Chest from solving colored cube puzzle in d6 (related to subid $02)
void miscPuzzles_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6db2, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x6db5);
  CALL_C(0x6db5, miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook, 0x73e8, 0x6db8);
  CYC(0x6db8, 0x6dbb); SET_HL(0x6dc7); // @wantedFloorTiles
  CALL_C(0x6dbb, miscPuzzles_verifyTilesAtPositions_hook, 0x6dcc, 0x6dbe);
  if (!(F & FZ)) { RET_TAKEN(0x6dbe); return; } // ret nz
  CYC(0x6dbe, 0x6dbf);
  CYC(0x6dbf, 0x6dc2); SET_HL(0x4f1d); // agesInteractionsBank08.spawnChestAndDeleteSelf
  CYC(0x6dc2, 0x6dc4); E = 0x08; // bank of agesInteractionsBank08
  CYC(0x6dc4, 0x6dc7); interBankCall_hook(gb); return; // jp
}

// @param hl Pointer to data. First byte is a tile index; then an arbitrary
// number of positions in the room where that tile should be; $ff to
// give a new tile index; $00 to stop.
// @param[out] zflag z if all tiles matched as expected.
void miscPuzzles_verifyTilesAtPositions_hook(GB *gb) {
  CYC(0x6dcc, 0x6dce); B = 0xcf; // >wRoomLayout

newTileIndex:
  CYC(0x6dce, 0x6dcf); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6dcf, 0x6dd0); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6dd0); return; } // ret z
  CYC(0x6dd0, 0x6dd1);
  CYC(0x6dd1, 0x6dd2); E = A;

nextTile:
  CYC(0x6dd2, 0x6dd3); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6dd3, 0x6dd4); C = A;
  CYC(0x6dd4, 0x6dd5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6dd5); return; } // ret z
  CYC(0x6dd5, 0x6dd6);
  CYC(0x6dd6, 0x6dd7); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x6dd7, 0x6dd9); goto newTileIndex; } // jr z
  CYC(0x6dd7, 0x6dd9);
  CYC(0x6dd9, 0x6dda); A = mem_rd(gb, BC);
  CYC(0x6dda, 0x6ddb); alu_cp(gb, E);
  if (!(F & FZ)) { RET_TAKEN(0x6ddb); return; } // ret nz
  CYC(0x6ddb, 0x6ddc);
  CYC(0x6ddc, 0x6dde); goto nextTile; // jr
}

// Floor changer in present D6, triggered by orb
void miscPuzzles_subid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6dde, checkInteractionState_hook, 0x23fe, 0x6de1);
  if (F & FZ) { CYCT(0x6de1, 0x6de3); goto state0; } // jr z
  CYC(0x6de1, 0x6de3);

  // miscPuzzles_subid04@state1
  CYC(0x6de3, 0x6de6); A = W8(wToggleBlocksState);
  CYC(0x6de6, 0x6de7); B = A;
  CYC(0x6de7, 0x6de9); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x6de9, 0x6dea); A = mem_rd(gb, DE);
  CYC(0x6dea, 0x6deb); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(0x6deb); return; } // ret z
  CYC(0x6deb, 0x6dec);
  CYC(0x6dec, 0x6ded); A = B;
  CYC(0x6ded, 0x6dee); mem_wr(gb, DE, A);
  CYC(0x6dee, 0x6df0); A = 0xff;
  CYC(0x6df0, 0x6df3); W8(wDisabledObjects) = A;
  CYC(0x6df3, 0x6df6); W8(wMenuDisabled) = A;
  CYC(0x6df6, 0x6df8); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6df8, 0x6df9); A = mem_rd(gb, DE);
  CYC(0x6df9, 0x6dfa); A = alu_inc8(gb, A);
  CYC(0x6dfa, 0x6dfc); alu_and(gb, 0x01);
  CYC(0x6dfc, 0x6dfd); B = A;
  CYC(0x6dfd, 0x6dfe); mem_wr(gb, DE, A);
  CYC(0x6dfe, 0x6e00); C = 0x05;
  CYC(0x6e00, 0x6e03); push_effect(gb, 0x6e03); goto spawnSubid; // call
afterSpawnSubid1:
  CYC(0x6e03, 0x6e05); C = 0x06;
  CYC(0x6e05, 0x6e08); push_effect(gb, 0x6e08); goto spawnSubid; // call
afterSpawnSubid2:
  CYC(0x6e08, 0x6e0b); SET_HL(0x5766); // bank16.loadD6ChangingFloorPatternToBigBuffer
  CYC(0x6e0b, 0x6e0d); E = 0x16; // bank of bank16
  CALL_C(0x6e0d, interBankCall_hook, 0x008a, 0x6e10);
  RET(0x6e10); return; // ret

spawnSubid:
  CALL_C(0x6e11, getFreeInteractionSlot_hook, 0x3aef, 0x6e14);
  if (!(F & FZ)) {
    RET_TAKEN(0x6e14);
    if (gb->pc == 0x6e03 && gb->sp == sp0_) goto afterSpawnSubid1;
    if (gb->pc == 0x6e08 && gb->sp == sp0_) goto afterSpawnSubid2;
    return;
  } // ret nz
  CYC(0x6e14, 0x6e15);
  CYC(0x6e15, 0x6e17); mem_wr(gb, HL, 0x90); // INTERAC_MISC_PUZZLES
  CYC(0x6e17, 0x6e18); L = alu_inc8(gb, L);
  CYC(0x6e18, 0x6e19); mem_wr(gb, HL, C);
  CYC(0x6e19, 0x6e1a); L = alu_inc8(gb, L);
  CYC(0x6e1a, 0x6e1b); mem_wr(gb, HL, B);
  RET(0x6e1b);
  if (gb->pc == 0x6e03 && gb->sp == sp0_) goto afterSpawnSubid1;
  if (gb->pc == 0x6e08 && gb->sp == sp0_) goto afterSpawnSubid2;
  return; // ret

state0:
  CYC(0x6e1c, 0x6e1f); A = W8(wToggleBlocksState);
  CYC(0x6e1f, 0x6e21); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x6e21, 0x6e22); mem_wr(gb, DE, A);
  CYC(0x6e22, 0x6e25); interactionIncState_hook(gb); return; // jp
}

// Helpers for floor changer (subid $04). subid05 and subid06 both target 0a:6e25 in
// interactionCode90's jump table -- identical code, so subid05 just hands off to subid06.
void miscPuzzles_subid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6e25, 0x6e27); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6e27, 0x6e28); A = mem_rd(gb, DE);
  CYC(0x6e28, 0x6e29); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6e29, 0x6e2b); goto substate1; } // jr nz
  CYC(0x6e29, 0x6e2b);

  // miscPuzzles_subid06@substate0
  CYC(0x6e2b, 0x6e2d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6e2d, 0x6e2e); A = mem_rd(gb, DE);
  CYC(0x6e2e, 0x6e30); alu_sub(gb, 0x05);
  CYC(0x6e30, 0x6e31); alu_add(gb, A); // add a
  CYC(0x6e31, 0x6e34); SET_HL(0x6e3f); // @data: var30/var31/var32/var33 init table (pure ROM data)
  CYC(0x6e34, 0x6e35); miscPuzzles_add_double_index(gb, 0x6e35);
  CYC(0x6e35, 0x6e37); B = 0x04;
  CYC(0x6e37, 0x6e39); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CALL_C(0x6e39, copyMemory_hook, 0x0486, 0x6e3c);
  CYC(0x6e3c, 0x6e3f); interactionIncSubstate_hook(gb); return; // jp

substate1:
  CYC(0x6e47, 0x6e49); E = INTERACTION_BASE + 0x33; // Interaction.var33
  CYC(0x6e49, 0x6e4a); A = mem_rd(gb, DE);
  CYC(0x6e4a, 0x6e4b); L = A;
  CYC(0x6e4b, 0x6e4d); H = 0xc3; // >wBigBuffer

nextTile:
  CYC(0x6e4d, 0x6e4e); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6e4e, 0x6e4f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6e4f, 0x6e51); goto deleteSelf; } // jr z
  CYC(0x6e4f, 0x6e51);
  CYC(0x6e51, 0x6e53); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x6e53, 0x6e55); goto setTileLbl; } // jr nz
  CYC(0x6e53, 0x6e55);

  CYC(0x6e55, 0x6e57); E = INTERACTION_BASE + 0x32; // Interaction.var32
  CYC(0x6e57, 0x6e58); A = mem_rd(gb, DE);
  CYC(0x6e58, 0x6e59); B = A;
  CYC(0x6e59, 0x6e5b); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x6e5b, 0x6e5c); A = mem_rd(gb, DE);
  CYC(0x6e5c, 0x6e5d); alu_add(gb, B);
  CYC(0x6e5d, 0x6e5e); mem_wr(gb, DE, A);
  CYC(0x6e5e, 0x6e60); E = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x6e60, 0x6e61); A = mem_rd(gb, DE);
  CYC(0x6e61, 0x6e62); alu_cpl(gb);
  CYC(0x6e62, 0x6e63); A = alu_inc8(gb, A);
  CYC(0x6e63, 0x6e64); mem_wr(gb, DE, A);
  CYC(0x6e64, 0x6e67); push_effect(gb, 0x6e67); goto nextRow;
afterNextRow_fromNextTile:
  CYC(0x6e67, 0x6e69); goto nextTile; // jr

setTileLbl:
  CYC(0x6e69, 0x6e6b); H8(hFF8B) = A; // ldh ($ff8b),a
  CYC(0x6e6b, 0x6e6d); E = INTERACTION_BASE + 0x33; // Interaction.var33
  CYC(0x6e6d, 0x6e6e); A = L;
  CYC(0x6e6e, 0x6e6f); mem_wr(gb, DE, A);
  CYC(0x6e6f, 0x6e72); push_effect(gb, 0x6e72); goto nextRow;
afterNextRow_fromSetTile:
  CYC(0x6e72, 0x6e74); A = H8(hFF8B); // ldh a,($ff8b)
  CYC(0x6e74, 0x6e77); setTile_hook(gb); return; // jp

deleteSelf:
  CYC(0x6e82, 0x6e83); alu_xor(gb, A);
  CYC(0x6e83, 0x6e86); W8(wDisabledObjects) = A;
  CYC(0x6e86, 0x6e89); W8(wMenuDisabled) = A;
  CYC(0x6e89, 0x6e8c); interactionDelete_hook(gb); return; // jp

  // [var30] += [var31]; shared by @nextTile and @setTile, both at top-level depth (sp0_).
nextRow:
  CYC(0x6e77, 0x6e79); E = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x6e79, 0x6e7a); A = mem_rd(gb, DE);
  CYC(0x6e7a, 0x6e7b); B = A;
  CYC(0x6e7b, 0x6e7d); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x6e7d, 0x6e7e); A = mem_rd(gb, DE);
  CYC(0x6e7e, 0x6e7f); C = A;
  CYC(0x6e7f, 0x6e80); alu_add(gb, B);
  CYC(0x6e80, 0x6e81); mem_wr(gb, DE, A);
  RET(0x6e81);
  if (gb->pc == 0x6e67 && gb->sp == sp0_) goto afterNextRow_fromNextTile;
  if (gb->pc == 0x6e72 && gb->sp == sp0_) goto afterNextRow_fromSetTile;
  return; // ret
}

void miscPuzzles_subid05_hook(GB *gb) {
  miscPuzzles_subid06_hook(gb);
}

// Wall retraction event after lighting torches in past d6
void miscPuzzles_subid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6e8c, checkInteractionState_hook, 0x23fe, 0x6e8f);
  if (F & FZ) { CYCT(0x6e8f, 0x6e91); goto state0; } // jr z
  CYC(0x6e8f, 0x6e91);

// miscPuzzles_subid07@state1
  CALL_C(0x6e91, checkLinkVulnerable_hook, 0x1d28, 0x6e94);
  if (!(F & FC)) { RET_TAKEN(0x6e94); return; } // ret nc
  CYC(0x6e94, 0x6e95);
  CYC(0x6e95, 0x6e98); push_effect(gb, 0x6e98); goto checkLitTorches;
afterCheckLitTorches_fromState1:
  CYC(0x6e98, 0x6e9a); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6e9a, 0x6e9b); A = mem_rd(gb, DE);
  CYC(0x6e9b, 0x6e9c); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(0x6e9c); return; } // ret z
  CYC(0x6e9c, 0x6e9d);
  CYC(0x6e9d, 0x6e9e); A = B;
  CYC(0x6e9e, 0x6e9f); mem_wr(gb, DE, A);
  CYC(0x6e9f, 0x6ea1); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6ea1, 0x6ea2); A = mem_rd(gb, DE);
  CYC(0x6ea2, 0x6ea5); SET_HL(0x6eef); // @torchLightOrder (pure ROM data table)
  CYC(0x6ea5, 0x6ea6); miscPuzzles_addAToHl_from_rst(gb, 0x6ea6);
  CYC(0x6ea6, 0x6ea7); A = mem_rd(gb, HL);
  CYC(0x6ea7, 0x6ea8); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x6ea8, 0x6eaa); goto litWrongTorch; } // jr nz
  CYC(0x6ea8, 0x6eaa);
  CYC(0x6eaa, 0x6eab); A = mem_rd(gb, DE);
  CYC(0x6eab, 0x6ead); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x6ead, 0x6eb0); interactionIncSubstate_hook(gb); return; } // jp c
  CYC(0x6ead, 0x6eb0);

  // Lit all torches
  CYC(0x6eb0, 0x6eb2); A = 0x6f; // $ff ~ (DISABLE_ITEMS | DISABLE_ALL_BUT_INTERACTIONS)
  CYC(0x6eb2, 0x6eb5); W8(wDisabledObjects) = A;
  CYC(0x6eb5, 0x6eb8); W8(wMenuDisabled) = A;
  CYC(0x6eb8, 0x6eba); A = 0x0b; // CUTSCENE_WALL_RETRACTION
  CYC(0x6eba, 0x6ebd); W8(wCutsceneTrigger) = A;
  CALL_C(0x6ebd, getThisRoomFlags_hook, 0x197d, 0x6ec0);
  CYC(0x6ec0, 0x6ec2); L = 0x25; // <ROOM_AGES_525
  CYC(0x6ec2, 0x6ec4); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)
  CYC(0x6ec4, 0x6ec7); interactionDelete_hook(gb); return; // jp

litWrongTorch:
  CYC(0x6ec7, 0x6ec8); alu_xor(gb, A);
  CYC(0x6ec8, 0x6ec9); mem_wr(gb, DE, A);
  CYC(0x6ec9, 0x6ecb); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6ecb, 0x6ecc); mem_wr(gb, DE, A);
  CYC(0x6ecc, 0x6ece); A = 0x5a; // SND_ERROR
  CALL_C(0x6ece, playSound_b00_hook, 0x0c98, 0x6ed1);
  CYC(0x6ed1, 0x6ed3); A = 0x08; // TILEINDEX_UNLIT_TORCH
  CYC(0x6ed3, 0x6ed5); C = 0x31;
  CALL_C(0x6ed5, setTile_hook, 0x3a9c, 0x6ed8);
  CYC(0x6ed8, 0x6eda); A = 0x08;
  CYC(0x6eda, 0x6edc); C = 0x33;
  CALL_C(0x6edc, setTile_hook, 0x3a9c, 0x6edf);
  CYC(0x6edf, 0x6ee1); A = 0x08;
  CYC(0x6ee1, 0x6ee3); C = 0x35;
  CALL_C(0x6ee3, setTile_hook, 0x3a9c, 0x6ee6);
  CYC(0x6ee6, 0x6ee8); A = 0x08;
  CYC(0x6ee8, 0x6eea); C = 0x53;
  CALL_C(0x6eea, setTile_hook, 0x3a9c, 0x6eed);
  CYC(0x6eed, 0x6eef); goto makeTorchesLightable; // jr

state0:
  CALL_C(0x6ef3, getThisRoomFlags_hook, 0x197d, 0x6ef6);
  CYC(0x6ef6, 0x6ef8); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(0x6ef8, 0x6efb); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x6ef8, 0x6efb);
  CALL_C(0x6efb, interactionIncState_hook, 0x23e0, 0x6efe);
  CYC(0x6efe, 0x6f01); push_effect(gb, 0x6f01); goto checkLitTorches;
afterCheckLitTorches_fromState0:
  CYC(0x6f01, 0x6f02); A = B;
  CYC(0x6f02, 0x6f04); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6f04, 0x6f05); mem_wr(gb, DE, A);

makeTorchesLightable:
  CYC(0x6f05, 0x6f08); push_effect(gb, 0x6f08); goto makeTorchesUnlightable;
afterMakeTorchesUnlightable:
  CYC(0x6f08, 0x6f0b); SET_HL(0x78e0); // objectData.objectData_makeTorchesLightableForD6Room
  CYC(0x6f0b, 0x6f0e); parseGivenObjectData_b00_hook(gb); return; // jp

  // Single caller (makeTorchesLightable, above), top-level depth sp0_.
makeTorchesUnlightable:
  CYC(0x6f0e, 0x6f11); SET_HL(w7d0c1); // FIRST_PART_INDEX, Part.id

makeTorchesUnlightable_loop:
  CYC(0x6f11, 0x6f12); A = mem_rd(gb, HL);
  CYC(0x6f12, 0x6f14); alu_cp(gb, 0x06); // PART_LIGHTABLE_TORCH
  if (F & FZ) { CYCT(0x6f14, 0x6f17); push_effect(gb, 0x6f17); goto deletePartObject; } // call z
  CYC(0x6f14, 0x6f17);

afterDeletePartObject:
  CYC(0x6f17, 0x6f18); H = alu_inc8(gb, H);
  CYC(0x6f18, 0x6f19); A = H;
  CYC(0x6f19, 0x6f1b); alu_cp(gb, 0xe0); // LAST_PART_INDEX+1
  if (F & FC) { CYCT(0x6f1b, 0x6f1d); goto makeTorchesUnlightable_loop; } // jr c
  CYC(0x6f1b, 0x6f1d);
  RET(0x6f1d);
  if (gb->pc == 0x6f08 && gb->sp == sp0_) goto afterMakeTorchesUnlightable;
  return; // ret

  // Single caller (makeTorchesUnlightable_loop, above); entered one call deep since
  // makeTorchesUnlightable's own return address 0x6f08 is still outstanding on the stack.
deletePartObject:
  CYC(0x6f1e, 0x6f1f); push_effect(gb, HL); // push hl
  CYC(0x6f1f, 0x6f20); L = alu_dec8(gb, L);
  CYC(0x6f20, 0x6f22); B = 0x40;
  CALL_C(0x6f22, clearMemory_hook, 0x046f, 0x6f25);
  CYC(0x6f25, 0x6f26); SET_HL(pop_effect(gb)); // pop hl
  RET(0x6f26);
  if (gb->pc == 0x6f17 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterDeletePartObject;
  return; // ret

  // @param[out] b Bitset of lit torches, bits 0-3. Two callers, state0 and state1 above,
  // both at top-level depth sp0_; has two exit points: early ret nz, and the final ret.
checkLitTorches:
  CYC(0x6f27, 0x6f29); A = 0x09; // TILEINDEX_LIT_TORCH
  CYC(0x6f29, 0x6f2b); B = 0x00;
  CYC(0x6f2b, 0x6f2e); SET_HL(wRoomLayout + 0x31);
  CYC(0x6f2e, 0x6f2f); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6f2f, 0x6f31); goto checkLitTorches_L6f33; } // jr nz
  CYC(0x6f2f, 0x6f31);
  CYC(0x6f31, 0x6f33); B |= (1 << 0); // set 0,b

checkLitTorches_L6f33:
  CYC(0x6f33, 0x6f35); L = 0x33;
  CYC(0x6f35, 0x6f36); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6f36, 0x6f38); goto checkLitTorches_L6f3a; } // jr nz
  CYC(0x6f36, 0x6f38);
  CYC(0x6f38, 0x6f3a); B |= (1 << 1); // set 1,b

checkLitTorches_L6f3a:
  CYC(0x6f3a, 0x6f3c); L = 0x53;
  CYC(0x6f3c, 0x6f3d); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6f3d, 0x6f3f); goto checkLitTorches_L6f41; } // jr nz
  CYC(0x6f3d, 0x6f3f);
  CYC(0x6f3f, 0x6f41); B |= (1 << 2); // set 2,b

checkLitTorches_L6f41:
  CYC(0x6f41, 0x6f43); L = 0x35;
  CYC(0x6f43, 0x6f44); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    RET_TAKEN(0x6f44);
    if (gb->pc == 0x6e98 && gb->sp == sp0_) goto afterCheckLitTorches_fromState1;
    if (gb->pc == 0x6f01 && gb->sp == sp0_) goto afterCheckLitTorches_fromState0;
    return;
  } // ret nz
  CYC(0x6f44, 0x6f45);
  CYC(0x6f45, 0x6f47); B |= (1 << 3); // set 3,b
  RET(0x6f47);
  if (gb->pc == 0x6e98 && gb->sp == sp0_) goto afterCheckLitTorches_fromState1;
  if (gb->pc == 0x6f01 && gb->sp == sp0_) goto afterCheckLitTorches_fromState0;
  return; // ret
}

// Checks to set the "bombable wall open" bit in d6 (north)
void miscPuzzles_subid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f48, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x6f4b);
  CALL_C(0x6f4b, getThisRoomFlags_hook, 0x197d, 0x6f4e);
  CYC(0x6f4e, 0x6f50); alu_bit(gb, 0, mem_rd(gb, HL)); // ROOMFLAG_BIT_KEYDOOR_UP
  if (F & FZ) { RET_TAKEN(0x6f50); return; } // ret z
  CYC(0x6f50, 0x6f51);
  CYC(0x6f51, 0x6f53); L = 0x19; // <ROOM_AGES_519
  CYC(0x6f53, 0x6f55); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01); // set 0,(hl)
  CYC(0x6f55, 0x6f58); interactionDelete_hook(gb); return; // jp
}

// Checks to set the "bombable wall open" bit in d6 (east)
void miscPuzzles_subid09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f58, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x6f5b);
  CALL_C(0x6f5b, getThisRoomFlags_hook, 0x197d, 0x6f5e);
  CYC(0x6f5e, 0x6f60); alu_bit(gb, 1, mem_rd(gb, HL)); // ROOMFLAG_BIT_KEYDOOR_RIGHT
  if (F & FZ) { RET_TAKEN(0x6f60); return; } // ret z
  CYC(0x6f60, 0x6f61);
  CYC(0x6f61, 0x6f63); L = 0x26; // <ROOM_AGES_526
  CYC(0x6f63, 0x6f65); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02); // set 1,(hl)
  CYC(0x6f65, 0x6f68); interactionDelete_hook(gb); return; // jp
}

// Jabu-jabu water level controller script, in the room with the 3 buttons
void miscPuzzles_subid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f68, 0x6f6a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6f6a, 0x6f6b); A = mem_rd(gb, DE);
  {
    CYC(0x6f6b, 0x6f6c); push_effect(gb, 0x6f6c);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == 0x6f85) goto state1;
    if (target == 0x6fd8) goto state2;
    if (target == 0x6fea) goto state3;
  }

  // miscPuzzles_subid0a@state0
  CYC(0x6f74, 0x6f77); A = W8(wActiveTriggers);
  CYC(0x6f77, 0x6f79); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x6f79, 0x6f7a); mem_wr(gb, DE, A);
  CYC(0x6f7a, 0x6f7d); A = W8(wJabuWaterLevel);
  CYC(0x6f7d, 0x6f7f); alu_and(gb, 0xf0);
  CYC(0x6f7f, 0x6f82); W8(wSwitchState) = A;
  CYC(0x6f82, 0x6f85); interactionIncState_hook(gb); return; // jp

state1:
  CYC(0x6f85, 0x6f88); A = W8(wActiveTriggers);
  CYC(0x6f88, 0x6f89); B = A;
  CYC(0x6f89, 0x6f8b); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x6f8b, 0x6f8c); A = mem_rd(gb, DE);
  CYC(0x6f8c, 0x6f8d); alu_xor(gb, B);
  CYC(0x6f8d, 0x6f8e); C = A;
  CYC(0x6f8e, 0x6f8f); A = B;
  CYC(0x6f8f, 0x6f90); mem_wr(gb, DE, A);
  CYC(0x6f90, 0x6f92); alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(0x6f92, 0x6f94); goto drainWater; } // jr nz
  CYC(0x6f92, 0x6f94);
  CYC(0x6f94, 0x6f95); alu_and(gb, C);
  if (F & FZ) { RET_TAKEN(0x6f95); return; } // ret z
  CYC(0x6f95, 0x6f96);
  CYC(0x6f96, 0x6f99); A = W8(wSwitchState);
  CYC(0x6f99, 0x6f9a); alu_and(gb, C);
  if (!(F & FZ)) { RET_TAKEN(0x6f9a); return; } // ret nz
  CYC(0x6f9a, 0x6f9b);
  CYC(0x6f9b, 0x6f9c); A = C;
  CYC(0x6f9c, 0x6f9f); SET_HL(wSwitchState);
  CYC(0x6f9f, 0x6fa0); alu_or(gb, mem_rd(gb, HL));
  CYC(0x6fa0, 0x6fa1); mem_wr(gb, HL, A);
  CYC(0x6fa1, 0x6fa3); alu_and(gb, 0xf0);
  CYC(0x6fa3, 0x6fa4); B = A;
  CYC(0x6fa4, 0x6fa7); SET_HL(wJabuWaterLevel);
  CYC(0x6fa7, 0x6fa8); A = mem_rd(gb, HL);
  CYC(0x6fa8, 0x6faa); alu_and(gb, 0x03);
  CYC(0x6faa, 0x6fab); A = alu_inc8(gb, A);
  CYC(0x6fab, 0x6fac); alu_or(gb, B);
  CYC(0x6fac, 0x6fad); mem_wr(gb, HL, A);
  CYC(0x6fad, 0x6faf); A = 0x09; // <TX_1209
  CYC(0x6faf, 0x6fb1); goto beginCutscene; // jr

drainWater:
  CYC(0x6fb1, 0x6fb4); A = W8(wJabuWaterLevel);
  CYC(0x6fb4, 0x6fb6); alu_and(gb, 0x07);
  if (F & FZ) { RET_TAKEN(0x6fb6); return; } // ret z
  CYC(0x6fb6, 0x6fb7);
  CYC(0x6fb7, 0x6fb8); alu_xor(gb, A);
  CYC(0x6fb8, 0x6fbb); W8(wJabuWaterLevel) = A;
  CYC(0x6fbb, 0x6fbe); W8(wSwitchState) = A;
  CYC(0x6fbe, 0x6fc0); A = 0x08; // <TX_1208

beginCutscene:
  CYC(0x6fc0, 0x6fc2); E = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x6fc2, 0x6fc3); mem_wr(gb, DE, A);
  CYC(0x6fc3, 0x6fc5); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(0x6fc5, 0x6fc8); W8(wDisabledObjects) = A;
  CYC(0x6fc8, 0x6fcb); W8(wMenuDisabled) = A;
  CYC(0x6fcb, 0x6fcd); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6fcd, 0x6fcf); A = 60;
  CYC(0x6fcf, 0x6fd0); mem_wr(gb, DE, A);
  CYC(0x6fd0, 0x6fd2); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(0x6fd2, playSound_b00_hook, 0x0c98, 0x6fd5);
  CYC(0x6fd5, 0x6fd8); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(0x6fd8, interactionDecCounter1_hook, 0x23cc, 0x6fdb);
  if (!(F & FZ)) { RET_TAKEN(0x6fdb); return; } // ret nz
  CYC(0x6fdb, 0x6fdc);
  CYC(0x6fdc, 0x6fde); A = 0xf0;
  CYC(0x6fde, 0x6fdf); mem_wr(gb, HL, A);
  CALL_C(0x6fdf, setScreenShakeCounter_hook, 0x24bb, 0x6fe2);
  CYC(0x6fe2, 0x6fe4); A = 0xc2; // SND_FLOODGATES
  CALL_C(0x6fe4, playSound_b00_hook, 0x0c98, 0x6fe7);
  CYC(0x6fe7, 0x6fea); interactionIncState_hook(gb); return; // jp

state3:
  CALL_C(0x6fea, interactionDecCounter1_hook, 0x23cc, 0x6fed);
  if (!(F & FZ)) { RET_TAKEN(0x6fed); return; } // ret nz
  CYC(0x6fed, 0x6fee);
  CYC(0x6fee, 0x6ff0); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6ff0, 0x6ff2); mem_wr(gb, HL, 0x01);
  CYC(0x6ff2, 0x6ff3); alu_xor(gb, A);
  CYC(0x6ff3, 0x6ff6); W8(wDisabledObjects) = A;
  CYC(0x6ff6, 0x6ff9); W8(wMenuDisabled) = A;
  CYC(0x6ff9, 0x6ffb); B = 0x12; // >TX_1200
  CYC(0x6ffb, 0x6ffd); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x6ffd, 0x6ffe); C = mem_rd(gb, HL);
  CALL_C(0x6ffe, showText_hook, 0x1872, 0x7001);
  CYC(0x7001, 0x7003); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(0x7003, playSound_b00_hook, 0x0c98, 0x7006);
  CYC(0x7006, 0x7009); A = W8(wActiveMusic);
  CYC(0x7009, 0x700c); playSound_b00_hook(gb); return; // jp
}

// Ladder spawner in d7 miniboss room
void miscPuzzles_subid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x700c, 0x700e); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x700e, 0x700f); A = mem_rd(gb, DE);
  CYC(0x700f, 0x7010); push_effect(gb, 0x7010);
  {
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == 0x7016) goto state1;
    if (target == 0x702c) goto state2;
  }
  miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(gb); return; // target == 0x73fd

state1:
  CYC(0x7016, 0x7019); A = W8(wNumEnemies);
  CYC(0x7019, 0x701a); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x701a); return; } // ret nz
  CYC(0x701a, 0x701b);
  CALL_C(0x701b, getThisRoomFlags_hook, 0x197d, 0x701e);
  CYC(0x701e, 0x7020); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(0x7020, 0x7022); L = 0x4d; // <ROOM_AGES_54d
  CYC(0x7022, 0x7024); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(0x7024, 0x7026); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7026, 0x7028); A = 0x08;
  CYC(0x7028, 0x7029); mem_wr(gb, DE, A);
  CYC(0x7029, 0x702c); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(0x702c, interactionDecCounter1_hook, 0x23cc, 0x702f);
  if (!(F & FZ)) { RET_TAKEN(0x702f); return; } // ret nz
  CYC(0x702f, 0x7030);
  CYC(0x7030, 0x7032); mem_wr(gb, HL, 0x08);
  CALL_C(0x7032, objectGetTileAtPosition_hook, 0x1444, 0x7035);
  CYC(0x7035, 0x7036); C = L;
  CYC(0x7036, 0x7037); A = C;
  CYC(0x7037, 0x7039); H8(hFF92) = A; // ldh ($ff92),a
  CYC(0x7039, 0x703b); A = 0x18; // TILEINDEX_SS_LADDER
  CALL_C(0x703b, setTile_hook, 0x3a9c, 0x703e);
  CYC(0x703e, 0x7040); B = 0x05; // INTERAC_PUFF
  CALL_C(0x7040, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x7043);
  CYC(0x7043, 0x7045); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x7045, 0x7046); A = mem_rd(gb, DE);
  CYC(0x7046, 0x7048); alu_add(gb, 0x10);
  CYC(0x7048, 0x7049); mem_wr(gb, DE, A);
  CYC(0x7049, 0x704b); A = H8(hFF92); // ldh a,($ff92)
  CYC(0x704b, 0x704d); alu_cp(gb, 0x90);
  if (F & FC) { RET_TAKEN(0x704d); return; } // ret c
  CYC(0x704d, 0x704e);
  CYC(0x704e, 0x7050); C = 0x80;
  CYC(0x7050, 0x7052); A = 0x52; // TILEINDEX_SS_52
  CALL_C(0x7052, setTile_hook, 0x3a9c, 0x7055);
  CYC(0x7055, 0x7057); C = 0x90;
  CYC(0x7057, 0x7059); A = 0x01; // TILEINDEX_SS_EMPTY
  CALL_C(0x7059, setTile_hook, 0x3a9c, 0x705c);
  CYC(0x705c, 0x705e); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x705e, playSound_b00_hook, 0x0c98, 0x7061);
  CYC(0x7061, 0x7062); alu_xor(gb, A);
  CYC(0x7062, 0x7065); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x7065, 0x7068); interactionDelete_hook(gb); return; // jp
}

// Shared with subid19, which reaches this via `jp` from a fully independent call
// site -- must stand alone with no dependency on subid0c's context.
void miscPuzzles_dropSmallKeyHere_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7075, 0x7078); SET_BC(0x3001); // TREASURE_SMALL_KEY, $01
  CALL_C(0x7078, createTreasure_hook, 0x27d4, 0x707b);
  if (!(F & FZ)) { RET_TAKEN(0x707b); return; } // ret nz
  CYC(0x707b, 0x707c);
  CALL_C(0x707c, objectCopyPosition_hook, 0x2242, 0x707f);
  CYC(0x707f, 0x7082); interactionDelete_hook(gb); return; // jp
}

// Switch hook puzzle early in d7 for a small key
void miscPuzzles_subid0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7068, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x706b);
  CALL_C(0x706b, miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook, 0x73e8, 0x706e);
  CYC(0x706e, 0x7071); SET_HL(0x7082); // miscPuzzles_subid0c_wantedTiles (pure ROM data, no C rep needed)
  CALL_C(0x7071, miscPuzzles_verifyTilesAtPositions_hook, 0x6dcc, 0x7074);
  if (!(F & FZ)) { RET_TAKEN(0x7074); return; } // ret nz
  CYC(0x7074, 0x7075);
  miscPuzzles_dropSmallKeyHere_hook(gb); return; // falls through
}

// Staircase spawner after moving first set of stone panels in d8
void miscPuzzles_subid0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7088, 0x708a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x708a, 0x708b); A = mem_rd(gb, DE);
  {
    CYC(0x708b, 0x708c); push_effect(gb, 0x708c);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == 0x70b9) goto state1;
    if (target == 0x70cf) goto state2;
  }

  // miscPuzzles_subid0d@state0
  CALL_C(0x7092, getThisRoomFlags_hook, 0x197d, 0x7095);
  CYC(0x7095, 0x7097); alu_and(gb, 0x40); // ROOMFLAG_40
  if (!(F & FZ)) { CYCT(0x7097, 0x709a); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x7097, 0x709a);
  CYC(0x709a, 0x709d); A = W8(wNumTorchesLit);
  CYC(0x709d, 0x709f); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(0x709f); return; } // ret nz
  CYC(0x709f, 0x70a0);
  CYC(0x70a0, 0x70a3); SET_HL(wActiveTriggers);
  CYC(0x70a3, 0x70a4); A = mem_rd(gb, HL);
  CYC(0x70a4, 0x70a6); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x70a6); return; } // ret nz
  CYC(0x70a6, 0x70a7);
  CYC(0x70a7, 0x70a9); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x70a9, 0x70ab); A = 0x1e;
  CYC(0x70ab, 0x70ac); mem_wr(gb, DE, A);
  CYC(0x70ac, 0x70ae); A = 0x08;
  CALL_C(0x70ae, setScreenShakeCounter_hook, 0x24bb, 0x70b1);
  CYC(0x70b1, 0x70b3); A = 0x70; // SND_DOORCLOSE
  CALL_C(0x70b3, playSound_b00_hook, 0x0c98, 0x70b6);
  CYC(0x70b6, 0x70b9); interactionIncState_hook(gb); return; // jp

state1:
  CALL_C(0x70b9, interactionDecCounter1_hook, 0x23cc, 0x70bc);
  if (!(F & FZ)) { RET_TAKEN(0x70bc); return; } // ret nz
  CYC(0x70bc, 0x70bd);
  CYC(0x70bd, 0x70c0); SET_HL(wActiveTriggers);
  CYC(0x70c0, 0x70c1); A = mem_rd(gb, HL);
  CYC(0x70c1, 0x70c3); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x70c3, 0x70c5); goto l70ca; } // jr z
  CYC(0x70c3, 0x70c5);
  CYC(0x70c5, 0x70c7); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x70c7, 0x70c8); alu_xor(gb, A);
  CYC(0x70c8, 0x70c9); mem_wr(gb, DE, A);
  RET(0x70c9); return; // ret

l70ca:
  CYC(0x70ca, 0x70cc); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(0x70cc, 0x70cf); interactionIncState_hook(gb); return; // jp

state2:
  CYC(0x70cf, 0x70d2); A = W8(wActiveTriggers);
  CYC(0x70d2, 0x70d4); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(0x70d4); return; } // ret nz
  CYC(0x70d4, 0x70d5);
  CYC(0x70d5, 0x70d7); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x70d7, playSound_b00_hook, 0x0c98, 0x70da);
  CYC(0x70da, 0x70dc); B = 0x05; // INTERAC_PUFF
  CALL_C(0x70dc, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x70df);
  CALL_C(0x70df, objectGetTileAtPosition_hook, 0x1444, 0x70e2);
  CYC(0x70e2, 0x70e3); C = L;
  CYC(0x70e3, 0x70e5); A = 0x52; // TILEINDEX_NORTH_STAIRS
  CALL_C(0x70e5, setTile_hook, 0x3a9c, 0x70e8);
  CYC(0x70e8, 0x70eb); interactionDelete_hook(gb); return; // jp
}

// Staircase spawner after putting in slates in d8
void miscPuzzles_subid0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x70eb, checkInteractionState_hook, 0x23fe, 0x70ee);
  if (!(F & FZ)) { CYCT(0x70ee, 0x70f1); goto state1; } // jp nz
  CYC(0x70ee, 0x70f1);

  // miscPuzzles_subid0e@state0
  CALL_C(0x70f1, getThisRoomFlags_hook, 0x197d, 0x70f4);
  CYC(0x70f4, 0x70f6); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x70f6, 0x70f9); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x70f6, 0x70f9);
  CYC(0x70f9, 0x70fa); A = mem_rd(gb, HL);
  CYC(0x70fa, 0x70fc); alu_and(gb, 0x0f); // ROOMFLAG_01|02|04|08
  CYC(0x70fc, 0x70fe); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(0x70fe); return; } // ret nz
  CYC(0x70fe, 0x70ff);
  CYC(0x70ff, 0x7102); SET_HL(wActiveTriggers);
  CYC(0x7102, 0x7104); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(0x7104, 0x7107); interactionIncState_hook(gb); return; // jp

state1:
  CYC(0x7107, 0x710a); A = W8(wActiveTriggers);
  CYC(0x710a, 0x710c); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(0x710c); return; } // ret nz
  CYC(0x710c, 0x710d);
  CYC(0x710d, 0x710f); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x710f, playSound_b00_hook, 0x0c98, 0x7112);
  CYC(0x7112, 0x7114); B = 0x05; // INTERAC_PUFF
  CALL_C(0x7114, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x7117);
  CALL_C(0x7117, objectGetTileAtPosition_hook, 0x1444, 0x711a);
  CYC(0x711a, 0x711b); C = L;
  CYC(0x711b, 0x711d); A = 0x52; // TILEINDEX_NORTH_STAIRS
  CALL_C(0x711d, setTile_hook, 0x3a9c, 0x7120);
  CYC(0x7120, 0x7123); interactionDelete_hook(gb); return; // jp
}

// Octogon boss initialization (in the room just before the boss)
void miscPuzzles_subid0f_hook(GB *gb) {
  CYC(0x7123, 0x7126); SET_HL(wTmpcfc0_octogonBoss_loadedExtraGfx);
  CYC(0x7126, 0x7127); alu_xor(gb, A);
  CYC(0x7127, 0x7128); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7128, 0x7129); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [var03] = 0
  CYC(0x7129, 0x712a); A = alu_dec8(gb, A);
  CYC(0x712a, 0x712b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [direction] = $ff
  CYC(0x712b, 0x712d); mem_wr(gb, HL, 0x28); // [health]
  CYC(0x712d, 0x712e); L = alu_inc8(gb, L);
  CYC(0x712e, 0x7130); mem_wr(gb, HL, 0x28); // [y]
  CYC(0x7130, 0x7131); L = alu_inc8(gb, L);
  CYC(0x7131, 0x7133); mem_wr(gb, HL, 0x78); // [x]
  CYC(0x7133, 0x7134); L = alu_inc8(gb, L);
  CYC(0x7134, 0x7135); mem_wr(gb, HL, A); // [var30] = $ff
  CYC(0x7135, 0x7138); interactionDelete_hook(gb); return; // jp
}

// Something at the top of Talus Peaks?
void miscPuzzles_subid10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7138, 0x713b); SET_HL(wTmpcfc0_patchMinigame_fixingSword);
  CYC(0x713b, 0x713d); B = 0x08;
  CALL_C(0x713d, clearMemory_hook, 0x046f, 0x7140);
  CYC(0x7140, 0x7143); interactionDelete_hook(gb); return; // jp
}

// D5 keyhole opening
void miscPuzzles_subid11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7143, checkInteractionState_hook, 0x23fe, 0x7146);
  if (!(F & FZ)) { CYCT(0x7146, 0x7149); interactionRunScript_hook(gb); return; } // jp nz
  CYC(0x7146, 0x7149);
  CALL_C(0x7149, returnIfScrollMode01Unset_hook, 0x26e4, 0x714c);
  CALL_C(0x714c, getThisRoomFlags_hook, 0x197d, 0x714f);
  CYC(0x714f, 0x7151); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(0x7151, 0x7154); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x7151, 0x7154);
  CYC(0x7154, 0x7155); push_effect(gb, DE); // push de
  CALL_C(0x7155, reloadTileMap_hook, 0x12fc, 0x7158);
  CYC(0x7158, 0x7159); SET_DE(pop_effect(gb)); // pop de
  CYC(0x7159, 0x715c); SET_HL(0x783c); // mainScripts.miscPuzzles_crownDungeonOpeningScript
  miscPuzzles_setScriptAndIncState_hook(gb); return; // falls into miscPuzzles_setScriptAndIncState
}

// Shared tail helper: also reached by tail-jump (jr) from subid12/subid13.
void miscPuzzles_setScriptAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x715c, interactionSetScript_hook, 0x2544, 0x715f);
  CALL_C(0x715f, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7162);
  CYC(0x7162, 0x7165); interactionIncState_hook(gb); return; // jp
}

// D6 present/past keyhole opening
void miscPuzzles_subid12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7165, checkInteractionState_hook, 0x23fe, 0x7168);
  if (!(F & FZ)) { CYCT(0x7168, 0x716b); interactionRunScript_hook(gb); return; } // jp nz
  CYC(0x7168, 0x716b);
  CALL_C(0x716b, getThisRoomFlags_hook, 0x197d, 0x716e);
  CYC(0x716e, 0x7170); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(0x7170, 0x7173); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x7170, 0x7173);
  CYC(0x7173, 0x7176); SET_HL(0x7856); // mainScripts.miscPuzzles_mermaidsCaveDungeonOpeningScript
  CYC(0x7176, 0x7178); miscPuzzles_setScriptAndIncState_hook(gb); return; // jr
}

// Eyeglass library keyhole opening
void miscPuzzles_subid13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7178, checkInteractionState_hook, 0x23fe, 0x717b);
  if (!(F & FZ)) { CYCT(0x717b, 0x717e); interactionRunScript_hook(gb); return; } // jp nz
  CYC(0x717b, 0x717e);
  CALL_C(0x717e, getThisRoomFlags_hook, 0x197d, 0x7181);
  CYC(0x7181, 0x7183); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(0x7183, 0x7186); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x7183, 0x7186);
  CYC(0x7186, 0x7189); SET_HL(0x7860); // mainScripts.miscPuzzles_eyeglassLibraryOpeningScript
  CYC(0x7189, 0x718b); miscPuzzles_setScriptAndIncState_hook(gb); return; // jr
}

// Spot to put a rolling colored block on in Hero's Cave
void miscPuzzles_subid14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x718b, checkInteractionState_hook, 0x23fe, 0x718e);
  if (F & FZ) { CYCT(0x718e, 0x7191); miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(gb); return; } // jp z
  CYC(0x718e, 0x7191);
  CALL_C(0x7191, objectGetTileAtPosition_hook, 0x1444, 0x7194);
  CYC(0x7194, 0x7196); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(0x7196, 0x7198); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(0x7198); return; } // ret nc
  CYC(0x7198, 0x7199);
  CYC(0x7199, 0x719a); B = A;
  CYC(0x719a, 0x719d); A = W8(wRotatingCubePos);
  CYC(0x719d, 0x719e); alu_cp(gb, L);
  if (!(F & FZ)) { RET_TAKEN(0x719e); return; } // ret nz
  CYC(0x719e, 0x719f);
  CYC(0x719f, 0x71a2); A = W8(wRotatingCubeColor);
  CYC(0x71a2, 0x71a4); alu_and(gb, 0x03);
  CYC(0x71a4, 0x71a5); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(0x71a5); return; } // ret nz
  CYC(0x71a5, 0x71a6);
  CYC(0x71a6, 0x71a7); C = L;
  CYC(0x71a7, 0x71aa); SET_HL(wActiveTriggers);
  CYC(0x71aa, 0x71ab); A = B;
  CALL_C(0x71ab, setFlag_hook, 0x020e, 0x71ae);
  CYC(0x71ae, 0x71b0); A = 0xa3;
  CALL_C(0x71b0, setTile_hook, 0x3a9c, 0x71b3);
  CYC(0x71b3, 0x71b5); B = 0xce; // >wRoomCollisions
  CYC(0x71b5, 0x71b7); A = 0x0f;
  CYC(0x71b7, 0x71b8); mem_wr(gb, BC, A);
  CYC(0x71b8, 0x71ba); A = 0x50; // SND_CLINK
  CYC(0x71ba, 0x71bd); playSound_b00_hook(gb); return; // jp
}

// Stairs from solving colored cube puzzle in Hero's Cave (related to subid $14)
void miscPuzzles_subid15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x71bd, checkInteractionState_hook, 0x23fe, 0x71c0);
  if (F & FZ) { CYCT(0x71c0, 0x71c3); miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(gb); return; } // jp z
  CYC(0x71c0, 0x71c3);
  CYC(0x71c3, 0x71c6); A = W8(wActiveTriggers);
  CYC(0x71c6, 0x71c8); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x71c8); return; } // ret nz
  CYC(0x71c8, 0x71c9);
  CYC(0x71c9, 0x71cb); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x71cb, playSound_b00_hook, 0x0c98, 0x71ce);
  CYC(0x71ce, 0x71d0); A = 0x45; // TILEINDEX_INDOOR_DOWNSTAIRCASE
  CYC(0x71d0, 0x71d2); C = 0x15;
  CALL_C(0x71d2, setTile_hook, 0x3a9c, 0x71d5);
  CALL_C(0x71d5, getThisRoomFlags_hook, 0x197d, 0x71d8);
  CYC(0x71d8, 0x71da); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(0x71da, 0x71dd); interactionDelete_hook(gb); return; // jp
}

// Warps Link out of Hero's Cave upon opening the chest
void miscPuzzles_subid16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71dd, 0x71df); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x71df, 0x71e0); A = mem_rd(gb, DE);
  {
    CYC(0x71e0, 0x71e1); push_effect(gb, 0x71e1);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == 0x71e7) goto state1;
    if (target == 0x71f0) goto state2;
    // target == 0x73f2 (miscPuzzles_deleteSelfOrIncStateIfItemFlagSet, bare global elsewhere)
    miscPuzzles_deleteSelfOrIncStateIfItemFlagSet_hook(gb);
    return;
  }

state1:
  CALL_C(0x71e7, getThisRoomFlags_hook, 0x197d, 0x71ea);
  CYC(0x71ea, 0x71ec); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (F & FZ) { RET_TAKEN(0x71ec); return; } // ret z
  CYC(0x71ec, 0x71ed);
  CALL_C(0x71ed, interactionIncState_hook, 0x23e0, 0x71f0);
  // falls through into state2

state2:
  CYC(0x71f0, 0x71f2); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(0x71f2, 0x71f5); W8(wDisabledObjects) = A;
  CYC(0x71f5, 0x71f8); W8(wDisableLinkCollisionsAndMenu) = A;
  CALL_C(0x71f8, retIfTextIsActive_hook, 0x1859, 0x71fb);
  CYC(0x71fb, 0x71fe); SET_HL(0x7204); // @warpDestData
  CALL_C(0x71fe, setWarpDestVariables_hook, 0x1997, 0x7201);
  CYC(0x7201, 0x7204); interactionDelete_hook(gb); return; // jp
}

// Enables portal in Hero's Cave first room if its other end is active
void miscPuzzles_subid17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7209, getThisRoomFlags_hook, 0x197d, 0x720c);
  CYC(0x720c, 0x720d); push_effect(gb, HL); // push hl
  CYC(0x720d, 0x720f); L = 0xc9; // <ROOM_AGES_4c9
  CYC(0x720f, 0x7211); alu_bit(gb, 5, mem_rd(gb, HL)); // ROOMFLAG_BIT_ITEM
  CYC(0x7211, 0x7212); SET_HL(pop_effect(gb)); // pop hl
  if (F & FZ) { CYCT(0x7212, 0x7214); goto subid17_skip; } // jr z
  CYC(0x7212, 0x7214);
  CYC(0x7214, 0x7216); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)

subid17_skip:
  CYC(0x7216, 0x7219); interactionDelete_hook(gb); return; // jp
}

// Drops a key in hero's cave block-pushing puzzle
void miscPuzzles_subid18_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7219, checkInteractionState_hook, 0x23fe, 0x721c);
  if (F & FZ) { CYCT(0x721c, 0x721f); miscPuzzles_deleteSelfOrIncStateIfItemFlagSet_hook(gb); return; } // jp z
  CYC(0x721c, 0x721f);
  CYC(0x721f, 0x7222); SET_HL(wRoomLayout + 0x95);
  CYC(0x7222, 0x7223); A = mem_rd(gb, HL);
  CYC(0x7223, 0x7225); alu_cp(gb, 0x2a); // TILEINDEX_PUSHABLE_STATUE
  if (!(F & FZ)) { RET_TAKEN(0x7225); return; } // ret nz
  CYC(0x7225, 0x7226);
  CYC(0x7226, 0x7228); L = 0x5d;
  CYC(0x7228, 0x7229); A = mem_rd(gb, HL);
  CYC(0x7229, 0x722b); alu_cp(gb, 0x2a); // TILEINDEX_PUSHABLE_STATUE
  if (!(F & FZ)) { RET_TAKEN(0x722b); return; } // ret nz
  CYC(0x722b, 0x722c);
  CYC(0x722c, 0x722f); miscPuzzles_dropSmallKeyHere_hook(gb); return; // jp
}

// Bridge controller in d5 room after the miniboss
void miscPuzzles_subid19_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x722f, 0x7231); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7231, 0x7232); A = mem_rd(gb, DE);
  {
    CYC(0x7232, 0x7233); push_effect(gb, 0x7233);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == 0x723d) goto subid19_state1;
    if (target == 0x724a) goto subid19_state2;
    if (target == 0x7276) goto subid19_state3;
    if (target == 0x727e) goto subid19_state4;
    interactionIncState_hook(gb); return; // target == 0x23e0
  }

subid19_state1:
  CYC(0x723d, 0x7240); A = W8(wActiveTriggers);
  CYC(0x7240, 0x7241); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x7241); return; } // ret nc
  CYC(0x7241, 0x7242);
  CYC(0x7242, 0x7244); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7244, 0x7246); A = 0x08;
  CYC(0x7246, 0x7247); mem_wr(gb, DE, A);
  CYC(0x7247, 0x724a); interactionIncState_hook(gb); return; // jp

subid19_state2:
  CYC(0x724a, 0x724d); A = W8(wActiveTriggers);
  CYC(0x724d, 0x724e); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x724e, 0x7250); goto subid19_releasedTrigger; } // jr nc
  CYC(0x724e, 0x7250);
  CALL_C(0x7250, interactionDecCounter1_hook, 0x23cc, 0x7253);
  if (!(F & FZ)) { RET_TAKEN(0x7253); return; } // ret nz
  CYC(0x7253, 0x7254);
  CYC(0x7254, 0x7256); mem_wr(gb, HL, 0x08);
  CYC(0x7256, 0x7259); SET_HL(wRoomLayout + 0x55);

subid19_state2_loop:
  CYC(0x7259, 0x725a); C = L;
  CYC(0x725a, 0x725b); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x725b, 0x725d); alu_cp(gb, 0xf4); // TILEINDEX_BLANK_HOLE
  if (!(F & FZ)) { CYCT(0x725d, 0x725f); goto subid19_state2_advance; } // jr nz
  CYC(0x725d, 0x725f);
  CYC(0x725f, 0x7261); A = 0x6d; // TILEINDEX_HORIZONTAL_BRIDGE
  CALL_C(0x7261, setTileInAllBuffers_hook, 0x3ac6, 0x7264);
  CYC(0x7264, 0x7266); A = 0x70; // SND_DOORCLOSE
  CYC(0x7266, 0x7269); playSound_b00_hook(gb); return; // jp

subid19_state2_advance:
  CYC(0x7269, 0x726a); A = L;
  CYC(0x726a, 0x726c); alu_cp(gb, 0x5a);
  if (F & FC) { CYCT(0x726c, 0x726e); goto subid19_state2_loop; } // jr c
  CYC(0x726c, 0x726e);
  CYC(0x726e, 0x7271); interactionIncState_hook(gb); return; // jp

subid19_releasedTrigger:
  CALL_C(0x7271, interactionIncState_hook, 0x23e0, 0x7274);
  CYC(0x7274, 0x7275); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x7275); return; // ret

subid19_state3:
  CYC(0x7276, 0x7279); A = W8(wActiveTriggers);
  CYC(0x7279, 0x727a); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x727a); return; } // ret c
  CYC(0x727a, 0x727b);
  CYC(0x727b, 0x727e); interactionIncState_hook(gb); return; // jp

subid19_state4:
  CYC(0x727e, 0x7281); A = W8(wActiveTriggers);
  CYC(0x7281, 0x7282); alu_rrca(gb);
  if (F & FC) { CYCT(0x7282, 0x7284); goto subid19_pressedTrigger; } // jr c
  CYC(0x7282, 0x7284);
  CALL_C(0x7284, interactionDecCounter1_hook, 0x23cc, 0x7287);
  if (!(F & FZ)) { RET_TAKEN(0x7287); return; } // ret nz
  CYC(0x7287, 0x7288);
  CYC(0x7288, 0x728a); mem_wr(gb, HL, 0x08);
  CYC(0x728a, 0x728d); SET_HL(wRoomLayout + 0x59);

subid19_state4_loop:
  CYC(0x728d, 0x728e); C = L;
  CYC(0x728e, 0x728f); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x728f, 0x7291); alu_cp(gb, 0xf4); // TILEINDEX_BLANK_HOLE
  if (F & FZ) { CYCT(0x7291, 0x7293); goto subid19_state4_advance; } // jr z
  CYC(0x7291, 0x7293);
  CYC(0x7293, 0x7295); alu_cp(gb, 0xdb); // TILEINDEX_SWITCH_DIAMOND
  if (F & FZ) { CYCT(0x7295, 0x7298); push_effect(gb, 0x7298); goto subid19_createDebris; } // call z
  CYC(0x7295, 0x7298);

subid19_afterCreateDebris:
  CYC(0x7298, 0x729a); A = 0xf4; // TILEINDEX_BLANK_HOLE
  CALL_C(0x729a, setTileInAllBuffers_hook, 0x3ac6, 0x729d);
  CYC(0x729d, 0x729f); A = 0x70; // SND_DOORCLOSE
  CYC(0x729f, 0x72a2); playSound_b00_hook(gb); return; // jp

subid19_state4_advance:
  CYC(0x72a2, 0x72a3); A = L;
  CYC(0x72a3, 0x72a5); alu_cp(gb, 0x55);
  if (!(F & FC)) { CYCT(0x72a5, 0x72a7); goto subid19_state4_loop; } // jr nc
  CYC(0x72a5, 0x72a7);

subid19_pressedTrigger:
  CYC(0x72a7, 0x72a9); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x72a9, 0x72ab); A = 0x01;
  CYC(0x72ab, 0x72ac); mem_wr(gb, DE, A);
  RET(0x72ac); return; // ret

// miscPuzzles_subid19@createDebris -- single caller, the conditional call z in state4's loop
subid19_createDebris:
  CYC(0x72ad, 0x72ae); push_effect(gb, HL); // push hl
  CYC(0x72ae, 0x72af); push_effect(gb, BC); // push bc
  CYC(0x72af, 0x72b1); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(0x72b1, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x72b4);
  CYC(0x72b4, 0x72b5); SET_BC(pop_effect(gb)); // pop bc
  CYC(0x72b5, 0x72b6); SET_HL(pop_effect(gb)); // pop hl
  RET(0x72b6);
  if (gb->pc == 0x7298 && gb->sp == sp0_) goto subid19_afterCreateDebris;
  return; // ret
}

// Checks solution to pushblock puzzle in Hero's Cave
void miscPuzzles_subid1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x72b7, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x72ba);
  CALL_C(0x72ba, miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook, 0x73e8, 0x72bd);
  CYC(0x72bd, 0x72c0); SET_HL(0x72cc); // @wantedTiles
  CALL_C(0x72c0, miscPuzzles_verifyTilesAtPositions_hook, 0x6dcc, 0x72c3);
  if (!(F & FZ)) { RET_TAKEN(0x72c3); return; } // ret nz
  CYC(0x72c3, 0x72c4);
  CYC(0x72c4, 0x72c7); SET_HL(0x4f1d); // agesInteractionsBank08.spawnChestAndDeleteSelf
  CYC(0x72c7, 0x72c9); E = 0x08;
  CYC(0x72c9, 0x72cc); interBankCall_hook(gb); return; // jp
}

// Subids $1b-$1d: Spawn gasha seeds at the top of the maku tree at specific times.
// b = essence that must be obtained; c = position to spawn it at.
static void miscPuzzles_subid1bcd_common(GB *gb, uint16_t sp0_) {
  CYC(0x72e6, 0x72e7); push_effect(gb, BC); // push bc
  CYC(0x72e7, 0x72e9); A = 0x40; // TREASURE_ESSENCE
  CALL_C(0x72e9, checkTreasureObtained_hook, 0x1748, 0x72ec);
  CYC(0x72ec, 0x72ed); SET_BC(pop_effect(gb)); // pop bc
  if (!(F & FC)) { CYCT(0x72ed, 0x72ef); goto subid1bcd_delete; } // jr nc
  CYC(0x72ed, 0x72ef);
  CYC(0x72ef, 0x72f0); alu_and(gb, B);
  if (F & FZ) { CYCT(0x72f0, 0x72f2); goto subid1bcd_delete; } // jr z
  CYC(0x72f0, 0x72f2);
  CALL_C(0x72f2, objectSetShortPosition_hook, 0x20c3, 0x72f5);
  CALL_C(0x72f5, getThisRoomFlags_hook, 0x197d, 0x72f8);
  CYC(0x72f8, 0x72fa); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(0x72fa, 0x72fc); goto subid1bcd_delete; } // jr nz
  CYC(0x72fa, 0x72fc);
  CYC(0x72fc, 0x72ff); SET_BC(0x3407); // TREASURE_OBJECT_GASHA_SEED_07
  CALL_C(0x72ff, createTreasure_hook, 0x27d4, 0x7302);
  if (F & FZ) {
    CALL_C_CC(0x7302, objectCopyPosition_hook, 0x2242, 0x7305);
  } else {
    CYC(0x7302, 0x7305);
  } // call z

subid1bcd_delete:
  CYC(0x7305, 0x7308); interactionDelete_hook(gb); return; // jp
}

void miscPuzzles_subid1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x72d9, 0x72dc); SET_BC(0x0853); // b = essence $08, c = position $53
  CYC(0x72dc, 0x72de); miscPuzzles_subid1bcd_common(gb, sp0_); return; // jr
}

void miscPuzzles_subid1c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x72de, 0x72e1); SET_BC(0x4034); // b = essence $40, c = position $34
  CYC(0x72e1, 0x72e3); miscPuzzles_subid1bcd_common(gb, sp0_); return; // jr
}

void miscPuzzles_subid1d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x72e3, 0x72e6); SET_BC(0x2034); // b = essence $20, c = position $34
  miscPuzzles_subid1bcd_common(gb, sp0_); return;
}

// Play "puzzle solved" sound after navigating eyeball puzzle in final dungeon
void miscPuzzles_subid1e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7308, returnIfScrollMode01Unset_hook, 0x26e4, 0x730b);
  CYC(0x730b, 0x730e); A = W8(wScreenTransitionDirection);
  CYC(0x730e, 0x730f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x730f, 0x7312); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x730f, 0x7312);
  CYC(0x7312, 0x7314); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x7314, playSound_b00_hook, 0x0c98, 0x7317);
  CYC(0x7317, 0x731a); interactionDelete_hook(gb); return; // jp
}

// Checks if Link gets stuck in the d5 boss key puzzle, resets the room if so
void miscPuzzles_subid1f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x731a, 0x731c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x731c, 0x731d); A = mem_rd(gb, DE);
  {
    CYC(0x731d, 0x731e); push_effect(gb, 0x731e);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == 0x7324) goto subid1f_state1;
    if (target == 0x737c) goto subid1f_state2;
    interactionIncState_hook(gb); return; // target == 0x23e0
  }

subid1f_state1:
  CALL_C(0x7324, interactionDecCounter1_hook, 0x23cc, 0x7327);
  if (!(F & FZ)) { RET_TAKEN(0x7327); return; } // ret nz
  CYC(0x7327, 0x7328);
  CYC(0x7328, 0x732a); mem_wr(gb, HL, 30);
  CYC(0x732a, 0x732d); SET_HL(w1Link_yh);
  CYC(0x732d, 0x732e); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x732e, 0x7330); alu_and(gb, 0xf0);
  CYC(0x7330, 0x7331); B = A;
  CYC(0x7331, 0x7332); L = alu_inc8(gb, L); // inc l (now w1Link_xh)
  CYC(0x7332, 0x7333); A = mem_rd(gb, HL);
  CYC(0x7333, 0x7335); alu_and(gb, 0xf0);
  CYC(0x7335, 0x7337); A = alu_swap(gb, A);
  CYC(0x7337, 0x7338); alu_or(gb, B);
  CYC(0x7338, 0x7339); E = A;
  CYC(0x7339, 0x733a); push_effect(gb, DE); // push de
  CYC(0x733a, 0x733d); SET_HL(0x7374); // @offsetsToCheck
  CYC(0x733d, 0x733f); D = 0x08;

subid1f_checkNextOffset:
  CYC(0x733f, 0x7340); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7340, 0x7341); alu_add(gb, E);
  CYC(0x7341, 0x7342); C = A;
  CYC(0x7342, 0x7344); B = (uint8_t)(wRoomCollisions >> 8);
  CYC(0x7344, 0x7345); A = mem_rd(gb, BC);
  CYC(0x7345, 0x7346); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7346, 0x7348); goto subid1f_doneCheckingIfTrapped; } // jr z
  CYC(0x7346, 0x7348);
  CYC(0x7348, 0x734a); alu_bit(gb, 0, D);
  if (!(F & FZ)) { CYCT(0x734a, 0x734c); goto subid1f_decD; } // jr nz
  CYC(0x734a, 0x734c);
  CYC(0x734c, 0x734e); B = (uint8_t)(wRoomLayout >> 8);
  CYC(0x734e, 0x734f); A = mem_rd(gb, BC);
  CYC(0x734f, 0x7350); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7350, 0x7352); goto subid1f_decD; } // jr nz
  CYC(0x7350, 0x7352);
  CYC(0x7352, 0x7353); SET_HL(HL + 1); // inc hl
  CYC(0x7353, 0x7354); D = alu_dec8(gb, D); // dec d

subid1f_decD:
  CYC(0x7354, 0x7355); D = alu_dec8(gb, D); // dec d
  if (!(F & FZ)) { CYCT(0x7355, 0x7357); goto subid1f_checkNextOffset; } // jr nz
  CYC(0x7355, 0x7357);

subid1f_doneCheckingIfTrapped:
  CYC(0x7357, 0x7358); A = D;
  CYC(0x7358, 0x7359); SET_DE(pop_effect(gb)); // pop de
  CYC(0x7359, 0x735a); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x735a); return; } // ret nz
  CYC(0x735a, 0x735b);
  CALL_C(0x735b, checkLinkVulnerable_hook, 0x1d28, 0x735e);
  if (!(F & FC)) { RET_TAKEN(0x735e); return; } // ret nc
  CYC(0x735e, 0x735f);
  CYC(0x735f, 0x7361); A = 0x01; // DISABLE_LINK
  CYC(0x7361, 0x7364); W8(wMenuDisabled) = A;
  CYC(0x7364, 0x7367); W8(wDisabledObjects) = A;
  CYC(0x7367, 0x7369); A = 0x5a; // SND_ERROR
  CALL_C(0x7369, playSound_b00_hook, 0x0c98, 0x736c);
  CYC(0x736c, 0x736e); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x736e, 0x7370); A = 60;
  CYC(0x7370, 0x7371); mem_wr(gb, DE, A);
  CYC(0x7371, 0x7374); interactionIncState_hook(gb); return; // jp

subid1f_state2:
  CALL_C(0x737c, interactionDecCounter1_hook, 0x23cc, 0x737f);
  if (!(F & FZ)) { RET_TAKEN(0x737f); return; } // ret nz
  CYC(0x737f, 0x7380);
  CYC(0x7380, 0x7381); alu_xor(gb, A);
  CYC(0x7381, 0x7384); W8(wMenuDisabled) = A;
  CYC(0x7384, 0x7387); W8(wDisabledObjects) = A;
  CYC(0x7387, 0x738a); SET_HL(0x738d); // @warpDest
  CYC(0x738a, 0x738d); setWarpDestVariables_hook(gb); return; // jp
}

// Money in sidescrolling room in Hero's Cave
void miscPuzzles_subid20_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7392, getThisRoomFlags_hook, 0x197d, 0x7395);
  CYC(0x7395, 0x7397); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(0x7397, 0x7399); goto subid20_delete; } // jr nz
  CYC(0x7397, 0x7399);
  CYC(0x7399, 0x739c); SET_BC(0x2816); // TREASURE_OBJECT_RUPEES_16
  CALL_C(0x739c, createTreasure_hook, 0x27d4, 0x739f);
  if (!(F & FZ)) { CYCT(0x739f, 0x73a2); goto subid20_delete; } // jp nz
  CYC(0x739f, 0x73a2);
  CALL_C(0x73a2, objectCopyPosition_hook, 0x2242, 0x73a5);

subid20_delete:
  CYC(0x73a5, 0x73a8); interactionDelete_hook(gb); return; // jp
}

// Creates explosions while screen is fading out; used in some cutscene?
void miscPuzzles_subid21_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x73a8, checkInteractionState_hook, 0x23fe, 0x73ab);
  if (F & FZ) { CYCT(0x73ab, 0x73ad); goto subid21_state0; } // jr z
  CYC(0x73ab, 0x73ad);
  CYC(0x73ad, 0x73b0); A = W8(wPaletteThread_mode);
  CYC(0x73b0, 0x73b1); alu_or(gb, A);
  if (F & FZ) { CYCT(0x73b1, 0x73b4); interactionDelete_hook(gb); return; } // jp z
  CYC(0x73b1, 0x73b4);
  CYC(0x73b4, 0x73b7); A = W8(wFrameCounter);
  CYC(0x73b7, 0x73b8); B = A;
  CYC(0x73b8, 0x73ba); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(0x73ba); return; } // ret nz
  CYC(0x73ba, 0x73bb);
  CYC(0x73bb, 0x73bc); A = B;
  CYC(0x73bc, 0x73be); alu_and(gb, 0x70);
  CYC(0x73be, 0x73c0); A = alu_swap(gb, A);
  CYC(0x73c0, 0x73c3); SET_HL(0x73d0); // @explosionPositions
  CYC(0x73c3, 0x73c4); miscPuzzles_add_double_index(gb, 0x73c4);
  CYC(0x73c4, 0x73c5); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x73c5, 0x73c6); B = A;
  CYC(0x73c6, 0x73c7); C = mem_rd(gb, HL); // ld c,(hl)
  CALL_C(0x73c7, getFreeInteractionSlot_hook, 0x3aef, 0x73ca);
  if (!(F & FZ)) { RET_TAKEN(0x73ca); return; } // ret nz
  CYC(0x73ca, 0x73cb);
  CYC(0x73cb, 0x73cd); mem_wr(gb, HL, 0x56); // INTERAC_EXPLOSION
  CYC(0x73cd, 0x73d0); objectCopyPositionWithOffset_hook(gb); return; // jp

subid21_state0:
  CALL_C(0x73e0, interactionIncState_hook, 0x23e0, 0x73e3);
  CYC(0x73e3, 0x73e5); A = 0x04;
  CYC(0x73e5, 0x73e8); fadeoutToWhiteWithDelay_hook(gb); return; // jp
}

// Shared helper: called with `call`; if the room's item flag is already set, discards
// its own return address and jumps into interactionDelete instead of returning, so the
// caller's remaining code never runs. Otherwise a plain `ret`.
void miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x73e8, getThisRoomFlags_hook, 0x197d, 0x73eb);
  CYC(0x73eb, 0x73ed); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (F & FZ) { RET_TAKEN(0x73ed); return; } // ret z
  CYC(0x73ed, 0x73ee);
  CYC(0x73ee, 0x73ef); SET_HL(pop_effect(gb)); // pop hl (discard return address)
  CYC(0x73ef, 0x73f2); interactionDelete_hook(gb); return; // jp
}

// Shared helper: deletes self if the room's item flag is set, else increments state.
void miscPuzzles_deleteSelfOrIncStateIfItemFlagSet_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x73f2, getThisRoomFlags_hook, 0x197d, 0x73f5);
  CYC(0x73f5, 0x73f7); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(0x73f7, 0x73fa); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x73f7, 0x73fa);
  CYC(0x73fa, 0x73fd); interactionIncState_hook(gb); return; // jp
}

// Shared helper: deletes self if room flag bit 7 is set, else increments state.
void miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x73fd, getThisRoomFlags_hook, 0x197d, 0x7400);
  CYC(0x7400, 0x7402); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x7402, 0x7405); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x7402, 0x7405);
  CYC(0x7405, 0x7408); interactionIncState_hook(gb); return; // jp
}

// Shared helper (unused): deletes self if room flag bit 6 is set, else increments state.
void miscPuzzles_deleteSelfOrIncStateIfRoomFlag6Set_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7408, getThisRoomFlags_hook, 0x197d, 0x740b);
  CYC(0x740b, 0x740d); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x740d, 0x7410); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x740d, 0x7410);
  CYC(0x7410, 0x7413); interactionIncState_hook(gb); return; // jp
}
