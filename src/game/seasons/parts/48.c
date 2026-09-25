#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/48.s.
// PART_48

static uint16_t i48_jump_table(GB *gb) {
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

static void i48_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// PART_48
void s_partCode48_hook(GB *gb) {
  BASE(partCode48);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = PART_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (i48_jump_table(gb));
    if (jt_ == b_+18) goto subid0;
    if (jt_ == b_+49) goto subid1;
    if (jt_ == b_+172) goto subid2;
    if (jt_ == b_+208) goto subid3;
    if (jt_ == b_+284) goto subid4;
    if (jt_ == b_+316) goto subid5;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+20, b_+22); goto L_77ce; }
  CYC(b_+20, b_+22);
  CALL_C(b_+22, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+25);
  if (F & FZ) { CYCT(b_+25, b_+28); TAIL(partDelete); }
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CALL_C(b_+32, s_getFreePartSlot, SYM(getFreePartSlot), b_+35);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x48);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+40); return;
L_77ce:
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+43); L = E;
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+44, b_+46); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x96);
  RET(b_+48); return;
subid1:
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); push_effect(gb, b_+51);
  do { uint16_t jt_ = (i48_jump_table(gb));
    if (jt_ == b_+57) goto subid1_state0;
    if (jt_ == b_+105) goto subid1_state1;
    if (jt_ == b_+140) goto subid1_state2;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+59); L = E;
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+60, b_+62); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+62, b_+64); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+64, b_+66); A = mem_rd(gb, hCameraY);
  CYC(b_+66, b_+67); B = A;
  CYC(b_+67, b_+69); A = mem_rd(gb, hCameraX);
  CYC(b_+69, b_+70); C = A;
  CALL_C(b_+70, s_getRandomNumber, SYM(getRandomNumber), b_+73);
  CYC(b_+73, b_+74); E = A;
  CYC(b_+74, b_+76); alu_and(gb, 0x07);
  CYC(b_+76, b_+78); A = alu_swap(gb, A);
  CYC(b_+78, b_+80); alu_add(gb, 0x28);
  CYC(b_+80, b_+81); alu_add(gb, C);
  CYC(b_+81, b_+83); L = PART_BASE + OBJ_XH;
  CYC(b_+83, b_+84); mem_wr(gb, HL, A);
  CYC(b_+84, b_+85); A = E;
  CYC(b_+85, b_+87); alu_and(gb, 0x70);
  CYC(b_+87, b_+89); alu_add(gb, 0x08);
  CYC(b_+89, b_+90); E = A;
  CYC(b_+90, b_+91); alu_add(gb, B);
  CYC(b_+91, b_+93); L = PART_BASE + OBJ_YH;
  CYC(b_+93, b_+94); mem_wr(gb, HL, A);
  CYC(b_+94, b_+95); A = E;
  CYC(b_+95, b_+96); alu_cpl(gb);
  CYC(b_+96, b_+97); A = alu_inc8(gb, A);
  CYC(b_+97, b_+99); alu_sub(gb, 0x07);
  CYC(b_+99, b_+101); L = PART_BASE + OBJ_ZH;
  CYC(b_+101, b_+102); mem_wr(gb, HL, A);
  CYC(b_+102, b_+105);
  TAIL(objectSetVisiblec1);
subid1_state1:
  CYC(b_+105, b_+107); C = 0x20;
  CALL_C(b_+107, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+110);
  if (!(F & FZ)) { CYCT(b_+110, b_+112); goto L_7844; }
  CYC(b_+110, b_+112);
  CALL_C(b_+112, s_objectReplaceWithAnimationIfOnHazard, SYM(objectReplaceWithAnimationIfOnHazard), b_+115);
  if (F & FC) { CYCT(b_+115, b_+118); TAIL(partDelete); }
  CYC(b_+115, b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = PART_BASE + OBJ_STATE;
  CYC(b_+121, b_+122); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+122, b_+124); L = PART_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+124, b_+126); A = 0x0b;
  CYC(b_+126, b_+127); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+127, b_+128); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x02);
  CYC(b_+130, b_+132); A = 0xa5;
  CALL_C(b_+132, s_playSound, SYM(playSound_b00), b_+135);
  CYC(b_+135, b_+137); A = 0x01;
  CALL_C(b_+137, s_partSetAnimation, SYM(partSetAnimation), b_+140);
