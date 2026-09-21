#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t partCode52_jump_table(GB *gb) {
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

void partCode52_hook(GB *gb) {
  BASE(partCode52);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x0e);
  if (F & FZ) { CYCT(b_+8, b_+11); partDelete_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = 0xc2; // Part.subid
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); E = 0xc4; // Part.state
  {
    CYC(b_+16, b_+17); push_effect(gb, b_+17);
    uint16_t target = partCode52_jump_table(gb);
    if (target == b_+23) goto subid0;
    if (target == b_+70) goto subid1;
    goto subid2;
  }

subid0:
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  {
    CYC(b_+24, b_+25); push_effect(gb, b_+25);
    uint16_t target = partCode52_jump_table(gb);
    if (target == b_+31) goto subid0_state0;
    if (target == b_+41) goto subid0_state1;
    goto subid0_state2;
  }

subid0_state0:
  CYC(b_+31, b_+32); H = D;
  CYC(b_+32, b_+33); L = E;
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+34, b_+36); L = 0xc6; // Part.counter1
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x0a);
  CYC(b_+38, b_+41); TAIL(objectSetVisible82); // jp

subid0_state1:
  CALL_C(b_+41, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); goto L_5c9f; } // jr nz
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); L = E;
  CYC(b_+47, b_+48); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+48, b_+50); A = 0xa4; // SND_BEAM
  CALL_C(b_+50, playSound_b00_hook, SYM(playSound_b00), b_+53);
  CYC(b_+53, b_+55); A = 0x02;
  CALL_C(b_+55, partSetAnimation_hook, SYM(partSetAnimation), b_+58);

subid0_state2:
  CALL_C(b_+58, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+61);
  if (F & FZ) { CYCT(b_+61, b_+64); partDelete_hook(gb); return; } // jp z
  CYC(b_+61, b_+64);
  CALL_C(b_+64, objectApplySpeed_hook, SYM(objectApplySpeed), b_+67);

L_5c9f:
  CYC(b_+67, b_+70); TAIL(partAnimate); // jp

subid1:
  CYC(b_+70, b_+71); A = mem_rd(gb, DE);
  {
    CYC(b_+71, b_+72); push_effect(gb, b_+72);
    uint16_t target = partCode52_jump_table(gb);
    if (target == b_+80) goto subid1_state0;
    if (target == b_+132) goto subid1_state1;
    if (target == b_+181) goto subid1_state2;
    goto subid0_state2;
  }

subid1_state0:
  CYC(b_+80, b_+81); H = D;
  CYC(b_+81, b_+83); L = 0xd0; // Part.speed
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x50);
  CYC(b_+85, b_+86); L = E;
  CALL_C(b_+86, objectSetVisible82_hook, SYM(objectSetVisible82), b_+89);
  CYC(b_+89, b_+91); E = 0xc3; // Part.var03
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+93); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+93, b_+95); goto L_5cc4; } // jr z
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x03);
  CYC(b_+97, b_+99); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+99, b_+101); A = 0x02;
  CYC(b_+101, b_+102); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+102, b_+103); mem_wr(gb, HL, A);
  RET(b_+103); return; // ret

L_5cc4:
  CYC(b_+104, b_+105); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+105, b_+107); L = 0xc6; // Part.counter1
  CYC(b_+107, b_+109); mem_wr(gb, HL, 0x28);
  CYC(b_+109, b_+111); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+111, b_+113); A = 0x04;
  CYC(b_+113, b_+114); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+114, b_+115); mem_wr(gb, HL, A);
  CYC(b_+115, b_+117); E = 0xcb; // Part.yh
  CYC(b_+117, b_+119); L = 0xf0; // Part.var30
  CYC(b_+119, b_+120); A = mem_rd(gb, DE);
  CYC(b_+120, b_+122); alu_add(gb, 0x20);
  CYC(b_+122, b_+123); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+123, b_+125); E = 0xcd; // Part.xh
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+127); mem_wr(gb, HL, A);
  CYC(b_+127, b_+129); A = 0x01;
  CALL_C(b_+129, partSetAnimation_hook, SYM(partSetAnimation), b_+132);

