#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/datingRosaEvent.s.
// INTERAC_DATING_ROSA_EVENT

static uint16_t datingRosaEvent_jump_table(GB *gb) {
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

static void datingRosaEvent_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode31@subid1@func_628e
static void datingRosaEvent_subid1_func_628e(GB *gb) {
  BASE(interactionCode31);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+449, b_+450); H = D;
  CYC(b_+450, b_+452); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+452, b_+453); A = mem_rd(gb, HL);
  CYC(b_+453, b_+455); L = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+455, b_+456); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+456); return; }
  CYC(b_+456, b_+457);
  CYC(b_+457, b_+458); mem_wr(gb, HL, A);
  CYC(b_+458, b_+461);
  TAIL(interactionSetAnimation);
}

// interactionCode31@subid1@func_61a4
static void datingRosaEvent_subid1_func_61a4(GB *gb) {
  BASE(interactionCode31);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+215, b_+218); A = mem_rd(gb, wActiveGroup);
  CYC(b_+218, b_+220); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+220, b_+222); goto subid1_goToState3; }
  CYC(b_+220, b_+222);
  CYC(b_+222, b_+225); A = mem_rd(gb, wActiveRoom);
  CYC(b_+225, b_+228); SET_HL(b_+272 /* @subid1@roomTable_61dd */);
  CALL_C(b_+228, s_findRoomSpecificData, SYM(findRoomSpecificData), b_+231);
  if (!(F & FC)) { RET_TAKEN(b_+231); return; }
  CYC(b_+231, b_+232);
  CYC(b_+232, b_+233); push_effect(gb, b_+233);
  do { uint16_t jt_ = (datingRosaEvent_jump_table(gb));
    if (jt_ == b_+239) goto subid1_screenAboveRosa;
    if (jt_ == b_+261) goto subid1_beachEntranceScreen;
    if (jt_ == b_+266) goto subid1_goToState3;
    HANDOFF(HL);
  } while (0);
subid1_screenAboveRosa:
  CYC(b_+239, b_+241); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+241, b_+242); A = mem_rd(gb, DE);
  CYC(b_+242, b_+243); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+243, b_+245); goto L_61d0; }
  CYC(b_+243, b_+245);
  CYC(b_+245, b_+248); A = mem_rd(gb, wScrollMode);
  CYC(b_+248, b_+250); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+250, b_+252); goto L_61d0; }
  CYC(b_+250, b_+252);
  CYC(b_+252, b_+254); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+254, b_+256); A = 0x02;
  CYC(b_+256, b_+257); mem_wr(gb, DE, A);
  CYC(b_+257, b_+258); alu_scf(gb);
  RET(b_+258); return;
L_61d0:
  CYC(b_+259, b_+260); alu_xor(gb, A);
  RET(b_+260); return;
subid1_beachEntranceScreen:
  CYC(b_+261, b_+263); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+263, b_+264); alu_xor(gb, A);
  CYC(b_+264, b_+265); mem_wr(gb, DE, A);
  RET(b_+265); return;
subid1_goToState3:
  CYC(b_+266, b_+268); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+268, b_+270); A = 0x03;
  CYC(b_+270, b_+271); mem_wr(gb, DE, A);
  RET(b_+271); return;
}

// interactionCode31@subid1@func_6182
static void datingRosaEvent_subid1_func_6182(GB *gb) {
  BASE(interactionCode31);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+181, b_+182); H = D;
  CYC(b_+182, b_+184); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+184, b_+185); A = mem_rd(gb, HL);
  CYC(b_+185, b_+186); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+186, b_+188); goto L_6191; }
  CYC(b_+186, b_+188);
  CYC(b_+188, b_+189); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+189); return; }
  CYC(b_+189, b_+190);
  CYC(b_+190, b_+193); SET_BC(0xfe40);
  CYC(b_+193, b_+196);
  TAIL(objectSetSpeedZ);
L_6191:
  CYC(b_+196, b_+199); A = mem_rd(gb, wLinkInAir);
  CYC(b_+199, b_+200); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+200); return; }
  CYC(b_+200, b_+201);
  CYC(b_+201, b_+204); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+204, b_+206); alu_and(gb, 0x81);
  if (!(F & FZ)) { RET_TAKEN(b_+206); return; }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+210); A = mem_rd(gb, wMenuDisabled);
  CYC(b_+210, b_+211); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+211); return; }
  CYC(b_+211, b_+212);
  CYC(b_+212, b_+214); mem_wr(gb, HL, 0x10);
  RET(b_+214); return;
}

// makeActiveObjectFollowLink
static void datingRosaEvent_makeActiveObjectFollowLink_makeActiveObjectFollowLink(GB *gb) {
  BASE(paletteTransitionGroup1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+2, b_+5); SET_HL(wFollowingLinkObjectType);
  CYC(b_+5, b_+7); A = mem_rd(gb, hActiveObjectType);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+10); A = mem_rd(gb, hActiveObject);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  s_resetFollowingLinkPath(gb); return; // falls through
}

