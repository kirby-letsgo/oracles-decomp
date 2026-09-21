#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t shopkeeper_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

void checkReloadShopItemTiles_hook(GB *gb);
void interactionCode46_hook(GB *gb);
void shopkeeperState0_hook(GB *gb);
void shopkeeperState1_hook(GB *gb);
void shopkeeperState2_hook(GB *gb);
void shopkeeperState3_hook(GB *gb);
void shopkeeperState5_hook(GB *gb);
void shopkeeperState6_hook(GB *gb);
void shopkeeperLoadScript_hook(GB *gb);
void shopkeeperPromptChestGame_hook(GB *gb);
void shopkeeperGotoState1_hook(GB *gb);
void shopkeeperGetItemPrice_hook(GB *gb);
void shopkeeperCloseOpenedChest_hook(GB *gb);
void shopkeeperCheckLinkHasItemAlready_hook(GB *gb);
void shopkeeperCheckAllItemsBought_hook(GB *gb);
void shopkeeperTurnToFaceLink_hook(GB *gb);

void checkReloadShopItemTiles_hook(GB *gb) {
  BASE(checkReloadShopItemTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenVariables);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wAButtonSensitiveObjectListEnd);
  CYC(b_+9, b_+11); alu_bit(gb, 2, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; } CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 2)));
  CYC(b_+14, b_+15); push_effect(gb, DE);
  CYC(b_+15, b_+17); A = 0x11;
  CALL_C(b_+17, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+20);
  CYC(b_+20, b_+21); SET_DE(pop_effect(gb));
  CYC(b_+21, b_+22); ret_effect(gb);
}

void interactionCode46_hook(GB *gb) {
  BASE(interactionCode46);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkReloadShopItemTiles_hook, SYM(checkReloadShopItemTiles), b_+3);
  CYC(b_+3, b_+6); push_effect(gb, b_+6);
  CYC(b_+9, b_+11); E = 0x44;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13); SET_HL(shopkeeper_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(shopkeeperState0)) { shopkeeperState0_hook(gb); break; }
    else if (jt_ == SYM(shopkeeperState1)) { shopkeeperState1_hook(gb); break; }
    else if (jt_ == SYM(shopkeeperState2)) { shopkeeperState2_hook(gb); break; }
    else if (jt_ == SYM(shopkeeperState3)) { shopkeeperState3_hook(gb); break; }
    else if (jt_ == SYM(shopkeeperState5)) { shopkeeperState5_hook(gb); break; }
    else if (jt_ == SYM(shopkeeperState6)) { shopkeeperState6_hook(gb); break; }
    else { hook_continue(gb, HL, gb->sp); return; }
  } while (0);
  CYC(b_+6, b_+9); interactionAnimateAsNpc_hook(gb);
}

void shopkeeperState6_hook(GB *gb) {
  BASE(shopkeeperState6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x71;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_40e9; } CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); SET_HL(w1Link_xh);
  CYC(b_+9, b_+11); E = 0x4d;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+13, b_+14); ret_effect(gb); return; } CYC(b_+13, b_+14);
  CYC(b_+14, b_+17); TAIL(shopkeeperGotoState1);
L_40e9:
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, objectRemoveFromAButtonSensitiveObjectList_hook, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+22);
  CYC(b_+22, b_+24); A = 0x81;
  CYC(b_+24, b_+27); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+27, b_+29); E = 0x44;
  CYC(b_+29, b_+31); A = 2;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+35); shopkeeperTurnToFaceLink_hook(gb);
}

void shopkeeperState2_hook(GB *gb) {
  BASE(shopkeeperState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); shopkeeperPromptChestGame_hook(gb); return; } CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto L_4124; } CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, (w1Link_relatedObj2 + 1));
  CYC(b_+16, b_+17); H = A;
  CYC(b_+17, b_+19); E = 0x7b;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); L = 0x42;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); E = 0x77;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CALL_C(b_+26, shopkeeperGetItemPrice_hook, SYM(shopkeeperGetItemPrice), b_+29);
  CYC(b_+29, b_+31); E = 0x77;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CALL_C(b_+32, shopkeeperCheckLinkHasItemAlready_hook, SYM(shopkeeperCheckLinkHasItemAlready), b_+35);
  CYC(b_+35, b_+38); SET_HL((SYM(interactionCode50__state0__substate0) + 14));
  CYC(b_+38, b_+41); TAIL(shopkeeperLoadScript);
