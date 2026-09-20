#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(runScriptCommand), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(runScriptCommand), (from), (to), true)

void scriptFunc_checkRoomFlag_hook(GB *gb);
void scriptFunc_popHlAndInc_hook(GB *gb);
void scriptCmd_disableMenu_hook(GB *gb);
void scriptCmd_enableMenu_hook(GB *gb);
void scriptFunc_setLinkCantMove_hook(GB *gb);
void func_0c_4177_hook(GB *gb);
void scriptFunc_setState_hook(GB *gb);
void scriptFunc_restoreActiveObject_hook(GB *gb);
void scriptFunc_loadBcAndDe_hook(GB *gb);
void scriptFunc_initializeObject_hook(GB *gb);
void scriptFunc_4310_hook(GB *gb);
void scriptFunc_getTextIndex_hook(GB *gb);
void scriptCmd_jump_hook(GB *gb);

static uint16_t script_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0c:4000
void runScriptCommand_hook(GB *gb) {
  BASE(runScriptCommand);
  CYC(b_+0, b_+2); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+2, b_+5);
    scriptCmd_jump_hook(gb);
    return;
  }
  CYC(b_+2, b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x7f);
  CYC(b_+8, b_+9); push_effect(gb, b_+9);
  hook_handoff(gb, script_jump_table(gb));
}

// 0c:4103
void scriptCmd_none_hook(GB *gb) {
  BASE(scriptCmd_none);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, SYM(scriptCmd_stopIfItemFlagSet)); ret_effect(gb);
}

// 0c:4105
void scriptCmd_stopIfItemFlagSet_hook(GB *gb) {
  BASE(scriptCmd_stopIfItemFlagSet);
  CYC(b_+0, b_+2); B = 0x20;
  CYC(b_+2, SYM(scriptCmd_stopIfRoomFlag40Set));
  scriptFunc_checkRoomFlag_hook(gb);
}

// 0c:4109
void scriptCmd_stopIfRoomFlag40Set_hook(GB *gb) {
  BASE(scriptCmd_stopIfRoomFlag40Set);
  CYC(b_+0, b_+2); B = 0x40;
  CYC(b_+2, SYM(scriptCmd_stopIfRoomFlag80Set));
  scriptFunc_checkRoomFlag_hook(gb);
}

// 0c:410d
void scriptCmd_stopIfRoomFlag80Set_hook(GB *gb) {
  BASE(scriptCmd_stopIfRoomFlag80Set);
  CYC(b_+0, SYM(scriptFunc_checkRoomFlag)); B = 0x80;
  scriptFunc_checkRoomFlag_hook(gb);
}

// 0c:410f
void scriptFunc_checkRoomFlag_hook(GB *gb) {
  BASE(scriptFunc_checkRoomFlag);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+4); alu_and(gb, B);
  if (F & FZ) {
    CYCT(b_+4, b_+7);
    scriptFunc_popHlAndInc_hook(gb);
    return;
  }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); SET_HL(pop_effect(gb));
  CYC(b_+8, b_+11); SET_HL(SYM(stubScript));
  CYC(b_+11, b_+12); alu_scf(gb);
  CYC(b_+12, SYM(scriptCmd_showPasswordScreen)); ret_effect(gb);
}

static void scriptCmd_showPasswordScreen_finish(GB *gb) {
  BASE(scriptCmd_showPasswordScreen);
  CYC(b_+40, b_+41); SET_HL(pop_effect(gb));
  CYC(b_+41, b_+42); alu_xor(gb, A);
  CYC(b_+42, SYM(scriptCmd_disableInput)); ret_effect(gb);
}

static void scriptCmd_showPasswordScreen_openSecretMenu(GB *gb, uint16_t sp0_) {
  BASE(scriptCmd_showPasswordScreen);
  CALL_C(b_+25, openSecretInputMenu_hook, SYM(openSecretInputMenu), b_+28);
  CYC(b_+28, b_+30);
  scriptCmd_showPasswordScreen_finish(gb);
}

static void scriptCmd_showPasswordScreen_askForSecret(GB *gb, uint16_t sp0_) {
  BASE(scriptCmd_showPasswordScreen);
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+25); alu_or(gb, 0x80);
  scriptCmd_showPasswordScreen_openSecretMenu(gb, sp0_);
}

static void scriptCmd_showPasswordScreen_generateSecret(GB *gb, uint16_t sp0_) {
  BASE(scriptCmd_showPasswordScreen);
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+34); mem_wr(gb, wShortSecretIndex, A);
  CYC(b_+34, b_+37); SET_BC(0x0003);
  CALL_C(b_+37, secretFunctionCaller_hook, SYM(secretFunctionCaller), b_+40);
  scriptCmd_showPasswordScreen_finish(gb);
}

// 0c:411c
void scriptCmd_showPasswordScreen_hook(GB *gb) {
  BASE(scriptCmd_showPasswordScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); push_effect(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    scriptCmd_showPasswordScreen_openSecretMenu(gb, sp0_);
    return;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+13); alu_and(gb, 0x03);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (script_jump_table(gb));
    if (jt_ == b_+22) { scriptCmd_showPasswordScreen_askForSecret(gb, sp0_); return; }
    else if (jt_ == b_+30) { scriptCmd_showPasswordScreen_generateSecret(gb, sp0_); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void scriptCmd_disableInput_hook(GB *gb) {
  BASE(scriptCmd_disableInput);
  CYC(b_+0, b_+2); A = 0x81;
  CYC(b_+2, SYM(scriptCmd_disableMenu)); W8(wDisabledObjects) = A;
  scriptCmd_disableMenu_hook(gb);
}

void scriptCmd_disableMenu_hook(GB *gb) {
  BASE(scriptCmd_disableMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x80;
  CYC(b_+2, b_+5); W8(wMenuDisabled) = A;
  CALL_C(b_+5, clearAllParentItems_hook, SYM(clearAllParentItems), b_+8);
  CALL_C(b_+8, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+11);
  CALL_C(b_+11, func_0c_4177_hook, SYM(func_0c_4177), SYM(scriptFunc_popHlAndInc));
  scriptFunc_popHlAndInc_hook(gb);
}

void scriptFunc_popHlAndInc_hook(GB *gb) {
  BASE(scriptFunc_popHlAndInc);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); alu_scf(gb);
  CYC(b_+3, SYM(scriptCmd_enableInput)); ret_effect(gb);
}

void scriptCmd_enableInput_hook(GB *gb) {
  BASE(scriptCmd_enableInput);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, SYM(scriptCmd_enableMenu)); W8(wDisabledObjects) = A;
  scriptCmd_enableMenu_hook(gb);
}

void scriptCmd_enableMenu_hook(GB *gb) {
  BASE(scriptCmd_enableMenu);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wMenuDisabled) = A;
  CYC(b_+4, SYM(scriptCmd_setLinkCantMoveTo91)); scriptFunc_popHlAndInc_hook(gb);
}

