#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(goron_targetCarts_setLinkPositionToCartPlatform), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(goron_targetCarts_setLinkPositionToCartPlatform), (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

void goron_setLinkPositionAndDirection_hook(GB *gb);
void goron_findInteractionWithID_hook(GB *gb);

static void goronTargetCarts_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void goronTargetCarts_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void goron_targetCarts_setLinkPositionToCartPlatform_hook(GB *gb) {
  BASE(goron_targetCarts_setLinkPositionToCartPlatform);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+5); SET_BC(0x8838);
  CYC(b_+5, SYM(goron_targetCarts_setLinkPositionAfterGame)); goron_setLinkPositionAndDirection_hook(gb);
}

void goron_targetCarts_setLinkPositionAfterGame_hook(GB *gb) {
  BASE(goron_targetCarts_setLinkPositionAfterGame);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); SET_BC((SYM(symmetryNpcSubid8And9Script__askForSecret_b15) + 6));
  CYC(b_+5, SYM(goron_bigBang_initLinkPosition)); goron_setLinkPositionAndDirection_hook(gb);
}

void goron_targetCarts_spawnPrize_hook(GB *gb) {
  BASE(goron_targetCarts_spawnPrize);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto alreadyGotBrisket; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+10); goto spawnPrize;

alreadyGotBrisket:
  CALL_C(b_+10, getRandomNumber_hook, SYM(getRandomNumber), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x0f);
  CYC(b_+15, b_+18); SET_HL(b_+64);
  CYC(b_+18, b_+19); goronTargetCarts_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto spawnPrize; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); A = 0x06;
  CALL_C(b_+26, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+29);
  CYC(b_+29, b_+31); A = 0x04;
  if (!(F & FC)) { CYCT(b_+31, b_+33); goto spawnPrize; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); A = 0x03;

spawnPrize:
  CYC(b_+35, b_+38); mem_wr(gb, wTmpcfc0_bigBangGame_prizeIndex, A);
  CYC(b_+38, b_+41); SET_HL(b_+80);
  CYC(b_+41, b_+42); goronTargetCarts_addDoubleIndexToHl_from_rst(gb, b_+42);
  CYC(b_+42, b_+43); B = mem_rd(gb, HL);
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+45); C = mem_rd(gb, HL);
  CALL_C(b_+45, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+48);
  if (!(F & FZ)) { CYCT(b_+48, b_+49); ret_effect(gb); return; }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x60);
  CYC(b_+51, b_+52); L = alu_inc8(gb, L);
  CYC(b_+52, b_+53); mem_wr(gb, HL, B);
  CYC(b_+53, b_+54); L = alu_inc8(gb, L);
  CYC(b_+54, b_+55); mem_wr(gb, HL, C);
  CYC(b_+55, b_+57); L = 0x4b;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x78);
  CYC(b_+59, b_+61); L = 0x4d;
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x78);
  CYC(b_+63, b_+64); ret_effect(gb);
}

void goron_targetCarts_deleteMinecartAndClearStaticObjects_hook(GB *gb) {
  BASE(goron_targetCarts_deleteMinecartAndClearStaticObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x16;
  CALL_C(b_+2, goron_findInteractionWithID_hook, SYM(goron_findInteractionWithID), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, DE);
  CYC(b_+6, b_+7); E = L;
  CYC(b_+7, b_+8); D = H;
  CALL_C(b_+8, objectDelete_de_hook, SYM(objectDelete_de), b_+11);
  CYC(b_+11, b_+12); SET_DE(pop_effect(gb));
  CYC(b_+12, SYM(goron_findInteractionWithID)); clearStaticObjects_hook(gb);
}

void goron_targetCarts_deleteCrystals_hook(GB *gb) {
  BASE(goron_targetCarts_deleteCrystals);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((w1ReservedInteraction0_var3f + 1));
loop:
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto nextEnemy; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+9, b_+11); alu_cp(gb, 0x63);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto nextEnemy; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); push_effect(gb, DE);
  CYC(b_+14, b_+15); push_effect(gb, HL);
  CYC(b_+15, b_+16); E = L;
  CYC(b_+16, b_+17); D = H;
  CALL_C(b_+17, objectDelete_de_hook, SYM(objectDelete_de), b_+20);
  CYC(b_+20, b_+23); SET_HL(wNumEnemies);
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+24, b_+25); SET_HL(pop_effect(gb));
  CYC(b_+25, b_+26); SET_DE(pop_effect(gb));