subid1_state1:
  CALL_C(b_+132, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+135);
  if (F & FZ) { CYCT(b_+135, b_+137); goto L_5d02; } // jr z
  CYC(b_+135, b_+137);
  CYC(b_+137, b_+138); A = mem_rd(gb, HL);
  CYC(b_+138, b_+139); alu_rrca(gb);
  CYC(b_+139, b_+141); E = 0xc9; // Part.angle
  if (F & FC) { CYCT(b_+141, b_+143); goto L_5cf0; } // jr c
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+144); A = mem_rd(gb, DE);
  CYC(b_+144, b_+145); A = alu_inc8(gb, A);
  CYC(b_+145, b_+147); alu_and(gb, 0x1f);
  CYC(b_+147, b_+148); mem_wr(gb, DE, A);

L_5cf0:
  CYC(b_+148, b_+150); L = 0xda; // Part.visible
  CYC(b_+150, b_+151); A = mem_rd(gb, HL);
  CYC(b_+151, b_+153); alu_xor(gb, 0x80);
  CYC(b_+153, b_+154); mem_wr(gb, HL, A);
  CYC(b_+154, b_+156); L = 0xf0; // Part.var30
  CYC(b_+156, b_+157); B = mem_rd(gb, HL);
  CYC(b_+157, b_+158); L = alu_inc8(gb, L);
  CYC(b_+158, b_+159); C = mem_rd(gb, HL);
  CYC(b_+159, b_+161); A = 0x08;
  CALL_C(b_+161, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+164);
  CYC(b_+164, b_+166); goto subid1_animate; // jr

L_5d02:
  CYC(b_+166, b_+168); mem_wr(gb, HL, 0x0a);
  CYC(b_+168, b_+169); L = E;
  CYC(b_+169, b_+170); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+170, b_+172); A = 0xbe; // SND_VERAN_PROJECTILE
  CALL_C(b_+172, playSound_b00_hook, SYM(playSound_b00), b_+175);
  CALL_C(b_+175, objectSetVisible82_hook, SYM(objectSetVisible82), b_+178);

subid1_animate:
  CYC(b_+178, b_+181); TAIL(partAnimate); // jp

subid1_state2:
  CALL_C(b_+181, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+184);
  if (F & FZ) { CYCT(b_+184, b_+186); goto L_5d1b; } // jr z
  CYC(b_+184, b_+186);
  CALL_C(b_+186, objectApplySpeed_hook, SYM(objectApplySpeed), b_+189);
  CYC(b_+189, b_+191); goto subid1_animate; // jr

L_5d1b:
  CYC(b_+191, b_+192); L = E;
  CYC(b_+192, b_+193); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+193, b_+195); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+195, b_+197); A = 0x02;
  CYC(b_+197, b_+198); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+198, b_+199); mem_wr(gb, HL, A);
  CYC(b_+199, b_+200); alu_xor(gb, A);
  CALL_C(b_+200, partSetAnimation_hook, SYM(partSetAnimation), b_+203);
  CALL_C(b_+203, objectCreatePuff_hook, SYM(objectCreatePuff), b_+206);
  CYC(b_+206, b_+208); B = 0xfd;
  CYC(b_+208, b_+211); push_effect(gb, b_+211); goto func_5d31_call1; // call $5d31

