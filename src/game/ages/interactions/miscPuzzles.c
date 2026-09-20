#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode90);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == SYM(miscPuzzles_subid00)) { miscPuzzles_subid00_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid01)) { miscPuzzles_subid01_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid02)) { miscPuzzles_subid02_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid03)) { miscPuzzles_subid03_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid04)) { miscPuzzles_subid04_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid05)) { miscPuzzles_subid05_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid07)) { miscPuzzles_subid07_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid08)) { miscPuzzles_subid08_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid09)) { miscPuzzles_subid09_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid0a)) { miscPuzzles_subid0a_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid0b)) { miscPuzzles_subid0b_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid0c)) { miscPuzzles_subid0c_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid0d)) { miscPuzzles_subid0d_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid0e)) { miscPuzzles_subid0e_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid0f)) { miscPuzzles_subid0f_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid10)) { miscPuzzles_subid10_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid11)) { miscPuzzles_subid11_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid12)) { miscPuzzles_subid12_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid13)) { miscPuzzles_subid13_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid14)) { miscPuzzles_subid14_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid15)) { miscPuzzles_subid15_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid16)) { miscPuzzles_subid16_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid17)) { miscPuzzles_subid17_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid18)) { miscPuzzles_subid18_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid19)) { miscPuzzles_subid19_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid1a)) { miscPuzzles_subid1a_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid1b)) { miscPuzzles_subid1b_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid1c)) { miscPuzzles_subid1c_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid1d)) { miscPuzzles_subid1d_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid1e)) { miscPuzzles_subid1e_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid1f)) { miscPuzzles_subid1f_hook(gb); return; }
    if (target == SYM(miscPuzzles_subid20)) { miscPuzzles_subid20_hook(gb); return; }
    miscPuzzles_subid21_hook(gb); return; // target == 0x73a8
  }
}

// Boss key puzzle in D6
void miscPuzzles_subid00_hook(GB *gb) {
  BASE(miscPuzzles_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == b_+87) goto state2;
    if (target == b_+98) goto state3;
  }

  // miscPuzzles_subid00@state0
  CALL_C(b_+12, interactionIncState_hook, SYM(interactionIncState), b_+15);

  // miscPuzzles_subid00@state1
  CYC(b_+15, b_+18); SET_HL(wLever1PullDistance); // wLever1PullDistance
  CYC(b_+18, b_+20); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto checkAlreadyOpened; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+25); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+25); return; } // ret z
  CYC(b_+25, b_+26);

checkAlreadyOpened:
  CALL_C(b_+26, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+29);
  CYC(b_+29, b_+31); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto alreadyOpened; } // jr nz
  CYC(b_+31, b_+33);
  CALL_C(b_+33, interactionIncState_hook, SYM(interactionIncState), b_+36);
  CYC(b_+36, b_+38); L = 0x47; // Interaction.counter2
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto checkRng; } // jr nz
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x01);
  CYC(b_+44, b_+46); goto error; // jr

checkRng:
  CALL_C(b_+46, getRandomNumber_hook, SYM(getRandomNumber), b_+49);
  CYC(b_+49, b_+51); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(b_+51, b_+54); interactionIncState_hook(gb); return; } // jp z
  CYC(b_+51, b_+54);

error:
  CYC(b_+54, b_+56); A = 0x5a; // SND_ERROR
  CALL_C(b_+56, playSound_b00_hook, SYM(playSound_b00), b_+59);
  CYC(b_+59, b_+62); A = W8(wActiveTilePos);
  CYC(b_+62, b_+65); W8(wWarpDestPos) = A;
  CYC(b_+65, b_+68); SET_HL(wEnemyPlacement); // wTmpcec0
  CYC(b_+68, b_+70); B = 0x20;
  CALL_C(b_+70, clearMemory_hook, SYM(clearMemory), b_+73);
  CYC(b_+73, b_+76); SET_HL((SYM(twinrova_state1__subid00State2) + 21)); // roomInitialization.generateRandomBuffer
  CYC(b_+76, b_+78); E = 0x02; // bank of roomInitialization
  CALL_C(b_+78, interBankCall_hook, 0x008a, b_+81);
  CYC(b_+81, b_+84); SET_HL((SYM(patch_subid00__state0) + 61)); // objectData.objectData78db
  CYC(b_+84, b_+87); parseGivenObjectData_b00_hook(gb); return; // jp

state2:
  CYC(b_+87, b_+90); A = W8(wNumEnemies);
  CYC(b_+90, b_+91); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+91); return; } // ret nz
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+94); A = 0x01;
  CYC(b_+94, b_+96); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+96, b_+97); mem_wr(gb, DE, A);
  RET(b_+97); return; // ret

state3:
  CYC(b_+98, b_+100); A = 0x01;
  CYC(b_+100, b_+103); W8(wActiveTriggers) = A;
  CYC(b_+103, b_+106); SET_HL((SYM(interaction6b_subid05__substate1) + 32)); // agesInteractionsBank08.spawnChestAndDeleteSelf
  CYC(b_+106, b_+108); E = 0x08; // bank of agesInteractionsBank08
  CYC(b_+108, b_+111); interBankCall_hook(gb); return; // jp

alreadyOpened:
  CYC(b_+111, b_+113); A = 0x01;
  CYC(b_+113, b_+116); W8(wActiveTriggers) = A;
  CYC(b_+116, b_+119); interactionDelete_hook(gb); return; // jp
}

// Underwater switch hook puzzle in past d6
void miscPuzzles_subid01_hook(GB *gb) {
  BASE(miscPuzzles_subid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook, SYM(miscPuzzles_deleteSelfAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+21); // @diamondPositions
  CALL_C(b_+9, miscPuzzles_verifyTilesAtPositions_hook, SYM(miscPuzzles_verifyTilesAtPositions), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); SET_HL((SYM(interaction6b_subid05__substate1) + 32)); // agesInteractionsBank08.spawnChestAndDeleteSelf
  CYC(b_+16, b_+18); E = 0x08; // bank of agesInteractionsBank08
  CYC(b_+18, b_+21); interBankCall_hook(gb); return; // jp
}

// Spot to put a rolling colored block on in present d6
void miscPuzzles_subid02_hook(GB *gb) {
  BASE(miscPuzzles_subid02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+6);
  CYC(b_+6, b_+8); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+12); A = W8(wRotatingCubePos);
  CYC(b_+12, b_+13); alu_cp(gb, L);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } // ret nz
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+17); A = W8(wRotatingCubeColor);
  CYC(b_+17, b_+19); alu_and(gb, 0x03);
  CYC(b_+19, b_+20); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); C = L;
  CYC(b_+22, b_+24); A = 0xa0; // TILEINDEX_STANDARD_FLOOR
  CALL_C(b_+24, setTile_hook, SYM(setTile), b_+27);
  CYC(b_+27, b_+29); B = 0xce; // >wRoomCollisions
  CYC(b_+29, b_+31); A = 0x0f;
  CYC(b_+31, b_+32); mem_wr(gb, BC, A);
  CYC(b_+32, b_+34); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+34, playSound_b00_hook, SYM(playSound_b00), b_+37);
  CYC(b_+37, b_+40); interactionDelete_hook(gb); return; // jp
}

