#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodec9_jump_table(GB *gb) {
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

// 0b:761e, tail-jumped into once from interactionCodec9_hook (top-level).
void interactionCodec9_updateAnimation_hook(GB *gb) {
  BASE(interactionCodec9);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+25); ret_effect(gb); return; } // ret z
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+28); interactionAnimate_hook(gb); return; // jp
}

// 0b:76f0, called (real `call`) from interactionCodec9@state0, and reached by plain fallthrough
// from interactionCodec9_updateHopping_hook.
void interactionCodec9_beginHop_hook(GB *gb) {
  BASE(interactionCodec9);
  CYC(b_+230, b_+233); SET_BC(0xff40);
  CYC(b_+233, b_+236); objectSetSpeedZ_hook(gb); return; // jp
}

// 0b:76e9, called (real `call`) from interactionCodec9@state1, @state2 and @state3.
void interactionCodec9_updateHopping_hook(GB *gb) {
  BASE(interactionCodec9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+223, b_+225); C = 0x20;
  CALL_C(b_+225, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+228);
  if (!(F & FZ)) { CYCT(b_+228, b_+229); ret_effect(gb); return; } // ret nz
  CYC(b_+228, b_+229);
  CYC(b_+229, b_+230); H = D;
  interactionCodec9_beginHop_hook(gb); return; // falls through into @beginHop
}

// 0b:76f6, called (real `call`) once from interactionCodec9@state1.
void interactionCodec9_updateMovement_hook(GB *gb) {
  BASE(interactionCodec9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+236, objectApplySpeed_hook, SYM(objectApplySpeed), b_+239);
  CYC(b_+239, b_+241); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+241, b_+242); A = mem_rd(gb, DE);
  CYC(b_+242, b_+244); alu_sub(gb, 0x68);
  CYC(b_+244, b_+246); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(b_+246, b_+247); ret_effect(gb); return; } // ret c
  CYC(b_+246, b_+247);
  CYC(b_+247, b_+249); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+249, b_+250); A = mem_rd(gb, DE);
  CYC(b_+250, b_+252); alu_xor(gb, 0x10);
  CYC(b_+252, b_+253); mem_wr(gb, DE, A);
  CYC(b_+253, b_+255); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+255, b_+256); A = mem_rd(gb, DE);
  CYC(b_+256, b_+258); alu_xor(gb, 0x01);
  CYC(b_+258, b_+259); mem_wr(gb, DE, A);
  CYC(b_+259, b_+262); interactionSetAnimation_hook(gb); return; // jp
}

// 0b:768f, real jump-table target reached by `jp` from interactionCodec9@state2, and by
// tail-jump from interactionCodec9_gotoState4_hook / interactionCodec9_func_766f_hook.
void interactionCodec9_setScriptAndGotoState4_hook(GB *gb) {
  BASE(interactionCodec9);
  CYC(b_+133, b_+135); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+135, b_+137); A = 0x04;
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+141); interactionSetScript_hook(gb); return; // jp
}

// 0b:768c, unreachable in practice (only interactionCodec9_func_766f_hook's dead `jr z` branch
// targets it, and that branch's own tail-jump path bypasses it entirely; zero real callers).
void interactionCodec9_gotoState4_hook(GB *gb) {
  BASE(interactionCodec9);
  CYC(b_+130, b_+133); SET_HL((SYM(interactionCoded8__subid4Script) + 81)); // script pointer
  interactionCodec9_setScriptAndGotoState4_hook(gb); return;
}

