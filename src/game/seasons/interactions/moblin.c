#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/moblin.s.
// INTERAC_MOBLIN

static uint16_t moblin_jump_table(GB *gb) {
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

static void moblin_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode95@state1@func_557c
static void moblin_state1_func_557c(GB *gb) {
  BASE(interactionCode95);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+347, b_+348); H = D;
  CYC(b_+348, b_+350); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+350, b_+351); A = mem_rd(gb, HL);
  CYC(b_+351, b_+353); alu_cp(gb, 0x70);
  if (!(F & FZ)) { RET_TAKEN(b_+353); return; }
  CYC(b_+353, b_+354);
  CYC(b_+354, b_+356); mem_wr(gb, HL, 0x00);
  CYC(b_+356, b_+359);
  TAIL(playSound_b00);
}

// INTERAC_MOBLIN
void s_interactionCode96_hook(GB *gb) {
  BASE(interactionCode96);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (moblin_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+115) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (moblin_jump_table(gb));
    if (jt_ == b_+32) goto state0_subid0;
    if (jt_ == b_+40) goto state0_subid1;
    if (jt_ == SYM(objectSetVisible82) && hook_is(gb, SYM(objectSetVisible82), s_objectSetVisible82)) { s_objectSetVisible82(gb); return; }
    if (jt_ == b_+48) goto state0_subid3;
    if (jt_ == b_+56) goto state0_subid4;
    if (jt_ == b_+76) goto state0_subid5;
    HANDOFF(HL);
  } while (0);
state0_subid0:
  CYC(b_+32, b_+35); SET_HL(SYM(table_57d0));
L_55e8:
  CALL_C(b_+35, s_func_57ba_hook, SYM(func_57ba), b_+38);
  CYC(b_+38, b_+40);
  goto state1;
state0_subid1:
  CALL_C(b_+40, s_objectSetVisible81, SYM(objectSetVisible81), b_+43);
  CYC(b_+43, b_+46); SET_HL(SYM(table_57d6));
  CYC(b_+46, b_+48);
  goto L_55e8;
state0_subid3:
  CYC(b_+48, b_+50); A = 0x02;
  CALL_C(b_+50, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+53);
  CYC(b_+53, b_+56);
  TAIL(objectSetVisible80);
state0_subid4:
  CYC(b_+56, b_+59); SET_HL(SYM(script7421));
  CALL_C(b_+59, s_interactionSetScript, SYM(interactionSetScript), b_+62);
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+66, b_+68); goto L_560a; }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+69); A = alu_inc8(gb, A);
L_560a:
  CYC(b_+69, b_+70); A = alu_inc8(gb, A);
  CALL_C(b_+70, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+73);
  CYC(b_+73, b_+76);
  TAIL(interactionAnimateAsNpc);
state0_subid5:
  CYC(b_+76, b_+78); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+78, b_+79); A = mem_rd(gb, DE);
  CYC(b_+79, b_+82); SET_HL(SYM(table_57dc));
  CYC(b_+82, b_+83); moblin_add_double_index(gb, b_+83);
  CYC(b_+83, b_+84); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+84, b_+85); H = mem_rd(gb, HL);
  CYC(b_+85, b_+86); L = A;
  CALL_C(b_+86, s_interactionSetScript, SYM(interactionSetScript), b_+89);
  CYC(b_+89, b_+91); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+95); SET_HL(b_+107 /* @table_5630 */);
  CYC(b_+95, b_+96); moblin_add_double_index(gb, b_+96);
  CYC(b_+96, b_+97); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+97, b_+99); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CALL_C(b_+101, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+104);
  CYC(b_+104, b_+107);
  TAIL(interactionAnimateAsNpc);

state1:
  CYC(b_+115, b_+117); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+117, b_+118); A = mem_rd(gb, DE);
  CYC(b_+118, b_+119); push_effect(gb, b_+119);
  do { uint16_t jt_ = (moblin_jump_table(gb));
    if (jt_ == b_+133) goto state1_subid0;
    if (jt_ == b_+363) goto state1_subid2;
    if (jt_ == b_+398) goto state1_subid3;
    if (jt_ == b_+410) goto state1_subid4;
    HANDOFF(HL);
  } while (0);