L_4124:
  CALL_C(b_+41, shopkeeperCheckAllItemsBought_hook, SYM(shopkeeperCheckAllItemsBought), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); shopkeeperLoadScript_hook(gb); return; } CYC(b_+44, b_+46);
  CYC(b_+46, b_+48); E = 0x42;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+51); alu_cp(gb, 2);
  CYC(b_+51, b_+54); SET_HL((SYM(interactionCode50__state0__substate0) + 5));
  if (!(F & FZ)) { CYCT(b_+54, b_+56); shopkeeperLoadScript_hook(gb); return; } CYC(b_+54, b_+56);
  CYC(b_+56, b_+59); SET_HL((SYM(interactionCode50__state0__substate0) + 8));
  shopkeeperLoadScript_hook(gb);
}

void shopkeeperLoadScript_hook(GB *gb) {
  BASE(shopkeeperLoadScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+4); A = 4;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+8); interactionSetScript_hook(gb);
}

void shopkeeperPromptChestGame_hook(GB *gb) {
  BASE(shopkeeperPromptChestGame);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0c;
  CALL_C(b_+2, shopkeeperGetItemPrice_hook, SYM(shopkeeperGetItemPrice), b_+5);
  CYC(b_+5, b_+8); SET_HL(GV((SYM(interactionCode50__func_48d0) + 25), 0x48ba));
  CYC(b_+8, b_+10); shopkeeperLoadScript_hook(gb);
}

void shopkeeperState3_hook(GB *gb) {
  BASE(shopkeeperState3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  CYC(b_+5, b_+7); A = 0x0c;
  if (!(F & FZ)) CALL_C_CC(b_+7, shopkeeperGetItemPrice_hook, SYM(shopkeeperGetItemPrice), b_+10); else CYC(b_+7, b_+10);
  CALL_C(b_+10, interactionRunScript_hook, SYM(interactionRunScript), b_+13);
  if (!(F & FC)) { CYCT(b_+13, b_+14); ret_effect(gb); return; } CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+18, b_+20); E = 0x7f;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+22, b_+24); goto L_4173; } CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CALL_C(b_+27, getRandomRingOfGivenTier_hook, SYM(getRandomRingOfGivenTier), b_+30);
  CYC(b_+30, b_+31); B = C;
  CYC(b_+31, b_+33); C = 0;
  CALL_C(b_+33, giveRingToLink_hook, SYM(giveRingToLink), b_+36);
  CYC(b_+36, b_+38); A = 1;
  CYC(b_+38, b_+41); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+41, b_+43); TAIL(shopkeeperGotoState1);
L_4173:
  CYC(b_+43, b_+45); E = 0x7a;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+47, b_+49); shopkeeperGotoState1_hook(gb); return; } CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); A = alu_inc8(gb, A);
  CYC(b_+50, b_+52); C = 4;
  if (F & FZ) { CYCT(b_+52, b_+54); goto L_4185; } CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); C = 3;
  CYC(b_+56, b_+58); A = 0x81;
  CYC(b_+58, b_+61); mem_wr(gb, wDisabledObjects, A);
L_4185:
  CYC(b_+61, b_+62); alu_xor(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+65); E = 0x7b;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); H = A;
  CYC(b_+67, b_+69); L = 0x44;
  CYC(b_+69, b_+70); mem_wr(gb, HL, C);
  CALL_C(b_+70, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), SYM(shopkeeperGotoState1));
  shopkeeperGotoState1_hook(gb);
}

void shopkeeperGotoState1_hook(GB *gb) {
  BASE(shopkeeperGotoState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+4); A = 1;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+8); SET_BC(0x0614);
  CALL_C(b_+8, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+11);
  CYC(b_+11, b_+13); E = 0x42;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  CYC(b_+15, b_+17); A = 3;
  if (F & FZ) { CYCT(b_+17, b_+19); goto L_41a6; } CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 1;
L_41a6:
  CALL_C(b_+21, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+24);
  CYC(b_+24, b_+26); E = 0x71;
  CYC(b_+26, b_+29); objectAddToAButtonSensitiveObjectList_hook(gb);
}

