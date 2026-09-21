#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Enemy struct fields live at $80 + offset (same layout as interactions at $40).

// Relative byte offsets (dbrel) to the per-phase tile replacement lists.
#define interactionCode33_tileReplacementTable_bank08 SYM(interactionCode33__tileReplacementTable)
// One byte per phase: number of smogs to spawn.
#define interactionCode33_numEnemiesToSpawn_bank08 SYM(interactionCode33__numEnemiesToSpawn)
// 4 bytes per enemy: var03, Y, X, direction.
#define interactionCode33_smogEnemyData_bank08 SYM(interactionCode33__smogEnemyData)
// 2 bytes per phase: Y, X to drop Link at.
#define interactionCode33_linkPlacementPositions_bank08 SYM(interactionCode33__linkPlacementPositions)

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
  BASE(interactionCode33);
  CYC(b_+538, b_+539); H = alu_inc8(gb, H);
loop:
  CYC(b_+539, b_+541); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+541, b_+542); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+542, b_+543); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+543, b_+545); goto nextEnemy;
  }
  CYC(b_+543, b_+545);
  CYC(b_+545, b_+546); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+546, b_+548); alu_cp(gb, 0x7c); // ENEMY_SMOG
  if (!(F & FZ)) {
    CYCT(b_+548, b_+550); goto nextEnemy;
  }
  CYC(b_+548, b_+550);
  CYC(b_+550, b_+551); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+551, b_+553); alu_bit(gb, 1, A);
  if (F & FZ) {
    CYCT(b_+553, b_+555); goto nextEnemy;
  }
  CYC(b_+553, b_+555);
  CYC(b_+555, b_+556); alu_xor(gb, A);
  CYC(b_+556, b_+557); ret_effect(gb);
  return;

nextEnemy:
  CYC(b_+557, b_+558); H = alu_inc8(gb, H);
  CYC(b_+558, b_+559); A = H;
  CYC(b_+559, b_+561); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) {
    CYCT(b_+561, b_+563); goto loop;
  }
  CYC(b_+561, b_+563);
  CYC(b_+563, b_+564); alu_or(gb, D);
  CYC(b_+564, b_+565); ret_effect(gb);
}

// interactionCode33@findFirstSmogEnemy: falls through into findNextSmogEnemy.
static void smogBoss_findFirstSmogEnemy(GB *gb) {
  BASE(interactionCode33);
  CYC(b_+536, b_+538); H = (ENEMY_SLOTS >> 8) - 1; // FIRST_ENEMY_INDEX-1
  smogBoss_findNextSmogEnemy(gb);
}

// interactionCode33@spawnEnemy: spawn the next smog from @smogEnemyData (indexed by
// this.subid, which is incremented). Preserves af.
static void smogBoss_spawnEnemy(GB *gb, uint16_t sp0_) {
  BASE(interactionCode33);
  CYC(b_+565, b_+566); push_effect(gb, AF);
  CALL_C(b_+566, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+569);
  CYC(b_+569, b_+571); mem_wr(gb, HL, 0x7c); // ENEMY_SMOG
  CYC(b_+571, b_+572); B = H;
  CYC(b_+572, b_+574); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+574, b_+575); A = mem_rd(gb, DE);
  CYC(b_+575, b_+576); A = alu_inc8(gb, A);
  CYC(b_+576, b_+577); mem_wr(gb, DE, A);
  CYC(b_+577, b_+578); alu_add(gb, A);
  CYC(b_+578, b_+581); SET_HL(interactionCode33_smogEnemyData_bank08);
  CYC(b_+581, b_+582); smogBoss_addDoubleIndex(gb, b_+582);
  CYC(b_+582, b_+584); C = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+584, b_+585); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+585, b_+586); mem_wr(gb, BC, A);
  CYC(b_+586, b_+587); C = alu_inc8(gb, C);
  CYC(b_+587, b_+589); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+589, b_+590); A = mem_rd(gb, DE);
  CYC(b_+590, b_+591); mem_wr(gb, BC, A);
  CYC(b_+591, b_+593); C = ENEMY_BASE + OBJ_YH;
  CYC(b_+593, b_+594); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+594, b_+595); mem_wr(gb, BC, A);
  CYC(b_+595, b_+597); C = ENEMY_BASE + OBJ_XH;
  CYC(b_+597, b_+598); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+598, b_+599); mem_wr(gb, BC, A);
  CYC(b_+599, b_+601); C = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+601, b_+602); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+602, b_+603); mem_wr(gb, BC, A);
  CYC(b_+603, b_+604); SET_AF(pop_effect(gb));
  CYC(b_+604, b_+605); ret_effect(gb);
}

