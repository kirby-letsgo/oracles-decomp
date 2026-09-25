#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define mapleShadowPathsTable_bank05 SYM(mapleShadowPathsTable)
#define mapleMovementPatternIndices_bank05 SYM(mapleMovementPatternIndices)
#define mapleMovementPatternTable_bank05 SYM(mapleMovementPatternTable)
#define mapleKnockbackSpeeds_bank05 SYM(mapleCollideWithLink__speeds)
#define mapleWallOffsets_bank05 SYM(mapleState3__offsets)
#define mapleNormalEncounterText_bank05 SYM(mapleState5__normalEncounterText)
#define mapleCollisionRadii_bank05 SYM(mapleState5__collisionRadii)
#define mapleItemBitmasks_bank05 SYM(mapleSpawnItemDrops__itemBitmasks)
#define mapleItemDropDistributionTable_bank05 SYM(maple_itemDropDistributionTable)
#define mapleLinkItemDropDistribution_bank05 SYM(maple_linkItemDropDistribution)
#define mapleItemDropTreasureIndices_bank05 SYM(mapleItemDropTreasureIndices)
#define mapleItemValues_bank05 SYM(mapleItemValues)
#define mapleState9TextIndices_bank05 SYM(mapleState9__textIndices)
#define mapleItemIDs_bank05 SYM(mapleDecideItemToCollect__itemIDs)

void specialObjectCode_maple_hook(GB *gb);
void mapleState0_hook(GB *gb);
void mapleState1_hook(GB *gb);
void mapleState2_hook(GB *gb);
void mapleState3_hook(GB *gb);
void mapleState4_hook(GB *gb);
void mapleState5_hook(GB *gb);
void mapleState6_hook(GB *gb);
void mapleState7_hook(GB *gb);
void mapleStateA_hook(GB *gb);
void mapleState8_hook(GB *gb);
void mapleState9_hook(GB *gb);
void mapleEndEncounter_hook(GB *gb);
void mapleDeleteSelf_hook(GB *gb);
void mapleStateB_hook(GB *gb);
void mapleStateC_hook(GB *gb);
void mapleDecideNextAngle_hook(GB *gb);
void mapleCollideWithLink_hook(GB *gb);
void mapleUpdateOscillation_hook(GB *gb);
void mapleUpdateAngle_hook(GB *gb);
void mapleDecideAnimation_hook(GB *gb);
void mapleKeepInBounds_hook(GB *gb);
void mapleSpawnItemDrops_hook(GB *gb);
void mapleSpawnItemDrop_hook(GB *gb);
void mapleCheckLinkCanDropItem_hook(GB *gb);
void mapleInitZPositionAndSpeed_hook(GB *gb);
void mapleGetCardinalAngleTowardLink_hook(GB *gb);
void mapleFindUnexplodedBomb_hook(GB *gb);
void mapleFindNextUnexplodedBomb_hook(GB *gb);
void mapleSpawnItemDrop_variant_hook(GB *gb);
void mapleDecideItemToCollect_hook(GB *gb);
void mapleDecideItemToCollectAndUpdateTargetAngle_hook(GB *gb);
void mapleSetTargetDirectionToRelatedObj2_hook(GB *gb);
void mapleFunc_6c27_hook(GB *gb);
void mapleIncrementMeetingCounter_hook(GB *gb);

static uint16_t maple_jump_table(GB *gb) {
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

static void maple_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void maple_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void specialObjectCode_maple_hook(GB *gb) {
  BASE(specialObjectCode_maple);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionRetIfInactiveWithoutStateCheck_hook, SYM(companionRetIfInactiveWithoutStateCheck), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (maple_jump_table(gb));
    if (jt_ == SYM(mapleState0) && hook_is(gb, SYM(mapleState0), mapleState0_hook)) { mapleState0_hook(gb); return; }
    else if (jt_ == SYM(mapleState1) && hook_is(gb, SYM(mapleState1), mapleState1_hook)) { mapleState1_hook(gb); return; }
    else if (jt_ == SYM(mapleState2) && hook_is(gb, SYM(mapleState2), mapleState2_hook)) { mapleState2_hook(gb); return; }
    else if (jt_ == SYM(mapleState3) && hook_is(gb, SYM(mapleState3), mapleState3_hook)) { mapleState3_hook(gb); return; }
    else if (jt_ == SYM(mapleState4) && hook_is(gb, SYM(mapleState4), mapleState4_hook)) { mapleState4_hook(gb); return; }
    else if (jt_ == SYM(mapleState5) && hook_is(gb, SYM(mapleState5), mapleState5_hook)) { mapleState5_hook(gb); return; }
    else if (jt_ == SYM(mapleState6) && hook_is(gb, SYM(mapleState6), mapleState6_hook)) { mapleState6_hook(gb); return; }
    else if (jt_ == SYM(mapleState7) && hook_is(gb, SYM(mapleState7), mapleState7_hook)) { mapleState7_hook(gb); return; }
    else if (jt_ == SYM(mapleState8) && hook_is(gb, SYM(mapleState8), mapleState8_hook)) { mapleState8_hook(gb); return; }
    else if (jt_ == SYM(mapleState9) && hook_is(gb, SYM(mapleState9), mapleState9_hook)) { mapleState9_hook(gb); return; }
    else if (jt_ == SYM(mapleStateA) && hook_is(gb, SYM(mapleStateA), mapleStateA_hook)) { mapleStateA_hook(gb); return; }
    else if (jt_ == SYM(mapleStateB) && hook_is(gb, SYM(mapleStateB), mapleStateB_hook)) { mapleStateB_hook(gb); return; }
    else if (jt_ == SYM(mapleStateC) && hook_is(gb, SYM(mapleStateC), mapleStateC_hook)) { mapleStateC_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void mapleState0_hook(GB *gb) {
  BASE(mapleState0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wcc85) = A;
  CALL_C(b_+4, specialObjectSetOamVariables_hook, SYM(specialObjectSetOamVariables), b_+7);
  CYC(b_+7, b_+9); C = 0x02;
  CYC(b_+9, b_+12); A = W8(wMapleState);
  CYC(b_+12, b_+14); alu_and(gb, 0x0f);
  CYC(b_+14, b_+16); alu_cp(gb, 0x0f);
  if (F & FZ) {
    CYCT(b_+16, b_+18);
    goto variation_decided;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); C = alu_dec8(gb, C);
  CYC(b_+19, b_+21); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(b_+21, b_+23);
    goto variation_decided;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); C = alu_dec8(gb, C);

variation_decided:
  CYC(b_+24, b_+25); A = C;
  CYC(b_+25, b_+27); E = 0x3f;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+29, b_+31);
  } else {
    CYC(b_+29, b_+31);
    CYC(b_+31, b_+33); A = 0x01;
  }
  CYC(b_+33, b_+35); E = 0x28;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+37, b_+39);
    goto vehicle_decided;
  }
  CYC(b_+37, b_+39);
  CALL_C(b_+39, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+42);
  if (F & FZ) {
    CYCT(b_+42, b_+44);
    goto vehicle_decided;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); A = 0x02;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);

vehicle_decided:
  CALL_C(b_+47, itemIncState_hook, SYM(itemIncState), b_+50);
  CYC(b_+50, b_+52); L = 0x0b;
  CYC(b_+52, b_+54); A = 0x10;
  CYC(b_+54, b_+55); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0xb8);
  CYC(b_+58, b_+60); L = 0x0f;
  CYC(b_+60, b_+62); A = 0x88;
  CYC(b_+62, b_+63); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x32);
  CYC(b_+65, b_+67); L = 0x26;
  CYC(b_+67, b_+69); A = 0x08;
  CYC(b_+69, b_+70); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+70, b_+71); mem_wr(gb, HL, A);
  CYC(b_+71, b_+73); L = 0x2d;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x03);
  CALL_C(b_+75, getRandomNumber_hook, SYM(getRandomNumber), b_+78);
  CYC(b_+78, b_+80); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+80, b_+82);
  } else {
    CYC(b_+80, b_+82);
    CYC(b_+82, b_+84); A = 0x01;
  }
  CYC(b_+84, b_+86); E = 0x03;
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CYC(b_+87, b_+90); SET_HL(mapleShadowPathsTable_bank05);
  CYC(b_+90, b_+91); maple_add_double_index(gb, b_+91);
  CYC(b_+91, b_+92); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+92, b_+93); H = mem_rd(gb, HL);
  CYC(b_+93, b_+94); L = A;
  CYC(b_+94, b_+96); E = 0x3a;
  CYC(b_+96, b_+97); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+97, b_+98); mem_wr(gb, DE, A);
  CYC(b_+98, b_+99); E = alu_inc8(gb, E);
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+102); E = 0x18;
  CYC(b_+102, b_+103); A = L;
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CYC(b_+104, b_+105); E = alu_inc8(gb, E);
  CYC(b_+105, b_+106); A = H;
  CYC(b_+106, b_+107); mem_wr(gb, DE, A);
  CYC(b_+107, b_+108); A = mem_rd(gb, HL);
  CYC(b_+108, b_+110); E = 0x09;
  CYC(b_+110, b_+111); mem_wr(gb, DE, A);
  CALL_C(b_+111, mapleDecideNextAngle_hook, SYM(mapleDecideNextAngle), b_+114);
  CALL_C(b_+114, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+117);
  CYC(b_+117, b_+119); A = 0x19;
  CYC(b_+119, b_+122); TAIL(specialObjectSetAnimation);
}

