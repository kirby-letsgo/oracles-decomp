#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t shopItem_jump_table(GB *gb) {
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
  burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

static void shopItem_addAToHl(GB *gb, uint16_t returnAddress) {
  push_effect(gb, returnAddress);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

static void shopItem_addDoubleIndex(GB *gb, uint16_t returnAddress) {
  push_effect(gb, returnAddress);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void shopItemDrawDigit(GB *gb) {
  BASE(shopItemGetTilesForRupeeDisplay);
  CYC(b_+64, b_+66); alu_and(gb, 0x0f);
  CYC(b_+66, b_+67); alu_add(gb, D);
  CYC(b_+67, b_+68); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+68, b_+69); mem_wr(gb, HL, E);
  CYC(b_+69, b_+70); L = alu_inc8(gb, L);
  CYC(b_+70, b_+71); ret_effect(gb);
}

static void shopItemUpdateRupeeDisplay_tail(GB *gb) {
  BASE(shopItemUpdateRupeeDisplay);
  CYC(b_+4, b_+6); A = mem_rd(gb, IO_SVBK);
  CYC(b_+6, b_+7); push_effect(gb, AF);
  CYC(b_+7, b_+9); A = 3;
  CYC(b_+9, b_+11); mem_wr(gb, IO_SVBK, A);
  CYC(b_+11, b_+12); push_effect(gb, DE);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); E = A;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); D = A;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); B = A;
L_43f4:
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); D = (uint8_t)(D | 4);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+26); D = (uint8_t)(D & ~4);
  CYC(b_+26, b_+27); SET_DE(DE + 1);
  CYC(b_+27, b_+28); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto L_43f4; } CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); SET_DE(pop_effect(gb));
  CYC(b_+31, b_+32); SET_AF(pop_effect(gb));
  CYC(b_+32, b_+34); mem_wr(gb, IO_SVBK, A);
  CYC(b_+34, b_+37); SET_HL(wInShop);
  CYC(b_+37, b_+39); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 4));
  CYC(b_+39, b_+40); ret_effect(gb);
}

void interactionCode47_hook(GB *gb);
void shopItemState0_hook(GB *gb);
void shopItemState5_hook(GB *gb);
void shopItemPopStackAndDeleteSelf_hook(GB *gb);
void shopItemState2_hook(GB *gb);
void shopItemState4_hook(GB *gb);
void shopItemClearRupeeDisplay_hook(GB *gb);
void shopItemUpdateRupeeDisplay_hook(GB *gb);
void shopItemGetTilesForRupeeDisplay_hook(GB *gb);
void shopItemCheckGrabbed_hook(GB *gb);
void shopItemState3_hook(GB *gb);

void interactionCode47_hook(GB *gb) {
  BASE(interactionCode47);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(shopItem_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(shopItemState0) && hook_enabled_at(gb, SYM(shopItemState0))) { shopItemState0_hook(gb); return; }
    else if (jt_ == SYM(shopItemState2) && hook_enabled_at(gb, SYM(shopItemState2))) { shopItemState2_hook(gb); return; }
    else if (jt_ == SYM(shopItemState4) && hook_enabled_at(gb, SYM(shopItemState4))) { shopItemState4_hook(gb); return; }
    else if (jt_ == SYM(shopItemState5) && hook_enabled_at(gb, SYM(shopItemState5))) { shopItemState5_hook(gb); return; }
    else if (jt_ == SYM(objectAddToGrabbableObjectBuffer) && hook_enabled_at(gb, SYM(objectAddToGrabbableObjectBuffer))) { objectAddToGrabbableObjectBuffer_hook(gb); return; }
    else if (jt_ == SYM(shopItemState3) && hook_enabled_at(gb, SYM(shopItemState3))) { shopItemState3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void shopItemState0_hook(GB *gb) {
  BASE(shopItemState0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wAButtonSensitiveObjectListEnd);
  CYC(b_+3, b_+5); alu_and(gb, 2);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 1;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); E = 0x42;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_cp(gb, 0);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto L_430c; } CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); A = 0x2c;
  CALL_C(b_+18, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+21);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto L_430c; } CYC(b_+21, b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wRingBoxLevel);
  CYC(b_+26, b_+27); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+27, b_+29); goto L_430c; } CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0x14;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