// interactionCode33@checkEnemiesCloseEnoughToMerge: b = enemy 1, h = enemy 2.
// cflag set if they are within 4 pixels of each other (and on the same "layer", var31).
static void smogBoss_checkEnemiesCloseEnoughToMerge(GB *gb) {
  BASE(interactionCode33);
  CYC(b_+605, b_+607); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+607, b_+608); C = L;
  CYC(b_+608, b_+609); A = mem_rd(gb, BC);
  CYC(b_+609, b_+610); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+610, b_+612); alu_add(gb, 0x03);
  CYC(b_+612, b_+614); alu_cp(gb, 0x07);
  if (!(F & FC)) {
    CYCT(b_+614, b_+615); ret_effect(gb); return;
  }
  CYC(b_+614, b_+615);
  CYC(b_+615, b_+617); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+617, b_+618); C = L;
  CYC(b_+618, b_+619); A = mem_rd(gb, BC);
  CYC(b_+619, b_+620); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+620, b_+622); alu_add(gb, 0x04);
  CYC(b_+622, b_+624); alu_cp(gb, 0x09);
  if (!(F & FC)) {
    CYCT(b_+624, b_+625); ret_effect(gb); return;
  }
  CYC(b_+624, b_+625);
  CYC(b_+625, b_+627); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+627, b_+628); C = L;
  CYC(b_+628, b_+629); A = mem_rd(gb, BC);
  CYC(b_+629, b_+630); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+630, b_+632); alu_add(gb, 0x04);
  CYC(b_+632, b_+634); alu_cp(gb, 0x09);
  CYC(b_+634, b_+635); ret_effect(gb);
}