void scriptCmd_setLinkCantMoveTo91_hook(GB *gb) {
  BASE(scriptCmd_setLinkCantMoveTo91);
  CYC(b_+0, SYM(scriptFunc_setLinkCantMove)); A = 0x91;
  scriptFunc_setLinkCantMove_hook(gb);
}

void scriptFunc_setLinkCantMove_hook(GB *gb) {
  BASE(scriptFunc_setLinkCantMove);
  CYC(b_+0, b_+3); W8(wDisabledObjects) = A;
  CYC(b_+3, b_+4); SET_HL(pop_effect(gb));
  CYC(b_+4, b_+5); SET_HL(HL + 1);
  CYC(b_+5, SYM(scriptCmd_setLinkCantMoveTo00)); ret_effect(gb);
}

void scriptCmd_setLinkCantMoveTo00_hook(GB *gb) {
  BASE(scriptCmd_setLinkCantMoveTo00);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, SYM(scriptCmd_setLinkCantMoveTo11)); scriptFunc_setLinkCantMove_hook(gb);
}

void scriptCmd_setLinkCantMoveTo11_hook(GB *gb) {
  BASE(scriptCmd_setLinkCantMoveTo11);
  CYC(b_+0, b_+2); A = 0x11;
  CYC(b_+2, SYM(func_0c_4177)); scriptFunc_setLinkCantMove_hook(gb);
}

void func_0c_4177_hook(GB *gb) {
  BASE(func_0c_4177);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); A = W8(wLinkObjectIndex);
  CYC(b_+4, b_+5); H = A;
  CYC(b_+5, b_+7); L = 0x2b;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x80);
  CYC(b_+9, b_+11); L = 0x2d;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, SYM(scriptCmd_setState)); ret_effect(gb);
}

void scriptCmd_setState_hook(GB *gb) {
  BASE(scriptCmd_setState);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, SYM(scriptFunc_setState)); E = 0x44;
  scriptFunc_setState_hook(gb);
}

void scriptFunc_setState_hook(GB *gb) {
  BASE(scriptFunc_setState);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+3); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+3, b_+5); goto increment_state; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); ret_effect(gb);
  return;
increment_state:
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, SYM(scriptCmd_setSubstate)); ret_effect(gb);
}

void scriptCmd_setSubstate_hook(GB *gb) {
  BASE(scriptCmd_setSubstate);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+4); E = 0x45;
  CYC(b_+4, SYM(scriptCmd_jump)); scriptFunc_setState_hook(gb);
}

static void scriptCmd_jump_far(GB *gb) {
  BASE(scriptCmd_jump);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+26); L = mem_rd(gb, HL);
  CYC(b_+26, b_+27); H = A;
  CYC(b_+27, b_+28); alu_scf(gb);
  CYC(b_+28, SYM(scriptCmd_spawnInteraction)); ret_effect(gb);
}

// 0c:419d
void scriptCmd_jump_hook(GB *gb) {
  BASE(scriptCmd_jump);
  CYC(b_+0, b_+1); A = H;
  CYC(b_+1, b_+3); alu_cp(gb, 0x80);
  if (F & FC) {
    CYCT(b_+3, b_+5);
    scriptCmd_jump_far(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = mem_rd(gb, hScriptAddressL);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); A = mem_rd(gb, hScriptAddressH);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); SET_HL(HL + 1);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+13, b_+14); alu_sub(gb, C);
  CYC(b_+14, b_+15); E = A;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_sbc(gb, B);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+20);
    scriptCmd_jump_far(gb);
    return;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); L = E;
  CYC(b_+21, b_+23); H = 0xc3;
  CYC(b_+23, b_+24); ret_effect(gb);
}

void scriptCmd_spawnInteraction_hook(GB *gb) {
  BASE(scriptCmd_spawnInteraction);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CALL_C(b_+2, scriptFunc_loadBcAndDe_hook, SYM(scriptFunc_loadBcAndDe), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CALL_C(b_+6, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); scriptFunc_restoreActiveObject_hook(gb); return; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x4b;
  CALL_C(b_+13, scriptFunc_initializeObject_hook, SYM(scriptFunc_initializeObject), SYM(scriptFunc_restoreActiveObject));
  scriptFunc_restoreActiveObject_hook(gb);
}

void scriptFunc_restoreActiveObject_hook(GB *gb) {
  BASE(scriptFunc_restoreActiveObject);
  CYC(b_+0, b_+2); A = H8(hActiveObject);
  CYC(b_+2, b_+3); D = A;
  CYC(b_+3, b_+4); SET_HL(pop_effect(gb));
  CYC(b_+4, SYM(scriptFunc_loadBcAndDe)); ret_effect(gb);
}

void scriptFunc_loadBcAndDe_hook(GB *gb) {
  BASE(scriptFunc_loadBcAndDe);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); B = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); D = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); E = A;
  CYC(b_+8, SYM(scriptFunc_initializeObject)); ret_effect(gb);
}

void scriptFunc_initializeObject_hook(GB *gb) {
  BASE(scriptFunc_initializeObject);
  CYC(b_+0, b_+1); mem_wr(gb, HL, B);
  CYC(b_+1, b_+2); L = alu_inc8(gb, L);
  CYC(b_+2, b_+3); mem_wr(gb, HL, C);
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);
  CYC(b_+4, b_+5); L = A;
  CYC(b_+5, b_+6); mem_wr(gb, HL, D);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); mem_wr(gb, HL, E);
  CYC(b_+9, SYM(scriptCmd_spawnEnemy)); ret_effect(gb);
}

