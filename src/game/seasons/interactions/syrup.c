#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/interactions/syrup.s.
// INTERAC_SYRUP
// Variables:
// var37: Item being bought
// var38: Set to 1 if Link can't purchase an item (because he has too many of it)
// var3a: "Return value" from purchase script (if $ff, the purchase failed)
// var3b: Object index of item that Link is holding

static uint16_t syrup_jump_table(GB *gb) {
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

static void syrup_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void syrup_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode43@runState
static void syrup_runState(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (syrup_jump_table(gb));
    if (jt_ == b_+19) goto state0;
    if (jt_ == b_+55) goto state1;
    if (jt_ == b_+173) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+19, b_+21); A = 0x01;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+25);
  CALL_C(b_+25, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+28);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x12);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x07);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+37, s_objectAddToAButtonSensitiveObjectList, SYM(objectAddToAButtonSensitiveObjectList), b_+40);
  CALL_C(b_+40, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+43);
  CYC(b_+43, b_+45); alu_and(gb, 0x40);
  CYC(b_+45, b_+48); SET_HL(SYM(syrupScript_notTradedMushroomYet));
  if (F & FZ) { CYCT(b_+48, b_+50); goto L_6d4d; }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+53); SET_HL(SYM(syrupScript_spawnShopItems));
L_6d4d:
  CYC(b_+53, b_+55);
  goto setScriptAndGotoState2;
state1:
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+59); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+59); return; }
  CYC(b_+59, b_+60);
  CYC(b_+60, b_+61); alu_xor(gb, A);
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+64); A = 0x81;
  CYC(b_+64, b_+67); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+67, b_+70); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+70, b_+71); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+71, b_+73); goto talkToSyrupWithoutItem; }
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+76); A = mem_rd(gb, w1Link_relatedObj2 + 0x01);
  CYC(b_+76, b_+77); H = A;
  CYC(b_+77, b_+79); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+82); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+84); push_effect(gb, AF);
  CYC(b_+84, b_+85); B = A;
  CYC(b_+85, b_+87); alu_sub(gb, 0x07);
  CYC(b_+87, b_+89); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+89, b_+90); mem_wr(gb, DE, A);
  CYC(b_+90, b_+91); A = B;
  CYC(b_+91, b_+94); SET_HL(SYM(shopItemPrices));
  CYC(b_+94, b_+95); syrup_add_a_to_hl(gb, b_+95);
  CYC(b_+95, b_+96); A = mem_rd(gb, HL);
  CALL_C(b_+96, s_cpRupeeValue, SYM(cpRupeeValue), b_+99);
  CYC(b_+99, b_+101); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  CYC(b_+102, b_+105); mem_wr(gb, wcbad, A);
  CYC(b_+105, b_+106); SET_AF(pop_effect(gb));
  CYC(b_+106, b_+108); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+108, b_+110); goto checkPotion; }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(b_+112, b_+114); goto checkPotion; }
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+116); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(b_+116, b_+118); goto checkBombchus; }
  CYC(b_+116, b_+118);
  CYC(b_+118, b_+121); A = mem_rd(gb, wNumGashaSeeds);
  CYC(b_+121, b_+123);
  goto checkQuantity;
checkBombchus:
  CYC(b_+123, b_+126); A = mem_rd(gb, wNumBombchus);
checkQuantity:
  CYC(b_+126, b_+128); alu_cp(gb, 0x99);
  CYC(b_+128, b_+130); A = 0x01;
  if (!(F & FC)) { CYCT(b_+130, b_+132); goto setCanPurchase; }
  CYC(b_+130, b_+132);
  CYC(b_+132, b_+134);
  goto canPurchase;
checkPotion:
  CYC(b_+134, b_+136); A = 0x2f;
  CALL_C(b_+136, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+139);
  CYC(b_+139, b_+141); A = 0x01;
  if (F & FC) { CYCT(b_+141, b_+143); goto setCanPurchase; }
  CYC(b_+141, b_+143);
canPurchase:
  CYC(b_+143, b_+144); alu_xor(gb, A);
setCanPurchase:
  CYC(b_+144, b_+146); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+146, b_+147); mem_wr(gb, DE, A);
  CYC(b_+147, b_+150); SET_HL(SYM(syrupScript_purchaseItem));
  CYC(b_+150, b_+152);
  goto setScriptAndGotoState2;
talkToSyrupWithoutItem:
  CALL_C(b_+152, s_shopkeeperCheckAllItemsBought, SYM(shopkeeperCheckAllItemsBought), b_+155);
  if (F & FZ) { CYCT(b_+155, b_+157); goto showWelcomeText; }
  CYC(b_+155, b_+157);
  CYC(b_+157, b_+160); SET_HL(SYM(syrupScript_showClosedText));
  CYC(b_+160, b_+162);
  goto setScriptAndGotoState2;
showWelcomeText:
  CYC(b_+162, b_+165); SET_HL(SYM(syrupScript_showWelcomeText));
setScriptAndGotoState2:
  CYC(b_+165, b_+167); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+167, b_+169); A = 0x02;
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);
  CYC(b_+170, b_+173);
  TAIL(interactionSetScript);
state2:
  CALL_C(b_+173, s_interactionRunScript, SYM(interactionRunScript), b_+176);
  if (!(F & FC)) { RET_TAKEN(b_+176); return; }
  CYC(b_+176, b_+177);
  CYC(b_+177, b_+178); alu_xor(gb, A);
  CYC(b_+178, b_+181); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+181, b_+183); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+183, b_+184); A = mem_rd(gb, DE);
  CYC(b_+184, b_+185); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+185, b_+187); goto gotoState1; }
  CYC(b_+185, b_+187);
  CYC(b_+187, b_+188); A = alu_inc8(gb, A);
  CYC(b_+188, b_+190); C = 0x03;
  if (!(F & FZ)) { CYCT(b_+190, b_+192); goto L_6dda; }
  CYC(b_+190, b_+192);
  CYC(b_+192, b_+194); C = 0x04;
L_6dda:
  CYC(b_+194, b_+195); alu_xor(gb, A);
  CYC(b_+195, b_+196); mem_wr(gb, DE, A);
  CYC(b_+196, b_+198); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CYC(b_+199, b_+200); H = A;
  CYC(b_+200, b_+202); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+202, b_+203); mem_wr(gb, HL, C);
  CALL_C(b_+203, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+206);
gotoState1:
  CYC(b_+206, b_+208); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+208, b_+210); A = 0x01;
  CYC(b_+210, b_+211); mem_wr(gb, DE, A);
  RET(b_+211); return;
}

void s_interactionCode43_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkReloadShopItemTiles, SYM(checkReloadShopItemTiles), b_+3);
  CALL_L(b_+3, syrup_runState, b_+6);
  CYC(b_+6, b_+9);
  TAIL(interactionAnimateAsNpc);
}
