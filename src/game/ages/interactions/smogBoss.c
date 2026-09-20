#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Enemy struct fields live at $80 + offset (same layout as interactions at $40).

// Relative byte offsets (dbrel) to the per-phase tile replacement lists.
#define interactionCode33_tileReplacementTable_bank08 0x637f
// One byte per phase: number of smogs to spawn.
#define interactionCode33_numEnemiesToSpawn_bank08 0x63e7
// 4 bytes per enemy: var03, Y, X, direction.
#define interactionCode33_smogEnemyData_bank08 0x63eb
// 2 bytes per phase: Y, X to drop Link at.
#define interactionCode33_linkPlacementPositions_bank08 0x6417

static uint16_t smogBoss_jumpTable(GB *gb) {
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

static void smogBoss_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void smogBoss_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode33@findNextSmogEnemy: h = enemy index after which to start looking.
// Returns h = index of the first smog enemy with bit 1 of subid set; nz if none found.
static void smogBoss_findNextSmogEnemy(GB *gb) {
  CYC(0x631e, 0x631f); H = alu_inc8(gb, H);
loop:
  CYC(0x631f, 0x6321); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x6321, 0x6322); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6322, 0x6323); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6323, 0x6325); goto nextEnemy;
  }
  CYC(0x6323, 0x6325);
  CYC(0x6325, 0x6326); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6326, 0x6328); alu_cp(gb, 0x7c); // ENEMY_SMOG
  if (!(F & FZ)) {
    CYCT(0x6328, 0x632a); goto nextEnemy;
  }
  CYC(0x6328, 0x632a);
  CYC(0x632a, 0x632b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x632b, 0x632d); alu_bit(gb, 1, A);
  if (F & FZ) {
    CYCT(0x632d, 0x632f); goto nextEnemy;
  }
  CYC(0x632d, 0x632f);
  CYC(0x632f, 0x6330); alu_xor(gb, A);
  CYC(0x6330, 0x6331); ret_effect(gb);
  return;

nextEnemy:
  CYC(0x6331, 0x6332); H = alu_inc8(gb, H);
  CYC(0x6332, 0x6333); A = H;
  CYC(0x6333, 0x6335); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) {
    CYCT(0x6335, 0x6337); goto loop;
  }
  CYC(0x6335, 0x6337);
  CYC(0x6337, 0x6338); alu_or(gb, D);
  CYC(0x6338, 0x6339); ret_effect(gb);
}

// interactionCode33@findFirstSmogEnemy: falls through into findNextSmogEnemy.
static void smogBoss_findFirstSmogEnemy(GB *gb) {
  CYC(0x631c, 0x631e); H = (ENEMY_SLOTS >> 8) - 1; // FIRST_ENEMY_INDEX-1
  smogBoss_findNextSmogEnemy(gb);
}

// interactionCode33@spawnEnemy: spawn the next smog from @smogEnemyData (indexed by
// this.subid, which is incremented). Preserves af.
static void smogBoss_spawnEnemy(GB *gb, uint16_t sp0_) {
  CYC(0x6339, 0x633a); push_effect(gb, AF);
  CALL_C(0x633a, getFreeEnemySlot_hook, 0x2e27, 0x633d);
  CYC(0x633d, 0x633f); mem_wr(gb, HL, 0x7c); // ENEMY_SMOG
  CYC(0x633f, 0x6340); B = H;
  CYC(0x6340, 0x6342); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6342, 0x6343); A = mem_rd(gb, DE);
  CYC(0x6343, 0x6344); A = alu_inc8(gb, A);
  CYC(0x6344, 0x6345); mem_wr(gb, DE, A);
  CYC(0x6345, 0x6346); alu_add(gb, A);
  CYC(0x6346, 0x6349); SET_HL(interactionCode33_smogEnemyData_bank08);
  CYC(0x6349, 0x634a); smogBoss_addDoubleIndex(gb, 0x634a);
  CYC(0x634a, 0x634c); C = ENEMY_BASE + OBJ_SUBID;
  CYC(0x634c, 0x634d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x634d, 0x634e); mem_wr(gb, BC, A);
  CYC(0x634e, 0x634f); C = alu_inc8(gb, C);
  CYC(0x634f, 0x6351); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6351, 0x6352); A = mem_rd(gb, DE);
  CYC(0x6352, 0x6353); mem_wr(gb, BC, A);
  CYC(0x6353, 0x6355); C = ENEMY_BASE + OBJ_YH;
  CYC(0x6355, 0x6356); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6356, 0x6357); mem_wr(gb, BC, A);
  CYC(0x6357, 0x6359); C = ENEMY_BASE + OBJ_XH;
  CYC(0x6359, 0x635a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x635a, 0x635b); mem_wr(gb, BC, A);
  CYC(0x635b, 0x635d); C = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x635d, 0x635e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x635e, 0x635f); mem_wr(gb, BC, A);
  CYC(0x635f, 0x6360); SET_AF(pop_effect(gb));
  CYC(0x6360, 0x6361); ret_effect(gb);
}

