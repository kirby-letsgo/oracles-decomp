#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/bank1.s, the Seasons-only pieces.

static void bank1_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void bank1_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// After a warp, the season of the new room pack: always spring once the flag is set, the
// companion region's for packs $f0+, otherwise the pack's fixed season from roomPackSeasonTable.
void s_checkRoomPackAfterWarp_body_hook(GB *gb) {
  BASE(checkRoomPackAfterWarp_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x30; // GLOBALFLAG_SEASON_ALWAYS_SPRING
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wRoomPack);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); TAIL_S(determineSeasonForRoomPack); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+13, b_+16); TAIL_S(determineCompanionRegionSeason); }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+21); SET_HL(SYM(roomPackSeasonTable));
  CYC(b_+21, b_+22); bank1_add_a_to_hl(gb, b_+22);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+26); mem_wr(gb, wRoomStateModifier, A);
  RET(b_+26); return;
}

static uint16_t bank1_d_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void bank1_d_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// screenTransitionLostWoods@checkTransition
static void bank1_d_checkTransition(GB *gb) {
  BASE(screenTransitionLostWoods);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+46, b_+49); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+49, b_+50); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto wrongWay; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+55); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+55, b_+56); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto wrongWay; }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+59); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+59, b_+61);
  goto L_5e12;
wrongWay:
  CYC(b_+61, b_+62); alu_xor(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
L_5e12:
  CYC(b_+63, b_+64); alu_xor(gb, A);
  RET(b_+64); return;
}

// screenTransitionLostWoods@checkSwordUpgradeTransitions
static void bank1_d_checkSwordUpgradeTransitions(GB *gb) {
  BASE(screenTransitionLostWoods);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto checkSwordUpgradeTransitions;
checkTransition:
  CYC(b_+46, b_+49); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+49, b_+50); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto wrongWay; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+55); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+55, b_+56); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto wrongWay; }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+59); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+59, b_+61);
  goto L_5e12;
wrongWay:
  CYC(b_+61, b_+62); alu_xor(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
L_5e12:
  CYC(b_+63, b_+64); alu_xor(gb, A);
  RET(b_+64); return;

checkSwordUpgradeTransitions:
  CYC(b_+94, b_+97); A = mem_rd(gb, wLostWoodsTransitionCounter2);
  CYC(b_+97, b_+98); push_effect(gb, b_+98);
  do { uint16_t jt_ = (bank1_d_jump_table(gb));
    if (jt_ == b_+106) goto checkSwordUpgradeTransitions_transition0;
    if (jt_ == b_+114) goto checkSwordUpgradeTransitions_transition1;
    if (jt_ == b_+122) goto checkSwordUpgradeTransitions_transition2;
    if (jt_ == b_+130) goto checkSwordUpgradeTransitions_transition3;
    HANDOFF(HL);
  } while (0);
checkSwordUpgradeTransitions_transition0:
  CYC(b_+106, b_+109); SET_BC(0x0303);
  CYC(b_+109, b_+112); SET_HL(wLostWoodsTransitionCounter2);
  CYC(b_+112, b_+114);
  goto checkTransition;
checkSwordUpgradeTransitions_transition1:
  CYC(b_+114, b_+117); SET_BC(0x0302);
  CYC(b_+117, b_+120); SET_HL(wLostWoodsTransitionCounter2);
  CYC(b_+120, b_+122);
  goto checkTransition;
checkSwordUpgradeTransitions_transition2:
  CYC(b_+122, b_+125); SET_BC(0x0300);
  CYC(b_+125, b_+128); SET_HL(wLostWoodsTransitionCounter2);
  CYC(b_+128, b_+130);
  goto checkTransition;
checkSwordUpgradeTransitions_transition3:
  CYC(b_+130, b_+133); SET_BC(0x0301);
  CYC(b_+133, b_+136); SET_HL(wLostWoodsTransitionCounter2);
  CALL_L(b_+136, bank1_d_checkTransition, b_+139);
  CYC(b_+139, b_+140); A = mem_rd(gb, HL);
  CYC(b_+140, b_+142); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+142); return; }
  CYC(b_+142, b_+143);
  CYC(b_+143, b_+145); mem_wr(gb, HL, 0x00);
  CYC(b_+145, b_+147); A = 0xc9;
  CYC(b_+147, b_+150); mem_wr(gb, wActiveRoom, A);
  CYC(b_+150, b_+151); alu_scf(gb);
  RET(b_+151); return;
}

// screenTransitionLostWoods@checkTransitionForNorth
static void bank1_d_checkTransitionForNorth(GB *gb) {
  BASE(screenTransitionLostWoods);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+43, b_+46); SET_HL(wEyePuzzleTransitionCounter);
  CYC(b_+46, b_+49); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+49, b_+50); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto wrongWay; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+55); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+55, b_+56); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto wrongWay; }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+59); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+59, b_+61);
  goto L_5e12;
