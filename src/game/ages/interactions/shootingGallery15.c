#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(shootingGallery_cpScore), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(shootingGallery_cpScore), (from), (to), true)

void shootingGallery_cpScore_hook(GB *gb);
void shootingGallery_equipSword_hook(GB *gb);
void shootingGallery_equipBiggoronSword_hook(GB *gb);
void shootingGallery_changeEquips_hook(GB *gb);
void shootingGallery_restoreEquips_hook(GB *gb);
void func_50e4_hook(GB *gb);
void createSparkle_hook(GB *gb);
void shootingGallery_removeAllTargets_b15_hook(GB *gb);
void shootingGallery_setEntranceTiles_hook(GB *gb);
void shootingGallery_checkLinkHasRupees_hook(GB *gb);
void writeFlagsTocddb_hook(GB *gb);
void giveRupees_hook(GB *gb);
void giveHealthRefill_hook(GB *gb);
void shootingGallery_giveOneHeart_hook(GB *gb);
void giveHealth_hook(GB *gb);
void giveRingAToLink_hook(GB *gb);
void shootingGallery_giveRandomRingToLink_hook(GB *gb);
void forceLinkDirection_hook(GB *gb);
void shootingGallery_initLinkPosition_hook(GB *gb);
void shootingGallery_initLinkPositionAfterGame_hook(GB *gb);
void shootingGallery_initLinkPositionAfterBiggoronGame_hook(GB *gb);
void setLinkToState08AndSetDirection_hook(GB *gb);
void setLinkToState08_hook(GB *gb);
void checkIsLinkedGameForScript_hook(GB *gb);
void shootingGallery_checkIsNotLinkedGame_hook(GB *gb);
void beginJump_hook(GB *gb);
void updateGravity_hook(GB *gb);
void addToccd4_hook(GB *gb);
void addTocfc0_hook(GB *gb);

static void shootingGallery_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void shootingGallery_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void shootingGallery_cpScoreImpl(GB *gb, uint16_t sp0_) {
  BASE(shootingGallery_cpScore);
  CYC(b_+6, b_+9); SET_HL(b_+29);
  CYC(b_+9, b_+10); shootingGallery_addDoubleIndexToHl_from_rst(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); B = mem_rd(gb, HL);
  CYC(b_+12, b_+13); C = A;
  CYC(b_+13, b_+16); SET_HL(wTextNumberSubstitution);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); H = mem_rd(gb, HL);
  CYC(b_+18, b_+19); L = A;
  CALL_C(b_+19, compareHlToBc_hook, SYM(compareHlToBc), b_+22);
  CYC(b_+22, b_+23); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+25);
    goto notEqual;
  }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); A = alu_inc8(gb, A);
  RET(b_+26);
  return;

notEqual:
  CYC(b_+27, b_+28); alu_xor(gb, A);
  RET(b_+28);
}

void shootingGallery_cpScore_hook(GB *gb) {
  BASE(shootingGallery_cpScore);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); push_effect(gb, b_+3); shootingGallery_cpScoreImpl(gb, sp0_);
  CYC(b_+3, b_+6); writeFlagsTocddb_hook(gb);
}

void shootingGallery_equipSword_hook(GB *gb) {
  BASE(shootingGallery_equipSword);
  CYC(b_+0, b_+3); SET_HL(hFF8A);
  CYC(b_+3, b_+6); A = mem_rd(gb, wInventoryA);
  CYC(b_+6, b_+8); alu_cp(gb, 0x05);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto equipOnB;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); alu_xor(gb, A);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+14); A = 0x05;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17);
  shootingGallery_changeEquips_hook(gb);
  return;

equipOnB:
  CYC(b_+17, b_+19); A = 0x05;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, SYM(shootingGallery_equipBiggoronSword));
  shootingGallery_changeEquips_hook(gb);
}

void shootingGallery_equipBiggoronSword_hook(GB *gb) {
  BASE(shootingGallery_equipBiggoronSword);
  CYC(b_+0, b_+3); SET_HL(hFF8A);
  CYC(b_+3, b_+5); A = 0x0c;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, SYM(shootingGallery_changeEquips)); mem_wr(gb, HL, A);
  shootingGallery_changeEquips_hook(gb);
}

