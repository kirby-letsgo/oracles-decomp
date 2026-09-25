#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/47.s.
// PART_47

static uint16_t i47_jump_table(GB *gb) {
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

static void i47_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// partCode47@func_778b
static void i47_func_778b(GB *gb) {
  BASE(partCode47);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+601, b_+603); A = 0x30;
  CALL_C(b_+603, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+606);
  CYC(b_+606, b_+607); B = mem_rd(gb, HL);
  CYC(b_+607, b_+608); L = alu_inc8(gb, L);
  CYC(b_+608, b_+609); C = mem_rd(gb, HL);
  CYC(b_+609, b_+611); L = PART_BASE + OBJ_YH;
  CYC(b_+611, b_+612); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+612, b_+613); alu_sub(gb, B);
  CYC(b_+613, b_+615); A = alu_sra(gb, A);
  CYC(b_+615, b_+617); A = alu_sra(gb, A);
  CYC(b_+617, b_+618); E = A;
  CYC(b_+618, b_+619); L = alu_inc8(gb, L);
  CYC(b_+619, b_+620); A = mem_rd(gb, HL);
  CYC(b_+620, b_+621); alu_sub(gb, C);
  CYC(b_+621, b_+623); A = alu_sra(gb, A);
  CYC(b_+623, b_+625); A = alu_sra(gb, A);
  CYC(b_+625, b_+626); L = A;
  RET(b_+626); return;
}

// partCode47@func_777f
static void i47_func_777f(GB *gb) {
  BASE(partCode47);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+589, b_+591); A = 0x01;
  CALL_C(b_+591, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+594);
  CYC(b_+594, b_+595); A = mem_rd(gb, HL);
  CYC(b_+595, b_+596); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+596); return; }
  CYC(b_+596, b_+597);
  CYC(b_+597, b_+598); SET_HL(pop_effect(gb));
  CYC(b_+598, b_+601);
  TAIL(partDelete);
}

// partCode47@func_776f
static void i47_func_776f(GB *gb) {
  BASE(partCode47);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+573, s_getFreePartSlot, SYM(getFreePartSlot), b_+576);
  if (!(F & FZ)) { RET_TAKEN(b_+576); return; }
  CYC(b_+576, b_+577);
  CYC(b_+577, b_+579); mem_wr(gb, HL, 0x48);
  CYC(b_+579, b_+580); L = alu_inc8(gb, L);
  CYC(b_+580, b_+582); mem_wr(gb, HL, 0x02);
  CYC(b_+582, b_+584); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+584, b_+586); A = 0xc0;
  CYC(b_+586, b_+587); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+587, b_+588); mem_wr(gb, HL, D);
  RET(b_+588); return;
}

// partCode47@func_7566
static void i47_func_7566(GB *gb) {
  BASE(partCode47);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+52, s_getFreePartSlot, SYM(getFreePartSlot), b_+55);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x47);
  CYC(b_+57, b_+58); L = alu_inc8(gb, L);
  CYC(b_+58, b_+60); A = 0x05;
  CYC(b_+60, b_+61); alu_sub(gb, B);
  CYC(b_+61, b_+62); mem_wr(gb, HL, A);
  CALL_C(b_+62, s_objectCopyPosition, SYM(objectCopyPosition), b_+65);
  CYC(b_+65, b_+67); L = 0xd7;
  CYC(b_+67, b_+68); mem_wr(gb, HL, C);
  CYC(b_+68, b_+69); L = alu_dec8(gb, L);
  RET(b_+69); return;
}

// PART_47
void s_partCode47_hook(GB *gb) {
  BASE(partCode47);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = PART_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (i47_jump_table(gb));
    if (jt_ == b_+16) goto subid0;
    if (jt_ == b_+70) goto subid1;
    if (jt_ == b_+461) goto subid2;
    if (jt_ == b_+507) goto subid3;
    if (jt_ == b_+533) goto subid4;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+16, b_+18); B = 0x04;
  CALL_C(b_+18, s_checkBPartSlotsAvailable, SYM(checkBPartSlotsAvailable), b_+21);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); B = 0x04;
  CYC(b_+24, b_+26); E = 0xd7;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); C = A;
  CALL_L(b_+28, i47_func_7566, b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x80);
  CYC(b_+33, b_+34); C = H;
  CYC(b_+34, b_+35); B = alu_dec8(gb, B);