// Chest from solving colored cube puzzle in d6 (related to subid $02)
void miscPuzzles_subid03_hook(GB *gb) {
  BASE(miscPuzzles_subid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook, SYM(miscPuzzles_deleteSelfAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+21); // @wantedFloorTiles
  CALL_C(b_+9, miscPuzzles_verifyTilesAtPositions_hook, SYM(miscPuzzles_verifyTilesAtPositions), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); SET_HL((SYM(interaction6b_subid05__substate1) + 32)); // agesInteractionsBank08.spawnChestAndDeleteSelf
  CYC(b_+16, b_+18); E = 0x08; // bank of agesInteractionsBank08
  CYC(b_+18, b_+21); interBankCall_hook(gb); return; // jp
}

// @param hl Pointer to data. First byte is a tile index; then an arbitrary
// number of positions in the room where that tile should be; $ff to
// give a new tile index; $00 to stop.
// @param[out] zflag z if all tiles matched as expected.
void miscPuzzles_verifyTilesAtPositions_hook(GB *gb) {
  BASE(miscPuzzles_verifyTilesAtPositions);
  CYC(b_+0, b_+2); B = 0xcf; // >wRoomLayout

newTileIndex:
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); E = A;

nextTile:
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+9); return; } // ret z
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto newTileIndex; } // jr z
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, BC);
  CYC(b_+14, b_+15); alu_cp(gb, E);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; } // ret nz
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); goto nextTile; // jr
}

// Floor changer in present D6, triggered by orb
void miscPuzzles_subid04_hook(GB *gb) {
  BASE(miscPuzzles_subid04);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; } // jr z
  CYC(b_+3, b_+5);

  // miscPuzzles_subid04@state1
  CYC(b_+5, b_+8); A = W8(wToggleBlocksState);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+13); return; } // ret z
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+18); A = 0xff;
  CYC(b_+18, b_+21); W8(wDisabledObjects) = A;
  CYC(b_+21, b_+24); W8(wMenuDisabled) = A;
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); A = alu_inc8(gb, A);
  CYC(b_+28, b_+30); alu_and(gb, 0x01);
  CYC(b_+30, b_+31); B = A;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); C = 0x05;
  CYC(b_+34, b_+37); push_effect(gb, b_+37); goto spawnSubid; // call
afterSpawnSubid1:
  CYC(b_+37, b_+39); C = 0x06;
  CYC(b_+39, b_+42); push_effect(gb, b_+42); goto spawnSubid; // call
afterSpawnSubid2:
  CYC(b_+42, b_+45); SET_HL((SYM(interactionCode70__substate0__nextTile) + 32)); // bank16.loadD6ChangingFloorPatternToBigBuffer
  CYC(b_+45, b_+47); E = 0x16; // bank of bank16
  CALL_C(b_+47, interBankCall_hook, 0x008a, b_+50);
  RET(b_+50); return; // ret

spawnSubid:
  CALL_C(b_+51, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+54);
  if (!(F & FZ)) {
    RET_TAKEN(b_+54);
    if (gb->pc == b_+37 && gb->sp == sp0_) goto afterSpawnSubid1;
    if (gb->pc == b_+42 && gb->sp == sp0_) goto afterSpawnSubid2;
    return;
  } // ret nz
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x90); // INTERAC_MISC_PUZZLES
  CYC(b_+57, b_+58); L = alu_inc8(gb, L);
  CYC(b_+58, b_+59); mem_wr(gb, HL, C);
  CYC(b_+59, b_+60); L = alu_inc8(gb, L);
  CYC(b_+60, b_+61); mem_wr(gb, HL, B);
  RET(b_+61);
  if (gb->pc == b_+37 && gb->sp == sp0_) goto afterSpawnSubid1;
  if (gb->pc == b_+42 && gb->sp == sp0_) goto afterSpawnSubid2;
  return; // ret

state0:
  CYC(b_+62, b_+65); A = W8(wToggleBlocksState);
  CYC(b_+65, b_+67); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+67, b_+68); mem_wr(gb, DE, A);
  CYC(b_+68, b_+71); interactionIncState_hook(gb); return; // jp
}

// Helpers for floor changer (subid $04). subid05 and subid06 both target 0a:6e25 in
// interactionCode90's jump table -- identical code, so subid05 just hands off to subid06.
void miscPuzzles_subid06_hook(GB *gb) {
  BASE(miscPuzzles_subid05);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto substate1; } // jr nz
  CYC(b_+4, b_+6);

  // miscPuzzles_subid06@substate0
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_sub(gb, 0x05);
  CYC(b_+11, b_+12); alu_add(gb, A); // add a
  CYC(b_+12, b_+15); SET_HL(b_+26); // @data: var30/var31/var32/var33 init table (pure ROM data)
  CYC(b_+15, b_+16); miscPuzzles_add_double_index(gb, b_+16);
  CYC(b_+16, b_+18); B = 0x04;
  CYC(b_+18, b_+20); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CALL_C(b_+20, copyMemory_hook, SYM(copyMemory), b_+23);
  CYC(b_+23, b_+26); interactionIncSubstate_hook(gb); return; // jp

substate1:
  CYC(b_+34, b_+36); E = INTERACTION_BASE + 0x33; // Interaction.var33
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); L = A;
  CYC(b_+38, b_+40); H = 0xc3; // >wBigBuffer

nextTile:
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+41, b_+42); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+42, b_+44); goto deleteSelf; } // jr z
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto setTileLbl; } // jr nz
  CYC(b_+46, b_+48);

  CYC(b_+48, b_+50); E = INTERACTION_BASE + 0x32; // Interaction.var32
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); B = A;
  CYC(b_+52, b_+54); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+56); alu_add(gb, B);
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); E = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); alu_cpl(gb);
  CYC(b_+61, b_+62); A = alu_inc8(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+66); push_effect(gb, b_+66); goto nextRow;
afterNextRow_fromNextTile:
  CYC(b_+66, b_+68); goto nextTile; // jr

setTileLbl:
  CYC(b_+68, b_+70); H8(hFF8B) = A; // ldh ($ff8b),a
  CYC(b_+70, b_+72); E = INTERACTION_BASE + 0x33; // Interaction.var33
  CYC(b_+72, b_+73); A = L;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CYC(b_+74, b_+77); push_effect(gb, b_+77); goto nextRow;
afterNextRow_fromSetTile:
  CYC(b_+77, b_+79); A = H8(hFF8B); // ldh a,($ff8b)
  CYC(b_+79, b_+82); setTile_hook(gb); return; // jp

deleteSelf:
  CYC(b_+93, b_+94); alu_xor(gb, A);
  CYC(b_+94, b_+97); W8(wDisabledObjects) = A;
  CYC(b_+97, b_+100); W8(wMenuDisabled) = A;
  CYC(b_+100, b_+103); interactionDelete_hook(gb); return; // jp

  // [var30] += [var31]; shared by @nextTile and @setTile, both at top-level depth (sp0_).
nextRow:
  CYC(b_+82, b_+84); E = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); B = A;
  CYC(b_+86, b_+88); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); C = A;
  CYC(b_+90, b_+91); alu_add(gb, B);
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);
  RET(b_+92);
  if (gb->pc == b_+66 && gb->sp == sp0_) goto afterNextRow_fromNextTile;
  if (gb->pc == b_+77 && gb->sp == sp0_) goto afterNextRow_fromSetTile;
  return; // ret
}

