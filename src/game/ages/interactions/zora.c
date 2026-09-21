#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define zoraAnimationTable SYM(zora_subid0B__animationTable)
#define zoraScriptTable SYM(zora_subid12__scriptTable)
#define zoraTextTable SYM(zora_subid1B__textTable)
#define zoraTextIndices SYM(zora_textIndices)

void zora_commonInit__afterCall60d7_hook(GB *gb);
void zora_subid0B__afterCall611f_hook(GB *gb);
void zora_subid12__afterCall6193_hook(GB *gb);
void zora_subid0E__afterCall6206_hook(GB *gb);
void zora_subid1B__afterCall6236_hook(GB *gb);
void zora_commonInit_hook(GB *gb);
void zora_commonInitWithScript_hook(GB *gb);
void zora_getWorldState_hook(GB *gb);

static uint16_t zora_jump_table(GB *gb) {
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

static void zora_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void zora_add_double_index(GB *gb, uint16_t return_address) {
  BASE(zora_subid10);
  CYC(b_+28, b_+29); push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void zora_commonInit__afterCall60d7_hook(GB *gb) {
  BASE(zora_commonInit);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+3, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+6);
  CALL_C(b_+6, interactionIncState_hook, SYM(interactionIncState), b_+9);
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_TEXT_ID + 1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x34);
  CYC(b_+13, b_+16); objectSetVisiblec2_hook(gb);
}

void zora_commonInit_hook(GB *gb) {
  BASE(zora_commonInit);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  zora_commonInit__afterCall60d7_hook(gb);
}

void zora_commonInitWithScript_hook(GB *gb) {
  BASE(zora_commonInitWithScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionSetScript_hook, SYM(interactionSetScript), SYM(zora_commonInit));
  TAIL(zora_commonInit);
}

void zora_getWorldState_hook(GB *gb) {
  BASE(zora_getWorldState);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x27;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  CYC(b_+5, b_+7); B = 0;
  if (F & FZ) {
    CYCT(b_+7, b_+8); ret_effect(gb); return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = 0x30;
  CALL_C(b_+10, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+13);
  CYC(b_+13, b_+15); B = 1;
  if (F & FZ) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+19); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+19, b_+21); alu_bit(gb, 6, A);
  CYC(b_+21, b_+23); B = 2;
  if (!(F & FC)) {
    CYCT(b_+23, b_+24); ret_effect(gb); return;
  }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); B = alu_inc8(gb, B);
  CYC(b_+25, b_+26); ret_effect(gb);
}

void zora_subid0B__afterCall611f_hook(GB *gb) {
  BASE(zora_subid0A);
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x1e);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+24); alu_sub(gb, 0x0a);
  CYC(b_+24, b_+26); A = alu_swap(gb, A);
  CYC(b_+26, b_+27); alu_rrca(gb);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+29); ret_effect(gb);
}

void zora_subid0B_hook(GB *gb) {
  BASE(zora_subid0A);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (zora_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+29) { goto state1; }
    else if (jt_ == b_+40) { goto state2; }
    else if (jt_ == b_+94) { goto state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+12, zora_commonInit_hook, SYM(zora_commonInit), b_+15);
  zora_subid0B__afterCall611f_hook(gb);
  return;

state1:
  CALL_C(b_+29, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+32);
  if (!(F & FZ)) {
    CYCT(b_+32, b_+33); ret_effect(gb); return;
  }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x78);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+37, b_+40); interactionIncState_hook(gb);
  return;

state2:
  CALL_C(b_+40, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+43);
  if (!(F & FZ)) {
    CYCT(b_+43, b_+45); goto decrement_counter2;
  }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x5a);
  CYC(b_+47, b_+49); A = 2;
  CALL_C(b_+49, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+52);
  CYC(b_+52, b_+55); interactionIncState_hook(gb);
  return;

decrement_counter2:
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+57, b_+58); ret_effect(gb); return;
  }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x14);
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+62, b_+63); A = mem_rd(gb, HL);
  CYC(b_+63, b_+64); A = alu_inc8(gb, A);
  CYC(b_+64, b_+66); alu_and(gb, 7);
  CYC(b_+66, b_+67); mem_wr(gb, HL, A);
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+69, b_+70); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+70, b_+73); SET_HL(zoraAnimationTable);
  CYC(b_+73, b_+74); push_effect(gb, b_+74);
  zora_add_a_to_hl(gb);
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CYC(b_+75, b_+78); interactionSetAnimation_hook(gb);
  return;

state3:
  CALL_C(b_+94, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+97);
  if (!(F & FZ)) {
    CYCT(b_+97, b_+99); goto update_speed_z;
  }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+102); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+102, b_+104); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);

