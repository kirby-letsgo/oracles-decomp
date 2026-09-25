#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/dinDancingEvent.s.
// INTERAC_DIN_DANCING_EVENT

static uint16_t dinDancingEvent_jump_table(GB *gb) {
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

static void dinDancingEvent_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode4e@state1@subid7@func_76e9
static void dinDancingEvent_state1_subid7_func_76e9(GB *gb) {
  BASE(interactionCode4e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+465, s_interactionAnimate, SYM(interactionAnimate), b_+468);
  CYC(b_+468, b_+471); A = mem_rd(gb, wFrameCounter);
  CYC(b_+471, b_+473); alu_and(gb, 0x3f);
  if (!(F & FZ)) { RET_TAKEN(b_+473); return; }
  CYC(b_+473, b_+474);
  CYC(b_+474, b_+476); A = 0xd3;
  CYC(b_+476, b_+479);
  TAIL(playSound_b00);
}

// interactionCode4e@state1@func_7612
static void dinDancingEvent_state1_func_7612(GB *gb) {
  BASE(interactionCode4e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+250, b_+252); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+252, b_+253); A = mem_rd(gb, DE);
  CYC(b_+253, b_+256); SET_HL(b_+266 /* @state1@table_7622 */);
  CYC(b_+256, b_+257); dinDancingEvent_add_double_index(gb, b_+257);
  CYC(b_+257, b_+258); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+258, b_+260); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+260, b_+261); mem_wr(gb, DE, A);
  CYC(b_+261, b_+262); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+262, b_+264); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+264, b_+265); mem_wr(gb, DE, A);
  RET(b_+265); return;
}

// interactionCode4e@state1@subid9@runScriptPushLinkAway
static void dinDancingEvent_state1_subid9_runScriptPushLinkAway(GB *gb) {
  BASE(interactionCode4e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+167, s_interactionRunScript, SYM(interactionRunScript), b_+170);
  CYC(b_+170, b_+173);
  TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
}

// interactionCode4e@state1@subid9@func75b5
static void dinDancingEvent_state1_subid9_func75b5(GB *gb) {
  BASE(interactionCode4e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+157, b_+160); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+160, b_+161); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+161, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+164);
  else CYC(b_+161, b_+164);
  CALL_C(b_+164, s_interactionAnimate, SYM(interactionAnimate), b_+167);
  CALL_C(b_+167, s_interactionRunScript, SYM(interactionRunScript), b_+170);
  CYC(b_+170, b_+173);
  TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
}

// INTERAC_DIN_DANCING_EVENT
void s_interactionCode4e_hook(GB *gb) {
  BASE(interactionCode4e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dinDancingEvent_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+101) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto func_754f; }
  CYC(b_+16, b_+18);
  CALL_C(b_+18, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+23); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+23, b_+26); goto seasonsFunc_08_754c; }
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+29); SET_HL(SYM(objectData7e4e));
  CALL_C(b_+29, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+32);
  CYC(b_+32, b_+35); SET_HL(wInteractionIDToLoadExtraGfx);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x4e);
  CYC(b_+37, b_+38); SET_HL(HL + 1);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x06);
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CYC(b_+41, b_+44); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+44, b_+45); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);
  CYC(b_+47, b_+49); A = 0x03;
  CALL_C(b_+49, s_setMusicVolume, SYM(setMusicVolume), b_+52);
seasonsFunc_08_754c:
  CYC(b_+52, b_+55);
  TAIL(interactionDelete);
func_754f:
  CALL_C(b_+55, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+58);
  CYC(b_+58, b_+60); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+63); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+63, b_+65); goto L_7562; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+67, b_+69); A = 0x06;
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);
  CYC(b_+70, b_+71); E = alu_inc8(gb, E);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+74);
  goto L_757a;
L_7562:
  CYC(b_+74, b_+77); SET_HL(SYM(table_770a));
  CYC(b_+77, b_+78); dinDancingEvent_add_double_index(gb, b_+78);
  CYC(b_+78, b_+79); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+79, b_+80); H = mem_rd(gb, HL);
  CYC(b_+80, b_+81); L = A;
  CALL_C(b_+81, s_interactionSetScript, SYM(interactionSetScript), b_+84);
  CYC(b_+84, b_+86); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+86, b_+87); A = mem_rd(gb, DE);
  CYC(b_+87, b_+89); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+89, b_+92); goto L_757a; }
  CYC(b_+89, b_+92);
  CALL_C(b_+92, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+95);
  CYC(b_+95, b_+98);
  TAIL(objectSetVisible80);