void miscPuzzles_subid05_hook(GB *gb) {
  miscPuzzles_subid06_hook(gb);
}

// Wall retraction event after lighting torches in past d6
void miscPuzzles_subid07_hook(GB *gb) {
  BASE(miscPuzzles_subid07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; } // jr z
  CYC(b_+3, b_+5);

// miscPuzzles_subid07@state1
  CALL_C(b_+5, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } // ret nc
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); push_effect(gb, b_+12); goto checkLitTorches;
afterCheckLitTorches_fromState1:
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+16); return; } // ret z
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+25); SET_HL(b_+99); // @torchLightOrder (pure ROM data table)
  CYC(b_+25, b_+26); miscPuzzles_addAToHl_from_rst(gb, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto litWrongTorch; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+33, b_+36); interactionIncSubstate_hook(gb); return; } // jp c
  CYC(b_+33, b_+36);

  // Lit all torches
  CYC(b_+36, b_+38); A = 0x6f; // $ff ~ (DISABLE_ITEMS | DISABLE_ALL_BUT_INTERACTIONS)
  CYC(b_+38, b_+41); W8(wDisabledObjects) = A;
  CYC(b_+41, b_+44); W8(wMenuDisabled) = A;
  CYC(b_+44, b_+46); A = 0x0b; // CUTSCENE_WALL_RETRACTION
  CYC(b_+46, b_+49); W8(wCutsceneTrigger) = A;
  CALL_C(b_+49, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+52);
  CYC(b_+52, b_+54); L = 0x25; // <ROOM_AGES_525
  CYC(b_+54, b_+56); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)
  CYC(b_+56, b_+59); interactionDelete_hook(gb); return; // jp

litWrongTorch:
  CYC(b_+59, b_+60); alu_xor(gb, A);
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+63); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+66); A = 0x5a; // SND_ERROR
  CALL_C(b_+66, playSound_b00_hook, SYM(playSound_b00), b_+69);
  CYC(b_+69, b_+71); A = 0x08; // TILEINDEX_UNLIT_TORCH
  CYC(b_+71, b_+73); C = 0x31;
  CALL_C(b_+73, setTile_hook, SYM(setTile), b_+76);
  CYC(b_+76, b_+78); A = 0x08;
  CYC(b_+78, b_+80); C = 0x33;
  CALL_C(b_+80, setTile_hook, SYM(setTile), b_+83);
  CYC(b_+83, b_+85); A = 0x08;
  CYC(b_+85, b_+87); C = 0x35;
  CALL_C(b_+87, setTile_hook, SYM(setTile), b_+90);
  CYC(b_+90, b_+92); A = 0x08;
  CYC(b_+92, b_+94); C = 0x53;
  CALL_C(b_+94, setTile_hook, SYM(setTile), b_+97);
  CYC(b_+97, b_+99); goto makeTorchesLightable; // jr

state0:
  CALL_C(b_+103, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+106);
  CYC(b_+106, b_+108); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(b_+108, b_+111); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+108, b_+111);
  CALL_C(b_+111, interactionIncState_hook, SYM(interactionIncState), b_+114);
  CYC(b_+114, b_+117); push_effect(gb, b_+117); goto checkLitTorches;
afterCheckLitTorches_fromState0:
  CYC(b_+117, b_+118); A = B;
  CYC(b_+118, b_+120); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+120, b_+121); mem_wr(gb, DE, A);

makeTorchesLightable:
  CYC(b_+121, b_+124); push_effect(gb, b_+124); goto makeTorchesUnlightable;
afterMakeTorchesUnlightable:
  CYC(b_+124, b_+127); SET_HL((SYM(patch_subid00__state0) + 66)); // objectData.objectData_makeTorchesLightableForD6Room
  CYC(b_+127, b_+130); parseGivenObjectData_b00_hook(gb); return; // jp

  // Single caller (makeTorchesLightable, above), top-level depth sp0_.
makeTorchesUnlightable:
  CYC(b_+130, b_+133); SET_HL(w7d0c1); // FIRST_PART_INDEX, Part.id

makeTorchesUnlightable_loop:
  CYC(b_+133, b_+134); A = mem_rd(gb, HL);
  CYC(b_+134, b_+136); alu_cp(gb, 0x06); // PART_LIGHTABLE_TORCH
  if (F & FZ) { CYCT(b_+136, b_+139); push_effect(gb, b_+139); goto deletePartObject; } // call z
  CYC(b_+136, b_+139);

afterDeletePartObject:
  CYC(b_+139, b_+140); H = alu_inc8(gb, H);
  CYC(b_+140, b_+141); A = H;
  CYC(b_+141, b_+143); alu_cp(gb, 0xe0); // LAST_PART_INDEX+1
  if (F & FC) { CYCT(b_+143, b_+145); goto makeTorchesUnlightable_loop; } // jr c
  CYC(b_+143, b_+145);
  RET(b_+145);
  if (gb->pc == b_+124 && gb->sp == sp0_) goto afterMakeTorchesUnlightable;
  return; // ret

  // Single caller (makeTorchesUnlightable_loop, above); entered one call deep since
  // makeTorchesUnlightable's own return address 0x6f08 is still outstanding on the stack.
deletePartObject:
  CYC(b_+146, b_+147); push_effect(gb, HL); // push hl
  CYC(b_+147, b_+148); L = alu_dec8(gb, L);
  CYC(b_+148, b_+150); B = 0x40;
  CALL_C(b_+150, clearMemory_hook, SYM(clearMemory), b_+153);
  CYC(b_+153, b_+154); SET_HL(pop_effect(gb)); // pop hl
  RET(b_+154);
  if (gb->pc == b_+139 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterDeletePartObject;
  return; // ret

  // @param[out] b Bitset of lit torches, bits 0-3. Two callers, state0 and state1 above,
  // both at top-level depth sp0_; has two exit points: early ret nz, and the final ret.
checkLitTorches:
  CYC(b_+155, b_+157); A = 0x09; // TILEINDEX_LIT_TORCH
  CYC(b_+157, b_+159); B = 0x00;
  CYC(b_+159, b_+162); SET_HL(wRoomLayout + 0x31);
  CYC(b_+162, b_+163); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+163, b_+165); goto checkLitTorches_L6f33; } // jr nz
  CYC(b_+163, b_+165);
  CYC(b_+165, b_+167); B |= (1 << 0); // set 0,b

checkLitTorches_L6f33:
  CYC(b_+167, b_+169); L = 0x33;
  CYC(b_+169, b_+170); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+170, b_+172); goto checkLitTorches_L6f3a; } // jr nz
  CYC(b_+170, b_+172);
  CYC(b_+172, b_+174); B |= (1 << 1); // set 1,b

checkLitTorches_L6f3a:
  CYC(b_+174, b_+176); L = 0x53;
  CYC(b_+176, b_+177); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+177, b_+179); goto checkLitTorches_L6f41; } // jr nz
  CYC(b_+177, b_+179);
  CYC(b_+179, b_+181); B |= (1 << 2); // set 2,b

