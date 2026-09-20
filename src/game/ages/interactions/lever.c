#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode61), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode61), (from), (to), true)
#define LEVER_VAR30 OBJ_USE_TEXT_ID
#define LEVER_VAR31 OBJ_PRESSED_A_BUTTON

static uint16_t lever_jump_table(GB *gb) {
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

static void lever_set_speed_and_angle(GB *gb) {
  BASE(interactionCode61);
  CYC(b_+272, b_+274); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+274, b_+275); mem_wr(gb, HL, B);
  CYC(b_+275, b_+277); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+277, b_+278); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+278, b_+280); A = alu_swap(gb, A);
  CYC(b_+280, b_+282); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+282, b_+283); mem_wr(gb, HL, A);
  CYC(b_+283, b_+284); ret_effect(gb);
}

static void lever_negative_comparison(GB *gb) {
  BASE(interactionCode61);
  CYC(b_+403, b_+405); L = INTERACTION_BASE + LEVER_VAR30;
  CYC(b_+405, b_+406); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+406, b_+408); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+408, b_+409); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+409, b_+410); ret_effect(gb); return; }
  CYC(b_+409, b_+410);
  CYC(b_+410, b_+411); mem_wr(gb, HL, A);
  CYC(b_+411, b_+412); ret_effect(gb);
}

static void lever_positive_comparison(GB *gb) {
  BASE(interactionCode61);
  CYC(b_+420, b_+422); L = INTERACTION_BASE + LEVER_VAR30;
  CYC(b_+422, b_+423); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+423, b_+424); B = A;
  CYC(b_+424, b_+426); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+426, b_+427); A = mem_rd(gb, HL);
  CYC(b_+427, b_+428); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+428, b_+429); ret_effect(gb); return; }
  CYC(b_+428, b_+429);
  CYC(b_+429, b_+430); mem_wr(gb, HL, B);
  CYC(b_+430, b_+431); ret_effect(gb);
}

static void lever_check_fully_extended(GB *gb) {
  BASE(interactionCode61);
  CYC(b_+391, b_+393); E = INTERACTION_BASE + LEVER_VAR31;
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+395); H = D;
  CYC(b_+395, b_+397); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+397, b_+399); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+399, b_+401);
    lever_positive_comparison(gb);
    return;
  }
  CYC(b_+399, b_+401);
  CYC(b_+401, b_+402); alu_cpl(gb);
  CYC(b_+402, b_+403); A = alu_inc8(gb, A);
  lever_negative_comparison(gb);
}

static void lever_check_fully_retracted(GB *gb) {
  BASE(interactionCode61);
  CYC(b_+412, b_+413); alu_xor(gb, A);
  CYC(b_+413, b_+414); H = D;
  CYC(b_+414, b_+416); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+416, b_+418); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+418, b_+420);
    lever_negative_comparison(gb);
    return;
  }
  CYC(b_+418, b_+420);
  lever_positive_comparison(gb);
}

static void lever_update_pull_offset(GB *gb, uint16_t sp0_) {
  BASE(interactionCode61);
  if (!(F & FC)) {
    CYCT(b_+431, b_+433);
  } else {
    CYC(b_+431, b_+433);
    CYC(b_+433, b_+434); alu_cpl(gb);
    CYC(b_+434, b_+435); A = alu_inc8(gb, A);
  }
  CYC(b_+435, b_+436); H = D;
  CYC(b_+436, b_+438); L = INTERACTION_BASE + LEVER_VAR31;
  CYC(b_+438, b_+439); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYC(b_+439, b_+441);
    CYC(b_+441, b_+442); H = A;
    CYC(b_+442, b_+443); push_effect(gb, HL);
    CYC(b_+443, b_+445); A = 0x6c;
    CALL_C(b_+445, playSound_b00_hook, SYM(playSound_b00), b_+448);
    CYC(b_+448, b_+449); SET_HL(pop_effect(gb));
    CYC(b_+449, b_+450); A = H;
    CYC(b_+450, b_+452); alu_or(gb, 0x80);
    CYC(b_+452, b_+453); H = D;
  } else {
    CYCT(b_+439, b_+441);
  }
  CYC(b_+453, b_+454); B = A;
  CYC(b_+454, b_+455); L = alu_inc8(gb, L);
  CYC(b_+455, b_+456); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+456, b_+457); H = mem_rd(gb, HL);
  CYC(b_+457, b_+458); L = A;
  CYC(b_+458, b_+459); A = mem_rd(gb, HL);
  CYC(b_+459, b_+460); mem_wr(gb, HL, B);
  CYC(b_+460, SYM(interactionCode62)); ret_effect(gb);
}