// @checkMergeSmogs: check up to 3 smog enemies to see whether any two should merge;
// if so, merge smogs 'b' and 'h' into a brand new enemy slot (@mergeSmogs).
static void smogBoss_checkMergeSmogs(GB *gb, uint16_t sp0_) {
  BASE(interactionCode33);
  CYC(b_+327, b_+330); push_effect(gb, b_+330); smogBoss_findFirstSmogEnemy(gb);
  if (!(F & FZ)) {
    CYCT(b_+330, b_+331); ret_effect(gb); return;
  }
  CYC(b_+330, b_+331);
  CYC(b_+331, b_+332); push_effect(gb, HL);
  CYC(b_+332, b_+335); push_effect(gb, b_+335); smogBoss_findNextSmogEnemy(gb);
  CYC(b_+335, b_+336); SET_BC(pop_effect(gb));
  if (!(F & FZ)) {
    CYCT(b_+336, b_+337); ret_effect(gb); return;
  }
  CYC(b_+336, b_+337);
  CYC(b_+337, b_+340); push_effect(gb, b_+340); smogBoss_checkEnemiesCloseEnoughToMerge(gb);
  if (F & FC) {
    CYCT(b_+340, b_+342); goto mergeSmogs;
  }
  CYC(b_+340, b_+342);
  CYC(b_+342, b_+345); push_effect(gb, b_+345); smogBoss_findNextSmogEnemy(gb);
  if (!(F & FZ)) {
    CYCT(b_+345, b_+346); ret_effect(gb); return;
  }
  CYC(b_+345, b_+346);
  CYC(b_+346, b_+349); push_effect(gb, b_+349); smogBoss_checkEnemiesCloseEnoughToMerge(gb);
  if (F & FC) {
    CYCT(b_+349, b_+351); goto mergeSmogs;
  }
  CYC(b_+349, b_+351);
  CYC(b_+351, b_+352); push_effect(gb, HL);
  CYC(b_+352, b_+353); H = B;
  CYC(b_+353, b_+356); push_effect(gb, b_+356); smogBoss_findNextSmogEnemy(gb);
  CYC(b_+356, b_+357); push_effect(gb, HL);
  CYC(b_+357, b_+358); SET_BC(pop_effect(gb));
  CYC(b_+358, b_+359); SET_HL(pop_effect(gb));
  CYC(b_+359, b_+362); push_effect(gb, b_+362); smogBoss_checkEnemiesCloseEnoughToMerge(gb);
  if (!(F & FC)) {
    CYCT(b_+362, b_+363); ret_effect(gb); return;
  }
  CYC(b_+362, b_+363);

mergeSmogs:
  CYC(b_+363, b_+365); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+365, b_+366); C = L;
  CYC(b_+366, b_+367); A = mem_rd(gb, BC);
  CYC(b_+367, b_+368); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+368, b_+370); alu_and(gb, 0x80);
  CYC(b_+370, b_+371); mem_wr(gb, BC, A);
  // Set subid to $06; slate it for deletion
  CYC(b_+371, b_+373); mem_wr(gb, HL, 0x06);
  // This sets hl to a brand new enemy slot
  CALL_C(b_+373, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+376);
  CYC(b_+376, b_+378); mem_wr(gb, HL, 0x7c); // ENEMY_SMOG
  CYC(b_+378, b_+379); A = mem_rd(gb, BC);
  CYC(b_+379, b_+380); L = C;
  CYC(b_+380, b_+382); alu_or(gb, 0x03);
  CYC(b_+382, b_+383); mem_wr(gb, HL, A); SET_HL(HL + 1); // [new subid] = [old subid] | 3
  // Slate the other old smog for deletion
  CYC(b_+383, b_+385); A = 0x06;
  CYC(b_+385, b_+386); mem_wr(gb, BC, A);
  // [New var03] = [Interaction.var03]
  CYC(b_+386, b_+388); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+388, b_+389); A = mem_rd(gb, DE);
  CYC(b_+389, b_+390); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+390, b_+392); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+392, b_+394); mem_wr(gb, HL, 0x05);
  // Copy old smog's direction
  CYC(b_+394, b_+395); L = alu_inc8(gb, L);
  CYC(b_+395, b_+396); C = L;
  CYC(b_+396, b_+397); A = mem_rd(gb, BC);
  CYC(b_+397, b_+398); mem_wr(gb, HL, A);
  // Copy old smog's position
  CYC(b_+398, b_+400); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+400, b_+401); C = L;
  CYC(b_+401, b_+402); A = mem_rd(gb, BC);
  CYC(b_+402, b_+403); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+403, b_+404); L = alu_inc8(gb, L);
  CYC(b_+404, b_+405); C = L;
  CYC(b_+405, b_+406); A = mem_rd(gb, BC);
  CYC(b_+406, b_+407); mem_wr(gb, HL, A);
  CYC(b_+407, b_+408); ret_effect(gb);
}

// @nextPhase: initialize the block-removal loop variables (var32-34) and go to state A.
static void smogBoss_nextPhase(GB *gb) {
  BASE(interactionCode33);
  CYC(b_+423, b_+425); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+425, b_+427); A = 0x05;
  CYC(b_+427, b_+428); mem_wr(gb, DE, A);
  CYC(b_+428, b_+430); A = 0x01;
  CYC(b_+430, b_+433); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+433, b_+436); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+436, b_+438); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+438, b_+440); A = 0x11;
  CYC(b_+440, b_+441); mem_wr(gb, DE, A);
  CYC(b_+441, b_+443); A = 0x09; // LARGE_ROOM_HEIGHT-2
  CYC(b_+443, b_+444); E = alu_inc8(gb, E);
  CYC(b_+444, b_+445); mem_wr(gb, DE, A); // var33
  CYC(b_+445, b_+446); E = alu_inc8(gb, E);
  CYC(b_+446, b_+448); A = 0x0d; // LARGE_ROOM_WIDTH-2
  CYC(b_+448, b_+449); mem_wr(gb, DE, A); // var34
  CYC(b_+449, b_+452); interactionIncState_hook(gb);
}

// @stateA: clear out all block tiles on-screen, one per 5 frames, in preparation for
// the next phase; then return to state 1.
static void smogBoss_stateA(GB *gb, uint16_t sp0_) {
  BASE(interactionCode33);
  CALL_C(b_+452, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+455);
  if (!(F & FZ)) {
    CYCT(b_+455, b_+456); ret_effect(gb); return;
  }
  CYC(b_+455, b_+456);
  CYC(b_+456, b_+458); A = 0x05;
  CYC(b_+458, b_+459); mem_wr(gb, HL, A);
  CYC(b_+459, b_+461); L = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+461, b_+462); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+462, b_+463); B = mem_rd(gb, HL);
  CYC(b_+463, b_+464); L = A;
  CYC(b_+464, b_+466); H = wRoomCollisions >> 8;
