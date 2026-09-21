#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void sidescrollingPlatformCommon_hook(GB *gb);
void sidescrollPlatform_checkLinkSquished_hook(GB *gb);
void sidescrollPlatform_checkLinkIsClose_hook(GB *gb);
void sidescrollPlatform_getTileCollisionBehindLink_hook(GB *gb);
void sidescrollPlatformFunc_5b51_hook(GB *gb);
void sidescrollPlatform_checkLinkOnPlatform_hook(GB *gb);
void sidescrollPlatform_updateLinkKnockbackForConveyor_hook(GB *gb);
void sidescrollPlatform_decCounter1_hook(GB *gb);
void sidescrollPlatform_pushLinkAwayVertical_hook(GB *gb);
void sidescrollPlatform_pushLinkAwayHorizontal_hook(GB *gb);
void sidescrollPlatformFunc_5bfc_hook(GB *gb);
void sidescrollPlatform_updateLinkSubpixels_hook(GB *gb);

static uint16_t interactionCodea4_jump_table(GB *gb) {
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

static void interactionCodea4_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

// 0b:5a67. Reached both by a genuine `call` from @state0 and by plain fallthrough from the end of
// @state1 (no jump instruction at all) -- a plain function works for both.
void interactionCodea4_func_5a67_hook(GB *gb) {
  BASE(interactionCodea4);
  CYC(b_+121, b_+122); H = D;
  CYC(b_+122, b_+124); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+124, b_+126); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CYC(b_+127, b_+128); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+128, b_+130); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+132); mem_wr(gb, HL, A);
  RET(b_+132); return;
}

// 0b:59f7, called once from interactionCodea4_hook.
void interactionCodea4_updateState_hook(GB *gb) {
  BASE(interactionCodea4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (interactionCodea4_jump_table(gb));
    if (jt_ == b_+17) { goto state0; }
    else if (jt_ == b_+74) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+17, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+26, b_+28); A = 0x08;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0xc0); // SPEED_c0
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x07);
  CYC(b_+38, b_+40); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+44); SET_HL(b_+71); // @angles
  CYC(b_+44, b_+45); interactionCodea4_add_a_to_hl(gb); // rst $10
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+52); SET_BC((SYM(interactionCode9e__subid0Substate5) + 2));
  CYC(b_+52, b_+54); A = 0x35;
  CALL_C(b_+54, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+57);
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+62); alu_add(gb, 0x08);
  CYC(b_+62, b_+64); alu_and(gb, 0x1f);
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);
  CALL_L(b_+65, interactionCodea4_func_5a67_hook, b_+68);
  CYC(b_+68, b_+71); TAIL(objectSetVisible82); // jp

  // @angles (0b:5a35): pure ROM data (ANGLE_UP, ANGLE_RIGHT, ANGLE_DOWN), referenced only via
  // SET_HL(0x5a35) above.

state1:
  CALL_C(b_+74, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+77);
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto l_5a46; } // jr nz
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x0e);
  CYC(b_+81, b_+83); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+83, b_+84); A = mem_rd(gb, HL);
  CYC(b_+84, b_+85); A = alu_inc8(gb, A);
  CYC(b_+85, b_+87); alu_and(gb, 0x1f);
  CYC(b_+87, b_+88); mem_wr(gb, HL, A);

l_5a46:
  CALL_C(b_+88, objectApplySpeed_hook, SYM(objectApplySpeed), b_+91);
  CYC(b_+91, b_+93); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  CYC(b_+94, b_+95); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+95, b_+97); interactionCodea4_func_5a67_hook(gb); return; } // jr z
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+98); H = D;
  CYC(b_+98, b_+100); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  CYC(b_+103, b_+104); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+104, b_+105); B = A;
  CYC(b_+105, b_+106); L = alu_inc8(gb, L);
  CYC(b_+106, b_+108); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+108, b_+109); A = mem_rd(gb, DE);
  CYC(b_+109, b_+110); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+110, b_+111); C = A;
  CYC(b_+111, b_+114); SET_HL(w1Link_yh);
  CYC(b_+114, b_+115); A = mem_rd(gb, HL);
  CYC(b_+115, b_+116); alu_add(gb, B);
  CYC(b_+116, b_+117); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+117, b_+118); L = alu_inc8(gb, L);
  CYC(b_+118, b_+119); A = mem_rd(gb, HL);
  CYC(b_+119, b_+120); alu_add(gb, C);
  CYC(b_+120, b_+121); mem_wr(gb, HL, A);
  interactionCodea4_func_5a67_hook(gb); // falls through (no jump at all)
}