L_757a:
  CALL_C(b_+98, s_objectSetVisible83, SYM(objectSetVisible83), b_+101);
state1:
  CYC(b_+101, b_+103); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+105); push_effect(gb, b_+105);
  do { uint16_t jt_ = (dinDancingEvent_jump_table(gb));
    if (jt_ == b_+145) goto state1_subid0;
    if (jt_ == b_+127) goto state1_subid5;
    if (jt_ == b_+276) goto state1_subid6;
    if (jt_ == b_+415) goto state1_subid7;
    if (jt_ == b_+479) goto state1_subidA;
    HANDOFF(HL);
  } while (0);
state1_subid5:
  CYC(b_+127, b_+130); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+130, b_+131); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+131, b_+133); goto L_75a3; }
  CYC(b_+131, b_+133);
  CALL_C(b_+133, s_interactionAnimate, SYM(interactionAnimate), b_+136);
  CYC(b_+136, b_+139);
  TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
L_75a3:
  CALL_C(b_+139, s_objectCreatePuff, SYM(objectCreatePuff), b_+142);
  CYC(b_+142, b_+145);
  TAIL(interactionDelete);
state1_subid0:
  CYC(b_+145, b_+147); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+147, b_+148); A = mem_rd(gb, DE);
  CYC(b_+148, b_+149); push_effect(gb, b_+149);
  do { uint16_t jt_ = (dinDancingEvent_jump_table(gb));
    if (jt_ == b_+157) goto state1_subid9_func75b5;
    if (jt_ == b_+173) goto state1_subid9_substate1;
    if (jt_ == b_+210) goto state1_subid9_substate2;
    if (jt_ == b_+237) goto state1_subid9_substate3;
    HANDOFF(HL);
  } while (0);
state1_subid9_func75b5:
  CYC(b_+157, b_+160); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+160, b_+161); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+161, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+164);
  else CYC(b_+161, b_+164);
state1_subid9_animate:
  CALL_C(b_+164, s_interactionAnimate, SYM(interactionAnimate), b_+167);
state1_subid9_runScriptPushLinkAway:
  CALL_C(b_+167, s_interactionRunScript, SYM(interactionRunScript), b_+170);
  CYC(b_+170, b_+173);
  TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
state1_subid9_substate1:
  CYC(b_+173, b_+175); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+175, b_+176); A = mem_rd(gb, DE);
  CYC(b_+176, b_+179); SET_HL(SYM(bitTable));
  CYC(b_+179, b_+180); alu_add(gb, L);
  CYC(b_+180, b_+181); L = A;
  CYC(b_+181, b_+182); B = mem_rd(gb, HL);
  CYC(b_+182, b_+185); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+185, b_+186); alu_and(gb, B);
  if (F & FZ) { CYCT(b_+186, b_+188); goto L_75df; }
  CYC(b_+186, b_+188);
  CALL_C(b_+188, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+191);
  CYC(b_+191, b_+193); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+193, b_+195); mem_wr(gb, HL, 0x20);
  CYC(b_+195, b_+197); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+197, b_+198); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+198, b_+199); mem_wr(gb, HL, A);
L_75df:
  CYC(b_+199, b_+201); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+201, b_+202); A = mem_rd(gb, DE);
  CYC(b_+202, b_+204); alu_cp(gb, 0x05);
  if (F & FZ) CALL_C_CC(b_+204, s_interactionAnimate, SYM(interactionAnimate), b_+207);
  else CYC(b_+204, b_+207);
  CYC(b_+207, b_+210);
  goto state1_subid9_runScriptPushLinkAway;
state1_subid9_substate2:
  CALL_C(b_+210, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+213);
  if (!(F & FZ)) { CYCT(b_+213, b_+215); goto L_75f5; }
  CYC(b_+213, b_+215);
  CALL_L(b_+215, dinDancingEvent_state1_func_7612, b_+218);
  CYC(b_+218, b_+221);
  TAIL(interactionIncSubstate);
