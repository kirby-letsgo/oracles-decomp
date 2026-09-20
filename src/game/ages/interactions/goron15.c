#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(goron_setLinkPositionAndDirection), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(goron_setLinkPositionAndDirection), (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

void goron_putLinkInState08_hook(GB *gb);
void goron_findInteractionWithID_hook(GB *gb);

static void goron_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void goron_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void goron_setLinkPositionAndDirection_hook(GB *gb) {
  BASE(goron_setLinkPositionAndDirection);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); L = 0x0b;
  CYC(b_+6, b_+7); mem_wr(gb, HL, B);
  CYC(b_+7, b_+9); L = 0x0d;
  CYC(b_+9, SYM(goron_putLinkInState08)); mem_wr(gb, HL, C);
  goron_putLinkInState08_hook(gb);
}

void goron_putLinkInState08_hook(GB *gb) {
  BASE(goron_putLinkInState08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, putLinkOnGround_hook, SYM(putLinkOnGround), b_+3);
  CYC(b_+3, SYM(goronDance_checkNumFailedRounds)); setLinkForceStateToState08_hook(gb);
}

static void goron_showText_differentForPresent_add20(GB *gb, uint16_t return_address) {
  BASE(goron_showText_differentForPresent);
  push_effect(gb, return_address);
  CYC(b_+14, b_+15); A = C;
  CYC(b_+15, b_+17); alu_add(gb, 0x20);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, SYM(goron_decideTextToShow_differentForLinkedInPast)); ret_effect(gb);
}

void goron_showText_differentForPresent_hook(GB *gb) {
  BASE(goron_showText_differentForPresent);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+4, b_+6); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+6, b_+9); goron_showText_differentForPresent_add20(gb, b_+9); }
  else CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); B = 0x24;
  CYC(b_+11, b_+14); showText_hook(gb);
}

void goron_decideTextToShow_differentForLinkedInPast_hook(GB *gb) {
  BASE(goron_decideTextToShow_differentForLinkedInPast);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CALL_C(b_+1, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+4);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto linked; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+9, b_+11); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+11, b_+13); goto showPresentText; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); goto showText;

showPresentText:
  CYC(b_+15, b_+16); A = C;
  CYC(b_+16, b_+18); alu_add(gb, 0x10);
  CYC(b_+18, b_+19); C = A;

showText:
  CYC(b_+19, b_+21); B = 0x24;
  CYC(b_+21, b_+24); showText_hook(gb); return;

linked:
  CYC(b_+24, b_+27); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+27, b_+29); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+29, b_+31); goto showPresentText; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); A = C;
  CYC(b_+32, b_+34); alu_add(gb, 0x20);
  CYC(b_+34, b_+35); C = A;
  CYC(b_+35, SYM(goron_showText_differentForPast)); goto showText;
}

static void goron_showText_differentForPast_add0c(GB *gb, uint16_t return_address) {
  BASE(goron_showText_differentForPast);
  push_effect(gb, return_address);
  CYC(b_+14, b_+15); A = C;
  CYC(b_+15, b_+17); alu_add(gb, 0x0c);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, SYM(goron_showTextForGoronWorriedAboutElder)); ret_effect(gb);
}

void goron_showText_differentForPast_hook(GB *gb) {
  BASE(goron_showText_differentForPast);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+4, b_+6); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+6, b_+9); goron_showText_differentForPast_add0c(gb, b_+9); }
  else CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); B = 0x24;
  CYC(b_+11, b_+14); showText_hook(gb);
}

void goron_showTextForGoronWorriedAboutElder_hook(GB *gb) {
  BASE(goron_showTextForGoronWorriedAboutElder);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x2f;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_63ee; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); C = 0x79;
  CYC(b_+9, b_+11); goto L_63f0;

L_63ee:
  CYC(b_+11, b_+13); C = 0x7a;
L_63f0:
  CYC(b_+13, b_+15); B = 0x24;
  CYC(b_+15, SYM(goron_showTextForSubid05)); showText_hook(gb);
}