void scriptCmd_spawnEnemy_hook(GB *gb) {
  BASE(scriptCmd_spawnEnemy);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CALL_C(b_+2, scriptFunc_loadBcAndDe_hook, SYM(scriptFunc_loadBcAndDe), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CALL_C(b_+6, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); scriptFunc_restoreActiveObject_hook(gb); return; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x8b;
  CALL_C(b_+13, scriptFunc_initializeObject_hook, SYM(scriptFunc_initializeObject), b_+16);
  CYC(b_+16, SYM(scriptCmd_spawnEnemyHere)); scriptFunc_restoreActiveObject_hook(gb);
}

// 0c:41f4
void scriptCmd_spawnEnemyHere_hook(GB *gb) {
  BASE(scriptCmd_spawnEnemyHere);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+9); E = 0x4b;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+13); E = 0x4d;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); E = A;
  CYC(b_+15, b_+16); D = L;
  CALL_C(b_+16, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+19);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    scriptFunc_restoreActiveObject_hook(gb);
    return;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = 0x8b;
  CALL_C(b_+23, scriptFunc_initializeObject_hook, SYM(scriptFunc_initializeObject), b_+26);
  CYC(b_+26, SYM(scriptCmd_jumpTable_memoryAddress));
  scriptFunc_restoreActiveObject_hook(gb);
}

// 0c:4210
void scriptCmd_jumpTable_memoryAddress_hook(GB *gb) {
  BASE(scriptCmd_jumpTable_memoryAddress);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); add_double_index_to_hl_from_rst(gb, b_+8);
  CYC(b_+8, SYM(scriptCmd_setCoords)); scriptFunc_jump_hook(gb);
}

void scriptCmd_setCoords_hook(GB *gb) {
  BASE(scriptCmd_setCoords);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = 0x4b;
  CYC(b_+10, b_+11); mem_wr(gb, HL, B);
  CYC(b_+11, b_+13); L = 0x4d;
  CYC(b_+13, b_+14); mem_wr(gb, HL, C);
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CYC(b_+15, SYM(scriptCmd_setAngle)); ret_effect(gb);
}

void scriptCmd_setAngle_hook(GB *gb) {
  BASE(scriptCmd_setAngle);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); E = 0x49;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, SYM(scriptCmd_setSpeed)); ret_effect(gb);
}

void scriptCmd_setSpeed_hook(GB *gb) {
  BASE(scriptCmd_setSpeed);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); E = 0x50;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, SYM(scriptCmd_setZSpeed)); ret_effect(gb);
}

void scriptCmd_setZSpeed_hook(GB *gb) {
  BASE(scriptCmd_setZSpeed);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+4); E = 0x54;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); alu_scf(gb);
  CYC(b_+10, SYM(scriptCmd_checkCounter2ZeroAndReset)); ret_effect(gb);
}

void scriptCmd_checkCounter2ZeroAndReset_hook(GB *gb) {
  BASE(scriptCmd_checkCounter2ZeroAndReset);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+3); E = 0x47;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_HL(HL + 1);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, SYM(scriptCmd_setCollideRadii)); ret_effect(gb);
}

void scriptCmd_setCollideRadii_hook(GB *gb) {
  BASE(scriptCmd_setCollideRadii);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); E = 0x66;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, SYM(scriptCmd_writeInteractionByte)); ret_effect(gb);
}

void scriptCmd_writeInteractionByte_hook(GB *gb) {
  BASE(scriptCmd_writeInteractionByte);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, SYM(scriptCmd_addinteractionByte)); ret_effect(gb);
}

void scriptCmd_addinteractionByte_hook(GB *gb) {
  BASE(scriptCmd_addinteractionByte);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_add(gb, B);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); alu_scf(gb);
  CYC(b_+10, SYM(scriptCmd_getRandomBits)); ret_effect(gb);
}

void scriptCmd_getRandomBits_hook(GB *gb) {
  BASE(scriptCmd_getRandomBits);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CALL_C(b_+2, getRandomNumber_hook, SYM(getRandomNumber), b_+5);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); E = A;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); alu_and(gb, B);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, SYM(scriptCmd_loadSprite)); ret_effect(gb);
}

// 0c:4276
void scriptCmd_loadSprite_hook(GB *gb) {
  BASE(scriptCmd_loadSprite);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto check_sprite_index;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = 0x49;
  CALL_C(b_+9, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+12);
  CYC(b_+12, b_+14);
  goto set_animation;
check_sprite_index:
  CYC(b_+14, b_+16); alu_cp(gb, 0xfe);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto set_animation;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); E = A;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
set_animation:
  CYC(b_+21, b_+22); push_effect(gb, HL);
  CALL_C(b_+22, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+25);
  CYC(b_+25, b_+26); SET_HL(pop_effect(gb));
  CYC(b_+26, b_+28); A = 0x0c;
  CYC(b_+28, b_+30); H8(hRomBank) = A;
  CYC(b_+30, b_+33); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+33, SYM(scriptCmd_turnToFaceLink)); ret_effect(gb);
}

void scriptCmd_turnToFaceLink_hook(GB *gb) {
  BASE(scriptCmd_turnToFaceLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_rlca(gb);
  CALL_C(b_+10, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+13);
  CYC(b_+13, SYM(scriptCmd_setAngleAndExtra)); scriptFunc_popHlAndInc_hook(gb);
}

void scriptCmd_setAngleAndExtra_hook(GB *gb) {
  BASE(scriptCmd_setAngleAndExtra);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); E = 0x49;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CALL_C(b_+6, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+9);
  CYC(b_+9, b_+10); push_effect(gb, HL);
  CALL_C(b_+10, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+13);
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, b_+15); alu_scf(gb);
  CYC(b_+15, SYM(scriptCmd_runGenericNpc)); ret_effect(gb);
}