L_7555:
  CALL_L(b_+35, i47_func_7566, b_+38);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0xc0);
  CYC(b_+40, b_+41); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto L_7555; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); A = 0x19;
  CALL_C(b_+45, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+48);
  CYC(b_+48, b_+49); mem_wr(gb, HL, C);
  CYC(b_+49, b_+52);
  TAIL(partDelete);

subid1:
  CYC(b_+70, b_+72); B = 0x02;
  CALL_L(b_+72, i47_func_777f, b_+75);
  CYC(b_+75, b_+77); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+77, b_+78); A = mem_rd(gb, HL);
  CYC(b_+78, b_+79); alu_or(gb, A);
  CYC(b_+79, b_+81); E = PART_BASE + OBJ_STATE;
  if (F & FZ) { CYCT(b_+81, b_+83); goto L_7599; }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); push_effect(gb, b_+85);
  do { uint16_t jt_ = (i47_jump_table(gb));
    if (jt_ == b_+119) goto subid1_state0;
    if (jt_ == b_+235) goto subid1_state1;
    if (jt_ == b_+256) goto subid1_state2;
    if (jt_ == b_+269) goto subid1_state3;
    if (jt_ == b_+324) goto subid1_state4;
    if (jt_ == b_+346) goto subid1_state5;
    if (jt_ == b_+394) goto subid1_state6;
    if (jt_ == b_+405) goto subid1_state7;
    if (jt_ == b_+446) goto subid1_state8;
    HANDOFF(HL);
  } while (0);
L_7599:
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+106); alu_cp(gb, 0x08);
  if (F & FZ) { RET_TAKEN(b_+106); return; }
  CYC(b_+106, b_+107);
  CYC(b_+107, b_+108); H = D;
  CYC(b_+108, b_+110); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+110, b_+112); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+112, b_+114); L = PART_BASE + OBJ_VISIBLE;
  CYC(b_+114, b_+115); A = mem_rd(gb, HL);
  CYC(b_+115, b_+117); alu_xor(gb, 0x80);
  CYC(b_+117, b_+118); mem_wr(gb, HL, A);
  RET(b_+118); return;
subid1_state0:
  CYC(b_+119, b_+120); E = alu_inc8(gb, E);
  CYC(b_+120, b_+121); A = mem_rd(gb, DE);
  CYC(b_+121, b_+122); push_effect(gb, b_+122);
  do { uint16_t jt_ = (i47_jump_table(gb));
    if (jt_ == b_+132) goto subid1_state0_substate0;
    if (jt_ == b_+166) goto subid1_state0_substate1;
    if (jt_ == b_+203) goto subid1_state0_substate2;
    if (jt_ == b_+217) goto subid1_state0_substate3;
    if (jt_ == b_+234) goto subid1_state0_substate4;
    HANDOFF(HL);
  } while (0);
subid1_state0_substate0:
  CYC(b_+132, b_+133); H = D;
  CYC(b_+133, b_+134); L = E;
  CYC(b_+134, b_+135); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+135, b_+137); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+137, b_+139); A = 0x20;
  CYC(b_+139, b_+140); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+140, b_+142); mem_wr(gb, HL, 0xff);
  CYC(b_+142, b_+144); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+144, b_+146); mem_wr(gb, HL, 0x10);
  CYC(b_+146, b_+148); L = PART_BASE + OBJ_SPEED;
  CYC(b_+148, b_+150); mem_wr(gb, HL, 0x78);
  CYC(b_+150, b_+152); L = PART_BASE + OBJ_YH;
  CYC(b_+152, b_+154); A = 0x18;
  CYC(b_+154, b_+155); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+155, b_+156); L = alu_inc8(gb, L);
  CYC(b_+156, b_+158); mem_wr(gb, HL, 0x78);
  CYC(b_+158, b_+160); L = PART_BASE + OBJ_VAR30;
  CYC(b_+160, b_+161); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+161, b_+163); mem_wr(gb, HL, 0x78);
  CYC(b_+163, b_+166);
  TAIL(objectSetVisible82);
