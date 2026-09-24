#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/bc_bd_be.s.
// INTERAC_bc
// INTERAC_bd
// INTERAC_be

static uint16_t bc_bd_be_jump_table(GB *gb) {
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

static void bc_bd_be_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void bc_bd_be_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCodebb@subid2@func_772e
static void bc_bd_be_subid2_func_772e(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto subid2_func_772e;
subid2_setAnimationBasedOnAngle:
  CYC(b_+196, b_+198); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CALL_C(b_+199, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+202);
  CYC(b_+202, b_+205);
  TAIL(interactionSetAnimation);

subid2_func_772e:
  CYC(b_+373, b_+375); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+375, b_+376); A = mem_rd(gb, DE);
  CYC(b_+376, b_+377); A = alu_dec8(gb, A);
  CYC(b_+377, b_+378); B = A;
  CYC(b_+378, b_+380); A = alu_swap(gb, A);
  CYC(b_+380, b_+382); A = alu_sra(gb, A);
  CYC(b_+382, b_+383); alu_add(gb, B);
  CYC(b_+383, b_+386); SET_HL(b_+417 /* @subid2@table_775a */);
  CYC(b_+386, b_+387); bc_bd_be_add_a_to_hl(gb, b_+387);
  CYC(b_+387, b_+389); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+389, b_+390); A = mem_rd(gb, DE);
  CYC(b_+390, b_+391); bc_bd_be_add_double_index(gb, b_+391);
  CYC(b_+391, b_+392); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+392, b_+393); B = mem_rd(gb, HL);
  CYC(b_+393, b_+394); L = alu_inc8(gb, L);
  CYC(b_+394, b_+396); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+396, b_+397); mem_wr(gb, DE, A);
  CYC(b_+397, b_+399); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+399, b_+400); A = B;
  CYC(b_+400, b_+401); mem_wr(gb, DE, A);
  CYC(b_+401, b_+403); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+403, b_+404); A = mem_rd(gb, DE);
  CYC(b_+404, b_+405); B = A;
  CYC(b_+405, b_+406); B = alu_inc8(gb, B);
  CYC(b_+406, b_+407); A = mem_rd(gb, HL);
  CYC(b_+407, b_+408); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+408, b_+410); goto L_7755; }
  CYC(b_+408, b_+410);
  CYC(b_+410, b_+412); B = 0x00;
L_7755:
  CYC(b_+412, b_+413); A = B;
  CYC(b_+413, b_+414); mem_wr(gb, DE, A);
  CYC(b_+414, b_+417);
  goto subid2_setAnimationBasedOnAngle;
}

// interactionCodebb@subid2@animateTwiceAndApplySpeed
static void bc_bd_be_subid2_animateTwiceAndApplySpeed(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+230, s_interactionAnimate, SYM(interactionAnimate), b_+233);
  CALL_C(b_+233, s_interactionAnimate, SYM(interactionAnimate), b_+236);
  CYC(b_+236, b_+239);
  TAIL(objectApplySpeed);
}

// interactionCodebb@subid2@setAnimationBasedOnAngle
static void bc_bd_be_subid2_setAnimationBasedOnAngle(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+196, b_+198); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CALL_C(b_+199, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+202);
  CYC(b_+202, b_+205);
  TAIL(interactionSetAnimation);
}

// interactionCodebb@subid1
static void bc_bd_be_subid1(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+115, b_+117); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+117, b_+118); A = mem_rd(gb, DE);
  CYC(b_+118, b_+119); push_effect(gb, b_+119);
  do { uint16_t jt_ = (bc_bd_be_jump_table(gb));
    if (jt_ == b_+123) goto subid2_state0;
    if (jt_ == b_+151) goto subid2_state1;
    HANDOFF(HL);
  } while (0);
subid2_state0:
  CYC(b_+123, b_+125); A = 0x01;
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CALL_C(b_+126, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+129);
  CYC(b_+129, b_+130); H = D;
  CYC(b_+130, b_+132); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+132, b_+133); A = mem_rd(gb, HL);
  CYC(b_+133, b_+135); B = 0x02;
  CYC(b_+135, b_+137); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+137, b_+139); goto L_7646; }
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+141); B = 0x00;
L_7646:
  CYC(b_+141, b_+143); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+143, b_+144); mem_wr(gb, HL, B);
  CYC(b_+144, b_+146); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x78);
  CYC(b_+148, b_+151);
  TAIL(objectSetVisiblec1);
