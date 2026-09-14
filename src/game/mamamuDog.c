#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t mamamuDog_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

void interactionCode54_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x66ed, 0x66ef); E = 0x42;
  CYC(0x66ef, 0x66f0); A = mem_rd(gb, DE);
  CYC(0x66f0, 0x66f1); push_effect(gb, 0x66f1);
  HANDOFF(mamamuDog_jump_table(gb));
}

static void mamamuDog_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void mamamuDog_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void dog_subid00_hook(GB *gb);
void dog_subid01_hook(GB *gb);
void dog_initGraphicsLoadScriptAndIncState_hook(GB *gb);
void dog_moveTowardTargetPosition_hook(GB *gb);
void dog_checkCloseToTargetPosition_hook(GB *gb);
void dog_updateDirection_hook(GB *gb);
void dog_incTargetPositionIndex_hook(GB *gb);
void dog_snapToTargetPosition_hook(GB *gb);
void dog_getTargetPositionAddress_hook(GB *gb);
void dog_setTargetPositionIndex_hook(GB *gb);

void dog_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x66f5, checkInteractionState_hook, 0x23fe, 0x66f8);
  if (!(F & FZ)) { CYCT(0x66f8, 0x66fa); goto state1; }
  CYC(0x66f8, 0x66fa);
  CYC(0x66fa, 0x66fc); A = 0x14;
  CALL_C(0x66fc, checkGlobalFlag_hook, 0x31f3, 0x66ff);
  if (F & FZ) { CYCT(0x66ff, 0x6701); goto init; }
  CYC(0x66ff, 0x6701);
  CYC(0x6701, 0x6703); A = 0x3b;
  CALL_C(0x6703, checkGlobalFlag_hook, 0x31f3, 0x6706);
  if (!(F & FZ)) { CYCT(0x6706, 0x6709); goto init; }
  CYC(0x6706, 0x6709);
  CALL_C(0x6709, getThisRoomFlags_hook, 0x197d, 0x670c);
  CYC(0x670c, 0x670e); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x670e, 0x6711); interactionDelete_hook(gb); return; }
  CYC(0x670e, 0x6711);
init:
  CALL_C(0x6711, dog_initGraphicsLoadScriptAndIncState_hook, 0x680f, 0x6714);
  CYC(0x6714, 0x6715); H = D;
  CYC(0x6715, 0x6717); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6717, 0x6719); mem_wr(gb, HL, 0x18);
  CYC(0x6719, 0x671b); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x671b, 0x671d); mem_wr(gb, HL, 0x28);
  CYC(0x671d, 0x671f); A = 2;
  CYC(0x671f, 0x6721); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6721, 0x6722); mem_wr(gb, HL, A);
  CALL_C(0x6722, interactionSetAnimation_hook, 0x262e, 0x6725);
state1:
  CALL_C(0x6725, interactionRunScript_hook, 0x2552, 0x6728);
  if (F & FC) { CYCT(0x6728, 0x672b); interactionDelete_hook(gb); return; }
  CYC(0x6728, 0x672b);
  CALL_C(0x672b, interactionAnimate_hook, 0x261b, 0x672e);
  CYC(0x672e, 0x6731); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

void dog_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6731, 0x6733); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6733, 0x6734); A = mem_rd(gb, DE);
  CYC(0x6734, 0x6735); push_effect(gb, 0x6735);
  switch (mamamuDog_jump_table(gb)) {
    case 0x673b: goto state0;
    case 0x676f: goto state1;
    case 0x679c: goto state2;
    default: HANDOFF(HL);
  }
state0:
  CYC(0x673b, 0x673d); A = 0x3b;
  CALL_C(0x673d, checkGlobalFlag_hook, 0x31f3, 0x6740);
  if (!(F & FZ)) { CYCT(0x6740, 0x6743); interactionDelete_hook(gb); return; }
  CYC(0x6740, 0x6743);
  CYC(0x6743, 0x6746); SET_HL(wGroup0RoomFlags + 0xe7);
  CYC(0x6746, 0x6748); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6748, 0x674b); interactionDelete_hook(gb); return; }
  CYC(0x6748, 0x674b);
  CYC(0x674b, 0x674e); A = W8(wMamamuDogLocation);
  CYC(0x674e, 0x674f); H = D;
  CYC(0x674f, 0x6751); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6751, 0x6752); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6752, 0x6755); interactionDelete_hook(gb); return; }
  CYC(0x6752, 0x6755);
  CALL_C(0x6755, dog_initGraphicsLoadScriptAndIncState_hook, 0x680f, 0x6758);
  CYC(0x6758, 0x675a); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x675a, 0x675c); mem_wr(gb, HL, 0x14);
  CYC(0x675c, 0x675e); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x675e, 0x6760); mem_wr(gb, HL, 0xff);
  CALL_C(0x6760, dog_setTargetPositionIndex_hook, 0x689c, 0x6763);
  CYC(0x6763, 0x6766); SET_HL(wMamamuDogLocation);