nextRow:
  CYC(b_+466, b_+468); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+468, b_+469); A = mem_rd(gb, DE);
  CYC(b_+469, b_+470); C = A;
nextColumn:
  CYC(b_+470, b_+471); A = mem_rd(gb, HL);
  CYC(b_+471, b_+472); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+472, b_+474); goto foundNextBlockTile;
  }
  CYC(b_+472, b_+474);
  CYC(b_+474, b_+476); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+476, b_+477); L = alu_inc8(gb, L);
  CYC(b_+477, b_+478); A = L;
  CYC(b_+478, b_+479); mem_wr(gb, DE, A);
  CYC(b_+479, b_+480); C = alu_dec8(gb, C);
  CYC(b_+480, b_+482); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+482, b_+483); A = C;
  CYC(b_+483, b_+484); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(b_+484, b_+486); goto nextColumn;
  }
  CYC(b_+484, b_+486);
  // Reset number of columns to check for the next row
  CYC(b_+486, b_+488); A = 0x0d; // LARGE_ROOM_WIDTH-2
  CYC(b_+488, b_+489); mem_wr(gb, DE, A);
  // Adjust position for the next row
  CYC(b_+489, b_+490); C = A;
  CYC(b_+490, b_+492); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+492, b_+493); A = L;
  CYC(b_+493, b_+495); alu_add(gb, 0x03); // $10 - (LARGE_ROOM_WIDTH-2)
  CYC(b_+495, b_+496); mem_wr(gb, DE, A);
  CYC(b_+496, b_+497); L = A;
  CYC(b_+497, b_+498); E = alu_inc8(gb, E); // var33
  CYC(b_+498, b_+499); B = alu_dec8(gb, B);
  CYC(b_+499, b_+500); A = B;
  CYC(b_+500, b_+501); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(b_+501, b_+503); goto nextRow;
  }
  CYC(b_+501, b_+503);
  // Return to state 1 to begin the next phase
  CYC(b_+503, b_+505); A = 0x01;
  CYC(b_+505, b_+507); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+507, b_+508); mem_wr(gb, DE, A);
  CYC(b_+508, b_+509); ret_effect(gb);
  return;

foundNextBlockTile:
  CYC(b_+509, b_+510); A = L;
  CYC(b_+510, b_+512); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+512, b_+514); alu_and(gb, 0xf0);
  CYC(b_+514, b_+516); alu_or(gb, 0x08);
  CYC(b_+516, b_+517); mem_wr(gb, DE, A);
  CYC(b_+517, b_+519); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+519, b_+520); A = L;
  CYC(b_+520, b_+522); A = alu_swap(gb, A);
  CYC(b_+522, b_+524); alu_and(gb, 0xf0);
  CYC(b_+524, b_+526); alu_or(gb, 0x08);
  CYC(b_+526, b_+527); mem_wr(gb, DE, A);
  CYC(b_+527, b_+528); C = L;
  CYC(b_+528, b_+530); A = 0xa3;
  CALL_C(b_+530, setTile_hook, SYM(setTile), b_+533);
  CYC(b_+533, b_+536); objectCreatePuff_hook(gb);
}

// INTERAC_SMOG_BOSS: orchestrates the four phases of the smog boss fight (spawning smogs,
// merging them, rearranging the room's block tiles between phases).
void interactionCode33_hook(GB *gb) {
  BASE(interactionCode33);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (smogBoss_jumpTable(gb));
    if (jt_ == b_+26) { goto state0; }
    else if (jt_ == b_+55) { goto state1; }
    else if (jt_ == b_+71) { goto state2; }
    else if (jt_ == b_+96) { goto state3; }
    else if (jt_ == b_+123) { goto state4; }
    else if (jt_ == b_+131) { goto state5; }
    else if (jt_ == b_+160) { goto state6; }
    else if (jt_ == b_+206) { goto state7; }
    else if (jt_ == b_+251) { goto state8; }
    else if (jt_ == b_+408) { goto state9; }
    else if (jt_ == b_+452) { smogBoss_stateA(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+26, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+29);
  CYC(b_+29, b_+31); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+31, b_+34); TAIL(interactionDelete);
  }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+36); A = 0x01;
  CYC(b_+36, b_+39); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+39, b_+42); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+42, b_+45); A = mem_rd(gb, wcc93);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+46, b_+47); ret_effect(gb); return;
  }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+48); A = alu_inc8(gb, A);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A); // [state] = 1
  CYC(b_+49, b_+52); push_effect(gb, b_+52); smogBoss_spawnEnemy(gb, sp0_);
  CYC(b_+52, b_+55); objectCreatePuff_hook(gb);
  return;