void scriptCmd_runGenericNpc_hook(GB *gb) {
  BASE(scriptCmd_runGenericNpc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CALL_C(b_+2, scriptFunc_getTextIndex_hook, SYM(scriptFunc_getTextIndex), b_+5);
  CYC(b_+5, b_+6); A = C;
  CYC(b_+6, b_+8); E = 0x72;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+11); E = alu_inc8(gb, E);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+15); SET_HL(SYM(genericNpcScript));
  CYC(b_+15, SYM(scriptFunc_getTextIndex)); ret_effect(gb);
}

void scriptFunc_getTextIndex_hook(GB *gb) {
  BASE(scriptFunc_getTextIndex);
  CYC(b_+0, b_+2); E = 0x70;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto use_script_text_index; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = 0x73;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+12); goto load_text_index;
use_script_text_index:
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); B = A;
load_text_index:
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, SYM(scriptCmd_showText)); ret_effect(gb);
}

void scriptCmd_showText_hook(GB *gb) {
  BASE(scriptCmd_showText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CALL_C(b_+2, scriptFunc_getTextIndex_hook, SYM(scriptFunc_getTextIndex), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CALL_C(b_+6, showText_hook, SYM(showText), b_+9);
  CYC(b_+9, b_+10); SET_HL(pop_effect(gb));
  CYC(b_+10, SYM(scriptCmd_showTextDifferentForLinked)); ret_effect(gb);
}

void scriptCmd_showTextDifferentForLinked_hook(GB *gb) {
  BASE(scriptCmd_showTextDifferentForLinked);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); B = A;
  CALL_C(b_+4, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+7);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+9);
    goto linked;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); SET_HL(HL + 1);
  CYC(b_+11, b_+13); goto show_text;
linked:
  CYC(b_+13, b_+14); SET_HL(HL + 1);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
show_text:
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+17); push_effect(gb, HL);
  CALL_C(b_+17, showText_hook, SYM(showText), b_+20);
  CYC(b_+20, b_+21); SET_HL(pop_effect(gb));
  CYC(b_+21, SYM(scriptCmd_showTextNonExitable)); ret_effect(gb);
}

void scriptCmd_showTextNonExitable_hook(GB *gb) {
  BASE(scriptCmd_showTextNonExitable);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CALL_C(b_+2, scriptFunc_getTextIndex_hook, SYM(scriptFunc_getTextIndex), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CALL_C(b_+6, showTextNonExitable_hook, SYM(showTextNonExitable), b_+9);
  CYC(b_+9, b_+10); SET_HL(pop_effect(gb));
  CYC(b_+10, SYM(scriptCmd_waitForText)); ret_effect(gb);
}

void scriptCmd_waitForText_hook(GB *gb) {
  BASE(scriptCmd_waitForText);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4); A = W8(wTextIsActive);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_HL(HL + 1);
  CYC(b_+7, SYM(scriptCmd_setCounter1)); ret_effect(gb);
}

void scriptCmd_setCounter1_hook(GB *gb) {
  BASE(scriptCmd_setCounter1);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, SYM(scriptFunc_4310)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  scriptFunc_4310_hook(gb);
}

void scriptFunc_4310_hook(GB *gb) {
  BASE(scriptFunc_4310);
  CYC(b_+0, b_+2); E = 0x46;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, SYM(scriptCmd_cpLinkX)); ret_effect(gb);
}

void scriptCmd_cpLinkX_hook(GB *gb) {
  BASE(scriptCmd_cpLinkX);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); push_effect(gb, HL);
  CYC(b_+3, b_+5); E = 0x4d;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+9); SET_HL(w1Link_xh);
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+11); SET_HL(pop_effect(gb));
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); E = A;
  CYC(b_+13, b_+15); A = 0x00;
  if (!(F & FC)) {
    CYCT(b_+15, b_+17);
    goto store_result;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); A = alu_inc8(gb, A);
store_result:
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); alu_scf(gb);
  CYC(b_+20, SYM(scriptCmd_shakeScreen)); ret_effect(gb);
}

void scriptCmd_shakeScreen_hook(GB *gb) {
  BASE(scriptCmd_shakeScreen);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+6); W8(wScreenShakeCounterX) = A;
  CYC(b_+6, SYM(scriptCmd_writeMemory)); ret_effect(gb);
}

void scriptCmd_writeMemory_hook(GB *gb) {
  BASE(scriptCmd_writeMemory);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, BC, A);
  CYC(b_+8, b_+9); alu_scf(gb);
  CYC(b_+9, SYM(scriptCmd_checkPaletteFadeDone)); ret_effect(gb);
}

void scriptCmd_checkPaletteFadeDone_hook(GB *gb) {
  BASE(scriptCmd_checkPaletteFadeDone);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4); A = W8(wPaletteThread_mode);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_HL(HL + 1);
  CYC(b_+7, SYM(scriptCmd_checkCFC0Bit)); ret_effect(gb);
}

void scriptCmd_checkCFC0Bit_hook(GB *gb) {
  BASE(scriptCmd_checkCFC0Bit);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); A = mem_rd(gb, HL);
  CYC(b_+2, b_+4); alu_and(gb, 0x07);
  CYC(b_+4, b_+7); SET_BC(0x00f8);
  CYC(b_+7, b_+8); alu_add(gb, C);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+10); A = mem_rd(gb, BC);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+14); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+14, b_+15); alu_and(gb, B);
  if (F & FZ) {
    CYCT(b_+15, b_+16); ret_effect(gb);
    return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); SET_HL(HL + 1);
  CYC(b_+17, SYM(scriptCmd_xorCFC0Bit)); ret_effect(gb);
}

void scriptCmd_xorCFC0Bit_hook(GB *gb) {
  BASE(scriptCmd_xorCFC0Bit);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); A = mem_rd(gb, HL);
  CYC(b_+2, b_+4); alu_and(gb, 0x07);
  CYC(b_+4, b_+7); SET_BC(0x00f8);
  CYC(b_+7, b_+8); alu_add(gb, C);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+10); A = mem_rd(gb, BC);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+14); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+14, b_+15); alu_xor(gb, B);
  CYC(b_+15, b_+18); mem_wr(gb, wRoomLayoutEnd, A);
  CYC(b_+18, b_+19); SET_HL(HL + 1);
  CYC(b_+19, SYM(scriptCmd_jumpIfNoEnemies)); ret_effect(gb);
}