checkLitTorches_L6f41:
  CYC(b_+181, b_+183); L = 0x35;
  CYC(b_+183, b_+184); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    RET_TAKEN(b_+184);
    if (gb->pc == b_+12 && gb->sp == sp0_) goto afterCheckLitTorches_fromState1;
    if (gb->pc == b_+117 && gb->sp == sp0_) goto afterCheckLitTorches_fromState0;
    return;
  } // ret nz
  CYC(b_+184, b_+185);
  CYC(b_+185, b_+187); B |= (1 << 3); // set 3,b
  RET(b_+187);
  if (gb->pc == b_+12 && gb->sp == sp0_) goto afterCheckLitTorches_fromState1;
  if (gb->pc == b_+117 && gb->sp == sp0_) goto afterCheckLitTorches_fromState0;
  return; // ret
}

// Checks to set the "bombable wall open" bit in d6 (north)
void miscPuzzles_subid08_hook(GB *gb) {
  BASE(miscPuzzles_subid08);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 0, mem_rd(gb, HL)); // ROOMFLAG_BIT_KEYDOOR_UP
  if (F & FZ) { RET_TAKEN(b_+8); return; } // ret z
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = 0x19; // <ROOM_AGES_519
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01); // set 0,(hl)
  CYC(b_+13, b_+16); interactionDelete_hook(gb); return; // jp
}

// Checks to set the "bombable wall open" bit in d6 (east)
void miscPuzzles_subid09_hook(GB *gb) {
  BASE(miscPuzzles_subid09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 1, mem_rd(gb, HL)); // ROOMFLAG_BIT_KEYDOOR_RIGHT
  if (F & FZ) { RET_TAKEN(b_+8); return; } // ret z
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = 0x26; // <ROOM_AGES_526
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02); // set 1,(hl)
  CYC(b_+13, b_+16); interactionDelete_hook(gb); return; // jp
}

// Jabu-jabu water level controller script, in the room with the 3 buttons
void miscPuzzles_subid0a_hook(GB *gb) {
  BASE(miscPuzzles_subid0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == b_+29) goto state1;
    if (target == b_+112) goto state2;
    if (target == b_+130) goto state3;
  }

  // miscPuzzles_subid0a@state0
  CYC(b_+12, b_+15); A = W8(wActiveTriggers);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+21); A = W8(wJabuWaterLevel);
  CYC(b_+21, b_+23); alu_and(gb, 0xf0);
  CYC(b_+23, b_+26); W8(wSwitchState) = A;
  CYC(b_+26, b_+29); interactionIncState_hook(gb); return; // jp

state1:
  CYC(b_+29, b_+32); A = W8(wActiveTriggers);
  CYC(b_+32, b_+33); B = A;
  CYC(b_+33, b_+35); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); alu_xor(gb, B);
  CYC(b_+37, b_+38); C = A;
  CYC(b_+38, b_+39); A = B;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+42); alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto drainWater; } // jr nz
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); alu_and(gb, C);
  if (F & FZ) { RET_TAKEN(b_+45); return; } // ret z
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+49); A = W8(wSwitchState);
  CYC(b_+49, b_+50); alu_and(gb, C);
  if (!(F & FZ)) { RET_TAKEN(b_+50); return; } // ret nz
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+52); A = C;
  CYC(b_+52, b_+55); SET_HL(wSwitchState);
  CYC(b_+55, b_+56); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+56, b_+57); mem_wr(gb, HL, A);
  CYC(b_+57, b_+59); alu_and(gb, 0xf0);
  CYC(b_+59, b_+60); B = A;
  CYC(b_+60, b_+63); SET_HL(wJabuWaterLevel);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+66); alu_and(gb, 0x03);
  CYC(b_+66, b_+67); A = alu_inc8(gb, A);
  CYC(b_+67, b_+68); alu_or(gb, B);
  CYC(b_+68, b_+69); mem_wr(gb, HL, A);
  CYC(b_+69, b_+71); A = 0x09; // <TX_1209
  CYC(b_+71, b_+73); goto beginCutscene; // jr

drainWater:
  CYC(b_+73, b_+76); A = W8(wJabuWaterLevel);
  CYC(b_+76, b_+78); alu_and(gb, 0x07);
  if (F & FZ) { RET_TAKEN(b_+78); return; } // ret z
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+80); alu_xor(gb, A);
  CYC(b_+80, b_+83); W8(wJabuWaterLevel) = A;
  CYC(b_+83, b_+86); W8(wSwitchState) = A;
  CYC(b_+86, b_+88); A = 0x08; // <TX_1208

beginCutscene:
  CYC(b_+88, b_+90); E = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+93); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(b_+93, b_+96); W8(wDisabledObjects) = A;
  CYC(b_+96, b_+99); W8(wMenuDisabled) = A;
  CYC(b_+99, b_+101); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+101, b_+103); A = 60;
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CYC(b_+104, b_+106); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+106, playSound_b00_hook, SYM(playSound_b00), b_+109);
  CYC(b_+109, b_+112); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(b_+112, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+115);
  if (!(F & FZ)) { RET_TAKEN(b_+115); return; } // ret nz
  CYC(b_+115, b_+116);
  CYC(b_+116, b_+118); A = 0xf0;
  CYC(b_+118, b_+119); mem_wr(gb, HL, A);
  CALL_C(b_+119, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+122);
  CYC(b_+122, b_+124); A = 0xc2; // SND_FLOODGATES
  CALL_C(b_+124, playSound_b00_hook, SYM(playSound_b00), b_+127);
  CYC(b_+127, b_+130); interactionIncState_hook(gb); return; // jp

state3:
  CALL_C(b_+130, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+133);
  if (!(F & FZ)) { RET_TAKEN(b_+133); return; } // ret nz
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+136); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x01);
  CYC(b_+138, b_+139); alu_xor(gb, A);
  CYC(b_+139, b_+142); W8(wDisabledObjects) = A;
  CYC(b_+142, b_+145); W8(wMenuDisabled) = A;
  CYC(b_+145, b_+147); B = 0x12; // >TX_1200
  CYC(b_+147, b_+149); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+149, b_+150); C = mem_rd(gb, HL);
  CALL_C(b_+150, showText_hook, SYM(showText), b_+153);
  CYC(b_+153, b_+155); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+155, playSound_b00_hook, SYM(playSound_b00), b_+158);
  CYC(b_+158, b_+161); A = W8(wActiveMusic);
  CYC(b_+161, b_+164); playSound_b00_hook(gb); return; // jp
}

// Ladder spawner in d7 miniboss room
void miscPuzzles_subid0b_hook(GB *gb) {
  BASE(miscPuzzles_subid0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == b_+10) goto state1;
    if (target == b_+32) goto state2;
  }
  miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(gb); return; // target == 0x73fd

