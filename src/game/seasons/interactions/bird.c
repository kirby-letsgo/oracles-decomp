#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/bird.s.
// INTERAC_BIRD

static uint16_t bird_jump_table(GB *gb) {
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

// interactionCode2a@decVar36
static void bird_decVar36(GB *gb) {
  BASE(interactionCode2a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+343, b_+344); H = D;
  CYC(b_+344, b_+346); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+346, b_+347); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+347); return;
}

// interactionCode2a@func_5de5
static void bird_func_5de5(GB *gb) {
  BASE(interactionCode2a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+330, b_+332); C = 0x10;
  CALL_C(b_+332, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+335);
  if (!(F & FZ)) { RET_TAKEN(b_+335); return; }
  CYC(b_+335, b_+336);
  CYC(b_+336, b_+337); H = D;
  CYC(b_+337, b_+340); SET_BC(0xfec0);
  CYC(b_+340, b_+343);
  TAIL(objectSetSpeedZ);
}

// INTERAC_BIRD
void s_interactionCode2a_hook(GB *gb) {
  BASE(interactionCode2a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+16, b_+18); goto birdWithImpa; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto knowItAllBird; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = 0x22;
  CALL_C(b_+24, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+27);
  if (F & FZ) { CYCT(b_+27, b_+30); TAIL(interactionDelete); }
  CYC(b_+27, b_+30);
  CYC(b_+30, b_+32); A = 0x23;
  CALL_C(b_+32, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+35);
  if (!(F & FZ)) { CYCT(b_+35, b_+38); TAIL(interactionDelete); }
  CYC(b_+35, b_+38);
  CYC(b_+38, b_+41); SET_HL(SYM(panickingBirdScript));
  CYC(b_+41, b_+43);
  goto setScript;
knowItAllBird:
  CYC(b_+43, b_+46); SET_HL(SYM(knowItAllBirdScript));
setScript:
  CALL_C(b_+46, s_interactionSetScript, SYM(interactionSetScript), b_+49);
  CALL_C(b_+49, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+52);
  CYC(b_+52, b_+54); alu_and(gb, 0x01);
  CYC(b_+54, b_+56); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CALL_C(b_+57, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+60);
  CALL_C(b_+60, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+63);
  CYC(b_+63, b_+65); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+67, s_beginJump_hook, SYM(beginJump), b_+70);
  CYC(b_+70, b_+72); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+72, b_+73); A = mem_rd(gb, HL);
  CYC(b_+73, b_+75); L = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+75, b_+76); mem_wr(gb, HL, A);
  CYC(b_+76, b_+78); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x32);
  CYC(b_+80, b_+83);
  TAIL(objectSetVisible82);
birdWithImpa:
  CALL_C(b_+83, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+86);
  CYC(b_+86, b_+88); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0xb4);
  CYC(b_+90, b_+92); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x19);
  CALL_C(b_+94, s_beginJump_hook, SYM(beginJump), b_+97);
  CALL_C(b_+97, s_objectSetVisible82, SYM(objectSetVisible82), b_+100);
  CYC(b_+100, b_+103);
  TAIL(objectSetInvisible);
state1:
  CYC(b_+103, b_+105); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+105, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+108); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+108, b_+110); goto panickingBirdState1; }
  CYC(b_+108, b_+110);
  CALL_C(b_+110, s_interactionRunScript, SYM(interactionRunScript), b_+113);
  CYC(b_+113, b_+115); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+117); push_effect(gb, b_+117);
  do { uint16_t jt_ = (bird_jump_table(gb));
    if (jt_ == b_+121) goto substate0;
    if (jt_ == b_+166) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+121, b_+123); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+125); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+125, b_+127); goto label_10_337; }
  CYC(b_+125, b_+127);
  CALL_C(b_+127, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+130);
  CYC(b_+130, b_+132); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+132, b_+133); A = mem_rd(gb, HL);
  CYC(b_+133, b_+135); alu_add(gb, 0x02);
  CYC(b_+135, b_+138);
  TAIL(interactionSetAnimation);