void goron_showTextForSubid05_hook(GB *gb) {
  BASE(goron_showTextForSubid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x43;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto threeOrHigher; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x2f;
  CALL_C(b_+9, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+12);
  CYC(b_+12, b_+14); B = 0x00;
  if (F & FZ) { CYCT(b_+14, b_+16); goto L_6407; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); B = 0x01;

L_6407:
  CYC(b_+18, b_+20); E = 0x43;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_rlca(gb);
  CYC(b_+22, b_+24); goto showTextTail;

threeOrHigher:
  CYC(b_+24, b_+26); B = 0x03;

showTextTail:
  CYC(b_+26, b_+27); alu_add(gb, B);
  CYC(b_+27, b_+30); SET_HL(b_+37);
  CYC(b_+30, b_+31); goron_addAToHl_from_rst(gb, b_+31);
  CYC(b_+31, b_+33); B = 0x24;
  CYC(b_+33, b_+34); C = mem_rd(gb, HL);
  CYC(b_+34, b_+37); showText_hook(gb);
}

static void goron_determineTextForGenericNpc_getGameState(GB *gb, uint16_t return_address, uint16_t sp0_) {
  BASE(goron_determineTextForGenericNpc);
  push_effect(gb, return_address);
  CYC(b_+6, b_+9); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+9, b_+11); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+11, b_+13); goto inPresent; }
  CYC(b_+11, b_+13);

  CYC(b_+13, b_+15); A = 0x14;
  CALL_C(b_+15, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto val02; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); A = 0x2f;
  CALL_C(b_+22, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+25);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto val01; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); goto val00;

inPresent:
  CYC(b_+29, b_+31); A = 0x14;
  CALL_C(b_+31, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+34);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto val03; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); A = 0x1a;
  CALL_C(b_+38, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+41);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto val02; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); A = 0x03;
  CYC(b_+45, b_+48); SET_HL(wEssencesObtained);
  CALL_C(b_+48, checkFlag_hook, SYM(checkFlag), b_+51);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto val01; }
  CYC(b_+51, b_+53);

val00:
  CYC(b_+53, b_+54); alu_xor(gb, A);
  CYC(b_+54, b_+56); goto writeVal;

val01:
  CYC(b_+56, b_+58); A = 0x01;
  CYC(b_+58, b_+60); goto writeVal;

val02:
  CYC(b_+60, b_+62); A = 0x02;
  CYC(b_+62, b_+64); goto writeVal;

val03:
  CYC(b_+64, b_+66); A = 0x03;

writeVal:
  CYC(b_+66, b_+68); E = 0x7b;
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+70); ret_effect(gb);
}

void goron_determineTextForGenericNpc_hook(GB *gb) {
  BASE(goron_determineTextForGenericNpc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); goron_determineTextForGenericNpc_getGameState(gb, b_+3, sp0_);
  CYC(b_+3, b_+6);

  CYC(b_+70, b_+72); E = 0x42;
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+75); alu_sub(gb, 0x0c);
  CYC(b_+75, b_+78); SET_HL(b_+112);
  CYC(b_+78, b_+79); goron_addAToHl_from_rst(gb, b_+79);
  CYC(b_+79, b_+80); A = mem_rd(gb, HL);
  CYC(b_+80, b_+81); goron_addAToHl_from_rst(gb, b_+81);
  CYC(b_+81, b_+83); E = 0x43;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); alu_rlca(gb);
  CYC(b_+85, b_+86); goron_addDoubleIndexToHl_from_rst(gb, b_+86);
  CYC(b_+86, b_+88); E = 0x7b;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); goron_addAToHl_from_rst(gb, b_+90);
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+93); E = 0x72;
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, b_+95); B = A;
  CYC(b_+95, b_+96); E = alu_inc8(gb, E);
  CYC(b_+96, b_+98); A = 0x31;
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+100); A = B;
  CYC(b_+100, b_+102); alu_cp(gb, 0x27);
  if (!(F & FZ)) { CYCT(b_+102, b_+103); ret_effect(gb); return; }
  CYC(b_+102, b_+103);
  CALL_C(b_+103, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+106);
  if (!(F & FZ)) { CYCT(b_+106, b_+107); ret_effect(gb); return; }
  CYC(b_+106, b_+107);
  CYC(b_+107, b_+109); A = 0xff;
  CYC(b_+109, b_+110); E = alu_dec8(gb, E);
  CYC(b_+110, b_+111); mem_wr(gb, DE, A);
  CYC(b_+111, b_+112); ret_effect(gb);
}

