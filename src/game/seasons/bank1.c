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

// setSeason
static void bank1_d_setSeason(GB *gb) {
  BASE(determineSeasonForRoomPack);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+21, b_+24); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+24, b_+26); alu_or(gb, 0x01);
  RET(b_+26); return;
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
  bank1_d_setSeason(gb); return; // falls through
}

// Set a random season for horon village (unless it's spring).
void s_setHoronVillageSeason_hook(GB *gb) {
  BASE(setHoronVillageSeason);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x30;
  CALL_C(b_+2, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+5);
  CYC(b_+5, b_+7); A = 0x00;
  if (!(F & FZ)) { CYCT(b_+7, b_+9); bank1_d_setSeason(gb); return; }
  CYC(b_+7, b_+9);
  CALL_C(b_+9, s_getRandomNumber, SYM(getRandomNumber), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x03);
  CYC(b_+14, b_+16);
  bank1_d_setSeason(gb); return;
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
  bank1_d_setSeason(gb); return;
companionRegion:
  CYC(b_+8, b_+11); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+11, b_+13); alu_sub(gb, 0x0a);
  CYC(b_+13, b_+15); alu_and(gb, 0x03);
  CYC(b_+15, b_+18); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+18, b_+20);
  bank1_d_setSeason(gb); return;
}