again:
  CALL_C(0x6766, getRandomNumber_hook, 0x043e, 0x6769);
  CYC(0x6769, 0x676b); alu_and(gb, 3);
  CYC(0x676b, 0x676c); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x676c, 0x676e); goto again; }
  CYC(0x676c, 0x676e);
  CYC(0x676e, 0x676f); mem_wr(gb, HL, A);
state1:
  CALL_C(0x676f, dog_moveTowardTargetPosition_hook, 0x6825, 0x6772);
  CALL_C(0x6772, dog_checkCloseToTargetPosition_hook, 0x6841, 0x6775);
  if (F & FC) CALL_C_CC(0x6775, dog_incTargetPositionIndex_hook, 0x686b, 0x6778);
  else CYC(0x6775, 0x6778);
  if (F & FC) { CYCT(0x6778, 0x677a); goto delete; }
  CYC(0x6778, 0x677a);
  CALL_C(0x677a, dog_moveTowardTargetPosition_hook, 0x6825, 0x677d);
  CALL_C(0x677d, dog_updateDirection_hook, 0x6857, 0x6780);
  CALL_C(0x6780, dog_checkCloseToTargetPosition_hook, 0x6841, 0x6783);
  if (F & FC) CALL_C_CC(0x6783, dog_incTargetPositionIndex_hook, 0x686b, 0x6786);
  else CYC(0x6783, 0x6786);
  if (F & FC) { CYCT(0x6786, 0x6788); goto delete; }
  CYC(0x6786, 0x6788);
  CYC(0x6788, 0x678b); SET_HL(0x5ed4);
  CYC(0x678b, 0x678d); E = 0x15;
  CALL_C(0x678d, interBankCall_hook, 0x008a, 0x6790);
  CALL_C(0x6790, interactionAnimate_hook, 0x261b, 0x6793);
  CALL_C(0x6793, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x6796);
  CYC(0x6796, 0x6799); objectAddToGrabbableObjectBuffer_hook(gb); return;
delete:
  CYC(0x6799, 0x679c); interactionDelete_hook(gb); return;
state2:
  CYC(0x679c, 0x679d); E = alu_inc8(gb, E);
  CYC(0x679d, 0x679e); A = mem_rd(gb, DE);
  CYC(0x679e, 0x679f); push_effect(gb, 0x679f);
  switch (mamamuDog_jump_table(gb)) {
    case 0x67a7: goto substate0;
    case 0x67bd: goto substate1;
    case 0x6802: CYC(0x6802, 0x6803); ret_effect(gb); return;
    case 0x6803: CYC(0x6803, 0x6806); objectSetVisiblec2_hook(gb); return;
    default: HANDOFF(HL);
  }
substate0:
  CYC(0x67a7, 0x67a8); alu_xor(gb, A);
  CYC(0x67a8, 0x67ab); W8(wLinkGrabState2) = A;
  CYC(0x67ab, 0x67ac); A = alu_inc8(gb, A);
  CYC(0x67ac, 0x67ad); mem_wr(gb, DE, A);
  CYC(0x67ad, 0x67af); A = 0x3b;
  CALL_C(0x67af, setGlobalFlag_hook, 0x31f9, 0x67b2);
  CYC(0x67b2, 0x67b4); A = 0x81;
  CYC(0x67b4, 0x67b7); W8(wMenuDisabled) = A;
  CYC(0x67b7, 0x67ba); W8(wDisableScreenTransitions) = A;
  CYC(0x67ba, 0x67bd); objectSetVisiblec1_hook(gb); return;
substate1:
  CYC(0x67bd, 0x67bf); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x67bf, 0x67c0); A = mem_rd(gb, DE);
  CYC(0x67c0, 0x67c1); push_effect(gb, 0x67c1);
  switch (mamamuDog_jump_table(gb)) {
    case 0x67c7: goto minor0;
    case 0x67df: goto minor1;
    case 0x67ed: goto minor2;
    default: HANDOFF(HL);
  }
