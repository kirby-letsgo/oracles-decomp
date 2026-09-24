#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/flyingRooster.s.
// INTERAC_FLYING_ROOSTER
// Variables:
// var30/var31: Initial position
// var32: Y-position necessary to clear the cliff
// var33: Counter used along with var34
// var34: Direction chicken is hopping in (up or down; when moving back to "base"
// position)
// var35: X-position at which the "destination" is (Link loses control)

static uint16_t flyingRooster_jump_table(GB *gb) {
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

static void flyingRooster_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void flyingRooster_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_FLYING_ROOSTER
// Variables:
// var30/var31: Initial position
// var32: Y-position necessary to clear the cliff
// var33: Counter used along with var34
// var34: Direction chicken is hopping in (up or down; when moving back to "base"
// position)
// var35: X-position at which the "destination" is (Link loses control)
void s_interactionCode8c_hook(GB *gb) {
  BASE(interactionCode8c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); TAIL_S(flyingRooster_subidBit7Set); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); TAIL(interactionAnimate); }
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); push_effect(gb, b_+19);
  do { uint16_t jt_ = (flyingRooster_jump_table(gb));
    if (jt_ == b_+33) goto state0;
    if (jt_ == b_+77) goto state1;
    if (jt_ == b_+95) goto state2;
    if (jt_ == b_+261) goto state3;
    if (jt_ == b_+397) goto state4;
    if (jt_ == b_+475) goto state5;
    if (jt_ == b_+382) goto state6;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+33, b_+35); A = 0x01;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38); A = 0x02;
  CALL_C(b_+38, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+41);
  CALL_C(b_+41, s_flyingRooster_getSubidAndInitSpeed_hook, SYM(flyingRooster_getSubidAndInitSpeed), b_+44);
  CYC(b_+44, b_+46); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); mem_wr(gb, HL, A);
  CYC(b_+54, b_+55); A = C;
  CYC(b_+55, b_+58); SET_HL(b_+73 /* @subidData */);
  CYC(b_+58, b_+59); flyingRooster_add_double_index(gb, b_+59);
  CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+60, b_+62); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+65); E = INTERACTION_BASE + OBJ_SCRIPT_RET;
  CYC(b_+65, b_+66); A = mem_rd(gb, HL);
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CALL_C(b_+67, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+70);
  CYC(b_+70, b_+73);
  TAIL(objectSetVisiblec2);

state1:
  CALL_C(b_+77, s_interactionAnimate, SYM(interactionAnimate), b_+80);
  CALL_C(b_+80, s_objectAddToGrabbableObjectBuffer, SYM(objectAddToGrabbableObjectBuffer), b_+83);
  CYC(b_+83, b_+85); C = 0x10;
  CALL_C(b_+85, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+88);
  if (!(F & FZ)) { RET_TAKEN(b_+88); return; }
  CYC(b_+88, b_+89);
  CYC(b_+89, b_+92); SET_BC(IO_P1);
  CYC(b_+92, b_+95);
  TAIL(objectSetSpeedZ);
state2:
  CALL_C(b_+95, s_interactionAnimate, SYM(interactionAnimate), b_+98);
  CYC(b_+98, b_+100); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+102); push_effect(gb, b_+102);
  do { uint16_t jt_ = (flyingRooster_jump_table(gb));
    if (jt_ == b_+112) goto justGrabbed;
    if (jt_ == b_+127) goto state2Substate1;
    if (jt_ == b_+162) goto state2Substate2;
    if (jt_ == b_+441) goto releaseFromLink;
    if (jt_ == b_+218) goto state2Substate4;
    HANDOFF(HL);
  } while (0);
justGrabbed:
  CYC(b_+112, b_+114); A = 0x01;
  CYC(b_+114, b_+115); mem_wr(gb, DE, A);
  CYC(b_+115, b_+118); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+118, b_+121); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+121, b_+123); A = 0x08;
  CYC(b_+123, b_+126); mem_wr(gb, wLinkGrabState2, A);
  RET(b_+126); return;