L_75f5:
  CALL_C(b_+221, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+224);
  CYC(b_+224, b_+226); alu_and(gb, 0x0f);
  CYC(b_+226, b_+228); alu_sub(gb, 0x08);
  CYC(b_+228, b_+229); H = D;
  CYC(b_+229, b_+231); L = INTERACTION_BASE + OBJ_X;
  CYC(b_+231, b_+232); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+232, b_+233); L = alu_inc8(gb, L);
  CYC(b_+233, b_+234); mem_wr(gb, HL, A);
  CYC(b_+234, b_+237);
  goto state1_subid9_runScriptPushLinkAway;
state1_subid9_substate3:
  CALL_C(b_+237, s_objectApplySpeed, SYM(objectApplySpeed), b_+240);
  CALL_C(b_+240, s_objectApplySpeed, SYM(objectApplySpeed), b_+243);
  CALL_C(b_+243, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+246);
  if (F & FC) { RET_TAKEN(b_+246); return; }
  CYC(b_+246, b_+247);
  CYC(b_+247, b_+250);
  TAIL(interactionDelete);

state1_subid6:
  CYC(b_+276, b_+278); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+278, b_+279); A = mem_rd(gb, DE);
  CYC(b_+279, b_+280); push_effect(gb, b_+280);
  do { uint16_t jt_ = (dinDancingEvent_jump_table(gb));
    if (jt_ == b_+294) goto state1_subid6_substate0;
    if (jt_ == b_+311) goto state1_subid6_substate1;
    if (jt_ == b_+326) goto state1_subid6_substate2;
    if (jt_ == b_+356) goto state1_subid6_substate3;
    if (jt_ == b_+367) goto state1_subid6_substate4;
    if (jt_ == b_+381) goto state1_subid6_substate5;
    if (jt_ == b_+399) goto state1_subid6_substate6;
    HANDOFF(HL);
  } while (0);
state1_subid6_substate0:
  CYC(b_+294, b_+297); A = mem_rd(gb, wTmpcfc0 + 0x13);
  CYC(b_+297, b_+299); alu_cp(gb, 0x3f);
  if (!(F & FZ)) { CYCT(b_+299, b_+302); goto state1_subid9_func75b5; }
  CYC(b_+299, b_+302);
  CALL_C(b_+302, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+305);
  CYC(b_+305, b_+308); SET_HL(SYM(troupeScript_startDanceScene));
  CALL_C(b_+308, s_interactionSetScript, SYM(interactionSetScript), b_+311);
state1_subid6_substate1:
  CALL_L(b_+311, dinDancingEvent_state1_subid9_func75b5, b_+314);
  CYC(b_+314, b_+317); A = mem_rd(gb, wTmpcfc0 + 0x13);
  CYC(b_+317, b_+319); alu_and(gb, 0x40);
  if (F & FZ) { RET_TAKEN(b_+319); return; }
  CYC(b_+319, b_+320);
  CALL_C(b_+320, s_fastFadeoutToWhite, SYM(fastFadeoutToWhite), b_+323);
  CYC(b_+323, b_+326);
  TAIL(interactionIncSubstate);
state1_subid6_substate2:
  CYC(b_+326, b_+329); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+329, b_+330); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+330); return; }
  CYC(b_+330, b_+331);
  CYC(b_+331, b_+333); A = 0x80;
  CYC(b_+333, b_+336); mem_wr(gb, wTmpcfc0 + 0x13, A);
  CYC(b_+336, b_+338); A = 0x06;
  CYC(b_+338, b_+341); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+341, b_+343); A = 0x08;
  CALL_C(b_+343, s_setLinkIDOverride, SYM(setLinkIDOverride), b_+346);
  CYC(b_+346, b_+348); L = 0x02;
  CYC(b_+348, b_+350); mem_wr(gb, HL, 0x01);
  CYC(b_+350, b_+352); L = 0x19;
  CYC(b_+352, b_+353); mem_wr(gb, HL, D);
  CYC(b_+353, b_+356);
  TAIL(interactionIncSubstate);
state1_subid6_substate3:
  CYC(b_+356, b_+359); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+359, b_+360); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+360); return; }
  CYC(b_+360, b_+361);
  CALL_L(b_+361, dinDancingEvent_state1_subid9_runScriptPushLinkAway, b_+364);
  CYC(b_+364, b_+367);
  TAIL(interactionIncSubstate);