// ==================================================================================================
// INTERAC_CIRCULAR_SIDESCROLL_PLATFORM
// ==================================================================================================
void interactionCodea4_hook(GB *gb) {
  BASE(interactionCodea4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, sidescrollPlatform_checkLinkOnPlatform_hook, SYM(sidescrollPlatform_checkLinkOnPlatform), b_+3);
  CALL_L(b_+3, interactionCodea4_updateState_hook, b_+6);
  CYC(b_+6, b_+9); TAIL(sidescrollingPlatformCommon); // jp
}

//;;
// Used by:
// * INTERAC_MOVING_SIDESCROLL_PLATFORM
// * INTERAC_MOVING_SIDESCROLL_CONVEYOR
// * INTERAC_DISAPPEARING_SIDESCROLL_PLATFORM
// * INTERAC_CIRCULAR_SIDESCROLL_PLATFORM
void sidescrollingPlatformCommon_hook(GB *gb) {
  BASE(sidescrollingPlatformCommon);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_state);
  CYC(b_+3, b_+5); alu_cp(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } // ret nz
  CYC(b_+5, b_+6);
  CALL_C(b_+6, objectCheckCollidedWithLink_hook, SYM(objectCheckCollidedWithLink), b_+9);
  if (!(F & FC)) { CYCT(b_+9, b_+10); ret_effect(gb); return; } // ret nc
  CYC(b_+9, b_+10);
  CALL_C(b_+10, sidescrollPlatform_checkLinkIsClose_hook, SYM(sidescrollPlatform_checkLinkIsClose), b_+13);
  if (F & FC) { CYCT(b_+13, b_+15); goto label_0b_183; } // jr c
  CYC(b_+13, b_+15);
  CALL_C(b_+15, sidescrollPlatform_getTileCollisionBehindLink_hook, SYM(sidescrollPlatform_getTileCollisionBehindLink), b_+18);
  if (F & FZ) { CYCT(b_+18, b_+21); sidescrollPlatform_pushLinkAwayHorizontal_hook(gb); return; } // jp z
  CYC(b_+18, b_+21);
  CALL_C(b_+21, sidescrollPlatform_checkLinkSquished_hook, SYM(sidescrollPlatform_checkLinkSquished), b_+24);
  if (F & FC) { CYCT(b_+24, b_+25); ret_effect(gb); return; } // ret c
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+32); A = mem_rd(gb, w1Link_yh);
  CYC(b_+32, b_+33); alu_cp(gb, B);
  CYC(b_+33, b_+35); C = 0x00; // ANGLE_UP
  if (!(F & FC)) { CYCT(b_+35, b_+37); goto moveLinkAtAngle; } // jr nc
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); C = 0x10; // ANGLE_DOWN
  CYC(b_+39, b_+41); goto moveLinkAtAngle; // jr (unconditional)

label_0b_183:
  CALL_C(b_+41, sidescrollPlatformFunc_5b51_hook, SYM(sidescrollPlatformFunc_5b51), b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+46, b_+49); sidescrollPlatform_pushLinkAwayVertical_hook(gb); return; } // jp z
  CYC(b_+46, b_+49);
  CALL_C(b_+49, sidescrollPlatform_checkLinkSquished_hook, SYM(sidescrollPlatform_checkLinkSquished), b_+52);
  if (F & FC) { CYCT(b_+52, b_+53); ret_effect(gb); return; } // ret c
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+56); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+56, b_+57); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(b_+57, b_+59); goto label_0b_184; } // jr nz
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); A = mem_rd(gb, hFF8B);
  CYC(b_+61, b_+63); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+63, b_+65); goto label_0b_184; } // jr z
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+66); push_effect(gb, AF);
  CALL_C(b_+66, sidescrollPlatform_pushLinkAwayVertical_hook, SYM(sidescrollPlatform_pushLinkAwayVertical), b_+69);
  CYC(b_+69, b_+70); SET_AF(pop_effect(gb));
  CYC(b_+70, b_+71); alu_rrca(gb);
  CYC(b_+71, b_+73); goto l_5ac4; // jr (unconditional)

label_0b_184:
  CYC(b_+73, b_+75); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+75, b_+76); A = mem_rd(gb, DE);
  CYC(b_+76, b_+77); B = A;
  CYC(b_+77, b_+80); A = mem_rd(gb, w1Link_xh);
  CYC(b_+80, b_+81); alu_cp(gb, B);

l_5ac4:
  CYC(b_+81, b_+83); C = 0x08; // ANGLE_RIGHT
  if (!(F & FC)) { CYCT(b_+83, b_+85); goto moveLinkAtAngle; } // jr nc
  CYC(b_+83, b_+85);
  CYC(b_+85, b_+87); C = 0x18; // ANGLE_LEFT

moveLinkAtAngle:
  CYC(b_+87, b_+89); B = 0x14; // SPEED_80
  CYC(b_+89, b_+92); TAIL(updateLinkPositionGivenVelocity); // jp
}