update_speed_z:
  CYC(b_+104, b_+106); C = 0x20;
  CALL_C(b_+106, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+109);
  if (!(F & FZ)) {
    CYCT(b_+109, b_+110); ret_effect(gb); return;
  }
  CYC(b_+109, b_+110);
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+112, b_+114); A = 0x80;
  CYC(b_+114, b_+115); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0xfe);
  CYC(b_+117, b_+118); ret_effect(gb);
}

void zora_subid0D_hook(GB *gb) {
  BASE(zora_subid0C);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); goto state0;
  }
  CYC(b_+3, b_+5);

  CYC(b_+5, b_+7); C = 0x20;
  CALL_C(b_+7, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+10);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+11); ret_effect(gb); return;
  }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, interactionRunScript_hook, SYM(interactionRunScript), b_+14);
  if (!(F & FC)) {
    CYCT(b_+14, b_+16); goto animate;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+19, b_+21); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);

animate:
  CYC(b_+21, b_+24); interactionAnimate_hook(gb);
  return;

state0:
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+26, b_+28); A = 0x3c;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_cp(gb, 0x0c);
  CYC(b_+34, b_+37); SET_HL((SYM(interactiond7_essence__state3) + 9));
  if (F & FZ) {
    CYCT(b_+37, b_+39); TAIL(zora_commonInitWithScript);
  }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+42); SET_HL(SYM(interactiond7_essence__playCirclingSound));
  CYC(b_+42, b_+44); zora_commonInitWithScript_hook(gb);
}

void zora_subid12__afterCall6193_hook(GB *gb) {
  BASE(zora_subid10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+13, objectSetVisible82_hook, SYM(objectSetVisible82), b_+16);
  CALL_C(b_+16, interactionIncState_hook, SYM(interactionIncState), b_+19);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_sub(gb, 0x10);
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+28); SET_HL(zoraScriptTable);
  zora_add_double_index(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); H = mem_rd(gb, HL);
  CYC(b_+31, b_+32); L = A;
  CALL_C(b_+32, interactionSetScript_hook, SYM(interactionSetScript), b_+35);
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+37); push_effect(gb, b_+37);
  do { uint16_t jt_ = (zora_jump_table(gb));
    if (jt_ == b_+43) { goto subid10; }
    else if (jt_ == b_+67) { goto subid11; }
    else if (jt_ == b_+75) { goto subid12; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid10:
  CALL_C(b_+43, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+46);
  CYC(b_+46, b_+48); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+48, b_+51); TAIL(interactionDelete);
  }
  CYC(b_+48, b_+51);
  CYC(b_+51, b_+54); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+54, b_+56); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(b_+56, b_+59); TAIL(interactionDelete);
  }
  CYC(b_+56, b_+59);
  CYC(b_+59, b_+61); A = 3;
  CALL_C(b_+61, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+64);
  CYC(b_+64, b_+67); interactionIncState_hook(gb);
  return;

subid11:
  CALL_C(b_+67, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+70);
  if (!(F & FZ)) {
    CYCT(b_+70, b_+73); TAIL(interactionDelete);
  }
  CYC(b_+70, b_+73);
  CYC(b_+73, b_+75); goto delete_if_flag_set;

subid12:
  CALL_C(b_+75, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+78);
  if (F & FZ) {
    CYCT(b_+78, b_+81); TAIL(interactionDelete);
  }
  CYC(b_+78, b_+81);

delete_if_flag_set:
  CALL_C(b_+81, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+84);
  CYC(b_+84, b_+86); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+86, b_+89); TAIL(interactionDelete);
  }
  CYC(b_+86, b_+89);
  CYC(b_+89, b_+90); ret_effect(gb);
}

void zora_subid12_hook(GB *gb) {
  BASE(zora_subid10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (zora_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+96) { goto state1; }
    else if (jt_ == b_+105) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+10, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+13);
  zora_subid12__afterCall6193_hook(gb);
  return;

state1:
  CALL_C(b_+96, interactionRunScript_hook, SYM(interactionRunScript), b_+99);
  if (F & FC) {
    CYCT(b_+99, b_+102); TAIL(interactionDelete);
  }
  CYC(b_+99, b_+102);
  CYC(b_+102, b_+105); npcFaceLinkAndAnimate_hook(gb);
  return;

state2:
  CALL_C(b_+105, interactionRunScript_hook, SYM(interactionRunScript), b_+108);
  if (F & FC) {
    CYCT(b_+108, b_+111); TAIL(interactionDelete);
  }
  CYC(b_+108, b_+111);
  CYC(b_+111, b_+114); interactionAnimate_hook(gb);
}

void zora_subid0E__afterCall6206_hook(GB *gb) {
  BASE(zora_subid0E);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+14, interactionIncState_hook, SYM(interactionIncState), b_+17);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_TEXT_ID + 1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x34);
  CYC(b_+21, b_+23); A = 0x30;
  CALL_C(b_+23, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+26);
  CYC(b_+26, b_+28); A = 0x33;
  if (F & FZ) {
    CYCT(b_+28, b_+30); goto set_text_id;
  }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); A = 0x34;