state2Substate1:
  CYC(b_+127, b_+130); A = mem_rd(gb, wLinkInAir);
  CYC(b_+130, b_+131); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+131); return; }
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+135); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+135, b_+137); alu_and(gb, 0x07);
  CYC(b_+137, b_+139); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+139); return; }
  CYC(b_+139, b_+140);
  CYC(b_+140, b_+143); SET_HL(w1Link_direction);
  CYC(b_+143, b_+145); mem_wr(gb, HL, 0x01);
  CYC(b_+145, b_+147); A = 0x01;
  CYC(b_+147, b_+150); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+150, b_+152); L = 0x0f;
  CYC(b_+152, b_+153); A = mem_rd(gb, HL);
  CYC(b_+153, b_+154); A = alu_dec8(gb, A);
  CYC(b_+154, b_+155); mem_wr(gb, HL, A);
  CYC(b_+155, b_+157); alu_cp(gb, 0xf8);
  if (!(F & FZ)) { RET_TAKEN(b_+157); return; }
  CYC(b_+157, b_+158);
  CYC(b_+158, b_+160); A = 0x02;
  CYC(b_+160, b_+161); mem_wr(gb, DE, A);
  RET(b_+161); return;
state2Substate2:
  CYC(b_+162, b_+164); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+164, b_+165); A = mem_rd(gb, DE);
  CYC(b_+165, b_+166); B = A;
  CYC(b_+166, b_+167); E = alu_inc8(gb, E);
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+169); C = A;
  CYC(b_+169, b_+170); push_effect(gb, DE);
  CYC(b_+170, b_+173); SET_DE(w1Link_yh);
  CALL_C(b_+173, s_getRelativeAngle, SYM(getRelativeAngle), b_+176);
  CYC(b_+176, b_+177); SET_DE(pop_effect(gb));
  CYC(b_+177, b_+179); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+179, b_+180); mem_wr(gb, DE, A);
  CALL_C(b_+180, s_flyingRooster_applySpeedAndUpdatePositions_hook, SYM(flyingRooster_applySpeedAndUpdatePositions), b_+183);
  CYC(b_+183, b_+184); H = D;
  CYC(b_+184, b_+186); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+186, b_+187); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+187, b_+188); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+188); return; }
  CYC(b_+188, b_+189);
  CYC(b_+189, b_+190); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+190, b_+191); alu_cp(gb, C);
  if (!(F & FZ)) { RET_TAKEN(b_+191); return; }
  CYC(b_+191, b_+192);
  CYC(b_+192, b_+194); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+194, b_+196); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+196, b_+198); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+198, b_+200); mem_wr(gb, HL, 0x04);
  CYC(b_+200, b_+202); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+202, b_+203); A = mem_rd(gb, DE);
  CYC(b_+203, b_+206); SET_HL(b_+216 /* @angles */);
  CYC(b_+206, b_+207); flyingRooster_add_a_to_hl(gb, b_+207);
  CYC(b_+207, b_+208); A = mem_rd(gb, HL);
  CYC(b_+208, b_+210); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+210, b_+211); mem_wr(gb, DE, A);
  CYC(b_+211, b_+212); alu_xor(gb, A);
  CYC(b_+212, b_+215); mem_wr(gb, wDisableScreenTransitions, A);
  RET(b_+215); return;