state1:
  // Waiting for Link to complete this phase
  CYC(b_+55, b_+58); A = mem_rd(gb, wNumEnemies);
  CYC(b_+58, b_+59); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+59, b_+60); ret_effect(gb); return;
  }
  CYC(b_+59, b_+60);
  CYC(b_+60, b_+62); A = 0x01;
  CYC(b_+62, b_+65); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+65, b_+68); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+68, b_+71); interactionIncState_hook(gb);
  return;

state2:
  // Raise Link off the floor
  CYC(b_+71, b_+74); SET_HL(w1Link_zh);
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+75, b_+77); A = 0xf9;
  CYC(b_+77, b_+78); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+78, b_+79); ret_effect(gb); return;
  }
  CYC(b_+78, b_+79);
  // Get the position to place Link at
  CYC(b_+79, b_+81); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+85); SET_HL(interactionCode33_linkPlacementPositions_bank08);
  CYC(b_+85, b_+86); smogBoss_addDoubleIndex(gb, b_+86);
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_USE_TEXT_ID; // var30
  CYC(b_+88, b_+89); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+89, b_+90); mem_wr(gb, DE, A);
  CYC(b_+90, b_+91); E = alu_inc8(gb, E);
  CYC(b_+91, b_+92); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+92, b_+93); mem_wr(gb, DE, A);
  CYC(b_+93, b_+96); interactionIncState_hook(gb);
  return;

state3:
  // Moving Link to the target position (var30/var31)
  CYC(b_+96, b_+99); SET_HL(w1Link_yh);
  CYC(b_+99, b_+101); E = INTERACTION_BASE + OBJ_USE_TEXT_ID; // var30
  CYC(b_+101, b_+102); A = mem_rd(gb, DE);
  CYC(b_+102, b_+103); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+103, b_+106); goto incOrDecPosition;
  }
  CYC(b_+103, b_+106);
  CYC(b_+106, b_+108); L = (uint8_t)w1Link_xh;
  CYC(b_+108, b_+109); E = alu_inc8(gb, E);
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+111); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+111, b_+114); goto incOrDecPosition;
  }
  CYC(b_+111, b_+114);
  CYC(b_+114, b_+117); interactionIncState_hook(gb);
  return;

incOrDecPosition:
  if (F & FC) {
    CYCT(b_+117, b_+119);
    CYC(b_+121, b_+122); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+122, b_+123); ret_effect(gb);
    return;
  }
  CYC(b_+117, b_+119);
  CYC(b_+119, b_+120); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+120, b_+121); ret_effect(gb);
  return;

state4:
  // Moving Link back to the ground
  CYC(b_+123, b_+126); SET_HL(w1Link_zh);
  CYC(b_+126, b_+127); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+127, b_+128); ret_effect(gb); return;
  }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+131); interactionIncState_hook(gb);
  return;

state5:
  // Waiting for Link to complete this phase; then point var18 at this phase's tile data
  CYC(b_+131, b_+134); A = mem_rd(gb, wNumEnemies);
  CYC(b_+134, b_+135); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+135, b_+136); ret_effect(gb); return;
  }
  CYC(b_+135, b_+136);
  CYC(b_+136, b_+138); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+138, b_+139); A = mem_rd(gb, DE);
  CYC(b_+139, b_+142); SET_HL(interactionCode33_tileReplacementTable_bank08);
  CYC(b_+142, b_+143); push_effect(gb, b_+143); smogBoss_addAToHl(gb);
  CYC(b_+143, b_+144); A = mem_rd(gb, HL);
  CYC(b_+144, b_+145); push_effect(gb, b_+145); smogBoss_addAToHl(gb);
  CYC(b_+145, b_+147); E = INTERACTION_BASE + OBJ_VAR18;
  CYC(b_+147, b_+148); A = L;
  CYC(b_+148, b_+149); mem_wr(gb, DE, A);
  CYC(b_+149, b_+150); E = alu_inc8(gb, E);
  CYC(b_+150, b_+151); A = H;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CYC(b_+152, b_+154); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+154, b_+156); A = 0x05;
  CYC(b_+156, b_+157); mem_wr(gb, DE, A);
  CYC(b_+157, b_+160); interactionIncState_hook(gb);
  return;