// interactionCode33@checkEnemiesCloseEnoughToMerge: b = enemy 1, h = enemy 2.
// cflag set if they are within 4 pixels of each other (and on the same "layer", var31).
static void smogBoss_checkEnemiesCloseEnoughToMerge(GB *gb) {
  CYC(0x6361, 0x6363); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6363, 0x6364); C = L;
  CYC(0x6364, 0x6365); A = mem_rd(gb, BC);
  CYC(0x6365, 0x6366); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x6366, 0x6368); alu_add(gb, 0x03);
  CYC(0x6368, 0x636a); alu_cp(gb, 0x07);
  if (!(F & FC)) {
    CYCT(0x636a, 0x636b); ret_effect(gb); return;
  }
  CYC(0x636a, 0x636b);
  CYC(0x636b, 0x636d); L = ENEMY_BASE + OBJ_YH;
  CYC(0x636d, 0x636e); C = L;
  CYC(0x636e, 0x636f); A = mem_rd(gb, BC);
  CYC(0x636f, 0x6370); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x6370, 0x6372); alu_add(gb, 0x04);
  CYC(0x6372, 0x6374); alu_cp(gb, 0x09);
  if (!(F & FC)) {
    CYCT(0x6374, 0x6375); ret_effect(gb); return;
  }
  CYC(0x6374, 0x6375);
  CYC(0x6375, 0x6377); L = ENEMY_BASE + OBJ_XH;
  CYC(0x6377, 0x6378); C = L;
  CYC(0x6378, 0x6379); A = mem_rd(gb, BC);
  CYC(0x6379, 0x637a); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x637a, 0x637c); alu_add(gb, 0x04);
  CYC(0x637c, 0x637e); alu_cp(gb, 0x09);
  CYC(0x637e, 0x637f); ret_effect(gb);
}

