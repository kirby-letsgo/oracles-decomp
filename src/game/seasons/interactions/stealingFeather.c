#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/stealingFeather.s.
// INTERAC_STEALING_FEATHER

static uint16_t stealingFeather_jump_table(GB *gb) {
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

static void stealingFeather_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// interactionCode6e@subid1@func_6c22
static void stealingFeather_subid1_func_6c22(GB *gb) {
  BASE(interactionCode6e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+256, b_+259); SET_HL(w1Link);
  CYC(b_+259, b_+262);
  TAIL(objectCopyPosition);
}

// interactionCode6e@subid1@func_6c19
static void stealingFeather_subid1_func_6c19(GB *gb) {
  BASE(interactionCode6e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+247, b_+250); SET_HL(w1Link_direction);
  CYC(b_+250, b_+251); A = mem_rd(gb, HL);
  CYC(b_+251, b_+252); A = alu_inc8(gb, A);
  CYC(b_+252, b_+254); alu_and(gb, 0x03);
  CYC(b_+254, b_+255); mem_wr(gb, HL, A);
  RET(b_+255); return;
}

// INTERAC_STEALING_FEATHER
void s_interactionCode6e_hook(GB *gb) {
  BASE(interactionCode6e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (stealingFeather_jump_table(gb));
    if (jt_ == b_+10) goto subid0;
    if (jt_ == b_+152) goto subid1;
    if (jt_ == b_+262) goto subid2;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (stealingFeather_jump_table(gb));
    if (jt_ == b_+20) goto subid0_state0;
    if (jt_ == b_+55) goto subid0_state1;
    if (jt_ == b_+103) goto subid0_state2;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+26);
  CYC(b_+26, b_+28); A = 0x17;
  CALL_C(b_+28, s_loseTreasure, SYM(loseTreasure), b_+31);
  CYC(b_+31, b_+34); SET_BC(0xfd80);
  CALL_C(b_+34, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+37);
  CYC(b_+37, b_+39); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0f);
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x18);
  CYC(b_+45, b_+47); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+49, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+52);
  CYC(b_+52, b_+55);
  TAIL(objectSetVisiblec0);
subid0_state1:
  CALL_C(b_+55, s_objectApplySpeed, SYM(objectApplySpeed), b_+58);
  CYC(b_+58, b_+59); H = D;
  CYC(b_+59, b_+61); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+61, b_+63); A = 0x18;
  CYC(b_+63, b_+64); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+64, b_+66); goto L_6b65; }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+67); mem_wr(gb, HL, A);
L_6b65:
  CALL_C(b_+67, s_interactionAnimate, SYM(interactionAnimate), b_+70);
  CYC(b_+70, b_+72); C = 0x14;
  CALL_C(b_+72, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+75);
  CALL_C(b_+75, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+78);
  if (!(F & FZ)) { RET_TAKEN(b_+78); return; }
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+81); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+81, b_+82); A = mem_rd(gb, HL);
  CYC(b_+82, b_+84); L = INTERACTION_BASE + OBJ_SPEED_X;
  CYC(b_+84, b_+85); mem_wr(gb, HL, A);
  CYC(b_+85, b_+87); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+87, b_+88); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+88, b_+90); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+94); mem_wr(gb, wTmpcfc0 + 0x01, A);
  CYC(b_+94, b_+97); SET_HL(wTmpcfc0);
  CYC(b_+97, b_+99); mem_wr(gb, HL, mem_rd(gb, HL) | 0x04);
  CYC(b_+99, b_+100); alu_xor(gb, A);
  CALL_C(b_+100, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+103);
subid0_state2:
  CYC(b_+103, b_+106); SET_HL(wTmpcfc0);
  CYC(b_+106, b_+108); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+108, b_+111); TAIL(interactionDelete); }
  CYC(b_+108, b_+111);
  CYC(b_+111, b_+114); A = mem_rd(gb, wFrameCounter);
  CYC(b_+114, b_+116); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+116); return; }
  CYC(b_+116, b_+117);
  CYC(b_+117, b_+118); H = D;
  CYC(b_+118, b_+120); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+120, b_+121); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+124); alu_and(gb, 0x0f);
  CYC(b_+124, b_+127); SET_HL(b_+136 /* @subid0@table_6baa */);
  CYC(b_+127, b_+128); stealingFeather_add_a_to_hl(gb, b_+128);
  CYC(b_+128, b_+130); E = INTERACTION_BASE + OBJ_SPEED_X;
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+132); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+132, b_+134); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+134, b_+135); mem_wr(gb, DE, A);
  RET(b_+135); return;