void shootingGallery_changeEquips_hook(GB *gb) {
  BASE(shootingGallery_changeEquips);
  CYC(b_+0, b_+3); SET_BC(wInventoryB);
  CYC(b_+3, b_+6); SET_HL(wTmpcfc0_shootingGallery_savedBItem);
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+10); A = hram_rd(gb, 0x8a);
  CYC(b_+10, b_+11); mem_wr(gb, BC, A);
  CYC(b_+11, b_+12); C = alu_inc8(gb, C);
  CYC(b_+12, b_+13); A = mem_rd(gb, BC);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+16); A = hram_rd(gb, 0x8b);
  CYC(b_+16, b_+17); mem_wr(gb, BC, A);
  CYC(b_+17, b_+19); A = 0xff;
  CYC(b_+19, b_+22); mem_wr(gb, wStatusBarNeedsRefresh, A);
  RET(b_+22);
}

void shootingGallery_restoreEquips_hook(GB *gb) {
  BASE(shootingGallery_restoreEquips);
  CYC(b_+0, b_+3); SET_BC(wInventoryB);
  CYC(b_+3, b_+6); SET_HL(wTmpcfc0_shootingGallery_savedBItem);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, BC, A);
  CYC(b_+8, b_+9); C = alu_inc8(gb, C);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); mem_wr(gb, BC, A);
  CYC(b_+11, b_+13); A = 0xff;
  CYC(b_+13, b_+16); mem_wr(gb, wStatusBarNeedsRefresh, A);
  RET(b_+16);
}

void func_50e4_hook(GB *gb) {
  BASE(func_50e4);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, w1Link_xh);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); A = 0x6e;
  CYC(b_+10, SYM(createSparkle)); createEnergySwirlGoingIn_hook(gb);
}

void createSparkle_hook(GB *gb) {
  BASE(createSparkle);
  CYC(b_+0, b_+2); B = 0x84;
  CYC(b_+2, SYM(shootingGallery_removeAllTargets_b15)); objectCreateInteractionWithSubid00_hook(gb);
}

void shootingGallery_removeAllTargets_b15_hook(GB *gb) {
  BASE(shootingGallery_removeAllTargets_b15);
  CYC(b_+0, b_+3); SET_HL((SYM(ralphSubid0cScript__landed_b15) + 7));
  CYC(b_+3, b_+5); E = 0x08;
  CYC(b_+5, SYM(shootingGallery_setEntranceTiles)); interBankCall_hook(gb);
}

void shootingGallery_setEntranceTiles_hook(GB *gb) {
  BASE(shootingGallery_setEntranceTiles);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(b_+19);
  CYC(b_+3, b_+4); shootingGallery_addAToHl_from_rst(gb, b_+4);
  CYC(b_+4, b_+6); C = 0x74;

loop:
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); push_effect(gb, HL);
  CALL_C(b_+8, setTile_hook, SYM(setTile), b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+13); C = alu_inc8(gb, C);
  CYC(b_+13, b_+14); A = C;
  CYC(b_+14, b_+16); alu_cp(gb, 0x76);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto loop;
  }
  CYC(b_+16, b_+18);
  RET(b_+18);
}

void shootingGallery_checkLinkHasRupees_hook(GB *gb) {
  BASE(shootingGallery_checkLinkHasRupees);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRupeeValue_hook, SYM(cpRupeeValue), SYM(writeFlagsTocddb));
  writeFlagsTocddb_hook(gb);
}

void writeFlagsTocddb_hook(GB *gb) {
  BASE(writeFlagsTocddb);
  CYC(b_+0, b_+1); push_effect(gb, AF);
  CYC(b_+1, b_+2); SET_BC(pop_effect(gb));
  CYC(b_+2, b_+3); A = C;
  CYC(b_+3, b_+6); mem_wr(gb, wcddb, A);
  RET(b_+6);
}

void giveRupees_hook(GB *gb) {
  BASE(giveRupees);
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+3); A = 0x28;
  CYC(b_+3, SYM(giveHealthRefill)); giveTreasure_hook(gb);
}

static void shootingGallery_giveHealthFromC(GB *gb) {
  BASE(giveHealth);
  CYC(b_+1, b_+3); A = 0x29;
  CYC(b_+3, SYM(giveRingAToLink)); giveTreasure_hook(gb);
}

void giveHealthRefill_hook(GB *gb) {
  BASE(giveHealthRefill);
  CYC(b_+0, b_+2); C = 0x40;
  CYC(b_+2, SYM(shootingGallery_giveOneHeart));
  shootingGallery_giveHealthFromC(gb);
}

void shootingGallery_giveOneHeart_hook(GB *gb) {
  BASE(shootingGallery_giveOneHeart);
  CYC(b_+0, b_+2); C = 0x04;
  CYC(b_+2, SYM(giveHealth));
  shootingGallery_giveHealthFromC(gb);
}

