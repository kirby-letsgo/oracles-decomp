#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/impa.s.
// INTERAC_IMPA

static uint16_t impa_jump_table(GB *gb) {
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

static void impa_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_IMPA
void s_interactionCode9d_hook(GB *gb) {
  BASE(interactionCode9d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (impa_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+281) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x28;
  CALL_C(b_+13, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); TAIL(interactionDelete); }
  CYC(b_+16, b_+19);
  CALL_C(b_+19, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+22);
  CALL_C(b_+22, s_objectSetVisible82, SYM(objectSetVisible82), b_+25);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); push_effect(gb, b_+29);
  do { uint16_t jt_ = (impa_jump_table(gb));
    if (jt_ == b_+41) goto state0_subid0;
    if (jt_ == b_+87) goto state0_subid1;
    if (jt_ == b_+141) goto state0_subid2;
    if (jt_ == b_+159) goto state0_subid3;
    if (jt_ == b_+177) goto state0_subid4;
    if (jt_ == b_+261) goto state0_subid5;
    HANDOFF(HL);
  } while (0);
state0_subid0:
  CYC(b_+41, b_+43); A = 0x40;
  CALL_C(b_+43, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+46);
  if (F & FC) { CYCT(b_+46, b_+49); TAIL(interactionDelete); }
  CYC(b_+46, b_+49);
  CALL_C(b_+49, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+52);
  CYC(b_+52, b_+54); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto state0_func_5d73; }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); A = 0x1f;
  CALL_C(b_+58, s_playSound, SYM(playSound_b00), b_+61);
  CYC(b_+61, b_+64); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+64, b_+67); mem_wr(gb, wActiveMusic, A);
  CYC(b_+67, b_+69);
  goto L_5d80;
state0_func_5d73:
  CYC(b_+69, b_+70); H = D;
  CYC(b_+70, b_+72); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x28);
  CYC(b_+74, b_+75); L = alu_inc8(gb, L);
  CYC(b_+75, b_+76); L = alu_inc8(gb, L);
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x18);
  CYC(b_+78, b_+80); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x01);
L_5d80:
  CYC(b_+82, b_+85); SET_HL(SYM(impaScript_afterOnoxTakesDin));
  CYC(b_+85, b_+87);
  goto state0_setScript;
state0_subid1:
  CALL_C(b_+87, s_checkZeldaVillagersSeenButNoMakuSeed_hook, SYM(checkZeldaVillagersSeenButNoMakuSeed), b_+90);
  if (!(F & FZ)) { CYCT(b_+90, b_+93); TAIL(interactionDelete); }
  CYC(b_+90, b_+93);
  CALL_C(b_+93, s_checkGotMakuSeedDidNotSeeZeldaKidnapped_body_hook, SYM(checkGotMakuSeedDidNotSeeZeldaKidnapped_body), b_+96);
  if (!(F & FZ)) { CYCT(b_+96, b_+99); TAIL(interactionDelete); }
  CYC(b_+96, b_+99);
  CYC(b_+99, b_+101); A = 0x22;
  CALL_C(b_+101, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+104);
  if (F & FZ) { CYCT(b_+104, b_+106); goto L_5da0; }
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+108); A = 0x23;
  CALL_C(b_+108, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+111);
  if (F & FZ) { CYCT(b_+111, b_+114); TAIL(interactionDelete); }
  CYC(b_+111, b_+114);
L_5da0:
  CALL_C(b_+114, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+117);
  if (F & FZ) { CYCT(b_+117, b_+119); goto state0_func_5db1; }
  CYC(b_+117, b_+119);
  CYC(b_+119, b_+121); A = 0x1f;
  CALL_C(b_+121, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+124);
  if (F & FZ) { CYCT(b_+124, b_+127); goto state0_func_5db1; }
  CYC(b_+124, b_+127);
  CYC(b_+127, b_+129); A = 0x0c;
  CYC(b_+129, b_+131);
  goto state0_func_5dd5;
state0_func_5db1:
  CYC(b_+131, b_+133); A = 0x40;
  CALL_C(b_+133, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+136);
  CALL_C(b_+136, s_getHighestSetBit, SYM(getHighestSetBit), b_+139);
  CYC(b_+139, b_+141);
  goto state0_func_5dd5;