void scriptCmd_jumpIfNoEnemies_hook(GB *gb) {
  BASE(scriptCmd_jumpIfNoEnemies);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4); A = W8(wNumEnemies);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    scriptFunc_add3ToHl_hook(gb);
    return;
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); SET_HL(HL + 1);
  CYC(b_+9, SYM(scriptCmd_jumpIfC6xxSet)); scriptFunc_jump_hook(gb);
}

void scriptCmd_jumpIfC6xxSet_hook(GB *gb) {
  BASE(scriptCmd_jumpIfC6xxSet);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+4); B = 0xc6;
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+6); SET_HL(HL + 1);
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+8, b_+11);
    scriptFunc_add3ToHl_hook(gb);
    return;
  }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); SET_HL(HL + 1);
  CYC(b_+12, SYM(scriptCmd_playSound)); scriptFunc_jump_hook(gb);
}

void scriptCmd_playSound_hook(GB *gb) {
  BASE(scriptCmd_playSound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); push_effect(gb, HL);
  CALL_C(b_+4, playSound_b00_hook, SYM(playSound_b00), b_+7);
  CYC(b_+7, b_+8); SET_HL(pop_effect(gb));
  CYC(b_+8, SYM(scriptCmd_updateLinkLocalRespawnPosition)); ret_effect(gb);
}

void scriptCmd_updateLinkLocalRespawnPosition_hook(GB *gb) {
  BASE(scriptCmd_updateLinkLocalRespawnPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, updateLinkLocalRespawnPosition_hook, SYM(updateLinkLocalRespawnPosition), b_+3);
  CYC(b_+3, b_+4); SET_HL(pop_effect(gb));
  CYC(b_+4, b_+5); SET_HL(HL + 1);
  CYC(b_+5, SYM(scriptCmd_jumpIfLinkVariableNe)); ret_effect(gb);
}

void scriptCmd_jumpIfLinkVariableNe_hook(GB *gb) {
  BASE(scriptCmd_jumpIfLinkVariableNe);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); D = 0xd0;
  CYC(b_+5, b_+6); E = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    CYC(b_+14, b_+17); SET_BC(0x0003);
    CYC(b_+17, b_+18); alu_add_hl(gb, BC);
    CYC(b_+18, b_+20); A = H8(hActiveObject);
    CYC(b_+20, b_+21); D = A;
    CYC(b_+21, SYM(scriptCmd_jumpIfMemoryEq)); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); SET_HL(HL + 1);
  CYC(b_+11, b_+14); scriptFunc_jump_hook(gb);
}

static void scriptCmd_compareMemoryThenJump(GB *gb) {
  BASE(scriptCmd_jumpIfMemoryEq);
  CYC(b_+7, b_+8); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+8, b_+11);
    scriptFunc_add3ToHl_scf_hook(gb);
    return;
  }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); SET_HL(HL + 1);
  CYC(b_+12, SYM(scriptCmd_jumpIfInteractionByteEq)); scriptFunc_jump_scf_hook(gb);
}

void scriptCmd_jumpIfMemoryEq_hook(GB *gb) {
  BASE(scriptCmd_jumpIfMemoryEq);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  CYC(b_+3, b_+4); SET_HL(HL + 1);
  CYC(b_+4, b_+5); B = mem_rd(gb, HL);
  CYC(b_+5, b_+6); SET_HL(HL + 1);
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  scriptCmd_compareMemoryThenJump(gb);
}

void scriptCmd_jumpIfInteractionByteEq_hook(GB *gb) {
  BASE(scriptCmd_jumpIfInteractionByteEq);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, SYM(scriptCmd_jumpIfRoomFlagSet));
  scriptCmd_compareMemoryThenJump(gb);
}

void scriptCmd_jumpIfRoomFlagSet_hook(GB *gb) {
  BASE(scriptCmd_jumpIfRoomFlagSet);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); push_effect(gb, HL);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+9); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+11);
    CYC(b_+16, b_+17); SET_HL(pop_effect(gb));
    CYC(b_+17, SYM(scriptCmd_orRoomFlags)); scriptFunc_jump_scf_hook(gb);
    return;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+13); SET_HL(HL + 1);
  CYC(b_+13, b_+14); SET_HL(HL + 1);
  CYC(b_+14, b_+15); alu_scf(gb);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void scriptCmd_orRoomFlags_hook(GB *gb) {
  BASE(scriptCmd_orRoomFlags);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); push_effect(gb, HL);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+9); alu_or(gb, B);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+11); SET_HL(pop_effect(gb));
  CYC(b_+11, SYM(scriptCmd_checkSomething)); ret_effect(gb);
}

void scriptCmd_checkSomething_hook(GB *gb) {
  BASE(scriptCmd_checkSomething);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x71;
  CALL_C(b_+2, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+5);
  CYC(b_+5, b_+6); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+6, b_+8);
    CYC(b_+9, SYM(scriptCmd_showLoadedText)); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); SET_HL(HL + 1);
  CYC(b_+9, SYM(scriptCmd_showLoadedText)); ret_effect(gb);
}

void scriptCmd_showLoadedText_hook(GB *gb) {
  BASE(scriptCmd_showLoadedText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x72;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); B = A;
  CALL_C(b_+7, showText_hook, SYM(showText), b_+10);
  CYC(b_+10, b_+11); SET_HL(pop_effect(gb));
  CYC(b_+11, b_+12); SET_HL(HL + 1);
  CYC(b_+12, SYM(scriptCmd_setTextID)); ret_effect(gb);
}

void scriptCmd_setTextID_hook(GB *gb) {
  BASE(scriptCmd_setTextID);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); E = 0x72;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); alu_scf(gb);
  CYC(b_+10, SYM(scriptCmd_setMusic)); ret_effect(gb);
}