subid1_state0_substate1:
  CYC(b_+166, b_+168); C = 0x0e;
  CALL_C(b_+168, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+171);
  if (F & FZ) { CYCT(b_+171, b_+173); goto L_75eb; }
  CYC(b_+171, b_+173);
  CALL_C(b_+173, s_objectApplySpeed, SYM(objectApplySpeed), b_+176);
  CYC(b_+176, b_+178); E = PART_BASE + OBJ_YH;
  CYC(b_+178, b_+179); A = mem_rd(gb, DE);
  CYC(b_+179, b_+181); alu_sub(gb, 0x18);
  CYC(b_+181, b_+183); E = PART_BASE + OBJ_VAR33;
  CYC(b_+183, b_+184); mem_wr(gb, DE, A);
  RET(b_+184); return;
L_75eb:
  CYC(b_+185, b_+187); L = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+187, b_+188); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+188, b_+189); L = alu_inc8(gb, L);
  CYC(b_+189, b_+191); A = 0x3c;
  CYC(b_+191, b_+192); mem_wr(gb, HL, A);
  CALL_C(b_+192, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+195);
  CYC(b_+195, b_+197); A = 0x6f;
  CALL_C(b_+197, s_playSound, SYM(playSound_b00), b_+200);
  CYC(b_+200, b_+203);
  goto func_776f;
subid1_state0_substate2:
  CALL_C(b_+203, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+206);
  if (!(F & FZ)) { RET_TAKEN(b_+206); return; }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+208); L = E;
  CYC(b_+208, b_+209); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+209, b_+211); L = PART_BASE + OBJ_SPEED;
  CYC(b_+211, b_+213); A = 0x80;
  CYC(b_+213, b_+214); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+214, b_+216); mem_wr(gb, HL, 0xff);
  RET(b_+216); return;
subid1_state0_substate3:
  CALL_C(b_+217, s_objectApplyComponentSpeed, SYM(objectApplyComponentSpeed), b_+220);
  CYC(b_+220, b_+222); E = PART_BASE + OBJ_YH;
  CYC(b_+222, b_+223); A = mem_rd(gb, DE);
  CYC(b_+223, b_+225); alu_cp(gb, 0x18);
  if (!(F & FC)) { RET_TAKEN(b_+225); return; }
  CYC(b_+225, b_+226);
  CYC(b_+226, b_+228); E = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+228, b_+230); A = 0x04;
  CYC(b_+230, b_+231); mem_wr(gb, DE, A);
  CYC(b_+231, b_+234);
  TAIL(objectSetInvisible);
subid1_state0_substate4:
  RET(b_+234); return;
subid1_state1:
  CYC(b_+235, b_+236); H = D;
  CYC(b_+236, b_+237); L = E;
  CYC(b_+237, b_+238); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+238, b_+240); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+240, b_+242); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+242, b_+244); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+244, b_+246); mem_wr(gb, HL, 0x12);
  CYC(b_+246, b_+248); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+248, b_+250); A = 0x00;
  CYC(b_+250, b_+251); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+251, b_+253); mem_wr(gb, HL, 0xfe);
  CALL_C(b_+253, s_objectSetVisible82, SYM(objectSetVisible82), b_+256);
subid1_state2:
  CYC(b_+256, b_+257); H = D;
  CYC(b_+257, b_+259); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+259, b_+260); A = mem_rd(gb, HL);
  CYC(b_+260, b_+262); alu_cp(gb, 0x1e);
  if (!(F & FZ)) { CYCT(b_+262, b_+264); goto subid1_state3; }
  CYC(b_+262, b_+264);
  CYC(b_+264, b_+265); L = E;
  CYC(b_+265, b_+266); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+266, s_objectSetVisible81, SYM(objectSetVisible81), b_+269);
