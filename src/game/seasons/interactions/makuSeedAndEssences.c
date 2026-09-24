#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/makuSeedAndEssences.s.
// INTERAC_MAKU_SEED_AND_ESSENCES

static uint16_t makuSeedAndEssences_jump_table(GB *gb) {
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

static void makuSeedAndEssences_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_MAKU_SEED_AND_ESSENCES
void s_interactionCodede_hook(GB *gb) {
  BASE(interactionCodede);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (makuSeedAndEssences_jump_table(gb));
    if (jt_ == b_+22) goto subid0;
    if (jt_ == b_+334) goto subid1To8;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, b_+26);
  do { uint16_t jt_ = (makuSeedAndEssences_jump_table(gb));
    if (jt_ == b_+34) goto subid0_state0;
    if (jt_ == b_+83) goto subid0_state1;
    if (jt_ == b_+101) goto subid0_state2;
    if (jt_ == b_+139) goto subid0_state3;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CYC(b_+34, b_+36); A = 0x01;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CALL_C(b_+37, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+40);
  CYC(b_+40, b_+43); A = mem_rd(gb, w1Link_yh);
  CYC(b_+43, b_+45); alu_sub(gb, 0x0e);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+51); A = mem_rd(gb, w1Link_xh);
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CALL_C(b_+54, s_setLinkForceStateToState08, SYM(setLinkForceStateToState08), b_+57);
  CYC(b_+57, b_+59); A = 0xf1;
  CALL_C(b_+59, s_playSound, SYM(playSound_b00), b_+62);
  CYC(b_+62, b_+64); A = 0x77;
  CALL_C(b_+64, s_playSound, SYM(playSound_b00), b_+67);
  CYC(b_+67, b_+69); B = 0x84;
  CALL_C(b_+69, s_objectCreateInteractionWithSubid00, SYM(objectCreateInteractionWithSubid00), b_+72);
  if (!(F & FZ)) { RET_TAKEN(b_+72); return; }
  CYC(b_+72, b_+73);
  CYC(b_+73, b_+75); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+75, b_+76); E = L;
  CYC(b_+76, b_+78); A = 0x78;
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+83);
  TAIL(objectSetVisible82);
subid0_state1:
  CYC(b_+83, b_+85); A = 0x0f;
  CYC(b_+85, b_+88); mem_wr(gb, wcc50, A);
  CALL_C(b_+88, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+91);
  if (!(F & FZ)) { RET_TAKEN(b_+91); return; }
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x40);
  CYC(b_+94, b_+96); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x14);
  CYC(b_+98, b_+101);
  TAIL(interactionIncState);
subid0_state2:
  CALL_C(b_+101, s_objectApplySpeed, SYM(objectApplySpeed), b_+104);
  CALL_C(b_+104, s_func_6c94_hook, SYM(func_6c94), b_+107);
  CALL_C(b_+107, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+110);
  if (!(F & FZ)) { RET_TAKEN(b_+110); return; }
  CYC(b_+110, b_+111);
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x78);
  CYC(b_+113, b_+115); A = 0x10;
  CYC(b_+115, b_+118); mem_wr(gb, wcc50, A);
  CYC(b_+118, b_+120); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x28);
  CYC(b_+122, b_+124); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0x50);
  CYC(b_+126, b_+128); A = 0x8a;
  CALL_C(b_+128, s_playSound, SYM(playSound_b00), b_+131);
  CYC(b_+131, b_+133); A = 0x03;
  CALL_C(b_+133, s_fadeinFromWhiteWithDelay, SYM(fadeinFromWhiteWithDelay), b_+136);
  CYC(b_+136, b_+139);
  TAIL(interactionIncState);
subid0_state3:
  CALL_C(b_+139, s_func_6c94_hook, SYM(func_6c94), b_+142);
  CALL_C(b_+142, s_func_6ccb_hook, SYM(func_6ccb), b_+145);
  CYC(b_+145, b_+147); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+147, b_+148); A = mem_rd(gb, DE);
  CYC(b_+148, b_+149); push_effect(gb, b_+149);
  do { uint16_t jt_ = (makuSeedAndEssences_jump_table(gb));
    if (jt_ == b_+169) goto subid0_substate0;
    if (jt_ == b_+181) goto subid0_substate1;
    if (jt_ == b_+244) goto subid0_substate2;
    if (jt_ == b_+263) goto subid0_substate3;
    if (jt_ == b_+276) goto subid0_substate4;
    if (jt_ == b_+296) goto subid0_substate8;
    if (jt_ == b_+313) goto subid0_substate9;
    HANDOFF(HL);
  } while (0);
subid0_substate0:
  CALL_C(b_+169, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+172);
  if (!(F & FZ)) { RET_TAKEN(b_+172); return; }
  CYC(b_+172, b_+173);
  CYC(b_+173, b_+175); mem_wr(gb, HL, 0x14);
  CYC(b_+175, b_+176); L = alu_inc8(gb, L);
  CYC(b_+176, b_+178); mem_wr(gb, HL, 0x08);
  CYC(b_+178, b_+181);
  TAIL(interactionIncSubstate);
