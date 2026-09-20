#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

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
  CYC(0x5074, 0x5077); SET_HL(0x508b);
  CYC(0x5077, 0x5078); shootingGallery_addDoubleIndexToHl_from_rst(gb, 0x5078);
  CYC(0x5078, 0x5079); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5079, 0x507a); B = mem_rd(gb, HL);
  CYC(0x507a, 0x507b); C = A;
  CYC(0x507b, 0x507e); SET_HL(wTextNumberSubstitution);
  CYC(0x507e, 0x507f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x507f, 0x5080); H = mem_rd(gb, HL);
  CYC(0x5080, 0x5081); L = A;
  CALL_C(0x5081, compareHlToBc_hook, 0x01d6, 0x5084);
  CYC(0x5084, 0x5085); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5085, 0x5087);
    goto notEqual;
  }
  CYC(0x5085, 0x5087);
  CYC(0x5087, 0x5088); A = alu_inc8(gb, A);
  RET(0x5088);
  return;

notEqual:
  CYC(0x5089, 0x508a); alu_xor(gb, A);
  RET(0x508a);
}

void shootingGallery_cpScore_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x506e, 0x5071); push_effect(gb, 0x5071); shootingGallery_cpScoreImpl(gb, sp0_);
  CYC(0x5071, 0x5074); writeFlagsTocddb_hook(gb);
}

void shootingGallery_equipSword_hook(GB *gb) {
  CYC(0x509d, 0x50a0); SET_HL(hFF8A);
  CYC(0x50a0, 0x50a3); A = mem_rd(gb, wInventoryA);
  CYC(0x50a3, 0x50a5); alu_cp(gb, 0x05);
  if (!(F & FZ)) {
    CYCT(0x50a5, 0x50a7);
    goto equipOnB;
  }
  CYC(0x50a5, 0x50a7);
  CYC(0x50a7, 0x50a8); alu_xor(gb, A);
  CYC(0x50a8, 0x50a9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x50a9, 0x50ab); A = 0x05;
  CYC(0x50ab, 0x50ac); mem_wr(gb, HL, A);
  CYC(0x50ac, 0x50ae);
  shootingGallery_changeEquips_hook(gb);
  return;

equipOnB:
  CYC(0x50ae, 0x50b0); A = 0x05;
  CYC(0x50b0, 0x50b1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x50b1, 0x50b2); alu_xor(gb, A);
  CYC(0x50b2, 0x50b3); mem_wr(gb, HL, A);
  CYC(0x50b3, 0x50b5);
  shootingGallery_changeEquips_hook(gb);
}

void shootingGallery_equipBiggoronSword_hook(GB *gb) {
  CYC(0x50b5, 0x50b8); SET_HL(hFF8A);
  CYC(0x50b8, 0x50ba); A = 0x0c;
  CYC(0x50ba, 0x50bb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x50bb, 0x50bc); mem_wr(gb, HL, A);
  shootingGallery_changeEquips_hook(gb);
}

void shootingGallery_changeEquips_hook(GB *gb) {
  CYC(0x50bc, 0x50bf); SET_BC(wInventoryB);
  CYC(0x50bf, 0x50c2); SET_HL(wTmpcfc0_shootingGallery_savedBItem);
  CYC(0x50c2, 0x50c3); A = mem_rd(gb, BC);
  CYC(0x50c3, 0x50c4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x50c4, 0x50c6); A = hram_rd(gb, 0x8a);
  CYC(0x50c6, 0x50c7); mem_wr(gb, BC, A);
  CYC(0x50c7, 0x50c8); C = alu_inc8(gb, C);
  CYC(0x50c8, 0x50c9); A = mem_rd(gb, BC);
  CYC(0x50c9, 0x50ca); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x50ca, 0x50cc); A = hram_rd(gb, 0x8b);
  CYC(0x50cc, 0x50cd); mem_wr(gb, BC, A);
  CYC(0x50cd, 0x50cf); A = 0xff;
  CYC(0x50cf, 0x50d2); mem_wr(gb, wStatusBarNeedsRefresh, A);
  RET(0x50d2);
}