wrongWay:
  CYC(b_+61, b_+62); alu_xor(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
L_5e12:
  CYC(b_+63, b_+64); alu_xor(gb, A);
  RET(b_+64); return;
}

// screenTransitionLostWoods@checkMoveNorthTransitions
static void bank1_d_checkMoveNorthTransitions(GB *gb) {
  BASE(screenTransitionLostWoods);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+28, b_+31); A = mem_rd(gb, wEyePuzzleTransitionCounter);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (bank1_d_jump_table(gb));
    if (jt_ == b_+40) goto transition0;
    if (jt_ == b_+65) goto transition1;
    if (jt_ == b_+70) goto transition2;
    if (jt_ == b_+75) goto transition3;
    HANDOFF(HL);
  } while (0);
transition0:
  CYC(b_+40, b_+43); SET_BC(0x0303);
checkTransitionForNorth:
  CYC(b_+43, b_+46); SET_HL(wEyePuzzleTransitionCounter);
  CYC(b_+46, b_+49); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+49, b_+50); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto wrongWay; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+55); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+55, b_+56); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto wrongWay; }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+59); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+59, b_+61);
  goto L_5e12;
wrongWay:
  CYC(b_+61, b_+62); alu_xor(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
L_5e12:
  CYC(b_+63, b_+64); alu_xor(gb, A);
  RET(b_+64); return;
transition1:
  CYC(b_+65, b_+68); SET_BC(0x0202);
  CYC(b_+68, b_+70);
  goto checkTransitionForNorth;
transition2:
  CYC(b_+70, b_+73); SET_BC(0x0100);
  CYC(b_+73, b_+75);
  goto checkTransitionForNorth;
transition3:
  CYC(b_+75, b_+78); SET_BC(0x0001);
  CALL_L(b_+78, bank1_d_checkTransitionForNorth, b_+81);
  CYC(b_+81, b_+82); A = mem_rd(gb, HL);
  CYC(b_+82, b_+84); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+84); return; }
  CYC(b_+84, b_+85);
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x00);
  CYC(b_+87, b_+89); A = 0x30;
  CYC(b_+89, b_+92); mem_wr(gb, wActiveRoom, A);
  CYC(b_+92, b_+93); alu_scf(gb);
  RET(b_+93); return;
}

void s_screenTransitionLostWoods_hook(GB *gb) {
  BASE(screenTransitionLostWoods);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_L(b_+0, bank1_d_checkMoveNorthTransitions, b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_L(b_+4, bank1_d_checkSwordUpgradeTransitions, b_+7);
  if (F & FC) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto L_5de8; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); A = mem_rd(gb, wEyePuzzleTransitionCounter);
  CYC(b_+17, b_+19); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); TAIL(screenTransitionStandard); }
  CYC(b_+19, b_+21);
L_5de8:
  CYC(b_+21, b_+23); A = 0x40;
  CYC(b_+23, b_+26); mem_wr(gb, wActiveRoom, A);
  CYC(b_+26, b_+27); alu_scf(gb);
  RET(b_+27); return;
}

// The sword upgrade screen is actually located where you'd expect the maku tree to be, so
// override the destination room.
void s_screenTransitionSwordUpgrade_hook(GB *gb) {
  BASE(screenTransitionSwordUpgrade);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_clearEyePuzzleVars, SYM(clearEyePuzzleVars), b_+3);
  CYC(b_+3, b_+5); A = 0x40;
  CYC(b_+5, b_+8); mem_wr(gb, wActiveRoom, A);
  CYC(b_+8, b_+9); alu_scf(gb);
  RET(b_+9); return;
}

// Can't proceed in onox's dungeon until enemies are dead. Also, going to the left or right
// rooms always send you back near the entrance.
void s_screenTransitionOnoxDungeon_hook(GB *gb) {
  BASE(screenTransitionOnoxDungeon);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (bank1_d_jump_table(gb));
    if (jt_ == b_+14) goto up;
    if (jt_ == b_+28) goto right;
    if (jt_ == SYM(screenTransitionStandard) && hook_is(gb, SYM(screenTransitionStandard), s_screenTransitionStandard)) { s_screenTransitionStandard(gb); return; }
    if (jt_ == b_+33) goto left;
    HANDOFF(HL);
  } while (0);
up:
  CALL_C(b_+14, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); TAIL(screenTransitionStandard); }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+25); A = mem_rd(gb, wActiveRoom);
  CYC(b_+25, b_+26); B = A;
  CYC(b_+26, b_+28);
  goto L_5e9d;
right:
  CYC(b_+28, b_+31); SET_BC(0x9834);
  CYC(b_+31, b_+33);
  goto L_5e99;
left:
  CYC(b_+33, b_+36); SET_BC(0x9632);
L_5e99:
  CYC(b_+36, b_+37); A = C;
  CYC(b_+37, b_+40); mem_wr(gb, wDungeonMapPosition, A);
L_5e9d:
  CYC(b_+40, b_+41); A = B;
  CYC(b_+41, b_+44); mem_wr(gb, wActiveRoom, A);
  CYC(b_+44, b_+45); alu_scf(gb);
  RET(b_+45); return;
}

