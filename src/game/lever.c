#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)
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
  CYC(0x7075, 0x7077); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7077, 0x7078); mem_wr(gb, HL, B);
  CYC(0x7078, 0x707a); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x707a, 0x707b); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x707b, 0x707d); A = alu_swap(gb, A);
  CYC(0x707d, 0x707f); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x707f, 0x7080); mem_wr(gb, HL, A);
  CYC(0x7080, 0x7081); ret_effect(gb);
}

static void lever_negative_comparison(GB *gb) {
  CYC(0x70f8, 0x70fa); L = INTERACTION_BASE + LEVER_VAR30;
  CYC(0x70fa, 0x70fb); alu_add(gb, mem_rd(gb, HL));
  CYC(0x70fb, 0x70fd); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x70fd, 0x70fe); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x70fe, 0x70ff); ret_effect(gb); return; }
  CYC(0x70fe, 0x70ff);
  CYC(0x70ff, 0x7100); mem_wr(gb, HL, A);
  CYC(0x7100, 0x7101); ret_effect(gb);
}

static void lever_positive_comparison(GB *gb) {
  CYC(0x7109, 0x710b); L = INTERACTION_BASE + LEVER_VAR30;
  CYC(0x710b, 0x710c); alu_add(gb, mem_rd(gb, HL));
  CYC(0x710c, 0x710d); B = A;
  CYC(0x710d, 0x710f); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x710f, 0x7110); A = mem_rd(gb, HL);
  CYC(0x7110, 0x7111); alu_cp(gb, B);
  if (F & FC) { CYCT(0x7111, 0x7112); ret_effect(gb); return; }
  CYC(0x7111, 0x7112);
  CYC(0x7112, 0x7113); mem_wr(gb, HL, B);
  CYC(0x7113, 0x7114); ret_effect(gb);
}

static void lever_check_fully_extended(GB *gb) {
  CYC(0x70ec, 0x70ee); E = INTERACTION_BASE + LEVER_VAR31;
  CYC(0x70ee, 0x70ef); A = mem_rd(gb, DE);
  CYC(0x70ef, 0x70f0); H = D;
  CYC(0x70f0, 0x70f2); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x70f2, 0x70f4); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x70f4, 0x70f6);
    lever_positive_comparison(gb);
    return;
  }
  CYC(0x70f4, 0x70f6);
  CYC(0x70f6, 0x70f7); alu_cpl(gb);
  CYC(0x70f7, 0x70f8); A = alu_inc8(gb, A);
  lever_negative_comparison(gb);
}

static void lever_check_fully_retracted(GB *gb) {
  CYC(0x7101, 0x7102); alu_xor(gb, A);
  CYC(0x7102, 0x7103); H = D;
  CYC(0x7103, 0x7105); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7105, 0x7107); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x7107, 0x7109);
    lever_negative_comparison(gb);
    return;
  }
  CYC(0x7107, 0x7109);
  lever_positive_comparison(gb);
}

static void lever_update_pull_offset(GB *gb, uint16_t sp0_) {
  if (!(F & FC)) {
    CYCT(0x7114, 0x7116);
  } else {
    CYC(0x7114, 0x7116);
    CYC(0x7116, 0x7117); alu_cpl(gb);
    CYC(0x7117, 0x7118); A = alu_inc8(gb, A);
  }
  CYC(0x7118, 0x7119); H = D;
  CYC(0x7119, 0x711b); L = INTERACTION_BASE + LEVER_VAR31;
  CYC(0x711b, 0x711c); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYC(0x711c, 0x711e);
    CYC(0x711e, 0x711f); H = A;
    CYC(0x711f, 0x7120); push_effect(gb, HL);
    CYC(0x7120, 0x7122); A = 0x6c;
    CALL_C(0x7122, playSound_b00_hook, 0x0c98, 0x7125);
    CYC(0x7125, 0x7126); SET_HL(pop_effect(gb));
    CYC(0x7126, 0x7127); A = H;
    CYC(0x7127, 0x7129); alu_or(gb, 0x80);
    CYC(0x7129, 0x712a); H = D;
  } else {
    CYCT(0x711c, 0x711e);
  }
  CYC(0x712a, 0x712b); B = A;
  CYC(0x712b, 0x712c); L = alu_inc8(gb, L);
  CYC(0x712c, 0x712d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x712d, 0x712e); H = mem_rd(gb, HL);
  CYC(0x712e, 0x712f); L = A;
  CYC(0x712f, 0x7130); A = mem_rd(gb, HL);
  CYC(0x7130, 0x7131); mem_wr(gb, HL, B);
  CYC(0x7131, 0x7132); ret_effect(gb);
}