void mapleState1_hook(GB *gb) {
  BASE(mapleState1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, mapleState4_hook, SYM(mapleState4), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = W8(wMenuDisabled);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+11); TAIL(mapleDeleteSelf);
  }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); A = 0x2b;
  CYC(b_+13, b_+16); W8(wActiveMusic) = A;
  CYC(b_+16, b_+19); TAIL(playSound_b00);
}

void mapleState4_hook(GB *gb) {
  BASE(mapleState4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(w1Companion_knockbackCounter);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, itemIncState_hook, SYM(itemIncState), b_+8);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void mapleState2_hook(GB *gb) {
  BASE(mapleState2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto animate;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); SET_HL(w1Companion_counter2);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+11, b_+13);
  } else {
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+14, b_+15); ret_effect(gb); return;
  }
  CYC(b_+15, b_+17); L = 0x3d;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); L = 0x09;
  CYC(b_+20, b_+21); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+21, b_+23);
    goto at_target_angle;
  }
  CYC(b_+21, b_+23);
  CALL_C(b_+23, mapleUpdateAngle_hook, SYM(mapleUpdateAngle), b_+26);
  CYC(b_+26, b_+28);
  goto move;

at_target_angle:
  CYC(b_+28, b_+30); L = 0x06;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CALL_C_CC(b_+31, mapleDecideNextAngle_hook, SYM(mapleDecideNextAngle), b_+34);
  } else {
    CYC(b_+31, b_+34);
  }
  if (F & FZ) {
    CYCT(b_+34, b_+36);
    goto choose_path;
  }
  CYC(b_+34, b_+36);

move:
  CALL_C(b_+36, objectApplySpeed_hook, SYM(objectApplySpeed), b_+39);
  CYC(b_+39, b_+41); E = 0x3e;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+43, b_+44); ret_effect(gb); return;
  }
  CYC(b_+43, b_+44);
  CALL_C(b_+44, checkLinkVulnerableAndIDZero_hook, GV(SYM(checkLinkVulnerableAndIDZero), 0x1cde), b_+47);
  if (!(F & FC)) {
    CYCT(b_+47, b_+49);
    goto animate;
  }
  CYC(b_+47, b_+49);
  CALL_C(b_+49, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+52);
  if (F & FC) {
    CYCT(b_+52, b_+54); TAIL(mapleCollideWithLink);
  }
  CYC(b_+52, b_+54);

animate:
  CALL_C(b_+54, mapleUpdateOscillation_hook, SYM(mapleUpdateOscillation), b_+57);
  CYC(b_+57, b_+60); TAIL(specialObjectAnimate);

choose_path:
  CYC(b_+60, b_+63); SET_HL(w1Companion_var3e);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+65, b_+68); TAIL(mapleDeleteSelf);
  }
  CYC(b_+65, b_+68);
  CYC(b_+68, b_+69); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+69, mapleInitZPositionAndSpeed_hook, SYM(mapleInitZPositionAndSpeed), b_+72);
  CYC(b_+72, b_+74); L = 0x10;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x50);
  CYC(b_+76, b_+78); L = 0x07;
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x3c);
  CYC(b_+80, b_+82); E = 0x3f;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+85); E = 0x03;
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+86, b_+88);
    goto variation_mask_ready;
  }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+90); E = (uint8_t)(E | 0x04);
  CYC(b_+90, b_+92); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+92, b_+94);
    goto variation_mask_ready;
  }
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+96); E = (uint8_t)(E | 0x08);

variation_mask_ready:
  CALL_C(b_+96, getRandomNumber_hook, SYM(getRandomNumber), b_+99);
  CYC(b_+99, b_+100); alu_and(gb, E);
  CYC(b_+100, b_+103); SET_HL(mapleMovementPatternIndices_bank05);
  CYC(b_+103, b_+104); maple_add_a_to_hl(gb, b_+104);
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+108); SET_HL(mapleMovementPatternTable_bank05);
  CYC(b_+108, b_+109); maple_add_double_index(gb, b_+109);
  CYC(b_+109, b_+111); E = 0x0b;
  CYC(b_+111, b_+112); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+112, b_+113); H = mem_rd(gb, HL);
  CYC(b_+113, b_+114); L = A;
  CYC(b_+114, b_+115); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+115, b_+116); mem_wr(gb, DE, A);
  CYC(b_+116, b_+118); E = 0x0d;
  CYC(b_+118, b_+119); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+119, b_+120); mem_wr(gb, DE, A);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+121, b_+123); E = 0x3a;
  CYC(b_+123, b_+124); mem_wr(gb, DE, A);
  CYC(b_+124, b_+125); E = alu_inc8(gb, E);
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+127); A = mem_rd(gb, HL);
  CYC(b_+127, b_+129); E = 0x09;
  CYC(b_+129, b_+130); mem_wr(gb, DE, A);
  CYC(b_+130, b_+132); E = 0x18;
  CYC(b_+132, b_+133); A = L;
  CYC(b_+133, b_+134); mem_wr(gb, DE, A);
  CYC(b_+134, b_+135); E = alu_inc8(gb, E);
  CYC(b_+135, b_+136); A = H;
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  TAIL(mapleDecideNextAngle);
}

void mapleDecideNextAngle_hook(GB *gb) {
  BASE(mapleDecideNextAngle);
  CYC(b_+0, b_+3); SET_HL(w1Companion_relatedObj2);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); H = mem_rd(gb, HL);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); E = 0x3d;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+13); E = 0x06;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = 0x18;
  CYC(b_+17, b_+18); A = L;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = H;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); A = C;
  CYC(b_+23, b_+25); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+25, b_+26); ret_effect(gb); return;
  }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+29); TAIL(mapleDecideAnimation);
}

void mapleCollideWithLink_hook(GB *gb) {
  BASE(mapleCollideWithLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+3);
  CALL_C(b_+3, mapleSpawnItemDrops_hook, SYM(mapleSpawnItemDrops), b_+6);
  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+11); W8(wDisableScreenTransitions) = A;
  CYC(b_+11, b_+14); W8(wMenuDisabled) = A;
  CYC(b_+14, b_+16); A = 0x3c;
  CYC(b_+16, b_+19); W8(wInstrumentsDisabledCounter) = A;
  CYC(b_+19, b_+21); E = 0x06;
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, mapleGetCardinalAngleTowardLink_hook, SYM(mapleGetCardinalAngleTowardLink), b_+26);
  CYC(b_+26, b_+27); B = A;
  CYC(b_+27, b_+30); SET_HL(w1Link_knockbackCounter);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x18);
  CYC(b_+32, b_+34); E = 0x09;
  CYC(b_+34, b_+36); L = (uint8_t)w1Link_knockbackAngle;
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  CYC(b_+37, b_+39); alu_xor(gb, 0x10);
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+42); E = 0x28;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+46); SET_HL(mapleKnockbackSpeeds_bank05);
  CYC(b_+46, b_+47); maple_add_a_to_hl(gb, b_+47);
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+50); E = 0x10;
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+53); E = 0x3a;
  CYC(b_+53, b_+55); A = 0xfc;
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CYC(b_+56, b_+58); A = 0x0f;
  CYC(b_+58, b_+61); W8(wScreenShakeCounterX) = A;
  CYC(b_+61, b_+63); E = 0x04;
  CYC(b_+63, b_+65); A = 0x03;
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+67); A = B;
  CYC(b_+67, b_+69); alu_add(gb, 0x04);
  CYC(b_+69, b_+70); alu_add(gb, A);
  CYC(b_+70, b_+71); alu_add(gb, A);
  CYC(b_+71, b_+73); A = alu_swap(gb, A);
  CYC(b_+73, b_+75); alu_and(gb, 0x01);
  CYC(b_+75, b_+77); alu_xor(gb, 0x01);
  CYC(b_+77, b_+79); alu_add(gb, 0x10);
  CYC(b_+79, b_+80); B = A;
  CYC(b_+80, b_+82); E = 0x28;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+84); alu_add(gb, A);
  CYC(b_+84, b_+85); alu_add(gb, B);
  CALL_C(b_+85, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+88);
  CYC(b_+88, b_+90); A = 0x85;
  CYC(b_+90, b_+93); TAIL(playSound_b00);
}

void mapleState3_hook(GB *gb) {
  BASE(mapleState3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(w1Link_knockbackCounter);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
  } else {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+8); A = 0x01;
    CYC(b_+8, b_+11); W8(wDisabledObjects) = A;
  }
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); E = 0x3a;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+16, b_+18);
    goto animate;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); E = 0x0f;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+24);
    goto apply_knockback;
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); E = 0x3a;
  CYC(b_+26, b_+28); L = 0x15;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);

apply_knockback:
  CYC(b_+32, b_+34); C = 0x40;
  CALL_C(b_+34, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+37);
  CALL_C(b_+37, objectApplySpeed_hook, SYM(objectApplySpeed), b_+40);
  CALL_C(b_+40, mapleKeepInBounds_hook, SYM(mapleKeepInBounds), b_+43);
  CALL_C(b_+43, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+46);
  if (F & FZ) {
    CYCT(b_+46, b_+47); ret_effect(gb); return;
  }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49);
  goto counteract_wall_speed;

