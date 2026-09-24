#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/companionScripts.s.
// INTERAC_COMPANION_SCRIPTS

static uint16_t companionScripts_jump_table(GB *gb) {
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

static void companionScripts_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_COMPANION_SCRIPTS
void s_interactionCode71_hook(GB *gb) {
  BASE(interactionCode71);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_6c9b; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+10); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+10, b_+13);
  TAIL(interactionDelete);
L_6c9b:
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == SYM(companionScript_subid00) && hook_is(gb, SYM(companionScript_subid00), s_companionScript_subid00_hook)) { s_companionScript_subid00_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid01) && hook_is(gb, SYM(companionScript_subid01), s_companionScript_subid01_hook)) { s_companionScript_subid01_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid02) && hook_is(gb, SYM(companionScript_subid02), s_companionScript_subid02_hook)) { s_companionScript_subid02_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid03) && hook_is(gb, SYM(companionScript_subid03), s_companionScript_subid03_hook)) { s_companionScript_subid03_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid04) && hook_is(gb, SYM(companionScript_subid04), s_companionScript_subid04_hook)) { s_companionScript_subid04_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid05) && hook_is(gb, SYM(companionScript_subid05), s_companionScript_subid05_hook)) { s_companionScript_subid05_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid06) && hook_is(gb, SYM(companionScript_subid06), s_companionScript_subid06_hook)) { s_companionScript_subid06_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid07) && hook_is(gb, SYM(companionScript_subid07), s_companionScript_subid07_hook)) { s_companionScript_subid07_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid08) && hook_is(gb, SYM(companionScript_subid08), s_companionScript_subid08_hook)) { s_companionScript_subid08_hook(gb); return; }
    if (jt_ == SYM(companionScript_subid09) && hook_is(gb, SYM(companionScript_subid09), s_companionScript_subid09_hook)) { s_companionScript_subid09_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// Ricky running off after jumping up cliff in North Horon
void s_companionScript_subid00_hook(GB *gb) {
  BASE(companionScript_subid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == SYM(companionScript_runScriptDeleteWhenDone) && hook_is(gb, SYM(companionScript_runScriptDeleteWhenDone), s_companionScript_runScriptDeleteWhenDone_hook)) { s_companionScript_runScriptDeleteWhenDone_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+14, b_+16); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+16, b_+18); TAIL(companionScript_delete); }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); A = mem_rd(gb, w1Companion_id);
  CYC(b_+21, b_+23); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); TAIL(companionScript_delete); }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+28, b_+30); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(b_+30, b_+33); TAIL(interactionDelete); }
  CYC(b_+30, b_+33);
  CYC(b_+33, b_+35); A = 0x0a;
  CYC(b_+35, b_+38); SET_HL(w1Companion_state);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+39, b_+41); L = 0x03;
  CYC(b_+41, b_+43); A = 0x02;
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CYC(b_+44, b_+46); L = 0x30;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+49); L = 0x3f;
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);
  CYC(b_+50, b_+53); SET_HL(SYM(companionScript_RickyLeavingYouInSpoolSwamp));
  CYC(b_+53, b_+56);
  TAIL(interactionSetScript);
}

