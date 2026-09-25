#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/interactions/syrupCucco.s.
// INTERAC_SYRUP_CUCCO
// Variables:
// var3c: $00 normally, $01 while cucco is chastizing Link
// var3d: Animation index?
// var3e: Also an animation index?

static uint16_t syrupCucco_jump_table(GB *gb) {
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

void s_interactionCodec9_hook(GB *gb) {
  BASE(interactionCodec9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (syrupCucco_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+31) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+18); SET_BC(0x4128);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+20, b_+21); mem_wr(gb, HL, B);
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+23, b_+24); mem_wr(gb, HL, C);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x18);
  CYC(b_+28, b_+31);
  TAIL(objectSetVisible82);
state1:
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); push_effect(gb, b_+35);
  do { uint16_t jt_ = (syrupCucco_jump_table(gb));
    if (jt_ == b_+39) goto substate0;
    if (jt_ == b_+69) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+39, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+42);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto L_791d; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x08);
  CALL_C(b_+48, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+51);
L_791d:
  CALL_C(b_+51, s_objectApplySpeed, SYM(objectApplySpeed), b_+54);
L_7920:
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+59); alu_or(gb, A);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x00);
  CYC(b_+61, b_+63); A = 0x78;
  if (!(F & FZ)) CALL_C_CC(b_+63, s_playSound, SYM(playSound_b00), b_+66);
  else CYC(b_+63, b_+66);
  CYC(b_+66, b_+69);
  TAIL(interactionAnimate);
substate1:
  CALL_C(b_+69, s_objectApplySpeed, SYM(objectApplySpeed), b_+72);
  CALL_C(b_+72, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+75);
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+77, b_+78); E = L;
  CYC(b_+78, b_+79); A = mem_rd(gb, DE);
  CYC(b_+79, b_+81); alu_add(gb, 0x08);
  CYC(b_+81, b_+82); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+82, b_+84); goto L_7920; }
  CYC(b_+82, b_+84);
  CYC(b_+84, b_+86); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x00);
  CYC(b_+88, b_+91);
  TAIL(interactionDelete);
}

// interactionCode49@updateMovement
static void syrupCucco_updateMovement(GB *gb) {
  BASE(interactionCode49);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+245, s_objectApplySpeed, SYM(objectApplySpeed), b_+248);
  CYC(b_+248, b_+250); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+250, b_+251); A = mem_rd(gb, DE);
  CYC(b_+251, b_+253); alu_sub(gb, 0x68);
  CYC(b_+253, b_+255); alu_cp(gb, 0x20);
  if (F & FC) { RET_TAKEN(b_+255); return; }
  CYC(b_+255, b_+256);
  CYC(b_+256, b_+258); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+258, b_+259); A = mem_rd(gb, DE);
  CYC(b_+259, b_+261); alu_xor(gb, 0x10);
  CYC(b_+261, b_+262); mem_wr(gb, DE, A);
  CYC(b_+262, b_+264); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+264, b_+265); A = mem_rd(gb, DE);
  CYC(b_+265, b_+267); alu_xor(gb, 0x01);
  CYC(b_+267, b_+268); mem_wr(gb, DE, A);
  CYC(b_+268, b_+271);
  TAIL(interactionSetAnimation);
}

// interactionCode49@beginHop
static void syrupCucco_beginHop(GB *gb) {
  BASE(interactionCode49);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+239, b_+242); SET_BC(IO_LCDC);
  CYC(b_+242, b_+245);
  TAIL(objectSetSpeedZ);
}

// interactionCode49@updateHopping
static void syrupCucco_updateHopping(GB *gb) {
  BASE(interactionCode49);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+232, b_+234); C = 0x20;
  CALL_C(b_+234, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+237);
  if (!(F & FZ)) { RET_TAKEN(b_+237); return; }
  CYC(b_+237, b_+238);
  CYC(b_+238, b_+239); H = D;
  CYC(b_+239, b_+242); SET_BC(IO_LCDC);
  CYC(b_+242, b_+245);
  TAIL(objectSetSpeedZ);
}