// @checkMergeSmogs: check up to 3 smog enemies to see whether any two should merge;
// if so, merge smogs 'b' and 'h' into a brand new enemy slot (@mergeSmogs).
static void smogBoss_checkMergeSmogs(GB *gb, uint16_t sp0_) {
  CYC(0x624b, 0x624e); push_effect(gb, 0x624e); smogBoss_findFirstSmogEnemy(gb);
  if (!(F & FZ)) {
    CYCT(0x624e, 0x624f); ret_effect(gb); return;
  }
  CYC(0x624e, 0x624f);
  CYC(0x624f, 0x6250); push_effect(gb, HL);
  CYC(0x6250, 0x6253); push_effect(gb, 0x6253); smogBoss_findNextSmogEnemy(gb);
  CYC(0x6253, 0x6254); SET_BC(pop_effect(gb));
  if (!(F & FZ)) {
    CYCT(0x6254, 0x6255); ret_effect(gb); return;
  }
  CYC(0x6254, 0x6255);
  CYC(0x6255, 0x6258); push_effect(gb, 0x6258); smogBoss_checkEnemiesCloseEnoughToMerge(gb);
  if (F & FC) {
    CYCT(0x6258, 0x625a); goto mergeSmogs;
  }
  CYC(0x6258, 0x625a);
  CYC(0x625a, 0x625d); push_effect(gb, 0x625d); smogBoss_findNextSmogEnemy(gb);
  if (!(F & FZ)) {
    CYCT(0x625d, 0x625e); ret_effect(gb); return;
  }
  CYC(0x625d, 0x625e);
  CYC(0x625e, 0x6261); push_effect(gb, 0x6261); smogBoss_checkEnemiesCloseEnoughToMerge(gb);
  if (F & FC) {
    CYCT(0x6261, 0x6263); goto mergeSmogs;
  }
  CYC(0x6261, 0x6263);
  CYC(0x6263, 0x6264); push_effect(gb, HL);
  CYC(0x6264, 0x6265); H = B;
  CYC(0x6265, 0x6268); push_effect(gb, 0x6268); smogBoss_findNextSmogEnemy(gb);
  CYC(0x6268, 0x6269); push_effect(gb, HL);
  CYC(0x6269, 0x626a); SET_BC(pop_effect(gb));
  CYC(0x626a, 0x626b); SET_HL(pop_effect(gb));
  CYC(0x626b, 0x626e); push_effect(gb, 0x626e); smogBoss_checkEnemiesCloseEnoughToMerge(gb);
  if (!(F & FC)) {
    CYCT(0x626e, 0x626f); ret_effect(gb); return;
  }
  CYC(0x626e, 0x626f);

mergeSmogs:
  CYC(0x626f, 0x6271); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6271, 0x6272); C = L;
  CYC(0x6272, 0x6273); A = mem_rd(gb, BC);
  CYC(0x6273, 0x6274); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x6274, 0x6276); alu_and(gb, 0x80);
  CYC(0x6276, 0x6277); mem_wr(gb, BC, A);
  // Set subid to $06; slate it for deletion
  CYC(0x6277, 0x6279); mem_wr(gb, HL, 0x06);
  // This sets hl to a brand new enemy slot
  CALL_C(0x6279, getFreeEnemySlot_hook, 0x2e27, 0x627c);
  CYC(0x627c, 0x627e); mem_wr(gb, HL, 0x7c); // ENEMY_SMOG
  CYC(0x627e, 0x627f); A = mem_rd(gb, BC);
  CYC(0x627f, 0x6280); L = C;
  CYC(0x6280, 0x6282); alu_or(gb, 0x03);
  CYC(0x6282, 0x6283); mem_wr(gb, HL, A); SET_HL(HL + 1); // [new subid] = [old subid] | 3
  // Slate the other old smog for deletion
  CYC(0x6283, 0x6285); A = 0x06;
  CYC(0x6285, 0x6286); mem_wr(gb, BC, A);
  // [New var03] = [Interaction.var03]
  CYC(0x6286, 0x6288); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6288, 0x6289); A = mem_rd(gb, DE);
  CYC(0x6289, 0x628a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x628a, 0x628c); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x628c, 0x628e); mem_wr(gb, HL, 0x05);
  // Copy old smog's direction
  CYC(0x628e, 0x628f); L = alu_inc8(gb, L);
  CYC(0x628f, 0x6290); C = L;
  CYC(0x6290, 0x6291); A = mem_rd(gb, BC);
  CYC(0x6291, 0x6292); mem_wr(gb, HL, A);
  // Copy old smog's position
  CYC(0x6292, 0x6294); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6294, 0x6295); C = L;
  CYC(0x6295, 0x6296); A = mem_rd(gb, BC);
  CYC(0x6296, 0x6297); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6297, 0x6298); L = alu_inc8(gb, L);
  CYC(0x6298, 0x6299); C = L;
  CYC(0x6299, 0x629a); A = mem_rd(gb, BC);
  CYC(0x629a, 0x629b); mem_wr(gb, HL, A);
  CYC(0x629b, 0x629c); ret_effect(gb);
}