animate:
  CYC(b_+49, b_+52); A = W8(wDisabledObjects);
  CYC(b_+52, b_+53); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+53, b_+54); ret_effect(gb); return;
  }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); E = 0x21;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+59); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+59, b_+62); TAIL(specialObjectAnimate);
  }
  CYC(b_+59, b_+62);
  CYC(b_+62, b_+64); E = 0x2d;
  CYC(b_+64, b_+66); A = 0x78;
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+69); E = 0x04;
  CYC(b_+69, b_+71); A = 0x04;
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+73); ret_effect(gb); return;

counteract_wall_speed:
  CYC(b_+73, b_+75); E = 0x09;
  CALL_C(b_+75, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+78);
  CYC(b_+78, b_+81); SET_HL(mapleWallOffsets_bank05);
  CYC(b_+81, b_+82); maple_add_double_index(gb, b_+82);
  CYC(b_+82, b_+84); E = 0x0b;
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+86, b_+87); B = A;
  CYC(b_+87, b_+88); SET_HL(HL + 1);
  CYC(b_+88, b_+90); E = 0x0d;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+92, b_+93); C = A;
  CYC(b_+93, b_+94); H = D;
  CYC(b_+94, b_+96); L = 0x0b;
  CYC(b_+96, b_+97); mem_wr(gb, HL, B);
  CYC(b_+97, b_+99); L = 0x0d;
  CYC(b_+99, b_+100); mem_wr(gb, HL, C);
  CYC(b_+100, b_+101); ret_effect(gb);
}

void mapleState5_hook(GB *gb) {
  BASE(mapleState5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(3)); SET_HL(w1Companion_counter1);
  CYC(b_+O(3), b_+OE(4)); A = mem_rd(gb, HL);
  CYC(b_+O(4), b_+OE(5)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(5), b_+OE(7));
    goto float_up;
  }
  CYC(b_+O(5), b_+OE(7));
  CYC(b_+O(7), b_+OE(8)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+O(8), mapleInitZPositionAndSpeed_hook, SYM(mapleInitZPositionAndSpeed), b_+OE(11));
  CYC(b_+O(11), b_+OE(13)); L = 0x0f;
  CYC(b_+O(13), b_+OE(15)); mem_wr(gb, HL, 0xff);
  CYC(b_+O(15), b_+OE(17)); A = 0x01;
  CYC(b_+O(17), b_+OE(19)); L = 0x3a;
  CYC(b_+O(19), b_+OE(20)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(20), b_+OE(21)); mem_wr(gb, HL, A);
  CYC(b_+O(21), b_+OE(23)); E = 0x09;
  CYC(b_+O(23), b_+OE(24)); A = mem_rd(gb, DE);
  CYC(b_+O(24), b_+OE(26)); alu_xor(gb, 0x10);
  CYC(b_+O(26), b_+OE(27)); mem_wr(gb, DE, A);
  CALL_C(b_+O(27), mapleDecideAnimation_hook, SYM(mapleDecideAnimation), b_+OE(30));

float_up:
  CYC(b_+O(30), b_+OE(32)); E = 0x28;
  CYC(b_+O(32), b_+OE(33)); A = mem_rd(gb, DE);
  CYC(b_+O(33), b_+OE(34)); C = A;
  CYC(b_+O(34), b_+OE(36)); E = 0x0f;
  CYC(b_+O(36), b_+OE(37)); A = mem_rd(gb, DE);
  CYC(b_+O(37), b_+OE(38)); A = alu_dec8(gb, A);
  CYC(b_+O(38), b_+OE(39)); mem_wr(gb, DE, A);
  CYC(b_+O(39), b_+OE(41)); alu_cp(gb, 0xf9);
  if (!(F & FC)) {
    CYCT(b_+O(41), b_+OE(42)); ret_effect(gb); return;
  }
  CYC(b_+O(41), b_+OE(42));
  CYC(b_+O(42), b_+OE(43)); A = C;
  CYC(b_+O(43), b_+OE(44)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(44), b_+OE(46));
    goto finished_floating_up;
  }
  CYC(b_+O(44), b_+OE(46));
  CYC(b_+O(46), b_+OE(47)); A = mem_rd(gb, DE);
  CYC(b_+O(47), b_+OE(49)); alu_cp(gb, 0xe9);
  if (!(F & FC)) {
    CYCT(b_+O(49), b_+OE(50)); ret_effect(gb); return;
  }
  CYC(b_+O(49), b_+OE(50));

finished_floating_up:
  CYC(b_+O(50), b_+OE(53)); A = W8(wMapleState);
  CYC(b_+O(53), b_+OE(55)); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(b_+O(55), b_+OE(57));
    goto exchange_touching_book;
  }
  CYC(b_+O(55), b_+OE(57));
  CYC(b_+O(57), b_+OE(59)); L = 0x04;
  CYC(b_+O(59), b_+OE(61)); mem_wr(gb, HL, 0x06);
  CYC(b_+O(61), b_+OE(63)); E = 0x28;
  CYC(b_+O(63), b_+OE(64)); A = mem_rd(gb, DE);
  CYC(b_+O(64), b_+OE(67)); SET_HL(mapleCollisionRadii_bank05);
  CYC(b_+O(67), b_+OE(68)); maple_add_double_index(gb, b_+O(68));
  CYC(b_+O(68), b_+OE(70)); E = 0x26;
  CYC(b_+O(70), b_+OE(71)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(71), b_+OE(72)); mem_wr(gb, DE, A);
  CYC(b_+O(72), b_+OE(73)); E = alu_inc8(gb, E);
  CYC(b_+O(73), b_+OE(74)); A = mem_rd(gb, HL);
  CYC(b_+O(74), b_+OE(75)); mem_wr(gb, DE, A);
  if (!game_seasons) {
    CYC(b_+75, b_+78); A = W8(wActiveGroup);
    CYC(b_+78, b_+79); A = alu_dec8(gb, A);
    if (!(F & FZ)) {
      CYCT(b_+79, b_+81);
      goto normal_encounter;
    }
    CYC(b_+79, b_+81);
    CYC(b_+81, b_+84); A = W8(wMapleState);
    CYC(b_+84, b_+86); alu_and(gb, 0x0f);
    CYC(b_+86, b_+89); SET_BC(0x0712);
    if (F & FZ) {
      CYCT(b_+89, b_+91);
      goto set_past_encounter_flag;
    }
    CYC(b_+89, b_+91);
    CYC(b_+91, b_+93); A = 0x44;
    CALL_C(b_+93, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+96);
    CYC(b_+96, b_+99); SET_BC(0x0713);
    if (!(F & FZ)) {
      CYCT(b_+99, b_+101);
      goto normal_encounter;
    }
    CYC(b_+99, b_+101);

set_past_encounter_flag:
    CYC(b_+101, b_+103); A = 0x44;
    CALL_C(b_+103, setGlobalFlag_hook, SYM(setGlobalFlag), b_+106);
    CYC(b_+106, b_+108);
    goto show_text;
  }

normal_encounter:
  CYC(b_+O(108), b_+OE(111)); A = W8(wMapleState);
  CYC(b_+O(111), b_+OE(113)); alu_and(gb, 0x0f);
  CYC(b_+O(113), b_+OE(116)); SET_BC(0x0700);
  if (F & FZ) {
    CYCT(b_+O(116), b_+OE(118));
    goto show_text;
  }
  CYC(b_+O(116), b_+OE(118));
  CYC(b_+O(118), b_+OE(120)); C = 0x05;
  CYC(b_+O(120), b_+OE(122)); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(b_+O(122), b_+OE(124));
    goto show_text;
  }
  CYC(b_+O(122), b_+OE(124));
  CALL_C(b_+O(124), getRandomNumber_hook, SYM(getRandomNumber), b_+OE(127));
  CYC(b_+O(127), b_+OE(129)); alu_and(gb, 0x03);
  CYC(b_+O(129), b_+OE(132)); SET_HL(mapleNormalEncounterText_bank05);
  CYC(b_+O(132), b_+OE(133)); maple_add_a_to_hl(gb, b_+O(133));
  CYC(b_+O(133), b_+OE(134)); C = mem_rd(gb, HL);

show_text:
  CALL_C(b_+O(134), showText_hook, SYM(showText), b_+OE(137));
  CYC(b_+O(137), b_+OE(138)); alu_xor(gb, A);
  CYC(b_+O(138), b_+OE(141)); W8(wDisabledObjects) = A;
  CYC(b_+O(141), b_+OE(144)); W8(wMenuDisabled) = A;
  CYC(b_+O(144), b_+OE(147)); TAIL(mapleDecideItemToCollectAndUpdateTargetAngle);

exchange_touching_book:
  CYC(b_+O(147), b_+OE(149)); A = 0x0b;
  CYC(b_+O(149), b_+OE(151)); L = 0x04;
  CYC(b_+O(151), b_+OE(152)); mem_wr(gb, HL, A);
  CYC(b_+O(152), b_+OE(154)); L = 0x08;
  CYC(b_+O(154), b_+OE(155)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(155), b_+OE(157)); mem_wr(gb, HL, 0xff);
  CYC(b_+O(157), b_+OE(159)); L = 0x10;
  CYC(b_+O(159), b_+OE(161)); mem_wr(gb, HL, 0x28);
  CYC(b_+O(161), b_+OE(164)); SET_BC(GV(0x070d, 0x0709)); // TX_070d, TX_0709
  CYC(b_+O(164), b_+OE(167)); TAIL(showText);
}

