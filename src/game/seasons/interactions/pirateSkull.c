#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/pirateSkull.s.
// INTERAC_PIRATE_SKULL

static uint16_t pirateSkull_jump_table(GB *gb) {
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

// INTERAC_PIRATE_SKULL
void s_interactionCode4d_hook(GB *gb) {
  BASE(interactionCode4d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (pirateSkull_jump_table(gb));
    if (jt_ == b_+8) goto subid0;
    if (jt_ == b_+278) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (pirateSkull_jump_table(gb));
    if (jt_ == b_+18) goto subid0_state0;
    if (jt_ == b_+80) goto subid0_state1;
    if (jt_ == b_+143) goto subid0_state2;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CYC(b_+18, b_+20); A = 0x01;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+23); A = 0x1b;
  CALL_C(b_+23, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+26);
  if (F & FZ) { CYCT(b_+26, b_+29); TAIL(interactionDelete); }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+31); C = 0x4d;
  CALL_C(b_+31, s_objectFindSameTypeObjectWithID, SYM(objectFindSameTypeObjectWithID), b_+34);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto L_739d; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); A = H;
  CYC(b_+37, b_+38); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(b_+38, b_+41); TAIL(interactionDelete); }
  CYC(b_+38, b_+41);
  CALL_C(b_+41, s_func_228f, SYM(func_228f), b_+44);
  if (F & FZ) { CYCT(b_+44, b_+47); TAIL(interactionDelete); }
  CYC(b_+44, b_+47);
L_739d:
  CYC(b_+47, b_+49); A = 0x4a;
  CALL_C(b_+49, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+52);
  if (F & FC) { CYCT(b_+52, b_+54); goto L_73ad; }
  CYC(b_+52, b_+54);
  CALL_C(b_+54, s_getRandomNumber, SYM(getRandomNumber), b_+57);
  CYC(b_+57, b_+59); alu_and(gb, 0x03);
  CYC(b_+59, b_+60); A = alu_inc8(gb, A);
  CYC(b_+60, b_+62); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
L_73ad:
  CYC(b_+63, b_+65); A = 0x4d;
  CALL_C(b_+65, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+68);
  CYC(b_+68, b_+71); SET_HL(SYM(pirateSkullScript_notYetCarried));
  CALL_C(b_+71, s_interactionSetScript, SYM(interactionSetScript), b_+74);
  CALL_C(b_+74, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+77);
  CYC(b_+77, b_+80);
  TAIL(objectSetVisiblec2);
subid0_state1:
  CYC(b_+80, b_+82); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+84); push_effect(gb, b_+84);
  do { uint16_t jt_ = (pirateSkull_jump_table(gb));
    if (jt_ == b_+88) goto subid0_state1_substate0;
    if (jt_ == b_+94) goto subid0_state1_substate1;
    HANDOFF(HL);
  } while (0);
subid0_state1_substate0:
  CALL_C(b_+88, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+91);
  CYC(b_+91, b_+94);
  TAIL(interactionRunScript);
subid0_state1_substate1:
  CYC(b_+94, b_+96); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+98, b_+100); goto L_73eb; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+101); B = A;
  CYC(b_+101, b_+102); E = alu_inc8(gb, E);
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  CYC(b_+103, b_+104); C = A;
  CYC(b_+104, b_+105); push_effect(gb, BC);
  CALL_C(b_+105, s_objectCheckContainsPoint, SYM(objectCheckContainsPoint), b_+108);
  CYC(b_+108, b_+109); SET_BC(pop_effect(gb));
  if (F & FC) { CYCT(b_+109, b_+111); goto L_73f4; }
  CYC(b_+109, b_+111);
  CALL_C(b_+111, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+114);
  CYC(b_+114, b_+116); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CYC(b_+117, b_+119); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+119, b_+121); A = 0x14;
  CYC(b_+121, b_+122); mem_wr(gb, DE, A);
  CALL_C(b_+122, s_objectApplySpeed, SYM(objectApplySpeed), b_+125);
L_73eb:
  CYC(b_+125, b_+126); H = D;
  CYC(b_+126, b_+128); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+128, b_+129); alu_xor(gb, A);
  CYC(b_+129, b_+130); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+130, b_+131); mem_wr(gb, HL, A);
  CYC(b_+131, b_+134);
  TAIL(objectAddToGrabbableObjectBuffer);
L_73f4:
  CYC(b_+134, b_+137); SET_BC(0x4d0a);
  CALL_C(b_+137, s_showText, SYM(showText), b_+140);
  CYC(b_+140, b_+143);
  TAIL(interactionDelete);