// @nextPhase: initialize the block-removal loop variables (var32-34) and go to state A.
static void smogBoss_nextPhase(GB *gb) {
  CYC(0x62ab, 0x62ad); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x62ad, 0x62af); A = 0x05;
  CYC(0x62af, 0x62b0); mem_wr(gb, DE, A);
  CYC(0x62b0, 0x62b2); A = 0x01;
  CYC(0x62b2, 0x62b5); mem_wr(gb, wMenuDisabled, A);
  CYC(0x62b5, 0x62b8); mem_wr(gb, wDisabledObjects, A);
  CYC(0x62b8, 0x62ba); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x62ba, 0x62bc); A = 0x11;
  CYC(0x62bc, 0x62bd); mem_wr(gb, DE, A);
  CYC(0x62bd, 0x62bf); A = 0x09; // LARGE_ROOM_HEIGHT-2
  CYC(0x62bf, 0x62c0); E = alu_inc8(gb, E);
  CYC(0x62c0, 0x62c1); mem_wr(gb, DE, A); // var33
  CYC(0x62c1, 0x62c2); E = alu_inc8(gb, E);
  CYC(0x62c2, 0x62c4); A = 0x0d; // LARGE_ROOM_WIDTH-2
  CYC(0x62c4, 0x62c5); mem_wr(gb, DE, A); // var34
  CYC(0x62c5, 0x62c8); interactionIncState_hook(gb);
}

// @stateA: clear out all block tiles on-screen, one per 5 frames, in preparation for
// the next phase; then return to state 1.
static void smogBoss_stateA(GB *gb, uint16_t sp0_) {
  CALL_C(0x62c8, interactionDecCounter1_hook, 0x23cc, 0x62cb);
  if (!(F & FZ)) {
    CYCT(0x62cb, 0x62cc); ret_effect(gb); return;
  }
  CYC(0x62cb, 0x62cc);
  CYC(0x62cc, 0x62ce); A = 0x05;
  CYC(0x62ce, 0x62cf); mem_wr(gb, HL, A);
  CYC(0x62cf, 0x62d1); L = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x62d1, 0x62d2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x62d2, 0x62d3); B = mem_rd(gb, HL);
  CYC(0x62d3, 0x62d4); L = A;
  CYC(0x62d4, 0x62d6); H = wRoomCollisions >> 8;
nextRow:
  CYC(0x62d6, 0x62d8); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x62d8, 0x62d9); A = mem_rd(gb, DE);
  CYC(0x62d9, 0x62da); C = A;
nextColumn:
  CYC(0x62da, 0x62db); A = mem_rd(gb, HL);
  CYC(0x62db, 0x62dc); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x62dc, 0x62de); goto foundNextBlockTile;
  }
  CYC(0x62dc, 0x62de);
  CYC(0x62de, 0x62e0); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x62e0, 0x62e1); L = alu_inc8(gb, L);
  CYC(0x62e1, 0x62e2); A = L;
  CYC(0x62e2, 0x62e3); mem_wr(gb, DE, A);
  CYC(0x62e3, 0x62e4); C = alu_dec8(gb, C);
  CYC(0x62e4, 0x62e6); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x62e6, 0x62e7); A = C;
  CYC(0x62e7, 0x62e8); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(0x62e8, 0x62ea); goto nextColumn;
  }
  CYC(0x62e8, 0x62ea);
  // Reset number of columns to check for the next row
  CYC(0x62ea, 0x62ec); A = 0x0d; // LARGE_ROOM_WIDTH-2
  CYC(0x62ec, 0x62ed); mem_wr(gb, DE, A);
  // Adjust position for the next row
  CYC(0x62ed, 0x62ee); C = A;
  CYC(0x62ee, 0x62f0); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x62f0, 0x62f1); A = L;
  CYC(0x62f1, 0x62f3); alu_add(gb, 0x03); // $10 - (LARGE_ROOM_WIDTH-2)
  CYC(0x62f3, 0x62f4); mem_wr(gb, DE, A);
  CYC(0x62f4, 0x62f5); L = A;
  CYC(0x62f5, 0x62f6); E = alu_inc8(gb, E); // var33
  CYC(0x62f6, 0x62f7); B = alu_dec8(gb, B);
  CYC(0x62f7, 0x62f8); A = B;
  CYC(0x62f8, 0x62f9); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(0x62f9, 0x62fb); goto nextRow;
  }
  CYC(0x62f9, 0x62fb);
  // Return to state 1 to begin the next phase
  CYC(0x62fb, 0x62fd); A = 0x01;
  CYC(0x62fd, 0x62ff); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x62ff, 0x6300); mem_wr(gb, DE, A);
  CYC(0x6300, 0x6301); ret_effect(gb);
  return;