// @param	a	Room pack value
void s_determineSeasonForRoomPack_hook(GB *gb) {
  BASE(determineSeasonForRoomPack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+2, b_+4); TAIL_S(determineCompanionRegionSeason); }
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); A = 0x30;
  CALL_C(b_+6, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wLoadingRoomPack);
  if (F & FZ) { CYCT(b_+12, b_+14); goto L_7e19; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x0f);
L_7e19:
  CYC(b_+16, b_+19); SET_HL(SYM(roomPackSeasonTable));
  CYC(b_+19, b_+20); bank1_d_add_a_to_hl(gb, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  s_setSeason_b01_hook(gb); return; // falls through
}

// Set a random season for horon village (unless it's spring).
void s_setHoronVillageSeason_hook(GB *gb) {
  BASE(setHoronVillageSeason);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x30;
  CALL_C(b_+2, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+5);
  CYC(b_+5, b_+7); A = 0x00;
  if (!(F & FZ)) { CYCT(b_+7, b_+9); s_setSeason_b01_hook(gb); return; }
  CYC(b_+7, b_+9);
  CALL_C(b_+9, s_getRandomNumber, SYM(getRandomNumber), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x03);
  CYC(b_+14, b_+16);
  s_setSeason_b01_hook(gb); return;
}

// @param	a
void s_determineCompanionRegionSeason_hook(GB *gb) {
  BASE(determineCompanionRegionSeason);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+2, b_+4); goto companionRegion; }
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); A = 0x01;
  CYC(b_+6, b_+8);
  s_setSeason_b01_hook(gb); return;
companionRegion:
  CYC(b_+8, b_+11); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+11, b_+13); alu_sub(gb, 0x0a);
  CYC(b_+13, b_+15); alu_and(gb, 0x03);
  CYC(b_+15, b_+18); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+18, b_+20);
  s_setSeason_b01_hook(gb); return;
}


// setSeason in bank 1 (the symbol names bank 0's copy, so the base is the routine before it):
// room state modifier a, NZ.
void s_setSeason_b01_hook(GB *gb) {
  BASE(determineSeasonForRoomPack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+21, b_+24); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+24, b_+26); alu_or(gb, 0x01);
  RET(b_+26); return;
}

static void bank1_incSubstate(GB *gb);

void s_cutscene06_hook(GB *gb) {
  BASE(cutscene06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x00;
  CYC(b_+2, b_+5);
  TAIL_S(multiIntroCutsceneCaller);
}

void s_cutscene07_hook(GB *gb) {
  BASE(cutscene07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x01;
  CALL_C(b_+2, s_multiIntroCutsceneCaller_hook, SYM(multiIntroCutsceneCaller), b_+5);
  CALL_C(b_+5, s_updateInteractionsAndDrawAllSprites, SYM(updateInteractionsAndDrawAllSprites), b_+8);
  CYC(b_+8, b_+11);
  TAIL_S(updateAnimationsAfterCutscene);
}

void s_cutscene08_hook(GB *gb) {
  BASE(cutscene08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x02;
  CALL_C(b_+2, s_multiIntroCutsceneCaller_hook, SYM(multiIntroCutsceneCaller), b_+5);
  CYC(b_+5, b_+8);
  TAIL(updateInteractionsAndDrawAllSprites);
}

void s_cutscene0b_hook(GB *gb) {
  BASE(cutscene0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(cutsceneHandler_0b));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, s_interBankCall, SYM(interBankCall), b_+8);
  CYC(b_+8, b_+10);
  TAIL(func_5d31);
}

void s_cutscene0c_hook(GB *gb) {
  BASE(cutscene0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(cutsceneHandler_0c));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, s_interBankCall, SYM(interBankCall), b_+8);
  CYC(b_+8, b_+10);
  TAIL(func_5d31);
}

void s_cutscene0d_hook(GB *gb) {
  BASE(cutscene0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_refreshLoadedTreeGfx, SYM(refreshLoadedTreeGfx), b_+3);
  CYC(b_+3, b_+5); E = 0x03;
  CYC(b_+5, b_+8);
  TAIL_S(multiIntroCutsceneCaller);
}

void s_cutscene0e_hook(GB *gb) {
  BASE(cutscene0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wWarpTransition2);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); TAIL(applyWarpTransition2); }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = 0x04;
  CALL_C(b_+8, s_multiIntroCutsceneCaller_hook, SYM(multiIntroCutsceneCaller), b_+11);
  CYC(b_+11, b_+14);
  TAIL_S(updateAnimationsAfterCutscene);
}

void s_cutscene10_hook(GB *gb) {
  BASE(cutscene10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_flameOfDestructionsCutsceneCaller_hook, SYM(flameOfDestructionsCutsceneCaller), b_+3);
  CYC(b_+3, b_+6);
  TAIL(func_5d41);
}