func_5d31_call1:
  CALL_C(b_+213, getFreePartSlot_hook, SYM(getFreePartSlot), b_+216);
  if (!(F & FZ)) { CYCT(b_+216, b_+217); ret_effect(gb); goto resume_5d2f; } // ret nz
  CYC(b_+216, b_+217);
  CYC(b_+217, b_+219); mem_wr(gb, HL, 0x52); // PART_52
  CYC(b_+219, b_+220); L = alu_inc8(gb, L);
  CYC(b_+220, b_+221); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+221, b_+222); L = alu_inc8(gb, L);
  CYC(b_+222, b_+223); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+223, b_+225); L = 0xc9; // Part.angle
  CYC(b_+225, b_+226); E = L;
  CYC(b_+226, b_+227); A = mem_rd(gb, DE);
  CYC(b_+227, b_+228); alu_add(gb, B);
  CYC(b_+228, b_+230); alu_and(gb, 0x1f);
  CYC(b_+230, b_+231); mem_wr(gb, HL, A);
  CYC(b_+231, b_+234); objectCopyPosition_hook(gb); goto resume_5d2f; // jp

resume_5d2f:
  CYC(b_+211, b_+213); B = 0x03;

func_5d31_call2:
  CALL_C(b_+213, getFreePartSlot_hook, SYM(getFreePartSlot), b_+216);
  if (!(F & FZ)) { RET_TAKEN(b_+216); return; } // ret nz
  CYC(b_+216, b_+217);
  CYC(b_+217, b_+219); mem_wr(gb, HL, 0x52); // PART_52
  CYC(b_+219, b_+220); L = alu_inc8(gb, L);
  CYC(b_+220, b_+221); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+221, b_+222); L = alu_inc8(gb, L);
  CYC(b_+222, b_+223); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+223, b_+225); L = 0xc9; // Part.angle
  CYC(b_+225, b_+226); E = L;
  CYC(b_+226, b_+227); A = mem_rd(gb, DE);
  CYC(b_+227, b_+228); alu_add(gb, B);
  CYC(b_+228, b_+230); alu_and(gb, 0x1f);
  CYC(b_+230, b_+231); mem_wr(gb, HL, A);
  CYC(b_+231, b_+234); TAIL(objectCopyPosition); // jp

subid2:
  CYC(b_+234, b_+235); A = mem_rd(gb, DE);
  {
    CYC(b_+235, b_+236); push_effect(gb, b_+236);
    uint16_t target = partCode52_jump_table(gb);
    if (target == b_+244) goto subid2_state0;
    if (target == b_+254) goto subid2_state1;
    if (target == b_+274) goto subid2_state2;
    goto subid0_state2;
  }

subid2_state0:
  CYC(b_+244, b_+245); H = D;
  CYC(b_+245, b_+246); L = E;
  CYC(b_+246, b_+247); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+247, b_+249); L = 0xc6; // Part.counter1
  CYC(b_+249, b_+251); mem_wr(gb, HL, 0x0f);
  CYC(b_+251, b_+254); TAIL(objectSetVisible82); // jp

subid2_state1:
  CALL_C(b_+254, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+257);
  if (!(F & FZ)) { CYCT(b_+257, b_+260); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+257, b_+260);
  CYC(b_+260, b_+262); mem_wr(gb, HL, 0x0f);
  CYC(b_+262, b_+263); L = E;
  CYC(b_+263, b_+264); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+264, b_+266); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CALL_C(b_+266, playSound_b00_hook, SYM(playSound_b00), b_+269);
  CYC(b_+269, b_+271); A = 0x01;
  CYC(b_+271, b_+274); TAIL(partSetAnimation); // jp

subid2_state2:
  CALL_C(b_+274, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+277);
  if (!(F & FZ)) { CYCT(b_+277, b_+280); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+277, b_+280);
  CYC(b_+280, b_+281); L = E;
  CYC(b_+281, b_+282); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+282, b_+284); L = 0xd0; // Part.speed
  CYC(b_+284, b_+286); mem_wr(gb, HL, 0x5a);
  CALL_C(b_+286, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+289);
  CYC(b_+289, b_+291); E = 0xc9; // Part.angle
  CYC(b_+291, b_+292); mem_wr(gb, DE, A);
  CYC(b_+292, b_+294); A = 0x02;
  CYC(b_+294, b_+297); TAIL(partSetAnimation); // jp
}
