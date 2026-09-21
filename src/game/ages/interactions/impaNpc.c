#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t impa_npc_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void impaNpc_subid00_hook(GB *gb);
void impaNpc_setScriptAndInitialize_hook(GB *gb);
void impaNpc_determineTextAndPositionInHouse_hook(GB *gb);
void impaNpc_subid01_hook(GB *gb);
void impaNpc_setTextIndexAndLoadGenericNpcScript_hook(GB *gb);
void impaNpc_subid02_hook(GB *gb);
void impaNpc_runScriptAndFaceLink_hook(GB *gb);
void impaNpc_subid03_hook(GB *gb);
void impaNpc_faceLinkIfClose_hook(GB *gb);
void getImpaNpcState_hook(GB *gb);

static void impa_set_text_and_position(GB *gb) {
  BASE(impaNpc_determineTextAndPositionInHouse);
  CYC(b_+83, b_+85); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+88, b_+89); A = B;
  CYC(b_+89, b_+90); mem_wr(gb, DE, A);
  CYC(b_+90, b_+92); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+92, b_+93); A = C;
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, b_+96); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+96, b_+98); A = 2;
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+102); SET_HL((SYM(interactionCode4a__initSubid09) + 17));
  CYC(b_+102, b_+103); alu_xor(gb, A);
  CYC(b_+103, b_+104); ret_effect(gb);
}

void interactionCode4f_hook(GB *gb) {
  BASE(interactionCode4f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (impa_npc_jump_table(gb));
    if (jt_ == SYM(impaNpc_subid00) && hook_enabled_at(gb, SYM(impaNpc_subid00))) { impaNpc_subid00_hook(gb); return; }
    else if (jt_ == SYM(impaNpc_subid01) && hook_enabled_at(gb, SYM(impaNpc_subid01))) { impaNpc_subid01_hook(gb); return; }
    else if (jt_ == SYM(impaNpc_subid02) && hook_enabled_at(gb, SYM(impaNpc_subid02))) { impaNpc_subid02_hook(gb); return; }
    else if (jt_ == SYM(impaNpc_subid03) && hook_enabled_at(gb, SYM(impaNpc_subid03))) { impaNpc_subid03_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void impaNpc_subid00_hook(GB *gb) {
  BASE(impaNpc_subid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; }
  CYC(b_+3, b_+5);
state1:
  CALL_C(b_+5, interactionRunScript_hook, SYM(interactionRunScript), b_+8);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+12, b_+14); goto animate; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_cp(gb, 9);
  if (!(F & FZ)) CALL_C_CC(b_+16, impaNpc_faceLinkIfClose_hook, SYM(impaNpc_faceLinkIfClose), b_+19);
  else CYC(b_+16, b_+19);
animate:
  CYC(b_+19, b_+22); interactionAnimateAsNpc_hook(gb);
  return;
state0:
  CYC(b_+22, b_+25); SET_HL(wRoomLayout + 0x22);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x45);
  CALL_C(b_+27, getImpaNpcState_hook, SYM(getImpaNpcState), b_+30);
  CYC(b_+30, b_+32); alu_bit(gb, 7, B);
  if (!(F & FZ)) { CYCT(b_+32, b_+35); interactionDelete_hook(gb); return; }
  CYC(b_+32, b_+35);
  CALL_C(b_+35, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+38);
  if (F & FZ) { CYCT(b_+38, b_+40); goto choose; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); A = 9;
choose:
  CYC(b_+42, b_+43); alu_add(gb, B);
  CALL_C(b_+43, impaNpc_determineTextAndPositionInHouse_hook, SYM(impaNpc_determineTextAndPositionInHouse), SYM(impaNpc_setScriptAndInitialize));
  TAIL(impaNpc_setScriptAndInitialize);
}

void impaNpc_setScriptAndInitialize_hook(GB *gb) {
  BASE(impaNpc_setScriptAndInitialize);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionSetScript_hook, SYM(interactionSetScript), b_+3);
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6);
  CALL_C(b_+6, interactionIncState_hook, SYM(interactionIncState), b_+9);
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_TEXT_ID + 1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 1);
  CALL_C(b_+13, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+16);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CALL_C(b_+19, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+22);
  CYC(b_+22, b_+25); TAIL(objectSetVisiblec2);
}

void impaNpc_determineTextAndPositionInHouse_hook(GB *gb) {
  BASE(impaNpc_determineTextAndPositionInHouse);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (impa_npc_jump_table(gb));
    if (jt_ == b_+40) {
      CYC(b_+40, b_+41); SET_HL(pop_effect(gb));
      CYC(b_+41, b_+44); interactionDelete_hook(gb);
      return;
    }
    else if (jt_ == b_+44) {
      CYC(b_+44, b_+47); SET_BC(0x3838);
      CYC(b_+47, b_+49); A = 0x20;
      CYC(b_+49, b_+51); impa_set_text_and_position(gb);
      return;
    }
    else if (jt_ == b_+51) {
      CYC(b_+51, b_+54); SET_BC((SYM(interactionCode50__state1) + 7));
      CYC(b_+54, b_+56); A = 0x21;
      CYC(b_+56, b_+59); push_effect(gb, b_+59); impa_set_text_and_position(gb);
      CYC(b_+59, b_+60); mem_wr(gb, DE, A);
      CYC(b_+60, b_+63); SET_HL(b_+96);
      CYC(b_+63, b_+64); ret_effect(gb);
      return;
    }
    else if (jt_ == b_+64) {
      CYC(b_+64, b_+67); SET_BC(0x2868);
      CYC(b_+67, b_+69); A = 0x22;
      CYC(b_+69, b_+71); impa_set_text_and_position(gb);
      return;
    }
    else if (jt_ == b_+71) {
      CYC(b_+71, b_+74); SET_BC(0x2868);
      CYC(b_+74, b_+76); A = 0x2c;
      CYC(b_+76, b_+78); impa_set_text_and_position(gb);
      return;
    }
    else if (jt_ == b_+78) {
      CYC(b_+78, b_+81); SET_BC(0x2868);
      CYC(b_+81, b_+83); A = 0x23;
      impa_set_text_and_position(gb);
      return;
    }
    else { HANDOFF(HL); }
  } while (0);
}