subid2_state1:
  CYC(b_+151, b_+153); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+153, b_+154); A = mem_rd(gb, DE);
  CYC(b_+154, b_+155); push_effect(gb, b_+155);
  do { uint16_t jt_ = (bc_bd_be_jump_table(gb));
    if (jt_ == b_+179) goto subid2_substate0;
    if (jt_ == b_+205) goto subid2_substate1;
    if (jt_ == b_+239) goto subid2_substate2;
    if (jt_ == b_+249) goto subid2_substate3;
    if (jt_ == b_+271) goto subid2_substate4;
    if (jt_ == b_+285) goto subid2_substate5;
    if (jt_ == b_+444) goto subid2_substate6;
    if (jt_ == b_+467) goto subid2_substate7;
    if (jt_ == b_+489) goto subid2_substate8;
    if (jt_ == b_+504) goto subid2_substate9;
    if (jt_ == b_+526) goto subid2_substateA;
    if (jt_ == b_+546) goto subid2_substateB;
    HANDOFF(HL);
  } while (0);
subid2_substate0:
  CALL_C(b_+179, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+182);
  if (!(F & FZ)) { RET_TAKEN(b_+182); return; }
  CYC(b_+182, b_+183);
  CYC(b_+183, b_+185); mem_wr(gb, HL, 0x66);
  CYC(b_+185, b_+187); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+187, b_+189); mem_wr(gb, HL, 0x32);
  CYC(b_+189, b_+191); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x18);
  CALL_C(b_+193, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+196);
subid2_setAnimationBasedOnAngle:
  CYC(b_+196, b_+198); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CALL_C(b_+199, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+202);
  CYC(b_+202, b_+205);
  TAIL(interactionSetAnimation);
subid2_substate1:
  CALL_L(b_+205, bc_bd_be_subid2_animateTwiceAndApplySpeed, b_+208);
  CALL_C(b_+208, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+211);
  if (!(F & FZ)) { RET_TAKEN(b_+211); return; }
  CYC(b_+211, b_+212);
  CALL_C(b_+212, s_getRandomNumber, SYM(getRandomNumber), b_+215);
  CYC(b_+215, b_+217); alu_and(gb, 0x0f);
  CYC(b_+217, b_+219); alu_add(gb, 0x1e);
  CYC(b_+219, b_+220); mem_wr(gb, HL, A);
  CYC(b_+220, b_+222); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+222, b_+224); mem_wr(gb, HL, 0x08);
  CALL_L(b_+224, bc_bd_be_subid2_setAnimationBasedOnAngle, b_+227);
  CYC(b_+227, b_+230);
  TAIL(interactionIncSubstate);

subid2_substate2:
  CALL_C(b_+239, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+242);
  if (!(F & FZ)) { RET_TAKEN(b_+242); return; }
  CYC(b_+242, b_+243);
  CALL_C(b_+243, s_func_77eb_hook, SYM(func_77eb), b_+246);
  CYC(b_+246, b_+249);
  TAIL(interactionIncSubstate);
subid2_substate3:
  CALL_C(b_+249, s_func_77e5_hook, SYM(func_77e5), b_+252);
  CYC(b_+252, b_+255); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+255, b_+257); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+257); return; }
  CYC(b_+257, b_+258);
  CYC(b_+258, b_+260); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+260, b_+261); A = mem_rd(gb, DE);
  CYC(b_+261, b_+262); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+262); return; }
  CYC(b_+262, b_+263);
  CALL_C(b_+263, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+266);
  CYC(b_+266, b_+268); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+268, b_+270); mem_wr(gb, HL, 0x1e);
  RET(b_+270); return;
subid2_substate4:
  CALL_C(b_+271, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+274);
  if (!(F & FZ)) { RET_TAKEN(b_+274); return; }
  CYC(b_+274, b_+275);
  CYC(b_+275, b_+277); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+277, b_+279); mem_wr(gb, HL, 0x50);
  CALL_L(b_+279, bc_bd_be_subid2_func_772e, b_+282);
  CYC(b_+282, b_+285);
  TAIL(interactionIncSubstate);