state1:
  CYC(b_+10, b_+13); A = W8(wNumEnemies);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CALL_C(b_+15, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+18);
  CYC(b_+18, b_+20); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(b_+20, b_+22); L = 0x4d; // <ROOM_AGES_54d
  CYC(b_+22, b_+24); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+28); A = 0x08;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+32); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(b_+32, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+35);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; } // ret nz
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x08);
  CALL_C(b_+38, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+41);
  CYC(b_+41, b_+42); C = L;
  CYC(b_+42, b_+43); A = C;
  CYC(b_+43, b_+45); H8(hFF92) = A; // ldh ($ff92),a
  CYC(b_+45, b_+47); A = 0x18; // TILEINDEX_SS_LADDER
  CALL_C(b_+47, setTile_hook, SYM(setTile), b_+50);
  CYC(b_+50, b_+52); B = 0x05; // INTERAC_PUFF
  CALL_C(b_+52, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+55);
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+60); alu_add(gb, 0x10);
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+63); A = H8(hFF92); // ldh a,($ff92)
  CYC(b_+63, b_+65); alu_cp(gb, 0x90);
  if (F & FC) { RET_TAKEN(b_+65); return; } // ret c
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); C = 0x80;
  CYC(b_+68, b_+70); A = 0x52; // TILEINDEX_SS_52
  CALL_C(b_+70, setTile_hook, SYM(setTile), b_+73);
  CYC(b_+73, b_+75); C = 0x90;
  CYC(b_+75, b_+77); A = 0x01; // TILEINDEX_SS_EMPTY
  CALL_C(b_+77, setTile_hook, SYM(setTile), b_+80);
  CYC(b_+80, b_+82); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+82, playSound_b00_hook, SYM(playSound_b00), b_+85);
  CYC(b_+85, b_+86); alu_xor(gb, A);
  CYC(b_+86, b_+89); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+89, b_+92); interactionDelete_hook(gb); return; // jp
}

// Shared with subid19, which reaches this via `jp` from a fully independent call
// site -- must stand alone with no dependency on subid0c's context.
void miscPuzzles_dropSmallKeyHere_hook(GB *gb) {
  BASE(miscPuzzles_dropSmallKeyHere);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x3001); // TREASURE_SMALL_KEY, $01
  CALL_C(b_+3, createTreasure_hook, SYM(createTreasure), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; } // ret nz
  CYC(b_+6, b_+7);
  CALL_C(b_+7, objectCopyPosition_hook, SYM(objectCopyPosition), b_+10);
  CYC(b_+10, b_+13); interactionDelete_hook(gb); return; // jp
}

// Switch hook puzzle early in d7 for a small key
void miscPuzzles_subid0c_hook(GB *gb) {
  BASE(miscPuzzles_subid0c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook, SYM(miscPuzzles_deleteSelfAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+9); SET_HL(SYM(miscPuzzles_subid0c_wantedTiles)); // miscPuzzles_subid0c_wantedTiles (pure ROM data, no C rep needed)
  CALL_C(b_+9, miscPuzzles_verifyTilesAtPositions_hook, SYM(miscPuzzles_verifyTilesAtPositions), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  miscPuzzles_dropSmallKeyHere_hook(gb); return; // falls through
}

// Staircase spawner after moving first set of stone panels in d8
void miscPuzzles_subid0d_hook(GB *gb) {
  BASE(miscPuzzles_subid0d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == b_+49) goto state1;
    if (target == b_+71) goto state2;
  }

  // miscPuzzles_subid0d@state0
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x40); // ROOMFLAG_40
  if (!(F & FZ)) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+21); A = W8(wNumTorchesLit);
  CYC(b_+21, b_+23); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; } // ret nz
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+27); SET_HL(wActiveTriggers);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; } // ret nz
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+35); A = 0x1e;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38); A = 0x08;
  CALL_C(b_+38, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+41);
  CYC(b_+41, b_+43); A = 0x70; // SND_DOORCLOSE
  CALL_C(b_+43, playSound_b00_hook, SYM(playSound_b00), b_+46);
  CYC(b_+46, b_+49); interactionIncState_hook(gb); return; // jp

state1:
  CALL_C(b_+49, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+52);
  if (!(F & FZ)) { RET_TAKEN(b_+52); return; } // ret nz
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+56); SET_HL(wActiveTriggers);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+59); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+59, b_+61); goto l70ca; } // jr z
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+63, b_+64); alu_xor(gb, A);
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);
  RET(b_+65); return; // ret

l70ca:
  CYC(b_+66, b_+68); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(b_+68, b_+71); interactionIncState_hook(gb); return; // jp

state2:
  CYC(b_+71, b_+74); A = W8(wActiveTriggers);
  CYC(b_+74, b_+76); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+76); return; } // ret nz
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+79, playSound_b00_hook, SYM(playSound_b00), b_+82);
  CYC(b_+82, b_+84); B = 0x05; // INTERAC_PUFF
  CALL_C(b_+84, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+87);
  CALL_C(b_+87, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+90);
  CYC(b_+90, b_+91); C = L;
  CYC(b_+91, b_+93); A = 0x52; // TILEINDEX_NORTH_STAIRS
  CALL_C(b_+93, setTile_hook, SYM(setTile), b_+96);
  CYC(b_+96, b_+99); interactionDelete_hook(gb); return; // jp
}

// Staircase spawner after putting in slates in d8
void miscPuzzles_subid0e_hook(GB *gb) {
  BASE(miscPuzzles_subid0e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); goto state1; } // jp nz
  CYC(b_+3, b_+6);

  // miscPuzzles_subid0e@state0
  CALL_C(b_+6, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+9);
  CYC(b_+9, b_+11); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+11, b_+14); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_and(gb, 0x0f); // ROOMFLAG_01|02|04|08
  CYC(b_+17, b_+19); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+23); SET_HL(wActiveTriggers);
  CYC(b_+23, b_+25); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(b_+25, b_+28); interactionIncState_hook(gb); return; // jp

state1:
  CYC(b_+28, b_+31); A = W8(wActiveTriggers);
  CYC(b_+31, b_+33); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39);
  CYC(b_+39, b_+41); B = 0x05; // INTERAC_PUFF
  CALL_C(b_+41, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+44);
  CALL_C(b_+44, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+47);
  CYC(b_+47, b_+48); C = L;
  CYC(b_+48, b_+50); A = 0x52; // TILEINDEX_NORTH_STAIRS
  CALL_C(b_+50, setTile_hook, SYM(setTile), b_+53);
  CYC(b_+53, b_+56); interactionDelete_hook(gb); return; // jp
}

// Octogon boss initialization (in the room just before the boss)
void miscPuzzles_subid0f_hook(GB *gb) {
  BASE(miscPuzzles_subid0f);
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_octogonBoss_loadedExtraGfx);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [var03] = 0
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [direction] = $ff
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x28); // [health]
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x28); // [y]
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x78); // [x]
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); // [var30] = $ff
  CYC(b_+18, b_+21); interactionDelete_hook(gb); return; // jp
}

// Something at the top of Talus Peaks?
void miscPuzzles_subid10_hook(GB *gb) {
  BASE(miscPuzzles_subid10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_patchMinigame_fixingSword);
  CYC(b_+3, b_+5); B = 0x08;
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CYC(b_+8, b_+11); interactionDelete_hook(gb); return; // jp
}

// D5 keyhole opening
void miscPuzzles_subid11_hook(GB *gb) {
  BASE(miscPuzzles_subid11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); interactionRunScript_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CALL_C(b_+6, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+9);
  CALL_C(b_+9, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(b_+14, b_+17); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+18); push_effect(gb, DE); // push de
  CALL_C(b_+18, reloadTileMap_hook, SYM(reloadTileMap), b_+21);
  CYC(b_+21, b_+22); SET_DE(pop_effect(gb)); // pop de
  CYC(b_+22, b_+25); SET_HL((SYM(twinrova_state1__runOtherHalf) + 15)); // mainScripts.miscPuzzles_crownDungeonOpeningScript
  miscPuzzles_setScriptAndIncState_hook(gb); return; // falls into miscPuzzles_setScriptAndIncState
}