L_430c:
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+35); alu_cp(gb, 4);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto L_431b; } CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); A = 3;
  CALL_C(b_+39, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+42);
  if (!(F & FC)) { CYCT(b_+42, b_+45); shopItemPopStackAndDeleteSelf_hook(gb); return; } CYC(b_+42, b_+45);
  CYC(b_+45, b_+47); goto L_4327;
L_431b:
  CYC(b_+47, b_+49); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(b_+49, b_+51); goto L_4327; } CYC(b_+49, b_+51);
  CALL_C(b_+51, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+54);
  if (F & FZ) { CYCT(b_+54, b_+56); goto L_4327; } CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); A = 0x13;
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
L_4327:
  CYC(b_+59, b_+61); A = 0x0e;
  CALL_C(b_+61, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+64);
  if (F & FC) { CYCT(b_+64, b_+66); goto L_4339; } CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); A = 0x1d;
  CALL_C(b_+68, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+71);
  if (F & FZ) { CYCT(b_+71, b_+73); goto L_4339; } CYC(b_+71, b_+73);
  CYC(b_+73, b_+75); C = 8;
  CYC(b_+75, b_+77); goto L_433b;
L_4339:
  CYC(b_+77, b_+79); C = 0;
L_433b:
  CYC(b_+79, b_+82); A = mem_rd(gb, wBoughtShopItems2);
  CYC(b_+82, b_+84); alu_and(gb, 0xf7);
  CYC(b_+84, b_+85); alu_or(gb, C);
  CYC(b_+85, b_+88); mem_wr(gb, wBoughtShopItems2, A);
  CYC(b_+88, b_+90); A = 0x0d;
  CALL_C(b_+90, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+93);
  CYC(b_+93, b_+95); C = 0x10;
  if (F & FC) { CYCT(b_+95, b_+97); goto L_434f; } CYC(b_+95, b_+97);
  CYC(b_+97, b_+99); C = 0x20;
L_434f:
  CYC(b_+99, b_+102); A = mem_rd(gb, wBoughtShopItems2);
  CYC(b_+102, b_+104); alu_and(gb, 0xcf);
  CYC(b_+104, b_+105); alu_or(gb, C);
  CYC(b_+105, b_+108); mem_wr(gb, wBoughtShopItems2, A);
L_4358:
  CYC(b_+108, b_+110); E = 0x42;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); alu_add(gb, A);
  CYC(b_+112, b_+115); SET_HL(SYM(shopItemReplacementTable));
  CYC(b_+115, b_+116); shopItem_addDoubleIndex(gb, b_+116);
  CYC(b_+116, b_+117); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+117, b_+118); C = A;
  CYC(b_+118, b_+120); B = 0xc6;
  CYC(b_+120, b_+121); A = mem_rd(gb, BC);
  CYC(b_+121, b_+122); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+122, b_+124); goto L_4376; } CYC(b_+122, b_+124);
  CYC(b_+124, b_+125); SET_HL(HL + 1);
  CYC(b_+125, b_+126); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+126, b_+128); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+128, b_+130); shopItemPopStackAndDeleteSelf_hook(gb); return; } CYC(b_+128, b_+130);
  CYC(b_+130, b_+131); mem_wr(gb, DE, A);
  CYC(b_+131, b_+133); E = 0x4d;
  CYC(b_+133, b_+134); A = mem_rd(gb, DE);
  CYC(b_+134, b_+135); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+135, b_+136); mem_wr(gb, DE, A);
  CYC(b_+136, b_+138); goto L_4358;