static void lever_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x6fea, objectPushLinkAwayOnCollision_hook, 0x230e, 0x6fed);
  CALL_C(0x6fed, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6ff0);
  CYC(0x6ff0, 0x6ff2); alu_add(gb, 0x14);
  CYC(0x6ff2, 0x6ff4); alu_and(gb, 0x18);
  CYC(0x6ff4, 0x6ff6); A = alu_swap(gb, A);
  CYC(0x6ff6, 0x6ff7); alu_rlca(gb);
  CYC(0x6ff7, 0x6ff8); C = A;
  CYC(0x6ff8, 0x6ffa); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6ffa, 0x6ffb); A = mem_rd(gb, DE);
  CYC(0x6ffb, 0x6ffc); alu_add(gb, A);
  CYC(0x6ffc, 0x6ffd); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x6ffd, 0x6ffe); ret_effect(gb); return; }
  CYC(0x6ffd, 0x6ffe);
  CYC(0x6ffe, 0x7001); A = W8(w1Link_direction);
  CYC(0x7001, 0x7002); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x7002, 0x7003); ret_effect(gb); return; }
  CYC(0x7002, 0x7003);
  CYC(0x7003, 0x7006); objectAddToGrabbableObjectBuffer_hook(gb);
}

static void lever_state2(GB *gb, uint16_t sp0_) {
  CYC(0x7006, 0x7007); E = alu_inc8(gb, E);
  CYC(0x7007, 0x7008); A = mem_rd(gb, DE);
  CYC(0x7008, 0x7009); push_effect(gb, 0x7009);
  switch (lever_jump_table(gb)) {
    case 0x7011: goto substate0;
    case 0x7033: goto substate1;
    case 0x706b: goto substate2;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x7011, 0x7012); H = D;
  CYC(0x7012, 0x7013); L = E;
  CYC(0x7013, 0x7014); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7014, 0x7016); A = 0x80;
  CYC(0x7016, 0x7019); W8(wLinkGrabState2) = A;
  CYC(0x7019, 0x701b); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x701b, 0x701c); A = mem_rd(gb, HL);
  CYC(0x701c, 0x701f); W8(w1Link_xh) = A;
  CYC(0x701f, 0x7021); L = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x7021, 0x7022); A = mem_rd(gb, HL);
  CYC(0x7022, 0x7024); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7024, 0x7025); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7025, 0x7028); W8(w1Link_yh) = A;
  CYC(0x7028, 0x7029); alu_xor(gb, A);
  CYC(0x7029, 0x702a); L = alu_dec8(gb, L);
  CYC(0x702a, 0x702b); mem_wr(gb, HL, A);
  CYC(0x702b, 0x702e); W8(w1Link_y) = A;
  CYC(0x702e, 0x7030); B = 0x0a;
  CYC(0x7030, 0x7031); A = alu_inc8(gb, A);
  CYC(0x7031, 0x7033);
  lever_set_speed_and_angle(gb);
  return;

substate1:
  CYC(0x7033, 0x7036); A = W8(w1ParentItem2_animParameter);
  CYC(0x7036, 0x7037); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7037, 0x7039); goto pull; }
  CYC(0x7037, 0x7039);
  CYC(0x7039, 0x703b); E = INTERACTION_BASE + OBJ_VAR35;
  CYC(0x703b, 0x703c); mem_wr(gb, DE, A);
  CYC(0x703c, 0x703d); ret_effect(gb);
  return;