subid1_state3:
  CYC(b_+269, b_+272); A = mem_rd(gb, wFrameCounter);
  CYC(b_+272, b_+274); alu_and(gb, 0x0f);
  CYC(b_+274, b_+276); A = 0xa4;
  if (F & FZ) CALL_C_CC(b_+276, s_playSound, SYM(playSound_b00), b_+279);
  else CYC(b_+276, b_+279);
  CYC(b_+279, b_+281); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+283); A = alu_inc8(gb, A);
  CYC(b_+283, b_+285); alu_and(gb, 0x1f);
  CYC(b_+285, b_+286); mem_wr(gb, DE, A);
  CYC(b_+286, b_+288); alu_and(gb, 0x0f);
  CYC(b_+288, b_+291); SET_HL(b_+557 /* @table_775f */);
  CYC(b_+291, b_+292); i47_add_a_to_hl(gb, b_+292);
  CYC(b_+292, b_+294); E = PART_BASE + OBJ_VAR33;
  CYC(b_+294, b_+295); A = mem_rd(gb, HL);
  CYC(b_+295, b_+296); mem_wr(gb, DE, A);
  CYC(b_+296, b_+299); SET_BC(0xe605);
L_765d:
  CYC(b_+299, b_+301); A = 0x0b;
  CALL_C(b_+301, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+304);
  CYC(b_+304, b_+305); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+305, b_+306); alu_add(gb, B);
  CYC(b_+306, b_+307); B = A;
  CYC(b_+307, b_+309); E = PART_BASE + OBJ_VAR30;
  CYC(b_+309, b_+310); mem_wr(gb, DE, A);
  CYC(b_+310, b_+311); L = alu_inc8(gb, L);
  CYC(b_+311, b_+312); A = mem_rd(gb, HL);
  CYC(b_+312, b_+313); alu_add(gb, C);
  CYC(b_+313, b_+314); C = A;
  CYC(b_+314, b_+315); E = alu_inc8(gb, E);
  CYC(b_+315, b_+316); mem_wr(gb, DE, A);
  CYC(b_+316, b_+318); E = PART_BASE + OBJ_VAR33;
  CYC(b_+318, b_+319); A = mem_rd(gb, DE);
  CYC(b_+319, b_+321); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+321, b_+324);
  TAIL(objectSetPositionInCircleArc);
subid1_state4:
  CYC(b_+324, b_+327); A = mem_rd(gb, wFrameCounter);
  CYC(b_+327, b_+329); alu_and(gb, 0x07);
  CYC(b_+329, b_+331); A = 0xa4;
  if (F & FZ) CALL_C_CC(b_+331, s_playSound, SYM(playSound_b00), b_+334);
  else CYC(b_+331, b_+334);
  CYC(b_+334, b_+336); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+336, b_+337); A = mem_rd(gb, DE);
  CYC(b_+337, b_+338); A = alu_inc8(gb, A);
  CYC(b_+338, b_+340); alu_and(gb, 0x1f);
  CYC(b_+340, b_+341); mem_wr(gb, DE, A);
  CYC(b_+341, b_+344); SET_BC(0xe009);
  CYC(b_+344, b_+346);
  goto L_765d;
subid1_state5:
  CALL_C(b_+346, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+349);
  if (!(F & FZ)) { CYCT(b_+349, b_+351); goto L_76a3; }
  CYC(b_+349, b_+351);
  CYC(b_+351, b_+353); mem_wr(gb, HL, 0x02);
  CYC(b_+353, b_+355); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+355, b_+356); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+356, b_+357); A = mem_rd(gb, HL);
  CYC(b_+357, b_+359); alu_cp(gb, 0x15);
  if (F & FZ) { CYCT(b_+359, b_+361); goto L_76a6; }
  CYC(b_+359, b_+361);
  CYC(b_+361, b_+362); C = A;
  CYC(b_+362, b_+364); B = 0x5a;
  CYC(b_+364, b_+366); A = 0x03;
  CALL_C(b_+366, s_objectSetComponentSpeedByScaledVelocity, SYM(objectSetComponentSpeedByScaledVelocity), b_+369);
