#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode81), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode81), (from), (to), true)

static uint16_t tokayShopItem_jump_table(GB *gb) {
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

static void tokayShopItem_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// INTERAC_TOKAY_SHOP_ITEM
void interactionCode81_hook(GB *gb) {
  BASE(interactionCode81);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = tokayShopItem_jump_table(gb);
    if (target == b_+95) goto state1;
  }

  // interactionCode81@state0
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x0a; // >TX_0a00
  CALL_C(b_+13, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+16);
  CALL_C(b_+16, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+19);
  CYC(b_+19, b_+21); A = 0x06;
  CALL_C(b_+21, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+24);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); // [var03] = [subid]
  CYC(b_+28, b_+30); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto initializeItem; } // jr nz
  CYC(b_+30, b_+32);

  // This is the shield; only appears if all other items retrieved.
  CYC(b_+32, b_+34); A = 0x37; // GLOBALFLAG_BOUGHT_BRACELET_FROM_TOKAY
  CALL_C(b_+34, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+37);
  if (F & FZ) { CYCT(b_+37, b_+40); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+37, b_+40);

  CYC(b_+40, b_+43); A = W8(wShieldLevel);
  CYC(b_+43, b_+44); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+44, b_+46); goto initializeItem; } // jr z
  CYC(b_+44, b_+46);

  // [subid] = [var03] = [wShieldLevel] + [subid] - 1
  CYC(b_+46, b_+48); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+48, b_+49); C = A;
  CYC(b_+49, b_+50); C = alu_dec8(gb, C);
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_add(gb, C);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+54); E = alu_inc8(gb, E);
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);

initializeItem:
  CYC(b_+55, b_+58); push_effect(gb, b_+58); goto checkTransformItem;
after1_checkTransformItem:
  if (!(F & FZ)) { CYCT(b_+58, b_+61); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+58, b_+61);
  CYC(b_+61, b_+64); SET_HL((SYM(interactionCode93__subid0Or1) + 8)); // mainScripts.tokayShopItemScript
  CALL_C(b_+64, interactionSetScript_hook, SYM(interactionSetScript), b_+67);
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+69, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+72);
  CALL_C(b_+72, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+75);
  CYC(b_+75, b_+78); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(b_+95, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+98);
  CYC(b_+98, b_+101); push_effect(gb, b_+101); goto checkTransformItem;
after2_checkTransformItem:
  if (!(F & FZ)) CALL_C_CC(b_+101, objectSetInvisible_hook, SYM(objectSetInvisible), b_+104); else CYC(b_+101, b_+104); // call nz
  CALL_C(b_+104, interactionRunScript_hook, SYM(interactionRunScript), b_+107);
  if (!(F & FC)) { RET_TAKEN(b_+107); return; } // ret nc
  CYC(b_+107, b_+108);
  CYC(b_+108, b_+109); alu_xor(gb, A);
  CYC(b_+109, b_+112); W8(wDisabledObjects) = A;
  CYC(b_+112, b_+115); interactionDelete_hook(gb); return; // jp

  // This checks whether to replace the feather/bracelet with the shovel, changing the subid
  // accordingly and initializing the graphics after doing so. Returns nz if item should be deleted.
checkTransformItem:
  CYC(b_+115, b_+116); H = D;
  CYC(b_+116, b_+118); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+118, b_+119); alu_xor(gb, A);
  CYC(b_+119, b_+120); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+120, b_+121); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+121, b_+122); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+122, b_+123); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a

  CYC(b_+123, b_+125); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+127); C = A;
  CYC(b_+127, b_+129); A = 0x19; // TREASURE_SEED_SATCHEL
  CALL_C(b_+129, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+132);
  if (!(F & FC)) { CYCT(b_+132, b_+134); goto checkReplaceWithShovel; } // jr nc
  CYC(b_+132, b_+134);

  // Seed satchel obtained; set var38/var39 based on whether Link can buy the item.
  CYC(b_+134, b_+135); A = C;
  CYC(b_+135, b_+138); SET_HL(b_+80); // @seedsNeededToBuyItems
  CYC(b_+138, b_+139); tokayShopItem_addAToHl_from_rst(gb, b_+139);
  CYC(b_+139, b_+140); A = mem_rd(gb, HL);
  CALL_C(b_+140, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+143);
  if (!(F & FC)) { CYCT(b_+143, b_+145); goto checkReplaceWithShovel; } // jr nc
  CYC(b_+143, b_+145);

  CYC(b_+145, b_+146); A = alu_inc8(gb, A);
  CYC(b_+146, b_+148); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+148, b_+149); mem_wr(gb, DE, A);
  CYC(b_+149, b_+151); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+151, b_+153); goto checkReplaceWithShovel; } // jr c
  CYC(b_+151, b_+153);

  CYC(b_+153, b_+155); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+155, b_+156); mem_wr(gb, DE, A);