subid0_state2:
  CYC(b_+143, b_+145); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+147); push_effect(gb, b_+147);
  do { uint16_t jt_ = (pirateSkull_jump_table(gb));
    if (jt_ == b_+155) goto subid0_state2_substate0;
    if (jt_ == b_+172) goto subid0_state2_substate1;
    if (jt_ == b_+207) goto subid0_state2_substate2;
    if (jt_ == b_+227) goto subid0_state2_substate3;
    HANDOFF(HL);
  } while (0);
subid0_state2_substate0:
  CYC(b_+155, b_+156); H = D;
  CYC(b_+156, b_+157); L = E;
  CYC(b_+157, b_+158); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+158, b_+159); alu_xor(gb, A);
  CYC(b_+159, b_+162); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+162, b_+164); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+164, b_+165); mem_wr(gb, HL, A);
  CYC(b_+165, b_+166); A = alu_inc8(gb, A);
  CALL_C(b_+166, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+169);
  CYC(b_+169, b_+172);
  TAIL(objectSetVisiblec1);
subid0_state2_substate1:
  CYC(b_+172, b_+175); SET_HL(wPirateSkullRandomNumber);
  CYC(b_+175, b_+177); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+177, b_+179); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+179, b_+180); A = mem_rd(gb, DE);
  CYC(b_+180, b_+181); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(b_+181, b_+183); goto L_742f; }
  CYC(b_+181, b_+183);
  CYC(b_+183, b_+185); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+185, b_+187); A = 0x14;
  CYC(b_+187, b_+188); mem_wr(gb, DE, A);
  CYC(b_+188, b_+190); A = 0x01;
  CYC(b_+190, b_+193);
  TAIL(interactionSetAnimation);
L_742f:
  CALL_C(b_+193, s_interactionAnimate, SYM(interactionAnimate), b_+196);
  CALL_C(b_+196, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+199);
  if (!(F & FZ)) { RET_TAKEN(b_+199); return; }
  CYC(b_+199, b_+200);
  CYC(b_+200, b_+202); mem_wr(gb, HL, 0x14);
  CYC(b_+202, b_+204); A = 0x7e;
  CYC(b_+204, b_+207);
  TAIL(playSound_b00);
subid0_state2_substate2:
  CALL_C(b_+207, s_objectCheckWithinRoomBoundary, SYM(objectCheckWithinRoomBoundary), b_+210);
  if (!(F & FC)) { CYCT(b_+210, b_+213); TAIL(interactionDelete); }
  CYC(b_+210, b_+213);
  CALL_C(b_+213, s_objectReplaceWithAnimationIfOnHazard, SYM(objectReplaceWithAnimationIfOnHazard), b_+216);
  if (F & FC) { CYCT(b_+216, b_+218); goto subid0_state2_droppedInWater; }
  CYC(b_+216, b_+218);
  CYC(b_+218, b_+219); H = D;
  CYC(b_+219, b_+221); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+221, b_+223); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+223, b_+225); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+225, b_+226); mem_wr(gb, HL, D);
  RET(b_+226); return;
subid0_state2_substate3:
  CYC(b_+227, b_+229); C = 0x20;
  CALL_C(b_+229, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+232);
  if (!(F & FZ)) { RET_TAKEN(b_+232); return; }
  CYC(b_+232, b_+233);
  CALL_C(b_+233, s_objectReplaceWithAnimationIfOnHazard, SYM(objectReplaceWithAnimationIfOnHazard), b_+236);
  if (F & FC) { CYCT(b_+236, b_+238); goto subid0_state2_droppedInWater; }
  CYC(b_+236, b_+238);
  CALL_C(b_+238, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+241);
  if (!(F & FC)) { CYCT(b_+241, b_+244); TAIL(interactionDelete); }
  CYC(b_+241, b_+244);
  CYC(b_+244, b_+245); H = D;
  CYC(b_+245, b_+247); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+247, b_+249); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+249, b_+251); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+251, b_+253); A = 0x01;
  CYC(b_+253, b_+254); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+254, b_+255); mem_wr(gb, HL, A);
  CYC(b_+255, b_+257); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+257, b_+258); A = mem_rd(gb, HL);
  CYC(b_+258, b_+259); alu_or(gb, A);
  CYC(b_+259, b_+262); SET_BC(0x4d06);
  if (!(F & FZ)) CALL_C_CC(b_+262, s_showText, SYM(showText), b_+265);
  else CYC(b_+262, b_+265);
  CYC(b_+265, b_+266); alu_xor(gb, A);
  CALL_C(b_+266, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+269);
  CYC(b_+269, b_+272);
  TAIL(objectSetVisible82);
subid0_state2_droppedInWater:
  CYC(b_+272, b_+275); SET_BC(0x4d09);
  CYC(b_+275, b_+278);
  TAIL(showText);