state1_subid0:
  CYC(b_+133, b_+135); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+135, b_+136); A = mem_rd(gb, DE);
  CYC(b_+136, b_+137); push_effect(gb, b_+137);
  do { uint16_t jt_ = (moblin_jump_table(gb));
    if (jt_ == b_+153) goto state1_subid1_substate0;
    if (jt_ == b_+203) goto state1_subid1_substate1;
    if (jt_ == b_+227) goto state1_subid1_substate2;
    if (jt_ == b_+246) goto state1_subid1_substate3;
    if (jt_ == b_+257) goto state1_subid1_substate4;
    if (jt_ == b_+299) goto state1_subid1_substate5;
    if (jt_ == b_+319) goto state1_subid1_substate6;
    if (jt_ == b_+338) goto state1_subid1_substate7;
    HANDOFF(HL);
  } while (0);
state1_subid1_substate0:
  CYC(b_+153, b_+156); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+156, b_+157); A = mem_rd(gb, HL);
  CYC(b_+157, b_+159); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+159, b_+162); TAIL_S(func_5768); }
  CYC(b_+159, b_+162);
  CYC(b_+162, b_+163); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+163, b_+166); TAIL(interactionDelete); }
  CYC(b_+163, b_+166);
  CALL_C(b_+166, s_interactionRunScript, SYM(interactionRunScript), b_+169);
  CYC(b_+169, b_+171); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+171, b_+172); A = mem_rd(gb, DE);
  CYC(b_+172, b_+173); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+173, b_+176); TAIL_SG(npcFaceLinkAndAnimate); }
  CYC(b_+173, b_+176);
  CYC(b_+176, b_+178); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+178, b_+179); A = mem_rd(gb, DE);
  CYC(b_+179, b_+180); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+180, s_interactionAnimate, SYM(interactionAnimate), b_+183);
  else CYC(b_+180, b_+183);
  CYC(b_+183, b_+185); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+187, b_+189); goto L_568a; }
  CYC(b_+187, b_+189);
  CYC(b_+189, b_+190); alu_xor(gb, A);
  CYC(b_+190, b_+191); mem_wr(gb, DE, A);
  CYC(b_+191, b_+194); SET_BC(0x3801);
  CALL_C(b_+194, s_showText, SYM(showText), b_+197);
L_568a:
  CALL_C(b_+197, s_interactionAnimate, SYM(interactionAnimate), b_+200);
  CYC(b_+200, b_+203);
  TAIL(objectPreventLinkFromPassing);
state1_subid1_substate1:
  CALL_C(b_+203, s_interactionAnimate, SYM(interactionAnimate), b_+206);
  CALL_C(b_+206, s_interactionAnimate, SYM(interactionAnimate), b_+209);
  CALL_C(b_+209, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+212);
  if (!(F & FZ)) { CYCT(b_+212, b_+215); TAIL(objectApplyComponentSpeed); }
  CYC(b_+212, b_+215);
  CYC(b_+215, b_+217); A = 0x08;
  CALL_C(b_+217, s_setLinkIDOverride, SYM(setLinkIDOverride), b_+220);
  CYC(b_+220, b_+222); L = 0x02;
  CYC(b_+222, b_+224); mem_wr(gb, HL, 0x09);
  CYC(b_+224, b_+227);
  TAIL(interactionIncSubstate);
state1_subid1_substate2:
  CYC(b_+227, b_+230); SET_HL(wTmpcfc0 + 0x11);
  CYC(b_+230, b_+231); A = mem_rd(gb, HL);
  CYC(b_+231, b_+232); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+232, b_+235); TAIL_SG(npcFaceLinkAndAnimate); }
  CYC(b_+232, b_+235);
  CALL_C(b_+235, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+238);
  CYC(b_+238, b_+240); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+240, b_+241); A = mem_rd(gb, HL);
  CYC(b_+241, b_+243); alu_add(gb, 0x0b);
  CYC(b_+243, b_+246);
  TAIL(interactionSetAnimation);