void scriptCmd_setMusic_hook(GB *gb) {
  BASE(scriptCmd_setMusic);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto set_music;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = W8(wActiveMusic2);
set_music:
  CYC(b_+10, b_+13); W8(wActiveMusic) = A;
  CYC(b_+13, b_+14); push_effect(gb, HL);
  CALL_C(b_+14, playSound_b00_hook, SYM(playSound_b00), b_+17);
  CYC(b_+17, b_+18); SET_HL(pop_effect(gb));
  CYC(b_+18, SYM(scriptCmd_orMemory)); ret_effect(gb);
}

void scriptCmd_orMemory_hook(GB *gb) {
  BASE(scriptCmd_orMemory);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+9); mem_wr(gb, BC, A);
  CYC(b_+9, b_+10); SET_HL(HL + 1);
  CYC(b_+10, b_+11); alu_scf(gb);
  CYC(b_+11, SYM(scriptCmd_spawnItem)); ret_effect(gb);
}

void scriptCmd_spawnItem_hook(GB *gb) {
  BASE(scriptCmd_spawnItem);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); E = mem_rd(gb, HL);
  CYC(b_+2, b_+3); SET_HL(HL + 1);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+8); push_effect(gb, HL);
  CALL_C(b_+8, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+11);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+14);
    scriptFunc_restoreActiveObject_hook(gb);
    return;
  }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x60);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+18); mem_wr(gb, HL, B);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+20); mem_wr(gb, HL, C);
  CYC(b_+20, b_+21); A = E;
  CYC(b_+21, b_+23); alu_cp(gb, 0xde);
  if (F & FZ) {
    CYCT(b_+23, b_+25);
    goto create_link_item;
  }
  CYC(b_+23, b_+25);
  CALL_C(b_+25, objectCopyPosition_hook, SYM(objectCopyPosition), b_+28);
  CYC(b_+28, b_+31); scriptFunc_restoreActiveObject_hook(gb);
  return;
create_link_item:
  CYC(b_+31, b_+33); E = 0x46;
  CYC(b_+33, b_+35); A = 0x03;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+39); SET_DE(w1Link_yh);
  CALL_C(b_+39, objectCopyPosition_rawAddress_hook, SYM(objectCopyPosition_rawAddress), b_+42);
  CYC(b_+42, SYM(scriptCmd_df)); scriptFunc_restoreActiveObject_hook(gb);
}

void scriptCmd_df_hook(GB *gb) {
  BASE(scriptCmd_df);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(b_+3, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+6);
  CYC(b_+6, b_+9); mem_wr(gb, wTmpcfc0_bigBangGame_filler1, A);
  if (!(F & FC)) {
    CYCT(b_+9, b_+11);
    goto skip_jump;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); scriptFunc_jump_hook(gb);
  return;
skip_jump:
  CYC(b_+14, b_+15); SET_HL(HL + 1);
  CYC(b_+15, b_+16); SET_HL(HL + 1);
  CYC(b_+16, SYM(scriptCmd_jumpIfSomething)); ret_effect(gb);
}

void scriptCmd_jumpIfSomething_hook(GB *gb) {
  BASE(scriptCmd_jumpIfSomething);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+4); A = 0x41;
  CALL_C(b_+4, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+7);
  if (!(F & FC)) {
    CYCT(b_+7, b_+9);
    goto skip_first_argument;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  CYC(b_+12, b_+13); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+15);
    goto skip_remaining_arguments;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); scriptFunc_jump_hook(gb);
  return;
skip_first_argument:
  CYC(b_+18, b_+19); SET_HL(HL + 1);
skip_remaining_arguments:
  CYC(b_+19, b_+20); SET_HL(HL + 1);
  CYC(b_+20, b_+21); SET_HL(HL + 1);
  CYC(b_+21, SYM(scriptCmd_setLinkCantMove)); ret_effect(gb);
}

void scriptCmd_setLinkCantMove_hook(GB *gb) {
  BASE(scriptCmd_setLinkCantMove);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+6); W8(wDisabledObjects) = A;
  CYC(b_+6, SYM(scriptCmd_checkCounter2Zero)); ret_effect(gb);
}

void scriptCmd_checkCounter2Zero_hook(GB *gb) {
  BASE(scriptCmd_checkCounter2Zero);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+3); E = 0x47;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_HL(HL + 1);
  CYC(b_+7, SYM(scriptCmd_setTile)); ret_effect(gb);
}

void scriptCmd_setTile_body_hook(GB *gb) {
  BASE(scriptCmd_setTile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CALL_C(b_+6, setTile_hook, SYM(setTile), b_+9);
  CYC(b_+9, b_+10); SET_HL(pop_effect(gb));
  CYC(b_+10, b_+11); alu_scf(gb);
  CYC(b_+11, SYM(scriptCmd_setTileHere)); ret_effect(gb);
}

void scriptCmd_setTile_hook(GB *gb) {
  BASE(scriptCmd_setTile);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  scriptCmd_setTile_body_hook(gb);
}

void scriptCmd_setTileHere_hook(GB *gb) {
  BASE(scriptCmd_setTileHere);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CALL_C(b_+2, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+5);
  CYC(b_+5, SYM(scriptCmd_callScript));
  scriptCmd_setTile_body_hook(gb);
}

void scriptCmd_callScript_hook(GB *gb) {
  BASE(scriptCmd_callScript);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = 0x75;
  CYC(b_+8, b_+9); A = L;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+11); E = alu_inc8(gb, E);
  CYC(b_+11, b_+12); A = H;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14); L = C;
  CYC(b_+14, b_+15); H = B;
  CYC(b_+15, SYM(scriptCmd_ret)); ret_effect(gb);
}

void scriptCmd_ret_hook(GB *gb) {
  BASE(scriptCmd_ret);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+3); E = 0x75;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); L = A;
  CYC(b_+5, b_+6); E = alu_inc8(gb, E);
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); H = A;
  CYC(b_+8, b_+9); ret_effect(gb);
}

void scriptCmd_jumpIfCBA5Eq_hook(GB *gb) {
  BASE(scriptCmd_jumpIfCBA5Eq);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+5); A = W8(wSelectedTextOption);
  CYC(b_+5, b_+6); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    CYC((SYM(scriptCmd_ret) + 9), (SYM(scriptCmd_ret) + 10)); SET_HL(HL + 1);
    CYC((SYM(scriptCmd_ret) + 10), b_+0); scriptFunc_jump_scf_hook(gb);
    return;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, SYM(scriptCmd_jumpRandom)); scriptFunc_add3ToHl_scf_hook(gb);
}