// @param[out] cflag c if Link got squished
void sidescrollPlatform_checkLinkSquished_hook(GB *gb) {
  BASE(sidescrollPlatform_checkLinkSquished);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); alu_add(gb, A);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+10, b_+13); A = mem_rd(gb, w1Link_yh);
  CYC(b_+13, b_+14); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+15); alu_add(gb, B);
  CYC(b_+15, b_+16); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+16, b_+17); ret_effect(gb); return; } // ret nc
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_add(gb, 0x02);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+24); alu_add(gb, A);
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  CYC(b_+25, b_+26); C = A;
  CYC(b_+26, b_+28); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+28, b_+31); A = mem_rd(gb, w1Link_xh);
  CYC(b_+31, b_+32); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+32, b_+33); alu_add(gb, B);
  CYC(b_+33, b_+34); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+34, b_+35); ret_effect(gb); return; } // ret nc
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); alu_xor(gb, A);
  CYC(b_+36, b_+38); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+38, b_+40); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto l_5afa; } // jr nz
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = alu_inc8(gb, A);

l_5afa:
  CYC(b_+43, b_+46); mem_wr(gb, wcc50, A);
  CYC(b_+46, b_+48); A = 0x11; // LINK_STATE_SQUISHED
  CYC(b_+48, b_+51); mem_wr(gb, wLinkForceState, A);
  CYC(b_+51, b_+52); alu_scf(gb);
  RET(b_+52); return;
}

// @param[out] cflag c if Link's close enough to the platform?
void sidescrollPlatform_checkLinkIsClose_hook(GB *gb) {
  BASE(sidescrollPlatform_checkLinkIsClose);
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkInAir);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+6); B = 0x05;
  if (F & FZ) { CYCT(b_+6, b_+8); goto l_5b0d; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); B = alu_dec8(gb, B);

l_5b0d:
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_add(gb, B);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); A = alu_inc8(gb, A);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+20, b_+23); A = mem_rd(gb, w1Link_xh);
  CYC(b_+23, b_+24); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+24, b_+25); alu_add(gb, B);
  CYC(b_+25, b_+26); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+26, b_+27); ret_effect(gb); return; } // ret nc
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_sub(gb, 0x02);
  CYC(b_+32, b_+33); B = A;
  CYC(b_+33, b_+34); alu_add(gb, A);
  CYC(b_+34, b_+35); A = alu_inc8(gb, A);
  CYC(b_+35, b_+36); C = A;
  CYC(b_+36, b_+38); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+38, b_+41); A = mem_rd(gb, w1Link_yh);
  CYC(b_+41, b_+42); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+42, b_+43); alu_add(gb, B);
  CYC(b_+43, b_+44); alu_cp(gb, C);
  CYC(b_+44, b_+45); alu_ccf(gb);
  RET(b_+45); return;
}

// @param[out] a Collision value
// @param[out] zflag nz if a valid collision value is returned
void sidescrollPlatform_getTileCollisionBehindLink_hook(GB *gb) {
  BASE(sidescrollPlatform_getTileCollisionBehindLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+2, b_+5); A = mem_rd(gb, w1Link_xh);
  CYC(b_+5, b_+6); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); B = 0xfb; // -5
  if (F & FC) { CYCT(b_+8, b_+10); goto l_5b3e; } // jr c
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); B = 0x04;

l_5b3e:
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+17); A = mem_rd(gb, w1Link_yh);
  CYC(b_+17, b_+19); alu_sub(gb, 0x04);
  CYC(b_+19, b_+20); B = A;
  CALL_C(b_+20, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+23);
  if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; } // ret nz
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+27); alu_add(gb, 0x08);
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+31); TAIL(getTileCollisionsAtPosition); // jp
}

// @param[out] hl
void sidescrollPlatformFunc_5b51_hook(GB *gb) {
  BASE(sidescrollPlatformFunc_5b51);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+3, b_+6); A = mem_rd(gb, w1Link_yh);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); B = 0xfa; // -6
  if (F & FC) { CYCT(b_+9, b_+11); goto l_5b5e; } // jr c
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); B = 0x09;

l_5b5e:
  CYC(b_+13, b_+14); alu_add(gb, B);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+18); A = mem_rd(gb, w1Link_xh);
  CYC(b_+18, b_+20); alu_sub(gb, 0x03);
  CYC(b_+20, b_+21); C = A;
  CALL_C(b_+21, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+24);
  CYC(b_+24, b_+27); SET_HL(hFF8B);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x00);
  if (F & FZ) { CYCT(b_+29, b_+31); goto l_5b72; } // jr z
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));

l_5b72:
  CYC(b_+33, b_+34); A = C;
  CYC(b_+34, b_+36); alu_add(gb, 0x05);
  CYC(b_+36, b_+37); C = A;
  CALL_C(b_+37, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+40);
  CYC(b_+40, b_+43); SET_HL(hFF8B);
  if (F & FZ) { CYCT(b_+43, b_+44); ret_effect(gb); return; } // ret z
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+45); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+45); return;
}