subid2_substate5:
  CYC(b_+285, b_+288); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+288, b_+290); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+290, b_+292); goto L_76f1; }
  CYC(b_+290, b_+292);
  CYC(b_+292, b_+294); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+294, b_+295); A = mem_rd(gb, DE);
  CYC(b_+295, b_+296); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+296, b_+298); goto L_76f1; }
  CYC(b_+296, b_+298);
  CALL_C(b_+298, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+301);
  CYC(b_+301, b_+303); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+303, b_+305); mem_wr(gb, HL, 0x0a);
  CYC(b_+305, b_+307); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+307, b_+309); mem_wr(gb, HL, 0x18);
  CYC(b_+309, b_+312);
  goto subid2_setAnimationBasedOnAngle;
L_76f1:
  CYC(b_+312, b_+314); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+314, b_+315); A = mem_rd(gb, DE);
  CYC(b_+315, b_+316); push_effect(gb, b_+316);
  do { uint16_t jt_ = (bc_bd_be_jump_table(gb));
    if (jt_ == b_+322) goto subid2_var77_00;
    if (jt_ == b_+345) goto subid2_var77_01;
    if (jt_ == b_+357) goto subid2_var77_02;
    HANDOFF(HL);
  } while (0);
subid2_var77_00:
  CALL_L(b_+322, bc_bd_be_subid2_animateTwiceAndApplySpeed, b_+325);
  CALL_C(b_+325, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+328);
  if (!(F & FZ)) { RET_TAKEN(b_+328); return; }
  CYC(b_+328, b_+329);
  CYC(b_+329, b_+331); mem_wr(gb, HL, 0x0a);
  CYC(b_+331, b_+333); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+333, b_+334); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+334, b_+336); alu_cp(gb, 0x68);
  CYC(b_+336, b_+338); A = 0x01;
  if (F & FC) { CYCT(b_+338, b_+340); goto L_770f; }
  CYC(b_+338, b_+340);
  CYC(b_+340, b_+342); A = 0x03;
L_770f:
  CYC(b_+342, b_+345);
  TAIL(interactionSetAnimation);
subid2_var77_01:
  CALL_C(b_+345, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+348);
  if (!(F & FZ)) { RET_TAKEN(b_+348); return; }
  CYC(b_+348, b_+349);
  CYC(b_+349, b_+351); mem_wr(gb, HL, 0x1e);
  CYC(b_+351, b_+353); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+353, b_+354); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+354, b_+357);
  TAIL_S(func_77eb);
subid2_var77_02:
  CALL_C(b_+357, s_func_77e5_hook, SYM(func_77e5), b_+360);
  CALL_C(b_+360, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+363);
  if (!(F & FZ)) { RET_TAKEN(b_+363); return; }
  CYC(b_+363, b_+364);
  CYC(b_+364, b_+365); alu_xor(gb, A);
  CYC(b_+365, b_+367); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+367, b_+368); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+368, b_+369); mem_wr(gb, HL, A);
  CYC(b_+369, b_+371); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+371, b_+373); mem_wr(gb, HL, 0x00);
  CYC(b_+373, b_+375); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+375, b_+376); A = mem_rd(gb, DE);
  CYC(b_+376, b_+377); A = alu_dec8(gb, A);
  CYC(b_+377, b_+378); B = A;
  CYC(b_+378, b_+380); A = alu_swap(gb, A);
  CYC(b_+380, b_+382); A = alu_sra(gb, A);
  CYC(b_+382, b_+383); alu_add(gb, B);
  CYC(b_+383, b_+386); SET_HL(b_+417 /* @subid2@table_775a */);
  CYC(b_+386, b_+387); bc_bd_be_add_a_to_hl(gb, b_+387);
  CYC(b_+387, b_+389); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+389, b_+390); A = mem_rd(gb, DE);
  CYC(b_+390, b_+391); bc_bd_be_add_double_index(gb, b_+391);
  CYC(b_+391, b_+392); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+392, b_+393); B = mem_rd(gb, HL);
  CYC(b_+393, b_+394); L = alu_inc8(gb, L);
  CYC(b_+394, b_+396); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+396, b_+397); mem_wr(gb, DE, A);
  CYC(b_+397, b_+399); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+399, b_+400); A = B;
  CYC(b_+400, b_+401); mem_wr(gb, DE, A);
  CYC(b_+401, b_+403); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+403, b_+404); A = mem_rd(gb, DE);
  CYC(b_+404, b_+405); B = A;
  CYC(b_+405, b_+406); B = alu_inc8(gb, B);
  CYC(b_+406, b_+407); A = mem_rd(gb, HL);
  CYC(b_+407, b_+408); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+408, b_+410); goto L_7755; }
  CYC(b_+408, b_+410);
  CYC(b_+410, b_+412); B = 0x00;