// interactionCode49@runState
static void syrupCucco_runState(GB *gb) {
  BASE(interactionCode49);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (syrupCucco_jump_table(gb));
    if (jt_ == b_+32) goto state0;
    if (jt_ == b_+69) goto state1;
    if (jt_ == b_+150) goto state2;
    if (jt_ == b_+176) goto state3;
    if (jt_ == b_+200) goto state4;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+32, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+35);
  CYC(b_+35, b_+37); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+37, b_+40); TAIL(interactionDelete); }
  CYC(b_+37, b_+40);
  CYC(b_+40, b_+42); A = 0x01;
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CALL_C(b_+43, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+46);
  CYC(b_+46, b_+47); H = D;
  CYC(b_+47, b_+49); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x06);
  CYC(b_+51, b_+52); L = alu_inc8(gb, L);
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x06);
  CYC(b_+54, b_+56); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x19);
  CALL_L(b_+58, syrupCucco_beginHop, b_+61);
  CYC(b_+61, b_+63); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+63, s_objectAddToAButtonSensitiveObjectList, SYM(objectAddToAButtonSensitiveObjectList), b_+66);
  CYC(b_+66, b_+69);
  goto func_7710;
state1:
  CALL_L(b_+69, syrupCucco_updateHopping, b_+72);
  CALL_L(b_+72, syrupCucco_updateMovement, b_+75);
  CYC(b_+75, b_+78); SET_HL(w1Link_yh);
  CYC(b_+78, b_+80); C = 0x69;
  CYC(b_+80, b_+81); B = mem_rd(gb, HL);
  CYC(b_+81, b_+83); A = 0x69;
  CYC(b_+83, b_+84); L = A;
  CYC(b_+84, b_+85); A = C;
  CYC(b_+85, b_+86); alu_cp(gb, B);
  if (!(F & FC)) { RET_TAKEN(b_+86); return; }
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+90); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+90, b_+91); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+91); return; }
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+94); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+94, b_+96); A = 0x02;
  CYC(b_+96, b_+97); mem_wr(gb, DE, A);
  CYC(b_+97, b_+99); A = 0x80;
  CYC(b_+99, b_+102); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+102, b_+103); A = L;
  CYC(b_+103, b_+106); SET_HL(w1Link_yh);
  CYC(b_+106, b_+107); mem_wr(gb, HL, A);
  CYC(b_+107, b_+110);
  goto initState2;

setScriptAndGotoState4:
  CYC(b_+142, b_+144); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+144, b_+146); A = 0x04;
  CYC(b_+146, b_+147); mem_wr(gb, DE, A);
  CYC(b_+147, b_+150);
  TAIL(interactionSetScript);
state2:
  CALL_L(b_+150, syrupCucco_updateHopping, b_+153);
  CALL_C(b_+153, s_objectApplySpeed, SYM(objectApplySpeed), b_+156);
  CYC(b_+156, b_+158); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+158, b_+159); A = mem_rd(gb, DE);
  CYC(b_+159, b_+161); alu_sub(gb, 0x0c);
  CYC(b_+161, b_+164); SET_HL(w1Link_xh);
  CYC(b_+164, b_+165); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+165); return; }
  CYC(b_+165, b_+166);
  CYC(b_+166, b_+168); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+168, b_+169); alu_xor(gb, A);
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);
  CYC(b_+170, b_+173); SET_HL(SYM(syrupCuccoScript_triedToSteal));
  CYC(b_+173, b_+176);
  goto setScriptAndGotoState4;
state3:
  CALL_L(b_+176, syrupCucco_updateHopping, b_+179);
  CALL_C(b_+179, s_objectApplySpeed, SYM(objectApplySpeed), b_+182);
  CYC(b_+182, b_+184); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+184, b_+185); A = mem_rd(gb, DE);
  CYC(b_+185, b_+187); alu_cp(gb, 0x78);
  if (F & FC) { RET_TAKEN(b_+187); return; }
  CYC(b_+187, b_+188);
  CYC(b_+188, b_+189); alu_xor(gb, A);
  CYC(b_+189, b_+192); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+192, b_+194); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+194, b_+196); A = 0x01;
  CYC(b_+196, b_+197); mem_wr(gb, DE, A);
  CYC(b_+197, b_+200);
  goto func_7710;
