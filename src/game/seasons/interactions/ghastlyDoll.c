#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/ghastlyDoll.s.
// INTERAC_LON_LON_EGG

static uint16_t ghastlyDoll_jump_table(GB *gb) {
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

// INTERAC_LON_LON_EGG
void s_interactionCode94_hook(GB *gb) {
  BASE(interactionCode94);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ghastlyDoll_jump_table(gb));
    if (jt_ == b_+14) goto state0;
    if (jt_ == b_+66) goto state1;
    if (jt_ == b_+77) goto state2;
    if (jt_ == b_+134) goto state3;
    if (jt_ == b_+200) goto state4;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+19); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+19, b_+22); SET_HL(w1Link_knockbackCounter);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); A = 0x01;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CALL_C(b_+28, s_objectTakePosition, SYM(objectTakePosition), b_+31);
  CYC(b_+31, b_+34); SET_BC(0x3850);
  CALL_C(b_+34, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+37);
  CYC(b_+37, b_+39); alu_and(gb, 0x1c);
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+45); SET_BC(IO_P1);
  CALL_C(b_+45, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+48);
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x28);
  CALL_C(b_+52, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+55);
  CYC(b_+55, b_+58); A = mem_rd(gb, w1Link_visible);
  CYC(b_+58, b_+60); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+63); A = 0x57;
  CYC(b_+63, b_+66);
  TAIL(playSound_b00);
state1:
  CYC(b_+66, b_+68); C = 0x20;
  CALL_C(b_+68, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+71);
  if (!(F & FZ)) { CYCT(b_+71, b_+74); TAIL(objectApplySpeed); }
  CYC(b_+71, b_+74);
  CYC(b_+74, b_+77);
  TAIL(interactionIncState);
state2:
  CYC(b_+77, b_+80); SET_HL(w1Companion_yh);
  CYC(b_+80, b_+81); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+81, b_+82); B = A;
  CYC(b_+82, b_+83); L = alu_inc8(gb, L);
  CYC(b_+83, b_+84); C = mem_rd(gb, HL);
  CYC(b_+84, b_+87); A = mem_rd(gb, wMapleState);
  CYC(b_+87, b_+89); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(b_+89, b_+91); goto L_53ad; }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  CYC(b_+94, b_+95); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+95, b_+97); goto L_53ad; }
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+99); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+99, b_+100); A = mem_rd(gb, DE);
  CYC(b_+100, b_+101); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+101, b_+103); goto func_53b6; }
  CYC(b_+101, b_+103);
L_53ad:
  CALL_C(b_+103, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+106);
  CYC(b_+106, b_+108); alu_xor(gb, 0x10);
  CYC(b_+108, b_+111); mem_wr(gb, w1Companion_angle, A);
  RET(b_+111); return;
func_53b6:
  CYC(b_+112, b_+114); A = 0xff;
  CYC(b_+114, b_+117); mem_wr(gb, w1Companion_angle, A);
  CALL_C(b_+117, s_interactionIncState, SYM(interactionIncState), b_+120);
  CALL_C(b_+120, s_objectSetInvisible, SYM(objectSetInvisible), b_+123);
  CYC(b_+123, b_+125); A = 0x5e;
  CALL_C(b_+125, s_playSound, SYM(playSound_b00), b_+128);
  CYC(b_+128, b_+131); SET_BC(0x070a);
  CYC(b_+131, b_+134);
  TAIL(showText);
state3:
  CYC(b_+134, b_+136); A = 0x04;
  CYC(b_+136, b_+139); mem_wr(gb, wLinkForceState, A);
  CYC(b_+139, b_+141); A = 0x01;
  CYC(b_+141, b_+144); mem_wr(gb, wcc50, A);
  CYC(b_+144, b_+147); SET_HL(w1Link_yh);
  CYC(b_+147, b_+150); SET_BC(0xf200);
  CALL_C(b_+150, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+153);
  CALL_C(b_+153, s_interactionIncState, SYM(interactionIncState), b_+156);
  CYC(b_+156, b_+158); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+158, b_+160); mem_wr(gb, HL, 0x40);
  CYC(b_+160, b_+162); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_and(gb, 0xf8);
  CYC(b_+165, b_+166); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+166, b_+167); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+167, b_+168); A = mem_rd(gb, HL);
  CYC(b_+168, b_+170); alu_add(gb, 0x02);
  CYC(b_+170, b_+171); mem_wr(gb, HL, A);
  CYC(b_+171, b_+173); A = 0x03;
  CALL_C(b_+173, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+176);
  CYC(b_+176, b_+179); A = mem_rd(gb, w1Link_visible);
  CYC(b_+179, b_+181); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+181, b_+182); mem_wr(gb, DE, A);
  CYC(b_+182, b_+185); SET_BC(0x005c);
  CALL_C(b_+185, s_showText, SYM(showText), b_+188);
  CYC(b_+188, b_+190); A = 0x41;
  CYC(b_+190, b_+192); C = 0x02;
  CALL_C(b_+192, s_giveTreasure, SYM(giveTreasure), b_+195);
  CYC(b_+195, b_+197); A = 0x4c;
  CYC(b_+197, b_+200);
  TAIL(playSound_b00);
state4:
  CALL_C(b_+200, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+203);
  if (!(F & FZ)) { RET_TAKEN(b_+203); return; }
  CYC(b_+203, b_+204);
  CYC(b_+204, b_+205); alu_xor(gb, A);
  CYC(b_+205, b_+208); mem_wr(gb, wTextIsActive, A);
  CYC(b_+208, b_+211); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+211, b_+213); A = 0x02;
  CYC(b_+213, b_+216); mem_wr(gb, w1Companion_substate, A);
  CYC(b_+216, b_+219);
  TAIL(interactionDelete);
}

