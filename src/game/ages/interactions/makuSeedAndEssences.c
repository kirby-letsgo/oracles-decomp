#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactiond7_jump_table(GB *gb) {
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

static void interactiond7_addAToHl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// 0b:7c0a, interactiond7_essence@playCirclingSound. Reached by a genuine `call z` from
// @state3, which still has more work to do afterward, so the resume is verified like CALL_C
// (this local itself is a bare tail-call into playSound_b00_hook with no push of its own).
static void interactiond7_essence_playCirclingSound(GB *gb) {
  BASE(interactiond7_essence);
  CYC(b_+94, b_+96); A = 0xc9; // SND_CIRCLING
  CYC(b_+96, b_+99); TAIL(playSound_b00); // jp
}

// 0b:7c0f, called from interactiond7_makuSeed@state2 and @state3.
void interactiond7_updateSmallSparkles_hook(GB *gb) {
  BASE(interactiond7_updateSmallSparkles);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_BC(0x8403); // INTERAC_SPARKLE, 3
  CALL_C(b_+9, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+12);
  if (!(F & FZ)) { CYCT(b_+12, b_+13); ret_effect(gb); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, wFrameCounter);
  CYC(b_+16, b_+18); alu_and(gb, 0x38);
  CYC(b_+18, b_+20); alu_swap_a(gb);
  CYC(b_+20, b_+21); alu_rlca(gb);
  CYC(b_+21, b_+24); SET_BC(b_+39); // @sparklePositionOffsets
  CALL_C(b_+24, addDoubleIndexToBc_hook, 0x007e, b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+29, b_+30); A = mem_rd(gb, BC);
  CYC(b_+30, b_+31); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+33); SET_BC(BC + 1); // inc bc
  CYC(b_+33, b_+35); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+35, b_+36); A = mem_rd(gb, BC);
  CYC(b_+36, b_+37); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  RET(b_+38); return;
}

// 0b:7c46, called from interactiond7_makuSeed@state3.
// Updates Z-position based on frame counter.
void interactiond7_updateFloating_hook(GB *gb) {
  BASE(interactiond7_updateFloating);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+11); alu_and(gb, 0x38);
  CYC(b_+11, b_+13); alu_swap_a(gb);
  CYC(b_+13, b_+14); alu_rlca(gb);
  CYC(b_+14, b_+17); SET_HL(b_+23); // @zPositions
  CYC(b_+17, b_+18); interactiond7_addAToHl(gb, b_+18);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return;
}

// 0b:7bac, called from interactionCoded7 (subid 1-8).
void interactiond7_essence_hook(GB *gb) {
  BASE(interactiond7_essence);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiond7_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+43) { goto state1; }
    else if (jt_ == b_+53) { goto state2; }
    else if (jt_ == b_+61) { goto state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A); // [state]
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x10);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+24, b_+26); A = 0x98; // SND_POOF
  CALL_C(b_+26, playSound_b00_hook, SYM(playSound_b00), b_+29);
  CALL_C(b_+29, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+32);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  CALL_C(b_+37, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+40);
  CYC(b_+40, b_+43); TAIL(objectSetVisible80); // jp

state1:
  CALL_C(b_+43, objectApplySpeed_hook, SYM(objectApplySpeed), b_+46);
  CALL_C(b_+46, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+49);
  if (!(F & FZ)) { CYCT(b_+49, b_+50); ret_effect(gb); return; } // ret nz
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+53); TAIL(interactionIncState); // jp

state2:
  CYC(b_+53, b_+56); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+57, b_+58); ret_effect(gb); return; } // ret z
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+61); TAIL(interactionIncState); // jp