subid0_substate1:
  CALL_C(b_+181, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+184);
  if (!(F & FZ)) { RET_TAKEN(b_+184); return; }
  CYC(b_+184, b_+185);
  CYC(b_+185, b_+187); mem_wr(gb, HL, 0x14);
  CYC(b_+187, b_+188); L = alu_inc8(gb, L);
  CYC(b_+188, b_+189); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+189, b_+190); B = mem_rd(gb, HL);
  CALL_C(b_+190, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+193);
  if (!(F & FZ)) { RET_TAKEN(b_+193); return; }
  CYC(b_+193, b_+194);
  CYC(b_+194, b_+196); mem_wr(gb, HL, 0xde);
  CALL_C(b_+196, s_objectCopyPosition, SYM(objectCopyPosition), b_+199);
  CYC(b_+199, b_+200); A = B;
  CYC(b_+200, b_+203); SET_BC(b_+228 /* @subid0@table_6bc3 */);
  CALL_C(b_+203, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+206);
  CYC(b_+206, b_+207); A = mem_rd(gb, BC);
  CYC(b_+207, b_+209); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+209, b_+210); mem_wr(gb, HL, A);
  CYC(b_+210, b_+212); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+212, b_+213); SET_BC(BC + 1);
  CYC(b_+213, b_+214); A = mem_rd(gb, BC);
  CYC(b_+214, b_+215); mem_wr(gb, HL, A);
  CYC(b_+215, b_+217); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+217, b_+218); A = mem_rd(gb, DE);
  CYC(b_+218, b_+219); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+219); return; }
  CYC(b_+219, b_+220);
  CALL_C(b_+220, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+223);
  CYC(b_+223, b_+225); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+225, b_+227); mem_wr(gb, HL, 0x78);
  RET(b_+227); return;

subid0_substate2:
  CALL_C(b_+244, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+247);
  if (!(F & FZ)) { RET_TAKEN(b_+247); return; }
  CYC(b_+247, b_+248);
  CYC(b_+248, b_+250); mem_wr(gb, HL, 0x3c);
  CYC(b_+250, b_+252); A = 0x01;
  CYC(b_+252, b_+255); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+255, b_+257); A = 0x20;
  CYC(b_+257, b_+260); mem_wr(gb, wTmpcfc0 + 0x01, A);
  CYC(b_+260, b_+263);
  TAIL(interactionIncSubstate);
subid0_substate3:
  CYC(b_+263, b_+266); A = mem_rd(gb, wFrameCounter);
  CYC(b_+266, b_+268); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+268, b_+270); goto subid0_incSubstateAtInterval; }
  CYC(b_+268, b_+270);
  CYC(b_+270, b_+273); SET_HL(wTmpcfc0 + 0x01);
  CYC(b_+273, b_+274); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+274, b_+276);
  goto subid0_incSubstateAtInterval;
subid0_substate4:
  CYC(b_+276, b_+279); A = mem_rd(gb, wFrameCounter);
  CYC(b_+279, b_+281); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+281, b_+283); goto subid0_incSubstateAtInterval; }
  CYC(b_+281, b_+283);
  CYC(b_+283, b_+286); SET_HL(wTmpcfc0 + 0x01);
  CYC(b_+286, b_+287); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
subid0_incSubstateAtInterval:
  CALL_C(b_+287, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+290);
  if (!(F & FZ)) { RET_TAKEN(b_+290); return; }
  CYC(b_+290, b_+291);
  CYC(b_+291, b_+293); mem_wr(gb, HL, 0x3c);
  CYC(b_+293, b_+296);
  TAIL(interactionIncSubstate);
subid0_substate8:
  CYC(b_+296, b_+299); SET_HL(wTmpcfc0 + 0x01);
  CYC(b_+299, b_+300); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+300, b_+302); A = 0xb4;
  CALL_C(b_+302, s_playSound, SYM(playSound_b00), b_+305);
  CYC(b_+305, b_+307); A = 0x04;
  CALL_C(b_+307, s_fadeoutToWhiteWithDelay, SYM(fadeoutToWhiteWithDelay), b_+310);
  CYC(b_+310, b_+313);
  TAIL(interactionIncSubstate);
subid0_substate9:
  CYC(b_+313, b_+316); SET_HL(wTmpcfc0 + 0x01);
  CYC(b_+316, b_+317); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+317, b_+320); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+320, b_+321); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+321); return; }
  CYC(b_+321, b_+322);
  CYC(b_+322, b_+325); SET_HL(wTmpcbb3);
  CYC(b_+325, b_+326); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+326, b_+328); A = 0x08;
  CALL_C(b_+328, s_fadeinFromWhiteWithDelay, SYM(fadeinFromWhiteWithDelay), b_+331);
  CYC(b_+331, b_+334);
  TAIL(interactionDelete);