state2Substate4:
  CYC(b_+218, b_+220); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+220, b_+221); A = mem_rd(gb, DE);
  CYC(b_+221, b_+222); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+222, b_+224); goto incState; }
  CYC(b_+222, b_+224);
  CALL_C(b_+224, s_flyingRooster_applySpeedAndUpdatePositions_hook, SYM(flyingRooster_applySpeedAndUpdatePositions), b_+227);
  CYC(b_+227, b_+229); L = 0x0d;
  CYC(b_+229, b_+230); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+230, b_+232); alu_cp(gb, 0x30);
  if (F & FC) { RET_TAKEN(b_+232); return; }
  CYC(b_+232, b_+233);
  CYC(b_+233, b_+235); L = 0x0b;
  CYC(b_+235, b_+236); A = mem_rd(gb, HL);
  CYC(b_+236, b_+238); alu_sub(gb, 0x68);
  CYC(b_+238, b_+240); L = 0x0f;
  CYC(b_+240, b_+241); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+241, b_+242); mem_wr(gb, HL, A);
  CYC(b_+242, b_+244); A = 0x68;
  CYC(b_+244, b_+246); L = 0x0b;
  CYC(b_+246, b_+247); mem_wr(gb, HL, A);
  CYC(b_+247, b_+249); L = 0x1a;
  CYC(b_+249, b_+251); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+251, b_+253); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+253, b_+254); A = mem_rd(gb, DE);
  CYC(b_+254, b_+256); A = A & 0xbf;
  CYC(b_+256, b_+257); mem_wr(gb, DE, A);
incState:
  CALL_C(b_+257, s_interactionIncState, SYM(interactionIncState), b_+260);
  RET(b_+260); return;
state3:
  CALL_C(b_+261, s_interactionAnimate, SYM(interactionAnimate), b_+264);
  CALL_C(b_+264, s_flyingRooster_applySpeedAndUpdatePositions_hook, SYM(flyingRooster_applySpeedAndUpdatePositions), b_+267);
  CYC(b_+267, b_+269); L = 0x0b;
  CYC(b_+269, b_+270); A = mem_rd(gb, HL);
  CYC(b_+270, b_+272); alu_cp(gb, 0x58);
  if (!(F & FC)) { CYCT(b_+272, b_+274); goto L_4d7d; }
  CYC(b_+272, b_+274);
  CYC(b_+274, b_+275); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_4d7d:
  CYC(b_+275, b_+277); L = 0x0d;
  CYC(b_+277, b_+279); E = INTERACTION_BASE + OBJ_SCRIPT_RET;
  CYC(b_+279, b_+280); A = mem_rd(gb, DE);
  CYC(b_+280, b_+281); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+281, b_+283); goto reachedTargetXPosition; }
  CYC(b_+281, b_+283);
  CALL_C(b_+283, s_flyingRooster_updateGravityAndCheckCaps_hook, SYM(flyingRooster_updateGravityAndCheckCaps), b_+286);
  CYC(b_+286, b_+289); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+289, b_+291); alu_and(gb, 0x03);
  if (F & FZ) { RET_TAKEN(b_+291); return; }
  CYC(b_+291, b_+292);
  CYC(b_+292, b_+295); SET_BC(0xff50);
  CYC(b_+295, b_+297); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+297, b_+298); A = mem_rd(gb, DE);
  CYC(b_+298, b_+299); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+299, b_+301); goto L_4d9a; }
  CYC(b_+299, b_+301);
  CYC(b_+301, b_+304); SET_BC(0xff30);
L_4d9a:
  CALL_C(b_+304, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+307);
  CYC(b_+307, b_+309); A = 0xa0;
  CALL_C(b_+309, s_playSound, SYM(playSound_b00), b_+312);
  CYC(b_+312, b_+315);
  TAIL(interactionAnimate);
reachedTargetXPosition:
  CALL_C(b_+315, s_flyingRooster_getVisualLinkYPosition_hook, SYM(flyingRooster_getVisualLinkYPosition), b_+318);
  CYC(b_+318, b_+320); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+320, b_+321); A = mem_rd(gb, DE);
  CYC(b_+321, b_+323); alu_add(gb, 0x08);
  CYC(b_+323, b_+324); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+324, b_+326); goto notHighEnough; }
  CYC(b_+324, b_+326);
  CYC(b_+326, b_+328); A = 0x08;
  CYC(b_+328, b_+330); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+330, b_+331); mem_wr(gb, DE, A);
  CYC(b_+331, b_+333); A = 0x04;
  CYC(b_+333, b_+335); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+335, b_+336); mem_wr(gb, DE, A);
  RET(b_+336); return;