void s_cutscene11_hook(GB *gb) {
  BASE(cutscene11);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_zeldaAndVillagersCutsceneCaller_hook, SYM(zeldaAndVillagersCutsceneCaller), b_+3);
  CYC(b_+3, b_+6);
  TAIL(func_5d31);
}

void s_cutscene12_hook(GB *gb) {
  BASE(cutscene12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_zeldaKidnappedCutsceneCaller_hook, SYM(zeldaKidnappedCutsceneCaller), b_+3);
  CYC(b_+3, b_+6);
  TAIL(func_5d41);
}

void s_cutscene00_hook(GB *gb) {
  BASE(cutscene00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_updateStatusBar, SYM(updateStatusBar), b_+3);
  CALL_C(b_+3, s_updateAllObjects, SYM(updateAllObjects), b_+6);
  CALL_C(b_+6, s_refreshLoadedTreeGfx, SYM(refreshLoadedTreeGfx), b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wScrollMode);
  CYC(b_+12, b_+14); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CALL_C(b_+15, s_setInstrumentsDisabledCounterAndScrollMode, SYM(setInstrumentsDisabledCounterAndScrollMode), b_+18);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+22); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(b_+22, s_clearObjectsWithEnabled2, SYM(clearObjectsWithEnabled2), b_+25);
  CALL_C(b_+25, s_refreshObjectGfx, SYM(refreshObjectGfx), b_+28);
  CALL_C(b_+28, s_setVisitedRoomFlag, SYM(setVisitedRoomFlag), b_+31);
  CALL_C(b_+31, s_checkUpdateDungeonMinimap, SYM(checkUpdateDungeonMinimap), b_+34);
  CYC(b_+34, b_+36); A = 0x01;
  CYC(b_+36, b_+39); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CALL_C(b_+39, s_playCompassSoundIfKeyInRoom, SYM(playCompassSoundIfKeyInRoom), b_+42);
  CYC(b_+42, b_+45);
  TAIL(updateGrassAnimationModifier);
}

void s_cutscene02_hook(GB *gb) {
  BASE(cutscene02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

// cutscene05+14
static void bank1_cutscene05_14(GB *gb) {
  BASE(cutscene05);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+14, s_setInteractionsEnabledTo2, SYM(setInteractionsEnabledTo2), b_+17);
  CALL_C(b_+17, s_clearObjectsWithEnabled2, SYM(clearObjectsWithEnabled2), b_+20);
  CALL_C(b_+20, s_clearItems, SYM(clearItems), b_+23);
  CALL_C(b_+23, s_clearEnemies, SYM(clearEnemies), b_+26);
  CALL_C(b_+26, s_clearParts, SYM(clearParts), b_+29);
  CALL_C(b_+29, s_clearReservedInteraction0, SYM(clearReservedInteraction0), b_+32);
  CALL_C(b_+32, s_clearScreenVariables, SYM(clearScreenVariables), b_+35);
  CALL_C(b_+35, s_clearMemoryOnScreenReload, SYM(clearMemoryOnScreenReload), b_+38);
  CALL_C(b_+38, s_loadScreenMusicAndSetRoomPack, SYM(loadScreenMusicAndSetRoomPack), b_+41);
  CALL_C(b_+41, s_loadTilesetData, SYM(loadTilesetData), b_+44);
  CALL_C(b_+44, s_loadTilesetGraphics, SYM(loadTilesetGraphics), b_+47);
  CALL_C(b_+47, s_func_131f, SYM(func_131f), b_+50);
  CYC(b_+50, b_+53); SET_DE(w1Link_yh);
  CALL_C(b_+53, s_getShortPositionFromDE, SYM(getShortPositionFromDE), b_+56);
  CYC(b_+56, b_+59); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+59, b_+62);
  TAIL(func_5c18);
}

void s_cutscene04_hook(GB *gb) {
  BASE(cutscene04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, s_disableLcd, SYM(disableLcd), b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wWarpDestGroup);
  CYC(b_+11, b_+13); alu_and(gb, 0x07);
  CYC(b_+13, b_+16); mem_wr(gb, wActiveGroup, A);
  CYC(b_+16, b_+19); A = mem_rd(gb, wWarpDestRoom);
  CYC(b_+19, b_+22); mem_wr(gb, wActiveRoom, A);
  CYC(b_+22, b_+25); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+25, b_+26); H = A;
  CYC(b_+26, b_+28); L = 0x0b;
  CYC(b_+28, b_+31); A = mem_rd(gb, wWarpDestPos);
  CALL_C(b_+31, s_setShortPosition, SYM(setShortPosition), b_+34);
  CYC(b_+34, b_+36);
  bank1_cutscene05_14(gb); return;
}