state1_subid1_substate3:
  CALL_C(b_+246, s_interactionAnimate, SYM(interactionAnimate), b_+249);
  CYC(b_+249, b_+251); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+251, b_+252); A = mem_rd(gb, DE);
  CYC(b_+252, b_+253); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+253); return; }
  CYC(b_+253, b_+254);
  CYC(b_+254, b_+257);
  TAIL(interactionIncSubstate);
state1_subid1_substate4:
  CYC(b_+257, b_+259); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+259, b_+260); A = mem_rd(gb, DE);
  CYC(b_+260, b_+261); A = alu_inc8(gb, A);
  CYC(b_+261, b_+262); B = A;
  CYC(b_+262, b_+265); SET_HL(wTmpcfc0 + 0x11);
  CYC(b_+265, b_+266); A = mem_rd(gb, HL);
  CYC(b_+266, b_+267); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+267, b_+270); TAIL_SG(npcFaceLinkAndAnimate); }
  CYC(b_+267, b_+270);
  CALL_C(b_+270, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+273);
  CYC(b_+273, b_+275); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+275, b_+277); mem_wr(gb, HL, 0x28);
  CYC(b_+277, b_+279); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+279, b_+280); A = mem_rd(gb, HL);
  CYC(b_+280, b_+282); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(b_+282, b_+284); goto state1_subid1_func_56fe; }
  CYC(b_+282, b_+284);
  CYC(b_+284, b_+286); A = 0x18;
  if (!(F & FC)) { CYCT(b_+286, b_+288); goto L_56e7; }
  CYC(b_+286, b_+288);
  CYC(b_+288, b_+290); A = 0x08;
L_56e7:
  CYC(b_+290, b_+292); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+292, b_+293); mem_wr(gb, DE, A);
  CALL_C(b_+293, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+296);
  CYC(b_+296, b_+299);
  TAIL(interactionSetAnimation);
state1_subid1_substate5:
  CALL_C(b_+299, s_objectApplySpeed, SYM(objectApplySpeed), b_+302);
  CYC(b_+302, b_+304); alu_cp(gb, 0x50);
  if (!(F & FZ)) { CYCT(b_+304, b_+306); goto L_56fe; }
  CYC(b_+304, b_+306);
  CALL_C(b_+306, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+309);
  CYC(b_+309, b_+311); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+311, b_+313); mem_wr(gb, HL, 0x05);
L_56fe:
  CALL_C(b_+313, s_interactionAnimate, SYM(interactionAnimate), b_+316);
  CYC(b_+316, b_+319);
  TAIL(interactionAnimate);
state1_subid1_substate6:
  CALL_C(b_+319, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+322);
  if (!(F & FZ)) { CYCT(b_+322, b_+325); TAIL(interactionAnimate); }
  CYC(b_+322, b_+325);
state1_subid1_func_56fe:
  CYC(b_+325, b_+327); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+327, b_+329); mem_wr(gb, HL, 0x07);
  CYC(b_+329, b_+331); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+331, b_+333); mem_wr(gb, HL, 0x10);
  CYC(b_+333, b_+335); A = 0x02;
  CYC(b_+335, b_+338);
  TAIL(interactionSetAnimation);
state1_subid1_substate7:
  CALL_C(b_+338, s_interactionAnimate, SYM(interactionAnimate), b_+341);
  CALL_C(b_+341, s_interactionAnimate, SYM(interactionAnimate), b_+344);
  CALL_C(b_+344, s_objectApplySpeed, SYM(objectApplySpeed), b_+347);
  CALL_C(b_+347, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+350);
  if (F & FC) { RET_TAKEN(b_+350); return; }
  CYC(b_+350, b_+351);
  CYC(b_+351, b_+354); SET_HL(wTmpcfc0 + 0x11);
  CYC(b_+354, b_+356); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+356, b_+357); A = mem_rd(gb, DE);
  CYC(b_+357, b_+359); alu_add(gb, 0x02);
  CYC(b_+359, b_+360); mem_wr(gb, HL, A);
  CYC(b_+360, b_+363);
  TAIL(interactionDelete);