state6:
  // Generate the tiles to be used in this phase, one per 5 frames
  CALL_C(b_+160, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+163);
  if (!(F & FZ)) {
    CYCT(b_+163, b_+164); ret_effect(gb); return;
  }
  CYC(b_+163, b_+164);
  CYC(b_+164, b_+166); mem_wr(gb, HL, 0x05);
  // Retrieve pointer to tile replacement data
  CYC(b_+166, b_+168); L = INTERACTION_BASE + OBJ_VAR18;
  CYC(b_+168, b_+169); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+169, b_+170); H = mem_rd(gb, HL);
  CYC(b_+170, b_+171); L = A;
  CYC(b_+171, b_+172); A = mem_rd(gb, HL);
  CYC(b_+172, b_+173); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+173, b_+176); TAIL(interactionIncState);
  }
  CYC(b_+173, b_+176);
  // First byte read was position; move interaction here for the purpose of creating
  // the "poof".
  CALL_C(b_+176, convertShortToLongPosition_hook, SYM(convertShortToLongPosition), b_+179);
  CYC(b_+179, b_+181); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+181, b_+182); A = B;
  CYC(b_+182, b_+183); mem_wr(gb, DE, A);
  CYC(b_+183, b_+185); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+185, b_+186); A = C;
  CYC(b_+186, b_+187); mem_wr(gb, DE, A);
  // Change the tile index
  CYC(b_+187, b_+188); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+188, b_+189); C = A;
  CYC(b_+189, b_+190); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+190, b_+191); push_effect(gb, HL);
  CALL_C(b_+191, setTile_hook, SYM(setTile), b_+194);
  CYC(b_+194, b_+195); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(b_+195, b_+196); ret_effect(gb); return;
  }
  CYC(b_+195, b_+196);
  // Save pointer
  CYC(b_+196, b_+198); E = INTERACTION_BASE + OBJ_VAR18;
  CYC(b_+198, b_+199); A = L;
  CYC(b_+199, b_+200); mem_wr(gb, DE, A);
  CYC(b_+200, b_+201); E = alu_inc8(gb, E);
  CYC(b_+201, b_+202); A = H;
  CYC(b_+202, b_+203); mem_wr(gb, DE, A);
  CYC(b_+203, b_+206); objectCreatePuff_hook(gb);
  return;

state7:
  // Spawn the enemies
  CALL_C(b_+206, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+209);
  if (!(F & FZ)) {
    CYCT(b_+209, b_+210); ret_effect(gb); return;
  }
  CYC(b_+209, b_+210);
  CALL_C(b_+210, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+213);
  CYC(b_+213, b_+215); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  CYC(b_+215, b_+217); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+217, b_+218); A = mem_rd(gb, DE);
  CYC(b_+218, b_+220); E = INTERACTION_BASE + OBJ_VAR35;
  CYC(b_+220, b_+221); mem_wr(gb, DE, A);
  CYC(b_+221, b_+223); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+223, b_+224); A = mem_rd(gb, DE);
  CYC(b_+224, b_+227); SET_HL(interactionCode33_numEnemiesToSpawn_bank08);
  CYC(b_+227, b_+228); push_effect(gb, b_+228); smogBoss_addAToHl(gb);
  CYC(b_+228, b_+229); A = mem_rd(gb, HL);
spawnLoop:
  CYC(b_+229, b_+232); push_effect(gb, b_+232); smogBoss_spawnEnemy(gb, sp0_);
  CYC(b_+232, b_+233); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+233, b_+235); goto spawnLoop;
  }
  CYC(b_+233, b_+235);
  CYC(b_+235, b_+238); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  // Return position to top-left corner
  CYC(b_+238, b_+240); A = 0x18;
  CYC(b_+240, b_+242); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+242, b_+243); mem_wr(gb, DE, A);
  CYC(b_+243, b_+245); alu_sub(gb, 0x04);
  CYC(b_+245, b_+247); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+247, b_+248); mem_wr(gb, DE, A);
  CYC(b_+248, b_+251); interactionIncState_hook(gb);
  return;