subid1:
  CYC(b_+278, b_+280); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+280, b_+281); A = mem_rd(gb, DE);
  CYC(b_+281, b_+282); push_effect(gb, b_+282);
  do { uint16_t jt_ = (pirateSkull_jump_table(gb));
    if (jt_ == b_+294) goto subid1_state0;
    if (jt_ == b_+325) goto subid1_state1;
    if (jt_ == b_+345) goto subid1_state2;
    if (jt_ == b_+372) goto subid1_state3;
    if (jt_ == b_+398) goto subid1_state4;
    if (jt_ == b_+415) goto subid1_state5;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+294, b_+296); A = 0x01;
  CYC(b_+296, b_+297); mem_wr(gb, DE, A);
  CALL_C(b_+297, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+300);
  CYC(b_+300, b_+302); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+302, b_+305); TAIL(interactionDelete); }
  CYC(b_+302, b_+305);
  CYC(b_+305, b_+307); A = 0x01;
  CYC(b_+307, b_+310); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+310, b_+313); A = mem_rd(gb, w1Link_yh);
  CYC(b_+313, b_+315); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+315, b_+316); mem_wr(gb, DE, A);
  CYC(b_+316, b_+319); A = mem_rd(gb, w1Link_xh);
  CYC(b_+319, b_+321); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+321, b_+322); mem_wr(gb, DE, A);
  CYC(b_+322, b_+325);
  TAIL(interactionInitGraphics);
subid1_state1:
  CYC(b_+325, b_+328); A = mem_rd(gb, w1Link_zh);
  CYC(b_+328, b_+329); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+329); return; }
  CYC(b_+329, b_+330);
  CYC(b_+330, b_+332); A = 0x02;
  CYC(b_+332, b_+333); mem_wr(gb, DE, A);
  CALL_C(b_+333, s_objectGetZAboveScreen, SYM(objectGetZAboveScreen), b_+336);
  CYC(b_+336, b_+338); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+338, b_+339); mem_wr(gb, DE, A);
  CALL_C(b_+339, s_setLinkForceStateToState08, SYM(setLinkForceStateToState08), b_+342);
  CYC(b_+342, b_+345);
  TAIL(objectSetVisiblec1);
subid1_state2:
  CYC(b_+345, b_+347); C = 0x20;
  CALL_C(b_+347, s_objectUpdateSpeedZAndBounce, SYM(objectUpdateSpeedZAndBounce), b_+350);
  if (!(F & FZ)) { RET_TAKEN(b_+350); return; }
  CYC(b_+350, b_+351);
  CALL_C(b_+351, s_interactionIncState, SYM(interactionIncState), b_+354);
  CYC(b_+354, b_+356); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+356, b_+358); mem_wr(gb, HL, 0x14);
  CYC(b_+358, b_+360); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+360, b_+362); mem_wr(gb, HL, 0x10);
  CYC(b_+362, b_+364); A = 0x02;
  CYC(b_+364, b_+367); mem_wr(gb, wcc50, A);
  CYC(b_+367, b_+369); A = 0xca;
  CYC(b_+369, b_+372);
  TAIL(playSound_b00);
subid1_state3:
  CALL_C(b_+372, s_objectApplySpeed, SYM(objectApplySpeed), b_+375);
  CYC(b_+375, b_+377); C = 0x20;
  CALL_C(b_+377, s_objectUpdateSpeedZAndBounce, SYM(objectUpdateSpeedZAndBounce), b_+380);
  CYC(b_+380, b_+381); push_effect(gb, AF);
  CYC(b_+381, b_+383); A = 0xca;
  if (F & FZ) CALL_C_CC(b_+383, s_playSound, SYM(playSound_b00), b_+386);
  else CYC(b_+383, b_+386);
  CYC(b_+386, b_+387); SET_AF(pop_effect(gb));
  if (!(F & FC)) { RET_TAKEN(b_+387); return; }
  CYC(b_+387, b_+388);
  CALL_C(b_+388, s_interactionIncState, SYM(interactionIncState), b_+391);
  CYC(b_+391, b_+393); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+393, b_+395); mem_wr(gb, HL, 0x28);
  CYC(b_+395, b_+398);
  TAIL(objectSetVisible82);
subid1_state4:
  CALL_C(b_+398, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+401);
  if (!(F & FZ)) { RET_TAKEN(b_+401); return; }
  CYC(b_+401, b_+402);
  CYC(b_+402, b_+404); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+404, b_+405); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+405, b_+406); alu_xor(gb, A);
  CYC(b_+406, b_+409); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+409, b_+412); SET_BC(0x4d07);
  CYC(b_+412, b_+415);
  TAIL(showText);
subid1_state5:
  CYC(b_+415, b_+418); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+418, b_+419); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+419); return; }
  CYC(b_+419, b_+420);
  CALL_C(b_+420, s_objectCreatePuff, SYM(objectCreatePuff), b_+423);
  CYC(b_+423, b_+426);
  TAIL(interactionDelete);
}