void giveHealth_hook(GB *gb) {
  BASE(giveHealth);
  CYC(b_+0, b_+1); C = A;
  shootingGallery_giveHealthFromC(gb);
}

void giveRingAToLink_hook(GB *gb) {
  BASE(giveRingAToLink);
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); C = 0x00;
  CYC(b_+3, SYM(shootingGallery_giveRandomRingToLink)); giveRingToLink_hook(gb);
}

void shootingGallery_giveRandomRingToLink_hook(GB *gb) {
  BASE(shootingGallery_giveRandomRingToLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+8); SET_HL(b_+12);
  CYC(b_+8, b_+9); shootingGallery_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12);
  giveRingAToLink_hook(gb);
}

void forceLinkDirection_hook(GB *gb) {
  BASE(forceLinkDirection);
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, SYM(shootingGallery_initLinkPosition)); setLinkForceStateToState08_hook(gb);
}

static void shootingGallery_setLinkPositionTail(GB *gb) {
  BASE(shootingGallery_initLinkPositionAfterBiggoronGame);
  CYC(b_+5, b_+8); SET_HL(w1Link_yh);
  CYC(b_+8, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, b_+11); L = 0x0d;
  CYC(b_+11, SYM(setLinkToState08AndSetDirection)); mem_wr(gb, HL, C);
  setLinkToState08AndSetDirection_hook(gb);
}

void shootingGallery_initLinkPosition_hook(GB *gb) {
  BASE(shootingGallery_initLinkPosition);
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+5); SET_BC((SYM(hardhatWorkerSubid03Script_b15) + 2));
  CYC(b_+5, SYM(shootingGallery_initLinkPositionAfterGame));
  shootingGallery_setLinkPositionTail(gb);
}

void shootingGallery_initLinkPositionAfterGame_hook(GB *gb) {
  BASE(shootingGallery_initLinkPositionAfterGame);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); SET_BC((SYM(goron_targetCarts_reloadCrystalsInFirstRoom__nextCrystal) + 3));
  CYC(b_+5, SYM(shootingGallery_initLinkPositionAfterBiggoronGame));
  shootingGallery_setLinkPositionTail(gb);
}

void shootingGallery_initLinkPositionAfterBiggoronGame_hook(GB *gb) {
  BASE(shootingGallery_initLinkPositionAfterBiggoronGame);
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+5); SET_BC((SYM(goron_targetCarts_restoreInventory) + 24));
  shootingGallery_setLinkPositionTail(gb);
}

void setLinkToState08AndSetDirection_hook(GB *gb) {
  BASE(setLinkToState08AndSetDirection);
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, SYM(setLinkToState08)); mem_wr(gb, HL, A);
  setLinkToState08_hook(gb);
}

void setLinkToState08_hook(GB *gb) {
  BASE(setLinkToState08);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, putLinkOnGround_hook, SYM(putLinkOnGround), b_+3);
  CYC(b_+3, SYM(checkIsLinkedGameForScript)); setLinkForceStateToState08_hook(gb);
}

void checkIsLinkedGameForScript_hook(GB *gb) {
  BASE(checkIsLinkedGameForScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  CYC(b_+3, SYM(shootingGallery_checkIsNotLinkedGame)); writeFlagsTocddb_hook(gb);
}

void shootingGallery_checkIsNotLinkedGame_hook(GB *gb) {
  BASE(shootingGallery_checkIsNotLinkedGame);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  CALL_C(b_+3, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), b_+6);
  CYC(b_+6, b_+7); alu_cpl(gb);
  CYC(b_+7, b_+10); mem_wr(gb, wcddb, A);
  RET(b_+10);
}

void beginJump_hook(GB *gb) {
  BASE(beginJump);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
  CYC(b_+5, b_+6); SET_HL(HL + 1);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xfe);
  CYC(b_+8, b_+10); A = 0x53;
  CYC(b_+10, SYM(updateGravity)); playSound_b00_hook(gb);
}

void updateGravity_hook(GB *gb) {
  BASE(updateGravity);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x30;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, SYM(addToccd4)); writeFlagsTocddb_hook(gb);
}

void addToccd4_hook(GB *gb) {
  BASE(addTocfc0);
  CYC(SYM(addToccd4), (SYM(addToccd4) + 3)); SET_HL(wShootingGalleryHitTargets);
  CYC((SYM(addToccd4) + 3), b_+0);
  CYC(b_+3, b_+4); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  RET(b_+5);
}

void addTocfc0_hook(GB *gb) {
  BASE(addTocfc0);
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0);
  CYC(b_+3, b_+4); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  RET(b_+5);
}