state3:
  CALL_C(b_+61, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+64);
  if (!(F & FC)) { CYCT(b_+64, b_+67); interactionDelete_hook(gb); return; } // jp nc
  CYC(b_+64, b_+67);
  CYC(b_+67, b_+70); A = mem_rd(gb, wFrameCounter);
  CYC(b_+70, b_+71); alu_rrca(gb);
  if (F & FC) { CYCT(b_+71, b_+72); ret_effect(gb); return; } // ret c
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+73); H = D;
  CYC(b_+73, b_+75); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); alu_and(gb, 0x1f);
  CYC(b_+79, b_+80); mem_wr(gb, HL, A);
  CYC(b_+80, b_+81); E = L;
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+82, b_+85); push_effect(gb, b_+85); interactiond7_essence_playCirclingSound(gb);
    if (!(gb->pc == b_+85 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  } else {
    CYC(b_+82, b_+85);
  }
  CYC(b_+85, b_+88); SET_BC((SYM(interactionCodea1__state9) + 11));
  CYC(b_+88, b_+91); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfc1);
  CYC(b_+91, b_+94); TAIL(objectSetPositionInCircleArc); // jp
}

// 0b:79d8, called from interactionCoded7 (subid 0).
void interactiond7_makuSeed_hook(GB *gb) {
  BASE(interactiond7_makuSeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiond7_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == b_+64) { goto state1; }
    else if (jt_ == b_+82) { goto state2; }
    else if (jt_ == b_+120) { goto state3; }
    else if (jt_ == b_+312) { goto state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+17, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+20);
  CYC(b_+20, b_+23); A = mem_rd(gb, w1Link_yh);
  CYC(b_+23, b_+25); alu_sub(gb, 0x0e);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+31); A = mem_rd(gb, w1Link_xh);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CALL_C(b_+34, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+37);
  CYC(b_+37, b_+39); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+44); A = 0x77; // SND_DROPESSENCE
  CALL_C(b_+44, playSound_b00_hook, SYM(playSound_b00), b_+47);
  CYC(b_+47, b_+50); SET_BC(0x8404); // INTERAC_SPARKLE, 4
  CALL_C(b_+50, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+53);
  if (!(F & FZ)) { CYCT(b_+53, b_+54); ret_effect(gb); return; } // ret nz
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+56, b_+57); E = L;
  CYC(b_+57, b_+59); A = 0x78; // 120
  CYC(b_+59, b_+60); mem_wr(gb, HL, A);
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+64); TAIL(objectSetVisible82); // jp

state1:
  CYC(b_+64, b_+66); A = 0x0f; // LINK_ANIM_MODE_GETITEM2HAND
  CYC(b_+66, b_+69); mem_wr(gb, wcc50, A);
  CALL_C(b_+69, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+72);
  if (!(F & FZ)) { CYCT(b_+72, b_+73); ret_effect(gb); return; } // ret nz
  CYC(b_+72, b_+73);
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x40); // [counter1]
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+79, b_+82); TAIL(interactionIncState); // jp

state2:
  CALL_C(b_+82, objectApplySpeed_hook, SYM(objectApplySpeed), b_+85);
  CALL_C(b_+85, interactiond7_updateSmallSparkles_hook, SYM(interactiond7_updateSmallSparkles), b_+88);
  CALL_C(b_+88, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+91);
  if (!(F & FZ)) { CYCT(b_+91, b_+92); ret_effect(gb); return; } // ret nz
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x78); // [counter1]
  CYC(b_+94, b_+96); A = 0x10; // LINK_ANIM_MODE_WALK
  CYC(b_+96, b_+99); mem_wr(gb, wcc50, A);
  CYC(b_+99, b_+101); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x58);
  CYC(b_+103, b_+105); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x78);
  CYC(b_+107, b_+109); A = 0x8a; // SND_PIECE_OF_POWER
  CALL_C(b_+109, playSound_b00_hook, SYM(playSound_b00), b_+112);
  CYC(b_+112, b_+114); A = 0x03;
  CALL_C(b_+114, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+117);
  CYC(b_+117, b_+120); TAIL(interactionIncState); // jp