state0_subid2:
  CALL_C(b_+141, s_checkZeldaVillagersSeenButNoMakuSeed_hook, SYM(checkZeldaVillagersSeenButNoMakuSeed), b_+144);
  if (F & FZ) { CYCT(b_+144, b_+147); TAIL(interactionDelete); }
  CYC(b_+144, b_+147);
  CYC(b_+147, b_+149); A = 0x03;
  CYC(b_+149, b_+151); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CALL_C(b_+152, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+155);
  CYC(b_+155, b_+157); A = 0x08;
  CYC(b_+157, b_+159);
  goto state0_func_5dd5;
state0_subid3:
  CALL_C(b_+159, s_checkGotMakuSeedDidNotSeeZeldaKidnapped_body_hook, SYM(checkGotMakuSeedDidNotSeeZeldaKidnapped_body), b_+162);
  if (F & FZ) { CYCT(b_+162, b_+165); TAIL(interactionDelete); }
  CYC(b_+162, b_+165);
  CYC(b_+165, b_+167); A = 0x09;
state0_func_5dd5:
  CYC(b_+167, b_+170); SET_HL(SYM(table_5ec8));
  CYC(b_+170, b_+171); impa_add_double_index(gb, b_+171);
  CYC(b_+171, b_+172); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+172, b_+173); H = mem_rd(gb, HL);
  CYC(b_+173, b_+174); L = A;
state0_setScript:
  CYC(b_+174, b_+177);
  TAIL(interactionSetScript);
state0_subid4:
  CALL_C(b_+177, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+180);
  if (F & FZ) { CYCT(b_+180, b_+183); TAIL(interactionDelete); }
  CYC(b_+180, b_+183);
  CYC(b_+183, b_+185); A = 0x40;
  CALL_C(b_+185, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+188);
  if (!(F & FC)) { CYCT(b_+188, b_+191); TAIL(interactionDelete); }
  CYC(b_+188, b_+191);
  CYC(b_+191, b_+193); alu_and(gb, 0x02);
  if (F & FZ) { CYCT(b_+193, b_+196); TAIL(interactionDelete); }
  CYC(b_+193, b_+196);
  CYC(b_+196, b_+198); A = 0x22;
  CALL_C(b_+198, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+201);
  if (!(F & FZ)) { CYCT(b_+201, b_+204); TAIL(interactionDelete); }
  CYC(b_+201, b_+204);
  CYC(b_+204, b_+207); SET_BC(IO_P1);
  CALL_C(b_+207, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+210);
  CYC(b_+210, b_+213); SET_HL(SYM(simulatedInput_5ec3));
  CYC(b_+213, b_+215); A = 0x0a;
  CYC(b_+215, b_+216); push_effect(gb, DE);
  CALL_C(b_+216, s_setSimulatedInputAddress, SYM(setSimulatedInputAddress), b_+219);
  CYC(b_+219, b_+220); SET_DE(pop_effect(gb));
  CYC(b_+220, b_+223); SET_HL(w1Link_yh);
  CYC(b_+223, b_+225); mem_wr(gb, HL, 0x76);
  CYC(b_+225, b_+226); L = alu_inc8(gb, L);
  CYC(b_+226, b_+227); L = alu_inc8(gb, L);
  CYC(b_+227, b_+229); mem_wr(gb, HL, 0x56);
  CALL_C(b_+229, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+232);
  if (!(F & FZ)) { CYCT(b_+232, b_+234); goto L_5e26; }
  CYC(b_+232, b_+234);
  CYC(b_+234, b_+236); mem_wr(gb, HL, 0x2a);
  CYC(b_+236, b_+237); L = alu_inc8(gb, L);
  CYC(b_+237, b_+239); mem_wr(gb, HL, 0x0a);
  CYC(b_+239, b_+241); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+241, b_+243); mem_wr(gb, HL, 0x40);
  CYC(b_+243, b_+244); L = alu_inc8(gb, L);
  CYC(b_+244, b_+245); mem_wr(gb, HL, D);
  CALL_C(b_+245, s_objectCopyPosition, SYM(objectCopyPosition), b_+248);
L_5e26:
  CALL_C(b_+248, s_objectSetInvisible, SYM(objectSetInvisible), b_+251);
  CALL_C(b_+251, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+254);
  CYC(b_+254, b_+256); A = 0x0a;
  CYC(b_+256, b_+259); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+259, b_+261);
  goto state0_func_5dd5;