void s_cutscene13_hook(GB *gb) {
  BASE(cutscene13);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (bank1_d_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+60) goto state1;
    if (jt_ == b_+70) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+15); mem_wr(gb, wCutsceneState, A);
  CYC(b_+15, b_+18); SET_HL(wTmpcfc0 + 0x08);
  CYC(b_+18, b_+20); B = 0x18;
  CALL_C(b_+20, s_clearMemory, SYM(clearMemory), b_+23);
  CYC(b_+23, b_+25); A = 0x07;
  CYC(b_+25, b_+28); mem_wr(gb, wActiveGroup, A);
  CYC(b_+28, b_+30); A = 0xff;
  CYC(b_+30, b_+33); mem_wr(gb, wActiveRoom, A);
  CYC(b_+33, b_+35); A = 0x77;
  CYC(b_+35, b_+38); mem_wr(gb, wDungeonMapPosition, A);
  CYC(b_+38, b_+40); A = 0x28;
  CYC(b_+40, b_+43); mem_wr(gb, wTilesetFlags, A);
  CYC(b_+43, b_+45); A = 0x02;
  CYC(b_+45, b_+47); mem_wr(gb, IO_SVBK, A);
  CYC(b_+47, b_+50); SET_HL(w2DungeonLayout + 0x3f);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0xff);
  CYC(b_+52, b_+53); alu_xor(gb, A);
  CYC(b_+53, b_+55); mem_wr(gb, IO_SVBK, A);
  CYC(b_+55, b_+57); A = 0x04;
  CYC(b_+57, b_+60);
  TAIL(fadeoutToWhiteWithDelay);
state1:
  CYC(b_+60, b_+63); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+63, b_+64); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+64); return; }
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+67); A = 0x02;
  CYC(b_+67, b_+70); mem_wr(gb, wCutsceneState, A);
state2:
  CALL_C(b_+70, s_refreshLoadedTreeGfx, SYM(refreshLoadedTreeGfx), b_+73);
  CALL_C(b_+73, s_updateMenus, SYM(updateMenus), b_+76);
  if (!(F & FZ)) { RET_TAKEN(b_+76); return; }
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+80); A = mem_rd(gb, wWarpTransition2);
  CYC(b_+80, b_+81); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+81, b_+84); TAIL(applyWarpTransition2); }
  CYC(b_+81, b_+84);
  CALL_C(b_+84, s_seasonsFunc_331b_hook, SYM(seasonsFunc_331b), b_+87);
  CALL_C(b_+87, s_seasonsFunc_34a0_hook, SYM(seasonsFunc_34a0), b_+90);
  CALL_C(b_+90, s_updateStatusBar, SYM(updateStatusBar), b_+93);
  CYC(b_+93, b_+96); A = mem_rd(gb, wCutsceneTrigger);
  CYC(b_+96, b_+97); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+97, b_+100); TAIL(checkEnemyAndPartCollisionsIfTextInactive); }
  CYC(b_+97, b_+100);
  CYC(b_+100, b_+103);
  TAIL(setCutsceneIndexIfCutsceneTriggerSet);
}

// cutscene14@initWaveScrollValues_everyOtherLine
static void bank1_initWaveScrollValues_everyOtherLine(GB *gb) {
  BASE(cutscene14);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+320, s_initWaveScrollValues, SYM(initWaveScrollValues), b_+323);
  CYC(b_+323, b_+325); A = 0x02;
  CYC(b_+325, b_+327); mem_wr(gb, IO_SVBK, A);
  CYC(b_+327, b_+330); SET_HL(w2WaveScrollValues);
  CYC(b_+330, b_+332); B = 0x80;
clearLoop:
  CYC(b_+332, b_+333); alu_xor(gb, A);
  CYC(b_+333, b_+334); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+334, b_+335); SET_HL(HL + 1);
  CYC(b_+335, b_+336); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+336, b_+338); goto clearLoop; }
  CYC(b_+336, b_+338);
  CYC(b_+338, b_+339); alu_xor(gb, A);
  CYC(b_+339, b_+341); mem_wr(gb, IO_SVBK, A);
  RET(b_+341); return;
}

// cutscene14@state1@substate0
static void bank1_state1_substate0(GB *gb) {
  BASE(cutscene14);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_L(b_+81, bank1_incSubstate, b_+84);
  CYC(b_+84, b_+86); A = 0x10;
  CYC(b_+86, b_+89); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+89, b_+91); A = 0x02;
  CYC(b_+91, b_+93); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+93, b_+94); alu_xor(gb, A);
  CYC(b_+94, b_+97); mem_wr(gb, wTmpcbb7, A);
  CALL_L(b_+97, bank1_initWaveScrollValues_everyOtherLine, b_+100);
  CYC(b_+100, b_+102); A = 0x10;
  CYC(b_+102, b_+105); mem_wr(gb, wLinkForceState, A);
  CYC(b_+105, b_+107); A = 0xb9;
  CYC(b_+107, b_+110);
  TAIL(playSound_b00);
}

// cutscene14@incSubstate
static void bank1_incSubstate(GB *gb) {
  BASE(cutscene14);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+53, b_+56); SET_HL(wTmpcbb3);
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+57); return;
}