void shopkeeperState5_hook(GB *gb) {
  BASE(shopkeeperState5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(shopkeeper_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+12) { goto L_41ba; }
    else if (jt_ == b_+34) { goto L_41d0; }
    else if (jt_ == b_+131) { goto L_4231; }
    else if (jt_ == b_+152) { goto L_4246; }
    else { HANDOFF(HL); }
  } while (0);
L_41ba:
  CYC(b_+12, b_+14); A = 1;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, getRandomNumber_hook, SYM(getRandomNumber), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 1);
  CYC(b_+20, b_+22); E = 0x79;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, shopkeeperCloseOpenedChest_hook, SYM(shopkeeperCloseOpenedChest), b_+26);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CYC(b_+27, b_+30); mem_wr(gb, wcca2, A);
  CYC(b_+30, b_+32); E = 0x7f;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+34); ret_effect(gb); return;
L_41d0:
  CYC(b_+34, b_+36); E = 0x71;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+38, b_+40); goto L_41de; } CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+45); SET_HL(GV((SYM(interactionCode60__state0) + 28), 0x496e));
  CYC(b_+45, b_+48); TAIL(shopkeeperLoadScript);
L_41de:
  CYC(b_+48, b_+51); A = mem_rd(gb, wcca2);
  CYC(b_+51, b_+52); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+52, b_+53); ret_effect(gb); return; } CYC(b_+52, b_+53);
  CYC(b_+53, b_+55); E = 0x45;
  CYC(b_+55, b_+56); alu_xor(gb, A);
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); A = 0xf1;
  CALL_C(b_+59, findTileInRoom_hook, SYM(findTileInRoom), b_+62);
  CYC(b_+62, b_+65); A = mem_rd(gb, wcca2);
  CYC(b_+65, b_+66); alu_sub(gb, L);
  CYC(b_+66, b_+67); alu_rlca(gb);
  CYC(b_+67, b_+69); alu_xor(gb, 1);
  CYC(b_+69, b_+71); alu_and(gb, 1);
  CYC(b_+71, b_+72); H = D;
  CYC(b_+72, b_+74); L = 0x79;
  CYC(b_+74, b_+75); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+75, b_+77); L = 0x7c;
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto L_4204; } CYC(b_+77, b_+79);
  CYC(b_+79, b_+80); mem_wr(gb, HL, A);
  CYC(b_+80, b_+83); SET_HL(GV((SYM(objectOscillateZ_body) + 15), 0x490d));
  CYC(b_+83, b_+86); TAIL(shopkeeperLoadScript);
L_4204:
  CYC(b_+86, b_+87); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+87, b_+88); mem_wr(gb, HL, A);
  CALL_C(b_+88, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+91);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x60);
  CYC(b_+93, b_+95); L = 0x42;
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x28);
  CYC(b_+97, b_+98); L = alu_inc8(gb, L);
  CYC(b_+98, b_+100); mem_wr(gb, HL, 8);
  CYC(b_+100, b_+102); L = 0x71;
  CYC(b_+102, b_+104); mem_wr(gb, HL, 3);
  CYC(b_+104, b_+106); L = 0x79;
  CYC(b_+106, b_+108); mem_wr(gb, HL, 1);
  CYC(b_+108, b_+110); E = 0x79;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+114); SET_BC(SYM(shopkeeperChestXPositions));
  CALL_C(b_+114, addAToBc_hook, 0x006d, b_+117);
  CYC(b_+117, b_+119); L = 0x4b;
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x20);
  CYC(b_+121, b_+123); L = 0x4d;
  CYC(b_+123, b_+124); A = mem_rd(gb, BC);
  CYC(b_+124, b_+125); mem_wr(gb, HL, A);
  CYC(b_+125, b_+128); SET_HL(GV(SYM(interactionCode56), 0x4921));
  CYC(b_+128, b_+131); TAIL(shopkeeperLoadScript);
L_4231:
  CYC(b_+131, b_+133); E = 0x49;
  CYC(b_+133, b_+134); A = mem_rd(gb, DE);
  CYC(b_+134, b_+136); A = alu_swap(gb, A);
  CYC(b_+136, b_+138); alu_and(gb, 1);
  CYC(b_+138, b_+139); H = D;
  CYC(b_+139, b_+141); L = 0x79;
  CYC(b_+141, b_+142); alu_xor(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+142, b_+144); goto L_4246; } CYC(b_+142, b_+144);
  CALL_C(b_+144, shopkeeperCloseOpenedChest_hook, SYM(shopkeeperCloseOpenedChest), b_+147);
  CYC(b_+147, b_+149); E = 0x45;
  CYC(b_+149, b_+151); A = 3;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