state0_subid5:
  CYC(b_+261, b_+263); A = 0x22;
  CALL_C(b_+263, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+266);
  if (F & FZ) { CYCT(b_+266, b_+269); TAIL(interactionDelete); }
  CYC(b_+266, b_+269);
  CYC(b_+269, b_+271); A = 0x23;
  CALL_C(b_+271, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+274);
  if (!(F & FZ)) { CYCT(b_+274, b_+277); TAIL(interactionDelete); }
  CYC(b_+274, b_+277);
  CYC(b_+277, b_+279); A = 0x0b;
  CYC(b_+279, b_+281);
  goto state0_func_5dd5;
state1:
  CALL_C(b_+281, s_interactionRunScript, SYM(interactionRunScript), b_+284);
  CYC(b_+284, b_+286); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+286, b_+287); A = mem_rd(gb, DE);
  CYC(b_+287, b_+288); push_effect(gb, b_+288);
  do { uint16_t jt_ = (impa_jump_table(gb));
    if (jt_ == b_+300) goto state1_subid0;
    if (jt_ == b_+349) goto state1_faceLinkAndAnimate;
    if (jt_ == b_+360) goto state1_animateAsNPC;
    if (jt_ == b_+352) goto state1_subid3;
    if (jt_ == b_+363) goto state1_subid4;
    HANDOFF(HL);
  } while (0);
state1_subid0:
  CALL_C(b_+300, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+303);
  if (!(F & FZ)) { CYCT(b_+303, b_+305); goto L_5e76; }
  CYC(b_+303, b_+305);
  CYC(b_+305, b_+306); A = alu_inc8(gb, A);
  CYC(b_+306, b_+307); mem_wr(gb, DE, A);
  CYC(b_+307, b_+309); A = 0x08;
  CALL_C(b_+309, s_setLinkIDOverride, SYM(setLinkIDOverride), b_+312);
  CYC(b_+312, b_+314); L = 0x02;
  CYC(b_+314, b_+316); mem_wr(gb, HL, 0x02);
  CYC(b_+316, b_+318); L = 0x0b;
  CYC(b_+318, b_+320); mem_wr(gb, HL, 0x48);
  CYC(b_+320, b_+322); L = 0x0d;
  CYC(b_+322, b_+324); mem_wr(gb, HL, 0x58);
  CYC(b_+324, b_+326); L = 0x08;
  CYC(b_+326, b_+328); mem_wr(gb, HL, 0x00);
L_5e76:
  CALL_C(b_+328, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+331);
  CYC(b_+331, b_+333); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+333, b_+335); goto L_5e82; }
  CYC(b_+333, b_+335);
  CYC(b_+335, b_+337); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+337, b_+339); A = 0x01;
  CYC(b_+339, b_+340); mem_wr(gb, DE, A);
L_5e82:
  CALL_C(b_+340, s_interactionAnimate, SYM(interactionAnimate), b_+343);
  CALL_C(b_+343, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+346);
  CYC(b_+346, b_+349);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
state1_faceLinkAndAnimate:
  CYC(b_+349, b_+352);
  TAIL_SG(npcFaceLinkAndAnimate);
state1_subid3:
  CYC(b_+352, b_+354); A = 0x26;
  CALL_C(b_+354, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+357);
  if (!(F & FZ)) { CYCT(b_+357, b_+360); TAIL_SG(npcFaceLinkAndAnimate); }
  CYC(b_+357, b_+360);
state1_animateAsNPC:
  CYC(b_+360, b_+363);
  TAIL(interactionAnimateAsNpc);
state1_subid4:
  CALL_C(b_+363, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+366);
  if (!(F & FZ)) { CYCT(b_+366, b_+368); TAIL_S(func_5eb1); }
  CYC(b_+366, b_+368);
  CYC(b_+368, b_+371); A = mem_rd(gb, wUseSimulatedInput);
  CYC(b_+371, b_+372); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+372); return; }
  CYC(b_+372, b_+373);
  CYC(b_+373, b_+374); alu_xor(gb, A);
  CYC(b_+374, b_+377); mem_wr(gb, wUseSimulatedInput, A);
  CYC(b_+377, b_+378); A = alu_inc8(gb, A);
  CYC(b_+378, b_+381); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+381, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+384);
  CYC(b_+384, b_+387);
  TAIL(objectSetVisible);
}

void s_func_5eb1_hook(GB *gb) {
  BASE(func_5eb1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+4, s_seasonsFunc_0a_6710_hook, SYM(seasonsFunc_0a_6710), b_+7);
  else CYC(b_+4, b_+7);
  CALL_C(b_+7, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+10);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); TAIL(interactionAnimate); }
  CYC(b_+14, b_+17);
  RET(b_+17); return;
}