void mapleUpdateOscillation_hook(GB *gb) {
  BASE(mapleUpdateOscillation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); E = 0x28;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+6); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+6, b_+7); ret_effect(gb); return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); C = 0x00;
  CALL_C(b_+9, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+12);
  CYC(b_+12, b_+14); L = 0x3c;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); A = alu_dec8(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+18); ret_effect(gb); return;
  }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); A = 0x16;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); L = 0x14;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); alu_cpl(gb);
  CYC(b_+25, b_+26); A = alu_inc8(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_cpl(gb);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void mapleUpdateAngle_hook(GB *gb) {
  BASE(mapleUpdateAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(w1Companion_var3b);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = 0x3a;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = 0x09;
  CYC(b_+11, b_+13); E = 0x3d;
  CYC(b_+13, b_+14); L = mem_rd(gb, HL);
  CYC(b_+14, b_+16); H8(hFF8B) = A;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CALL_C(b_+17, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), SYM(mapleDecideAnimation));
  TAIL(mapleDecideAnimation);
}

void mapleDecideAnimation_hook(GB *gb) {
  BASE(mapleDecideAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x3e;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto done;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0x09;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CALL_C(b_+10, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+13);
  CYC(b_+13, b_+15); alu_add(gb, 0x04);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+18); E = 0x28;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_add(gb, A);
  CYC(b_+20, b_+21); alu_add(gb, A);
  CYC(b_+21, b_+22); alu_add(gb, B);
  CYC(b_+22, b_+24); L = 0x30;
  CYC(b_+24, b_+25); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CALL_C_CC(b_+25, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+28);
  } else {
    CYC(b_+25, b_+28);
  }

done:
  CYC(b_+28, b_+29); alu_or(gb, D);
  CYC(b_+29, b_+30); ret_effect(gb);
}

void mapleState6_hook(GB *gb) {
  BASE(mapleState6);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, mapleUpdateOscillation_hook, SYM(mapleUpdateOscillation), b_+3);
  CALL_C(b_+3, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+6);
  CALL_C(b_+6, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+9);
  CYC(b_+9, b_+12); A = W8(wActiveMusic);
  CYC(b_+12, b_+14); alu_cp(gb, 0x2c);
  if (F & FZ) {
    CYCT(b_+14, b_+16);
    goto update_angle;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); A = 0x2c;
  CYC(b_+18, b_+21); W8(wActiveMusic) = A;
  CALL_C(b_+21, playSound_b00_hook, SYM(playSound_b00), b_+24);

update_angle:
  CYC(b_+24, b_+26); L = 0x3d;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); L = 0x09;
  CYC(b_+29, b_+30); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CALL_C_CC(b_+30, mapleUpdateAngle_hook, SYM(mapleUpdateAngle), b_+33);
  } else {
    CYC(b_+30, b_+33);
  }
  CALL_C(b_+33, mapleDecideItemToCollectAndUpdateTargetAngle_hook, SYM(mapleDecideItemToCollectAndUpdateTargetAngle), b_+36);
  CALL_C(b_+36, objectApplySpeed_hook, SYM(objectApplySpeed), b_+39);
  CYC(b_+39, b_+41); E = 0x18;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); H = A;
  CYC(b_+43, b_+44); E = alu_inc8(gb, E);
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+46); L = A;
  CALL_C(b_+46, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+49);
  if (!(F & FC)) {
    CYCT(b_+49, b_+52);
    TAIL(mapleKeepInBounds);
  }
  CYC(b_+49, b_+52);
  CYC(b_+52, b_+54); E = 0x18;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+56); H = A;
  CYC(b_+56, b_+57); E = alu_inc8(gb, E);
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+60); alu_or(gb, 0x04);
  CYC(b_+60, b_+61); L = A;
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x04);
  CYC(b_+63, b_+64); L = alu_inc8(gb, L);
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x00);
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+69); alu_or(gb, 0x03);
  CYC(b_+69, b_+70); L = A;
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+73); E = 0x2e;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CYC(b_+74, b_+76); E = 0x04;
  CYC(b_+76, b_+78); A = 0x07;
  CYC(b_+78, b_+79); mem_wr(gb, DE, A);
  CYC(b_+79, b_+81); E = 0x28;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+83, mapleFunc_6c27_hook, SYM(mapleFunc_6c27), b_+86);
  } else {
    CYC(b_+83, b_+86);
  }
  if (F & FZ) {
    CYCT(b_+86, b_+87); ret_effect(gb); return;
  }
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+89); alu_add(gb, 0x16);
  CYC(b_+89, b_+92); TAIL(specialObjectSetAnimation);
}

void mapleState7_hook(GB *gb) {
  BASE(mapleState7);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x28;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+11);
    goto any_vehicle;
  }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = 0x26;
  CYC(b_+13, b_+15); A = 0x08;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+19); A = 0x0a;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CALL_C(b_+20, mapleFindUnexplodedBomb_hook, SYM(mapleFindUnexplodedBomb), b_+23);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+25);
    goto find_next_bomb;
  }
  CYC(b_+23, b_+25);
  CALL_C(b_+25, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+28);
  if (F & FC) {
    CYCT(b_+28, b_+30);
    goto explosive_bomb_near_maple;
  }
  CYC(b_+28, b_+30);

find_next_bomb:
  CALL_C(b_+30, mapleFindNextUnexplodedBomb_hook, SYM(mapleFindNextUnexplodedBomb), b_+33);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35);
    goto update_item_being_collected;
  }
  CYC(b_+33, b_+35);
  CALL_C(b_+35, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+38);
  if (F & FC) {
    CYCT(b_+38, b_+40);
    goto explosive_bomb_near_maple;
  }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); E = 0x16;
  CYC(b_+42, b_+43); alu_xor(gb, A);
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+45); E = alu_inc8(gb, E);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+48);
  goto update_item_being_collected;

explosive_bomb_near_maple:
  CYC(b_+48, b_+50); L = 0x2f;
  CYC(b_+50, b_+52); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(b_+52, b_+54); B = 0x00;
  CYC(b_+54, b_+56); L = 0x0b;
  CYC(b_+56, b_+57); E = L;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+59); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+59, b_+61);
    goto update_bomb_x;
  }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); B = alu_inc8(gb, B);
  if (F & FC) {
    CYCT(b_+62, b_+64);
    goto decrement_bomb_y;
  }
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+65); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+65, b_+67);
  goto update_bomb_x;

decrement_bomb_y:
  CYC(b_+67, b_+68); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

update_bomb_x:
  CYC(b_+68, b_+70); L = 0x0d;
  CYC(b_+70, b_+71); E = L;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+73); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+73, b_+75);
    goto check_bomb_position;
  }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+76); B = alu_inc8(gb, B);
  if (F & FC) {
    CYCT(b_+76, b_+78);
    goto decrement_bomb_x;
  }
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+79); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+79, b_+81);
  goto check_bomb_position;

decrement_bomb_x:
  CYC(b_+81, b_+82); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

check_bomb_position:
  CYC(b_+82, b_+83); A = B;
  CYC(b_+83, b_+84); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+84, b_+86);
    goto update_item_being_collected;
  }
  CYC(b_+84, b_+86);
  CYC(b_+86, b_+88); L = 0x0e;
  CYC(b_+88, b_+89); A = mem_rd(gb, HL);
  CYC(b_+89, b_+91); alu_sub(gb, 0x40);
  CYC(b_+91, b_+92); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+95); alu_sbc(gb, 0x00);
  CYC(b_+95, b_+96); mem_wr(gb, HL, A);
  CYC(b_+96, b_+98); alu_cp(gb, 0xf8);
  if (!(F & FZ)) {
    CYCT(b_+98, b_+100);
    goto update_item_being_collected;
  }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); L = 0x2f;
  CYC(b_+102, b_+104); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x20));
  CYC(b_+104, b_+106); A = 0x1a;
  CALL_C(b_+106, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+109);
  CYC(b_+109, b_+110); H = D;
  CYC(b_+110, b_+112); L = 0x04;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x08);
  CYC(b_+114, b_+115); L = alu_inc8(gb, L);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x00);
  CYC(b_+117, b_+119); L = 0x07;
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x20);
  CYC(b_+121, b_+123); E = 0x18;
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+125); H = A;
  CYC(b_+125, b_+126); E = alu_inc8(gb, E);
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CYC(b_+127, b_+128); L = A;
  CYC(b_+128, b_+129); A = mem_rd(gb, HL);
  CYC(b_+129, b_+130); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+130, b_+132);
    goto update_item_being_collected;
  }
  CYC(b_+130, b_+132);
  CYC(b_+132, b_+133); A = mem_rd(gb, DE);
  CYC(b_+133, b_+135); alu_add(gb, 0x04);
  CYC(b_+135, b_+136); L = A;
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x01);
  CYC(b_+138, b_+140); alu_add(gb, 0x05);
  CYC(b_+140, b_+141); L = A;
  CYC(b_+141, b_+143); mem_wr(gb, HL, 0x80);
  CYC(b_+143, b_+144); alu_xor(gb, A);
  CYC(b_+144, b_+146); E = 0x18;
  CYC(b_+146, b_+147); mem_wr(gb, DE, A);

update_item_being_collected:
  CYC(b_+147, b_+149); E = 0x26;
  CYC(b_+149, b_+151); A = 0x02;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CYC(b_+152, b_+153); E = alu_inc8(gb, E);
  CYC(b_+153, b_+155); A = 0x02;
  CYC(b_+155, b_+156); mem_wr(gb, DE, A);