// cutscene14@handleCutscene
static void bank1_handleCutscene(GB *gb) {
  BASE(cutscene14);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+6, b_+9); A = mem_rd(gb, wCutsceneState);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (bank1_d_jump_table(gb));
    if (jt_ == b_+16) goto state0;
    if (jt_ == b_+69) goto state1;
    if (jt_ == b_+190) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+16, s_reloadTileMap, SYM(reloadTileMap), b_+19);
  CYC(b_+19, b_+21); A = 0x36;
  CALL_C(b_+21, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+24);
  CYC(b_+24, b_+26); A = 0x01;
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto setState; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);
setState:
  CYC(b_+29, b_+32); mem_wr(gb, wCutsceneState, A);
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+36, b_+39); mem_wr(gb, wScrollMode, A);
  CYC(b_+39, b_+41); A = 0x28;
  CYC(b_+41, b_+44); mem_wr(gb, wTmpcbb4, A);
  RET(b_+44); return;

incSubstate:
  CYC(b_+53, b_+56); SET_HL(wTmpcbb3);
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+57); return;

state1:
  CYC(b_+69, b_+72); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+72, b_+73); push_effect(gb, b_+73);
  do { uint16_t jt_ = (bank1_d_jump_table(gb));
    if (jt_ == b_+81) goto state1_substate0;
    if (jt_ == b_+110) goto state1_substate1;
    if (jt_ == b_+125) goto state1_substate2;
    if (jt_ == b_+130) goto state1_substate3;
    HANDOFF(HL);
  } while (0);
state1_substate0:
  CALL_L(b_+81, bank1_incSubstate, b_+84);
  CYC(b_+84, b_+86); A = 0x10;
  CYC(b_+86, b_+89); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+89, b_+91); A = 0x02;
  CYC(b_+91, b_+93); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+93, b_+94); alu_xor(gb, A);
  CYC(b_+94, b_+97); mem_wr(gb, wTmpcbb7, A);
  CALL_L(b_+97, bank1_initWaveScrollValues_everyOtherLine, b_+100);
  CYC(b_+100, b_+102); A = 0x10;
  CYC(b_+102, b_+105); mem_wr(gb, wLinkForceState, A);
  CYC(b_+105, b_+107); A = 0xb9;
  CYC(b_+107, b_+110);
  TAIL(playSound_b00);
state1_substate1:
  CYC(b_+110, b_+112); A = 0x02;
  CALL_C(b_+112, s_loadBigBufferScrollValues, SYM(loadBigBufferScrollValues), b_+115);
  CYC(b_+115, b_+118); SET_HL(wTmpcbb7);
  CYC(b_+118, b_+119); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+119, b_+120); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(b_+120, b_+123); goto initWaveScrollValues_everyOtherLine; }
  CYC(b_+120, b_+123);
  CYC(b_+123, b_+125);
  goto incSubstate;
state1_substate2:
  CYC(b_+125, b_+127); A = 0x02;
  CYC(b_+127, b_+130);
  TAIL(loadBigBufferScrollValues);
state1_substate3:
  CYC(b_+130, b_+132); A = 0x02;
  CALL_C(b_+132, s_loadBigBufferScrollValues, SYM(loadBigBufferScrollValues), b_+135);
  CYC(b_+135, b_+138); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+138, b_+139); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+139); return; }
  CYC(b_+139, b_+140);
  CYC(b_+140, b_+143); A = mem_rd(gb, w2TmpGfxBuffer + 0x0d);
  CYC(b_+143, b_+145); alu_and(gb, 0xf0);
  CYC(b_+145, b_+147); A = alu_swap(gb, A);
  CYC(b_+147, b_+149); alu_or(gb, 0x10);
  CYC(b_+149, b_+152); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+152, b_+154); A = 0x80;
  CYC(b_+154, b_+157); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+157, b_+159); A = 0x33;
  CYC(b_+159, b_+162); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+162, b_+164); A = 0x0c;
  CYC(b_+164, b_+167); mem_wr(gb, wWarpTransition, A);
  CYC(b_+167, b_+168); alu_xor(gb, A);
  CYC(b_+168, b_+171); mem_wr(gb, wcc50, A);
  CYC(b_+171, b_+173); A = 0x03;
  CYC(b_+173, b_+176); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CYC(b_+176, b_+178); A = 0x03;
  CYC(b_+178, b_+180); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+180, b_+182); A = 0x24;
  CALL_C(b_+182, s_setGlobalFlag, SYM(setGlobalFlag), b_+185);
  CYC(b_+185, b_+187); A = 0xf1;
  CYC(b_+187, b_+190);
  TAIL(playSound_b00);
state2:
  CYC(b_+190, b_+193); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+193, b_+194); push_effect(gb, b_+194);
  do { uint16_t jt_ = (bank1_d_jump_table(gb));
    if (jt_ == b_+202) goto state2_substate0;
    if (jt_ == b_+230) goto state2_substate1;
    if (jt_ == b_+125) goto state1_substate2;
    if (jt_ == b_+274) goto state2_substate3;
    HANDOFF(HL);
  } while (0);