// makeActiveObjectFollowLink
static void datingRosaEvent_makeActiveObjectFollowLink(GB *gb) {
  BASE(makeActiveObjectFollowLink_b00);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x01;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_L(b_+10, datingRosaEvent_makeActiveObjectFollowLink_makeActiveObjectFollowLink, b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, 0x2222, A);
  RET(b_+19); return;
}

// INTERAC_DATING_ROSA_EVENT
void s_interactionCode31_hook(GB *gb) {
  BASE(interactionCode31);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (datingRosaEvent_jump_table(gb));
    if (jt_ == b_+12) goto subid0;
    if (jt_ == b_+79) goto subid1;
    if (jt_ == b_+461) goto subid2;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (datingRosaEvent_jump_table(gb));
    if (jt_ == b_+20) goto subid3_state0;
    if (jt_ == b_+73) goto subid3_state1;
    HANDOFF(HL);
  } while (0);
subid3_state0:
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+26);
  CYC(b_+26, b_+28); A = 0x0b;
  CALL_C(b_+28, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+34); TAIL(interactionDelete); }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); L = INTERACTION_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x00);
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0xff);
  CYC(b_+43, b_+45); A = 0x0c;
  CALL_C(b_+45, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+48);
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto L_6104; }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+52, b_+54); A = 0x04;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
L_6104:
  CYC(b_+55, b_+58); SET_HL(SYM(rosaScript_goOnDate));
  CALL_C(b_+58, s_interactionSetScript, SYM(interactionSetScript), b_+61);
  CYC(b_+61, b_+63); A = 0x29;
  CALL_C(b_+63, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+66);
  CALL_C(b_+66, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x00);
  CYC(b_+71, b_+73);
  goto L_6119;
subid3_state1:
  CALL_C(b_+73, s_interactionRunScript, SYM(interactionRunScript), b_+76);
L_6119:
  CYC(b_+76, b_+79);
  TAIL(npcFaceLinkAndAnimate);
subid1:
  CYC(b_+79, b_+81); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); push_effect(gb, b_+83);
  do { uint16_t jt_ = (datingRosaEvent_jump_table(gb));
    if (jt_ == b_+91) goto subid1_state0;
    if (jt_ == b_+137) goto subid1_state1;
    if (jt_ == b_+302) goto subid1_state2;
    if (jt_ == b_+415) goto subid1_state3;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+91, b_+93); A = 0x01;
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CALL_C(b_+94, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+97);
  CALL_L(b_+97, datingRosaEvent_makeActiveObjectFollowLink, b_+100);
  CALL_C(b_+100, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+103);
  CALL_C(b_+103, s_objectSetReservedBit1, SYM(objectSetReservedBit1), b_+106);
  CYC(b_+106, b_+108); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x01);
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+112, b_+114); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+114, b_+115); A = mem_rd(gb, DE);
  CYC(b_+115, b_+116); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+116, b_+118); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+118, b_+119); A = mem_rd(gb, DE);
  CYC(b_+119, b_+120); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+122, b_+125); A = mem_rd(gb, w1Link_direction);
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+128); mem_wr(gb, HL, 0x00);
  CALL_C(b_+128, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+131);
  CALL_C(b_+131, s_objectSetVisiblec3, SYM(objectSetVisiblec3), b_+134);
  CYC(b_+134, b_+137);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
subid1_state1:
  CALL_C(b_+137, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+140);
  CALL_L(b_+140, datingRosaEvent_subid1_func_61a4, b_+143);
  if (F & FC) { RET_TAKEN(b_+143); return; }
  CYC(b_+143, b_+144);
  CYC(b_+144, b_+146); C = 0x20;
  CALL_C(b_+146, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+149);
  if (F & FZ) CALL_L_CC(b_+149, datingRosaEvent_subid1_func_6182, b_+152);
  else CYC(b_+149, b_+152);
  CALL_L(b_+152, datingRosaEvent_subid1_func_628e, b_+155);
  CYC(b_+155, b_+156); H = D;
  CYC(b_+156, b_+158); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+158, b_+159); A = mem_rd(gb, HL);
  CYC(b_+159, b_+160); B = A;
  CYC(b_+160, b_+162); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+162, b_+163); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+163, b_+165); goto L_617a; }
  CYC(b_+163, b_+165);
  CYC(b_+165, b_+167); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+167, b_+168); A = mem_rd(gb, HL);
  CYC(b_+168, b_+169); C = A;
  CYC(b_+169, b_+171); L = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+171, b_+172); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+172); return; }
  CYC(b_+172, b_+173);