L_7755:
  CYC(b_+412, b_+413); A = B;
  CYC(b_+413, b_+414); mem_wr(gb, DE, A);
  CYC(b_+414, b_+417);
  goto subid2_setAnimationBasedOnAngle;

subid2_substate6:
  CALL_C(b_+444, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+447);
  if (!(F & FZ)) { RET_TAKEN(b_+447); return; }
  CYC(b_+447, b_+448);
  CYC(b_+448, b_+450); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+450, b_+451); A = mem_rd(gb, DE);
  CYC(b_+451, b_+453); B = 0x34;
  CYC(b_+453, b_+455); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+455, b_+457); goto L_7784; }
  CYC(b_+455, b_+457);
  CYC(b_+457, b_+459); B = 0x20;
L_7784:
  CYC(b_+459, b_+460); mem_wr(gb, HL, B);
  CYC(b_+460, b_+462); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+462, b_+464); mem_wr(gb, HL, 0x3c);
  CYC(b_+464, b_+467);
  TAIL(interactionIncSubstate);
subid2_substate7:
  CALL_L(b_+467, bc_bd_be_subid2_animateTwiceAndApplySpeed, b_+470);
  CALL_C(b_+470, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+473);
  if (!(F & FZ)) { RET_TAKEN(b_+473); return; }
  CYC(b_+473, b_+474);
  CALL_C(b_+474, s_getRandomNumber, SYM(getRandomNumber), b_+477);
  CYC(b_+477, b_+479); alu_and(gb, 0x07);
  CYC(b_+479, b_+480); A = alu_inc8(gb, A);
  CYC(b_+480, b_+481); mem_wr(gb, HL, A);
  CYC(b_+481, b_+483); A = 0x01;
  CALL_C(b_+483, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+486);
  CYC(b_+486, b_+489);
  TAIL(interactionIncSubstate);
subid2_substate8:
  CYC(b_+489, b_+492); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+492, b_+494); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+494); return; }
  CYC(b_+494, b_+495);
  CALL_C(b_+495, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+498);
  if (!(F & FZ)) { RET_TAKEN(b_+498); return; }
  CYC(b_+498, b_+499);
  CALL_C(b_+499, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+502);
  CYC(b_+502, b_+504);
  TAIL_S(func_77eb);
subid2_substate9:
  CALL_C(b_+504, s_func_77e5_hook, SYM(func_77e5), b_+507);
  CYC(b_+507, b_+510); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+510, b_+512); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+512); return; }
  CYC(b_+512, b_+513);
  CYC(b_+513, b_+515); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+515, b_+516); A = mem_rd(gb, DE);
  CYC(b_+516, b_+517); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+517); return; }
  CYC(b_+517, b_+518);
  CALL_C(b_+518, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+521);
  CYC(b_+521, b_+523); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+523, b_+525); mem_wr(gb, HL, 0x0c);
  RET(b_+525); return;
subid2_substateA:
  CALL_C(b_+526, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+529);
  if (!(F & FZ)) { RET_TAKEN(b_+529); return; }
  CYC(b_+529, b_+530);
  CALL_C(b_+530, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+533);
  CYC(b_+533, b_+535); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+535, b_+537); mem_wr(gb, HL, 0x50);
  CYC(b_+537, b_+539); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+539, b_+541); mem_wr(gb, HL, 0x3c);
  CYC(b_+541, b_+543); A = 0x03;
  CYC(b_+543, b_+546);
  TAIL(interactionSetAnimation);
subid2_substateB:
  CALL_L(b_+546, bc_bd_be_subid2_animateTwiceAndApplySpeed, b_+549);
  CALL_C(b_+549, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+552);
  if (F & FZ) { CYCT(b_+552, b_+555); TAIL(interactionDelete); }
  CYC(b_+552, b_+555);
  RET(b_+555); return;
}