state2_substate0:
  CALL_L(b_+202, bank1_state1_substate0, b_+205);
  CYC(b_+205, b_+207); A = 0xab;
  CALL_C(b_+207, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+210);
  CALL_C(b_+210, s_resetLinkInvincibility, SYM(resetLinkInvincibility), b_+213);
  CYC(b_+213, b_+215); A = 0x0b;
  CYC(b_+215, b_+218); mem_wr(gb, wLinkForceState, A);
  CYC(b_+218, b_+220); A = 0x20;
  CYC(b_+220, b_+223); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+223, b_+224); alu_xor(gb, A);
  CYC(b_+224, b_+227); SET_HL(w2TmpGfxBuffer + 0x08);
  CYC(b_+227, b_+228); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+228, b_+229); mem_wr(gb, HL, A);
  RET(b_+229); return;
state2_substate1:
  CYC(b_+230, b_+232); A = 0x02;
  CALL_C(b_+232, s_loadBigBufferScrollValues, SYM(loadBigBufferScrollValues), b_+235);
  CYC(b_+235, b_+238); SET_HL(wTmpcbb7);
  CYC(b_+238, b_+239); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+239, b_+240); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+240, b_+241); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(b_+241, b_+244); goto initWaveScrollValues_everyOtherLine; }
  CYC(b_+241, b_+244);
  CALL_L(b_+244, bank1_incSubstate, b_+247);
  CALL_C(b_+247, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+250);
  if (!(F & FZ)) { RET_TAKEN(b_+250); return; }
  CYC(b_+250, b_+251);
  CYC(b_+251, b_+253); mem_wr(gb, HL, 0xde);
  RET(b_+253); return;

state2_substate3:
  CYC(b_+274, b_+276); A = 0x02;
  CALL_C(b_+276, s_loadBigBufferScrollValues, SYM(loadBigBufferScrollValues), b_+279);
  CYC(b_+279, b_+282); SET_HL(wTmpcbb7);
  CYC(b_+282, b_+283); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+283, b_+284); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(b_+284, b_+287); goto initWaveScrollValues_everyOtherLine; }
  CYC(b_+284, b_+287);
  CALL_C(b_+287, s_refreshObjectGfx, SYM(refreshObjectGfx), b_+290);
  CYC(b_+290, b_+292); A = 0x01;
  CYC(b_+292, b_+295); mem_wr(gb, wLinkForceState, A);
  CYC(b_+295, b_+298); SET_HL(SYM(objectData7e40));
  CALL_C(b_+298, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+301);
  CYC(b_+301, b_+303); A = 0x01;
  CYC(b_+303, b_+306); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CYC(b_+306, b_+308); A = 0x03;
  CYC(b_+308, b_+310); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+310, b_+312); A = 0x01;
  CYC(b_+312, b_+315); mem_wr(gb, wScrollMode, A);
  CYC(b_+315, b_+317); A = 0xf1;
  CYC(b_+317, b_+320);
  TAIL(playSound_b00);
initWaveScrollValues_everyOtherLine:
  CALL_C(b_+320, s_initWaveScrollValues, SYM(initWaveScrollValues), b_+323);
  CYC(b_+323, b_+325); A = 0x02;
  CYC(b_+325, b_+327); mem_wr(gb, IO_SVBK, A);
  CYC(b_+327, b_+330); SET_HL(w2WaveScrollValues);
  CYC(b_+330, b_+332); B = 0x80;
clearLoop:
  CYC(b_+332, b_+333); alu_xor(gb, A);
  CYC(b_+333, b_+334); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+334, b_+335); SET_HL(HL + 1);
  CYC(b_+335, b_+336); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+336, b_+338); goto clearLoop; }
  CYC(b_+336, b_+338);
  CYC(b_+338, b_+339); alu_xor(gb, A);
  CYC(b_+339, b_+341); mem_wr(gb, IO_SVBK, A);
  RET(b_+341); return;
}

void s_cutscene14_hook(GB *gb) {
  BASE(cutscene14);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_L(b_+0, bank1_handleCutscene, b_+3);
  CYC(b_+3, b_+6);
  TAIL(updateAllObjects);
}

