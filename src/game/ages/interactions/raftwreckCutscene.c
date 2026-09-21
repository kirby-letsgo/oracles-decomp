#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode9b_jump_table(GB *gb) {
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

static void interactionCode9b_add_a_to_hl(GB *gb) {
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

// 0b:530f, called once from interactionCode9b@state1.
void interactionCode9b_updateSubstate_hook(GB *gb) {
  BASE(interactionCode9b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+62); alu_cp(gb, 0x02);
  if (!(F & FC)) { CALL_C_CC(b_+62, interactionRunScript_hook, SYM(interactionRunScript), b_+65); } else { CYC(b_+62, b_+65); } // call nc
  CYC(b_+65, b_+67); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); push_effect(gb, b_+69);
  do { uint16_t jt_ = (interactionCode9b_jump_table(gb));
    if (jt_ == b_+87) { goto substate0; }
    else if (jt_ == b_+128) { goto substate1; }
    else if (jt_ == b_+148) { goto substate2; }
    else if (jt_ == b_+167) { goto substate3; }
    else if (jt_ == b_+197) { goto substate4; }
    else if (jt_ == b_+203) { goto substate6; }
    else if (jt_ == b_+215) { goto substate7; }
    else if (jt_ == b_+238) { goto substate8; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+87, b_+90); A = mem_rd(gb, wScrollMode);
  CYC(b_+90, b_+92); alu_and(gb, 0x01); // SCROLLMODE_01
  if (F & FZ) { CYCT(b_+92, b_+93); ret_effect(gb); return; } // ret z
  CYC(b_+92, b_+93);
  CALL_C(b_+93, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+96);
  CYC(b_+96, b_+98); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+100, b_+102); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); alu_sub(gb, 0x50);
  CYC(b_+105, b_+107); C = 0x03; // DIR_LEFT
  CYC(b_+107, b_+109); B = 0x18; // ANGLE_LEFT
  if (!(F & FC)) { CYCT(b_+109, b_+111); goto l_534b; } // jr nc
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+113); C = 0x01; // DIR_RIGHT
  CYC(b_+113, b_+115); B = 0x08; // ANGLE_RIGHT
  CYC(b_+115, b_+116); alu_cpl(gb);
  CYC(b_+116, b_+117); A = alu_inc8(gb, A);

l_534b:
  CYC(b_+117, b_+119); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+119, b_+120); mem_wr(gb, HL, B);
  CYC(b_+120, b_+122); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+122, b_+123); alu_add(gb, A);
  CYC(b_+123, b_+124); mem_wr(gb, HL, A);
  CYC(b_+124, b_+125); A = C;
  CYC(b_+125, b_+128); TAIL(setLinkDirection); // jp

substate1:
  CYC(b_+128, b_+129); H = D;
  CYC(b_+129, b_+131); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+131, b_+132); A = mem_rd(gb, HL);
  CYC(b_+132, b_+133); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+133, b_+135); goto l_5361; } // jr z
  CYC(b_+133, b_+135);
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+136, b_+139); TAIL(objectApplySpeed); // jp

l_5361:
  CALL_C(b_+139, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+142);
  CYC(b_+142, b_+145); SET_HL((SYM(interactiond7_makuSeed__state3Substate2) + 13)); // mainScripts.raftwreckCutsceneScript
  CYC(b_+145, b_+148); TAIL(interactionSetScript); // jp

substate2:
  CYC(b_+148, b_+151); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+151, b_+153); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+153, b_+154); ret_effect(gb); return; } // ret nz
  CYC(b_+153, b_+154);

// @initScreenFlashing (falls into from substate2 above; also reached via goto from substate4)
initScreenFlashing:
  CYC(b_+154, b_+157); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+157, b_+159); mem_wr(gb, HL, 0x00);
  CYC(b_+159, b_+162); SET_HL(wGenericCutscene_cbba);
  CYC(b_+162, b_+164); mem_wr(gb, HL, 0xff);
  CYC(b_+164, b_+167); TAIL(interactionIncSubstate); // jp

substate3:
  CYC(b_+167, b_+170); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+170, b_+172); B = 0x01;
  CALL_C(b_+172, flashScreen_hook, SYM(flashScreen), b_+175);
  if (F & FZ) { CYCT(b_+175, b_+176); ret_effect(gb); return; } // ret z
  CYC(b_+175, b_+176);
  CALL_C(b_+176, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+179);
  CYC(b_+179, b_+180); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+180, b_+182); alu_cp(gb, 0x03);
  CYC(b_+182, b_+184); A = 0x5a;
  if (F & FZ) { CYCT(b_+184, b_+186); goto l_5392; } // jr z
  CYC(b_+184, b_+186);
  CYC(b_+186, b_+188); A = 0x78;

l_5392:
  CYC(b_+188, b_+189); mem_wr(gb, HL, A);
  CYC(b_+189, b_+191); A = 0xf1;
  CYC(b_+191, b_+194); mem_wr(gb, wPaletteThread_parameter, A);
  CYC(b_+194, b_+197); TAIL(darkenRoom); // jp

