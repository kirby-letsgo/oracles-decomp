#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/subrosianAtD8.s.
// INTERAC_SUBROSIAN_AT_VOLCANO

static uint16_t subrosianAtD8_jump_table(GB *gb) {
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

// INTERAC_SUBROSIAN_AT_VOLCANO
void s_interactionCode55_hook(GB *gb) {
  BASE(interactionCode55);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosianAtD8_jump_table(gb));
    if (jt_ == SYM(subrosianAtD8_subid0) && hook_is(gb, SYM(subrosianAtD8_subid0), s_subrosianAtD8_subid0_hook)) { s_subrosianAtD8_subid0_hook(gb); return; }
    if (jt_ == SYM(subrosianAtD8_subid1) && hook_is(gb, SYM(subrosianAtD8_subid1), s_subrosianAtD8_subid1_hook)) { s_subrosianAtD8_subid1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_subrosianAtD8_subid0_hook(GB *gb) {
  BASE(subrosianAtD8_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosianAtD8_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+57) goto state1;
    if (jt_ == b_+123) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+10, s_subrosianAtD8_getNumEssences_hook, SYM(subrosianAtD8_getNumEssences), b_+13);
  CYC(b_+13, b_+15); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(b_+15, b_+18); TAIL(interactionDelete); }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+21);
  CYC(b_+21, b_+24); SET_HL(SYM(subrosianAtD8Script));
  CALL_C(b_+24, s_interactionSetScript, SYM(interactionSetScript), b_+27);
  CYC(b_+27, b_+29); A = 0x06;
  CALL_C(b_+29, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+32);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x3c);
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+38, s_objectAddToAButtonSensitiveObjectList, SYM(objectAddToAButtonSensitiveObjectList), b_+41);
  CALL_C(b_+41, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+44);
  CYC(b_+44, b_+46); alu_and(gb, 0x40);
  CYC(b_+46, b_+48); A = 0x02;
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto L_7c66; }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+51); A = alu_dec8(gb, A);
L_7c66:
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+57);
  TAIL(objectSetVisiblec2);
state1:
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); push_effect(gb, b_+61);
  do { uint16_t jt_ = (subrosianAtD8_jump_table(gb));
    if (jt_ == b_+65) goto substate0;
    if (jt_ == b_+104) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+65, s_interactionAnimate, SYM(interactionAnimate), b_+68);
  CALL_C(b_+68, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+71);
  CALL_C(b_+71, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+74);
  CYC(b_+74, b_+76); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+78, b_+80); goto L_7c93; }
  CYC(b_+78, b_+80);
  CALL_C(b_+80, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+83);
  if (!(F & FZ)) { RET_TAKEN(b_+83); return; }
  CYC(b_+83, b_+84);
  CYC(b_+84, b_+86); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+86, b_+87); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+87, s_objectSetVisiblec2, SYM(objectSetVisiblec2), b_+90);
  CYC(b_+90, b_+93); SET_HL(SYM(subrosianAtD8Script_tossItemIntoHole));
  CYC(b_+93, b_+96);
  TAIL(interactionSetScript);
L_7c93:
  CYC(b_+96, b_+97); alu_xor(gb, A);
  CYC(b_+97, b_+98); mem_wr(gb, DE, A);
  CYC(b_+98, b_+101); SET_BC(0x3c00);
  CYC(b_+101, b_+104);
  TAIL(showText);
substate1:
  CALL_C(b_+104, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+107);
  CALL_C(b_+107, s_interactionAnimate, SYM(interactionAnimate), b_+110);
  CALL_C(b_+110, s_interactionRunScript, SYM(interactionRunScript), b_+113);
  if (!(F & FC)) { RET_TAKEN(b_+113); return; }
  CYC(b_+113, b_+114);
  CYC(b_+114, b_+115); H = D;
  CYC(b_+115, b_+117); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x3c);
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+121, b_+122); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+122); return;
state2:
  CYC(b_+123, b_+125); C = 0x60;
  CALL_C(b_+125, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+128);
  if (!(F & FZ)) { CYCT(b_+128, b_+130); goto L_7cbb; }
  CYC(b_+128, b_+130);
  CYC(b_+130, b_+133); SET_BC(0xfe00);
  CALL_C(b_+133, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+136);
L_7cbb:
  CALL_C(b_+136, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+139);
  CALL_C(b_+139, s_interactionAnimate, SYM(interactionAnimate), b_+142);
  CALL_C(b_+142, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+145);
  CYC(b_+145, b_+148);
  TAIL(interactionRunScript);
}

void s_subrosianAtD8_subid1_hook(GB *gb) {
  BASE(subrosianAtD8_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosianAtD8_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+64) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_subrosianAtD8_getNumEssences_hook, SYM(subrosianAtD8_getNumEssences), b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(b_+13, b_+16); TAIL(interactionDelete); }
  CYC(b_+13, b_+16);
  CALL_C(b_+16, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+19);
  CYC(b_+19, b_+21); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+21, b_+24); TAIL(interactionDelete); }
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+27); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+32); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CALL_C(b_+34, s_checkLinkVulnerableAndIDZero, SYM(checkLinkVulnerableAndIDZero), b_+37);
  if (!(F & FC)) { RET_TAKEN(b_+37); return; }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); A = 0x01;
  CYC(b_+40, b_+43); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+43, b_+46); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+46, b_+49); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+49, b_+51); A = 0x5a;
  CALL_C(b_+51, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+54);
  CYC(b_+54, b_+56); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+56, b_+58); A = 0x01;
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+61); A = 0xfb;
  CYC(b_+61, b_+64);
  TAIL(playSound_b00);
state1:
  CYC(b_+64, b_+67); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+67, b_+68); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CALL_C(b_+69, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+72);
  CYC(b_+72, b_+74); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+74, b_+76); A = 0x0b;
  CYC(b_+76, b_+79); mem_wr(gb, wCutsceneTrigger, A);
  CALL_C(b_+79, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+82);
  CYC(b_+82, b_+85);
  TAIL(interactionDelete);
}

void s_subrosianAtD8_getNumEssences_hook(GB *gb) {
  BASE(subrosianAtD8_getNumEssences);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+3, b_+6);
  TAIL(getNumSetBits);
}