// 0b:766f, unused (zero callers): none of interactionCodec9's states ever jump or call here.
void interactionCodec9_func_766f_hook(GB *gb) {
  BASE(interactionCodec9);
  CYC(b_+101, b_+102); alu_xor(gb, A);
  CYC(b_+102, b_+103); mem_wr(gb, DE, A);
  CYC(b_+103, b_+105); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+105, b_+106); mem_wr(gb, DE, A);
  CYC(b_+106, b_+108); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+108, b_+110); A = 0x01;
  CYC(b_+110, b_+111); mem_wr(gb, DE, A);
  CYC(b_+111, b_+114); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+114, b_+115); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+115, b_+117); interactionCodec9_gotoState4_hook(gb); return; } // jr z
  CYC(b_+115, b_+117);
  CYC(b_+117, b_+120); A = mem_rd(gb, w1Link_relatedObj2 + 1);
  CYC(b_+120, b_+121); H = A;
  CYC(b_+121, b_+123); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+123, b_+124); mem_wr(gb, DE, A);
  CYC(b_+124, b_+127); SET_HL((SYM(interactionCoded8__subid4Script) + 81)); // script pointer
  CYC(b_+127, b_+130); interactionCodec9_setScriptAndGotoState4_hook(gb); return; // jp $768f (skips @gotoState4)
}

// 0b:7610, the state dispatcher. Called (real `call`) once from interactionCodec9_hook.
void interactionCodec9_runState_hook(GB *gb) {
  BASE(interactionCodec9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (interactionCodec9_jump_table(gb));
    if (jt_ == b_+28) { goto state0; }
    else if (jt_ == b_+60) { goto state1; }
    else if (jt_ == b_+141) { goto state2; }
    else if (jt_ == b_+167) { goto state3; }
    else if (jt_ == b_+191) { goto state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CALL_C(b_+31, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+34);
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x06);
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x06);
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x19);
  CALL_C(b_+46, interactionCodec9_beginHop_hook, b_+230, b_+49);
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+51, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+54);
  CALL_C(b_+54, objectSetVisible80_hook, SYM(objectSetVisible80), b_+57);
  CYC(b_+57, b_+60); goto func_7710; // jp

state1:
  CALL_C(b_+60, interactionCodec9_updateHopping_hook, b_+223, b_+63);
  CALL_C(b_+63, interactionCodec9_updateMovement_hook, b_+236, b_+66);
  CYC(b_+66, b_+69); SET_HL(w1Link_yh);
  CYC(b_+69, b_+71); C = 0x69;
  CYC(b_+71, b_+72); B = mem_rd(gb, HL);
  CYC(b_+72, b_+74); A = 0x69;
  CYC(b_+74, b_+75); L = A;
  CYC(b_+75, b_+76); A = C;
  CYC(b_+76, b_+77); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+77, b_+78); ret_effect(gb); return; } // ret nc
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+81); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+82, b_+83); ret_effect(gb); return; } // ret z
  CYC(b_+82, b_+83);
  CYC(b_+83, b_+85); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+85, b_+87); A = 0x02;
  CYC(b_+87, b_+88); mem_wr(gb, DE, A);
  CYC(b_+88, b_+90); A = 0x80;
  CYC(b_+90, b_+93); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+93, b_+94); A = L;
  CYC(b_+94, b_+97); SET_HL(w1Link_yh);
  CYC(b_+97, b_+98); mem_wr(gb, HL, A);
  CYC(b_+98, b_+101); goto initState2; // jp

state2:
  CALL_C(b_+141, interactionCodec9_updateHopping_hook, b_+223, b_+144);
  CALL_C(b_+144, objectApplySpeed_hook, SYM(objectApplySpeed), b_+147);
  CYC(b_+147, b_+149); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); alu_sub(gb, 0x0c);
  CYC(b_+152, b_+155); SET_HL(w1Link_xh);
  CYC(b_+155, b_+156); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+156, b_+157); ret_effect(gb); return; } // ret nc
  CYC(b_+156, b_+157);
  CYC(b_+157, b_+159); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+159, b_+160); alu_xor(gb, A);
  CYC(b_+160, b_+161); mem_wr(gb, DE, A);
  CYC(b_+161, b_+164); SET_HL((SYM(interactionCoded8__subid4Script) + 81)); // script pointer
  CYC(b_+164, b_+167); interactionCodec9_setScriptAndGotoState4_hook(gb); return; // jp $768f