any_vehicle:
  CYC(b_+156, b_+158); E = 0x18;
  CYC(b_+158, b_+159); A = mem_rd(gb, DE);
  CYC(b_+159, b_+160); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+160, b_+161); ret_effect(gb); return;
  }
  CYC(b_+160, b_+161);
  CYC(b_+161, b_+162); H = A;
  CYC(b_+162, b_+163); E = alu_inc8(gb, E);
  CYC(b_+163, b_+164); A = mem_rd(gb, DE);
  CYC(b_+164, b_+165); L = A;
  CYC(b_+165, b_+166); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+166, b_+167); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+167, b_+169);
    goto item_collected;
  }
  CYC(b_+167, b_+169);
  CYC(b_+169, b_+170); L = alu_inc8(gb, L);
  CYC(b_+170, b_+172); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+172, b_+174);
    goto item_collected;
  }
  CYC(b_+172, b_+174);
  CYC(b_+174, b_+176); E = 0x25;
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+178); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+178, b_+179); ret_effect(gb); return;
  }
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+181); E = 0x18;
  CYC(b_+181, b_+182); A = mem_rd(gb, DE);
  CYC(b_+182, b_+183); H = A;
  CYC(b_+183, b_+185); L = 0xc3;
  CYC(b_+185, b_+187); A = 0x80;
  CYC(b_+187, b_+188); mem_wr(gb, HL, A);
  CYC(b_+188, b_+189); alu_xor(gb, A);
  CYC(b_+189, b_+191); L = 0xeb;
  CYC(b_+191, b_+192); mem_wr(gb, HL, A);
  CYC(b_+192, b_+194); L = 0xe4;
  CYC(b_+194, b_+195); mem_wr(gb, HL, A);
  CYC(b_+195, b_+197); E = 0x2e;
  CYC(b_+197, b_+198); A = mem_rd(gb, DE);
  CYC(b_+198, b_+201); SET_HL(mapleItemValues_bank05);
  CYC(b_+201, b_+202); maple_add_a_to_hl(gb, b_+202);
  CYC(b_+202, b_+204); A = 0x0e;
  CYC(b_+204, b_+205); mem_wr(gb, DE, A);
  CYC(b_+205, b_+207); E = 0x29;
  CYC(b_+207, b_+208); A = mem_rd(gb, DE);
  CYC(b_+208, b_+209); B = A;
  CYC(b_+209, b_+210); A = mem_rd(gb, HL);
  CYC(b_+210, b_+211); alu_add(gb, B);
  CYC(b_+211, b_+212); mem_wr(gb, DE, A);
  CYC(b_+212, b_+214); E = 0x28;
  CYC(b_+214, b_+215); A = mem_rd(gb, DE);
  CYC(b_+215, b_+216); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+216, b_+218);
    goto item_collected;
  }
  CYC(b_+216, b_+218);
  CYC(b_+218, b_+220); A = 0x0a;
  CYC(b_+220, b_+222);
  goto set_state;

item_collected:
  CYC(b_+222, b_+223); H = D;
  CYC(b_+223, b_+225); L = 0x16;
  CYC(b_+225, b_+226); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+226, b_+227); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+227, b_+228); ret_effect(gb); return;
  }
  CYC(b_+227, b_+228);
  CYC(b_+228, b_+230); A = 0x06;

set_state:
  CYC(b_+230, b_+232); E = 0x04;
  CYC(b_+232, b_+233); mem_wr(gb, DE, A);
  CYC(b_+233, b_+235); E = 0x3d;
  CYC(b_+235, b_+236); A = mem_rd(gb, DE);
  CYC(b_+236, b_+238); E = 0x09;
  CYC(b_+238, b_+239); mem_wr(gb, DE, A);
  CYC(b_+239, b_+240); ret_effect(gb);
}

void mapleStateA_hook(GB *gb) {
  BASE(mapleStateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CALL_C(b_+3, itemDecCounter2_hook, SYM(itemDecCounter2), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb); return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x04;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x06);
  CYC(b_+11, b_+13); L = 0x08;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); L = 0x0f;
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); A = 0x04;
  CYC(b_+19, b_+22); TAIL(specialObjectSetAnimation);
}

void mapleState8_hook(GB *gb) {
  BASE(mapleState8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x05;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (maple_jump_table(gb));
    if (jt_ == b_+15) { goto substate0; }
    else if (jt_ == b_+33) { goto substate1; }
    else if (jt_ == b_+48) { goto substate2; }
    else if (jt_ == b_+61) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+15, itemDecCounter2_hook, SYM(itemDecCounter2), b_+18);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+19); ret_effect(gb); return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); L = 0x05;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x01);
  CYC(b_+23, b_+25); L = 0x14;
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+30); A = 0x13;
  CYC(b_+30, b_+33); specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CYC(b_+33, b_+35); C = 0x40;
  CALL_C(b_+35, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+38);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+39); ret_effect(gb); return;
  }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); L = 0x05;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x02);
  CYC(b_+43, b_+45); L = 0x07;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x40);
  CYC(b_+47, b_+48); ret_effect(gb); return;

substate2:
  CALL_C(b_+48, itemDecCounter2_hook, SYM(itemDecCounter2), b_+51);
  if (!(F & FZ)) {
    CYCT(b_+51, b_+52); ret_effect(gb); return;
  }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); L = 0x05;
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x03);
  CYC(b_+56, b_+58); A = 0x08;
  CYC(b_+58, b_+61); specialObjectSetAnimation_hook(gb);
  return;

substate3:
  CYC(b_+61, b_+62); H = D;
  CYC(b_+62, b_+64); L = 0x0f;
  CYC(b_+64, b_+65); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+65, b_+66); A = mem_rd(gb, HL);
  CYC(b_+66, b_+68); alu_cp(gb, 0xe9);
  if (!(F & FC)) {
    CYCT(b_+68, b_+69); ret_effect(gb); return;
  }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); L = 0x04;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x06);
  CYC(b_+73, b_+75); L = 0x29;
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+76, b_+78); L = 0x14;
  CYC(b_+78, b_+80); A = 0x40;
  CYC(b_+80, b_+81); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x00);
  CYC(b_+83, b_+86); TAIL(mapleDecideItemToCollectAndUpdateTargetAngle);
}

void mapleState9_hook(GB *gb) {
  BASE(mapleState9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x05;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (maple_jump_table(gb));
    if (jt_ == b_+13) { goto substate0; }
    else if (jt_ == b_+78) { goto substate1; }
    else if (jt_ == b_+116) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+13, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+16);
  CYC(b_+16, b_+18); A = 0x3c;
  CYC(b_+18, b_+21); W8(wInstrumentsDisabledCounter) = A;
  CYC(b_+21, b_+23); A = 0x01;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = 0x29;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+30); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+30, b_+32);
    goto show_text;
  }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+34); alu_cp(gb, B);
  CYC(b_+34, b_+36); A = 0x01;
  if (F & FZ) {
    CYCT(b_+36, b_+38);
    goto show_text;
  }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  if (F & FC) {
    CYCT(b_+39, b_+41);
    goto show_text;
  }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); A = alu_inc8(gb, A);

show_text:
  CYC(b_+42, b_+45); SET_HL(mapleState9TextIndices_bank05);
  CYC(b_+45, b_+46); maple_add_double_index(gb, b_+46);
  CYC(b_+46, b_+47); C = mem_rd(gb, HL);
  CYC(b_+47, b_+48); SET_HL(HL + 1);
  CYC(b_+48, b_+49); B = mem_rd(gb, HL);
  CALL_C(b_+49, showText_hook, SYM(showText), b_+52);
  CALL_C(b_+52, mapleGetCardinalAngleTowardLink_hook, SYM(mapleGetCardinalAngleTowardLink), b_+55);
  CALL_C(b_+55, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+58);
  CYC(b_+58, b_+60); alu_add(gb, 0x04);
  CYC(b_+60, b_+61); B = A;
  CYC(b_+61, b_+63); E = 0x28;
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); alu_add(gb, A);
  CYC(b_+65, b_+66); alu_add(gb, A);
  CYC(b_+66, b_+67); alu_add(gb, B);
  CYC(b_+67, b_+70); specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(b_+78, mapleUpdateOscillation_hook, SYM(mapleUpdateOscillation), b_+81);
  CALL_C(b_+81, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+84);
  CYC(b_+84, b_+86); A = 0x80;
  CYC(b_+86, b_+89); W8(wTextIsActive) = A;
  CYC(b_+89, b_+91); A = 0x1f;
  CYC(b_+91, b_+94); W8(wDisabledObjects) = A;
  CYC(b_+94, b_+96); L = 0x09;
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x18);
  CYC(b_+98, b_+100); L = 0x10;
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x78);
  CYC(b_+102, b_+104); L = 0x05;
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x02);
  CYC(b_+106, b_+108); E = 0x28;
  CYC(b_+108, b_+109); A = mem_rd(gb, DE);
  CYC(b_+109, b_+110); alu_add(gb, A);
  CYC(b_+110, b_+111); alu_add(gb, A);
  CYC(b_+111, b_+113); alu_add(gb, 0x07);
  CYC(b_+113, b_+116); specialObjectSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(b_+116, mapleUpdateOscillation_hook, SYM(mapleUpdateOscillation), b_+119);
  CALL_C(b_+119, objectApplySpeed_hook, SYM(objectApplySpeed), b_+122);
  CALL_C(b_+122, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+125);
  if (F & FC) {
    CYCT(b_+125, b_+126); ret_effect(gb); return;
  }
  CYC(b_+125, b_+126);
  TAIL(mapleEndEncounter);
}