checkReplaceWithShovel:
  CYC(b_+156, b_+158); A = 0x15; // TREASURE_SHOVEL
  CALL_C(b_+158, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+161);
  if (!(F & FC)) { CYCT(b_+161, b_+163); goto afterShovelCheck; } // jr nc
  CYC(b_+161, b_+163);
  CYC(b_+163, b_+165); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+165, b_+167); A = 0x01;
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);

afterShovelCheck:
  CYC(b_+168, b_+170); A = 0x01; // TREASURE_SHIELD
  CALL_C(b_+170, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+173);
  if (!(F & FC)) { CYCT(b_+173, b_+175); goto afterShieldCheck; } // jr nc
  CYC(b_+173, b_+175);
  CYC(b_+175, b_+177); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+177, b_+179); A = 0x01;
  CYC(b_+179, b_+180); mem_wr(gb, DE, A);

afterShieldCheck:
  CYC(b_+180, b_+181); A = C;
  CYC(b_+181, b_+183); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+183, b_+185); goto setSubidAndInitGraphics; } // jr nc
  CYC(b_+183, b_+185);

  // The item is the feather or the bracelet.
  // If we've bought the item, it should be deleted.
  CYC(b_+185, b_+186); A = C;
  CYC(b_+186, b_+189); SET_HL(b_+220); // @boughtItemGlobalflags
  CYC(b_+189, b_+190); tokayShopItem_addAToHl_from_rst(gb, b_+190);
  CYC(b_+190, b_+191); A = mem_rd(gb, HL);
  CALL_C(b_+191, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+194);
  if (!(F & FZ)) {
    RET_TAKEN(b_+194);
    if (gb->pc == b_+58 && gb->sp == sp0_) goto after1_checkTransformItem;
    if (gb->pc == b_+101 && gb->sp == sp0_) goto after2_checkTransformItem;
    return;
  } // ret nz
  CYC(b_+194, b_+195);

  // Otherwise, if Link has the item, it should be replaced with the shovel.
  CYC(b_+195, b_+196); A = C;
  CYC(b_+196, b_+199); SET_HL(b_+78); // @initialShopTreasures
  CYC(b_+199, b_+200); tokayShopItem_addAToHl_from_rst(gb, b_+200);
  CYC(b_+200, b_+201); A = mem_rd(gb, HL);
  CYC(b_+201, b_+203); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+203, b_+204); mem_wr(gb, DE, A);

  CALL_C(b_+204, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+207);
  if (!(F & FC)) { CYCT(b_+207, b_+209); goto setSubidAndInitGraphics; } // jr nc
  CYC(b_+207, b_+209);

  // Increment subid by 2, making it a shovel
  CYC(b_+209, b_+210); C = alu_inc8(gb, C);
  CYC(b_+210, b_+211); C = alu_inc8(gb, C);

setSubidAndInitGraphics:
  CYC(b_+211, b_+213); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+213, b_+214); A = C;
  CYC(b_+214, b_+215); mem_wr(gb, DE, A);
  CALL_C(b_+215, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+218);
  CYC(b_+218, b_+219); alu_xor(gb, A);
  RET(b_+219);
  if (gb->pc == b_+58 && gb->sp == sp0_) goto after1_checkTransformItem;
  if (gb->pc == b_+101 && gb->sp == sp0_) goto after2_checkTransformItem;
  return; // ret
}