state8:
  // Run the phase; constantly checks whether any 2 enemies are close enough to merge.
  // If [wNumEnemies] == 1, this phase is over
  CYC(b_+251, b_+254); A = mem_rd(gb, wNumEnemies);
  CYC(b_+254, b_+255); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+255, b_+258); TAIL(interactionIncState);
  }
  CYC(b_+255, b_+258);
  // If [wNumEnemies] == 2, there's only one, big smog on-screen
  CYC(b_+258, b_+259); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+259, b_+261); smogBoss_checkMergeSmogs(gb, sp0_); return;
  }
  CYC(b_+259, b_+261);
  // Check whether the switch tile has changed (Link's stepped on it)
  CALL_C(b_+261, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+264);
  CYC(b_+264, b_+266); alu_cp(gb, 0x0c); // TILEINDEX_BUTTON
  if (!(F & FZ)) {
    CYCT(b_+266, b_+268); goto buttonPressed;
  }
  CYC(b_+266, b_+268);
  CYC(b_+268, b_+271); A = W8(w1Link_state);
  CYC(b_+271, b_+273); alu_cp(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) {
    CYCT(b_+273, b_+275); smogBoss_checkMergeSmogs(gb, sp0_); return;
  }
  CYC(b_+273, b_+275);
  CYC(b_+275, b_+278); A = mem_rd(gb, wLinkInAir);
  CYC(b_+278, b_+279); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+279, b_+281); smogBoss_checkMergeSmogs(gb, sp0_); return;
  }
  CYC(b_+279, b_+281);
  CYC(b_+281, b_+283); C = 0x04;
  CALL_C(b_+283, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+286);
  if (!(F & FC)) {
    CYCT(b_+286, b_+288); smogBoss_checkMergeSmogs(gb, sp0_); return;
  }
  CYC(b_+286, b_+288);
  // Switch pressed
  CYC(b_+288, b_+290); A = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CYC(b_+290, b_+292); C = 0x11;
  CALL_C(b_+292, setTile_hook, SYM(setTile), b_+295);

buttonPressed:
  // Subtract health as a penalty
  CYC(b_+295, b_+298); SET_HL(wLinkHealth);
  CYC(b_+298, b_+299); A = mem_rd(gb, HL);
  CYC(b_+299, b_+301); alu_cp(gb, 0x0c);
  if (F & FC) {
    CYCT(b_+301, b_+303);
  } else {
    CYC(b_+301, b_+303);
    CYC(b_+303, b_+305); alu_sub(gb, 0x04);
    CYC(b_+305, b_+306); mem_wr(gb, HL, A);
  }
  CYC(b_+306, b_+308); A = 0x87; // SND_SPLASH
  CALL_C(b_+308, playSound_b00_hook, SYM(playSound_b00), b_+311);
  CALL_C(b_+311, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+314);
  CYC(b_+314, b_+316); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));
  CYC(b_+316, b_+318); E = INTERACTION_BASE + OBJ_VAR35;
  CYC(b_+318, b_+319); A = mem_rd(gb, DE);
  CYC(b_+319, b_+321); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+321, b_+322); mem_wr(gb, DE, A);
  CALL_C(b_+322, interactionIncState_hook, SYM(interactionIncState), b_+325);
  CYC(b_+325, b_+327);
  smogBoss_nextPhase(gb);
  return;

state9:
  // Smog destroyed; proceed to the next phase
  CYC(b_+408, b_+410); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+410, b_+411); A = mem_rd(gb, DE);
  CYC(b_+411, b_+412); A = alu_inc8(gb, A);
  CYC(b_+412, b_+413); mem_wr(gb, DE, A);
  CYC(b_+413, b_+415); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+415, b_+417); smogBoss_nextPhase(gb); return;
  }
  CYC(b_+415, b_+417);
  // Final phase completed
  CALL_C(b_+417, decNumEnemies_hook, SYM(decNumEnemies), b_+420);
  CYC(b_+420, b_+423); interactionDelete_hook(gb);
}