void mapleEndEncounter_hook(GB *gb) {
  BASE(mapleEndEncounter);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wTextIsActive) = A;
  CYC(b_+4, b_+7); W8(wDisabledObjects) = A;
  CYC(b_+7, b_+10); W8(wMenuDisabled) = A;
  CYC(b_+10, b_+13); W8(wDisableScreenTransitions) = A;
  CALL_C(b_+13, mapleIncrementMeetingCounter_hook, SYM(mapleIncrementMeetingCounter), SYM(mapleDeleteSelf));
  TAIL(mapleDeleteSelf);
}

void mapleDeleteSelf_hook(GB *gb) {
  BASE(mapleDeleteSelf);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wActiveMusic2);
  CYC(b_+3, b_+6); W8(wActiveMusic) = A;
  CALL_C(b_+6, playSound_b00_hook, SYM(playSound_b00), b_+9);
  CYC(b_+9, b_+10); SET_AF(pop_effect(gb));
  CYC(b_+10, b_+11); alu_xor(gb, A);
  CYC(b_+11, b_+14); W8(wIsMaplePresent) = A;
  CYC(b_+14, b_+17); TAIL(itemDelete);
}

void mapleStateB_hook(GB *gb) {
  BASE(mapleStateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(1)); E = alu_inc8(gb, E);
  CYC(b_+O(1), b_+OE(2)); A = mem_rd(gb, DE);
  CYC(b_+O(2), b_+OE(3)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(3), b_+OE(5));
    goto substate1;
  }
  CYC(b_+O(3), b_+OE(5));
  CALL_C(b_+O(5), mapleUpdateOscillation_hook, SYM(mapleUpdateOscillation), b_+OE(8));
  if (!game_seasons) {      // Ages keeps a direction fixed by bit 7 of the direction byte
    CYC(b_+8, b_+10); E = 0x08;
    CYC(b_+10, b_+11); A = mem_rd(gb, DE);
    CYC(b_+11, b_+13); alu_bit(gb, 7, A);
    if (F & FZ) {
      CYCT(b_+13, b_+15);
      goto face_link;
    }
    CYC(b_+13, b_+15);
    CYC(b_+15, b_+17); alu_and(gb, 0x03);
    CYC(b_+17, b_+19);
    goto determine_animation;
  }

face_link:
  CALL_C(b_+O(19), objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+OE(22));
  CALL_C(b_+O(22), convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+OE(25));
  CYC(b_+O(25), b_+OE(26)); H = D;
  CYC(b_+O(26), b_+OE(28)); L = 0x08;
  CYC(b_+O(28), b_+OE(29)); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+O(29), b_+OE(30)); mem_wr(gb, HL, A);
  if (F & FZ) {
    CYCT(b_+O(30), b_+OE(32));
    goto wait_for_text;
  }
  CYC(b_+O(30), b_+OE(32));

determine_animation:
  CYC(b_+O(32), b_+OE(34)); alu_add(gb, 0x04);
  CYC(b_+O(34), b_+OE(35)); B = A;
  CYC(b_+O(35), b_+OE(37)); E = 0x28;
  CYC(b_+O(37), b_+OE(38)); A = mem_rd(gb, DE);
  CYC(b_+O(38), b_+OE(39)); alu_add(gb, A);
  CYC(b_+O(39), b_+OE(40)); alu_add(gb, A);
  CYC(b_+O(40), b_+OE(41)); alu_add(gb, B);
  CALL_C(b_+O(41), specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+OE(44));

wait_for_text:
  CALL_C(b_+O(44), retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+OE(47));
  CYC(b_+O(47), b_+OE(50)); SET_HL(wMapleState);
  CYC(b_+O(50), b_+OE(52)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x20));
  CYC(b_+O(52), b_+OE(54)); E = 0x09;
  CYC(b_+O(54), b_+OE(55)); A = mem_rd(gb, DE);
  CYC(b_+O(55), b_+OE(56)); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+O(56), b_+OE(59));
    TAIL(objectApplySpeed);
  }
  CYC(b_+O(56), b_+OE(59));
  CYC(b_+O(59), b_+OE(60)); ret_effect(gb); return;

substate1:
  CYC(b_+O(60), b_+OE(61)); A = alu_dec8(gb, A);
  CYC(b_+O(61), b_+OE(62)); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(b_+O(62), b_+OE(63)); ret_effect(gb); return;
  }
  CYC(b_+O(62), b_+OE(63));
  CYC(b_+O(63), b_+OE(66)); SET_BC(GV(0x0711, 0x070b));
  CALL_C(b_+O(66), showText_hook, SYM(showText), b_+OE(69));
  CYC(b_+O(69), b_+OE(71)); E = 0x09;
  CYC(b_+O(71), b_+OE(73)); A = 0x18;
  CYC(b_+O(73), b_+OE(74)); mem_wr(gb, DE, A);
  CALL_C(b_+O(74), itemIncState_hook, SYM(itemIncState), b_+OE(77));
  CYC(b_+O(77), b_+OE(79)); L = 0x10;
  CYC(b_+O(79), b_+OE(81)); mem_wr(gb, HL, 0x78);
  TAIL(mapleStateC);
}

void mapleStateC_hook(GB *gb) {
  BASE(mapleStateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, mapleUpdateOscillation_hook, SYM(mapleUpdateOscillation), b_+3);
  CALL_C(b_+3, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+6);
  CALL_C(b_+6, objectApplySpeed_hook, SYM(objectApplySpeed), b_+9);
  CYC(b_+9, b_+11); E = 0x28;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_add(gb, A);
  CYC(b_+13, b_+14); alu_add(gb, A);
  CYC(b_+14, b_+16); alu_add(gb, 0x07);
  CYC(b_+16, b_+19); SET_HL(wMapleState);
  CYC(b_+19, b_+21); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(b_+21, b_+23); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x10));
  if (!(F & FZ)) {
    CALL_C_CC(b_+23, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+26);
  } else {
    CYC(b_+23, b_+26);
  }
  CALL_C(b_+26, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+29);
  if (F & FC) {
    CYCT(b_+29, b_+30); ret_effect(gb); return;
  }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+33); TAIL(mapleEndEncounter);
}

void mapleKeepInBounds_hook(GB *gb) {
  BASE(mapleKeepInBounds);
  CYC(b_+0, b_+2); E = 0x0b;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0xf0);
  if (F & FC) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); alu_xor(gb, A);
  }
  CYC(b_+8, b_+10); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(b_+10, b_+12);
    goto check_y_upper;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0x20;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17);
  goto check_x;

check_y_upper:
  CYC(b_+17, b_+19); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(b_+19, b_+21);
    goto check_x;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = 0x78;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);

check_x:
  CYC(b_+24, b_+26); E = 0x0d;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+29); alu_cp(gb, 0xf0);
  if (F & FC) {
    CYCT(b_+29, b_+31);
  } else {
    CYC(b_+29, b_+31);
    CYC(b_+31, b_+32); alu_xor(gb, A);
  }
  CYC(b_+32, b_+34); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(b_+34, b_+36);
    goto check_x_upper;
  }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); A = 0x08;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+41);
  goto done;

check_x_upper:
  CYC(b_+41, b_+43); alu_cp(gb, 0x98);
  if (F & FC) {
    CYCT(b_+43, b_+45);
    goto done;
  }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); A = 0x98;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);

done:
  CYC(b_+48, b_+49); ret_effect(gb);
}

static void maple_spawn_item_drop_check(GB *gb, uint16_t sp0_) {
  BASE(mapleSpawnItemDrops);
  CYC(b_+105, b_+106); push_effect(gb, AF);
  CYC(b_+106, b_+109); SET_HL(mapleItemDropTreasureIndices_bank05);
  CYC(b_+109, b_+110); maple_add_a_to_hl(gb, b_+110);
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CALL_C(b_+111, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+114);
  CYC(b_+114, b_+115); SET_HL(pop_effect(gb));
  if (F & FC) {
    CYCT(b_+115, b_+117);
    goto obtained;
  }
  CYC(b_+115, b_+117);
  CYC(b_+117, b_+118); alu_or(gb, D);
  CYC(b_+118, b_+119); ret_effect(gb); return;

obtained:
  CYC(b_+119, b_+120); A = H;
  CYC(b_+120, b_+122); H8(hFF8B) = A;
  CYC(b_+122, b_+124); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(b_+124, b_+127); TAIL(mapleSpawnItemDrop);
  }
  CYC(b_+124, b_+127);
  CYC(b_+127, b_+128); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+128, b_+130);
    goto not_heart_piece;
  }
  CYC(b_+128, b_+130);
  CYC(b_+130, b_+133); A = W8(wMapleState);
  CYC(b_+133, b_+135); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+135, b_+136); ret_effect(gb); return;
  }
  CYC(b_+135, b_+136);
  CYC(b_+136, b_+138); E = 0x2b;
  CYC(b_+138, b_+139); A = mem_rd(gb, DE);
  CYC(b_+139, b_+140); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+140, b_+141); ret_effect(gb); return;
  }
  CYC(b_+140, b_+141);
  CYC(b_+141, b_+142); A = alu_inc8(gb, A);
  CYC(b_+142, b_+143); mem_wr(gb, DE, A);
  CYC(b_+143, b_+145);
  goto spawn_item;