state1_subid2:
  CALL_C(b_+363, s_interactionAnimate, SYM(interactionAnimate), b_+366);
  CALL_C(b_+366, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+369);
  if (!(F & FZ)) { CYCT(b_+369, b_+371); goto L_5743; }
  CYC(b_+369, b_+371);
  CALL_C(b_+371, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+374);
  if (!(F & FZ)) { RET_TAKEN(b_+374); return; }
  CYC(b_+374, b_+375);
  CYC(b_+375, b_+377); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+377, b_+379); mem_wr(gb, HL, 0x50);
  CYC(b_+379, b_+382);
  TAIL(interactionIncSubstate);
L_5743:
  CALL_C(b_+382, s_interactionAnimate, SYM(interactionAnimate), b_+385);
  CALL_L(b_+385, moblin_state1_func_557c, b_+388);
  CALL_C(b_+388, s_objectApplySpeed, SYM(objectApplySpeed), b_+391);
  CALL_C(b_+391, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+394);
  if (F & FC) { RET_TAKEN(b_+394); return; }
  CYC(b_+394, b_+395);
  CYC(b_+395, b_+398);
  TAIL(interactionDelete);
state1_subid3:
  CALL_C(b_+398, s_interactionAnimate, SYM(interactionAnimate), b_+401);
  CYC(b_+401, b_+404); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+404, b_+405); A = mem_rd(gb, HL);
  CYC(b_+405, b_+406); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+406); return; }
  CYC(b_+406, b_+407);
  CYC(b_+407, b_+410);
  TAIL(interactionDelete);
state1_subid4:
  CALL_C(b_+410, s_interactionRunScript, SYM(interactionRunScript), b_+413);
  if (F & FC) { CYCT(b_+413, b_+416); TAIL(interactionDelete); }
  CYC(b_+413, b_+416);
  CYC(b_+416, b_+419);
  TAIL(interactionAnimateAsNpc);
}

void s_func_5768_hook(GB *gb) {
  BASE(func_5768);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+3);
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x20);
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+14); SET_HL(w1Link_yh);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); alu_sub(gb, B);
  CALL_C(b_+16, s_func_57ad_hook, SYM(func_57ad), b_+19);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+22, b_+23); mem_wr(gb, HL, C);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+25); mem_wr(gb, HL, B);
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+32); SET_HL(w1Link_xh);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+34); C = A;
  CYC(b_+34, b_+36); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); alu_or(gb, A);
  CYC(b_+38, b_+40); A = 0x0c;
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto L_5794; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); A = 0xf4;
L_5794:
  CYC(b_+44, b_+45); alu_add(gb, C);
  CYC(b_+45, b_+46); alu_sub(gb, B);
  CALL_C(b_+46, s_func_57ad_hook, SYM(func_57ad), b_+49);
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_SPEED_X;
  CYC(b_+52, b_+53); mem_wr(gb, HL, C);
  CYC(b_+53, b_+54); L = alu_inc8(gb, L);
  CYC(b_+54, b_+55); mem_wr(gb, HL, B);
  CALL_C(b_+55, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+58);
  CYC(b_+58, b_+60); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CALL_C(b_+61, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+64);
  CYC(b_+64, b_+65); E = alu_dec8(gb, E);
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+69);
  TAIL(interactionSetAnimation);
}

void s_func_57ad_hook(GB *gb) {
  BASE(func_57ad);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); C = 0x00;
  CYC(b_+3, b_+5); A = 0x05;
L_57b2:
  CYC(b_+5, b_+7); B = alu_sra(gb, B);
  CYC(b_+7, b_+9); C = alu_rr(gb, C);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_57b2; }
  CYC(b_+10, b_+12);
  RET(b_+12); return;
}

void s_func_57ba_hook(GB *gb) {
  BASE(func_57ba);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CALL_C(b_+1, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+4);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+8); B = alu_sla(gb, B);
  CYC(b_+8, b_+10); alu_adc(gb, 0x00);
  CYC(b_+10, b_+12); B = alu_sla(gb, B);
  CYC(b_+12, b_+14); alu_adc(gb, 0x00);
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CYC(b_+15, b_+16); moblin_add_double_index(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); H = mem_rd(gb, HL);
  CYC(b_+18, b_+19); L = A;
  CYC(b_+19, b_+22);
  TAIL(interactionSetScript);
}