void goron_checkShouldBeNapping_hook(GB *gb) {
  BASE(goron_checkShouldBeNapping);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC((SYM(refillSeedSatchel) + 12));
  CALL_C(b_+3, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+6);
  CALL_C(b_+6, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+9);
  CYC(b_+9, b_+10); alu_ccf(gb);
  CALL_C(b_+10, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), b_+13);
  CYC(b_+13, b_+16); SET_BC((SYM(loadUncompressedGfxHeader) + 44));
  CYC(b_+16, SYM(goron_faceDown)); objectSetCollideRadii_hook(gb);
}

void goron_faceDown_hook(GB *gb) {
  BASE(goron_faceDown);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x6b;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
  CYC(b_+5, b_+7); L = 0x49;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x10);
  CYC(b_+9, b_+11); L = 0x7f;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
  CYC(b_+13, b_+15); A = 0x02;
  CYC(b_+15, SYM(goron_setAnimation)); interactionSetAnimation_hook(gb);
}

void goron_setAnimation_hook(GB *gb) {
  BASE(goron_setAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7f;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x01);
  CYC(b_+5, SYM(goron_beginWalkingLeft)); interactionSetAnimation_hook(gb);
}

void goron_reverseWalkingDirection_hook(GB *gb) {
  BASE(goron_reverseWalkingDirection);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7c;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x80);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+10); L = 0x49;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_xor(gb, 0x10);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); L = 0x7e;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_xor(gb, 0x02);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, SYM(goron_refreshWalkingAnimation)); interactionSetAnimation_hook(gb);
}

void goron_refreshWalkingAnimation_hook(GB *gb) {
  BASE(goron_refreshWalkingAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x7e;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, SYM(goron_setSpeedToMoveDown)); interactionSetAnimation_hook(gb);
}

void goron_setSpeedToMoveDown_hook(GB *gb) {
  BASE(goron_setSpeedToMoveDown);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x50;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x28);
  CYC(b_+5, b_+7); L = 0x49;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x10);
  CYC(b_+9, b_+11); A = 0x02;
  CYC(b_+11, SYM(goron_cpLinkY)); goron_setAnimation_hook(gb);
}

static void goron_cpLinkY_writeResult(GB *gb) {
  BASE(goron_cpYTo60);
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0x4b;
  CYC(b_+5, b_+6); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+6, SYM(goron_checkReachedLinkHorizontally)); writeFlagsTocddb_hook(gb);
}

void goron_cpLinkY_hook(GB *gb) {
  BASE(goron_cpLinkY);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); SET_HL(w1Link_yh);
  CYC(b_+4, b_+5); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+5, SYM(goron_cpYTo60)); goron_cpLinkY_writeResult(gb);
}

void goron_cpYTo60_hook(GB *gb) {
  BASE(goron_cpYTo60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x60;
  goron_cpLinkY_writeResult(gb);
}

static void goron_checkReachedLinkHorizontally_writeResult(GB *gb) {
  BASE(goron_cpXTo48);
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0x4d;
  CYC(b_+5, b_+6); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+6, SYM(goron_checkLinkApproachedWithBombFlower)); writeFlagsTocddb_hook(gb);
}

void goron_checkReachedLinkHorizontally_hook(GB *gb) {
  BASE(goron_checkReachedLinkHorizontally);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xf2;
  CYC(b_+2, b_+5); SET_HL(w1Link_xh);
  CYC(b_+5, b_+6); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+6, SYM(goron_cpXTo48)); goron_checkReachedLinkHorizontally_writeResult(gb);
}

void goron_cpXTo48_hook(GB *gb) {
  BASE(goron_cpXTo48);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x48;
  goron_checkReachedLinkHorizontally_writeResult(gb);
}