not_heart_piece:
  CYC(b_+145, b_+146); A = alu_dec8(gb, A);
  CYC(b_+146, b_+149); SET_HL(mapleItemBitmasks_bank05);
  CYC(b_+149, b_+150); maple_add_a_to_hl(gb, b_+150);
  CYC(b_+150, b_+151); B = mem_rd(gb, HL);
  CYC(b_+151, b_+153); E = 0x2c;
  CYC(b_+153, b_+154); A = mem_rd(gb, DE);
  CYC(b_+154, b_+155); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+155, b_+156); ret_effect(gb); return;
  }
  CYC(b_+155, b_+156);
  CYC(b_+156, b_+157); A = mem_rd(gb, DE);
  CYC(b_+157, b_+158); alu_or(gb, B);
  CYC(b_+158, b_+159); mem_wr(gb, DE, A);

spawn_item:
  CYC(b_+159, b_+161); mapleSpawnItemDrop_variant_hook(gb);
}

void mapleSpawnItemDrops_hook(GB *gb) {
  BASE(mapleSpawnItemDrops);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x41;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  if (!(F & FC)) {
    CYCT(b_+5, b_+7);
    goto no_trade_item;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, GV(0x08, 0x01));
  if (!(F & FZ)) {
    CYCT(b_+9, b_+11);
    goto no_trade_item;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); B = GV(0xa5, 0x94);
  CALL_C(b_+13, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+16);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+17); ret_effect(gb); return;
  }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); SET_HL(wMapleState);
  CYC(b_+20, b_+22); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x10));
  CYC(b_+22, b_+23); ret_effect(gb); return;

no_trade_item:
  CYC(b_+23, b_+25); E = 0x2a;
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); E = 0x29;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+32); E = 0x06;
  CYC(b_+32, b_+34); A = 0x05;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);

next_maple_item:
  CYC(b_+35, b_+37); E = 0x03;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+41); SET_HL(mapleItemDropDistributionTable_bank05);
  CYC(b_+41, b_+42); maple_add_double_index(gb, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+43, b_+44); H = mem_rd(gb, HL);
  CYC(b_+44, b_+45); L = A;
  CALL_C(b_+45, getRandomIndexFromProbabilityDistribution_hook, SYM(getRandomIndexFromProbabilityDistribution), b_+48);
  CYC(b_+48, b_+49); A = B;
  CYC(b_+49, b_+52); push_effect(gb, b_+52);
  uint16_t local_sp0 = gb->sp;
  maple_spawn_item_drop_check(gb, local_sp0);
  if (gb->pc != b_+52 || gb->sp != (uint16_t)(local_sp0 + 2)) return;
  if (F & FC) {
    CYCT(b_+52, b_+54);
    goto maple_item_done;
  }
  CYC(b_+52, b_+54);
  if (!(F & FZ)) {
    CYCT(b_+54, b_+56);
    goto next_maple_item;
  }
  CYC(b_+54, b_+56);

maple_item_done:
  CYC(b_+56, b_+58); E = 0x06;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); A = alu_dec8(gb, A);
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(b_+61, b_+63);
    goto next_maple_item;
  }
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+65); A = 0x20;
  CYC(b_+65, b_+67); H8(hFF8C) = A;
  CYC(b_+67, b_+69); E = 0x06;
  CYC(b_+69, b_+71); A = 0x05;
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);

next_link_item:
  CYC(b_+72, b_+74); A = H8(hFF8C);
  CYC(b_+74, b_+75); A = alu_dec8(gb, A);
  CYC(b_+75, b_+77); H8(hFF8C) = A;
  if (F & FZ) {
    CYCT(b_+77, b_+79);
    goto done;
  }
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+82); SET_HL(mapleLinkItemDropDistribution_bank05);
  CALL_C(b_+82, getRandomIndexFromProbabilityDistribution_hook, SYM(getRandomIndexFromProbabilityDistribution), b_+85);
  CALL_C(b_+85, mapleCheckLinkCanDropItem_hook, SYM(mapleCheckLinkCanDropItem), b_+88);
  if (F & FZ) {
    CYCT(b_+88, b_+90);
    goto next_link_item;
  }
  CYC(b_+88, b_+90);
  CYC(b_+90, b_+92); D = 0xd0;
  CALL_C(b_+92, mapleSpawnItemDrop_hook, SYM(mapleSpawnItemDrop), b_+95);
  CYC(b_+95, b_+97); D = 0xd1;
  CYC(b_+97, b_+99); E = 0x06;
  CYC(b_+99, b_+100); A = mem_rd(gb, DE);
  CYC(b_+100, b_+101); A = alu_dec8(gb, A);
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(b_+102, b_+104);
    goto next_link_item;
  }
  CYC(b_+102, b_+104);

done:
  CYC(b_+104, b_+105); ret_effect(gb);
}

void mapleSpawnItemDrop_hook(GB *gb) {
  BASE(mapleSpawnItemDrop);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  CYC(b_+3, b_+4); alu_scf(gb);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x14);
  CYC(b_+7, b_+9); E = 0x0b;
  CALL_C(b_+9, objectCopyPosition_rawAddress_hook, SYM(objectCopyPosition_rawAddress), b_+12);
  CYC(b_+12, b_+14); A = H8(hFF8B);
  CYC(b_+14, b_+16); L = 0xc3;
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void mapleSpawnItemDrop_variant_hook(GB *gb) {
  BASE(mapleSpawnItemDrop_variant);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  CYC(b_+3, b_+4); alu_scf(gb);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x15);
  CYC(b_+7, b_+9); L = 0xc2;
  CYC(b_+9, b_+11); A = H8(hFF8B);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CALL_C(b_+13, objectCopyPosition_hook, SYM(objectCopyPosition), b_+16);
  CYC(b_+16, b_+17); alu_or(gb, A);
  CYC(b_+17, b_+18); ret_effect(gb);
}

void mapleDecideItemToCollect_hook(GB *gb) {
  BASE(mapleDecideItemToCollect);
  CYC(b_+0, b_+2); B = 0x00;

id_loop1:
  CYC(b_+2, b_+5); SET_HL(PART_SLOTS);

part_loop1:
  CYC(b_+5, b_+7); L = 0xc0;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+9, b_+11);
    goto next_part1;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+14); alu_cp(gb, 0x15);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16);
    goto next_part1;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+17, b_+18); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+20);
    goto next_part1;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); L = alu_dec8(gb, L);
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+23); ret_effect(gb); return;

next_part1:
  CYC(b_+23, b_+24); H = alu_inc8(gb, H);
  CYC(b_+24, b_+25); A = H;
  CYC(b_+25, b_+27); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(b_+27, b_+29);
    goto part_loop1;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); B = alu_inc8(gb, B);
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+33); alu_cp(gb, 0x05);
  if (F & FC) {
    CYCT(b_+33, b_+35);
    goto id_loop1;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); alu_xor(gb, A);
  CYC(b_+36, b_+38); C = 0x00;
  CYC(b_+38, b_+41); SET_HL(mapleItemIDs_bank05);
  CYC(b_+41, b_+42); maple_add_a_to_hl(gb, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+44); B = A;
  CYC(b_+44, b_+45); alu_xor(gb, A);
  CYC(b_+45, b_+47); H8(hFF91) = A;

id_loop2:
  CYC(b_+47, b_+50); SET_HL(PART_SLOTS);

part_loop2:
  CYC(b_+50, b_+52); L = 0xc0;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+54, b_+56);
    goto next_part2;
  }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+57, b_+59); alu_cp(gb, 0x14);
  if (!(F & FZ)) {
    CYCT(b_+59, b_+61);
    goto next_part2;
  }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+62, b_+63); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+63, b_+65);
    goto next_part2;
  }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); L = 0xcb;
  CYC(b_+67, b_+68); L = mem_rd(gb, HL);
  CYC(b_+68, b_+70); E = 0x0b;
  CYC(b_+70, b_+71); A = mem_rd(gb, DE);
  CYC(b_+71, b_+72); alu_sub(gb, L);
  if (!(F & FC)) {
    CYCT(b_+72, b_+74);
    goto y_distance_done;
  }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+75); alu_cpl(gb);
  CYC(b_+75, b_+76); A = alu_inc8(gb, A);

y_distance_done:
  CYC(b_+76, b_+78); H8(hFF8C) = A;
  CYC(b_+78, b_+80); L = 0xcd;
  CYC(b_+80, b_+81); L = mem_rd(gb, HL);
  CYC(b_+81, b_+83); E = 0x0d;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); alu_sub(gb, L);
  if (!(F & FC)) {
    CYCT(b_+85, b_+87);
    goto x_distance_done;
  }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+88); alu_cpl(gb);
  CYC(b_+88, b_+89); A = alu_inc8(gb, A);

x_distance_done:
  CYC(b_+89, b_+90); L = A;
  CYC(b_+90, b_+92); A = H8(hFF8C);
  CYC(b_+92, b_+93); alu_add(gb, L);
  CYC(b_+93, b_+94); L = A;
  CYC(b_+94, b_+96); A = H8(hFF91);
  CYC(b_+96, b_+97); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+97, b_+99);
    goto select_item;
  }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+101); A = H8(hFF8D);
  CYC(b_+101, b_+102); alu_cp(gb, L);
  if (F & FC) {
    CYCT(b_+102, b_+104);
    goto next_part2;
  }
  CYC(b_+102, b_+104);