// Moosh being bullied in Spool
void s_companionScript_subid01_hook(GB *gb) {
  BASE(companionScript_subid01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == SYM(companionScript_runScriptDeleteWhenDone) && hook_is(gb, SYM(companionScript_runScriptDeleteWhenDone), s_companionScript_runScriptDeleteWhenDone_hook)) { s_companionScript_runScriptDeleteWhenDone_hook(gb); return; }
    if (jt_ == SYM(companionScript_giveFlute) && hook_is(gb, SYM(companionScript_giveFlute), s_companionScript_giveFlute_hook)) { s_companionScript_giveFlute_hook(gb); return; }
    if (jt_ == SYM(companionScriptFunc_6eaf) && hook_is(gb, SYM(companionScriptFunc_6eaf), s_companionScriptFunc_6eaf_hook)) { s_companionScriptFunc_6eaf_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+15); A = mem_rd(gb, w1Companion_id);
  CYC(b_+15, b_+17); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); TAIL(companionScript_delete); }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+22, b_+24); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+24, b_+26); TAIL(companionScript_delete); }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+31, b_+33); A = 0x0d;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CALL_C(b_+34, s_companionScript_setSubId0AndInitGraphics_hook, SYM(companionScript_setSubId0AndInitGraphics), b_+37);
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+39, b_+41); A = 0x01;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+44); A = 0x1c;
  CYC(b_+44, b_+46); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+49); A = 0x2c;
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+55); SET_HL(SYM(companionScript_mooshInSpoolSwamp));
  CALL_C(b_+55, s_interactionSetScript, SYM(interactionSetScript), b_+58);
  CYC(b_+58, b_+61); A = mem_rd(gb, wMooshState);
  CYC(b_+61, b_+63); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+63, b_+65); TAIL(companionScript_delete); }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+66); alu_or(gb, A);
  CYC(b_+66, b_+68); A = 0x01;
  CYC(b_+68, b_+71); mem_wr(gb, wDiggingUpEnemiesForbidden, A);
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; }
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+75);
  TAIL(interactionAnimateAsNpc);
}

void s_companionScript_runScriptDeleteWhenDone_hook(GB *gb) {
  BASE(companionScript_runScriptDeleteWhenDone);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_interactionRunScript, SYM(interactionRunScript), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, s_setStatusBarNeedsRefreshBit1, SYM(setStatusBarNeedsRefreshBit1), b_+7);
  s_companionScript_delete(gb); return; // falls through
}

// Sunken city entrance
void s_companionScript_subid02_hook(GB *gb) {
  BASE(companionScript_subid02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+63) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+14, b_+16); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+16, b_+18); TAIL(companionScript_delete); }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); A = mem_rd(gb, w1Companion_id);
  CYC(b_+21, b_+23); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(b_+23, b_+25); goto func_6d72; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); TAIL(companionScript_delete); }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0x0a;
  CYC(b_+31, b_+34); SET_HL(w1Companion_state);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+35, b_+37); L = 0x03;
  CYC(b_+37, b_+39); A = 0x08;
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+42); L = 0x3f;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x14);
  CYC(b_+44, b_+47); SET_HL(SYM(companionScript_mooshEnteringSunkenCity));
  CYC(b_+47, b_+50);
  TAIL(interactionSetScript);
func_6d72:
  CYC(b_+50, b_+53); SET_HL(w1Companion_state);
  CYC(b_+53, b_+55); A = 0x0a;
  CYC(b_+55, b_+56); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+56, b_+58); L = 0x03;
  CYC(b_+58, b_+60); A = 0x09;
  CYC(b_+60, b_+61); mem_wr(gb, HL, A);
  CYC(b_+61, b_+63);
  TAIL(companionScript_delete);
state1:
  CALL_C(b_+63, s_interactionRunScript, SYM(interactionRunScript), b_+66);
  if (F & FC) { CYCT(b_+66, b_+68); TAIL(companionScript_delete); }
  CYC(b_+66, b_+68);
  RET(b_+68); return;
}

// Moosh in Mt Cucco
void s_companionScript_subid06_hook(GB *gb) {
  BASE(companionScript_subid06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+47) goto state1;
    if (jt_ == b_+103) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+13); A = mem_rd(gb, wMooshState);
  CYC(b_+13, b_+15); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); TAIL(companionScript_delete); }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x01;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); A = 0x1c;
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); A = 0x2c;
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+32); A = 0x47;
  CALL_C(b_+32, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+35);
  CYC(b_+35, b_+37); A = 0x00;
  CYC(b_+37, b_+38); alu_rla(gb);
  CYC(b_+38, b_+40); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+44); SET_HL(SYM(companionScript_mooshInMtCucco));
  CYC(b_+44, b_+47);
  TAIL(interactionSetScript);