void goron_checkLinkApproachedWithBombFlower_hook(GB *gb) {
  BASE(goron_checkLinkApproachedWithBombFlower);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x49;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  CALL_C(b_+5, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0x4b;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x88);
  CYC(b_+15, b_+16); push_effect(gb, AF);
  CYC(b_+16, b_+18); L = 0x4d;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x58);
  CYC(b_+21, b_+22); push_effect(gb, AF);
  CYC(b_+22, b_+25); SET_BC((SYM(getRandomRingOfGivenTier) + 40));
  CALL_C(b_+25, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+28);
  CALL_C(b_+28, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+31);
  CALL_C(b_+31, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), b_+34);
  CYC(b_+34, b_+37); SET_BC((SYM(loadUncompressedGfxHeader) + 44));
  CALL_C(b_+37, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+40);
  CYC(b_+40, b_+41); SET_AF(pop_effect(gb));
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = 0x4d;
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  CYC(b_+45, b_+46); SET_AF(pop_effect(gb));
  CYC(b_+46, b_+48); L = 0x4b;
  CYC(b_+48, b_+49); mem_wr(gb, HL, A);
  CYC(b_+49, SYM(goron_decMovementCounter)); ret_effect(gb);
}

void goron_decMovementCounter_hook(GB *gb) {
  BASE(goron_decMovementCounter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7c;
  CALL_C(b_+3, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+6);
  CYC(b_+6, SYM(goron_initCountersForBombFlowerExplosion)); writeFlagsTocddb_hook(gb);
}

void goron_initCountersForBombFlowerExplosion_hook(GB *gb) {
  BASE(goron_initCountersForBombFlowerExplosion);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7c;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x5a);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+10); L = 0x7e;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x01);
  CYC(b_+12, SYM(goron_countdownToPlayRockSoundAndShakeScreen)); ret_effect(gb);
}

void goron_countdownToPlayRockSoundAndShakeScreen_hook(GB *gb) {
  BASE(goron_countdownToPlayRockSoundAndShakeScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7e;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x05);
  CYC(b_+7, b_+9); A = 0xa5;
  CALL_C(b_+9, playSound_b00_hook, SYM(playSound_b00), b_+12);
  CYC(b_+12, b_+14); A = 0x04;
  CYC(b_+14, SYM(goron_createFallingRockSpawner)); setScreenShakeCounter_hook(gb);
}

void goron_createFallingRockSpawner_hook(GB *gb) {
  BASE(goron_createFallingRockSpawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x92;
  CYC(b_+2, SYM(goron_clearRockBarrier)); objectCreateInteractionWithSubid00_hook(gb);
}

static void goron_clearRockBarrier_clearRow_body(GB *gb, uint16_t sp0_) {
  BASE(goron_clearRockBarrier);
  CYC(b_+15, b_+17); A = 0x05;
L_nextTile:
  CYC(b_+17, b_+19); hram_wr(gb, 0x8b, A);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); push_effect(gb, BC);
  CYC(b_+21, b_+22); push_effect(gb, HL);
  CALL_C(b_+22, setTile_hook, SYM(setTile), b_+25);
  CYC(b_+25, b_+26); SET_HL(pop_effect(gb));
  CYC(b_+26, b_+27); SET_BC(pop_effect(gb));
  CYC(b_+27, b_+28); C = alu_inc8(gb, C);
  CYC(b_+28, b_+30); A = hram_rd(gb, 0x8b);
  CYC(b_+30, b_+31); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto L_nextTile; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+34); ret_effect(gb);
}

static void goron_clearRockBarrier_clearRow_from_call(GB *gb, uint16_t return_address, uint16_t sp0_) {
  push_effect(gb, return_address);
  goron_clearRockBarrier_clearRow_body(gb, sp0_);
}