select_item:
  CYC(b_+104, b_+105); A = L;
  CYC(b_+105, b_+107); H8(hFF8D) = A;
  CYC(b_+107, b_+108); A = H;
  CYC(b_+108, b_+110); H8(hFF91) = A;

next_part2:
  CYC(b_+110, b_+111); H = alu_inc8(gb, H);
  CYC(b_+111, b_+112); A = H;
  CYC(b_+112, b_+114); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(b_+114, b_+116);
    goto part_loop2;
  }
  CYC(b_+114, b_+116);
  CYC(b_+116, b_+118); A = H8(hFF91);
  CYC(b_+118, b_+119); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+119, b_+121);
    goto found_item;
  }
  CYC(b_+119, b_+121);
  CYC(b_+121, b_+122); C = alu_inc8(gb, C);
  CYC(b_+122, b_+123); A = C;
  CYC(b_+123, b_+125); alu_cp(gb, 0x09);
  if (!(F & FC)) {
    CYCT(b_+125, b_+127);
    goto no_items_left;
  }
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+130); SET_HL(mapleItemIDs_bank05);
  CYC(b_+130, b_+131); maple_add_a_to_hl(gb, b_+131);
  CYC(b_+131, b_+132); A = mem_rd(gb, HL);
  CYC(b_+132, b_+133); B = A;
  CYC(b_+133, b_+135);
  goto id_loop2;

no_items_left:
  CYC(b_+135, b_+136); alu_and(gb, D);
  CYC(b_+136, b_+137); ret_effect(gb); return;

found_item:
  CYC(b_+137, b_+138); H = A;
  CYC(b_+138, b_+140); L = 0xc0;
  CYC(b_+140, b_+141); alu_xor(gb, A);
  CYC(b_+141, b_+142); ret_effect(gb);
}

static void maple_find_bomb_finish(GB *gb) {
  BASE(mapleFindNextUnexplodedBomb);
  CYC(b_+6, b_+8); L = 0x2f;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    goto found;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x60);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); L = 0x0f;
  CYC(b_+18, b_+20); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+20, b_+21); ret_effect(gb); return;
  }
  CYC(b_+20, b_+21);

found:
  CYC(b_+21, b_+23); E = 0x16;
  CYC(b_+23, b_+24); A = H;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); ret_effect(gb);
}

void mapleFindUnexplodedBomb_hook(GB *gb) {
  BASE(mapleFindUnexplodedBomb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x16;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); C = 0x03;
  CALL_C(b_+8, findItemWithID_hook, SYM(findItemWithID), b_+11);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14);
  maple_find_bomb_finish(gb);
}

void mapleFindNextUnexplodedBomb_hook(GB *gb) {
  BASE(mapleFindNextUnexplodedBomb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x03;
  CALL_C(b_+2, findItemWithID_startingAfterH_hook, SYM(findItemWithID_startingAfterH), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  maple_find_bomb_finish(gb);
}

void mapleInitZPositionAndSpeed_hook(GB *gb) {
  BASE(mapleInitZPositionAndSpeed);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x0f;
  CYC(b_+3, b_+5); A = 0xf8;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+8); L = 0x14;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x40);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
  CYC(b_+13, b_+15); L = 0x3c;
  CYC(b_+15, b_+17); A = 0x16;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void mapleGetCardinalAngleTowardLink_hook(GB *gb) {
  BASE(mapleGetCardinalAngleTowardLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x18);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void mapleDecideItemToCollectAndUpdateTargetAngle_hook(GB *gb) {
  BASE(mapleDecideItemToCollectAndUpdateTargetAngle);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, mapleDecideItemToCollect_hook, SYM(mapleDecideItemToCollect), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    goto no_more_items;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = 0x18;
  CYC(b_+7, b_+8); A = H;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); E = alu_inc8(gb, E);
  CYC(b_+10, b_+11); A = L;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = 0x25;
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+18); TAIL(mapleSetTargetDirectionToRelatedObj2);

no_more_items:
  CYC(b_+18, b_+20); E = 0x04;
  CYC(b_+20, b_+22); A = 0x09;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); ret_effect(gb);
}

void mapleSetTargetDirectionToRelatedObj2_hook(GB *gb) {
  BASE(mapleSetTargetDirectionToRelatedObj2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x18;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_or(gb, 0x0b);
  CYC(b_+8, b_+9); L = A;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); C = A;
  CALL_C(b_+14, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+17);
  CYC(b_+17, b_+19); E = 0x3d;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+21); ret_effect(gb);
}

void mapleCheckLinkCanDropItem_hook(GB *gb) {
  BASE(mapleCheckLinkCanDropItem);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+3); alu_sub(gb, 0x05);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (maple_jump_table(gb));
    if (jt_ == b_+23) { goto one_rupee; }
    else if (jt_ == b_+38) { goto bombs; }
    else if (jt_ == b_+62) { goto seed; }
    else if (jt_ == b_+91) { goto heart; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

one_rupee:
  CYC(b_+23, b_+26); SET_HL(wNumRupees);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+28, b_+29); ret_effect(gb); return;
  }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); A = 0x01;
  CALL_C(b_+31, removeRupeeValue_hook, SYM(removeRupeeValue), b_+34);
  CYC(b_+34, b_+36); A = 0x0c;
  CYC(b_+36, b_+38);
  goto set_item_index;

bombs:
  CYC(b_+38, b_+40); A = 0x0a;
  CYC(b_+40, b_+42); H8(hFF8B) = A;
  CALL_C(b_+42, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+45);
  if (!(F & FC)) {
    CYCT(b_+45, b_+47);
    goto cannot_drop;
  }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+50); SET_HL(wNumBombs);
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+53); alu_sub(gb, 0x04);
  if (F & FC) {
    CYCT(b_+53, b_+55);
    goto cannot_drop;
  }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+56); alu_daa(gb);
  CYC(b_+56, b_+57); mem_wr(gb, HL, A);
  CALL_C(b_+57, setStatusBarNeedsRefreshBit1_hook, SYM(setStatusBarNeedsRefreshBit1), b_+60);
  CYC(b_+60, b_+61); alu_or(gb, D);
  CYC(b_+61, b_+62); ret_effect(gb); return;

seed:
  CYC(b_+62, b_+63); A = B;
  CYC(b_+63, b_+65); alu_add(gb, 0x05);
  CYC(b_+65, b_+67); H8(hFF8B) = A;
  CALL_C(b_+67, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+70);
  if (!(F & FC)) {
    CYCT(b_+70, b_+72);
    goto cannot_drop;
  }
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+73); A = B;
  CYC(b_+73, b_+76); SET_HL(wNumEmberSeeds);
  CYC(b_+76, b_+77); maple_add_a_to_hl(gb, b_+77);
  CYC(b_+77, b_+78); A = mem_rd(gb, HL);
  CYC(b_+78, b_+80); alu_sub(gb, 0x05);
  if (F & FC) {
    CYCT(b_+80, b_+82);
    goto cannot_drop;
  }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+83); alu_daa(gb);
  CYC(b_+83, b_+84); mem_wr(gb, HL, A);
  CALL_C(b_+84, setStatusBarNeedsRefreshBit1_hook, SYM(setStatusBarNeedsRefreshBit1), b_+87);
  CYC(b_+87, b_+88); alu_or(gb, D);
  CYC(b_+88, b_+89); ret_effect(gb); return;

cannot_drop:
  CYC(b_+89, b_+90); alu_xor(gb, A);
  CYC(b_+90, b_+91); ret_effect(gb); return;

heart:
  CYC(b_+91, b_+94); SET_HL(wLinkHealth);
  CYC(b_+94, b_+95); A = mem_rd(gb, HL);
  CYC(b_+95, b_+97); alu_cp(gb, 0x0c);
  if (!(F & FC)) {
    CYCT(b_+97, b_+99);
  } else {
    CYC(b_+97, b_+99);
    CYC(b_+99, b_+100); alu_xor(gb, A);
    CYC(b_+100, b_+101); ret_effect(gb); return;
  }
  CYC(b_+101, b_+103); alu_sub(gb, 0x04);
  CYC(b_+103, b_+104); mem_wr(gb, HL, A);
  CYC(b_+104, b_+107); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+107, b_+109); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x04));
  CYC(b_+109, b_+111); A = 0x0b;

set_item_index:
  CYC(b_+111, b_+113); H8(hFF8B) = A;
  CYC(b_+113, b_+114); alu_or(gb, D);
  CYC(b_+114, b_+115); ret_effect(gb);
}

void mapleFunc_6c27_hook(GB *gb) {
  BASE(mapleFunc_6c27);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x07;
  CYC(b_+2, b_+4); A = 0x30;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = 0x0f;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); E = 0x08;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+14);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16);
    goto collision;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); E = 0x0f;
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+21); alu_or(gb, D);
  CYC(b_+21, b_+23); E = 0x28;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); ret_effect(gb); return;

collision:
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+27); ret_effect(gb);
}

void mapleIncrementMeetingCounter_hook(GB *gb) {
  BASE(mapleIncrementMeetingCounter);
  CYC(b_+0, b_+3); SET_HL(wMapleState);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x0f);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+9); alu_cp(gb, 0x0f);
  if (!(F & FC)) {
    CYCT(b_+9, b_+11);
  } else {
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+12); B = alu_inc8(gb, B);
  }
  CYC(b_+12, b_+13); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+14); alu_or(gb, B);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+16); ret_effect(gb);
}