foundNextBlockTile:
  CYC(0x6301, 0x6302); A = L;
  CYC(0x6302, 0x6304); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x6304, 0x6306); alu_and(gb, 0xf0);
  CYC(0x6306, 0x6308); alu_or(gb, 0x08);
  CYC(0x6308, 0x6309); mem_wr(gb, DE, A);
  CYC(0x6309, 0x630b); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x630b, 0x630c); A = L;
  CYC(0x630c, 0x630e); A = alu_swap(gb, A);
  CYC(0x630e, 0x6310); alu_and(gb, 0xf0);
  CYC(0x6310, 0x6312); alu_or(gb, 0x08);
  CYC(0x6312, 0x6313); mem_wr(gb, DE, A);
  CYC(0x6313, 0x6314); C = L;
  CYC(0x6314, 0x6316); A = 0xa3;
  CALL_C(0x6316, setTile_hook, 0x3a9c, 0x6319);
  CYC(0x6319, 0x631c); objectCreatePuff_hook(gb);
}

// INTERAC_SMOG_BOSS: orchestrates the four phases of the smog boss fight (spawning smogs,
// merging them, rearranging the room's block tiles between phases).
void interactionCode33_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6104, 0x6106); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6106, 0x6107); A = mem_rd(gb, DE);
  CYC(0x6107, 0x6108); push_effect(gb, 0x6108);
  switch (smogBoss_jumpTable(gb)) {
    case 0x611e: goto state0;
    case 0x613b: goto state1;
    case 0x614b: goto state2;
    case 0x6164: goto state3;
    case 0x617f: goto state4;
    case 0x6187: goto state5;
    case 0x61a4: goto state6;
    case 0x61d2: goto state7;
    case 0x61ff: goto state8;
    case 0x629c: goto state9;
    case 0x62c8: smogBoss_stateA(gb, sp0_); return;
    default: HANDOFF(HL);
  }

state0:
  CALL_C(0x611e, getThisRoomFlags_hook, 0x197d, 0x6121);
  CYC(0x6121, 0x6123); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x6123, 0x6126); interactionDelete_hook(gb); return;
  }
  CYC(0x6123, 0x6126);
  CYC(0x6126, 0x6128); A = 0x01;
  CYC(0x6128, 0x612b); mem_wr(gb, wMenuDisabled, A);
  CYC(0x612b, 0x612e); mem_wr(gb, wDisabledObjects, A);
  CYC(0x612e, 0x6131); A = mem_rd(gb, wcc93);
  CYC(0x6131, 0x6132); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6132, 0x6133); ret_effect(gb); return;
  }
  CYC(0x6132, 0x6133);
  CYC(0x6133, 0x6134); A = alu_inc8(gb, A);
  CYC(0x6134, 0x6135); mem_wr(gb, DE, A); // [state] = 1
  CYC(0x6135, 0x6138); push_effect(gb, 0x6138); smogBoss_spawnEnemy(gb, sp0_);
  CYC(0x6138, 0x613b); objectCreatePuff_hook(gb);
  return;

state1:
  // Waiting for Link to complete this phase
  CYC(0x613b, 0x613e); A = mem_rd(gb, wNumEnemies);
  CYC(0x613e, 0x613f); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x613f, 0x6140); ret_effect(gb); return;
  }
  CYC(0x613f, 0x6140);
  CYC(0x6140, 0x6142); A = 0x01;
  CYC(0x6142, 0x6145); mem_wr(gb, wMenuDisabled, A);
  CYC(0x6145, 0x6148); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6148, 0x614b); interactionIncState_hook(gb);
  return;

state2:
  // Raise Link off the floor
  CYC(0x614b, 0x614e); SET_HL(w1Link_zh);
  CYC(0x614e, 0x614f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x614f, 0x6151); A = 0xf9;
  CYC(0x6151, 0x6152); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x6152, 0x6153); ret_effect(gb); return;
  }
  CYC(0x6152, 0x6153);
  // Get the position to place Link at
  CYC(0x6153, 0x6155); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6155, 0x6156); A = mem_rd(gb, DE);
  CYC(0x6156, 0x6159); SET_HL(interactionCode33_linkPlacementPositions_bank08);
  CYC(0x6159, 0x615a); smogBoss_addDoubleIndex(gb, 0x615a);
  CYC(0x615a, 0x615c); E = INTERACTION_BASE + OBJ_USE_TEXT_ID; // var30
  CYC(0x615c, 0x615d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x615d, 0x615e); mem_wr(gb, DE, A);
  CYC(0x615e, 0x615f); E = alu_inc8(gb, E);
  CYC(0x615f, 0x6160); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6160, 0x6161); mem_wr(gb, DE, A);
  CYC(0x6161, 0x6164); interactionIncState_hook(gb);
  return;