minor0:
  CYC(0x67c7, 0x67ca); A = W8(wLinkGrabState);
  CYC(0x67ca, 0x67cc); alu_cp(gb, 0x83);
  if (!(F & FZ)) { CYCT(0x67cc, 0x67cd); ret_effect(gb); return; }
  CYC(0x67cc, 0x67cd);
  CYC(0x67cd, 0x67cf); A = 0x81;
  CYC(0x67cf, 0x67d2); W8(wDisabledObjects) = A;
  CYC(0x67d2, 0x67d4); A = 0x80;
  CYC(0x67d4, 0x67d7); W8(wMenuDisabled) = A;
  CYC(0x67d7, 0x67d8); H = D;
  CYC(0x67d8, 0x67da); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x67da, 0x67db); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x67db, 0x67dd); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x67dd, 0x67df); mem_wr(gb, HL, 40);
minor1:
  CALL_C(0x67df, interactionDecCounter1_hook, 0x23cc, 0x67e2);
  if (!(F & FZ)) { CYCT(0x67e2, 0x67e3); ret_effect(gb); return; }
  CYC(0x67e2, 0x67e3);
  CYC(0x67e3, 0x67e4); H = D;
  CYC(0x67e4, 0x67e6); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x67e6, 0x67e7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x67e7, 0x67ea); SET_BC(0x007f);
  CYC(0x67ea, 0x67ed); showText_hook(gb); return;
minor2:
  CYC(0x67ed, 0x67f0); A = W8(wTextIsActive);
  CYC(0x67f0, 0x67f1); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x67f1, 0x67f2); ret_effect(gb); return; }
  CYC(0x67f1, 0x67f2);
  CYC(0x67f2, 0x67f5); SET_HL(0x67fd);
  CALL_C(0x67f5, setWarpDestVariables_hook, 0x1997, 0x67f8);
  CYC(0x67f8, 0x67fa); A = 0x8d;
  CYC(0x67fa, 0x67fd); playSound_b00_hook(gb);
}

void dog_initGraphicsLoadScriptAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x680f, interactionInitGraphics_hook, 0x15fb, 0x6812);
  CALL_C(0x6812, objectMarkSolidPosition_hook, 0x24f0, 0x6815);
  CYC(0x6815, 0x6817); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6817, 0x6818); A = mem_rd(gb, DE);
  CYC(0x6818, 0x681b); SET_HL(0x68ee);
  CYC(0x681b, 0x681c); mamamuDog_add_double_index(gb, 0x681c);
  CYC(0x681c, 0x681d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x681d, 0x681e); H = mem_rd(gb, HL);
  CYC(0x681e, 0x681f); L = A;
  CALL_C(0x681f, interactionSetScript_hook, 0x2544, 0x6822);
  CYC(0x6822, 0x6825); interactionIncState_hook(gb);
}

void dog_moveTowardTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6825, 0x6826); H = D;
  CYC(0x6826, 0x6828); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6828, 0x6829); A = mem_rd(gb, HL);
  CYC(0x6829, 0x682a); alu_add(gb, A);
  CYC(0x682a, 0x682b); B = A;
  CYC(0x682b, 0x682d); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x682d, 0x682e); A = mem_rd(gb, DE);
  CYC(0x682e, 0x682f); L = A;
  CYC(0x682f, 0x6831); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6831, 0x6832); A = mem_rd(gb, DE);
  CYC(0x6832, 0x6833); H = A;
  CYC(0x6833, 0x6834); A = B;
  CYC(0x6834, 0x6835); mamamuDog_add_a_to_hl(gb, 0x6835);
  CYC(0x6835, 0x6836); B = mem_rd(gb, HL);
  CYC(0x6836, 0x6837); SET_HL(HL + 1);
  CYC(0x6837, 0x6838); C = mem_rd(gb, HL);
  CALL_C(0x6838, objectGetRelativeAngle_hook, 0x1ea4, 0x683b);
  CYC(0x683b, 0x683d); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x683d, 0x683e); mem_wr(gb, DE, A);
  CYC(0x683e, 0x6841); objectApplySpeed_hook(gb);
}