void impaNpc_subid01_hook(GB *gb) {
  BASE(impaNpc_subid01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); impaNpc_runScriptAndFaceLink_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getImpaNpcState_hook, SYM(getImpaNpcState), b_+8);
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+11); alu_cp(gb, 7);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); interactionDelete_hook(gb); return; }
  CYC(b_+11, b_+14);
  CALL_C(b_+14, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+17);
  CYC(b_+17, b_+19); A = 0x2b;
  if (F & FZ) { CYCT(b_+19, b_+21); impaNpc_setTextIndexAndLoadGenericNpcScript_hook(gb); return; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = 0x2e;
  TAIL(impaNpc_setTextIndexAndLoadGenericNpcScript);
}

void impaNpc_setTextIndexAndLoadGenericNpcScript_hook(GB *gb) {
  BASE(impaNpc_setTextIndexAndLoadGenericNpcScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+5, b_+7); A = 2;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+11); SET_HL((SYM(interactionCode4a__initSubid09) + 17));
  CYC(b_+11, b_+14); TAIL(impaNpc_setScriptAndInitialize);
}

void impaNpc_subid02_hook(GB *gb) {
  BASE(impaNpc_subid02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); impaNpc_runScriptAndFaceLink_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getImpaNpcState_hook, SYM(getImpaNpcState), b_+8);
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+11); alu_cp(gb, 8);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); interactionDelete_hook(gb); return; }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); A = 0x2f;
  CYC(b_+16, b_+18); TAIL(impaNpc_setTextIndexAndLoadGenericNpcScript);
}

void impaNpc_runScriptAndFaceLink_hook(GB *gb) {
  BASE(impaNpc_runScriptAndFaceLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CALL_C(b_+3, impaNpc_faceLinkIfClose_hook, SYM(impaNpc_faceLinkIfClose), b_+6);
  CYC(b_+6, b_+9); TAIL(interactionAnimateAsNpc);
}

void impaNpc_subid03_hook(GB *gb) {
  BASE(impaNpc_subid03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); impaNpc_runScriptAndFaceLink_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getImpaNpcState_hook, SYM(getImpaNpcState), b_+8);
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+11); alu_cp(gb, 6);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); interactionDelete_hook(gb); return; }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); A = 0x23;
  CYC(b_+16, b_+18); TAIL(impaNpc_setTextIndexAndLoadGenericNpcScript);
}

void impaNpc_faceLinkIfClose_hook(GB *gb) {
  BASE(impaNpc_faceLinkIfClose);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x28;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto no_change; }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+10);
  CYC(b_+10, b_+12); alu_add(gb, 4);
  CYC(b_+12, b_+14); alu_and(gb, 0x18);
  CYC(b_+14, b_+16); A = alu_swap(gb, A);
  CYC(b_+16, b_+17); alu_rlca(gb);
  CYC(b_+17, b_+19); goto update;
no_change:
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
update:
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+25, b_+26); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+26, b_+27); ret_effect(gb); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+31); TAIL(interactionSetAnimation);
}

void getImpaNpcState_hook(GB *gb) {
  BASE(getImpaNpcState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x14;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  CYC(b_+5, b_+7); B = 0xff;
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); B = alu_inc8(gb, B);
  CYC(b_+9, b_+12); A = mem_rd(gb, wGroup0RoomFlags + 0x83);
  CYC(b_+12, b_+13); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); A = 0x11;
  CALL_C(b_+16, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+19);
  CYC(b_+19, b_+21); B = 1;
  if (!(F & FC)) { CYCT(b_+21, b_+22); ret_effect(gb); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); A = 0x11;
  CALL_C(b_+24, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+27);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto saved_nayru; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0x38;
  CALL_C(b_+31, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+34);
  CYC(b_+34, b_+36); B = 4;
  if (!(F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); A = 0x40;
  CALL_C(b_+39, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+42);
  CYC(b_+42, b_+44); alu_bit(gb, 2, A);
  CYC(b_+44, b_+46); B = 2;
  if (F & FZ) { CYCT(b_+46, b_+47); ret_effect(gb); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+48); B = alu_inc8(gb, B);
  CYC(b_+48, b_+49); ret_effect(gb);
  return;
saved_nayru:
  CYC(b_+49, b_+51); A = 0x36;
  CALL_C(b_+51, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+54);
  CYC(b_+54, b_+56); B = 5;
  if (!(F & FC)) { CYCT(b_+56, b_+57); ret_effect(gb); return; }
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+59); A = 0x33;
  CALL_C(b_+59, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+62);
  CYC(b_+62, b_+64); B = 6;
  if (F & FZ) { CYCT(b_+64, b_+65); ret_effect(gb); return; }
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+67); A = 0x3a;
  CALL_C(b_+67, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+70);
  CYC(b_+70, b_+72); B = 7;
  if (F & FZ) { CYCT(b_+72, b_+73); ret_effect(gb); return; }
  CYC(b_+72, b_+73);
  CYC(b_+73, b_+74); B = alu_inc8(gb, B);
  CYC(b_+74, b_+75); ret_effect(gb);
}