L_617a:
  CYC(b_+173, b_+175); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+175, b_+176); mem_wr(gb, HL, B);
  CYC(b_+176, b_+177); L = alu_inc8(gb, L);
  CYC(b_+177, b_+178); mem_wr(gb, HL, C);
  CYC(b_+178, b_+181);
  TAIL(interactionAnimate);

subid1_state2:
  CYC(b_+302, b_+304); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+304, b_+305); A = mem_rd(gb, DE);
  CYC(b_+305, b_+306); push_effect(gb, b_+306);
  do { uint16_t jt_ = (datingRosaEvent_jump_table(gb));
    if (jt_ == b_+312) goto subid1_state2_substate0;
    if (jt_ == b_+344) goto subid1_state2_substate1;
    if (jt_ == b_+374) goto subid1_state2_substate2;
    HANDOFF(HL);
  } while (0);
subid1_state2_substate0:
  CYC(b_+312, b_+314); A = 0x01;
  CYC(b_+314, b_+315); mem_wr(gb, DE, A);
  CALL_C(b_+315, s_clearFollowingLinkObject, SYM(clearFollowingLinkObject), b_+318);
  CYC(b_+318, b_+320); A = 0x0b;
  CALL_C(b_+320, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+323);
  CYC(b_+323, b_+325); A = 0x01;
  CYC(b_+325, b_+328); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+328, b_+330); A = 0x02;
  CYC(b_+330, b_+333); mem_wr(gb, w1Link_direction, A);
  CYC(b_+333, b_+335); A = 0x29;
  CALL_C(b_+335, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+338);
  CYC(b_+338, b_+341); SET_HL(SYM(rosaScript_dateEnded));
  CYC(b_+341, b_+344);
  TAIL(interactionSetScript);
subid1_state2_substate1:
  CYC(b_+344, b_+346); C = 0x20;
  CALL_C(b_+346, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+349);
  CALL_C(b_+349, s_interactionAnimate, SYM(interactionAnimate), b_+352);
  CALL_C(b_+352, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+355);
  CALL_C(b_+355, s_interactionRunScript, SYM(interactionRunScript), b_+358);
  if (!(F & FC)) { RET_TAKEN(b_+358); return; }
  CYC(b_+358, b_+359);
  CYC(b_+359, b_+361); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+361, b_+363); A = 0x02;
  CYC(b_+363, b_+364); mem_wr(gb, DE, A);
  CYC(b_+364, b_+367); SET_BC(0x4858);
  CALL_C(b_+367, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+370);
  CYC(b_+370, b_+372); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+372, b_+373); mem_wr(gb, DE, A);
  RET(b_+373); return;
subid1_state2_substate2:
  CALL_C(b_+374, s_interactionAnimate, SYM(interactionAnimate), b_+377);
  CALL_C(b_+377, s_objectApplySpeed, SYM(objectApplySpeed), b_+380);
  CYC(b_+380, b_+382); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+382, b_+383); A = mem_rd(gb, DE);
  CYC(b_+383, b_+385); alu_cp(gb, 0x48);
  if (F & FC) { RET_TAKEN(b_+385); return; }
  CYC(b_+385, b_+386);
  CYC(b_+386, b_+387); H = D;
  CYC(b_+387, b_+389); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+389, b_+391); B = 0x06;
  CALL_C(b_+391, s_clearMemory, SYM(clearMemory), b_+394);
  CYC(b_+394, b_+396); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+396, b_+398); mem_wr(gb, HL, 0x01);
  CYC(b_+398, b_+399); alu_xor(gb, A);
  CYC(b_+399, b_+402); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+402, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+405);
  CYC(b_+405, b_+407); mem_wr(gb, HL, 0x00);
  CYC(b_+407, b_+409); A = 0x28;
  CYC(b_+409, b_+412); mem_wr(gb, wActiveMusic, A);
  CYC(b_+412, b_+415);
  TAIL(playSound_b00);
subid1_state3:
  CALL_C(b_+415, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+418);
  CYC(b_+418, b_+420); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+420, b_+421); A = mem_rd(gb, DE);
  CYC(b_+421, b_+422); push_effect(gb, b_+422);
  do { uint16_t jt_ = (datingRosaEvent_jump_table(gb));
    if (jt_ == b_+426) goto subid1_state3_substate0;
    if (jt_ == b_+443) goto subid1_state3_substate1;
    HANDOFF(HL);
  } while (0);
subid1_state3_substate0:
  CYC(b_+426, b_+428); A = 0x01;
  CYC(b_+428, b_+429); mem_wr(gb, DE, A);
  CALL_C(b_+429, s_clearFollowingLinkObject, SYM(clearFollowingLinkObject), b_+432);
  CYC(b_+432, b_+434); A = 0x0b;
  CALL_C(b_+434, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+437);
  CYC(b_+437, b_+440); SET_BC(0x291a);
  CYC(b_+440, b_+443);
  TAIL(showText);