set_text_id:
  CYC(b_+32, b_+34); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); alu_xor(gb, A);
  CALL_C(b_+36, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+39);
  CALL_C(b_+39, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+42);
  CYC(b_+42, b_+45); SET_HL((SYM(interactiond7_updateSmallSparkles) + 18));
  CYC(b_+45, b_+48); interactionSetScript_hook(gb);
}

void zora_subid0E_hook(GB *gb) {
  BASE(zora_subid0E);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); goto state0;
  }
  CYC(b_+3, b_+5);

  CALL_C(b_+5, interactionRunScript_hook, SYM(interactionRunScript), b_+8);
  CYC(b_+8, b_+11); interactionAnimateAsNpc_hook(gb);
  return;

state0:
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  zora_subid0E__afterCall6206_hook(gb);
}

void zora_subid1B__afterCall6236_hook(GB *gb) {
  BASE(zora_subid13);
  uint16_t sp0_ = gb->sp;
  CYC(b_+14, b_+16); A = 0x30;
  CALL_C(b_+16, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+19);
  CYC(b_+19, b_+21); B = 0;
  if (F & FZ) {
    CYCT(b_+21, b_+23); goto select_text;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); B = alu_inc8(gb, B);

select_text:
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+29); alu_sub(gb, 0x13);
  CYC(b_+29, b_+30); alu_add(gb, A);
  CYC(b_+30, b_+31); alu_add(gb, B);
  CYC(b_+31, b_+34); SET_HL(zoraTextTable);
  CYC(b_+34, b_+35); push_effect(gb, b_+35);
  zora_add_a_to_hl(gb);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+42); SET_HL((SYM(interactionCodeb6__state6) + 40));
  CYC(b_+42, b_+45); interactionSetScript_hook(gb);
}

void zora_subid1B_hook(GB *gb) {
  BASE(zora_subid13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); goto state0;
  }
  CYC(b_+3, b_+5);

  CALL_C(b_+5, interactionRunScript_hook, SYM(interactionRunScript), b_+8);
  CYC(b_+8, b_+11); npcFaceLinkAndAnimate_hook(gb);
  return;

state0:
  CALL_C(b_+11, zora_commonInit_hook, SYM(zora_commonInit), b_+14);
  zora_subid1B__afterCall6236_hook(gb);
}

void zora_subid0F_hook(GB *gb) {
  BASE(zora_subid00);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); goto state0;
  }
  CYC(b_+3, b_+5);

  CALL_C(b_+5, zora_getWorldState_hook, SYM(zora_getWorldState), b_+8);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_add(gb, A);
  CYC(b_+12, b_+13); alu_add(gb, A);
  CYC(b_+13, b_+14); alu_add(gb, B);
  CYC(b_+14, b_+17); SET_HL(zoraTextIndices);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  zora_add_a_to_hl(gb);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, interactionRunScript_hook, SYM(interactionRunScript), b_+25);
  CYC(b_+25, b_+28); npcFaceLinkAndAnimate_hook(gb);
  return;

state0:
  CALL_C(b_+28, zora_getWorldState_hook, SYM(zora_getWorldState), b_+31);
  CYC(b_+31, b_+32); A = B;
  CYC(b_+32, b_+33); alu_or(gb, A);
  CYC(b_+33, b_+35); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  if (!(F & FZ)) {
    CYCT(b_+36, b_+38); goto initialize;
  }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); alu_cp(gb, 6);
  if (!(F & FC)) {
    CYCT(b_+40, b_+43); TAIL(interactionDelete);
  }
  CYC(b_+40, b_+43);

initialize:
  CYC(b_+43, b_+46); SET_HL((SYM(interactionCodeb6__state6) + 40));
  TAIL(zora_commonInitWithScript);
}

void zora_subid00_hook(GB *gb) {
  zora_subid0F_hook(gb);
}

void zora_subid0A_hook(GB *gb) {
  zora_subid0B_hook(gb);
}

void zora_subid0C_hook(GB *gb) {
  zora_subid0D_hook(gb);
}

void zora_subid10_hook(GB *gb) {
  zora_subid12_hook(gb);
}

void zora_subid13_hook(GB *gb) {
  zora_subid1B_hook(gb);
}

void interactionCodeab_hook(GB *gb) {
  BASE(interactionCodeab);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = zora_jump_table(gb);
  if (target == SYM(zora_subid00)) { zora_subid00_hook(gb); return; }
  if (target == SYM(zora_subid0A)) { zora_subid0A_hook(gb); return; }
  if (target == SYM(zora_subid0C)) { zora_subid0C_hook(gb); return; }
  if (target == SYM(zora_subid0E)) { zora_subid0E_hook(gb); return; }
  if (target == SYM(zora_subid10)) { zora_subid10_hook(gb); return; }
  TAIL(zora_subid13); // target == 0x6228
}
