#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode54), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode54), (from), (to), true)

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
  BASE(interactionCode54);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
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
  BASE(dog_subid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x14;
  CALL_C(b_+7, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+12); goto init; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0x3b;
  CALL_C(b_+14, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+17);
  if (!(F & FZ)) { CYCT(b_+17, b_+20); goto init; }
  CYC(b_+17, b_+20);
  CALL_C(b_+20, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+23);
  CYC(b_+23, b_+25); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+25, b_+28); interactionDelete_hook(gb); return; }
  CYC(b_+25, b_+28);
init:
  CALL_C(b_+28, dog_initGraphicsLoadScriptAndIncState_hook, SYM(dog_initGraphicsLoadScriptAndIncState), b_+31);
  CYC(b_+31, b_+32); H = D;
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x18);
  CYC(b_+36, b_+38); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x28);
  CYC(b_+40, b_+42); A = 2;
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  CALL_C(b_+45, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+48);
state1:
  CALL_C(b_+48, interactionRunScript_hook, SYM(interactionRunScript), b_+51);
  if (F & FC) { CYCT(b_+51, b_+54); interactionDelete_hook(gb); return; }
  CYC(b_+51, b_+54);
  CALL_C(b_+54, interactionAnimate_hook, SYM(interactionAnimate), b_+57);
  CYC(b_+57, SYM(dog_subid01)); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

void dog_subid01_hook(GB *gb) {
  BASE(dog_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (mamamuDog_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+62) { goto state1; }
    else if (jt_ == b_+107) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x3b;
  CALL_C(b_+12, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; }
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+21); SET_HL(wGroup0RoomFlags + 0xe7);
  CYC(b_+21, b_+23); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+23, b_+26); interactionDelete_hook(gb); return; }
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+29); A = W8(wMamamuDogLocation);
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+32, b_+33); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+33, b_+36); interactionDelete_hook(gb); return; }
  CYC(b_+33, b_+36);
  CALL_C(b_+36, dog_initGraphicsLoadScriptAndIncState_hook, SYM(dog_initGraphicsLoadScriptAndIncState), b_+39);
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x14);
  CYC(b_+43, b_+45); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0xff);
  CALL_C(b_+47, dog_setTargetPositionIndex_hook, SYM(dog_setTargetPositionIndex), b_+50);
  CYC(b_+50, b_+53); SET_HL(wMamamuDogLocation);
again:
  CALL_C(b_+53, getRandomNumber_hook, SYM(getRandomNumber), b_+56);
  CYC(b_+56, b_+58); alu_and(gb, 3);
  CYC(b_+58, b_+59); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+59, b_+61); goto again; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); mem_wr(gb, HL, A);
state1:
  CALL_C(b_+62, dog_moveTowardTargetPosition_hook, SYM(dog_moveTowardTargetPosition), b_+65);
  CALL_C(b_+65, dog_checkCloseToTargetPosition_hook, SYM(dog_checkCloseToTargetPosition), b_+68);
  if (F & FC) CALL_C_CC(b_+68, dog_incTargetPositionIndex_hook, SYM(dog_incTargetPositionIndex), b_+71);
  else CYC(b_+68, b_+71);
  if (F & FC) { CYCT(b_+71, b_+73); goto delete; }
  CYC(b_+71, b_+73);
  CALL_C(b_+73, dog_moveTowardTargetPosition_hook, SYM(dog_moveTowardTargetPosition), b_+76);
  CALL_C(b_+76, dog_updateDirection_hook, SYM(dog_updateDirection), b_+79);
  CALL_C(b_+79, dog_checkCloseToTargetPosition_hook, SYM(dog_checkCloseToTargetPosition), b_+82);
  if (F & FC) CALL_C_CC(b_+82, dog_incTargetPositionIndex_hook, SYM(dog_incTargetPositionIndex), b_+85);
  else CYC(b_+82, b_+85);
  if (F & FC) { CYCT(b_+85, b_+87); goto delete; }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+90); SET_HL((SYM(forestFairy_subid04State1) + 10));
  CYC(b_+90, b_+92); E = 0x15;
  CALL_C(b_+92, interBankCall_hook, 0x008a, b_+95);
  CALL_C(b_+95, interactionAnimate_hook, SYM(interactionAnimate), b_+98);
  CALL_C(b_+98, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+101);
  CYC(b_+101, b_+104); objectAddToGrabbableObjectBuffer_hook(gb); return;
delete:
  CYC(b_+104, b_+107); interactionDelete_hook(gb); return;