L_4376:
  CALL_C(b_+138, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+141);
  CYC(b_+141, b_+143); A = 7;
  CALL_C(b_+143, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+146);
  CYC(b_+146, b_+148); L = 0x70;
  CYC(b_+148, b_+150); E = 0x4b;
  CYC(b_+150, b_+151); A = mem_rd(gb, DE);
  CYC(b_+151, b_+152); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+152, b_+154); E = 0x4d;
  CYC(b_+154, b_+155); A = mem_rd(gb, DE);
  CYC(b_+155, b_+156); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(b_+156, objectSetVisible83_hook, SYM(objectSetVisible83), b_+159);
  CYC(b_+159, b_+161); TAIL(shopItemUpdateRupeeDisplay);
}

void shopItemState5_hook(GB *gb) {
  BASE(shopItemState5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+7); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+7, b_+10); mem_wr(gb, wMenuDisabled, A);
  TAIL(shopItemPopStackAndDeleteSelf);
}

void shopItemPopStackAndDeleteSelf_hook(GB *gb) {
  BASE(shopItemPopStackAndDeleteSelf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_AF(pop_effect(gb));
  CYC(b_+1, b_+4); TAIL(interactionDelete);
}

void shopItemState2_hook(GB *gb) {
  BASE(shopItemState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(shopItem_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+8) { goto L_43a3; }
    else if (jt_ == b_+21) { goto L_43b0; }
    else { HANDOFF(HL); }
  } while (0);
L_43a3:
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 8;
  CYC(b_+13, b_+16); mem_wr(gb, wLinkGrabState2, A);
  CALL_C(b_+16, objectSetVisible80_hook, SYM(objectSetVisible80), b_+19);
  CYC(b_+19, b_+21); TAIL(shopItemClearRupeeDisplay);
L_43b0:
  CALL_C(b_+21, shopItemCheckGrabbed_hook, SYM(shopItemCheckGrabbed), b_+24);
  if (!(F & FZ)) { CYCT(b_+24, b_+25); ret_effect(gb); return; } CYC(b_+24, b_+25);
  TAIL(shopItemState4);
}

void shopItemState3_hook(GB *gb) {
  BASE(shopItemState3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(shopItemPrices));
  CYC(b_+6, b_+7); shopItem_addAToHl(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(b_+8, removeRupeeValue_hook, SYM(removeRupeeValue), b_+11);
  CYC(b_+11, b_+13); E = 0x42;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+17); SET_HL(SYM(shopItemTreasureToGive));
  CYC(b_+17, b_+18); shopItem_addDoubleIndex(gb, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); C = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_cp(gb, 0);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto L_4425; } CYC(b_+22, b_+24);
  CALL_C(b_+24, getRandomRingOfGivenTier_hook, SYM(getRandomRingOfGivenTier), b_+27);
L_4425:
  CALL_C(b_+27, giveTreasure_hook, SYM(giveTreasure), b_+30);
  CYC(b_+30, b_+32); E = 0x44;
  CYC(b_+32, b_+34); A = 5;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+37); A = 4;
  CYC(b_+37, b_+40); mem_wr(gb, wLinkForceState, A);
  CYC(b_+40, b_+42); A = 1;
  CYC(b_+42, b_+45); mem_wr(gb, wcc50, A);
  CYC(b_+45, b_+47); E = 0x42;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+51); SET_HL(SYM(shopItemTextTable));
  CYC(b_+51, b_+52); shopItem_addAToHl(gb, b_+52);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+54); C = A;
  CYC(b_+54, b_+55); alu_or(gb, A);
  CYC(b_+55, b_+57); B = 0;
  if (!(F & FZ)) { CYCT(b_+57, b_+60); showText_hook(gb); return; } CYC(b_+57, b_+60);
  CYC(b_+60, b_+61); ret_effect(gb);
}

void shopItemState4_hook(GB *gb) {
  BASE(shopItemState4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); E = 0x4b;
  CYC(b_+3, b_+5); L = 0x70;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); E = 0x4d;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); L = 0x4f;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0);
  CYC(b_+15, b_+17); L = 0x44;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 1);
  CALL_C(b_+19, shopItemUpdateRupeeDisplay_hook, SYM(shopItemUpdateRupeeDisplay), b_+22);
  CALL_C(b_+22, objectSetVisible83_hook, SYM(objectSetVisible83), b_+25);
  CYC(b_+25, b_+28); TAIL(dropLinkHeldItem);
}