// Checks if Link's on the platform, updates wLinkRidingObject if so.
// @param[out] zflag nz if Link is standing on the platform
void sidescrollPlatform_checkLinkOnPlatform_hook(GB *gb) {
  BASE(sidescrollPlatform_checkLinkOnPlatform);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectCheckCollidedWithLink_hook, SYM(objectCheckCollidedWithLink), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); goto notOnPlatform; } // jr nc
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+11, b_+12); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); alu_sub(gb, 0x02);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+18); A = mem_rd(gb, w1Link_yh);
  CYC(b_+18, b_+19); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+19, b_+21); goto notOnPlatform; } // jr nc
  CYC(b_+19, b_+21);
  CALL_C(b_+21, sidescrollPlatform_checkLinkIsClose_hook, SYM(sidescrollPlatform_checkLinkIsClose), b_+24);
  if (!(F & FC)) { CYCT(b_+24, b_+26); goto notOnPlatform; } // jr nc
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto onPlatform; } // jr nz
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CALL_C(b_+35, sidescrollPlatform_updateLinkSubpixels_hook, SYM(sidescrollPlatform_updateLinkSubpixels), b_+38);

onPlatform:
  CYC(b_+38, b_+39); A = D;
  CYC(b_+39, b_+42); mem_wr(gb, wLinkRidingObject, A);
  CYC(b_+42, b_+43); alu_xor(gb, A);
  RET(b_+43); return;

notOnPlatform:
  CYC(b_+44, b_+46); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+48, b_+49); ret_effect(gb); return; } // ret z
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); A = 0x00;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  RET(b_+52); return;
}

void sidescrollPlatform_updateLinkKnockbackForConveyor_hook(GB *gb) {
  BASE(sidescrollPlatform_updateLinkKnockbackForConveyor);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(w1Link_knockbackAngle);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); A = alu_swap(gb, A);
  CYC(b_+14, b_+16); alu_add(gb, 0x08);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = (uint8_t)w1Link_invincibilityCounter;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0xfc);
  CYC(b_+21, b_+23); L = (uint8_t)w1Link_knockbackCounter;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x0c);
  RET(b_+25); return;
}

// @param[out] hl counter1
void sidescrollPlatform_decCounter1_hook(GB *gb) {
  BASE(sidescrollPlatform_decCounter1);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+7); return;
}

// 0b:5bf0, the shared tail of pushLinkAwayVertical/pushLinkAwayHorizontal below. Entered either by
// straight fallthrough from pushLinkAwayHorizontal's own setup, or by a `jr` (no stack effect) from
// pushLinkAwayVertical after its own setup.
static void interactionCode9e_pushLinkAway_tail(GB *gb) {
  BASE(sidescrollPlatform_pushLinkAwayHorizontal);
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+14, b_+16); goto tail2; } // jr c
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = B;
  CYC(b_+17, b_+18); alu_cpl(gb);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+20); B = A;

tail2:
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_add(gb, B);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23); return;
}

void sidescrollPlatform_pushLinkAwayVertical_hook(GB *gb) {
  BASE(sidescrollPlatform_pushLinkAwayVertical);
  CYC(b_+0, b_+3); SET_HL(w1Link_collisionRadiusY);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); L = (uint8_t)w1Link_yh;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+12, b_+14); interactionCode9e_pushLinkAway_tail(gb); // jr
}

void sidescrollPlatform_pushLinkAwayHorizontal_hook(GB *gb) {
  BASE(sidescrollPlatform_pushLinkAwayHorizontal);
  CYC(b_+0, b_+3); SET_HL(w1Link_collisionRadiusX);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); L = (uint8_t)w1Link_xh;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_XH;
  interactionCode9e_pushLinkAway_tail(gb); // falls through
}

// 0b:5bfc, called from movingPlatform_stateC (not part of this file's cluster). Its "ret nz" not
// taken path falls straight into sidescrollPlatform_updateLinkSubpixels below (no jump at all).
void sidescrollPlatformFunc_5bfc_hook(GB *gb) {
  BASE(sidescrollPlatformFunc_5bfc);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectRunMovementScript_hook, SYM(objectRunMovementScript), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+6, b_+7); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; } // ret nz
  CYC(b_+7, b_+8);
  sidescrollPlatform_updateLinkSubpixels_hook(gb); // falls through
}

void sidescrollPlatform_updateLinkSubpixels_hook(GB *gb) {
  BASE(sidescrollPlatform_updateLinkSubpixels);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_Y;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); mem_wr(gb, w1Link_y, A);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_X;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); mem_wr(gb, w1Link_x, A);
  RET(b_+12); return;
}