void goron_clearRockBarrier_hook(GB *gb) {
  BASE(goron_clearRockBarrier);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+34);
  CYC(b_+3, b_+5); C = 0x31;
  CYC(b_+5, b_+8); goron_clearRockBarrier_clearRow_from_call(gb, b_+8, sp0_);
  CYC(b_+8, b_+10); C = 0x41;
  CYC(b_+10, b_+13); goron_clearRockBarrier_clearRow_from_call(gb, b_+13, sp0_);
  CYC(b_+13, b_+15); C = 0x51;
  goron_clearRockBarrier_clearRow_body(gb, sp0_);
}

static void goron_createRockDebris_body(GB *gb, uint16_t sp0_) {
  BASE(goron_createRockDebrisToRight);
  CALL_C(b_+3, getRandomNumber_hook, SYM(getRandomNumber), b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0x01);
  CYC(b_+8, b_+10); hram_wr(gb, 0x8d, A);
  CYC(b_+10, b_+11); alu_xor(gb, A);
L_nextRock:
  CYC(b_+11, b_+13); hram_wr(gb, 0x8b, A);
  CALL_C(b_+13, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto end; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x92);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x02);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x01);
  CYC(b_+26, b_+28); L = 0x46;
  CYC(b_+28, b_+30); A = hram_rd(gb, 0x8d);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+33); L = 0x49;
  CYC(b_+33, b_+35); A = hram_rd(gb, 0x8b);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CALL_C(b_+36, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+39);
  CYC(b_+39, b_+41); A = hram_rd(gb, 0x8b);
  CYC(b_+41, b_+42); A = alu_inc8(gb, A);
  CYC(b_+42, b_+44); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); goto L_nextRock; }
  CYC(b_+44, b_+46);
end:
  CYC(b_+46, b_+48); A = 0xa5;
  CYC(b_+48, SYM(goron_tryTakeEmberSeedsAndBombs)); playSound_b00_hook(gb);
}

void goron_createRockDebrisToLeft_hook(GB *gb) {
  BASE(goron_createRockDebrisToLeft);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xf6fa);
  CYC(b_+3, SYM(goron_createRockDebrisToRight)); goron_createRockDebris_body(gb, sp0_);
}

void goron_createRockDebrisToRight_hook(GB *gb) {
  BASE(goron_createRockDebrisToRight);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xf606);
  goron_createRockDebris_body(gb, sp0_);
}

void goron_tryTakeEmberSeedsAndBombs_hook(GB *gb) {
  BASE(goron_tryTakeEmberSeedsAndBombs);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x19;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto dontGiveItems; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x20;
  CALL_C(b_+9, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+12);
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto dontGiveItems; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(b_+16, b_+18); goto dontGiveItems; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); push_effect(gb, AF);
  CYC(b_+19, b_+21); A = 0x03;
  CALL_C(b_+21, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+24);
  if (!(F & FC)) { CYCT(b_+24, b_+26); goto popAndDontGiveItems; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(b_+28, b_+30); goto popAndDontGiveItems; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); alu_sub(gb, 0x20);
  CYC(b_+32, b_+33); alu_daa(gb);
  CYC(b_+33, b_+36); mem_wr(gb, wNumBombs, A);
  CYC(b_+36, b_+37); SET_AF(pop_effect(gb));
  CYC(b_+37, b_+39); alu_sub(gb, 0x20);
  CYC(b_+39, b_+40); alu_daa(gb);
  CYC(b_+40, b_+43); mem_wr(gb, wNumEmberSeeds, A);
  CALL_C(b_+43, setStatusBarNeedsRefreshBit1_hook, SYM(setStatusBarNeedsRefreshBit1), b_+46);
  CYC(b_+46, b_+47); alu_xor(gb, A);
  CYC(b_+47, b_+50); writeFlagsTocddb_hook(gb); return;

popAndDontGiveItems:
  CYC(b_+50, b_+51); SET_AF(pop_effect(gb));

dontGiveItems:
  CYC(b_+51, b_+52); alu_or(gb, D);
  CYC(b_+52, SYM(goron_checkEnoughTimePassed)); writeFlagsTocddb_hook(gb);
}