static void lever_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode61);
  CALL_C(b_+133, objectPushLinkAwayOnCollision_hook, SYM(objectPushLinkAwayOnCollision), b_+136);
  CALL_C(b_+136, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+139);
  CYC(b_+139, b_+141); alu_add(gb, 0x14);
  CYC(b_+141, b_+143); alu_and(gb, 0x18);
  CYC(b_+143, b_+145); A = alu_swap(gb, A);
  CYC(b_+145, b_+146); alu_rlca(gb);
  CYC(b_+146, b_+147); C = A;
  CYC(b_+147, b_+149); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+151); alu_add(gb, A);
  CYC(b_+151, b_+152); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+152, b_+153); ret_effect(gb); return; }
  CYC(b_+152, b_+153);
  CYC(b_+153, b_+156); A = W8(w1Link_direction);
  CYC(b_+156, b_+157); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+157, b_+158); ret_effect(gb); return; }
  CYC(b_+157, b_+158);
  CYC(b_+158, b_+161); objectAddToGrabbableObjectBuffer_hook(gb);
}

static void lever_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode61);
  CYC(b_+161, b_+162); E = alu_inc8(gb, E);
  CYC(b_+162, b_+163); A = mem_rd(gb, DE);
  CYC(b_+163, b_+164); push_effect(gb, b_+164);
  do { uint16_t jt_ = (lever_jump_table(gb));
    if (jt_ == b_+172) { goto substate0; }
    else if (jt_ == b_+206) { goto substate1; }
    else if (jt_ == b_+262) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+172, b_+173); H = D;
  CYC(b_+173, b_+174); L = E;
  CYC(b_+174, b_+175); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+175, b_+177); A = 0x80;
  CYC(b_+177, b_+180); W8(wLinkGrabState2) = A;
  CYC(b_+180, b_+182); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+182, b_+183); A = mem_rd(gb, HL);
  CYC(b_+183, b_+186); W8(w1Link_xh) = A;
  CYC(b_+186, b_+188); L = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+188, b_+189); A = mem_rd(gb, HL);
  CYC(b_+189, b_+191); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+191, b_+192); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+192, b_+195); W8(w1Link_yh) = A;
  CYC(b_+195, b_+196); alu_xor(gb, A);
  CYC(b_+196, b_+197); L = alu_dec8(gb, L);
  CYC(b_+197, b_+198); mem_wr(gb, HL, A);
  CYC(b_+198, b_+201); W8(w1Link_y) = A;
  CYC(b_+201, b_+203); B = 0x0a;
  CYC(b_+203, b_+204); A = alu_inc8(gb, A);
  CYC(b_+204, b_+206);
  lever_set_speed_and_angle(gb);
  return;

substate1:
  CYC(b_+206, b_+209); A = W8(w1ParentItem2_animParameter);
  CYC(b_+209, b_+210); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+210, b_+212); goto pull; }
  CYC(b_+210, b_+212);
  CYC(b_+212, b_+214); E = INTERACTION_BASE + OBJ_VAR35;
  CYC(b_+214, b_+215); mem_wr(gb, DE, A);
  CYC(b_+215, b_+216); ret_effect(gb);
  return;

pull:
  CYC(b_+216, b_+219); push_effect(gb, b_+219);
  lever_check_fully_extended(gb);
  if (!(F & FC)) { CYCT(b_+219, b_+220); ret_effect(gb); return; }
  CYC(b_+219, b_+220);
  CYC(b_+220, b_+222); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+222, b_+223); C = mem_rd(gb, HL);
  CYC(b_+223, b_+225); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+225, b_+226); B = mem_rd(gb, HL);
  CALL_C(b_+226, updateLinkPositionGivenVelocity_hook, SYM(updateLinkPositionGivenVelocity), b_+229);
  CYC(b_+229, b_+232); A = W8(w1Link_yh);
  CYC(b_+232, b_+233); H = D;
  CYC(b_+233, b_+235); L = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+235, b_+236); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+236, b_+238); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+238, b_+239); mem_wr(gb, HL, A);
  CYC(b_+239, b_+241); L = INTERACTION_BASE + LEVER_VAR30;
  CYC(b_+241, b_+242); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+242, b_+245); push_effect(gb, b_+245);
  lever_update_pull_offset(gb, sp0_);
  CYC(b_+245, b_+246); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+246, b_+247); ret_effect(gb); return; }
  CYC(b_+246, b_+247);
  CYC(b_+247, b_+248); H = D;
  CYC(b_+248, b_+250); L = INTERACTION_BASE + OBJ_VAR35;
  CYC(b_+250, b_+252); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+252, b_+253); ret_effect(gb); return; }
  CYC(b_+252, b_+253);
  CYC(b_+253, b_+254); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+254, b_+256); alu_bit(gb, 7, B);
  if (!(F & FZ)) { CYCT(b_+256, b_+257); ret_effect(gb); return; }
  CYC(b_+256, b_+257);
  CYC(b_+257, b_+259); A = 0x71;
  CYC(b_+259, b_+262); playSound_b00_hook(gb);
  return;