nextEnemy:
  CYC(b_+26, b_+27); H = alu_inc8(gb, H);
  CYC(b_+27, b_+28); A = H;
  CYC(b_+28, b_+30); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+30, b_+32); goto loop; }
  CYC(b_+30, b_+32);
  CYC(b_+32, SYM(goron_targetCarts_beginGame)); ret_effect(gb);
}

void goron_targetCarts_setPlayingFlag_hook(GB *gb);
void goron_targetCarts_clearPlayingFlag_hook(GB *gb);

void goron_targetCarts_beginGame_hook(GB *gb) {
  BASE(goron_targetCarts_beginGame);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0_goronDance_numFailedRounds, A);
  CYC(b_+4, b_+7); mem_wr(gb, wTmpcfc0_goronCutscenes_elderVar_cfdd, A);
  CYC(b_+7, b_+10); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CYC(b_+10, b_+13); mem_wr(gb, wTmpcfc0_goronDance_beat, A);
  CYC(b_+13, SYM(goron_targetCarts_endGame)); goron_targetCarts_setPlayingFlag_hook(gb);
}

void goron_targetCarts_endGame_hook(GB *gb) {
  BASE(goron_targetCarts_endGame);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, SYM(goron_targetCarts_setPlayingFlag)); goron_targetCarts_clearPlayingFlag_hook(gb);
}

void goron_targetCarts_setPlayingFlag_hook(GB *gb) {
  BASE(goron_targetCarts_setPlayingFlag);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+5, SYM(goron_targetCarts_clearPlayingFlag)); ret_effect(gb);
}

void goron_targetCarts_clearPlayingFlag_hook(GB *gb) {
  BASE(goron_targetCarts_clearPlayingFlag);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+5, SYM(goron_checkLinkNotInAir)); ret_effect(gb);
}

void goron_targetCarts_setupNumTargetsHitText_hook(GB *gb) {
  BASE(goron_targetCarts_setupNumTargetsHitText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+3, b_+5); alu_add(gb, 0x00);
  CYC(b_+5, b_+6); alu_daa(gb);
  CYC(b_+6, b_+9); SET_HL(wTextNumberSubstitution);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+11); SET_HL(HL + 1);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
  CYC(b_+13, SYM(goron_targetCarts_checkHitAllTargets)); ret_effect(gb);
}

void goron_targetCarts_checkHitAllTargets_hook(GB *gb) {
  BASE(goron_targetCarts_checkHitAllTargets);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0c);
  CYC(b_+5, SYM(goron_targetCarts_checkHit9OrMoreTargets)); writeFlagsTocddb_hook(gb);
}

void goron_targetCarts_checkHit9OrMoreTargets_hook(GB *gb) {
  BASE(goron_targetCarts_checkHit9OrMoreTargets);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+3, b_+5); alu_cp(gb, 0x09);
  CYC(b_+5, b_+6); alu_ccf(gb);
  CYC(b_+6, SYM(goron_targetCarts_configureInventory)); writeFlagsTocddb_hook(gb);
}

void goron_targetCarts_configureInventory_hook(GB *gb) {
  BASE(goron_targetCarts_configureInventory);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(wInventoryB);
  CYC(b_+3, b_+6); SET_HL(wTmpcfc0_bigBangGame_filler2);
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+11); A = mem_rd(gb, wInventoryA);
  CYC(b_+11, b_+13); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto equipToA; }
  CYC(b_+13, b_+15);