state3:
  CALL_C(b_+120, interactiond7_updateSmallSparkles_hook, SYM(interactiond7_updateSmallSparkles), b_+123);
  CALL_C(b_+123, interactiond7_updateFloating_hook, SYM(interactiond7_updateFloating), b_+126);
  CYC(b_+126, b_+128); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+130); push_effect(gb, b_+130);
  do { uint16_t jt_ = (interactiond7_jump_table(gb));
    if (jt_ == b_+150) { goto state3Substate0; }
    else if (jt_ == b_+162) { goto state3Substate1; }
    else if (jt_ == b_+225) { goto state3Substate2; }
    else if (jt_ == b_+244) { goto state3Substate3; }
    else if (jt_ == b_+257) { goto state3Substate4; }
    else if (jt_ == b_+277) { goto state3Substate8; }
    else if (jt_ == b_+294) { goto state3Substate9; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state3Substate0:
  CALL_C(b_+150, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+153);
  if (!(F & FZ)) { CYCT(b_+153, b_+154); ret_effect(gb); return; } // ret nz
  CYC(b_+153, b_+154);
  CYC(b_+154, b_+156); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(b_+156, b_+157); L = alu_inc8(gb, L);
  CYC(b_+157, b_+159); mem_wr(gb, HL, 0x08); // [counter2]
  CYC(b_+159, b_+162); TAIL(interactionIncSubstate); // jp

state3Substate1:
  CALL_C(b_+162, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+165);
  if (!(F & FZ)) { CYCT(b_+165, b_+166); ret_effect(gb); return; } // ret nz
  CYC(b_+165, b_+166);
  CYC(b_+166, b_+168); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(b_+168, b_+169); L = alu_inc8(gb, L); // [counter2]
  CYC(b_+169, b_+170); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+170, b_+171); B = mem_rd(gb, HL);
  CALL_C(b_+171, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+174);
  if (!(F & FZ)) { CYCT(b_+174, b_+175); ret_effect(gb); return; } // ret nz
  CYC(b_+174, b_+175);
  CYC(b_+175, b_+177); mem_wr(gb, HL, 0xd7); // INTERAC_MAKU_SEED_AND_ESSENCES
  CALL_C(b_+177, objectCopyPosition_hook, SYM(objectCopyPosition), b_+180);
  CYC(b_+180, b_+181); A = B;
  CYC(b_+181, b_+184); SET_BC(b_+209); // @essenceSpawnerData
  CALL_C(b_+184, addDoubleIndexToBc_hook, 0x007e, b_+187);
  CYC(b_+187, b_+188); A = mem_rd(gb, BC);
  CYC(b_+188, b_+190); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+190, b_+191); mem_wr(gb, HL, A);
  CYC(b_+191, b_+193); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+193, b_+194); SET_BC(BC + 1); // inc bc
  CYC(b_+194, b_+195); A = mem_rd(gb, BC);
  CYC(b_+195, b_+196); mem_wr(gb, HL, A);
  CYC(b_+196, b_+198); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CYC(b_+199, b_+200); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+200, b_+201); ret_effect(gb); return; } // ret nz
  CYC(b_+200, b_+201);
  CALL_C(b_+201, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+204);
  CYC(b_+204, b_+206); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+206, b_+208); mem_wr(gb, HL, 0x78); // 120
  RET(b_+208); return;

state3Substate2:
  CALL_C(b_+225, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+228);
  if (!(F & FZ)) { CYCT(b_+228, b_+229); ret_effect(gb); return; } // ret nz
  CYC(b_+228, b_+229);
  CYC(b_+229, b_+231); mem_wr(gb, HL, 0x3c); // [counter1], 60
  CYC(b_+231, b_+233); A = 0x01;
  CYC(b_+233, b_+236); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(b_+236, b_+238); A = 0x20;
  CYC(b_+238, b_+241); mem_wr(gb, wTmpcfc0_genericCutscene_cfc1, A);
  CYC(b_+241, b_+244); TAIL(interactionIncSubstate); // jp