L_4246:
  CALL_C(b_+152, interactionRunScript_hook, SYM(interactionRunScript), b_+155);
  if (!(F & FC)) { CYCT(b_+155, b_+156); ret_effect(gb); return; } CYC(b_+155, b_+156);
  CYC(b_+156, b_+158); E = 0x45;
  CYC(b_+158, b_+159); alu_xor(gb, A);
  CYC(b_+159, b_+160); mem_wr(gb, DE, A);
  shopkeeperGetItemPrice_hook(gb);
}

void shopkeeperGetItemPrice_hook(GB *gb) {
  BASE(shopkeeperGetItemPrice);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(shopItemPrices));
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) { burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb); }
  else { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb); }
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CALL_C(b_+5, cpRupeeValue_hook, SYM(cpRupeeValue), b_+8);
  CYC(b_+8, b_+11); mem_wr(gb, wShootingGalleryccd5, A);
  CYC(b_+11, b_+14); mem_wr(gb, wcbad, A);
  CYC(b_+14, b_+17); SET_HL(wTextNumberSubstitution);
  CYC(b_+17, b_+18); mem_wr(gb, HL, C);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+20); mem_wr(gb, HL, B);
  CYC(b_+20, b_+21); ret_effect(gb);
}

void shopkeeperCloseOpenedChest_hook(GB *gb) {
  BASE(shopkeeperCloseOpenedChest);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wcca2);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+8); A = 0xf1;
  if (F & FZ) { CYCT(b_+8, b_+11); setTile_hook(gb); return; } CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void shopkeeperCheckLinkHasItemAlready_hook(GB *gb) {
  BASE(shopkeeperCheckLinkHasItemAlready);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+2); alu_xor(gb, A);
  CYC(b_+2, b_+4); E = 0x78;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = 0x42;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); E = 0x78;
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; } CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); H = 0xc6;
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+17); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(b_+17, b_+18); ret_effect(gb); return; } CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); alu_cp(gb, 3);
  if (F & FZ) { CYCT(b_+20, b_+22); goto L_42a0; } CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); alu_cp(gb, 0x11);
  if (F & FZ) { CYCT(b_+24, b_+26); goto L_42a0; } CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); alu_cp(gb, 0x12);
  if (F & FZ) { CYCT(b_+28, b_+30); goto L_42a0; } CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(b_+32, b_+34); goto L_42a4; } CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); L = GV(0xb0, 0xaa);
  CYC(b_+36, b_+38); alu_cp(gb, 4);
  if (F & FZ) { CYCT(b_+38, b_+40); goto L_4299; } CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); L = GV(0xaa, 0xa2);
L_4299:
  CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+43, b_+44); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+44, b_+45); ret_effect(gb); return; } CYC(b_+44, b_+45);
L_429c:
  CYC(b_+45, b_+47); A = 1;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); ret_effect(gb); return;
L_42a0:
  CYC(b_+49, b_+51); A = 1;
  CYC(b_+51, b_+53); goto L_42a6;
L_42a4:
  CYC(b_+53, b_+55); A = 0x0e;
L_42a6:
  CALL_C(b_+55, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+58);
  CYC(b_+58, b_+60); E = 0x78;
  if (!(F & FC)) { CYCT(b_+60, b_+61); ret_effect(gb); return; } CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); goto L_429c;
}

void shopkeeperCheckAllItemsBought_hook(GB *gb) {
  BASE(shopkeeperCheckAllItemsBought);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w4StatusBarTileMap);
L_42b1:
  CYC(b_+3, b_+5); L = 0x40;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_42bb; } CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_cp(gb, 0x47);
  if (F & FZ) { CYCT(b_+12, b_+13); ret_effect(gb); return; } CYC(b_+12, b_+13);
L_42bb:
  CYC(b_+13, b_+14); H = alu_inc8(gb, H);
  CYC(b_+14, b_+15); A = H;
  CYC(b_+15, b_+17); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+17, b_+19); goto L_42b1; } CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); SET_HL(GV((SYM(interactionCode50__state0__substate0) + 11), 0x47b5));
  CYC(b_+22, b_+23); alu_or(gb, D);
  CYC(b_+23, b_+24); ret_effect(gb);
}