state1:
  CYC(b_+47, b_+50); A = mem_rd(gb, w1Companion_var3d);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+51, b_+53); goto goToRunScriptThenDelete; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+57, b_+59); goto goToRunScriptThenDelete; }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+63, b_+65); goto goToRunScriptThenDelete; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+66); A = alu_inc8(gb, A);
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+70); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+70, b_+73); SET_HL(w1Link);
  CALL_C(b_+73, s_objectTakePosition, SYM(objectTakePosition), b_+76);
  CYC(b_+76, b_+79); A = mem_rd(gb, w1Companion_yh);
  CYC(b_+79, b_+80); B = A;
  CYC(b_+80, b_+83); A = mem_rd(gb, w1Companion_xh);
  CYC(b_+83, b_+84); C = A;
  CALL_C(b_+84, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+87);
  CYC(b_+87, b_+89); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+89, b_+90); mem_wr(gb, DE, A);
  CYC(b_+90, b_+92); A = 0x02;
  CYC(b_+92, b_+95); mem_wr(gb, w1Companion_direction, A);
  CYC(b_+95, b_+97); alu_add(gb, 0x01);
  CYC(b_+97, b_+100); mem_wr(gb, w1Companion_var3f, A);
goToRunScriptThenDelete:
  CYC(b_+100, b_+103);
  TAIL_S(companionScript_runScriptDeleteWhenDone);
state2:
  CYC(b_+103, b_+104); H = D;
  CYC(b_+104, b_+106); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+106, b_+108); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+108, b_+110); goto L_6e0d; }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x32);
  CYC(b_+114, b_+117); SET_BC(0xfec0);
  CALL_C(b_+117, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+120);
  CALL_C(b_+120, s_objectSetVisible80, SYM(objectSetVisible80), b_+123);
  CALL_C(b_+123, s_companionScript_setSubId0AndInitGraphics_hook, SYM(companionScript_setSubId0AndInitGraphics), b_+126);
  CYC(b_+126, b_+128); A = 0x06;
  CYC(b_+128, b_+130); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+130, b_+131); mem_wr(gb, DE, A);
  CYC(b_+131, b_+133); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+133, b_+135); A = 0x10;
  CYC(b_+135, b_+136); mem_wr(gb, DE, A);
L_6e0d:
  CALL_C(b_+136, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+139);
  CYC(b_+139, b_+141); C = 0x40;
  CALL_C(b_+141, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+144);
  if (!(F & FZ)) { CYCT(b_+144, b_+147); TAIL(objectApplySpeed); }
  CYC(b_+144, b_+147);
  CALL_C(b_+147, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+150);
  if (!(F & FZ)) { RET_TAKEN(b_+150); return; }
  CYC(b_+150, b_+151);
  CYC(b_+151, b_+153); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+153, b_+154); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+154, b_+156); A = 0x47;
  CALL_C(b_+156, s_loseTreasure, SYM(loseTreasure), b_+159);
  CYC(b_+159, b_+162);
  TAIL(objectSetInvisible);
}

// Ricky in North Horon
void s_companionScript_subid03_hook(GB *gb) {
  BASE(companionScript_subid03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == SYM(companionScript_runScriptDeleteWhenDone) && hook_is(gb, SYM(companionScript_runScriptDeleteWhenDone), s_companionScript_runScriptDeleteWhenDone_hook)) { s_companionScript_runScriptDeleteWhenDone_hook(gb); return; }
    if (jt_ == b_+37) goto state2;
    if (jt_ == SYM(companionScriptFunc_6eaf) && hook_is(gb, SYM(companionScriptFunc_6eaf), s_companionScriptFunc_6eaf_hook)) { s_companionScriptFunc_6eaf_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+15); A = mem_rd(gb, wRickyState);
  CYC(b_+15, b_+17); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+17, b_+20); TAIL_S(companionScript_delete2); }
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+25, b_+27); A = 0x0b;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CALL_C(b_+28, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+31);
  CYC(b_+31, b_+34); SET_HL(SYM(companionScript_RickyInNorthHoron));
  CYC(b_+34, b_+37);
  TAIL(interactionSetScript);