state3Substate3: // also @state3Substate5, @state3Substate7
  CYC(b_+244, b_+247); A = mem_rd(gb, wFrameCounter);
  CYC(b_+247, b_+249); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+249, b_+251); goto essenceRotationCommon; } // jr nz
  CYC(b_+249, b_+251);
  CYC(b_+251, b_+254); SET_HL(wTmpcfc0_genericCutscene_cfc1);
  CYC(b_+254, b_+255); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYCT(b_+255, b_+257); goto essenceRotationCommon; // jr

state3Substate4: // also @state3Substate6
  CYC(b_+257, b_+260); A = mem_rd(gb, wFrameCounter);
  CYC(b_+260, b_+262); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+262, b_+264); goto essenceRotationCommon; } // jr nz
  CYC(b_+262, b_+264);
  CYC(b_+264, b_+267); SET_HL(wTmpcfc0_genericCutscene_cfc1);
  CYC(b_+267, b_+268); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  // falls through into @essenceRotationCommon

essenceRotationCommon:
  CALL_C(b_+268, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+271);
  if (!(F & FZ)) { CYCT(b_+271, b_+272); ret_effect(gb); return; } // ret nz
  CYC(b_+271, b_+272);
  CYC(b_+272, b_+274); mem_wr(gb, HL, 0x3c); // [counter1], 60
  CYC(b_+274, b_+277); TAIL(interactionIncSubstate); // jp

state3Substate8:
  CYC(b_+277, b_+280); SET_HL(wTmpcfc0_genericCutscene_cfc1);
  CYC(b_+280, b_+281); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+281, b_+283); A = 0xb4; // SND_FADEOUT
  CALL_C(b_+283, playSound_b00_hook, SYM(playSound_b00), b_+286);
  CYC(b_+286, b_+288); A = 0x04;
  CALL_C(b_+288, fadeoutToWhiteWithDelay_hook, SYM(fadeoutToWhiteWithDelay), b_+291);
  CYC(b_+291, b_+294); TAIL(interactionIncSubstate); // jp

state3Substate9:
  CYC(b_+294, b_+297); SET_HL(wTmpcfc0_genericCutscene_cfc1);
  CYC(b_+297, b_+298); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+298, b_+301); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+301, b_+302); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+302, b_+303); ret_effect(gb); return; } // ret nz
  CYC(b_+302, b_+303);
  CALL_C(b_+303, interactionIncState_hook, SYM(interactionIncState), b_+306);
  CYC(b_+306, b_+307); L = alu_inc8(gb, L);
  CYC(b_+307, b_+309); mem_wr(gb, HL, 0x00);
  CYC(b_+309, b_+312); TAIL(objectSetInvisible); // jp

state4:
  CYC(b_+312, b_+314); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+314, b_+315); A = mem_rd(gb, DE);
  CYC(b_+315, b_+316); push_effect(gb, b_+316);
  do { uint16_t jt_ = (interactiond7_jump_table(gb));
    if (jt_ == b_+326) { goto state4Substate0; }
    else if (jt_ == b_+387) { goto state4Substate1; }
    else if (jt_ == b_+401) { goto state4Substate2; }
    else if (jt_ == b_+414) { goto state4Substate3; }
    else if (jt_ == b_+455) { goto state4Substate4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state4Substate0:
  CYC(b_+326, b_+329); SET_HL(b_+350); // @tileReplacements

l_7b21:
  CYC(b_+329, b_+330); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+330, b_+331); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+331, b_+333); goto l_7b2e; } // jr z
  CYC(b_+331, b_+333);
  CYC(b_+333, b_+334); C = mem_rd(gb, HL);
  CYC(b_+334, b_+335); SET_HL(HL + 1); // inc hl
  CYC(b_+335, b_+336); push_effect(gb, HL); // push hl
  CALL_C(b_+336, setTile_hook, SYM(setTile), b_+339);
  CYC(b_+339, b_+340); SET_HL(pop_effect(gb)); // pop hl
  CYCT(b_+340, b_+342); goto l_7b21; // jr