state1_subid6_substate4:
  CYC(b_+367, b_+370); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+370, b_+372); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+372); return; }
  CYC(b_+372, b_+373);
  CALL_C(b_+373, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+376);
  CYC(b_+376, b_+378); A = 0x0d;
  CYC(b_+378, b_+381);
  TAIL(interactionSetAnimation);
state1_subid6_substate5:
  CYC(b_+381, b_+384); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+384, b_+386); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+386); return; }
  CYC(b_+386, b_+387);
  CALL_C(b_+387, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+390);
  CYC(b_+390, b_+392); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+392, b_+394); mem_wr(gb, HL, 0x0a);
  CYC(b_+394, b_+396); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+396, b_+398); mem_wr(gb, HL, 0x08);
  RET(b_+398); return;
state1_subid6_substate6:
  CALL_C(b_+399, s_objectApplySpeed, SYM(objectApplySpeed), b_+402);
  CYC(b_+402, b_+405); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+405, b_+406); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+406); return; }
  CYC(b_+406, b_+407);
  CYC(b_+407, b_+410); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+410, b_+412); mem_wr(gb, HL, 0x08);
  CYC(b_+412, b_+415);
  TAIL(interactionDelete);
state1_subid7:
  CYC(b_+415, b_+417); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+417, b_+418); A = mem_rd(gb, DE);
  CYC(b_+418, b_+419); push_effect(gb, b_+419);
  do { uint16_t jt_ = (dinDancingEvent_jump_table(gb));
    if (jt_ == b_+425) goto state1_subid7_substate0;
    if (jt_ == b_+440) goto state1_subid7_substate1;
    if (jt_ == b_+459) goto state1_subid7_substate2;
    HANDOFF(HL);
  } while (0);
state1_subid7_substate0:
  CALL_C(b_+425, s_interactionRunScript, SYM(interactionRunScript), b_+428);
  if (!(F & FC)) { CYCT(b_+428, b_+430); goto state1_subid7_func_76e9; }
  CYC(b_+428, b_+430);
  CALL_C(b_+430, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+433);
  CYC(b_+433, b_+436); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+436, b_+438); mem_wr(gb, HL, 0x04);
  CYC(b_+438, b_+440);
  goto state1_subid7_func_76e9;
state1_subid7_substate1:
  CALL_L(b_+440, dinDancingEvent_state1_subid7_func_76e9, b_+443);
  CYC(b_+443, b_+446); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+446, b_+447); A = mem_rd(gb, HL);
  CYC(b_+447, b_+449); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(b_+449); return; }
  CYC(b_+449, b_+450);
  CALL_C(b_+450, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+453);
  CYC(b_+453, b_+456); SET_HL(SYM(troupeScript_tornadoEnd));
  CYC(b_+456, b_+459);
  TAIL(interactionSetScript);
state1_subid7_substate2:
  CALL_C(b_+459, s_interactionRunScript, SYM(interactionRunScript), b_+462);
  if (F & FC) { CYCT(b_+462, b_+465); TAIL(interactionDelete); }
  CYC(b_+462, b_+465);
state1_subid7_func_76e9:
  CALL_C(b_+465, s_interactionAnimate, SYM(interactionAnimate), b_+468);
  CYC(b_+468, b_+471); A = mem_rd(gb, wFrameCounter);
  CYC(b_+471, b_+473); alu_and(gb, 0x3f);
  if (!(F & FZ)) { RET_TAKEN(b_+473); return; }
  CYC(b_+473, b_+474);
  CYC(b_+474, b_+476); A = 0xd3;
  CYC(b_+476, b_+479);
  TAIL(playSound_b00);
state1_subidA:
  CALL_C(b_+479, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+482);
  if (!(F & FZ)) { CYCT(b_+482, b_+484); goto L_7707; }
  CYC(b_+482, b_+484);
  CALL_C(b_+484, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+487);
  CYC(b_+487, b_+489); A = 0x28;
  CALL_C(b_+489, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+492);
  if (F & FZ) { CYCT(b_+492, b_+495); TAIL(interactionDelete); }
  CYC(b_+492, b_+495);
L_7707:
  CYC(b_+495, b_+498);
  goto state1_subid9_animate;
}