// interactionCodebb@subid0
static void bc_bd_be_subid0(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (bc_bd_be_jump_table(gb));
    if (jt_ == b_+24) goto subid0_state0;
    if (jt_ == b_+33) goto subid0_state1;
    if (jt_ == b_+51) goto subid0_state2;
    if (jt_ == b_+71) goto subid0_state3;
    if (jt_ == b_+100) goto subid0_state4;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CALL_C(b_+24, s_func_7867_hook, SYM(func_7867), b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CALL_C(b_+30, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+33);
subid0_state1:
  CALL_C(b_+33, s_interactionRunScript, SYM(interactionRunScript), b_+36);
  CALL_C(b_+36, s_func_7886_hook, SYM(func_7886), b_+39);
  CYC(b_+39, b_+42); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+42, b_+44); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+44); return; }
  CYC(b_+44, b_+45);
  CALL_C(b_+45, s_func_788e_hook, SYM(func_788e), b_+48);
  CYC(b_+48, b_+51);
  TAIL(interactionIncState);
subid0_state2:
  CALL_C(b_+51, s_interactionRunScript, SYM(interactionRunScript), b_+54);
  CALL_C(b_+54, s_func_7886_hook, SYM(func_7886), b_+57);
  CALL_C(b_+57, s_decVar3c_hook, SYM(decVar3c), b_+60);
  if (!(F & FZ)) { RET_TAKEN(b_+60); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+64, b_+66); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x0a);
  CYC(b_+68, b_+71);
  TAIL_S(func_78c3);
subid0_state3:
  CALL_C(b_+71, s_interactionRunScript, SYM(interactionRunScript), b_+74);
  CALL_C(b_+74, s_func_7886_hook, SYM(func_7886), b_+77);
  CALL_C(b_+77, s_decVar3c_hook, SYM(decVar3c), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+83, b_+84); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+84, b_+86); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x28);
  CYC(b_+88, b_+90); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x58);
  CALL_C(b_+92, s_func_78b3_hook, SYM(func_78b3), b_+95);
  CYC(b_+95, b_+97); A = 0xd2;
  CYC(b_+97, b_+100);
  TAIL(playSound_b00);
subid0_state4:
  CALL_C(b_+100, s_decVar3c_hook, SYM(decVar3c), b_+103);
  if (F & FZ) { CYCT(b_+103, b_+106); TAIL(interactionDelete); }
  CYC(b_+103, b_+106);
  CALL_C(b_+106, s_objectApplySpeed, SYM(objectApplySpeed), b_+109);
  CALL_C(b_+109, s_interactionRunScript, SYM(interactionRunScript), b_+112);
  CYC(b_+112, b_+115);
  TAIL_S(func_7886);
}

// INTERAC_bc
// INTERAC_bd
// INTERAC_be
void s_interactionCodebc_hook(GB *gb) {
  BASE(interactionCodebc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (bc_bd_be_jump_table(gb));
    if (jt_ == (b_ - 558)) { bc_bd_be_subid0(gb); return; }
    if (jt_ == b_+14) goto subid1;
    if (jt_ == b_+46) goto subid2;
    if (jt_ == (b_ - 453)) { bc_bd_be_subid1(gb); return; }
    if (jt_ == b_+67) goto subid4;
    HANDOFF(HL);
  } while (0);
subid1:
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (bc_bd_be_jump_table(gb));
    if (jt_ == b_+22) goto subid1_state0;
    if (jt_ == b_+40) goto runScriptAnimateAsNPC;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CALL_C(b_+22, s_func_7867_hook, SYM(func_7867), b_+25);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+30); alu_cp(gb, 0xbd);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto runScriptAnimateAsNPC; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+36); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CALL_C(b_+37, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+40);
runScriptAnimateAsNPC:
  CALL_C(b_+40, s_interactionRunScript, SYM(interactionRunScript), b_+43);
  CYC(b_+43, b_+46);
  TAIL(interactionAnimateAsNpc);