subid1_state2:
  CYC(b_+140, b_+142); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+145, b_+148); TAIL(partDelete); }
  CYC(b_+145, b_+148);
  CYC(b_+148, b_+151); SET_HL(b_+162 /* @table_7847 */);
  CYC(b_+151, b_+152); i48_add_a_to_hl(gb, b_+152);
  CYC(b_+152, b_+154); E = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+154, b_+155); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+155, b_+156); mem_wr(gb, DE, A);
  CYC(b_+156, b_+157); E = alu_inc8(gb, E);
  CYC(b_+157, b_+158); A = mem_rd(gb, HL);
  CYC(b_+158, b_+159); mem_wr(gb, DE, A);
L_7844:
  CYC(b_+159, b_+162);
  TAIL(partAnimate);

subid2:
  CYC(b_+172, b_+174); B = 0x06;
  CALL_C(b_+174, s_checkBPartSlotsAvailable, SYM(checkBPartSlotsAvailable), b_+177);
  if (!(F & FZ)) { RET_TAKEN(b_+177); return; }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+180); A = 0x00;
  CALL_C(b_+180, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+183);
  CALL_C(b_+183, s_objectTakePosition, SYM(objectTakePosition), b_+186);
  CYC(b_+186, b_+188); B = 0x06;
L_7861:
  CALL_C(b_+188, s_getFreePartSlot, SYM(getFreePartSlot), b_+191);
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x48);
  CYC(b_+193, b_+194); L = alu_inc8(gb, L);
  CYC(b_+194, b_+196); mem_wr(gb, HL, 0x03);
  CYC(b_+196, b_+198); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+198, b_+199); mem_wr(gb, HL, B);
  CALL_C(b_+199, s_objectCopyPosition, SYM(objectCopyPosition), b_+202);
  CYC(b_+202, b_+203); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+203, b_+205); goto L_7861; }
  CYC(b_+203, b_+205);
  CYC(b_+205, b_+208);
  TAIL(partDelete);
subid3:
  CYC(b_+208, b_+209); A = mem_rd(gb, DE);
  CYC(b_+209, b_+210); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+210, b_+212); goto L_7884; }
  CYC(b_+210, b_+212);
  CYC(b_+212, b_+214); C = 0x18;
  CALL_C(b_+214, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+217);
  if (F & FZ) { CYCT(b_+217, b_+220); TAIL(partDelete); }
  CYC(b_+217, b_+220);
  CYC(b_+220, b_+223);
  TAIL(objectApplySpeed);
L_7884:
  CYC(b_+223, b_+224); H = D;
  CYC(b_+224, b_+225); L = E;
  CYC(b_+225, b_+226); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+226, b_+228); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+228, b_+230); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+230, b_+232); L = PART_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+232, b_+234); A = 0x0b;
  CYC(b_+234, b_+235); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+235, b_+236); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+236, b_+238); A = 0x02;
  CYC(b_+238, b_+239); mem_wr(gb, HL, A);
  CYC(b_+239, b_+241); L = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+241, b_+242); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+242, b_+243); mem_wr(gb, HL, A);
  CYC(b_+243, b_+245); L = PART_BASE + OBJ_DAMAGE;
  CYC(b_+245, b_+247); mem_wr(gb, HL, 0xfc);
  CYC(b_+247, b_+249); L = PART_BASE + OBJ_SPEED;
  CYC(b_+249, b_+251); mem_wr(gb, HL, 0x50);
  CYC(b_+251, b_+253); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+253, b_+255); A = 0x20;
  CYC(b_+255, b_+256); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+256, b_+258); mem_wr(gb, HL, 0xff);
  CYC(b_+258, b_+260); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+260, b_+261); A = mem_rd(gb, HL);
  CYC(b_+261, b_+262); A = alu_dec8(gb, A);
  CYC(b_+262, b_+265); SET_BC(b_+278 /* @table_78bb */);
  CALL_C(b_+265, s_addAToBc, SYM(addAToBc), b_+268);
  CYC(b_+268, b_+269); A = mem_rd(gb, BC);
  CYC(b_+269, b_+270); mem_wr(gb, HL, A);
  CYC(b_+270, b_+272); A = 0x02;
  CALL_C(b_+272, s_partSetAnimation, SYM(partSetAnimation), b_+275);
  CYC(b_+275, b_+278);
  TAIL(objectSetVisible82);