L_76a3:
  CYC(b_+369, b_+372);
  TAIL(objectApplyComponentSpeed);
L_76a6:
  CYC(b_+372, b_+373); L = E;
  CYC(b_+373, b_+374); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+374, b_+376); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+376, b_+378); A = 0x3c;
  CYC(b_+378, b_+379); mem_wr(gb, HL, A);
  CYC(b_+379, b_+381); L = PART_BASE + OBJ_DAMAGE;
  CYC(b_+381, b_+383); mem_wr(gb, HL, 0xfc);
  CALL_C(b_+383, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+386);
  CALL_L(b_+386, i47_func_776f, b_+389);
  CYC(b_+389, b_+391); A = 0x6f;
  CYC(b_+391, b_+394);
  TAIL(playSound_b00);
subid1_state6:
  CALL_C(b_+394, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+397);
  if (!(F & FZ)) { RET_TAKEN(b_+397); return; }
  CYC(b_+397, b_+398);
  CYC(b_+398, b_+399); L = E;
  CYC(b_+399, b_+400); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+400, b_+402); L = PART_BASE + OBJ_SPEED;
  CYC(b_+402, b_+404); mem_wr(gb, HL, 0x1e);
  RET(b_+404); return;
subid1_state7:
  CYC(b_+405, b_+406); H = D;
  CYC(b_+406, b_+408); L = PART_BASE + OBJ_VAR30;
  CYC(b_+408, b_+409); B = mem_rd(gb, HL);
  CYC(b_+409, b_+410); L = alu_inc8(gb, L);
  CYC(b_+410, b_+411); C = mem_rd(gb, HL);
  CYC(b_+411, b_+413); L = PART_BASE + OBJ_YH;
  CYC(b_+413, b_+414); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+414, b_+416); mem_wr(gb, hFF8F, A);
  CYC(b_+416, b_+417); L = alu_inc8(gb, L);
  CYC(b_+417, b_+418); A = mem_rd(gb, HL);
  CYC(b_+418, b_+420); mem_wr(gb, hFF8E, A);
  CYC(b_+420, b_+421); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+421, b_+423); goto L_76e7; }
  CYC(b_+421, b_+423);
  CYC(b_+423, b_+425); A = mem_rd(gb, hFF8F);
  CYC(b_+425, b_+426); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+426, b_+428); goto L_76e7; }
  CYC(b_+426, b_+428);
  CYC(b_+428, b_+429); L = E;
  CYC(b_+429, b_+430); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+430, b_+432); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+432, b_+434); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+434, b_+437);
  TAIL(objectSetInvisible);
L_76e7:
  CALL_C(b_+437, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+440);
  CYC(b_+440, b_+442); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+442, b_+443); mem_wr(gb, DE, A);
  CYC(b_+443, b_+446);
  TAIL(objectApplySpeed);
subid1_state8:
  CYC(b_+446, b_+448); A = 0x04;
  CALL_C(b_+448, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+451);
  CYC(b_+451, b_+452); A = mem_rd(gb, HL);
  CYC(b_+452, b_+454); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { RET_TAKEN(b_+454); return; }
  CYC(b_+454, b_+455);
  CYC(b_+455, b_+457); E = PART_BASE + OBJ_STATE;
  CYC(b_+457, b_+459); A = 0x01;
  CYC(b_+459, b_+460); mem_wr(gb, DE, A);
  RET(b_+460); return;
subid2:
  CYC(b_+461, b_+462); A = mem_rd(gb, DE);
  CYC(b_+462, b_+463); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+463, b_+465); goto func_7726; }
  CYC(b_+463, b_+465);
  CYC(b_+465, b_+467); B = 0x47;
  CALL_L(b_+467, i47_func_777f, b_+470);
  CALL_L(b_+470, i47_func_778b, b_+473);
  CYC(b_+473, b_+474); A = E;
  CYC(b_+474, b_+475); alu_add(gb, A);
  CYC(b_+475, b_+476); alu_add(gb, E);
  CYC(b_+476, b_+477); alu_add(gb, B);
  CYC(b_+477, b_+479); E = PART_BASE + OBJ_YH;
  CYC(b_+479, b_+480); mem_wr(gb, DE, A);
  CYC(b_+480, b_+481); A = L;
  CYC(b_+481, b_+482); alu_add(gb, A);
  CYC(b_+482, b_+483); alu_add(gb, L);
  CYC(b_+483, b_+484); alu_add(gb, C);
  CYC(b_+484, b_+486); E = PART_BASE + OBJ_XH;
  CYC(b_+486, b_+487); mem_wr(gb, DE, A);