void dog_checkCloseToTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6841, dog_getTargetPositionAddress_hook, 0x688c, 0x6844);
  CYC(0x6844, 0x6846); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x6846, 0x6847); A = mem_rd(gb, BC);
  CYC(0x6847, 0x6848); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x6848, 0x684a); alu_add(gb, 1);
  CYC(0x684a, 0x684c); alu_cp(gb, 5);
  if (!(F & FC)) { CYCT(0x684c, 0x684d); ret_effect(gb); return; }
  CYC(0x684c, 0x684d);
  CYC(0x684d, 0x684e); SET_BC(BC + 1);
  CYC(0x684e, 0x6850); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x6850, 0x6851); A = mem_rd(gb, BC);
  CYC(0x6851, 0x6852); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x6852, 0x6854); alu_add(gb, 1);
  CYC(0x6854, 0x6856); alu_cp(gb, 5);
  CYC(0x6856, 0x6857); ret_effect(gb);
}

void dog_updateDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6857, 0x6858); H = D;
  CYC(0x6858, 0x685a); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x685a, 0x685b); A = mem_rd(gb, HL);
  CYC(0x685b, 0x685d); A = alu_swap(gb, A);
  CYC(0x685d, 0x685f); alu_and(gb, 1);
  CYC(0x685f, 0x6861); alu_xor(gb, 1);
  CYC(0x6861, 0x6863); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6863, 0x6864); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6864, 0x6865); ret_effect(gb); return; }
  CYC(0x6864, 0x6865);
  CYC(0x6865, 0x6866); mem_wr(gb, HL, A);
  CYC(0x6866, 0x6868); alu_add(gb, 2);
  CYC(0x6868, 0x686b); interactionSetAnimation_hook(gb);
}

void dog_incTargetPositionIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x686b, dog_snapToTargetPosition_hook, 0x687b, 0x686e);
  CYC(0x686e, 0x686f); H = D;
  CYC(0x686f, 0x6871); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x6871, 0x6872); A = mem_rd(gb, HL);
  CYC(0x6872, 0x6874); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6874, 0x6875); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6875, 0x6876); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x6876, 0x6877); ret_effect(gb); return; }
  CYC(0x6876, 0x6877);
  CYC(0x6877, 0x6879); mem_wr(gb, HL, 0);
  CYC(0x6879, 0x687a); alu_scf(gb);
  CYC(0x687a, 0x687b); ret_effect(gb);
}

void dog_snapToTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x687b, dog_getTargetPositionAddress_hook, 0x688c, 0x687e);
  CYC(0x687e, 0x6880); L = INTERACTION_BASE + OBJ_Y;
  CYC(0x6880, 0x6881); alu_xor(gb, A);
  CYC(0x6881, 0x6882); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6882, 0x6883); A = mem_rd(gb, BC);
  CYC(0x6883, 0x6884); mem_wr(gb, HL, A);
  CYC(0x6884, 0x6885); SET_BC(BC + 1);
  CYC(0x6885, 0x6887); L = INTERACTION_BASE + OBJ_X;
  CYC(0x6887, 0x6888); alu_xor(gb, A);
  CYC(0x6888, 0x6889); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6889, 0x688a); A = mem_rd(gb, BC);
  CYC(0x688a, 0x688b); mem_wr(gb, HL, A);
  CYC(0x688b, 0x688c); ret_effect(gb);
}

void dog_getTargetPositionAddress_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x688c, 0x688e); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x688e, 0x688f); A = mem_rd(gb, DE);
  CYC(0x688f, 0x6890); C = A;
  CYC(0x6890, 0x6892); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6892, 0x6893); A = mem_rd(gb, DE);
  CYC(0x6893, 0x6894); B = A;
  CYC(0x6894, 0x6895); H = D;
  CYC(0x6895, 0x6897); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6897, 0x6898); A = mem_rd(gb, HL);
  CALL_C(0x6898, addDoubleIndexToBc_hook, 0x007e, 0x689b);
  CYC(0x689b, 0x689c); ret_effect(gb);
}

void dog_setTargetPositionIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x689c, 0x689f); SET_HL(0x68ae);
  CYC(0x689f, 0x68a0); mamamuDog_add_double_index(gb, 0x68a0);
  CYC(0x68a0, 0x68a2); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x68a2, 0x68a3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x68a3, 0x68a4); mem_wr(gb, DE, A);
  CYC(0x68a4, 0x68a6); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x68a6, 0x68a7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x68a7, 0x68a8); mem_wr(gb, DE, A);
  CYC(0x68a8, 0x68aa); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x68aa, 0x68ac); A = 6;
  CYC(0x68ac, 0x68ae); mem_wr(gb, DE, A);
  CYC(0x68ae, 0x68af); ret_effect(gb);
}