void scriptCmd_jumpRandom_hook(GB *gb) {
  BASE(scriptCmd_jumpRandom);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, SYM(scriptCmd_jumpTable)); scriptFunc_jump_scf_hook(gb);
}

// 0c:44c3
void scriptCmd_jumpTable_hook(GB *gb) {
  BASE(scriptCmd_jumpTable);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); add_double_index_to_hl_from_rst(gb, b_+6);
  CYC(b_+6, SYM(scriptCmd_jumpIfMemorySet)); scriptFunc_jump_hook(gb);
}

void scriptCmd_jumpIfMemorySet_hook(GB *gb) {
  BASE(scriptCmd_jumpIfMemorySet);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+5); C = A;
  CYC(b_+5, b_+6); SET_HL(HL + 1);
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+8, b_+11);
    scriptFunc_add3ToHl_hook(gb);
    return;
  }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); SET_HL(HL + 1);
  CYC(b_+12, SYM(scriptCmd_writeC6xx)); scriptFunc_jump_scf_hook(gb);
}

void scriptCmd_writeC6xx_hook(GB *gb) {
  BASE(scriptCmd_writeC6xx);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+4); B = 0xc6;
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+6); SET_HL(HL + 1);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, BC, A);
  CYC(b_+8, SYM(scriptCmd_checkCollidedWithLink_ignoreZ)); ret_effect(gb);
}

void scriptCmd_checkCollidedWithLink_body_hook(GB *gb) {
  BASE(scriptCmd_checkCollidedWithLink_onGround);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+5, func_0c_4177_hook, SYM(func_0c_4177), b_+8);
  CYC(b_+8, b_+9); SET_HL(HL + 1);
  CYC(b_+9, SYM(scriptCmd_checkAButton)); ret_effect(gb);
}

void scriptCmd_checkCollidedWithLink_ignoreZ_hook(GB *gb) {
  BASE(scriptCmd_checkCollidedWithLink_ignoreZ);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+3);
  CYC(b_+3, b_+4); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, SYM(scriptCmd_checkCollidedWithLink_onGround));
  scriptCmd_checkCollidedWithLink_body_hook(gb);
}

void scriptCmd_checkCollidedWithLink_onGround_hook(GB *gb) {
  BASE(scriptCmd_checkCollidedWithLink_onGround);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckCollidedWithLink_onGround_hook, SYM(objectCheckCollidedWithLink_onGround), b_+3);
  CYC(b_+3, b_+4); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  scriptCmd_checkCollidedWithLink_body_hook(gb);
}

void scriptCmd_checkAButton_hook(GB *gb) {
  BASE(scriptCmd_checkAButton);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x71;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+5); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, func_0c_4177_hook, SYM(func_0c_4177), b_+11);
  CYC(b_+11, b_+12); SET_HL(HL + 1);
  CYC(b_+12, b_+13); alu_scf(gb);
  CYC(b_+13, SYM(scriptCmd_checkNoEnemies)); ret_effect(gb);
}

void scriptCmd_checkNoEnemies_hook(GB *gb) {
  BASE(scriptCmd_checkNoEnemies);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4); A = mem_rd(gb, wNumEnemies);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_HL(HL + 1);
  CYC(b_+7, SYM(scriptCmd_checkFlagSet)); ret_effect(gb);
}

void scriptCmd_checkFlagSet_hook(GB *gb) {
  BASE(scriptCmd_checkFlagSet);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CYC(b_+2, b_+3); SET_HL(HL + 1);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); H = mem_rd(gb, HL);
  CYC(b_+7, b_+8); L = A;
  CYC(b_+8, b_+9); A = B;
  CALL_C(b_+9, checkFlag_hook, SYM(checkFlag), b_+12);
  CYC(b_+12, b_+13); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(b_+13, b_+14); ret_effect(gb);
    return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+17); SET_BC(0x0004);
  CYC(b_+17, b_+18); alu_add_hl(gb, BC);
  CYC(b_+18, b_+19); alu_scf(gb);
  CYC(b_+19, SYM(scriptCmd_checkInteractionByteEq)); ret_effect(gb);
}

void scriptCmd_checkInteractionByteEq_hook(GB *gb) {
  BASE(scriptCmd_checkInteractionByteEq);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CYC(b_+2, b_+3); SET_HL(HL + 1);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+7, b_+9);
    goto matched;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); SET_HL(pop_effect(gb));
  CYC(b_+10, b_+11); alu_xor(gb, A);
  CYC(b_+11, b_+12); ret_effect(gb);
  return;
matched:
  CYC(b_+12, b_+13); SET_BC(pop_effect(gb));
  CYC(b_+13, b_+14); SET_HL(HL + 1);
  CYC(b_+14, SYM(scriptCmd_checkMemoryEq)); ret_effect(gb);
}

void scriptCmd_checkMemoryEq_hook(GB *gb) {
  BASE(scriptCmd_checkMemoryEq);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CYC(b_+2, b_+3); SET_HL(HL + 1);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); C = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+9, b_+11);
    goto matched;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+14); ret_effect(gb);
  return;
matched:
  CYC(b_+14, b_+15); SET_BC(pop_effect(gb));
  CYC(b_+15, b_+16); SET_HL(HL + 1);
  CYC(b_+16, SYM(scriptCmd_checkHeartDisplayUpdated)); ret_effect(gb);
}

void scriptCmd_checkHeartDisplayUpdated_hook(GB *gb) {
  BASE(scriptCmd_checkHeartDisplayUpdated);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4); A = mem_rd(gb, wDisplayedHearts);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+8); A = mem_rd(gb, wLinkHealth);
  CYC(b_+8, b_+9); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+9, b_+11);
    goto updated;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+13); ret_effect(gb);
  return;
updated:
  CYC(b_+13, b_+14); SET_HL(HL + 1);
  CYC(b_+14, b_+15); alu_scf(gb);
  CYC(b_+15, SYM(scriptCmd_checkRupeeDisplayUpdated)); ret_effect(gb);
}