pull:
  CYC(0x703d, 0x7040); push_effect(gb, 0x7040);
  lever_check_fully_extended(gb);
  if (!(F & FC)) { CYCT(0x7040, 0x7041); ret_effect(gb); return; }
  CYC(0x7040, 0x7041);
  CYC(0x7041, 0x7043); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7043, 0x7044); C = mem_rd(gb, HL);
  CYC(0x7044, 0x7046); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7046, 0x7047); B = mem_rd(gb, HL);
  CALL_C(0x7047, updateLinkPositionGivenVelocity_hook, 0x231e, 0x704a);
  CYC(0x704a, 0x704d); A = W8(w1Link_yh);
  CYC(0x704d, 0x704e); H = D;
  CYC(0x704e, 0x7050); L = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x7050, 0x7051); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x7051, 0x7053); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7053, 0x7054); mem_wr(gb, HL, A);
  CYC(0x7054, 0x7056); L = INTERACTION_BASE + LEVER_VAR30;
  CYC(0x7056, 0x7057); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x7057, 0x705a); push_effect(gb, 0x705a);
  lever_update_pull_offset(gb, sp0_);
  CYC(0x705a, 0x705b); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x705b, 0x705c); ret_effect(gb); return; }
  CYC(0x705b, 0x705c);
  CYC(0x705c, 0x705d); H = D;
  CYC(0x705d, 0x705f); L = INTERACTION_BASE + OBJ_VAR35;
  CYC(0x705f, 0x7061); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7061, 0x7062); ret_effect(gb); return; }
  CYC(0x7061, 0x7062);
  CYC(0x7062, 0x7063); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7063, 0x7065); alu_bit(gb, 7, B);
  if (!(F & FZ)) { CYCT(0x7065, 0x7066); ret_effect(gb); return; }
  CYC(0x7065, 0x7066);
  CYC(0x7066, 0x7068); A = 0x71;
  CYC(0x7068, 0x706b); playSound_b00_hook(gb);
  return;

substate2:
  CALL_C(0x706b, interactionIncState_hook, 0x23e0, 0x706e);
  CYC(0x706e, 0x7070); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x7070, 0x7072); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(0x7072, 0x7074); B = 0x0a;
  CYC(0x7074, 0x7075); alu_xor(gb, A);
  lever_set_speed_and_angle(gb);
}

static void lever_state3(GB *gb, uint16_t sp0_) {
  CALL_C(0x7081, objectApplySpeed_hook, 0x201d, 0x7084);
  CYC(0x7084, 0x7086); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x7086, 0x7087); A = mem_rd(gb, DE);
  CYC(0x7087, 0x7088); B = A;
  CYC(0x7088, 0x708a); E = INTERACTION_BASE + LEVER_VAR30;
  CYC(0x708a, 0x708b); A = mem_rd(gb, DE);
  CYC(0x708b, 0x708c); alu_sub(gb, B);
  CYC(0x708c, 0x708f); push_effect(gb, 0x708f);
  lever_update_pull_offset(gb, sp0_);
  CYC(0x708f, 0x7092); push_effect(gb, 0x7092);
  lever_check_fully_retracted(gb);
  if (F & FC) { CYCT(0x7092, 0x7094); goto make_grabbable; }
  CYC(0x7092, 0x7094);
  CYC(0x7094, 0x7096); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7096, 0x7098); mem_wr(gb, HL, 1);
  CYC(0x7098, 0x709a); B = 0x0a;
  CYC(0x709a, 0x709c); A = 1;
  CYC(0x709c, 0x709f); push_effect(gb, 0x709f);
  lever_set_speed_and_angle(gb);

make_grabbable:
  CYC(0x709f, 0x70a2);
  lever_state1(gb, sp0_);
}

static void lever_connection(GB *gb, uint16_t sp0_) {
  CYC(0x70a2, 0x70a3); A = mem_rd(gb, DE);
  CYC(0x70a3, 0x70a4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x70a4, 0x70a6); goto state1; }
  CYC(0x70a4, 0x70a6);
  CALL_C(0x70a6, interactionInitGraphics_hook, 0x15fb, 0x70a9);
  CALL_C(0x70a9, interactionIncState_hook, 0x23e0, 0x70ac);
  CALL_C(0x70ac, objectSetVisible83_hook, 0x1e72, 0x70af);
  CYC(0x70af, 0x70b1); A = 0x0d;
  CALL_C(0x70b1, objectGetRelatedObject1Var_hook, 0x2160, 0x70b4);
  CYC(0x70b4, 0x70b5); E = L;
  CYC(0x70b5, 0x70b6); A = mem_rd(gb, HL);
  CYC(0x70b6, 0x70b7); mem_wr(gb, DE, A);