notHighEnough:
  CYC(b_+337, b_+339); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+339, b_+340); A = mem_rd(gb, DE);
  CYC(b_+340, b_+341); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+341, b_+343); goto gotoState6; }
  CYC(b_+341, b_+343);
  CYC(b_+343, b_+346); A = mem_rd(gb, wScreenTransitionBoundaryY);
  CYC(b_+346, b_+347); B = A;
  CYC(b_+347, b_+349); L = 0x0b;
  CYC(b_+349, b_+350); A = mem_rd(gb, HL);
  CYC(b_+350, b_+351); alu_sub(gb, B);
  CYC(b_+351, b_+353); L = 0x0f;
  CYC(b_+353, b_+354); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+354, b_+355); mem_wr(gb, HL, A);
  CYC(b_+355, b_+357); L = 0x0b;
  CYC(b_+357, b_+358); mem_wr(gb, HL, B);
  CALL_C(b_+358, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+361);
  CYC(b_+361, b_+363); A = 0x8c;
  CYC(b_+363, b_+364); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+364, b_+366); mem_wr(gb, HL, 0x80);
  CYC(b_+366, b_+368); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+368, b_+370); A = 0x03;
  CYC(b_+370, b_+371); mem_wr(gb, HL, A); SET_HL(HL + 1);
gotoState6:
  CYC(b_+371, b_+373); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+373, b_+375); A = 0x06;
  CYC(b_+375, b_+376); mem_wr(gb, DE, A);
  CYC(b_+376, b_+378); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+378, b_+380); A = 0x3c;
  CYC(b_+380, b_+381); mem_wr(gb, DE, A);
  RET(b_+381); return;
state6:
  CALL_C(b_+382, s_interactionAnimate, SYM(interactionAnimate), b_+385);
  CALL_C(b_+385, s_interactionAnimate, SYM(interactionAnimate), b_+388);
  CYC(b_+388, b_+390); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+390, b_+391); A = mem_rd(gb, DE);
  CYC(b_+391, b_+392); A = alu_dec8(gb, A);
  CYC(b_+392, b_+393); mem_wr(gb, DE, A);
  if (!(F & FZ)) { RET_TAKEN(b_+393); return; }
  CYC(b_+393, b_+394);
  CYC(b_+394, b_+397);
  goto releaseFromLink;
state4:
  CALL_C(b_+397, s_interactionAnimate, SYM(interactionAnimate), b_+400);
  CALL_C(b_+400, s_flyingRooster_applySpeedAndUpdatePositions_hook, SYM(flyingRooster_applySpeedAndUpdatePositions), b_+403);
  CYC(b_+403, b_+405); E = INTERACTION_BASE + OBJ_SCRIPT_RET;
  CYC(b_+405, b_+406); A = mem_rd(gb, DE);
  CYC(b_+406, b_+408); alu_add(gb, 0x20);
  CYC(b_+408, b_+410); L = 0x0d;
  CYC(b_+410, b_+411); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+411, b_+413); goto releaseFromLink; }
  CYC(b_+411, b_+413);
  CYC(b_+413, b_+414); A = mem_rd(gb, HL);
  CYC(b_+414, b_+416); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+416); return; }
  CYC(b_+416, b_+417);
  CALL_C(b_+417, s_flyingRooster_getVisualLinkYPosition_hook, SYM(flyingRooster_getVisualLinkYPosition), b_+420);
  CYC(b_+420, b_+422); alu_add(gb, 0x08);
  CYC(b_+422, b_+424); L = 0x0b;
  CYC(b_+424, b_+425); mem_wr(gb, HL, A);
  CYC(b_+425, b_+427); L = 0x0f;
  CYC(b_+427, b_+429); A = 0xf8;
  CYC(b_+429, b_+430); mem_wr(gb, HL, A);
  CYC(b_+430, b_+432); L = 0x1a;
  CYC(b_+432, b_+434); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+434, b_+436); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+436, b_+437); A = mem_rd(gb, DE);
  CYC(b_+437, b_+439); alu_and(gb, 0xbf);
  CYC(b_+439, b_+440); mem_wr(gb, DE, A);
  RET(b_+440); return;
