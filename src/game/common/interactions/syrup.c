#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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

void interactionCode5f__runState_hook(GB *gb) {
  BASE(interactionCode5f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (syrup_jump_table(gb));
    if (jt_ == b_+24) { goto state0; }
    else if (jt_ == b_+50) { goto state1; }
    else if (jt_ == b_+168) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+24, b_+26); A = 1;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CALL_C(b_+27, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+30);
  CALL_C(b_+30, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+33);
  CYC(b_+33, b_+35); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x12);
  CYC(b_+37, b_+38); L = alu_inc8(gb, L);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 7);
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+42, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+45);
  CYC(b_+45, b_+48); SET_HL((SYM(dog_subid01__state0) + 36));
  CYC(b_+48, b_+50); goto set_script_and_state2;

state1:
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+54, b_+55); ret_effect(gb); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+56); alu_xor(gb, A);
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); A = 0x81;
  CYC(b_+59, b_+62); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+62, b_+65); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+65, b_+66); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+66, b_+68); goto talk_without_item; }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+71); A = mem_rd(gb, w1Link_relatedObj2 + 1);
  CYC(b_+71, b_+72); H = A;
  CYC(b_+72, b_+74); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+77, b_+78); A = mem_rd(gb, HL);
  CYC(b_+78, b_+79); push_effect(gb, AF);
  CYC(b_+79, b_+80); B = A;
  CYC(b_+80, b_+82); alu_sub(gb, 7);
  CYC(b_+82, b_+84); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+84, b_+85); mem_wr(gb, DE, A);
  CYC(b_+85, b_+86); A = B;
  CYC(b_+86, b_+89); SET_HL(SYM(shopItemPrices));
  CYC(b_+89, b_+90); push_effect(gb, b_+90);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CALL_C(b_+91, cpRupeeValue_hook, SYM(cpRupeeValue), b_+94);
  CYC(b_+94, b_+97); mem_wr(gb, wShopHaveEnoughRupees, A);
  CYC(b_+97, b_+100); mem_wr(gb, wcbad, A);
  CYC(b_+100, b_+101); SET_AF(pop_effect(gb));
  CYC(b_+101, b_+103); alu_cp(gb, 7);
  if (F & FZ) { CYCT(b_+103, b_+105); goto check_potion; }
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+107); alu_cp(gb, 9);
  if (F & FZ) { CYCT(b_+107, b_+109); goto check_potion; }
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+111); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(b_+111, b_+113); goto check_bombchus; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+116); A = mem_rd(gb, wNumGashaSeeds);
  CYC(b_+116, b_+118); goto check_quantity;

check_bombchus:
  CYC(b_+118, b_+121); A = mem_rd(gb, wNumBombchus);
check_quantity:
  CYC(b_+121, b_+123); alu_cp(gb, 0x99);
  CYC(b_+123, b_+125); A = 1;
  if (!(F & FC)) { CYCT(b_+125, b_+127); goto set_can_purchase; }
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+129); goto can_purchase;

check_potion:
  CYC(b_+129, b_+131); A = 0x2f;
  CALL_C(b_+131, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+134);
  CYC(b_+134, b_+136); A = 1;
  if (F & FC) { CYCT(b_+136, b_+138); goto set_can_purchase; }
  CYC(b_+136, b_+138);
can_purchase:
  CYC(b_+138, b_+139); alu_xor(gb, A);
set_can_purchase:
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+141, b_+142); mem_wr(gb, DE, A);
  CYC(b_+142, b_+145); SET_HL((SYM(dog_subid01__state1) + 8));
  CYC(b_+145, b_+147); goto set_script_and_state2;

talk_without_item:
  CALL_C(b_+147, shopkeeperCheckAllItemsBought_hook, SYM(shopkeeperCheckAllItemsBought), b_+150);
  if (F & FZ) { CYCT(b_+150, b_+152); goto show_welcome_text; }
  CYC(b_+150, b_+152);
  CYC(b_+152, b_+155); SET_HL((SYM(dog_subid01__state1) + 4));
  CYC(b_+155, b_+157); goto set_script_and_state2;
show_welcome_text:
  CYC(b_+157, b_+160); SET_HL(SYM(dog_subid01__state1));
set_script_and_state2:
  CYC(b_+160, b_+162); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+162, b_+164); A = 2;
  CYC(b_+164, b_+165); mem_wr(gb, DE, A);
  CYC(b_+165, b_+168); interactionSetScript_hook(gb);
  return;

state2:
  CALL_C(b_+168, interactionRunScript_hook, SYM(interactionRunScript), b_+171);
  if (!(F & FC)) { CYCT(b_+171, b_+172); ret_effect(gb); return; }
  CYC(b_+171, b_+172);
  CYC(b_+172, b_+173); alu_xor(gb, A);
  CYC(b_+173, b_+176); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+176, b_+178); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+178, b_+179); A = mem_rd(gb, DE);
  CYC(b_+179, b_+180); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+180, b_+182); goto goto_state1; }
  CYC(b_+180, b_+182);
  CYC(b_+182, b_+183); A = alu_inc8(gb, A);
  CYC(b_+183, b_+185); C = 3;
  if (!(F & FZ)) { CYCT(b_+185, b_+187); goto set_held_item_state; }
  CYC(b_+185, b_+187);
  CYC(b_+187, b_+189); C = 4;
set_held_item_state:
  CYC(b_+189, b_+190); alu_xor(gb, A);
  CYC(b_+190, b_+191); mem_wr(gb, DE, A);
  CYC(b_+191, b_+193); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+193, b_+194); A = mem_rd(gb, DE);
  CYC(b_+194, b_+195); H = A;
  CYC(b_+195, b_+197); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+197, b_+198); mem_wr(gb, HL, C);
  CALL_C(b_+198, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+201);
goto_state1:
  CYC(b_+201, b_+203); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+203, b_+205); A = 1;
  CYC(b_+205, b_+206); mem_wr(gb, DE, A);
  CYC(b_+206, b_+207); ret_effect(gb);
}

void interactionCode5f_hook(GB *gb) {
  BASE(interactionCode5f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(SYM(checkReloadShopItemTiles));
  CYC(b_+3, b_+5); E = 9;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CALL_C(b_+8, interactionCode5f__runState_hook, b_+14, b_+11);
  CYC(b_+11, b_+14); TAIL(interactionAnimateAsNpc);
}

void interactionCode5f__afterCall6ea1_hook(GB *gb) {
  BASE(interactionCode5f);
  CYC(b_+11, b_+14); TAIL(interactionAnimateAsNpc);
}