state1:
  CYC(0x70b7, 0x70b9); A = 2;
  CALL_C(0x70b9, objectGetRelatedObject1Var_hook, 0x2160, 0x70bc);
  CYC(0x70bc, 0x70bd); A = mem_rd(gb, HL);
  CYC(0x70bd, 0x70be); alu_add(gb, A);
  CYC(0x70be, 0x70bf); alu_add(gb, A);
  CYC(0x70bf, 0x70c0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x70c0, 0x70c1); B = A;
  CYC(0x70c1, 0x70c3); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x70c3, 0x70c4); A = mem_rd(gb, HL);
  CYC(0x70c4, 0x70c6); L = INTERACTION_BASE + LEVER_VAR30;
  CYC(0x70c6, 0x70c7); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(0x70c7, 0x70c9);
  } else {
    CYC(0x70c7, 0x70c9);
    CYC(0x70c9, 0x70ca); alu_cpl(gb);
    CYC(0x70ca, 0x70cb); A = alu_inc8(gb, A);
  }
  CYC(0x70cb, 0x70cd); A = alu_swap(gb, A);
  CYC(0x70cd, 0x70cf); alu_and(gb, 7);
  CYC(0x70cf, 0x70d0); push_effect(gb, AF);
  CYC(0x70d0, 0x70d1); alu_add(gb, B);
  CYC(0x70d1, 0x70d4); SET_BC(0x70e2);
  CALL_C(0x70d4, addAToBc_hook, 0x006d, 0x70d7);
  CYC(0x70d7, 0x70d8); A = mem_rd(gb, BC);
  CYC(0x70d8, 0x70d9); alu_add(gb, mem_rd(gb, HL));
  CYC(0x70d9, 0x70db); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x70db, 0x70dc); mem_wr(gb, DE, A);
  CYC(0x70dc, 0x70dd); SET_AF(pop_effect(gb));
  CYC(0x70dd, 0x70df); alu_add(gb, 2);
  CYC(0x70df, 0x70e2); interactionSetAnimation_hook(gb);
}

void interactionCode61_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f65, 0x6f67); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6f67, 0x6f68); A = mem_rd(gb, DE);
  CYC(0x6f68, 0x6f69); alu_rlca(gb);
  CYC(0x6f69, 0x6f6b); E = INTERACTION_BASE + OBJ_STATE;
  if (F & FC) {
    CYCT(0x6f6b, 0x6f6e);
    lever_connection(gb, sp0_);
    return;
  }
  CYC(0x6f6b, 0x6f6e);
  CYC(0x6f6e, 0x6f6f); A = mem_rd(gb, DE);
  CYC(0x6f6f, 0x6f70); push_effect(gb, 0x6f70);
  switch (lever_jump_table(gb)) {
    case 0x6f78: goto state0;
    case 0x6fea: lever_state1(gb, sp0_); return;
    case 0x7006: lever_state2(gb, sp0_); return;
    case 0x7081: lever_state3(gb, sp0_); return;
    default: HANDOFF(HL);
  }

state0:
  CALL_C(0x6f78, interactionInitGraphics_hook, 0x15fb, 0x6f7b);
  CYC(0x6f7b, 0x6f7d); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6f7d, 0x6f7e); A = mem_rd(gb, DE);
  CYC(0x6f7e, 0x6f7f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6f7f, 0x6f81); goto initialized; }
  CYC(0x6f7f, 0x6f81);
  CALL_C(0x6f81, getFreeInteractionSlot_hook, 0x3aef, 0x6f84);
  if (!(F & FZ)) { CYCT(0x6f84, 0x6f85); ret_effect(gb); return; }
  CYC(0x6f84, 0x6f85);
  CYC(0x6f85, 0x6f87); mem_wr(gb, HL, 0x61);
  CYC(0x6f87, 0x6f89); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x6f89, 0x6f8a); E = L;
  CYC(0x6f8a, 0x6f8c); A = INTERACTION_BASE;
  CYC(0x6f8c, 0x6f8d); mem_wr(gb, DE, A);
  CYC(0x6f8d, 0x6f8e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6f8e, 0x6f8f); E = alu_inc8(gb, E);
  CYC(0x6f8f, 0x6f90); mem_wr(gb, HL, D);
  CYC(0x6f90, 0x6f91); A = H;
  CYC(0x6f91, 0x6f92); mem_wr(gb, DE, A);
  CYC(0x6f92, 0x6f93); alu_cp(gb, D);
  if (!(F & FC)) { CYCT(0x6f93, 0x6f95); goto child_higher; }
  CYC(0x6f93, 0x6f95);
  CYC(0x6f95, 0x6f97); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6f97, 0x6f98); E = L;
  CYC(0x6f98, 0x6f99); A = mem_rd(gb, DE);
  CYC(0x6f99, 0x6f9a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6f9a, 0x6f9c); A = 0x80;
  CYC(0x6f9c, 0x6f9d); mem_wr(gb, DE, A);
  CYC(0x6f9d, 0x6f9e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f9e, 0x6fa1); objectCopyPosition_hook(gb);
  return;