void shootingGallery_restoreEquips_hook(GB *gb) {
  CYC(0x50d3, 0x50d6); SET_BC(wInventoryB);
  CYC(0x50d6, 0x50d9); SET_HL(wTmpcfc0_shootingGallery_savedBItem);
  CYC(0x50d9, 0x50da); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x50da, 0x50db); mem_wr(gb, BC, A);
  CYC(0x50db, 0x50dc); C = alu_inc8(gb, C);
  CYC(0x50dc, 0x50dd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x50dd, 0x50de); mem_wr(gb, BC, A);
  CYC(0x50de, 0x50e0); A = 0xff;
  CYC(0x50e0, 0x50e3); mem_wr(gb, wStatusBarNeedsRefresh, A);
  RET(0x50e3);
}

void func_50e4_hook(GB *gb) {
  CYC(0x50e4, 0x50e7); A = mem_rd(gb, w1Link_yh);
  CYC(0x50e7, 0x50e8); B = A;
  CYC(0x50e8, 0x50eb); A = mem_rd(gb, w1Link_xh);
  CYC(0x50eb, 0x50ec); C = A;
  CYC(0x50ec, 0x50ee); A = 0x6e;
  CYC(0x50ee, 0x50f1); createEnergySwirlGoingIn_hook(gb);
}

void createSparkle_hook(GB *gb) {
  CYC(0x50f1, 0x50f3); B = 0x84;
  CYC(0x50f3, 0x50f6); objectCreateInteractionWithSubid00_hook(gb);
}

void shootingGallery_removeAllTargets_b15_hook(GB *gb) {
  CYC(0x50f6, 0x50f9); SET_HL(0x57bd);
  CYC(0x50f9, 0x50fb); E = 0x08;
  CYC(0x50fb, 0x50fe); interBankCall_hook(gb);
}

void shootingGallery_setEntranceTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x50fe, 0x5101); SET_HL(0x5111);
  CYC(0x5101, 0x5102); shootingGallery_addAToHl_from_rst(gb, 0x5102);
  CYC(0x5102, 0x5104); C = 0x74;

loop:
  CYC(0x5104, 0x5105); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5105, 0x5106); push_effect(gb, HL);
  CALL_C(0x5106, setTile_hook, 0x3a9c, 0x5109);
  CYC(0x5109, 0x510a); SET_HL(pop_effect(gb));
  CYC(0x510a, 0x510b); C = alu_inc8(gb, C);
  CYC(0x510b, 0x510c); A = C;
  CYC(0x510c, 0x510e); alu_cp(gb, 0x76);
  if (!(F & FZ)) {
    CYCT(0x510e, 0x5110);
    goto loop;
  }
  CYC(0x510e, 0x5110);
  RET(0x5110);
}

void shootingGallery_checkLinkHasRupees_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5115, cpRupeeValue_hook, 0x1765, 0x5118);
  writeFlagsTocddb_hook(gb);
}

void writeFlagsTocddb_hook(GB *gb) {
  CYC(0x5118, 0x5119); push_effect(gb, AF);
  CYC(0x5119, 0x511a); SET_BC(pop_effect(gb));
  CYC(0x511a, 0x511b); A = C;
  CYC(0x511b, 0x511e); mem_wr(gb, wcddb, A);
  RET(0x511e);
}

void giveRupees_hook(GB *gb) {
  CYC(0x511f, 0x5120); C = A;
  CYC(0x5120, 0x5122); A = 0x28;
  CYC(0x5122, 0x5125); giveTreasure_hook(gb);
}

static void shootingGallery_giveHealthFromC(GB *gb) {
  CYC(0x512e, 0x5130); A = 0x29;
  CYC(0x5130, 0x5133); giveTreasure_hook(gb);
}

void giveHealthRefill_hook(GB *gb) {
  CYC(0x5125, 0x5127); C = 0x40;
  CYC(0x5127, 0x5129);
  shootingGallery_giveHealthFromC(gb);
}

void shootingGallery_giveOneHeart_hook(GB *gb) {
  CYC(0x5129, 0x512b); C = 0x04;
  CYC(0x512b, 0x512d);
  shootingGallery_giveHealthFromC(gb);
}

void giveHealth_hook(GB *gb) {
  CYC(0x512d, 0x512e); C = A;
  shootingGallery_giveHealthFromC(gb);
}

void giveRingAToLink_hook(GB *gb) {
  CYC(0x5133, 0x5134); B = A;
  CYC(0x5134, 0x5136); C = 0x00;
  CYC(0x5136, 0x5139); giveRingToLink_hook(gb);
}