// Shared tail helper: also reached by tail-jump (jr) from subid12/subid13.
void miscPuzzles_setScriptAndIncState_hook(GB *gb) {
  BASE(miscPuzzles_setScriptAndIncState);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionSetScript_hook, SYM(interactionSetScript), b_+3);
  CALL_C(b_+3, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+6);
  CYC(b_+6, b_+9); interactionIncState_hook(gb); return; // jp
}

// D6 present/past keyhole opening
void miscPuzzles_subid12_hook(GB *gb) {
  BASE(miscPuzzles_subid12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); interactionRunScript_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CALL_C(b_+6, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(b_+11, b_+14); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); SET_HL((SYM(twinrova_state1__runSubid06) + 2)); // mainScripts.miscPuzzles_mermaidsCaveDungeonOpeningScript
  CYC(b_+17, b_+19); miscPuzzles_setScriptAndIncState_hook(gb); return; // jr
}

// Eyeglass library keyhole opening
void miscPuzzles_subid13_hook(GB *gb) {
  BASE(miscPuzzles_subid13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); interactionRunScript_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CALL_C(b_+6, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(b_+11, b_+14); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); SET_HL((SYM(twinrova_loadScript) + 4)); // mainScripts.miscPuzzles_eyeglassLibraryOpeningScript
  CYC(b_+17, b_+19); miscPuzzles_setScriptAndIncState_hook(gb); return; // jr
}

// Spot to put a rolling colored block on in Hero's Cave
void miscPuzzles_subid14_hook(GB *gb) {
  BASE(miscPuzzles_subid14);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CALL_C(b_+6, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+9);
  CYC(b_+9, b_+11); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(b_+11, b_+13); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(b_+13); return; } // ret nc
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+18); A = W8(wRotatingCubePos);
  CYC(b_+18, b_+19); alu_cp(gb, L);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+23); A = W8(wRotatingCubeColor);
  CYC(b_+23, b_+25); alu_and(gb, 0x03);
  CYC(b_+25, b_+26); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } // ret nz
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); C = L;
  CYC(b_+28, b_+31); SET_HL(wActiveTriggers);
  CYC(b_+31, b_+32); A = B;
  CALL_C(b_+32, setFlag_hook, SYM(setFlag), b_+35);
  CYC(b_+35, b_+37); A = 0xa3;
  CALL_C(b_+37, setTile_hook, SYM(setTile), b_+40);
  CYC(b_+40, b_+42); B = 0xce; // >wRoomCollisions
  CYC(b_+42, b_+44); A = 0x0f;
  CYC(b_+44, b_+45); mem_wr(gb, BC, A);
  CYC(b_+45, b_+47); A = 0x50; // SND_CLINK
  CYC(b_+47, b_+50); playSound_b00_hook(gb); return; // jp
}

// Stairs from solving colored cube puzzle in Hero's Cave (related to subid $14)
void miscPuzzles_subid15_hook(GB *gb) {
  BASE(miscPuzzles_subid15);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); A = W8(wActiveTriggers);
  CYC(b_+9, b_+11); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } // ret nz
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+14, playSound_b00_hook, SYM(playSound_b00), b_+17);
  CYC(b_+17, b_+19); A = 0x45; // TILEINDEX_INDOOR_DOWNSTAIRCASE
  CYC(b_+19, b_+21); C = 0x15;
  CALL_C(b_+21, setTile_hook, SYM(setTile), b_+24);
  CALL_C(b_+24, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(b_+29, b_+32); interactionDelete_hook(gb); return; // jp
}

// Warps Link out of Hero's Cave upon opening the chest
void miscPuzzles_subid16_hook(GB *gb) {
  BASE(miscPuzzles_subid16);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == b_+10) goto state1;
    if (target == b_+19) goto state2;
    // target == 0x73f2 (miscPuzzles_deleteSelfOrIncStateIfItemFlagSet, bare global elsewhere)
    miscPuzzles_deleteSelfOrIncStateIfItemFlagSet_hook(gb);
    return;
  }

state1:
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (F & FZ) { RET_TAKEN(b_+15); return; } // ret z
  CYC(b_+15, b_+16);
  CALL_C(b_+16, interactionIncState_hook, SYM(interactionIncState), b_+19);
  // falls through into state2

state2:
  CYC(b_+19, b_+21); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(b_+21, b_+24); W8(wDisabledObjects) = A;
  CYC(b_+24, b_+27); W8(wDisableLinkCollisionsAndMenu) = A;
  CALL_C(b_+27, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+30);
  CYC(b_+30, b_+33); SET_HL(b_+39); // @warpDestData
  CALL_C(b_+33, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+36);
  CYC(b_+36, b_+39); interactionDelete_hook(gb); return; // jp
}

// Enables portal in Hero's Cave first room if its other end is active
void miscPuzzles_subid17_hook(GB *gb) {
  BASE(miscPuzzles_subid17);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+4); push_effect(gb, HL); // push hl
  CYC(b_+4, b_+6); L = 0xc9; // <ROOM_AGES_4c9
  CYC(b_+6, b_+8); alu_bit(gb, 5, mem_rd(gb, HL)); // ROOMFLAG_BIT_ITEM
  CYC(b_+8, b_+9); SET_HL(pop_effect(gb)); // pop hl
  if (F & FZ) { CYCT(b_+9, b_+11); goto subid17_skip; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)

subid17_skip:
  CYC(b_+13, b_+16); interactionDelete_hook(gb); return; // jp
}

// Drops a key in hero's cave block-pushing puzzle
void miscPuzzles_subid18_hook(GB *gb) {
  BASE(miscPuzzles_subid18);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); miscPuzzles_deleteSelfOrIncStateIfItemFlagSet_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x95);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_cp(gb, 0x2a); // TILEINDEX_PUSHABLE_STATUE
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); L = 0x5d;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_cp(gb, 0x2a); // TILEINDEX_PUSHABLE_STATUE
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; } // ret nz
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+22); miscPuzzles_dropSmallKeyHere_hook(gb); return; // jp
}

// Bridge controller in d5 room after the miniboss
void miscPuzzles_subid19_hook(GB *gb) {
  BASE(miscPuzzles_subid19);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == b_+14) goto subid19_state1;
    if (target == b_+27) goto subid19_state2;
    if (target == b_+71) goto subid19_state3;
    if (target == b_+79) goto subid19_state4;
    interactionIncState_hook(gb); return; // target == 0x23e0
  }

subid19_state1:
  CYC(b_+14, b_+17); A = W8(wActiveTriggers);
  CYC(b_+17, b_+18); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+18); return; } // ret nc
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+23); A = 0x08;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+27); interactionIncState_hook(gb); return; // jp

subid19_state2:
  CYC(b_+27, b_+30); A = W8(wActiveTriggers);
  CYC(b_+30, b_+31); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+31, b_+33); goto subid19_releasedTrigger; } // jr nc
  CYC(b_+31, b_+33);
  CALL_C(b_+33, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; } // ret nz
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x08);
  CYC(b_+39, b_+42); SET_HL(wRoomLayout + 0x55);