state3:
  CALL_C(b_+167, interactionCodec9_updateHopping_hook, b_+223, b_+170);
  CALL_C(b_+170, objectApplySpeed_hook, SYM(objectApplySpeed), b_+173);
  CYC(b_+173, b_+175); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+175, b_+176); A = mem_rd(gb, DE);
  CYC(b_+176, b_+178); alu_cp(gb, 0x78);
  if (F & FC) { CYCT(b_+178, b_+179); ret_effect(gb); return; } // ret c
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+180); alu_xor(gb, A);
  CYC(b_+180, b_+183); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+183, b_+185); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+185, b_+187); A = 0x01;
  CYC(b_+187, b_+188); mem_wr(gb, DE, A);
  CYC(b_+188, b_+191); goto func_7710; // jp

state4:
  CALL_C(b_+191, interactionRunScript_hook, SYM(interactionRunScript), b_+194);
  if (!(F & FC)) { CYCT(b_+194, b_+195); ret_effect(gb); return; } // ret nc
  CYC(b_+194, b_+195);
  CYC(b_+195, b_+197); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+197, b_+198); A = mem_rd(gb, DE);
  CYC(b_+198, b_+200); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+200, b_+202); goto beginMovingBack; } // jr z
  CYC(b_+200, b_+202);
  CYC(b_+202, b_+203); H = D;
  CYC(b_+203, b_+205); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+205, b_+207); mem_wr(gb, HL, 0x01);
  CYC(b_+207, b_+209); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+209, b_+211); mem_wr(gb, HL, 0x00);
  CYC(b_+211, b_+213); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0x01);
  CYC(b_+215, b_+216); alu_xor(gb, A);
  CYC(b_+216, b_+219); mem_wr(gb, wDisabledObjects, A);
  RET(b_+219); return;

beginMovingBack:
  CYC(b_+220, b_+223); goto initState3; // jp

func_7710:
  CYC(b_+262, b_+263); H = D;
  CYC(b_+263, b_+265); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+265, b_+267); mem_wr(gb, HL, 0x00);
  CYC(b_+267, b_+269); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+269, b_+271); mem_wr(gb, HL, 0x14);
  CYC(b_+271, b_+273); goto l_7724; // jr

initState2:
  CYC(b_+273, b_+274); H = D;
  CYC(b_+274, b_+276); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+276, b_+278); mem_wr(gb, HL, 0x02);
  CYC(b_+278, b_+280); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+280, b_+282); mem_wr(gb, HL, 0x50);
  // falls through into l_7724

l_7724:
  CYC(b_+282, b_+284); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+284, b_+286); mem_wr(gb, HL, 0x01);
  CYC(b_+286, b_+288); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+288, b_+290); mem_wr(gb, HL, 0x18);
  CYC(b_+290, b_+291); alu_xor(gb, A);
  CYC(b_+291, b_+293); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+293, b_+294); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+294, b_+295); mem_wr(gb, HL, A); // ld (hl),a
  CYC(b_+295, b_+297); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+297, b_+299); A = 0x00;
  CYC(b_+299, b_+300); mem_wr(gb, HL, A);
  CYC(b_+300, b_+303); interactionSetAnimation_hook(gb); return; // jp

initState3:
  CYC(b_+303, b_+304); H = D;
  CYC(b_+304, b_+306); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+306, b_+308); mem_wr(gb, HL, 0x03);
  CYC(b_+308, b_+310); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+310, b_+312); mem_wr(gb, HL, 0x50);
  CYC(b_+312, b_+314); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+314, b_+316); mem_wr(gb, HL, 0x01);
  CYC(b_+316, b_+318); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+318, b_+320); mem_wr(gb, HL, 0x08);
  CYC(b_+320, b_+321); alu_xor(gb, A);
  CYC(b_+321, b_+323); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+323, b_+324); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+324, b_+325); mem_wr(gb, HL, A); // ld (hl),a
  CYC(b_+325, b_+327); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+327, b_+329); A = 0x01;
  CYC(b_+329, b_+330); mem_wr(gb, HL, A);
  CYC(b_+330, b_+333); interactionSetAnimation_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_SYRUP_CUCCO
// ==================================================================================================
void interactionCodec9_hook(GB *gb) {
  BASE(interactionCodec9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionCodec9_runState_hook, b_+6, b_+3);
  CYC(b_+3, b_+6); interactionCodec9_updateAnimation_hook(gb); return; // jp
}