state3:
  // Moving Link to the target position (var30/var31)
  CYC(0x6164, 0x6167); SET_HL(w1Link_yh);
  CYC(0x6167, 0x6169); E = INTERACTION_BASE + OBJ_USE_TEXT_ID; // var30
  CYC(0x6169, 0x616a); A = mem_rd(gb, DE);
  CYC(0x616a, 0x616b); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x616b, 0x616e); goto incOrDecPosition;
  }
  CYC(0x616b, 0x616e);
  CYC(0x616e, 0x6170); L = (uint8_t)w1Link_xh;
  CYC(0x6170, 0x6171); E = alu_inc8(gb, E);
  CYC(0x6171, 0x6172); A = mem_rd(gb, DE);
  CYC(0x6172, 0x6173); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x6173, 0x6176); goto incOrDecPosition;
  }
  CYC(0x6173, 0x6176);
  CYC(0x6176, 0x6179); interactionIncState_hook(gb);
  return;

incOrDecPosition:
  if (F & FC) {
    CYCT(0x6179, 0x617b);
    CYC(0x617d, 0x617e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x617e, 0x617f); ret_effect(gb);
    return;
  }
  CYC(0x6179, 0x617b);
  CYC(0x617b, 0x617c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x617c, 0x617d); ret_effect(gb);
  return;

state4:
  // Moving Link back to the ground
  CYC(0x617f, 0x6182); SET_HL(w1Link_zh);
  CYC(0x6182, 0x6183); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x6183, 0x6184); ret_effect(gb); return;
  }
  CYC(0x6183, 0x6184);
  CYC(0x6184, 0x6187); interactionIncState_hook(gb);
  return;

state5:
  // Waiting for Link to complete this phase; then point var18 at this phase's tile data
  CYC(0x6187, 0x618a); A = mem_rd(gb, wNumEnemies);
  CYC(0x618a, 0x618b); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x618b, 0x618c); ret_effect(gb); return;
  }
  CYC(0x618b, 0x618c);
  CYC(0x618c, 0x618e); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x618e, 0x618f); A = mem_rd(gb, DE);
  CYC(0x618f, 0x6192); SET_HL(interactionCode33_tileReplacementTable_bank08);
  CYC(0x6192, 0x6193); push_effect(gb, 0x6193); smogBoss_addAToHl(gb);
  CYC(0x6193, 0x6194); A = mem_rd(gb, HL);
  CYC(0x6194, 0x6195); push_effect(gb, 0x6195); smogBoss_addAToHl(gb);
  CYC(0x6195, 0x6197); E = INTERACTION_BASE + OBJ_VAR18;
  CYC(0x6197, 0x6198); A = L;
  CYC(0x6198, 0x6199); mem_wr(gb, DE, A);
  CYC(0x6199, 0x619a); E = alu_inc8(gb, E);
  CYC(0x619a, 0x619b); A = H;
  CYC(0x619b, 0x619c); mem_wr(gb, DE, A);
  CYC(0x619c, 0x619e); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x619e, 0x61a0); A = 0x05;
  CYC(0x61a0, 0x61a1); mem_wr(gb, DE, A);
  CYC(0x61a1, 0x61a4); interactionIncState_hook(gb);
  return;