subid4:
  CYC(b_+284, b_+285); A = mem_rd(gb, DE);
  CYC(b_+285, b_+286); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+286, b_+288); goto L_78d9; }
  CYC(b_+286, b_+288);
  CALL_C(b_+288, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+291);
  if (F & FZ) { CYCT(b_+291, b_+294); TAIL(partDelete); }
  CYC(b_+291, b_+294);
  CYC(b_+294, b_+295); A = mem_rd(gb, HL);
  CYC(b_+295, b_+297); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+297); return; }
  CYC(b_+297, b_+298);
  CALL_C(b_+298, s_getFreePartSlot, SYM(getFreePartSlot), b_+301);
  if (!(F & FZ)) { RET_TAKEN(b_+301); return; }
  CYC(b_+301, b_+302);
  CYC(b_+302, b_+304); mem_wr(gb, HL, 0x48);
  CYC(b_+304, b_+305); L = alu_inc8(gb, L);
  CYC(b_+305, b_+307); mem_wr(gb, HL, 0x05);
  RET(b_+307); return;
L_78d9:
  CYC(b_+308, b_+309); H = D;
  CYC(b_+309, b_+310); L = E;
  CYC(b_+310, b_+311); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+311, b_+313); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+313, b_+315); mem_wr(gb, HL, 0x61);
  RET(b_+315); return;
subid5:
  CYC(b_+316, b_+317); A = mem_rd(gb, DE);
  CYC(b_+317, b_+318); push_effect(gb, b_+318);
  do { uint16_t jt_ = (i48_jump_table(gb));
    if (jt_ == b_+324) goto subid5_state0;
    if (jt_ == b_+348) goto subid5_state1;
    if (jt_ == b_+392) goto subid5_state2;
    HANDOFF(HL);
  } while (0);
subid5_state0:
  CYC(b_+324, b_+325); H = D;
  CYC(b_+325, b_+326); L = E;
  CYC(b_+326, b_+327); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+327, b_+329); L = PART_BASE + OBJ_YH;
  CYC(b_+329, b_+331); mem_wr(gb, HL, 0x28);
  CALL_C(b_+331, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+334);
  CYC(b_+334, b_+336); alu_and(gb, 0x7f);
  CYC(b_+336, b_+338); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(b_+338, b_+340); goto L_78fb; }
  CYC(b_+338, b_+340);
  CYC(b_+340, b_+342); alu_add(gb, 0x20);
L_78fb:
  CYC(b_+342, b_+344); E = PART_BASE + OBJ_XH;
  CYC(b_+344, b_+345); mem_wr(gb, DE, A);
  CYC(b_+345, b_+348);
  TAIL(objectSetVisible82);
subid5_state1:
  CYC(b_+348, b_+349); H = D;
  CYC(b_+349, b_+351); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+351, b_+353); E = PART_BASE + OBJ_Y;
  CALL_C(b_+353, s_add16BitRefs, SYM(add16BitRefs), b_+356);
  CYC(b_+356, b_+358); alu_cp(gb, 0xa0);
  if (!(F & FC)) { CYCT(b_+358, b_+360); goto L_7917; }
  CYC(b_+358, b_+360);
  CYC(b_+360, b_+361); L = alu_dec8(gb, L);
  CYC(b_+361, b_+362); A = mem_rd(gb, HL);
  CYC(b_+362, b_+364); alu_add(gb, 0x10);
  CYC(b_+364, b_+365); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+365, b_+366); A = mem_rd(gb, HL);
  CYC(b_+366, b_+368); alu_adc(gb, 0x00);
  CYC(b_+368, b_+369); mem_wr(gb, HL, A);
  RET(b_+369); return;
L_7917:
  CYC(b_+370, b_+371); H = D;
  CYC(b_+371, b_+373); L = PART_BASE + OBJ_STATE;
  CYC(b_+373, b_+374); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+374, b_+376); L = PART_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+376, b_+378); A = 0x0b;
  CYC(b_+378, b_+379); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+379, b_+380); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+380, b_+382); mem_wr(gb, HL, 0x02);
  CYC(b_+382, b_+384); A = 0xa5;
  CALL_C(b_+384, s_playSound, SYM(playSound_b00), b_+387);
  CYC(b_+387, b_+389); A = 0x01;
  CALL_C(b_+389, s_partSetAnimation, SYM(partSetAnimation), b_+392);
subid5_state2:
  CYC(b_+392, b_+394); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+394, b_+395); A = mem_rd(gb, DE);
  CYC(b_+395, b_+397); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+397, b_+400); TAIL(partDelete); }
  CYC(b_+397, b_+400);
  CYC(b_+400, b_+403);
  TAIL(partAnimate);
}