void shopItemClearRupeeDisplay_hook(GB *gb) {
  BASE(shopItemClearRupeeDisplay);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, shopItemGetTilesForRupeeDisplay_hook, SYM(shopItemGetTilesForRupeeDisplay), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  CYC(b_+5, b_+7); A = 3;
  CYC(b_+7, b_+8); shopItem_addAToHl(gb, b_+8);
  CYC(b_+8, b_+10); A = 0x20;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
  CYC(b_+16, b_+18); shopItemUpdateRupeeDisplay_tail(gb);
}

void shopItemUpdateRupeeDisplay_hook(GB *gb) {
  BASE(shopItemUpdateRupeeDisplay);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, shopItemGetTilesForRupeeDisplay_hook, SYM(shopItemGetTilesForRupeeDisplay), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  shopItemUpdateRupeeDisplay_tail(gb);
}

void shopItemGetTilesForRupeeDisplay_hook(GB *gb) {
  BASE(shopItemGetTilesForRupeeDisplay);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+7); SET_HL(b_+71);
  CYC(b_+7, b_+8); shopItem_addDoubleIndex(gb, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+11); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; } CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); push_effect(gb, DE);
  CYC(b_+13, b_+14); E = A;
  CYC(b_+14, b_+15); D = mem_rd(gb, HL);
  CYC(b_+15, b_+16); A = C;
  CYC(b_+16, b_+19); SET_HL(SYM(shopItemPrices));
  CYC(b_+19, b_+20); shopItem_addAToHl(gb, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CALL_C(b_+21, getRupeeValue_hook, SYM(getRupeeValue), b_+24);
  CYC(b_+24, b_+27); SET_HL(wEnemyPlacement);
  CYC(b_+27, b_+28); mem_wr(gb, HL, E);
  CYC(b_+28, b_+29); L = alu_inc8(gb, L);
  CYC(b_+29, b_+30); mem_wr(gb, HL, D);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+33); E = GV(0x06, 0x03);
  CYC(b_+33, b_+35); D = 0x30;
  CYC(b_+35, b_+37); A = 2;
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+39); A = B;
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+40, b_+42); goto L_4477; } CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); L = alu_dec8(gb, L);
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+44, b_+45); L = alu_inc8(gb, L);
  CYC(b_+45, b_+48); push_effect(gb, b_+48); shopItemDrawDigit(gb);
L_4477:
  CYC(b_+48, b_+49); A = C;
  CYC(b_+49, b_+51); A = alu_swap(gb, A);
  CYC(b_+51, b_+54); push_effect(gb, b_+54); shopItemDrawDigit(gb);
  CYC(b_+54, b_+55); A = C;
  CYC(b_+55, b_+58); push_effect(gb, b_+58); shopItemDrawDigit(gb);
  CYC(b_+58, b_+61); SET_HL(wEnemyPlacement);
  CYC(b_+61, b_+62); SET_DE(pop_effect(gb));
  CYC(b_+62, b_+63); alu_scf(gb);
  CYC(b_+63, b_+64); ret_effect(gb);
}

void shopItemCheckGrabbed_hook(GB *gb) {
  BASE(shopItemCheckGrabbed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+3, b_+5); alu_and(gb, 3);
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_44f5; } CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = 0x71;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_sub(gb, 0x0d);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+15); alu_add(gb, 0x1a);
  CYC(b_+15, b_+18); SET_HL(w1Link_xh);
  CYC(b_+18, b_+19); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+19, b_+21); goto L_44f5; } CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = B;
  CYC(b_+22, b_+23); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+23, b_+25); goto L_44f5; } CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); L = 0x0b;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); alu_cp(gb, 0x3d);
  if (!(F & FC)) { CYCT(b_+30, b_+32); goto L_44f5; } CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); L = 8;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); alu_or(gb, A);
  CYC(b_+36, b_+37); ret_effect(gb); return;
L_44f5:
  CYC(b_+37, b_+38); alu_or(gb, D);
  CYC(b_+38, b_+39); ret_effect(gb);
}