state6:
  // Generate the tiles to be used in this phase, one per 5 frames
  CALL_C(0x61a4, interactionDecCounter1_hook, 0x23cc, 0x61a7);
  if (!(F & FZ)) {
    CYCT(0x61a7, 0x61a8); ret_effect(gb); return;
  }
  CYC(0x61a7, 0x61a8);
  CYC(0x61a8, 0x61aa); mem_wr(gb, HL, 0x05);
  // Retrieve pointer to tile replacement data
  CYC(0x61aa, 0x61ac); L = INTERACTION_BASE + OBJ_VAR18;
  CYC(0x61ac, 0x61ad); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x61ad, 0x61ae); H = mem_rd(gb, HL);
  CYC(0x61ae, 0x61af); L = A;
  CYC(0x61af, 0x61b0); A = mem_rd(gb, HL);
  CYC(0x61b0, 0x61b1); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x61b1, 0x61b4); interactionIncState_hook(gb); return;
  }
  CYC(0x61b1, 0x61b4);
  // First byte read was position; move interaction here for the purpose of creating
  // the "poof".
  CALL_C(0x61b4, convertShortToLongPosition_hook, 0x20cb, 0x61b7);
  CYC(0x61b7, 0x61b9); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x61b9, 0x61ba); A = B;
  CYC(0x61ba, 0x61bb); mem_wr(gb, DE, A);
  CYC(0x61bb, 0x61bd); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x61bd, 0x61be); A = C;
  CYC(0x61be, 0x61bf); mem_wr(gb, DE, A);
  // Change the tile index
  CYC(0x61bf, 0x61c0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x61c0, 0x61c1); C = A;
  CYC(0x61c1, 0x61c2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x61c2, 0x61c3); push_effect(gb, HL);
  CALL_C(0x61c3, setTile_hook, 0x3a9c, 0x61c6);
  CYC(0x61c6, 0x61c7); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(0x61c7, 0x61c8); ret_effect(gb); return;
  }
  CYC(0x61c7, 0x61c8);
  // Save pointer
  CYC(0x61c8, 0x61ca); E = INTERACTION_BASE + OBJ_VAR18;
  CYC(0x61ca, 0x61cb); A = L;
  CYC(0x61cb, 0x61cc); mem_wr(gb, DE, A);
  CYC(0x61cc, 0x61cd); E = alu_inc8(gb, E);
  CYC(0x61cd, 0x61ce); A = H;
  CYC(0x61ce, 0x61cf); mem_wr(gb, DE, A);
  CYC(0x61cf, 0x61d2); objectCreatePuff_hook(gb);
  return;

state7:
  // Spawn the enemies
  CALL_C(0x61d2, interactionDecCounter1_hook, 0x23cc, 0x61d5);
  if (!(F & FZ)) {
    CYCT(0x61d5, 0x61d6); ret_effect(gb); return;
  }
  CYC(0x61d5, 0x61d6);
  CALL_C(0x61d6, getThisRoomFlags_hook, 0x197d, 0x61d9);
  CYC(0x61d9, 0x61db); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  CYC(0x61db, 0x61dd); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x61dd, 0x61de); A = mem_rd(gb, DE);
  CYC(0x61de, 0x61e0); E = INTERACTION_BASE + OBJ_VAR35;
  CYC(0x61e0, 0x61e1); mem_wr(gb, DE, A);
  CYC(0x61e1, 0x61e3); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x61e3, 0x61e4); A = mem_rd(gb, DE);
  CYC(0x61e4, 0x61e7); SET_HL(interactionCode33_numEnemiesToSpawn_bank08);
  CYC(0x61e7, 0x61e8); push_effect(gb, 0x61e8); smogBoss_addAToHl(gb);
  CYC(0x61e8, 0x61e9); A = mem_rd(gb, HL);
spawnLoop:
  CYC(0x61e9, 0x61ec); push_effect(gb, 0x61ec); smogBoss_spawnEnemy(gb, sp0_);
  CYC(0x61ec, 0x61ed); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61ed, 0x61ef); goto spawnLoop;
  }
  CYC(0x61ed, 0x61ef);
  CYC(0x61ef, 0x61f2); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  // Return position to top-left corner
  CYC(0x61f2, 0x61f4); A = 0x18;
  CYC(0x61f4, 0x61f6); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x61f6, 0x61f7); mem_wr(gb, DE, A);
  CYC(0x61f7, 0x61f9); alu_sub(gb, 0x04);
  CYC(0x61f9, 0x61fb); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x61fb, 0x61fc); mem_wr(gb, DE, A);
  CYC(0x61fc, 0x61ff); interactionIncState_hook(gb);
  return;