subid1To8:
  CYC(b_+334, b_+336); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+336, b_+337); A = mem_rd(gb, DE);
  CYC(b_+337, b_+338); push_effect(gb, b_+338);
  do { uint16_t jt_ = (makuSeedAndEssences_jump_table(gb));
    if (jt_ == b_+346) goto subid1To8_state0;
    if (jt_ == b_+381) goto subid1To8_state1;
    if (jt_ == b_+391) goto subid1To8_state2;
    if (jt_ == b_+399) goto subid1To8_state3;
    HANDOFF(HL);
  } while (0);
subid1To8_state0:
  CYC(b_+346, b_+348); A = 0x01;
  CYC(b_+348, b_+349); mem_wr(gb, DE, A);
  CYC(b_+349, b_+350); H = D;
  CYC(b_+350, b_+352); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+352, b_+354); mem_wr(gb, HL, 0x10);
  CYC(b_+354, b_+356); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+356, b_+358); mem_wr(gb, HL, 0x50);
  CYC(b_+358, b_+360); A = 0x98;
  CALL_C(b_+360, s_playSound, SYM(playSound_b00), b_+363);
  CALL_C(b_+363, s_objectCenterOnTile, SYM(objectCenterOnTile), b_+366);
  CYC(b_+366, b_+368); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+368, b_+369); A = mem_rd(gb, HL);
  CYC(b_+369, b_+371); alu_sub(gb, 0x08);
  CYC(b_+371, b_+372); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+372, b_+373); alu_xor(gb, A);
  CYC(b_+373, b_+374); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+374, b_+375); mem_wr(gb, HL, A);
  CALL_C(b_+375, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+378);
  CYC(b_+378, b_+381);
  TAIL(objectSetVisible80);
subid1To8_state1:
  CALL_C(b_+381, s_objectApplySpeed, SYM(objectApplySpeed), b_+384);
  CALL_C(b_+384, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+387);
  if (!(F & FZ)) { RET_TAKEN(b_+387); return; }
  CYC(b_+387, b_+388);
  CYC(b_+388, b_+391);
  TAIL(interactionIncState);
subid1To8_state2:
  CYC(b_+391, b_+394); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+394, b_+395); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+395); return; }
  CYC(b_+395, b_+396);
  CYC(b_+396, b_+399);
  TAIL(interactionIncState);
subid1To8_state3:
  CALL_C(b_+399, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+402);
  if (!(F & FC)) { CYCT(b_+402, b_+405); TAIL(interactionDelete); }
  CYC(b_+402, b_+405);
  CYC(b_+405, b_+408); A = mem_rd(gb, wFrameCounter);
  CYC(b_+408, b_+409); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+409); return; }
  CYC(b_+409, b_+410);
  CYC(b_+410, b_+411); H = D;
  CYC(b_+411, b_+413); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+413, b_+414); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+414, b_+415); A = mem_rd(gb, HL);
  CYC(b_+415, b_+417); alu_and(gb, 0x1f);
  CYC(b_+417, b_+418); mem_wr(gb, HL, A);
  CYC(b_+418, b_+419); E = L;
  CYC(b_+419, b_+420); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+420, s_func_6c8f_hook, SYM(func_6c8f), b_+423);
  else CYC(b_+420, b_+423);
  CYC(b_+423, b_+426); SET_BC(0x2850);
  CYC(b_+426, b_+429); A = mem_rd(gb, wTmpcfc0 + 0x01);
  CYC(b_+429, b_+432);
  TAIL(objectSetPositionInCircleArc);
}

void s_func_6c8f_hook(GB *gb) {
  BASE(func_6c8f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xc9;
  CYC(b_+2, b_+5);
  TAIL(playSound_b00);
}

void s_func_6c94_hook(GB *gb) {
  BASE(func_6c94);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_BC(0x8403);
  CALL_C(b_+9, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, wFrameCounter);
  CYC(b_+16, b_+18); alu_and(gb, 0x38);
  CYC(b_+18, b_+20); A = alu_swap(gb, A);
  CYC(b_+20, b_+21); alu_rlca(gb);
  CYC(b_+21, b_+24); SET_BC(SYM(table_6cbb));
  CALL_C(b_+24, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+29, b_+30); A = mem_rd(gb, BC);
  CYC(b_+30, b_+31); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+33); SET_BC(BC + 1);
  CYC(b_+33, b_+35); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+35, b_+36); A = mem_rd(gb, BC);
  CYC(b_+36, b_+37); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  RET(b_+38); return;
}

void s_func_6ccb_hook(GB *gb) {
  BASE(func_6ccb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+11); alu_and(gb, 0x38);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+14); alu_rlca(gb);
  CYC(b_+14, b_+17); SET_HL(SYM(table_6ce2));
  CYC(b_+17, b_+18); makuSeedAndEssences_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return;
}