void s_applyPaletteFadeTransitionData_hook(GB *gb) {
  BASE(applyPaletteFadeTransitionData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(HL + 1);
  CYC(b_+1, b_+3); A = 0x02;
  CYC(b_+3, b_+5); mem_wr(gb, IO_SVBK, A);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_rrca(gb);
  CYC(b_+10, b_+13); SET_HL(SYM(paletteTransitionSeasonData));
  CYC(b_+13, b_+14); bank1_add_a_to_hl(gb, b_+14);
  CYC(b_+14, b_+17); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+17, b_+18); bank1_add_double_index(gb, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); H = mem_rd(gb, HL);
  CYC(b_+20, b_+21); L = A;
  CYC(b_+21, b_+24); SET_DE(w2ColorComponentBuffer1);
  CALL_C(b_+24, s_extractColorComponents, SYM(extractColorComponents), b_+27);
  CYC(b_+27, b_+28); SET_HL(pop_effect(gb));
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); A = alu_swap(gb, A);
  CYC(b_+31, b_+32); alu_rrca(gb);
  CYC(b_+32, b_+35); SET_HL(SYM(paletteTransitionSeasonData));
  CYC(b_+35, b_+36); bank1_add_a_to_hl(gb, b_+36);
  CYC(b_+36, b_+39); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+39, b_+40); bank1_add_double_index(gb, b_+40);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+41, b_+42); H = mem_rd(gb, HL);
  CYC(b_+42, b_+43); L = A;
  CYC(b_+43, b_+46); SET_DE(w2ColorComponentBuffer2);
  CALL_C(b_+46, s_extractColorComponents, SYM(extractColorComponents), b_+49);
  CYC(b_+49, b_+51); A = 0x00;
  CYC(b_+51, b_+53); mem_wr(gb, IO_SVBK, A);
  CYC(b_+53, b_+55); A = 0xff;
  CYC(b_+55, b_+58); mem_wr(gb, wLoadedTilesetPalette, A);
  CYC(b_+58, b_+61);
  TAIL(startFadeBetweenTwoPalettes);
}

void s_checkRoomPack_hook(GB *gb) {
  BASE(checkRoomPack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveGroup);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto group0; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  RET(b_+7); return;
group0:
  CYC(b_+8, b_+11); A = mem_rd(gb, wRoomPack);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+15); A = mem_rd(gb, wLoadingRoomPack);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+19); A = mem_rd(gb, wRoomPack);
  CYC(b_+19, b_+20); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+26); mem_wr(gb, wRoomPack, A);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+27, b_+29); TAIL_S(setHoronVillageSeason); }
  CYC(b_+27, b_+29);
  s_determineSeasonForRoomPack_hook(gb); return; // falls through
}

void s_findActiveRoomInDungeonLayout_hook(GB *gb) {
  BASE(findActiveRoomInDungeonLayout);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, s_getFirstDungeonLayoutAddress, SYM(getFirstDungeonLayoutAddress), b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wActiveRoom);
  CYC(b_+7, b_+9); C = 0x00;
nextFloor:
  CYC(b_+9, b_+11); B = 0x40;
nextRoom:
  CYC(b_+11, b_+12); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+12, b_+14); goto found; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); SET_HL(HL + 1);
  CYC(b_+15, b_+16); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto nextRoom; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); C = alu_inc8(gb, C);
  CYC(b_+19, b_+21);
  goto nextFloor;
found:
  CYC(b_+21, b_+22); A = C;
  CYC(b_+22, b_+25); mem_wr(gb, wDungeonFloor, A);
  CYC(b_+25, b_+27); A = 0x40;
  CYC(b_+27, b_+28); alu_sub(gb, B);
  CYC(b_+28, b_+31); mem_wr(gb, wDungeonMapPosition, A);
  RET(b_+31); return;
}

void s_getPaletteFadeTransitionData_hook(GB *gb) {
  BASE(getPaletteFadeTransitionData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveGroup);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); alu_rrca(gb);
  CYC(b_+5, b_+7); alu_and(gb, 0x7f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+12); SET_HL(SYM(paletteTransitionIndexData));
  CYC(b_+12, b_+13); bank1_add_double_index(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  CYC(b_+15, b_+16); L = A;
  CYC(b_+16, b_+19); A = mem_rd(gb, wActiveRoom);
  CYC(b_+19, b_+20); B = A;
nextEntry:
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_cp(gb, 0xff);
  if (F & FZ) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+29); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+29, b_+30); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto skipEntry; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); A = C;
  CYC(b_+33, b_+34); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+34, b_+36); goto found; }
  CYC(b_+34, b_+36);
skipEntry:
  CYC(b_+36, b_+37); SET_HL(HL + 1);
  CYC(b_+37, b_+38); SET_HL(HL + 1);
  CYC(b_+38, b_+39); SET_HL(HL + 1);
  CYC(b_+39, b_+41);
  goto nextEntry;
found:
  CYC(b_+41, b_+42); alu_scf(gb);
  RET(b_+42); return;
}

void s_initializeSeedTreeRefillData_hook(GB *gb) {
  BASE(initializeSeedTreeRefillData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xfc;
  CYC(b_+2, b_+5); mem_wr(gb, wSeedTreeRefilledBitset, A);
  CYC(b_+5, b_+7); A = 0x03;
  CYC(b_+7, b_+9); mem_wr(gb, IO_SVBK, A);
  CYC(b_+9, b_+12); SET_HL(w3RoomLayoutBuffer + 0xc0);
  CYC(b_+12, b_+14); B = 0x40;
  CALL_C(b_+14, s_clearMemory, SYM(clearMemory), b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+20); mem_wr(gb, IO_SVBK, A);
  RET(b_+20); return;
}