void shopkeeperTurnToFaceLink_hook(GB *gb) {
  BASE(shopkeeperTurnToFaceLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CYC(b_+3, b_+5); E = 0x49;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CALL_C(b_+6, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+9);
  CYC(b_+9, b_+10); E = alu_dec8(gb, E);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); interactionSetAnimation_hook(gb);
}

void shopkeeperState0_hook(GB *gb) {
  BASE(shopkeeperState0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 1;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+5); E = 0x40;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_or(gb, 0x80);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); A = 0x80;
  CYC(b_+11, b_+14); mem_wr(gb, wcca2, A);
  CALL_C(b_+14, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+17);
  CYC(b_+17, b_+19); E = 0x49;
  CYC(b_+19, b_+21); A = 4;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+25); SET_BC(0x0614);
  CALL_C(b_+25, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+28);
  CYC(b_+28, b_+30); L = 0x42;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto L_405f; } CYC(b_+33, b_+35);
  CYC(b_+35, b_+38); A = mem_rd(gb, wBoughtShopItems1);
  CYC(b_+38, b_+40); alu_and(gb, 0x0f);
  CYC(b_+40, b_+42); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto L_405f; } CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
L_405f:
  CYC(b_+46, b_+48); E = 0x42;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); alu_or(gb, A);
  CYC(b_+50, b_+52); A = 3;
  if (F & FZ) CALL_C_CC(b_+52, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+55); else CYC(b_+52, b_+55);
  CYC(b_+55, b_+57); A = 0x0e;
  CALL_C(b_+57, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+60);
  CYC(b_+60, b_+62); E = 0x71;
  CYC(b_+62, b_+65); objectAddToAButtonSensitiveObjectList_hook(gb);
}

void shopkeeperState1_hook(GB *gb) {
  BASE(shopkeeperState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+5); E = 0x71;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto L_40c5; } CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); E = 0x42;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_or(gb, A);
  CYC(b_+13, b_+16); SET_HL(w1Link_xh);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto L_4084; } CYC(b_+16, b_+18);
L_4084:
  CYC(b_+18, b_+20); L = 0x0b;
  CYC(b_+20, b_+22); E = 0x42;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_and(gb, 1);
  CYC(b_+25, b_+27); C = 0x69;
  CYC(b_+27, b_+28); B = mem_rd(gb, HL);
  CYC(b_+28, b_+30); A = 0x69;
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_4097; } CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); B = 0x27;
  CYC(b_+34, b_+35); C = mem_rd(gb, HL);
  CYC(b_+35, b_+37); A = 0x27;
L_4097:
  CYC(b_+37, b_+38); L = A;
  CYC(b_+38, b_+39); A = C;
  CYC(b_+39, b_+40); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+40, b_+42); goto L_40bf; } CYC(b_+40, b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+46, b_+48); goto L_40bf; } CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); A = 0x81;
  CYC(b_+50, b_+53); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+53, b_+54); A = L;
  CYC(b_+54, b_+57); SET_HL(w1Link_yh);
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+61); SET_BC(0x0606);
  CALL_C(b_+61, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+64);
  CYC(b_+64, b_+66); E = 0x42;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+70); SET_HL(SYM(shopkeeperTheftPreventionScriptTable));
  CYC(b_+70, b_+71); push_effect(gb, b_+71);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC); burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0; burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+72, b_+73); H = mem_rd(gb, HL);
  CYC(b_+73, b_+74); L = A;
  CYC(b_+74, b_+77); TAIL(shopkeeperLoadScript);
L_40bf:
  CYC(b_+77, b_+80); SET_BC(0x0614);
  CYC(b_+80, b_+83); TAIL(objectSetCollideRadii);
L_40c5:
  CYC(b_+83, b_+84); alu_xor(gb, A);
  CYC(b_+84, b_+85); mem_wr(gb, DE, A);
  CALL_C(b_+85, objectRemoveFromAButtonSensitiveObjectList_hook, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+88);
  CALL_C(b_+88, shopkeeperTurnToFaceLink_hook, SYM(shopkeeperTurnToFaceLink), b_+91);
  CYC(b_+91, b_+93); A = 0x81;
  CYC(b_+93, b_+96); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+96, b_+98); E = 0x44;
  CYC(b_+98, b_+100); A = 2;
  CYC(b_+100, b_+101); mem_wr(gb, DE, A);
  CYC(b_+101, b_+102); ret_effect(gb);
}