releaseFromLink:
  CYC(b_+441, b_+442); alu_xor(gb, A);
  CYC(b_+442, b_+445); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+445, b_+448); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+448, b_+451); SET_HL(w1Link_angle);
  CYC(b_+451, b_+453); A = 0xff;
  CYC(b_+453, b_+454); mem_wr(gb, HL, A);
  CYC(b_+454, b_+456); A = 0x05;
  CYC(b_+456, b_+458); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+458, b_+459); mem_wr(gb, DE, A);
  CYC(b_+459, b_+461); A = 0x08;
  CYC(b_+461, b_+463); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+463, b_+464); mem_wr(gb, DE, A);
  CYC(b_+464, b_+465); alu_xor(gb, A);
  CYC(b_+465, b_+466); E = alu_inc8(gb, E);
  CYC(b_+466, b_+467); mem_wr(gb, DE, A);
  CYC(b_+467, b_+469); A = 0x00;
  CALL_C(b_+469, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+472);
  CYC(b_+472, b_+475);
  TAIL(dropLinkHeldItem);
state5:
  CALL_C(b_+475, s_interactionAnimate, SYM(interactionAnimate), b_+478);
  CYC(b_+478, b_+480); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+480, b_+481); A = mem_rd(gb, DE);
  CYC(b_+481, b_+482); A = alu_dec8(gb, A);
  CYC(b_+482, b_+483); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(b_+483, b_+485); goto updateHopping; }
  CYC(b_+483, b_+485);
  CYC(b_+485, b_+487); A = 0x08;
  CYC(b_+487, b_+488); mem_wr(gb, DE, A);
  CYC(b_+488, b_+489); E = alu_inc8(gb, E);
  CYC(b_+489, b_+490); A = mem_rd(gb, DE);
  CYC(b_+490, b_+492); alu_xor(gb, 0x01);
  CYC(b_+492, b_+493); mem_wr(gb, DE, A);
  CYC(b_+493, b_+495);
  goto moveTowardBasePosition;
updateHopping:
  CYC(b_+495, b_+497); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+497, b_+499); goto moveTowardBasePosition; }
  CYC(b_+497, b_+499);
  CYC(b_+499, b_+501); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+501, b_+502); A = mem_rd(gb, DE);
  CYC(b_+502, b_+503); alu_or(gb, A);
  CYC(b_+503, b_+505); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+505, b_+506); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(b_+506, b_+508); goto decZ; }
  CYC(b_+506, b_+508);
  CYC(b_+508, b_+509); A = alu_inc8(gb, A);
  CYC(b_+509, b_+511);
  goto setZ;
decZ:
  CYC(b_+511, b_+512); A = alu_dec8(gb, A);
setZ:
  CYC(b_+512, b_+513); mem_wr(gb, DE, A);