equipToB:
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, BC, A);
  CYC(b_+17, b_+18); C = alu_inc8(gb, C);
  CYC(b_+18, b_+19); A = mem_rd(gb, BC);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+22); A = 0x0f;
  CYC(b_+22, b_+23); mem_wr(gb, BC, A);
  CYC(b_+23, b_+25); goto setupSeedShooter;

equipToA:
  CYC(b_+25, b_+27); A = 0x0f;
  CYC(b_+27, b_+28); mem_wr(gb, BC, A);
  CYC(b_+28, b_+29); C = alu_inc8(gb, C);
  CYC(b_+29, b_+30); A = mem_rd(gb, BC);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CYC(b_+32, b_+33); mem_wr(gb, BC, A);

setupSeedShooter:
  CYC(b_+33, b_+35); C = 0xba;
  CYC(b_+35, b_+36); A = mem_rd(gb, BC);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+37, b_+39); A = 0x99;
  CYC(b_+39, b_+40); mem_wr(gb, BC, A);
  CYC(b_+40, b_+42); C = 0xc5;
  CYC(b_+42, b_+43); A = mem_rd(gb, BC);
  CYC(b_+43, b_+44); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+44, b_+46); A = 0x01;
  CYC(b_+46, b_+47); mem_wr(gb, BC, A);
  CYC(b_+47, b_+49); A = 0xff;
  CYC(b_+49, b_+52); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+52, SYM(goron_targetCarts_restoreInventory)); ret_effect(gb);
}

void goron_targetCarts_restoreInventory_hook(GB *gb) {
  BASE(goron_targetCarts_restoreInventory);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(wInventoryB);
  CYC(b_+3, b_+6); SET_HL(wTmpcfc0_bigBangGame_filler2);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, BC, A);
  CYC(b_+8, b_+9); C = alu_inc8(gb, C);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); mem_wr(gb, BC, A);
  CYC(b_+11, b_+13); C = 0xba;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, BC, A);
  CYC(b_+15, b_+17); C = 0xc5;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); mem_wr(gb, BC, A);
  CYC(b_+19, b_+21); A = 0xff;
  CYC(b_+21, b_+24); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+24, SYM(goron_targetCarts_loadCrystals)); ret_effect(gb);
}

void goron_targetCarts_loadCrystals_hook(GB *gb) {
  BASE(goron_targetCarts_loadCrystals);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(b_+5, b_+7); A = 0x00;
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_6848; }
  CYC(b_+7, b_+9);
  CALL_C(b_+9, getRandomNumber_hook, SYM(getRandomNumber), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x01);
  CYC(b_+14, b_+15); A = alu_inc8(gb, A);

L_6848:
  CYC(b_+15, b_+18); mem_wr(gb, wTmpcfc0_genericCutscene_cfd4, A);
  CYC(b_+18, b_+21); SET_HL((SYM(symmetryNpcSubid8And9Script__repeat_b15) + 4));
  CYC(b_+21, SYM(goron_targetCarts_reloadCrystalsInFirstRoom)); parseGivenObjectData_b00_hook(gb);
}

void goron_targetCarts_reloadCrystalsInFirstRoom_hook(GB *gb) {
  BASE(goron_targetCarts_reloadCrystalsInFirstRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);

loop:
  CYC(b_+1, b_+3); hram_wr(gb, 0x8b, A);
  CYC(b_+3, b_+6); SET_HL(wTmpcfc0_goronCutscenes_elderVar_cfdd);
  CALL_C(b_+6, checkFlag_hook, SYM(checkFlag), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto nextCrystal; }
  CYC(b_+9, b_+11);
  CALL_C(b_+11, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+14);
  CYC(b_+14, b_+16); A = hram_rd(gb, 0x8b);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x63);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);

nextCrystal:
  CYC(b_+20, b_+22); A = hram_rd(gb, 0x8b);
  CYC(b_+22, b_+23); A = alu_inc8(gb, A);
  CYC(b_+23, b_+25); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto loop; }
  CYC(b_+25, b_+27); ret_effect(gb);
}