subid19_state2_loop:
  CYC(b_+42, b_+43); C = L;
  CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+44, b_+46); alu_cp(gb, 0xf4); // TILEINDEX_BLANK_HOLE
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto subid19_state2_advance; } // jr nz
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); A = 0x6d; // TILEINDEX_HORIZONTAL_BRIDGE
  CALL_C(b_+50, setTileInAllBuffers_hook, SYM(setTileInAllBuffers), b_+53);
  CYC(b_+53, b_+55); A = 0x70; // SND_DOORCLOSE
  CYC(b_+55, b_+58); playSound_b00_hook(gb); return; // jp

subid19_state2_advance:
  CYC(b_+58, b_+59); A = L;
  CYC(b_+59, b_+61); alu_cp(gb, 0x5a);
  if (F & FC) { CYCT(b_+61, b_+63); goto subid19_state2_loop; } // jr c
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+66); interactionIncState_hook(gb); return; // jp

subid19_releasedTrigger:
  CALL_C(b_+66, interactionIncState_hook, SYM(interactionIncState), b_+69);
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+70); return; // ret

subid19_state3:
  CYC(b_+71, b_+74); A = W8(wActiveTriggers);
  CYC(b_+74, b_+75); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+75); return; } // ret c
  CYC(b_+75, b_+76);
  CYC(b_+76, b_+79); interactionIncState_hook(gb); return; // jp

subid19_state4:
  CYC(b_+79, b_+82); A = W8(wActiveTriggers);
  CYC(b_+82, b_+83); alu_rrca(gb);
  if (F & FC) { CYCT(b_+83, b_+85); goto subid19_pressedTrigger; } // jr c
  CYC(b_+83, b_+85);
  CALL_C(b_+85, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+88);
  if (!(F & FZ)) { RET_TAKEN(b_+88); return; } // ret nz
  CYC(b_+88, b_+89);
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x08);
  CYC(b_+91, b_+94); SET_HL(wRoomLayout + 0x59);

subid19_state4_loop:
  CYC(b_+94, b_+95); C = L;
  CYC(b_+95, b_+96); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+96, b_+98); alu_cp(gb, 0xf4); // TILEINDEX_BLANK_HOLE
  if (F & FZ) { CYCT(b_+98, b_+100); goto subid19_state4_advance; } // jr z
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); alu_cp(gb, 0xdb); // TILEINDEX_SWITCH_DIAMOND
  if (F & FZ) { CYCT(b_+102, b_+105); push_effect(gb, b_+105); goto subid19_createDebris; } // call z
  CYC(b_+102, b_+105);

subid19_afterCreateDebris:
  CYC(b_+105, b_+107); A = 0xf4; // TILEINDEX_BLANK_HOLE
  CALL_C(b_+107, setTileInAllBuffers_hook, SYM(setTileInAllBuffers), b_+110);
  CYC(b_+110, b_+112); A = 0x70; // SND_DOORCLOSE
  CYC(b_+112, b_+115); playSound_b00_hook(gb); return; // jp

subid19_state4_advance:
  CYC(b_+115, b_+116); A = L;
  CYC(b_+116, b_+118); alu_cp(gb, 0x55);
  if (!(F & FC)) { CYCT(b_+118, b_+120); goto subid19_state4_loop; } // jr nc
  CYC(b_+118, b_+120);

subid19_pressedTrigger:
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+122, b_+124); A = 0x01;
  CYC(b_+124, b_+125); mem_wr(gb, DE, A);
  RET(b_+125); return; // ret

// miscPuzzles_subid19@createDebris -- single caller, the conditional call z in state4's loop
subid19_createDebris:
  CYC(b_+126, b_+127); push_effect(gb, HL); // push hl
  CYC(b_+127, b_+128); push_effect(gb, BC); // push bc
  CYC(b_+128, b_+130); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(b_+130, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+133);
  CYC(b_+133, b_+134); SET_BC(pop_effect(gb)); // pop bc
  CYC(b_+134, b_+135); SET_HL(pop_effect(gb)); // pop hl
  RET(b_+135);
  if (gb->pc == b_+105 && gb->sp == sp0_) goto subid19_afterCreateDebris;
  return; // ret
}

// Checks solution to pushblock puzzle in Hero's Cave
void miscPuzzles_subid1a_hook(GB *gb) {
  BASE(miscPuzzles_subid1a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook, SYM(miscPuzzles_deleteSelfAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+21); // @wantedTiles
  CALL_C(b_+9, miscPuzzles_verifyTilesAtPositions_hook, SYM(miscPuzzles_verifyTilesAtPositions), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); SET_HL((SYM(interaction6b_subid05__substate1) + 32)); // agesInteractionsBank08.spawnChestAndDeleteSelf
  CYC(b_+16, b_+18); E = 0x08;
  CYC(b_+18, b_+21); interBankCall_hook(gb); return; // jp
}

// Subids $1b-$1d: Spawn gasha seeds at the top of the maku tree at specific times.
// b = essence that must be obtained; c = position to spawn it at.
static void miscPuzzles_subid1bcd_common(GB *gb, uint16_t sp0_) {
  BASE(miscPuzzles_subid1d);
  CYC(b_+3, b_+4); push_effect(gb, BC); // push bc
  CYC(b_+4, b_+6); A = 0x40; // TREASURE_ESSENCE
  CALL_C(b_+6, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+9);
  CYC(b_+9, b_+10); SET_BC(pop_effect(gb)); // pop bc
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto subid1bcd_delete; } // jr nc
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); alu_and(gb, B);
  if (F & FZ) { CYCT(b_+13, b_+15); goto subid1bcd_delete; } // jr z
  CYC(b_+13, b_+15);
  CALL_C(b_+15, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+18);
  CALL_C(b_+18, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+23); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto subid1bcd_delete; } // jr nz
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); SET_BC(0x3407); // TREASURE_OBJECT_GASHA_SEED_07
  CALL_C(b_+28, createTreasure_hook, SYM(createTreasure), b_+31);
  if (F & FZ) {
    CALL_C_CC(b_+31, objectCopyPosition_hook, SYM(objectCopyPosition), b_+34);
  } else {
    CYC(b_+31, b_+34);
  } // call z

subid1bcd_delete:
  CYC(b_+34, b_+37); interactionDelete_hook(gb); return; // jp
}

void miscPuzzles_subid1b_hook(GB *gb) {
  BASE(miscPuzzles_subid1b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x0853); // b = essence $08, c = position $53
  CYC(b_+3, b_+5); miscPuzzles_subid1bcd_common(gb, sp0_); return; // jr
}

void miscPuzzles_subid1c_hook(GB *gb) {
  BASE(miscPuzzles_subid1c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC((SYM(interactionCode78__state1) + 32)); // b = essence $40, c = position $34
  CYC(b_+3, b_+5); miscPuzzles_subid1bcd_common(gb, sp0_); return; // jr
}

void miscPuzzles_subid1d_hook(GB *gb) {
  BASE(miscPuzzles_subid1d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x2034); // b = essence $20, c = position $34
  miscPuzzles_subid1bcd_common(gb, sp0_); return;
}

// Play "puzzle solved" sound after navigating eyeball puzzle in final dungeon
void miscPuzzles_subid1e_hook(GB *gb) {
  BASE(miscPuzzles_subid1e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+3);
  CYC(b_+3, b_+6); A = W8(wScreenTransitionDirection);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+10); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+18); interactionDelete_hook(gb); return; // jp
}