moveTowardBasePosition:
  CYC(b_+513, b_+515); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+515, b_+516); A = mem_rd(gb, DE);
  CYC(b_+516, b_+517); B = A;
  CYC(b_+517, b_+518); E = alu_inc8(gb, E);
  CYC(b_+518, b_+519); A = mem_rd(gb, DE);
  CYC(b_+519, b_+520); C = A;
  CALL_C(b_+520, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+523);
  CYC(b_+523, b_+525); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+525, b_+526); mem_wr(gb, DE, A);
  CALL_C(b_+526, s_objectApplySpeed, SYM(objectApplySpeed), b_+529);
  CYC(b_+529, b_+530); H = D;
  CYC(b_+530, b_+532); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+532, b_+534); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+534, b_+535); A = mem_rd(gb, DE);
  CYC(b_+535, b_+536); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+536); return; }
  CYC(b_+536, b_+537);
  CYC(b_+537, b_+538); L = alu_inc8(gb, L);
  CYC(b_+538, b_+540); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+540, b_+541); A = mem_rd(gb, DE);
  CYC(b_+541, b_+542); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+542); return; }
  CYC(b_+542, b_+543);
  CYC(b_+543, b_+545); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+545, b_+547); mem_wr(gb, HL, 0x01);
  CYC(b_+547, b_+549); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+549, b_+551); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CALL_C(b_+551, s_flyingRooster_getSubidAndInitSpeed_hook, SYM(flyingRooster_getSubidAndInitSpeed), b_+554);
  CYC(b_+554, b_+556); A = 0x01;
  CYC(b_+556, b_+559);
  TAIL(interactionSetAnimation);
}

// @param[out]	bc	Y/X positions for Link
void s_flyingRooster_applySpeedAndUpdatePositions_hook(GB *gb) {
  BASE(flyingRooster_applySpeedAndUpdatePositions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CALL_C(b_+12, s_objectApplySpeed, SYM(objectApplySpeed), b_+15);
  CYC(b_+15, b_+18); SET_HL(w1Link_yh);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); C = A;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  RET(b_+29); return;
}

void s_flyingRooster_updateGravityAndCheckCaps_hook(GB *gb) {
  BASE(flyingRooster_updateGravityAndCheckCaps);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0x0e;
  CYC(b_+2, b_+4); E = INTERACTION_BASE + OBJ_Z;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); C = 0x20;
  CALL_C(b_+11, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+14);
  CYC(b_+14, b_+17); SET_HL(w1Link_z);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_Z;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); E = alu_inc8(gb, E);
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CALL_C(b_+24, s_flyingRooster_getVisualLinkYPosition_hook, SYM(flyingRooster_getVisualLinkYPosition), b_+27);
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+31, b_+33); goto checkBottomCap; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+34); alu_sub(gb, B);
  CYC(b_+34, b_+36); L = 0x0f;
  CYC(b_+36, b_+37); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  RET(b_+38); return;
checkBottomCap:
  CYC(b_+39, b_+41); L = 0x0f;
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CYC(b_+42, b_+44); alu_cp(gb, 0xf8);
  if (F & FC) { RET_TAKEN(b_+44); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); A = 0xf8;
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+49); alu_xor(gb, A);
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+54); E = 0x55;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  RET(b_+55); return;
}

// @param[out]	a,b	Link's Y-position + Z-position
void s_flyingRooster_getVisualLinkYPosition_hook(GB *gb) {
  BASE(flyingRooster_getVisualLinkYPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0x0b;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+5); L = 0x0f;
  CYC(b_+5, b_+6); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+7); B = A;
  RET(b_+7); return;
}

// Helper object which handles the screen transition when Link falls down
void s_flyingRooster_subidBit7Set_hook(GB *gb) {
  BASE(flyingRooster_subidBit7Set);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_zh);
  CYC(b_+3, b_+6); A = mem_rd(gb, wActiveRoom);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto nextScreen; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); L = 0x0b;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+18); A = 0x80;
  CYC(b_+18, b_+21); mem_wr(gb, wLinkInAir, A);
  CYC(b_+21, b_+23); A = 0x82;
  CYC(b_+23, b_+26); mem_wr(gb, wScreenTransitionDirection, A);
  RET(b_+26); return;
nextScreen:
  CYC(b_+27, b_+30); A = mem_rd(gb, wScrollMode);
  CYC(b_+30, b_+32); alu_and(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xe8);
  CYC(b_+35, b_+37); L = 0x0b;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x28);
  CYC(b_+39, b_+42);
  TAIL(interactionDelete);
}

void s_flyingRooster_getSubidAndInitSpeed_hook(GB *gb) {
  BASE(flyingRooster_getSubidAndInitSpeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0f);
  RET(b_+7); return;
}