void scriptCmd_checkRupeeDisplayUpdated_hook(GB *gb) {
  BASE(scriptCmd_checkRupeeDisplayUpdated);
  CYC(b_+0, b_+3); SET_HL(wNumRupees);
  CYC(b_+3, b_+6); A = mem_rd(gb, wDisplayedRupees);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+7, b_+9);
    goto not_updated;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+13); A = mem_rd(gb, (wDisplayedRupees + 1));
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+14, b_+17);
    scriptFunc_popHlAndInc_hook(gb);
    return;
  }
  CYC(b_+14, b_+17);
not_updated:
  CYC(b_+17, b_+18); SET_HL(pop_effect(gb));
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, SYM(scriptCmd_checkNotCollidedWithLink_ignoreZ)); ret_effect(gb);
}

void scriptCmd_checkNotCollidedWithLink_ignoreZ_hook(GB *gb) {
  BASE(scriptCmd_checkNotCollidedWithLink_ignoreZ);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+3);
  CYC(b_+3, b_+4); SET_HL(pop_effect(gb));
  if (F & FC) {
    CYCT(b_+4, b_+6);
    goto collided;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); SET_HL(HL + 1);
  CYC(b_+7, b_+8); ret_effect(gb);
  return;
collided:
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, SYM(scriptCmd_createPuff)); ret_effect(gb);
}

void scriptCmd_createPuff_hook(GB *gb) {
  BASE(scriptCmd_createPuff);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCreatePuff_hook, SYM(objectCreatePuff), b_+3);
  CYC(b_+3, b_+4); SET_HL(pop_effect(gb));
  CYC(b_+4, b_+5); SET_HL(HL + 1);
  CYC(b_+5, SYM(scriptCmd_jumpIfGlobalFlagSet)); ret_effect(gb);
}

void scriptCmd_jumpIfGlobalFlagSet_hook(GB *gb) {
  BASE(scriptCmd_jumpIfGlobalFlagSet);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); push_effect(gb, HL);
  CALL_C(b_+4, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+7);
  CYC(b_+7, b_+8); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    goto skip_jump;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); scriptFunc_jump_scf_hook(gb);
  return;
skip_jump:
  CYC(b_+13, b_+14); SET_HL(HL + 1);
  CYC(b_+14, b_+15); SET_HL(HL + 1);
  CYC(b_+15, b_+16); alu_scf(gb);
  CYC(b_+16, SYM(scriptCmd_setOrUnsetGlobalFlag)); ret_effect(gb);
}

void scriptCmd_setOrUnsetGlobalFlag_hook(GB *gb) {
  BASE(scriptCmd_setOrUnsetGlobalFlag);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto unset_flag;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); push_effect(gb, HL);
  CALL_C(b_+8, setGlobalFlag_hook, SYM(setGlobalFlag), b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+13); alu_scf(gb);
  CYC(b_+13, b_+14); ret_effect(gb);
  return;
unset_flag:
  CYC(b_+14, b_+16); alu_and(gb, 0x7f);
  CYC(b_+16, b_+17); push_effect(gb, HL);
  CALL_C(b_+17, unsetGlobalFlag_hook, SYM(unsetGlobalFlag), b_+20);
  CYC(b_+20, b_+21); SET_HL(pop_effect(gb));
  CYC(b_+21, b_+22); alu_scf(gb);
  CYC(b_+22, SYM(scriptCmd_initNpcHitbox)); ret_effect(gb);
}

void scriptCmd_initNpcHitbox_hook(GB *gb) {
  BASE(scriptCmd_initNpcHitbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x66;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto update_a_button_list;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x06;
  CALL_C(b_+8, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+11);
update_a_button_list:
  CYC(b_+11, b_+13); E = 0x71;
  CALL_C(b_+13, objectRemoveFromAButtonSensitiveObjectList_hook, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+16);
  CYC(b_+16, b_+18); E = 0x71;
  CALL_C(b_+18, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+21);
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+22, b_+23); ret_effect(gb);
    return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); SET_HL(HL + 1);
  CYC(b_+24, b_+25); alu_scf(gb);
  CYC(b_+25, SYM(scriptCmd_moveNpcUp)); ret_effect(gb);
}

void scriptCmd_moveNpc_body_hook(GB *gb) {
  BASE(scriptCmd_moveNpcUp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+2, b_+4); E = 0x49;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CALL_C(b_+5, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+8);
  CALL_C(b_+8, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+13); SET_HL(HL + 1);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+16); E = 0x47;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, SYM(scriptCmd_moveNpcRight)); ret_effect(gb);
}

void scriptCmd_moveNpcUp_hook(GB *gb) {
  BASE(scriptCmd_moveNpcUp);
  CYC(b_+0, b_+2); A = 0x00;
  scriptCmd_moveNpc_body_hook(gb);
}

void scriptCmd_moveNpcRight_hook(GB *gb) {
  BASE(scriptCmd_moveNpcRight);
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, SYM(scriptCmd_moveNpcDown));
  scriptCmd_moveNpc_body_hook(gb);
}

void scriptCmd_moveNpcDown_hook(GB *gb) {
  BASE(scriptCmd_moveNpcDown);
  CYC(b_+0, b_+2); A = 0x10;
  CYC(b_+2, SYM(scriptCmd_moveNpcLeft));
  scriptCmd_moveNpc_body_hook(gb);
}

void scriptCmd_moveNpcLeft_hook(GB *gb) {
  BASE(scriptCmd_moveNpcLeft);
  CYC(b_+0, b_+2); A = 0x18;
  CYC(b_+2, SYM(scriptCmd_delay));
  scriptCmd_moveNpc_body_hook(gb);
}

void scriptCmd_delay_hook(GB *gb) {
  BASE(scriptCmd_delay);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+2, b_+4); alu_and(gb, 0x0f);
  CYC(b_+4, b_+7); SET_BC(b_+14);
  CALL_C(b_+7, addAToBc_hook, 0x006d, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, BC);
  CYC(b_+11, b_+14); scriptFunc_4310_hook(gb);
}