void shootingGallery_giveRandomRingToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5139, getRandomNumber_hook, 0x043e, 0x513c);
  CYC(0x513c, 0x513e); alu_and(gb, 0x0f);
  CYC(0x513e, 0x5141); SET_HL(0x5145);
  CYC(0x5141, 0x5142); shootingGallery_addAToHl_from_rst(gb, 0x5142);
  CYC(0x5142, 0x5143); A = mem_rd(gb, HL);
  CYC(0x5143, 0x5145);
  giveRingAToLink_hook(gb);
}

void forceLinkDirection_hook(GB *gb) {
  CYC(0x5155, 0x5158); SET_HL(w1Link_direction);
  CYC(0x5158, 0x5159); mem_wr(gb, HL, A);
  CYC(0x5159, 0x515c); setLinkForceStateToState08_hook(gb);
}

static void shootingGallery_setLinkPositionTail(GB *gb) {
  CYC(0x516f, 0x5172); SET_HL(w1Link_yh);
  CYC(0x5172, 0x5173); mem_wr(gb, HL, B);
  CYC(0x5173, 0x5175); L = 0x0d;
  CYC(0x5175, 0x5176); mem_wr(gb, HL, C);
  setLinkToState08AndSetDirection_hook(gb);
}

void shootingGallery_initLinkPosition_hook(GB *gb) {
  CYC(0x515c, 0x515e); A = 0x00;
  CYC(0x515e, 0x5161); SET_BC(0x6050);
  CYC(0x5161, 0x5163);
  shootingGallery_setLinkPositionTail(gb);
}

void shootingGallery_initLinkPositionAfterGame_hook(GB *gb) {
  CYC(0x5163, 0x5165); A = 0x01;
  CYC(0x5165, 0x5168); SET_BC(0x6868);
  CYC(0x5168, 0x516a);
  shootingGallery_setLinkPositionTail(gb);
}

void shootingGallery_initLinkPositionAfterBiggoronGame_hook(GB *gb) {
  CYC(0x516a, 0x516c); A = 0x03;
  CYC(0x516c, 0x516f); SET_BC(0x6838);
  shootingGallery_setLinkPositionTail(gb);
}

void setLinkToState08AndSetDirection_hook(GB *gb) {
  CYC(0x5176, 0x5179); SET_HL(w1Link_direction);
  CYC(0x5179, 0x517a); mem_wr(gb, HL, A);
  setLinkToState08_hook(gb);
}

void setLinkToState08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x517a, putLinkOnGround_hook, 0x2a8c, 0x517d);
  CYC(0x517d, 0x5180); setLinkForceStateToState08_hook(gb);
}

void checkIsLinkedGameForScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5180, checkIsLinkedGame_hook, 0x1992, 0x5183);
  CYC(0x5183, 0x5186); writeFlagsTocddb_hook(gb);
}

void shootingGallery_checkIsNotLinkedGame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5186, checkIsLinkedGame_hook, 0x1992, 0x5189);
  CALL_C(0x5189, writeFlagsTocddb_hook, 0x5118, 0x518c);
  CYC(0x518c, 0x518d); alu_cpl(gb);
  CYC(0x518d, 0x5190); mem_wr(gb, wcddb, A);
  RET(0x5190);
}

void beginJump_hook(GB *gb) {
  CYC(0x5191, 0x5192); H = D;
  CYC(0x5192, 0x5194); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x5194, 0x5196); mem_wr(gb, HL, 0x00);
  CYC(0x5196, 0x5197); SET_HL(HL + 1);
  CYC(0x5197, 0x5199); mem_wr(gb, HL, 0xfe);
  CYC(0x5199, 0x519b); A = 0x53;
  CYC(0x519b, 0x519e); playSound_b00_hook(gb);
}

void updateGravity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x519e, 0x51a0); C = 0x30;
  CALL_C(0x51a0, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x51a3);
  CYC(0x51a3, 0x51a6); writeFlagsTocddb_hook(gb);
}

void addToccd4_hook(GB *gb) {
  CYC(0x51a6, 0x51a9); SET_HL(wShootingGalleryHitTargets);
  CYC(0x51a9, 0x51ab);
  CYC(0x51ae, 0x51af); alu_add(gb, mem_rd(gb, HL));
  CYC(0x51af, 0x51b0); mem_wr(gb, HL, A);
  RET(0x51b0);
}

void addTocfc0_hook(GB *gb) {
  CYC(0x51ab, 0x51ae); SET_HL(wTmpcfc0);
  CYC(0x51ae, 0x51af); alu_add(gb, mem_rd(gb, HL));
  CYC(0x51af, 0x51b0); mem_wr(gb, HL, A);
  RET(0x51b0);
}