void goron_deleteTreasure_hook(GB *gb) {
  BASE(goron_deleteTreasure);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x60;
  CALL_C(b_+2, goron_findInteractionWithID_hook, SYM(goron_findInteractionWithID), b_+5);
  CYC(b_+5, b_+7); L = 0x44;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x04);
  CYC(b_+9, SYM(goron_targetCarts_deleteMinecartAndClearStaticObjects)); ret_effect(gb);
}

void goron_findInteractionWithID_hook(GB *gb) {
  BASE(goron_findInteractionWithID);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w4StatusBarTileMap);
loop:
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto next; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+9, b_+10); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto next; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+14); ret_effect(gb); return;

next:
  CYC(b_+14, b_+15); H = alu_inc8(gb, H);
  CYC(b_+15, b_+16); A = H;
  CYC(b_+16, b_+18); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+18, b_+20); goto loop; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); alu_or(gb, H);
  CYC(b_+21, SYM(goron_targetCarts_deleteCrystals)); ret_effect(gb);
}

void goron_checkLinkNotInAir_hook(GB *gb) {
  BASE(goron_checkLinkNotInAir);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkInAir);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  CYC(b_+5, SYM(goron_checkLinkInAir)); writeFlagsTocddb_hook(gb);
}

void goron_checkGracefulGoronQuestStatus_hook(GB *gb) {
  BASE(goron_checkGracefulGoronQuestStatus);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x5a;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto noLavaJuice; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x45;
  CALL_C(b_+9, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+12);
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto noMermaidKey; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+17); goto writeByte;

noLavaJuice:
  CYC(b_+17, b_+19); A = 0x02;
  CYC(b_+19, b_+21); goto writeByte;

noMermaidKey:
  CYC(b_+21, b_+23); A = 0x01;

writeByte:
  CYC(b_+23, b_+25); E = 0x7e;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, SYM(goron_showTextForClairvoyantGoron)); ret_effect(gb);
}

void goron_showTextForClairvoyantGoron_hook(GB *gb) {
  BASE(goron_showTextForClairvoyantGoron);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+5); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+5, b_+7); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto finishedRollingRidgeSidequest; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(b_+84);

nextTreasure:
  CYC(b_+12, b_+13); B = alu_inc8(gb, B);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(b_+14, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+17);
  if (F & FC) { CYCT(b_+17, b_+19); goto finishedRollingRidgeSidequest; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+22); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto nextTreasure; }
  CYC(b_+22, b_+24);

finishedRollingRidgeSidequest:
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+27); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto L_68b0; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0x5a;
  CALL_C(b_+31, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+34);
  if (!(F & FC)) { CYCT(b_+34, b_+36); goto showTipForItem; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); B = 0x09;
  CYC(b_+38, b_+40); goto showTipForItem;

L_68b0:
  CYC(b_+40, b_+42); alu_cp(gb, 0x05);
  if (F & FC) { CYCT(b_+42, b_+44); goto showTipForItem; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); push_effect(gb, BC);
  CYC(b_+45, b_+47); A = 0x03;
  CYC(b_+47, b_+49); B = 0x3e;
  CALL_C(b_+49, getRoomFlags_hook, SYM(getRoomFlags), b_+52);
  CYC(b_+52, b_+53); SET_BC(pop_effect(gb));
  CYC(b_+53, b_+55); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+55, b_+57); goto showTipForItem; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); B = 0x0a;

showTipForItem:
  CYC(b_+59, b_+62); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+62, b_+64); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+64, b_+66); goto present; }
  CYC(b_+64, b_+66);

  CYC(b_+66, b_+68); A = 0x43;
  CYC(b_+68, b_+69); alu_add(gb, B);
  CYC(b_+69, b_+71); B = 0x31;
  CYC(b_+71, b_+72); C = A;
  CYC(b_+72, b_+75); showText_hook(gb); return;

present:
  CYC(b_+75, b_+77); A = 0x4f;
  CYC(b_+77, b_+78); alu_add(gb, B);
  CYC(b_+78, b_+80); B = 0x31;
  CYC(b_+80, b_+81); C = A;
  CYC(b_+81, b_+84); showText_hook(gb);
}