substate2:
  CALL_C(b_+262, interactionIncState_hook, SYM(interactionIncState), b_+265);
  CYC(b_+265, b_+267); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+267, b_+269); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(b_+269, b_+271); B = 0x0a;
  CYC(b_+271, b_+272); alu_xor(gb, A);
  lever_set_speed_and_angle(gb);
}

static void lever_state3(GB *gb, uint16_t sp0_) {
  BASE(interactionCode61);
  CALL_C(b_+284, objectApplySpeed_hook, SYM(objectApplySpeed), b_+287);
  CYC(b_+287, b_+289); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+289, b_+290); A = mem_rd(gb, DE);
  CYC(b_+290, b_+291); B = A;
  CYC(b_+291, b_+293); E = INTERACTION_BASE + LEVER_VAR30;
  CYC(b_+293, b_+294); A = mem_rd(gb, DE);
  CYC(b_+294, b_+295); alu_sub(gb, B);
  CYC(b_+295, b_+298); push_effect(gb, b_+298);
  lever_update_pull_offset(gb, sp0_);
  CYC(b_+298, b_+301); push_effect(gb, b_+301);
  lever_check_fully_retracted(gb);
  if (F & FC) { CYCT(b_+301, b_+303); goto make_grabbable; }
  CYC(b_+301, b_+303);
  CYC(b_+303, b_+305); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+305, b_+307); mem_wr(gb, HL, 1);
  CYC(b_+307, b_+309); B = 0x0a;
  CYC(b_+309, b_+311); A = 1;
  CYC(b_+311, b_+314); push_effect(gb, b_+314);
  lever_set_speed_and_angle(gb);

make_grabbable:
  CYC(b_+314, b_+317);
  lever_state1(gb, sp0_);
}

static void lever_connection(GB *gb, uint16_t sp0_) {
  BASE(interactionCode61);
  CYC(b_+317, b_+318); A = mem_rd(gb, DE);
  CYC(b_+318, b_+319); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+319, b_+321); goto state1; }
  CYC(b_+319, b_+321);
  CALL_C(b_+321, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+324);
  CALL_C(b_+324, interactionIncState_hook, SYM(interactionIncState), b_+327);
  CALL_C(b_+327, objectSetVisible83_hook, SYM(objectSetVisible83), b_+330);
  CYC(b_+330, b_+332); A = 0x0d;
  CALL_C(b_+332, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+335);
  CYC(b_+335, b_+336); E = L;
  CYC(b_+336, b_+337); A = mem_rd(gb, HL);
  CYC(b_+337, b_+338); mem_wr(gb, DE, A);

state1:
  CYC(b_+338, b_+340); A = 2;
  CALL_C(b_+340, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+343);
  CYC(b_+343, b_+344); A = mem_rd(gb, HL);
  CYC(b_+344, b_+345); alu_add(gb, A);
  CYC(b_+345, b_+346); alu_add(gb, A);
  CYC(b_+346, b_+347); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+347, b_+348); B = A;
  CYC(b_+348, b_+350); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+350, b_+351); A = mem_rd(gb, HL);
  CYC(b_+351, b_+353); L = INTERACTION_BASE + LEVER_VAR30;
  CYC(b_+353, b_+354); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(b_+354, b_+356);
  } else {
    CYC(b_+354, b_+356);
    CYC(b_+356, b_+357); alu_cpl(gb);
    CYC(b_+357, b_+358); A = alu_inc8(gb, A);
  }
  CYC(b_+358, b_+360); A = alu_swap(gb, A);
  CYC(b_+360, b_+362); alu_and(gb, 7);
  CYC(b_+362, b_+363); push_effect(gb, AF);
  CYC(b_+363, b_+364); alu_add(gb, B);
  CYC(b_+364, b_+367); SET_BC(b_+381);
  CALL_C(b_+367, addAToBc_hook, 0x006d, b_+370);
  CYC(b_+370, b_+371); A = mem_rd(gb, BC);
  CYC(b_+371, b_+372); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+372, b_+374); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+374, b_+375); mem_wr(gb, DE, A);
  CYC(b_+375, b_+376); SET_AF(pop_effect(gb));
  CYC(b_+376, b_+378); alu_add(gb, 2);
  CYC(b_+378, b_+381); interactionSetAnimation_hook(gb);
}