state8:
  // Run the phase; constantly checks whether any 2 enemies are close enough to merge.
  // If [wNumEnemies] == 1, this phase is over
  CYC(0x61ff, 0x6202); A = mem_rd(gb, wNumEnemies);
  CYC(0x6202, 0x6203); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x6203, 0x6206); interactionIncState_hook(gb); return;
  }
  CYC(0x6203, 0x6206);
  // If [wNumEnemies] == 2, there's only one, big smog on-screen
  CYC(0x6206, 0x6207); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x6207, 0x6209); smogBoss_checkMergeSmogs(gb, sp0_); return;
  }
  CYC(0x6207, 0x6209);
  // Check whether the switch tile has changed (Link's stepped on it)
  CALL_C(0x6209, objectGetTileAtPosition_hook, 0x1444, 0x620c);
  CYC(0x620c, 0x620e); alu_cp(gb, 0x0c); // TILEINDEX_BUTTON
  if (!(F & FZ)) {
    CYCT(0x620e, 0x6210); goto buttonPressed;
  }
  CYC(0x620e, 0x6210);
  CYC(0x6210, 0x6213); A = W8(w1Link_state);
  CYC(0x6213, 0x6215); alu_cp(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) {
    CYCT(0x6215, 0x6217); smogBoss_checkMergeSmogs(gb, sp0_); return;
  }
  CYC(0x6215, 0x6217);
  CYC(0x6217, 0x621a); A = mem_rd(gb, wLinkInAir);
  CYC(0x621a, 0x621b); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x621b, 0x621d); smogBoss_checkMergeSmogs(gb, sp0_); return;
  }
  CYC(0x621b, 0x621d);
  CYC(0x621d, 0x621f); C = 0x04;
  CALL_C(0x621f, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6222);
  if (!(F & FC)) {
    CYCT(0x6222, 0x6224); smogBoss_checkMergeSmogs(gb, sp0_); return;
  }
  CYC(0x6222, 0x6224);
  // Switch pressed
  CYC(0x6224, 0x6226); A = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CYC(0x6226, 0x6228); C = 0x11;
  CALL_C(0x6228, setTile_hook, 0x3a9c, 0x622b);

buttonPressed:
  // Subtract health as a penalty
  CYC(0x622b, 0x622e); SET_HL(wLinkHealth);
  CYC(0x622e, 0x622f); A = mem_rd(gb, HL);
  CYC(0x622f, 0x6231); alu_cp(gb, 0x0c);
  if (F & FC) {
    CYCT(0x6231, 0x6233);
  } else {
    CYC(0x6231, 0x6233);
    CYC(0x6233, 0x6235); alu_sub(gb, 0x04);
    CYC(0x6235, 0x6236); mem_wr(gb, HL, A);
  }
  CYC(0x6236, 0x6238); A = 0x87; // SND_SPLASH
  CALL_C(0x6238, playSound_b00_hook, 0x0c98, 0x623b);
  CALL_C(0x623b, getThisRoomFlags_hook, 0x197d, 0x623e);
  CYC(0x623e, 0x6240); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));
  CYC(0x6240, 0x6242); E = INTERACTION_BASE + OBJ_VAR35;
  CYC(0x6242, 0x6243); A = mem_rd(gb, DE);
  CYC(0x6243, 0x6245); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6245, 0x6246); mem_wr(gb, DE, A);
  CALL_C(0x6246, interactionIncState_hook, 0x23e0, 0x6249);
  CYC(0x6249, 0x624b);
  smogBoss_nextPhase(gb);
  return;

state9:
  // Smog destroyed; proceed to the next phase
  CYC(0x629c, 0x629e); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x629e, 0x629f); A = mem_rd(gb, DE);
  CYC(0x629f, 0x62a0); A = alu_inc8(gb, A);
  CYC(0x62a0, 0x62a1); mem_wr(gb, DE, A);
  CYC(0x62a1, 0x62a3); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x62a3, 0x62a5); smogBoss_nextPhase(gb); return;
  }
  CYC(0x62a3, 0x62a5);
  // Final phase completed
  CALL_C(0x62a5, decNumEnemies_hook, 0x24b3, 0x62a8);
  CYC(0x62a8, 0x62ab); interactionDelete_hook(gb);
}