subid1:
  CYC(b_+152, b_+154); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+154, b_+155); A = mem_rd(gb, DE);
  CYC(b_+155, b_+156); push_effect(gb, b_+156);
  do { uint16_t jt_ = (stealingFeather_jump_table(gb));
    if (jt_ == b_+162) goto subid1_state0;
    if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), s_interactionRunScript)) { s_interactionRunScript(gb); return; }
    if (jt_ == b_+179) goto subid1_state2;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+162, b_+164); A = 0x01;
  CYC(b_+164, b_+165); mem_wr(gb, DE, A);
  CALL_C(b_+165, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+168);
  CYC(b_+168, b_+170); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+170, b_+173); TAIL(interactionDelete); }
  CYC(b_+170, b_+173);
  CYC(b_+173, b_+176); SET_HL(SYM(stealingFeatherScript));
  CYC(b_+176, b_+179);
  TAIL(interactionSetScript);
subid1_state2:
  CYC(b_+179, b_+181); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+181, b_+182); A = mem_rd(gb, DE);
  CYC(b_+182, b_+183); push_effect(gb, b_+183);
  do { uint16_t jt_ = (stealingFeather_jump_table(gb));
    if (jt_ == b_+187) goto subid1_substate0;
    if (jt_ == b_+207) goto subid1_substate1;
    HANDOFF(HL);
  } while (0);
subid1_substate0:
  CYC(b_+187, b_+189); A = 0x01;
  CYC(b_+189, b_+190); mem_wr(gb, DE, A);
  CYC(b_+190, b_+193); SET_BC(0xfe00);
  CALL_C(b_+193, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+196);
  CYC(b_+196, b_+198); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+198, b_+201); A = mem_rd(gb, w1Link_yh);
  CYC(b_+201, b_+202); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+202, b_+203); L = alu_inc8(gb, L);
  CYC(b_+203, b_+206); A = mem_rd(gb, w1Link_xh);
  CYC(b_+206, b_+207); mem_wr(gb, HL, A);
subid1_substate1:
  CYC(b_+207, b_+210); A = mem_rd(gb, wFrameCounter);
  CYC(b_+210, b_+211); alu_rrca(gb);
  if (F & FC) CALL_L_CC(b_+211, stealingFeather_subid1_func_6c19, b_+214);
  else CYC(b_+211, b_+214);
  CALL_C(b_+214, s_objectApplySpeed, SYM(objectApplySpeed), b_+217);
  CYC(b_+217, b_+219); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+219, b_+220); A = mem_rd(gb, DE);
  CYC(b_+220, b_+222); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(b_+222, b_+224); goto L_6c07; }
  CYC(b_+222, b_+224);
  CYC(b_+224, b_+226); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+226, b_+228); A = 0x0c;
  CYC(b_+228, b_+229); mem_wr(gb, DE, A);
L_6c07:
  CYC(b_+229, b_+231); C = 0x40;
  CALL_C(b_+231, s_objectUpdateSpeedZAndBounce, SYM(objectUpdateSpeedZAndBounce), b_+234);
  if (!(F & FC)) { CYCT(b_+234, b_+236); goto subid1_func_6c22; }
  CYC(b_+234, b_+236);
  CALL_L(b_+236, stealingFeather_subid1_func_6c22, b_+239);
  CYC(b_+239, b_+241); A = 0x02;
  CYC(b_+241, b_+244); mem_wr(gb, wcc50, A);
  CYC(b_+244, b_+247);
  TAIL(interactionDelete);

subid1_func_6c22:
  CYC(b_+256, b_+259); SET_HL(w1Link);
  CYC(b_+259, b_+262);
  TAIL(objectCopyPosition);
subid2:
  CYC(b_+262, b_+264); A = 0x0f;
  CALL_C(b_+264, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+267);
  CYC(b_+267, b_+269); A = 0x10;
  CALL_C(b_+269, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+272);
  CYC(b_+272, b_+274); A = 0x11;
  CALL_C(b_+274, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+277);
  CYC(b_+277, b_+280);
  TAIL(interactionDelete);
}