subid1_state3_substate1:
  CALL_C(b_+443, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+446);
  CYC(b_+446, b_+449);
  TAIL(interactionDelete);

subid2:
  CYC(b_+461, b_+463); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+463, b_+464); A = mem_rd(gb, DE);
  CYC(b_+464, b_+465); push_effect(gb, b_+465);
  do { uint16_t jt_ = (datingRosaEvent_jump_table(gb));
    if (jt_ == b_+469) goto subid2_state0;
    if (jt_ == b_+526) goto subid2_state1;
    HANDOFF(HL);
  } while (0);
subid2_state0:
  CYC(b_+469, b_+471); A = 0x01;
  CYC(b_+471, b_+472); mem_wr(gb, DE, A);
  CYC(b_+472, b_+474); A = 0x0e;
  CALL_C(b_+474, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+477);
  if (!(F & FZ)) { CYCT(b_+477, b_+480); TAIL(interactionDelete); }
  CYC(b_+477, b_+480);
  CYC(b_+480, b_+483); A = mem_rd(gb, wcc84);
  CYC(b_+483, b_+484); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+484, b_+487); TAIL(interactionDelete); }
  CYC(b_+484, b_+487);
  CYC(b_+487, b_+488); A = D;
  CYC(b_+488, b_+491); mem_wr(gb, wcc84, A);
  CYC(b_+491, b_+492); H = D;
  CYC(b_+492, b_+494); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+494, b_+496); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CALL_C(b_+496, s_getRandomNumber, SYM(getRandomNumber), b_+499);
  CYC(b_+499, b_+501); alu_and(gb, 0x03);
  CYC(b_+501, b_+504); SET_HL(b_+518 /* @subid2@table_62d3 */);
  CYC(b_+504, b_+505); datingRosaEvent_add_double_index(gb, b_+505);
  CYC(b_+505, b_+507); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+507, b_+508); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+508, b_+509); mem_wr(gb, DE, A);
  CYC(b_+509, b_+510); E = alu_inc8(gb, E);
  CYC(b_+510, b_+511); A = mem_rd(gb, HL);
  CYC(b_+511, b_+512); mem_wr(gb, DE, A);
  CYC(b_+512, b_+514); A = 0xff;
  CYC(b_+514, b_+516); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+516, b_+517); mem_wr(gb, DE, A);
  RET(b_+517); return;

subid2_state1:
  CYC(b_+526, b_+528); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+528, b_+529); A = mem_rd(gb, DE);
  CYC(b_+529, b_+530); B = A;
  CYC(b_+530, b_+533); A = mem_rd(gb, wActiveRoom);
  CYC(b_+533, b_+534); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+534); return; }
  CYC(b_+534, b_+535);
  CYC(b_+535, b_+536); mem_wr(gb, DE, A);
  CYC(b_+536, b_+537); B = A;
  CYC(b_+537, b_+539); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+539, b_+540); A = mem_rd(gb, DE);
  CYC(b_+540, b_+541); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+541, b_+543); goto L_62fd; }
  CYC(b_+541, b_+543);
  CALL_C(b_+543, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+546);
  if (!(F & FZ)) { RET_TAKEN(b_+546); return; }
  CYC(b_+546, b_+547);
  CYC(b_+547, b_+549); mem_wr(gb, HL, 0x60);
  CYC(b_+549, b_+550); L = alu_inc8(gb, L);
  CYC(b_+550, b_+552); mem_wr(gb, HL, 0x45);
  CYC(b_+552, b_+554); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+554, b_+555); A = mem_rd(gb, DE);
  CYC(b_+555, b_+557); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+557, b_+560);
  TAIL(setShortPosition);
L_62fd:
  CYC(b_+560, b_+562); A = 0x45;
  CALL_C(b_+562, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+565);
  if (F & FC) { CYCT(b_+565, b_+567); goto L_630f; }
  CYC(b_+565, b_+567);
  CYC(b_+567, b_+568); A = B;
  CYC(b_+568, b_+570); alu_cp(gb, 0x60);
  if (!(F & FC)) { RET_TAKEN(b_+570); return; }
  CYC(b_+570, b_+571);
L_6308:
  CYC(b_+571, b_+572); alu_xor(gb, A);
  CYC(b_+572, b_+575); mem_wr(gb, wcc84, A);
  CYC(b_+575, b_+578);
  TAIL(interactionDelete);
L_630f:
  CYC(b_+578, b_+580); A = 0x0e;
  CALL_C(b_+580, s_setGlobalFlag, SYM(setGlobalFlag), b_+583);
  CYC(b_+583, b_+585);
  goto L_6308;
}