// Checks if Link gets stuck in the d5 boss key puzzle, resets the room if so
void miscPuzzles_subid1f_hook(GB *gb) {
  BASE(miscPuzzles_subid1f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = miscPuzzles_jump_table(gb);
    if (target == b_+10) goto subid1f_state1;
    if (target == b_+98) goto subid1f_state2;
    interactionIncState_hook(gb); return; // target == 0x23e0
  }

subid1f_state1:
  CALL_C(b_+10, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } // ret nz
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 30);
  CYC(b_+16, b_+19); SET_HL(w1Link_yh);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+20, b_+22); alu_and(gb, 0xf0);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+24); L = alu_inc8(gb, L); // inc l (now w1Link_xh)
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_and(gb, 0xf0);
  CYC(b_+27, b_+29); A = alu_swap(gb, A);
  CYC(b_+29, b_+30); alu_or(gb, B);
  CYC(b_+30, b_+31); E = A;
  CYC(b_+31, b_+32); push_effect(gb, DE); // push de
  CYC(b_+32, b_+35); SET_HL(b_+90); // @offsetsToCheck
  CYC(b_+35, b_+37); D = 0x08;

subid1f_checkNextOffset:
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+38, b_+39); alu_add(gb, E);
  CYC(b_+39, b_+40); C = A;
  CYC(b_+40, b_+42); B = (uint8_t)(wRoomCollisions >> 8);
  CYC(b_+42, b_+43); A = mem_rd(gb, BC);
  CYC(b_+43, b_+44); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+44, b_+46); goto subid1f_doneCheckingIfTrapped; } // jr z
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+48); alu_bit(gb, 0, D);
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto subid1f_decD; } // jr nz
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); B = (uint8_t)(wRoomLayout >> 8);
  CYC(b_+52, b_+53); A = mem_rd(gb, BC);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto subid1f_decD; } // jr nz
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); SET_HL(HL + 1); // inc hl
  CYC(b_+57, b_+58); D = alu_dec8(gb, D); // dec d

subid1f_decD:
  CYC(b_+58, b_+59); D = alu_dec8(gb, D); // dec d
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto subid1f_checkNextOffset; } // jr nz
  CYC(b_+59, b_+61);

subid1f_doneCheckingIfTrapped:
  CYC(b_+61, b_+62); A = D;
  CYC(b_+62, b_+63); SET_DE(pop_effect(gb)); // pop de
  CYC(b_+63, b_+64); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+64); return; } // ret nz
  CYC(b_+64, b_+65);
  CALL_C(b_+65, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+68);
  if (!(F & FC)) { RET_TAKEN(b_+68); return; } // ret nc
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); A = 0x01; // DISABLE_LINK
  CYC(b_+71, b_+74); W8(wMenuDisabled) = A;
  CYC(b_+74, b_+77); W8(wDisabledObjects) = A;
  CYC(b_+77, b_+79); A = 0x5a; // SND_ERROR
  CALL_C(b_+79, playSound_b00_hook, SYM(playSound_b00), b_+82);
  CYC(b_+82, b_+84); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+84, b_+86); A = 60;
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CYC(b_+87, b_+90); interactionIncState_hook(gb); return; // jp

subid1f_state2:
  CALL_C(b_+98, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+101);
  if (!(F & FZ)) { RET_TAKEN(b_+101); return; } // ret nz
  CYC(b_+101, b_+102);
  CYC(b_+102, b_+103); alu_xor(gb, A);
  CYC(b_+103, b_+106); W8(wMenuDisabled) = A;
  CYC(b_+106, b_+109); W8(wDisabledObjects) = A;
  CYC(b_+109, b_+112); SET_HL(b_+115); // @warpDest
  CYC(b_+112, b_+115); setWarpDestVariables_hook(gb); return; // jp
}

// Money in sidescrolling room in Hero's Cave
void miscPuzzles_subid20_hook(GB *gb) {
  BASE(miscPuzzles_subid20);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto subid20_delete; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_BC(0x2816); // TREASURE_OBJECT_RUPEES_16
  CALL_C(b_+10, createTreasure_hook, SYM(createTreasure), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); goto subid20_delete; } // jp nz
  CYC(b_+13, b_+16);
  CALL_C(b_+16, objectCopyPosition_hook, SYM(objectCopyPosition), b_+19);

subid20_delete:
  CYC(b_+19, b_+22); interactionDelete_hook(gb); return; // jp
}

// Creates explosions while screen is fading out; used in some cutscene?
void miscPuzzles_subid21_hook(GB *gb) {
  BASE(miscPuzzles_subid21);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto subid21_state0; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = W8(wPaletteThread_mode);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+12); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+15); A = W8(wFrameCounter);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+18); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; } // ret nz
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+22); alu_and(gb, 0x70);
  CYC(b_+22, b_+24); A = alu_swap(gb, A);
  CYC(b_+24, b_+27); SET_HL(b_+40); // @explosionPositions
  CYC(b_+27, b_+28); miscPuzzles_add_double_index(gb, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+31); C = mem_rd(gb, HL); // ld c,(hl)
  CALL_C(b_+31, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+34);
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; } // ret nz
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x56); // INTERAC_EXPLOSION
  CYC(b_+37, b_+40); objectCopyPositionWithOffset_hook(gb); return; // jp

subid21_state0:
  CALL_C(b_+56, interactionIncState_hook, SYM(interactionIncState), b_+59);
  CYC(b_+59, b_+61); A = 0x04;
  CYC(b_+61, b_+64); fadeoutToWhiteWithDelay_hook(gb); return; // jp
}

// Shared helper: called with `call`; if the room's item flag is already set, discards
// its own return address and jumps into interactionDelete instead of returning, so the
// caller's remaining code never runs. Otherwise a plain `ret`.
void miscPuzzles_deleteSelfAndRetIfItemFlagSet_hook(GB *gb) {
  BASE(miscPuzzles_deleteSelfAndRetIfItemFlagSet);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_HL(pop_effect(gb)); // pop hl (discard return address)
  CYC(b_+7, b_+10); interactionDelete_hook(gb); return; // jp
}

// Shared helper: deletes self if the room's item flag is set, else increments state.
void miscPuzzles_deleteSelfOrIncStateIfItemFlagSet_hook(GB *gb) {
  BASE(miscPuzzles_deleteSelfOrIncStateIfItemFlagSet);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(b_+5, b_+8); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); interactionIncState_hook(gb); return; // jp
}

// Shared helper: deletes self if room flag bit 7 is set, else increments state.
void miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set_hook(GB *gb) {
  BASE(miscPuzzles_deleteSelfOrIncStateIfRoomFlag7Set);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); interactionIncState_hook(gb); return; // jp
}

// Shared helper (unused): deletes self if room flag bit 6 is set, else increments state.
void miscPuzzles_deleteSelfOrIncStateIfRoomFlag6Set_hook(GB *gb) {
  BASE(miscPuzzles_deleteSelfOrIncStateIfRoomFlag6Set);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); interactionIncState_hook(gb); return; // jp
}