label_10_337:
  CALL_L(b_+138, bird_decVar36, b_+141);
  if (!(F & FZ)) { CYCT(b_+141, b_+143); goto animate; }
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+145); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+145, b_+147); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+147, s_getRandomNumber, SYM(getRandomNumber), b_+150);
  CYC(b_+150, b_+152); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+152, b_+154); goto animate; }
  CYC(b_+152, b_+154);
  CYC(b_+154, b_+156); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+156, b_+157); A = mem_rd(gb, HL);
  CYC(b_+157, b_+159); alu_xor(gb, 0x01);
  CYC(b_+159, b_+160); mem_wr(gb, HL, A);
  CYC(b_+160, b_+163);
  TAIL(interactionSetAnimation);
animate:
  CYC(b_+163, b_+166);
  TAIL(interactionAnimateAsNpc);
substate1:
  CALL_C(b_+166, s_interactionAnimate, SYM(interactionAnimate), b_+169);
  CYC(b_+169, b_+171); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+171, b_+172); A = mem_rd(gb, DE);
  CYC(b_+172, b_+173); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+173, b_+176); TAIL_S(updateSpeedZ); }
  CYC(b_+173, b_+176);
  CYC(b_+176, b_+178); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+178, b_+180); mem_wr(gb, HL, 0x3c);
  CYC(b_+180, b_+182); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+182, b_+183); mem_wr(gb, HL, A);
  CYC(b_+183, b_+185); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+185, b_+186); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+186, b_+187); mem_wr(gb, HL, A);
  CYC(b_+187, b_+189); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+189, b_+190); A = mem_rd(gb, HL);
  CYC(b_+190, b_+193);
  TAIL(interactionSetAnimation);
panickingBirdState1:
  CYC(b_+193, b_+195); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+195, b_+196); A = mem_rd(gb, DE);
  CYC(b_+196, b_+197); push_effect(gb, b_+197);
  do { uint16_t jt_ = (bird_jump_table(gb));
    if (jt_ == b_+207) goto panickingBirdSubstate0;
    if (jt_ == b_+225) goto panickingBirdSubstate1;
    if (jt_ == b_+262) goto panickingBirdSubstate2;
    if (jt_ == b_+280) goto panickingBirdSubstate3;
    if (jt_ == b_+307) goto panickingBirdSubstate4;
    HANDOFF(HL);
  } while (0);
panickingBirdSubstate0:
  CYC(b_+207, b_+210); A = mem_rd(gb, wUseSimulatedInput);
  CYC(b_+210, b_+211); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+211); return; }
  CYC(b_+211, b_+212);
  CALL_C(b_+212, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+215);
  if (!(F & FZ)) { RET_TAKEN(b_+215); return; }
  CYC(b_+215, b_+216);
  CYC(b_+216, b_+218); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+218, b_+219); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+219, s_func_5e04_hook, SYM(func_5e04), b_+222);
  CYC(b_+222, b_+225);
  TAIL(objectSetVisible);
panickingBirdSubstate1:
  CALL_C(b_+225, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+228);
  CALL_C(b_+228, s_updateSpeedZ_hook, SYM(updateSpeedZ), b_+231);
  CYC(b_+231, b_+234); A = mem_rd(gb, wFrameCounter);
  CYC(b_+234, b_+236); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(b_+236, s_func_5e04_hook, SYM(func_5e04), b_+239);
  else CYC(b_+236, b_+239);
  CYC(b_+239, b_+241); C = 0x10;
  CALL_C(b_+241, s_func_5e22_hook, SYM(func_5e22), b_+244);
  if (!(F & FC)) { CYCT(b_+244, b_+247); TAIL(objectApplySpeed); }
  CYC(b_+244, b_+247);
  CYC(b_+247, b_+248); H = D;
  CYC(b_+248, b_+250); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+250, b_+251); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+251, b_+253); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+253, b_+255); mem_wr(gb, HL, 0x14);
  CYC(b_+255, b_+257); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+257, b_+259); mem_wr(gb, HL, 0x00);
  CYC(b_+259, b_+262);
  TAIL_S(beginJump);