child_higher:
  CYC(0x6fa1, 0x6fa3); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6fa3, 0x6fa5); mem_wr(gb, HL, 0x80);

initialized:
  CALL_C(0x6fa5, interactionIncState_hook, 0x23e0, 0x6fa8);
  CYC(0x6fa8, 0x6faa); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6faa, 0x6fac); mem_wr(gb, HL, 5);
  CYC(0x6fac, 0x6fad); L = alu_inc8(gb, L);
  CYC(0x6fad, 0x6faf); mem_wr(gb, HL, 1);
  CYC(0x6faf, 0x6fb1); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x6fb1, 0x6fb2); A = mem_rd(gb, HL);
  CYC(0x6fb2, 0x6fb4); E = INTERACTION_BASE + LEVER_VAR30;
  CYC(0x6fb4, 0x6fb5); mem_wr(gb, DE, A);
  CYC(0x6fb5, 0x6fb7); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6fb7, 0x6fb8); A = mem_rd(gb, HL);
  CYC(0x6fb8, 0x6fba); alu_and(gb, 0x30);
  CYC(0x6fba, 0x6fbc); A = alu_swap(gb, A);
  CYC(0x6fbc, 0x6fbf); SET_BC(0x6fe6);
  CALL_C(0x6fbf, addAToBc_hook, 0x006d, 0x6fc2);
  CYC(0x6fc2, 0x6fc3); E = alu_inc8(gb, E);
  CYC(0x6fc3, 0x6fc4); A = mem_rd(gb, BC);
  CYC(0x6fc4, 0x6fc5); mem_wr(gb, DE, A);
  CYC(0x6fc5, 0x6fc8); SET_BC(wLever1PullDistance);
  CYC(0x6fc8, 0x6fca); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6fca, 0x6fcc);
  } else {
    CYC(0x6fca, 0x6fcc);
    CYC(0x6fcc, 0x6fcd); SET_BC(BC + 1);
  }
  CYC(0x6fcd, 0x6fce); E = alu_inc8(gb, E);
  CYC(0x6fce, 0x6fcf); A = C;
  CYC(0x6fcf, 0x6fd0); mem_wr(gb, DE, A);
  CYC(0x6fd0, 0x6fd1); E = alu_inc8(gb, E);
  CYC(0x6fd1, 0x6fd2); A = B;
  CYC(0x6fd2, 0x6fd3); mem_wr(gb, DE, A);
  CYC(0x6fd3, 0x6fd4); A = mem_rd(gb, HL);
  CYC(0x6fd4, 0x6fd6); alu_and(gb, 1);
  CYC(0x6fd6, 0x6fd7); mem_wr(gb, HL, A);
  CYC(0x6fd7, 0x6fd9); A = 0x0c;
  if (F & FZ) {
    CYCT(0x6fd9, 0x6fdb);
  } else {
    CYC(0x6fd9, 0x6fdb);
    CYC(0x6fdb, 0x6fdd); A = 0xf3;
  }
  CYC(0x6fdd, 0x6fde); E = alu_inc8(gb, E);
  CYC(0x6fde, 0x6fdf); mem_wr(gb, DE, A);
  CYC(0x6fdf, 0x6fe0); A = mem_rd(gb, HL);
  CALL_C(0x6fe0, interactionSetAnimation_hook, 0x262e, 0x6fe3);
  CYC(0x6fe3, 0x6fe6); objectSetVisible83_hook(gb);
}