state4:
  CALL_C(b_+200, s_interactionRunScript, SYM(interactionRunScript), b_+203);
  if (!(F & FC)) { RET_TAKEN(b_+203); return; }
  CYC(b_+203, b_+204);
  CYC(b_+204, b_+206); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+206, b_+207); A = mem_rd(gb, DE);
  CYC(b_+207, b_+209); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+209, b_+211); goto beginMovingBack; }
  CYC(b_+209, b_+211);
  CYC(b_+211, b_+212); H = D;
  CYC(b_+212, b_+214); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+214, b_+216); mem_wr(gb, HL, 0x01);
  CYC(b_+216, b_+218); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x00);
  CYC(b_+220, b_+222); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+222, b_+224); mem_wr(gb, HL, 0x01);
  CYC(b_+224, b_+225); alu_xor(gb, A);
  CYC(b_+225, b_+228); mem_wr(gb, wDisabledObjects, A);
  RET(b_+228); return;
beginMovingBack:
  CYC(b_+229, b_+232);
  goto initState3;

func_7710:
  CYC(b_+271, b_+272); H = D;
  CYC(b_+272, b_+274); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+274, b_+276); mem_wr(gb, HL, 0x00);
  CYC(b_+276, b_+278); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+278, b_+280); mem_wr(gb, HL, 0x14);
  CYC(b_+280, b_+282);
  goto L_7198;
initState2:
  CYC(b_+282, b_+283); H = D;
  CYC(b_+283, b_+285); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+285, b_+287); mem_wr(gb, HL, 0x02);
  CYC(b_+287, b_+289); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+289, b_+291); mem_wr(gb, HL, 0x50);
L_7198:
  CYC(b_+291, b_+293); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+293, b_+295); mem_wr(gb, HL, 0x01);
  CYC(b_+295, b_+297); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+297, b_+299); mem_wr(gb, HL, 0x18);
  CYC(b_+299, b_+300); alu_xor(gb, A);
  CYC(b_+300, b_+302); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+302, b_+303); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+303, b_+304); mem_wr(gb, HL, A);
  CYC(b_+304, b_+306); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+306, b_+308); A = 0x00;
  CYC(b_+308, b_+309); mem_wr(gb, HL, A);
  CYC(b_+309, b_+312);
  TAIL(interactionSetAnimation);
initState3:
  CYC(b_+312, b_+313); H = D;
  CYC(b_+313, b_+315); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+315, b_+317); mem_wr(gb, HL, 0x03);
  CYC(b_+317, b_+319); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+319, b_+321); mem_wr(gb, HL, 0x50);
  CYC(b_+321, b_+323); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+323, b_+325); mem_wr(gb, HL, 0x01);
  CYC(b_+325, b_+327); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+327, b_+329); mem_wr(gb, HL, 0x08);
  CYC(b_+329, b_+330); alu_xor(gb, A);
  CYC(b_+330, b_+332); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+332, b_+333); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+333, b_+334); mem_wr(gb, HL, A);
  CYC(b_+334, b_+336); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+336, b_+338); A = 0x01;
  CYC(b_+338, b_+339); mem_wr(gb, HL, A);
  CYC(b_+339, b_+342);
  TAIL(interactionSetAnimation);
}

void s_interactionCode49_hook(GB *gb) {
  BASE(interactionCode49);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_L(b_+0, syrupCucco_runState, b_+3);
  CYC(b_+3, b_+6);
  goto updateAnimation;

updateAnimation:
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+26); goto L_7092; }
  CYC(b_+24, b_+26);
  CALL_C(b_+26, s_interactionAnimate, SYM(interactionAnimate), b_+29);
L_7092:
  CYC(b_+29, b_+32);
  TAIL(objectSetVisible80);
}