state2:
  CYC(b_+37, b_+39); A = 0x48;
  CALL_C(b_+39, s_loseTreasure, SYM(loseTreasure), b_+42);
  s_companionScript_giveFlute_hook(gb); return; // falls through
}

void s_companionScript_giveFlute_hook(GB *gb) {
  BASE(companionScript_giveFlute);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+5, s_interactionIncState, SYM(interactionIncState), b_+8);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+14); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(b_+14, b_+16); goto L_6e67; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); SET_HL(wLastAnimalMountPointY);
  CYC(b_+19, b_+20); companionScripts_add_a_to_hl(gb, b_+20);
  CYC(b_+20, b_+22); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
L_6e67:
  CYC(b_+22, b_+23); A = C;
  CYC(b_+23, b_+26); SET_HL(wAnimalCompanion);
  CYC(b_+26, b_+27); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); alu_sub(gb, 0x0a);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_VAR2F;
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); C = A;
  CYC(b_+35, b_+37); A = 0x0e;
  CALL_C(b_+37, s_giveTreasure, SYM(giveTreasure), b_+40);
  CYC(b_+40, b_+43); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+43, b_+45); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+47, b_+49); A = 0x01;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CALL_C(b_+50, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+53);
  CYC(b_+53, b_+55); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+55, b_+57); A = 0x03;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+60); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+63); alu_sub(gb, 0x0a);
  CYC(b_+63, b_+64); C = A;
  CYC(b_+64, b_+66); alu_and(gb, 0x01);
  CYC(b_+66, b_+67); alu_add(gb, A);
  CYC(b_+67, b_+68); alu_xor(gb, C);
  CYC(b_+68, b_+70); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+74); SET_HL(wLinkForceState);
  CYC(b_+74, b_+76); A = 0x04;
  CYC(b_+76, b_+77); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x01);
  CYC(b_+79, b_+82); SET_HL(w1Link);
  CYC(b_+82, b_+85); SET_BC(0xf200);
  CALL_C(b_+85, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+88);
  CALL_C(b_+88, s_objectSetVisible80, SYM(objectSetVisible80), b_+91);
  CYC(b_+91, b_+94);
  TAIL(interactionRunScript);
}

void s_companionScriptFunc_6eaf_hook(GB *gb) {
  BASE(companionScriptFunc_6eaf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+6); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+6, s_objectSetInvisible, SYM(objectSetInvisible), b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+12, b_+14); alu_and(gb, 0x0f);
  CYC(b_+14, b_+15); alu_add(gb, A);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+20); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+20, s_interactionRunScript, SYM(interactionRunScript), b_+23);
  if (!(F & FC)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CYC(b_+25, b_+28); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+28, b_+31); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+31, b_+33);
  TAIL_S(companionScript_delete2);
}

// Dimitri in Spool Swamp
void s_companionScript_subid04_hook(GB *gb) {
  BASE(companionScript_subid04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == SYM(companionScript_runScriptDeleteWhenDone) && hook_is(gb, SYM(companionScript_runScriptDeleteWhenDone), s_companionScript_runScriptDeleteWhenDone_hook)) { s_companionScript_runScriptDeleteWhenDone_hook(gb); return; }
    if (jt_ == SYM(companionScript_giveFlute) && hook_is(gb, SYM(companionScript_giveFlute), s_companionScript_giveFlute_hook)) { s_companionScript_giveFlute_hook(gb); return; }
    if (jt_ == SYM(companionScriptFunc_6eaf) && hook_is(gb, SYM(companionScriptFunc_6eaf), s_companionScriptFunc_6eaf_hook)) { s_companionScriptFunc_6eaf_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+15); A = mem_rd(gb, wDimitriState);
  CYC(b_+15, b_+17); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); TAIL_S(companionScript_delete2); }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+22, b_+24); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+24, b_+26); TAIL_S(companionScript_delete2); }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+31, b_+33); A = 0x0c;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+37); SET_HL(SYM(companionScript_dimitriInSpoolSwamp));
  CYC(b_+37, b_+40);
  TAIL(interactionSetScript);
}