state2:
  CYC(b_+107, b_+108); E = alu_inc8(gb, E);
  CYC(b_+108, b_+109); A = mem_rd(gb, DE);
  CYC(b_+109, b_+110); push_effect(gb, b_+110);
  do { uint16_t jt_ = (mamamuDog_jump_table(gb));
    if (jt_ == b_+118) { goto substate0; }
    else if (jt_ == b_+140) { goto substate1; }
    else if (jt_ == b_+209) { CYC(b_+209, b_+210); ret_effect(gb); return; }
    else if (jt_ == b_+210) { CYC(b_+210, b_+213); objectSetVisiblec2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
substate0:
  CYC(b_+118, b_+119); alu_xor(gb, A);
  CYC(b_+119, b_+122); W8(wLinkGrabState2) = A;
  CYC(b_+122, b_+123); A = alu_inc8(gb, A);
  CYC(b_+123, b_+124); mem_wr(gb, DE, A);
  CYC(b_+124, b_+126); A = 0x3b;
  CALL_C(b_+126, setGlobalFlag_hook, SYM(setGlobalFlag), b_+129);
  CYC(b_+129, b_+131); A = 0x81;
  CYC(b_+131, b_+134); W8(wMenuDisabled) = A;
  CYC(b_+134, b_+137); W8(wDisableScreenTransitions) = A;
  CYC(b_+137, b_+140); objectSetVisiblec1_hook(gb); return;
substate1:
  CYC(b_+140, b_+142); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+144); push_effect(gb, b_+144);
  do { uint16_t jt_ = (mamamuDog_jump_table(gb));
    if (jt_ == b_+150) { goto minor0; }
    else if (jt_ == b_+174) { goto minor1; }
    else if (jt_ == b_+188) { goto minor2; }
    else { HANDOFF(HL); }
  } while (0);
minor0:
  CYC(b_+150, b_+153); A = W8(wLinkGrabState);
  CYC(b_+153, b_+155); alu_cp(gb, 0x83);
  if (!(F & FZ)) { CYCT(b_+155, b_+156); ret_effect(gb); return; }
  CYC(b_+155, b_+156);
  CYC(b_+156, b_+158); A = 0x81;
  CYC(b_+158, b_+161); W8(wDisabledObjects) = A;
  CYC(b_+161, b_+163); A = 0x80;
  CYC(b_+163, b_+166); W8(wMenuDisabled) = A;
  CYC(b_+166, b_+167); H = D;
  CYC(b_+167, b_+169); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+169, b_+170); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+170, b_+172); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+172, b_+174); mem_wr(gb, HL, 40);
minor1:
  CALL_C(b_+174, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+177);
  if (!(F & FZ)) { CYCT(b_+177, b_+178); ret_effect(gb); return; }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+179); H = D;
  CYC(b_+179, b_+181); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+181, b_+182); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+182, b_+185); SET_BC(0x007f);
  CYC(b_+185, b_+188); showText_hook(gb); return;
minor2:
  CYC(b_+188, b_+191); A = W8(wTextIsActive);
  CYC(b_+191, b_+192); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+192, b_+193); ret_effect(gb); return; }
  CYC(b_+192, b_+193);
  CYC(b_+193, b_+196); SET_HL(b_+204);
  CALL_C(b_+196, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+199);
  CYC(b_+199, b_+201); A = 0x8d;
  CYC(b_+201, b_+204); playSound_b00_hook(gb);
}

void dog_initGraphicsLoadScriptAndIncState_hook(GB *gb) {
  BASE(dog_initGraphicsLoadScriptAndIncState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); SET_HL(SYM(dog_scriptTable));
  CYC(b_+12, b_+13); mamamuDog_add_double_index(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  CYC(b_+15, b_+16); L = A;
  CALL_C(b_+16, interactionSetScript_hook, SYM(interactionSetScript), b_+19);
  CYC(b_+19, SYM(dog_moveTowardTargetPosition)); interactionIncState_hook(gb);
}

void dog_moveTowardTargetPosition_hook(GB *gb) {
  BASE(dog_moveTowardTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); H = A;
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+16); mamamuDog_add_a_to_hl(gb, b_+16);
  CYC(b_+16, b_+17); B = mem_rd(gb, HL);
  CYC(b_+17, b_+18); SET_HL(HL + 1);
  CYC(b_+18, b_+19); C = mem_rd(gb, HL);
  CALL_C(b_+19, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, SYM(dog_checkCloseToTargetPosition)); objectApplySpeed_hook(gb);
}

void dog_checkCloseToTargetPosition_hook(GB *gb) {
  BASE(dog_checkCloseToTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, dog_getTargetPositionAddress_hook, SYM(dog_getTargetPositionAddress), b_+3);
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, BC);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); alu_add(gb, 1);
  CYC(b_+9, b_+11); alu_cp(gb, 5);
  if (!(F & FC)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); SET_BC(BC + 1);
  CYC(b_+13, b_+15); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+15, b_+16); A = mem_rd(gb, BC);
  CYC(b_+16, b_+17); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+19); alu_add(gb, 1);
  CYC(b_+19, b_+21); alu_cp(gb, 5);
  CYC(b_+21, SYM(dog_updateDirection)); ret_effect(gb);
}

void dog_updateDirection_hook(GB *gb) {
  BASE(dog_updateDirection);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+8); alu_and(gb, 1);
  CYC(b_+8, b_+10); alu_xor(gb, 1);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); alu_add(gb, 2);
  CYC(b_+17, SYM(dog_incTargetPositionIndex)); interactionSetAnimation_hook(gb);
}

void dog_incTargetPositionIndex_hook(GB *gb) {
  BASE(dog_incTargetPositionIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, dog_snapToTargetPosition_hook, SYM(dog_snapToTargetPosition), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0);
  CYC(b_+14, b_+15); alu_scf(gb);
  CYC(b_+15, SYM(dog_snapToTargetPosition)); ret_effect(gb);
}

void dog_snapToTargetPosition_hook(GB *gb) {
  BASE(dog_snapToTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, dog_getTargetPositionAddress_hook, SYM(dog_getTargetPositionAddress), b_+3);
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_Y;
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+10); SET_BC(BC + 1);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_X;
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); A = mem_rd(gb, BC);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, SYM(dog_getTargetPositionAddress)); ret_effect(gb);
}

void dog_getTargetPositionAddress_hook(GB *gb) {
  BASE(dog_getTargetPositionAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CALL_C(b_+12, addDoubleIndexToBc_hook, 0x007e, b_+15);
  CYC(b_+15, SYM(dog_setTargetPositionIndex)); ret_effect(gb);
}

void dog_setTargetPositionIndex_hook(GB *gb) {
  BASE(dog_setTargetPositionIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+18);
  CYC(b_+3, b_+4); mamamuDog_add_double_index(gb, b_+4);
  CYC(b_+4, b_+6); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+14, b_+16); A = 6;
  CYC(b_+16, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}