substate4:
  CALL_C(b_+197, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+200);
  if (!(F & FZ)) { CYCT(b_+200, b_+201); ret_effect(gb); return; } // ret nz
  CYC(b_+200, b_+201);
  CYC(b_+201, b_+203); goto initScreenFlashing; // jr

substate6:
  CALL_C(b_+203, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+206);
  if (!(F & FZ)) { CYCT(b_+206, b_+207); ret_effect(gb); return; } // ret nz
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+209); A = 0x02;
  CYC(b_+209, b_+212); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(b_+212, b_+215); TAIL(interactionIncSubstate); // jp

substate7:
  CYC(b_+215, b_+218); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+218, b_+220); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+220, b_+222); goto l_53bc; } // jr nz
  CYC(b_+220, b_+222);
  CALL_C(b_+222, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+225);
  CYC(b_+225, b_+227); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+227, b_+229); mem_wr(gb, HL, 20);
  RET(b_+229); return;

l_53bc:
  CYC(b_+230, b_+232); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+232, b_+233); A = mem_rd(gb, DE);
  CYC(b_+233, b_+234); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+234, b_+235); ret_effect(gb); return; } // ret z
  CYC(b_+234, b_+235);
  CYC(b_+235, b_+238); goto oscillateY; // jp

substate8:
  CALL_C(b_+238, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+241);
  if (!(F & FZ)) { CYCT(b_+241, b_+242); ret_effect(gb); return; } // ret nz
  CYC(b_+241, b_+242);
  CYC(b_+242, b_+244); A = 0xfa; // SNDCTRL_FAST_FADEOUT
  CALL_C(b_+244, playSound_b00_hook, SYM(playSound_b00), b_+247);
  CALL_C(b_+247, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+250);
  CYC(b_+250, b_+252); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6))); // ROOMFLAG_BIT_40
  CYC(b_+252, b_+255); SET_HL(w1Companion_enabled);
  CYC(b_+255, b_+257); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(b_+257, b_+259); A = 0xd0; // >w1Link
  CYC(b_+259, b_+262); mem_wr(gb, wLinkObjectIndex, A);
  CYC(b_+262, b_+265); SET_HL(b_+268); // @tokayWarpDest
  CYC(b_+265, b_+268); TAIL(setWarpDestVariables); // jp

oscillateY:
  CYC(b_+273, b_+276); A = mem_rd(gb, wFrameCounter);
  CYC(b_+276, b_+278); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+278, b_+279); ret_effect(gb); return; } // ret nz
  CYC(b_+278, b_+279);
  CYC(b_+279, b_+282); A = mem_rd(gb, wFrameCounter);
  CYC(b_+282, b_+284); alu_and(gb, 0x38);
  CYC(b_+284, b_+286); A = alu_swap(gb, A);
  CYC(b_+286, b_+287); alu_rlca(gb);
  CYC(b_+287, b_+290); SET_HL(b_+297); // @yOscillation
  CYC(b_+290, b_+291); push_effect(gb, b_+291); interactionCode9b_add_a_to_hl(gb); // rst $10
  CYC(b_+291, b_+293); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+293, b_+294); A = mem_rd(gb, DE);
  CYC(b_+294, b_+295); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+295, b_+296); mem_wr(gb, DE, A);
  RET(b_+296); return;

  // @tokayWarpDest (0b:53e2): m_HardcodedWarpA ROOM_AGES_1aa, $00, $42, $03 -- pure ROM data
  // referenced only via SET_HL(0x53e2) above.
  // @yOscillation (0b:53ff): pure ROM data referenced only via SET_HL(0x53ff) above.
}

// ==================================================================================================
// INTERAC_RAFTWRECK_CUTSCENE
// ==================================================================================================
void interactionCode9b_hook(GB *gb) {
  BASE(interactionCode9b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCode9b_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+45) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 6, A); // ROOMFLAG_BIT_40
  if (!(F & FZ)) { CYCT(b_+13, b_+16); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+21); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+21, b_+24); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+24, b_+27); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+27, b_+28); H = A;
  CYC(b_+28, b_+30); L = 0x0d; // SpecialObject.xh
  CYC(b_+30, b_+31); C = mem_rd(gb, HL);
  CYC(b_+31, b_+33); B = 0x76;
  CALL_C(b_+33, interactionSetPosition_hook, SYM(interactionSetPosition), b_+36);
  CALL_C(b_+36, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+39);
  CYC(b_+39, b_+42); mem_wr(gb, wTmpcfc0_genericCutscene_cfd0, A);
  CYC(b_+42, b_+45); TAIL(interactionIncState); // jp

state1:
  CALL_L(b_+45, interactionCode9b_updateSubstate_hook, b_+48);
  CYC(b_+48, b_+51); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+51, b_+52); H = A;
  CYC(b_+52, b_+54); L = 0x0b; // SpecialObject.yh
  CYC(b_+54, b_+57); TAIL(objectCopyPosition); // jp
}