subid2:
  CYC(b_+46, b_+48); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); push_effect(gb, b_+50);
  do { uint16_t jt_ = (bc_bd_be_jump_table(gb));
    if (jt_ == b_+54) goto subid2_state0;
    if (jt_ == b_+40) goto runScriptAnimateAsNPC;
    HANDOFF(HL);
  } while (0);
subid2_state0:
  CALL_C(b_+54, s_func_7867_hook, SYM(func_7867), b_+57);
  CYC(b_+57, b_+59); A = 0x02;
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CALL_C(b_+62, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+65);
  CYC(b_+65, b_+67);
  goto runScriptAnimateAsNPC;
subid4:
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); push_effect(gb, b_+71);
  do { uint16_t jt_ = (bc_bd_be_jump_table(gb));
    if (jt_ == b_+75) goto subid4_state0;
    if (jt_ == b_+107) goto subid4_state1;
    HANDOFF(HL);
  } while (0);
subid4_state0:
  CALL_C(b_+75, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+78);
  if (F & FZ) { CYCT(b_+78, b_+81); TAIL(interactionDelete); }
  CYC(b_+78, b_+81);
  CALL_C(b_+81, s_func_78ce_hook, SYM(func_78ce), b_+84);
  CYC(b_+84, b_+86); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+86, b_+87); A = mem_rd(gb, DE);
  CYC(b_+87, b_+88); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+88, b_+91); TAIL(interactionDelete); }
  CYC(b_+88, b_+91);
  CALL_C(b_+91, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+94);
  CALL_C(b_+94, s_interactionIncState, SYM(interactionIncState), b_+97);
  CYC(b_+97, b_+99); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+99, b_+101); mem_wr(gb, HL, 0x02);
  CYC(b_+101, b_+104); SET_HL(SYM(linkedGameNpcScript));
  CALL_C(b_+104, s_interactionSetScript, SYM(interactionSetScript), b_+107);
subid4_state1:
  CALL_C(b_+107, s_interactionRunScript, SYM(interactionRunScript), b_+110);
  CYC(b_+110, b_+113);
  TAIL_SG(npcFaceLinkAndAnimate);
}

void s_decVar3c_hook(GB *gb) {
  BASE(decVar3c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void s_func_7867_hook(GB *gb) {
  BASE(func_7867);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_sub(gb, 0xba);
  CYC(b_+8, b_+11); SET_HL(SYM(ba_to_beScripts));
  CYC(b_+11, b_+12); bc_bd_be_add_double_index(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); H = mem_rd(gb, HL);
  CYC(b_+14, b_+15); L = A;
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); bc_bd_be_add_double_index(gb, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); H = mem_rd(gb, HL);
  CYC(b_+21, b_+22); L = A;
  CALL_C(b_+22, s_interactionSetScript, SYM(interactionSetScript), b_+25);
  CALL_C(b_+25, s_objectSetVisible81, SYM(objectSetVisible81), b_+28);
  CYC(b_+28, b_+31);
  TAIL(interactionIncState);
}

void s_func_7886_hook(GB *gb) {
  BASE(func_7886);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8);
  TAIL(interactionAnimate);
}

void s_func_788e_hook(GB *gb) {
  BASE(func_788e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0xba);
  CYC(b_+5, b_+8); SET_HL(SYM(table_78a9));
  CYC(b_+8, b_+9); bc_bd_be_add_double_index(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+19); alu_add(gb, 0x04);
  CYC(b_+19, b_+21); alu_and(gb, 0x18);
  CYC(b_+21, b_+23); A = alu_swap(gb, A);
  CYC(b_+23, b_+24); alu_rlca(gb);
  CYC(b_+24, b_+27);
  TAIL(interactionSetAnimation);
}

void s_func_78b3_hook(GB *gb) {
  BASE(func_78b3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x4b);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x02);
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x78);
  CYC(b_+13, b_+16);
  TAIL(objectCopyPosition);
}

void s_func_78c3_hook(GB *gb) {
  BASE(func_78c3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreePartSlot, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x27);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+11);
  TAIL(objectCopyPosition);
}

void s_func_78ce_hook(GB *gb) {
  BASE(func_78ce);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x40;
  CALL_C(b_+2, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto L_78d6; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
L_78d6:
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+11, b_+13); alu_cp(gb, 0x07);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  if (F & FC) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x01);
  RET(b_+18); return;
}