func_7719:
  CYC(b_+487, b_+489); A = 0x1a;
  CALL_C(b_+489, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+492);
  CYC(b_+492, b_+494); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+494, b_+497); TAIL(objectSetVisible82); }
  CYC(b_+494, b_+497);
  CYC(b_+497, b_+500);
  TAIL(objectSetInvisible);
func_7726:
  CYC(b_+500, b_+501); A = alu_inc8(gb, A);
  CYC(b_+501, b_+502); mem_wr(gb, DE, A);
  CALL_C(b_+502, s_partSetAnimation, SYM(partSetAnimation), b_+505);
  CYC(b_+505, b_+507);
  goto func_7719;
subid3:
  CYC(b_+507, b_+508); A = mem_rd(gb, DE);
  CYC(b_+508, b_+509); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+509, b_+511); goto func_7726; }
  CYC(b_+509, b_+511);
  CYC(b_+511, b_+513); B = 0x47;
  CALL_L(b_+513, i47_func_777f, b_+516);
  CALL_L(b_+516, i47_func_778b, b_+519);
  CYC(b_+519, b_+520); A = E;
  CYC(b_+520, b_+521); alu_add(gb, A);
  CYC(b_+521, b_+522); alu_add(gb, B);
  CYC(b_+522, b_+524); E = PART_BASE + OBJ_YH;
  CYC(b_+524, b_+525); mem_wr(gb, DE, A);
  CYC(b_+525, b_+526); A = L;
  CYC(b_+526, b_+527); alu_add(gb, A);
  CYC(b_+527, b_+528); alu_add(gb, C);
  CYC(b_+528, b_+530); E = PART_BASE + OBJ_XH;
  CYC(b_+530, b_+531); mem_wr(gb, DE, A);
  CYC(b_+531, b_+533);
  goto func_7719;
subid4:
  CYC(b_+533, b_+534); A = mem_rd(gb, DE);
  CYC(b_+534, b_+535); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+535, b_+537); goto func_7726; }
  CYC(b_+535, b_+537);
  CYC(b_+537, b_+539); B = 0x47;
  CALL_L(b_+539, i47_func_777f, b_+542);
  CALL_L(b_+542, i47_func_778b, b_+545);
  CYC(b_+545, b_+546); A = E;
  CYC(b_+546, b_+547); alu_add(gb, B);
  CYC(b_+547, b_+549); E = PART_BASE + OBJ_YH;
  CYC(b_+549, b_+550); mem_wr(gb, DE, A);
  CYC(b_+550, b_+551); A = L;
  CYC(b_+551, b_+552); alu_add(gb, C);
  CYC(b_+552, b_+554); E = PART_BASE + OBJ_XH;
  CYC(b_+554, b_+555); mem_wr(gb, DE, A);
  CYC(b_+555, b_+557);
  goto func_7719;

func_776f:
  CALL_C(b_+573, s_getFreePartSlot, SYM(getFreePartSlot), b_+576);
  if (!(F & FZ)) { RET_TAKEN(b_+576); return; }
  CYC(b_+576, b_+577);
  CYC(b_+577, b_+579); mem_wr(gb, HL, 0x48);
  CYC(b_+579, b_+580); L = alu_inc8(gb, L);
  CYC(b_+580, b_+582); mem_wr(gb, HL, 0x02);
  CYC(b_+582, b_+584); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+584, b_+586); A = 0xc0;
  CYC(b_+586, b_+587); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+587, b_+588); mem_wr(gb, HL, D);
  RET(b_+588); return;
}