panickingBirdSubstate2:
  CALL_C(b_+262, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+265);
  CALL_C(b_+265, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+268);
  if (!(F & FZ)) { RET_TAKEN(b_+268); return; }
  CYC(b_+268, b_+269);
  CYC(b_+269, b_+271); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+271, b_+272); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+272, b_+274); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+274, b_+275); A = mem_rd(gb, HL);
  CYC(b_+275, b_+277); alu_add(gb, 0x02);
  CYC(b_+277, b_+280);
  TAIL(interactionSetAnimation);
panickingBirdSubstate3:
  CALL_C(b_+280, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+283);
  CALL_L(b_+283, bird_func_5de5, b_+286);
  CYC(b_+286, b_+288); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+288, b_+289); A = mem_rd(gb, DE);
  CYC(b_+289, b_+290); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+290); return; }
  CYC(b_+290, b_+291);
  CYC(b_+291, b_+293); C = 0x18;
  CALL_C(b_+293, s_func_5e22_hook, SYM(func_5e22), b_+296);
  if (F & FC) { RET_TAKEN(b_+296); return; }
  CYC(b_+296, b_+297);
  CYC(b_+297, b_+298); H = D;
  CYC(b_+298, b_+300); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+300, b_+301); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+301, s_beginJump_hook, SYM(beginJump), b_+304);
  CYC(b_+304, b_+307);
  TAIL_S(func_5e04);
panickingBirdSubstate4:
  CALL_C(b_+307, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+310);
  CALL_C(b_+310, s_updateSpeedZ_hook, SYM(updateSpeedZ), b_+313);
  CYC(b_+313, b_+316); A = mem_rd(gb, wFrameCounter);
  CYC(b_+316, b_+318); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(b_+318, s_func_5e04_hook, SYM(func_5e04), b_+321);
  else CYC(b_+318, b_+321);
  CALL_C(b_+321, s_objectApplySpeed, SYM(objectApplySpeed), b_+324);
  CALL_C(b_+324, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+327);
  if (!(F & FC)) { CYCT(b_+327, b_+330); TAIL(interactionDelete); }
  CYC(b_+327, b_+330);
  CYC(b_+330, b_+332); C = 0x10;
  CALL_C(b_+332, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+335);
  if (!(F & FZ)) { RET_TAKEN(b_+335); return; }
  CYC(b_+335, b_+336);
  CYC(b_+336, b_+337); H = D;
  CYC(b_+337, b_+340); SET_BC(0xfec0);
  CYC(b_+340, b_+343);
  TAIL(objectSetSpeedZ);
}

void s_updateSpeedZ_hook(GB *gb) {
  BASE(updateSpeedZ);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  s_beginJump_hook(gb); return; // falls through
}

void s_beginJump_hook(GB *gb) {
  BASE(beginJump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(IO_LCDC);
  CYC(b_+3, b_+6);
  TAIL(objectSetSpeedZ);
}

void s_func_5e04_hook(GB *gb) {
  BASE(func_5e04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+3);
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); C = mem_rd(gb, HL);
  CALL_C(b_+9, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+12);
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); alu_and(gb, 0x10);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
  CYC(b_+19, b_+21); alu_xor(gb, 0x01);
  CYC(b_+21, b_+22); H = D;
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+24, b_+25); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+30);
  TAIL(interactionSetAnimation);
}

void s_func_5e22_hook(GB *gb) {
  BASE(func_5e22);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CALL_C(b_+4, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+7);
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_sub(gb, B);
  CYC(b_+11, b_+12); alu_cp(gb, C);
  RET(b_+12); return;
}