l_7b2e:
  CYC(b_+342, b_+344); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+344, b_+346); A = 0x1e; // 30
  CYC(b_+346, b_+347); mem_wr(gb, DE, A);
  CYC(b_+347, b_+350); TAIL(interactionIncSubstate); // jp

state4Substate1:
  CALL_C(b_+387, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+390);
  if (!(F & FZ)) { CYCT(b_+390, b_+391); ret_effect(gb); return; } // ret nz
  CYC(b_+390, b_+391);
  CYC(b_+391, b_+393); mem_wr(gb, HL, 0x78); // [counter1], 120
  CYC(b_+393, b_+395); A = 0x08;
  CALL_C(b_+395, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+398);
  CYC(b_+398, b_+401); TAIL(interactionIncSubstate); // jp

state4Substate2:
  CYC(b_+401, b_+404); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+404, b_+405); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+405, b_+406); ret_effect(gb); return; } // ret nz
  CYC(b_+405, b_+406);
  CYC(b_+406, b_+408); A = 0x5b; // SND_SOLVEPUZZLE_2
  CALL_C(b_+408, playSound_b00_hook, SYM(playSound_b00), b_+411);
  CYC(b_+411, b_+414); TAIL(interactionIncSubstate); // jp

state4Substate3:
  CALL_C(b_+414, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+417);
  if (!(F & FZ)) { CYCT(b_+417, b_+418); ret_effect(gb); return; } // ret nz
  CYC(b_+417, b_+418);
  CALL_C(b_+418, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+421);
  CYC(b_+421, b_+423); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6))); // set 6,(hl)
  CYC(b_+423, b_+426); SET_HL(wRoomLayout + 0x47);
  CYC(b_+426, b_+428); mem_wr(gb, HL, 0x44);
  CALL_C(b_+428, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+431);
  if (F & FZ) { CYCT(b_+431, b_+433); goto unlinkedGame; } // jr z
  CYC(b_+431, b_+433);
  CALL_C(b_+433, fadeoutToBlack_hook, SYM(fadeoutToBlack), b_+436);
  CYC(b_+436, b_+439); TAIL(interactionIncSubstate); // jp

unlinkedGame:
  CYC(b_+439, b_+440); alu_xor(gb, A);
  CYC(b_+440, b_+443); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+443, b_+446); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+446, b_+449); A = mem_rd(gb, wActiveMusic);
  CALL_C(b_+449, playSound_b00_hook, SYM(playSound_b00), b_+452);
  CYC(b_+452, b_+455); TAIL(interactionDelete); // jp

state4Substate4:
  CYC(b_+455, b_+458); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+458, b_+459); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+459, b_+460); ret_effect(gb); return; } // ret nz
  CYC(b_+459, b_+460);
  CYC(b_+460, b_+462); A = 0x11; // CUTSCENE_FLAME_OF_SORROW
  CYC(b_+462, b_+465); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+465, b_+468); TAIL(interactionDelete); // jp
}

// ==================================================================================================
// INTERAC_MAKU_SEED_AND_ESSENCES
//
// Variables:
//   counter1: Essence index (for the maku seed / spawner object)
// ==================================================================================================
void interactionCoded7_hook(GB *gb) {
  BASE(interactionCoded7);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiond7_jump_table(gb));
    if (jt_ == SYM(interactiond7_makuSeed) && hook_is(gb, SYM(interactiond7_makuSeed), interactiond7_makuSeed_hook)) { interactiond7_makuSeed_hook(gb); return; }
    else if (jt_ == SYM(interactiond7_essence) && hook_is(gb, SYM(interactiond7_essence), interactiond7_essence_hook)) { interactiond7_essence_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