// Dimitri being bullied
void s_companionScript_subid05_hook(GB *gb) {
  BASE(companionScript_subid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == SYM(companionScript_runScriptDeleteWhenDone) && hook_is(gb, SYM(companionScript_runScriptDeleteWhenDone), s_companionScript_runScriptDeleteWhenDone_hook)) { s_companionScript_runScriptDeleteWhenDone_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+11); A = mem_rd(gb, wDimitriState);
  CYC(b_+11, b_+13); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); TAIL_S(companionScript_delete2); }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+18, b_+20); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+20, b_+22); TAIL_S(companionScript_delete2); }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = 0x01;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28); SET_HL(SYM(companionScript_dimitriBeingBullied));
  CYC(b_+28, b_+31);
  TAIL(interactionSetScript);
}

// Moblin rest house
void s_companionScript_subid07_hook(GB *gb) {
  BASE(companionScript_subid07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wDimitriState);
  CYC(b_+3, b_+5); alu_or(gb, 0x20);
  CYC(b_+5, b_+8); mem_wr(gb, wDimitriState, A);
  s_companionScript_delete2_hook(gb); return; // falls through
}

void s_companionScript_delete2_hook(GB *gb) {
  BASE(companionScript_delete2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(interactionDelete);
}

// Sunken city entrance
void s_companionScript_subid08_hook(GB *gb) {
  BASE(companionScript_subid08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionScripts_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+25) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Companion_id);
  CYC(b_+14, b_+16); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); TAIL_S(companionScript_delete2); }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+21, b_+23); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+23, b_+25); TAIL_S(companionScript_delete2); }
  CYC(b_+23, b_+25);
state1:
  CYC(b_+25, b_+28); A = mem_rd(gb, wScrollMode);
  CYC(b_+28, b_+30); alu_and(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); SET_HL(w1Companion_yh);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+35, b_+37); alu_cp(gb, 0x50);
  if (!(F & FC)) { RET_TAKEN(b_+37); return; }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); alu_cp(gb, 0x30);
  if (F & FC) { RET_TAKEN(b_+40); return; }
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_cp(gb, 0x10);
  if (!(F & FC)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); A = 0x10;
  CYC(b_+48, b_+49); mem_wr(gb, HL, A);
  CYC(b_+49, b_+51); L = 0x04;
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+54); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+54, b_+56); goto L_6f63; }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+58, b_+60); goto L_6f63; }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x01);
  CALL_C(b_+62, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+65);
L_6f63:
  CYC(b_+65, b_+67); L = 0x04;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x0d);
  CYC(b_+69, b_+72); SET_BC(SYM(objectGetRelatedObject1Var));
  CYC(b_+72, b_+75);
  TAIL(showText);
}

// 1st screen of North Horon from Eyeglass lake area
void s_companionScript_subid09_hook(GB *gb) {
  BASE(companionScript_subid09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); H = 0xc6;
  CALL_C(b_+2, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_6f7f; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x0e;
  CALL_C(b_+9, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+12);
  if (F & FC) { CYCT(b_+12, b_+14); goto L_6f7f; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); L = 0x10;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0b);
L_6f7f:
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+20, b_+22); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+22, b_+24);
  TAIL_S(companionScript_delete2);
}

void s_companionScript_setSubId0AndInitGraphics_hook(GB *gb) {
  BASE(companionScript_setSubId0AndInitGraphics);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+7);
  TAIL(interactionInitGraphics);
}

