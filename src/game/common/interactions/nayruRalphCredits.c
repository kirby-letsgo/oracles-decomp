#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/interactions/nayruRalphCredits.s (INTERAC_NAYRU_RALPH_CREDITS), bank $10.

static uint16_t interactionCodedf_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void interactionCodedf_hook(GB *gb) {
  BASE(interactionCodedf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodedf_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+37) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x14);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x18);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x3c);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+31, b_+34);
    objectSetVisiblec2_hook(gb);
    return;
  }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+37);
  objectSetVisiblec0_hook(gb);
  return;
state1:
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); push_effect(gb, b_+41);
  do { uint16_t jt_ = (interactionCodedf_jump_table(gb));
    if (jt_ == b_+55) { goto substate0; }
    else if (jt_ == b_+62) { goto substate1; }
    else if (jt_ == b_+88) { goto substate2; }
    else if (jt_ == b_+109) { goto substate3; }
    else if (jt_ == b_+189) { goto substate4; }
    else if (jt_ == b_+212) { goto substate5; }
    else if (jt_ == b_+226) { goto substate6; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CALL_C(b_+55, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+58);
  if (!(F & FZ)) { RET_TAKEN(b_+58); return; }
  CYC(b_+58, b_+59);
  CALL_C(b_+59, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+62);
substate1:
  CALL_C(b_+62, interactionAnimate_hook, SYM(interactionAnimate), b_+65);
  CALL_C(b_+65, objectApplySpeed_hook, SYM(objectApplySpeed), b_+68);
  CYC(b_+68, b_+70); alu_cp(gb, 0x68);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; }
  CYC(b_+70, b_+71);
  CALL_C(b_+71, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+74);
  CYC(b_+74, b_+76); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0xb4);
  CYC(b_+78, b_+80); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+82); return; }
  CYC(b_+82, b_+83);
  CYC(b_+83, b_+85); A = 0x05;
  CYC(b_+85, b_+88);
  interactionSetAnimation_hook(gb);
  return;
substate2:
  CALL_C(b_+88, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+91);
  if (!(F & FZ)) { RET_TAKEN(b_+91); return; }
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+95); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x01);
  CALL_C(b_+97, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+100);
  CYC(b_+100, b_+102); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x04);
  CYC(b_+104, b_+105); L = alu_inc8(gb, L);
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x01);
  CYC(b_+107, b_+109);
  goto setRandomVar38;
substate3:
  CYC(b_+109, b_+110); H = D;
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_COUNTER1;
  CALL_C(b_+112, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+115);
  if (!(F & FZ)) {
    CYCT(b_+115, b_+117);
    goto label_10_330;
  }
  CYC(b_+115, b_+117);
  CALL_C(b_+117, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+120);
  CYC(b_+120, b_+122); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+122, b_+124); mem_wr(gb, HL, 0x64);
  CYC(b_+124, b_+126); B = 0x14;
  CYC(b_+126, b_+128); C = 0x04;
  CYC(b_+128, b_+130); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+130, b_+131); A = mem_rd(gb, HL);
  CYC(b_+131, b_+132); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+132, b_+134);
    goto setSpeed;
  }
  CYC(b_+132, b_+134);
  CYC(b_+134, b_+136); B = 0x3c;
  CYC(b_+136, b_+138); C = 0x02;
setSpeed:
  CYC(b_+138, b_+140); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+140, b_+141); mem_wr(gb, HL, B);
  CYC(b_+141, b_+142); A = C;
  CALL_C(b_+142, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+145);
  CYC(b_+145, b_+148); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+148, b_+151); mem_wr(gb, HL, 0x02);
  RET(b_+150); return;
label_10_330:
  CYC(b_+151, b_+153); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+153, b_+154); A = mem_rd(gb, HL);
  CYC(b_+154, b_+155); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+155, interactionAnimate_hook, SYM(interactionAnimate), b_+158);
  } else {
    CYC(b_+155, b_+158);
  }
  CYC(b_+158, b_+160); L = GV(0x78, 0x77);
  CYC(b_+160, b_+161); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+161); return; }
  CYC(b_+161, b_+162);
  CYC(b_+162, b_+164); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+164, b_+165); A = mem_rd(gb, HL);
  CYC(b_+165, b_+167); alu_xor(gb, 0x01);
  CYC(b_+167, b_+168); mem_wr(gb, HL, A);
  CYC(b_+168, b_+170); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+170, b_+171); A = mem_rd(gb, DE);
  CYC(b_+171, b_+172); alu_add(gb, A);
  CYC(b_+172, b_+173); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+173, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+176);
setRandomVar38:
  CALL_C(b_+176, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+179);
  CYC(b_+179, b_+181); alu_and(gb, 0x03);
  CYC(b_+181, b_+183); A = alu_swap(gb, A);
  CYC(b_+183, b_+185); alu_add(gb, 0x20);
  CYC(b_+185, b_+187); E = GV(0x78, 0x77);
  CYC(b_+187, b_+188); mem_wr(gb, DE, A);
  RET(b_+188); return;
substate4:
  CALL_C(b_+189, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+192);
  if (!(F & FZ)) { RET_TAKEN(b_+192); return; }
  CYC(b_+192, b_+193);
  CYC(b_+193, b_+195); B = 0x78;
  CYC(b_+195, b_+197); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+197, b_+198); A = mem_rd(gb, DE);
  CYC(b_+198, b_+199); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+199, b_+201);
    goto storeCounter1_4;
  }
  CYC(b_+199, b_+201);
  CYC(b_+201, b_+203); B = 0xa0;
storeCounter1_4:
  CYC(b_+203, b_+204); mem_wr(gb, HL, B);
  CYC(b_+204, b_+207); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+207, b_+209); mem_wr(gb, HL, 0x03);
  CYC(b_+209, b_+212);
  interactionIncSubstate_hook(gb);
  return;
substate5:
  CALL_C(b_+212, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+215);
  if (!(F & FZ)) { RET_TAKEN(b_+215); return; }
  CYC(b_+215, b_+216);
  CYC(b_+216, b_+218); mem_wr(gb, HL, 0x3c);
  CYC(b_+218, b_+221); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+221, b_+223); mem_wr(gb, HL, 0x04);
  CYC(b_+223, b_+226);
  interactionIncSubstate_hook(gb);
  return;
substate6:
  CALL_C(b_+226, interactionAnimate_hook, SYM(interactionAnimate), b_+229);
  CALL_C(b_+229, objectApplySpeed_hook, SYM(objectApplySpeed), b_+232);
  CALL_C(b_+232, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+235);
  if (!(F & FZ)) { RET_TAKEN(b_+235); return; }
  CYC(b_+235, b_+236);
  CYC(b_+236, b_+239); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+239, b_+241); mem_wr(gb, HL, 0x01);
  RET(b_+241); return;
}