void interactionCode61_hook(GB *gb) {
  BASE(interactionCode61);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  CYC(b_+4, b_+6); E = INTERACTION_BASE + OBJ_STATE;
  if (F & FC) {
    CYCT(b_+6, b_+9);
    lever_connection(gb, sp0_);
    return;
  }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (lever_jump_table(gb));
    if (jt_ == b_+19) { goto state0; }
    else if (jt_ == b_+133) { lever_state1(gb, sp0_); return; }
    else if (jt_ == b_+161) { lever_state2(gb, sp0_); return; }
    else if (jt_ == b_+284) { lever_state3(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+19, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto initialized; }
  CYC(b_+26, b_+28);
  CALL_C(b_+28, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+32); ret_effect(gb); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x61);
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+36, b_+37); E = L;
  CYC(b_+37, b_+39); A = INTERACTION_BASE;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+41, b_+42); E = alu_inc8(gb, E);
  CYC(b_+42, b_+43); mem_wr(gb, HL, D);
  CYC(b_+43, b_+44); A = H;
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+46); alu_cp(gb, D);
  if (!(F & FC)) { CYCT(b_+46, b_+48); goto child_higher; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+50, b_+51); E = L;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+53, b_+55); A = 0x80;
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+57, b_+60); objectCopyPosition_hook(gb);
  return;

child_higher:
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x80);

initialized:
  CALL_C(b_+64, interactionIncState_hook, SYM(interactionIncState), b_+67);
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+69, b_+71); mem_wr(gb, HL, 5);
  CYC(b_+71, b_+72); L = alu_inc8(gb, L);
  CYC(b_+72, b_+74); mem_wr(gb, HL, 1);
  CYC(b_+74, b_+76); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); E = INTERACTION_BASE + LEVER_VAR30;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+82); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+85); alu_and(gb, 0x30);
  CYC(b_+85, b_+87); A = alu_swap(gb, A);
  CYC(b_+87, b_+90); SET_BC(b_+129);
  CALL_C(b_+90, addAToBc_hook, 0x006d, b_+93);
  CYC(b_+93, b_+94); E = alu_inc8(gb, E);
  CYC(b_+94, b_+95); A = mem_rd(gb, BC);
  CYC(b_+95, b_+96); mem_wr(gb, DE, A);
  CYC(b_+96, b_+99); SET_BC(wLever1PullDistance);
  CYC(b_+99, b_+101); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+101, b_+103);
  } else {
    CYC(b_+101, b_+103);
    CYC(b_+103, b_+104); SET_BC(BC + 1);
  }
  CYC(b_+104, b_+105); E = alu_inc8(gb, E);
  CYC(b_+105, b_+106); A = C;
  CYC(b_+106, b_+107); mem_wr(gb, DE, A);
  CYC(b_+107, b_+108); E = alu_inc8(gb, E);
  CYC(b_+108, b_+109); A = B;
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CYC(b_+111, b_+113); alu_and(gb, 1);
  CYC(b_+113, b_+114); mem_wr(gb, HL, A);
  CYC(b_+114, b_+116); A = 0x0c;
  if (F & FZ) {
    CYCT(b_+116, b_+118);
  } else {
    CYC(b_+116, b_+118);
    CYC(b_+118, b_+120); A = 0xf3;
  }
  CYC(b_+120, b_+121); E = alu_inc8(gb, E);
  CYC(b_+121, b_+122); mem_wr(gb, DE, A);
  CYC(b_+122, b_+123); A = mem_rd(gb, HL);
  CALL_C(b_+123, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+126);
  CYC(b_+126, b_+129); objectSetVisible83_hook(gb);
}
