#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/din.s (INTERAC_DIN), bank $0a. Subids:
// 0 Din dancing in the intro (walks to Link, is captured by the vortex), 1 Din in the temple,
// 2/4 scripted Din in the ending, 3 Din being pulled up, 6 Din floating, 7 Din in the final
// cutscene, 8 Din after the game is finished, 9 Din discovering Link collapsed. The dispatcher
// interactionCodea5 itself stays generated (it has an Ages namesake).

static uint16_t din_jump_table(GB *gb) {
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

void s_dinState0_hook(GB *gb) {
  BASE(dinState0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6);
  CALL_C(b_+6, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+9);
  CYC(b_+9, b_+11); E = 0x42; // Interaction.subid
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (din_jump_table(gb));
    if (jt_ == b_+33) { goto subid0; }
    else if (jt_ == b_+60) { goto subid1; }
    else if (jt_ == b_+70) { goto subid2; }
    else if (jt_ == b_+91) { goto stub; }
    else if (jt_ == b_+76) { goto subid4; }
    else if (jt_ == b_+82) { goto subid6; }
    else if (jt_ == b_+92) { goto subid7; }
    else if (jt_ == b_+98) { goto subid8; }
    else if (jt_ == b_+125) { goto subid9; }
    else { HANDOFF(HL); }
  } while (0);

subid0:
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = 0x4b; // Interaction.yh
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x00);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0xa0);
  CYC(b_+42, b_+44); L = 0x66; // Interaction.collisionRadiusY
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x20);
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x08);
  CYC(b_+49, b_+51); L = 0x49; // Interaction.angle
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x10);
  CYC(b_+53, b_+55); L = 0x50; // Interaction.speed
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x14);
  CYC(b_+57, b_+60);
  TAIL(setCameraFocusedObject);

subid1:
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = 0x4b; // Interaction.yh
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x98);
  CYC(b_+65, b_+66); L = alu_inc8(gb, L);
  CYC(b_+66, b_+67); L = alu_inc8(gb, L);
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0xa0);
  RET(b_+69); return;

subid2:
  CYC(b_+70, b_+73); SET_HL(SYM(dinScript_subid2Init));
  CYC(b_+73, b_+76);
  TAIL(interactionSetScript);

subid4:
  CYC(b_+76, b_+79); SET_HL(SYM(dinScript_subid4Init));
  CYC(b_+79, b_+82);
  TAIL(interactionSetScript);

subid6:
  CYC(b_+82, b_+83); H = D;
  CYC(b_+83, b_+85); L = 0x4b; // Interaction.yh
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x48);
  CYC(b_+87, b_+88); L = alu_inc8(gb, L);
  CYC(b_+88, b_+89); L = alu_inc8(gb, L);
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x80);
stub:
  RET(b_+91); return;

subid7:
  CYC(b_+92, b_+95); SET_HL(SYM(dinScript_stubInit));
  CYC(b_+95, b_+98);
  TAIL(interactionSetScript);

subid8:
  CYC(b_+98, b_+100); A = 0x28; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+100, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+103);
  if (F & FZ) { CYCT(b_+103, b_+106); TAIL(interactionDelete); }
  CYC(b_+103, b_+106);
  CYC(b_+106, b_+108); A = 0x06;
  CALL_C(b_+108, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+111);
  CYC(b_+111, b_+113); A = 0xa5; // INTERAC_DIN
  CYC(b_+113, b_+116); mem_wr(gb, wInteractionIDToLoadExtraGfx, A);
  CYC(b_+116, b_+119); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+119, b_+122); SET_HL(SYM(dinScript_subid8Init));
  CYC(b_+122, b_+125);
  TAIL(interactionSetScript);

subid9:
  CALL_C(b_+125, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+128);
  CYC(b_+128, b_+130); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+130, b_+133); TAIL(interactionDelete); }
  CYC(b_+130, b_+133);
  CYC(b_+133, b_+136); SET_HL(SYM(dinScript_discoverLinkCollapsed));
  CYC(b_+136, b_+139);
  TAIL(interactionSetScript);
}

void s_dinState1_hook(GB *gb) {
  BASE(dinState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42; // Interaction.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (din_jump_table(gb));
    if (jt_ == SYM(dinState1_subid0) && hook_is(gb, SYM(dinState1_subid0), s_dinState1_subid0_hook)) { s_dinState1_subid0_hook(gb); return; }
    else if (jt_ == SYM(dinState1_subid0__ret)) { RET(SYM(dinState1_subid0__ret)); return; }
    else if (jt_ == SYM(dinState1_subid2) && hook_is(gb, SYM(dinState1_subid2), s_dinState1_subid2_hook)) { s_dinState1_subid2_hook(gb); return; }
    else if (jt_ == SYM(dinState1_subid3) && hook_is(gb, SYM(dinState1_subid3), s_dinState1_subid3_hook)) { s_dinState1_subid3_hook(gb); return; }
    else if (jt_ == SYM(dinState1_subid4) && hook_is(gb, SYM(dinState1_subid4), s_dinState1_subid4_hook)) { s_dinState1_subid4_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), interactionAnimate_hook)) { interactionAnimate_hook(gb); return; }
    else if (jt_ == SYM(dinState1_subid6) && hook_is(gb, SYM(dinState1_subid6), s_dinState1_subid6_hook)) { s_dinState1_subid6_hook(gb); return; }
    else if (jt_ == SYM(dinState1_subid7) && hook_is(gb, SYM(dinState1_subid7), s_dinState1_subid7_hook)) { s_dinState1_subid7_hook(gb); return; }
    else if (jt_ == SYM(dinState1_subid8) && hook_is(gb, SYM(dinState1_subid8), s_dinState1_subid8_hook)) { s_dinState1_subid8_hook(gb); return; }
    else if (jt_ == SYM(dinState1_subid9) && hook_is(gb, SYM(dinState1_subid9), s_dinState1_subid9_hook)) { s_dinState1_subid9_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// subid0@func_6521: every 16 frames, spawn a sparkle (subid 5) at a random x offset in -$10..$0f.
static void din_spawn_sparkle(GB *gb) {
  BASE(dinState1_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+54, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+57);
  if (!(F & FZ)) { RET_TAKEN(b_+57); return; }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x84); // INTERAC_SPARKLE
  CYC(b_+60, b_+61); L = alu_inc8(gb, L);
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x05);
  CALL_C(b_+63, getRandomNumber_hook, SYM(getRandomNumber), b_+66);
  CYC(b_+66, b_+68); alu_and(gb, 0x1f);
  CYC(b_+68, b_+70); alu_sub(gb, 0x10);
  CYC(b_+70, b_+72); B = 0x00;
  CYC(b_+72, b_+73); C = A;
  CYC(b_+73, b_+76);
  TAIL(objectCopyPositionWithOffset);
}

// subid0@func_6576: shake Din by a random -2..1 pixels around yh while the screen counts down.
static void din_shake(GB *gb) {
  BASE(dinState1_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+139, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+142);
  CYC(b_+142, b_+144); alu_and(gb, 0x03);
  CYC(b_+144, b_+146); alu_sub(gb, 0x02);
  CYC(b_+146, b_+147); H = D;
  CYC(b_+147, b_+149); L = 0x4c; // Interaction.var2c (saved yh)
  CYC(b_+149, b_+150); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+150, b_+151); L = alu_inc8(gb, L);
  CYC(b_+151, b_+152); mem_wr(gb, HL, A);
  CYC(b_+152, b_+155);
  TAIL(interactionAnimate);
}

// Din in the intro: walks down to y=$90 dropping sparkles, waits for Link to approach, then is
// caught by the vortex (screen flash, four fading crystals) and pulled up out of the room.
void s_dinState1_subid0_hook(GB *gb) {
  BASE(dinState1_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45; // Interaction.substate
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (din_jump_table(gb));
    if (jt_ == b_+20) { goto substate0; }
    else if (jt_ == b_+76) { goto substate1; }
    else if (jt_ == b_+120) { goto substate2; }
    else if (jt_ == b_+155) { goto substate3; }
    else if (jt_ == b_+202) { goto substate4; }
    else if (jt_ == b_+220) { goto substate5; }
    else if (jt_ == b_+243) { goto substate6; }
    else if (jt_ == b_+261) { goto substate7; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+20, interactionAnimate_hook, SYM(interactionAnimate), b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wFrameCounter);
  CYC(b_+26, b_+28); alu_and(gb, 0x0f);
  if (F & FZ) CALL_L_CC(b_+28, din_spawn_sparkle, b_+31);
  else CYC(b_+28, b_+31);
  CALL_C(b_+31, objectApplySpeed_hook, SYM(objectApplySpeed), b_+34);
  CYC(b_+34, b_+36); E = 0x4b; // Interaction.yh
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+39); alu_cp(gb, 0x90);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; }
  CYC(b_+39, b_+40);
  CALL_C(b_+40, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+43);
  CYC(b_+43, b_+44); alu_xor(gb, A);
  CYC(b_+44, b_+47); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+47, b_+48); A = alu_inc8(gb, A);
  CYC(b_+48, b_+51); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+51, b_+54);
  TAIL(setCameraFocusedObjectToLink);

substate1:
  CALL_C(b_+76, objectOscillateZ_hook, SYM(objectOscillateZ), b_+79);
  CALL_C(b_+79, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+82);
  CYC(b_+82, b_+84); C = 0x20;
  CALL_C(b_+84, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+87);
  if (!(F & FC)) { CYCT(b_+87, b_+90); TAIL(interactionAnimate); }
  CYC(b_+87, b_+90);
  CYC(b_+90, b_+93); A = mem_rd(gb, wLinkInAir);
  CYC(b_+93, b_+94); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+94); return; }
  CYC(b_+94, b_+95);
  CALL_C(b_+95, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+98);
  CYC(b_+98, b_+100); A = 0x80;
  CYC(b_+100, b_+103); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+103, b_+105); L = 0x46; // Interaction.counter1
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x32);
  CYC(b_+107, b_+109); L = 0x4d; // Interaction.xh
  CYC(b_+109, b_+110); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+110, b_+111); mem_wr(gb, HL, A);
  CYC(b_+111, b_+114); SET_HL(w1Link_direction);
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x03);
  CALL_C(b_+116, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+119);
  RET(b_+119); return;

substate2:
  CALL_C(b_+120, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+123);
  if (!(F & FZ)) { CYCT(b_+123, b_+125); din_shake(gb); return; }
  CYC(b_+123, b_+125);
  CALL_C(b_+125, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+128);
  CYC(b_+128, b_+131); SET_HL(wTmpcbb3);
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x00);
  CYC(b_+133, b_+136); SET_HL(wTmpcbba);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0xff);
  RET(b_+138); return;

substate3:
  CYC(b_+155, b_+158); SET_HL(wTmpcbb3);
  CYC(b_+158, b_+160); B = 0x01;
  CALL_C(b_+160, flashScreen_hook, SYM(flashScreen), b_+163);
  if (F & FZ) { RET_TAKEN(b_+163); return; }
  CYC(b_+163, b_+164);
  CALL_C(b_+164, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+167);
  CYC(b_+167, b_+169); L = 0x46; // Interaction.counter1
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x1e);
  CYC(b_+171, b_+173); B = 0x04;
  for (;;) {
    CALL_C(b_+173, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+176);
    if (!(F & FZ)) { CYCT(b_+176, b_+178); break; }
    CYC(b_+176, b_+178);
    CYC(b_+178, b_+180); mem_wr(gb, HL, 0xa6); // INTERAC_DINS_CRYSTAL_FADING
    CYC(b_+180, b_+181); L = alu_inc8(gb, L);
    CYC(b_+181, b_+182); mem_wr(gb, HL, B);
    CYC(b_+182, b_+183); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CALL_C(b_+183, objectCopyPosition_hook, SYM(objectCopyPosition), b_+186);
    CYC(b_+186, b_+187); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+187, b_+189); continue; }
    CYC(b_+187, b_+189);
    break;
  }
  CYC(b_+189, b_+191); A = 0x05;
  CALL_C(b_+191, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+194);
  CYC(b_+194, b_+196); A = 0x8a;
  CALL_C(b_+196, playSound_b00_hook, SYM(playSound_b00), b_+199);
  CYC(b_+199, b_+202);
  TAIL(clearPaletteFadeVariablesAndRefreshPalettes);

substate4:
  CALL_C(b_+202, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+205);
  if (!(F & FZ)) { RET_TAKEN(b_+205); return; }
  CYC(b_+205, b_+206);
  CALL_C(b_+206, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+209);
  CYC(b_+209, b_+211); L = 0x50; // Interaction.speed
  CYC(b_+211, b_+213); mem_wr(gb, HL, 0x28);
  CYC(b_+213, b_+215); L = 0x60; // Interaction.animCounter
  CYC(b_+215, b_+217); mem_wr(gb, HL, 0x01);
  CYC(b_+217, b_+220);
  TAIL(interactionAnimate);

substate5:
  CALL_C(b_+220, objectApplySpeed_hook, SYM(objectApplySpeed), b_+223);
  CYC(b_+223, b_+224); H = D;
  CYC(b_+224, b_+226); L = 0x4b; // Interaction.yh
  CYC(b_+226, b_+227); A = mem_rd(gb, HL);
  CYC(b_+227, b_+229); alu_sub(gb, 0x98);
  if (!(F & FZ)) { RET_TAKEN(b_+229); return; }
  CYC(b_+229, b_+230);
  CALL_C(b_+230, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+233);
  CYC(b_+233, b_+235); L = 0x4f; // Interaction.zh
  CYC(b_+235, b_+236); mem_wr(gb, HL, A);
  CYC(b_+236, b_+238); L = 0x60; // Interaction.animCounter
  CYC(b_+238, b_+240); mem_wr(gb, HL, 0x01);
  CYC(b_+240, b_+243);
  TAIL(interactionAnimate);

substate6:
  CALL_C(b_+243, interactionAnimate_hook, SYM(interactionAnimate), b_+246);
  CYC(b_+246, b_+248); E = 0x61; // Interaction.animParameter
  CYC(b_+248, b_+249); A = mem_rd(gb, DE);
  CYC(b_+249, b_+250); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+250); return; }
  CYC(b_+250, b_+251);
  CALL_C(b_+251, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+254);
  CYC(b_+254, b_+256); L = 0x46; // Interaction.counter1
  CYC(b_+256, b_+258); mem_wr(gb, HL, 0x1e);
  CYC(b_+258, b_+261);
  TAIL(npcFaceLinkAndAnimate);

substate7:
  CALL_C(b_+261, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+264);
  if (!(F & FZ)) { RET_TAKEN(b_+264); return; }
  CYC(b_+264, b_+265);
  CYC(b_+265, b_+267); A = 0x01;
  CYC(b_+267, b_+270); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  RET(b_+270); return;
}

void s_dinState1_subid2_hook(GB *gb) {
  BASE(dinState1_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, interactionAnimate_hook, SYM(interactionAnimate), b_+8);
  CYC(b_+8, b_+11);
  TAIL(interactionRunScript);
}

// Din pulled up out of the room once wTmpcfc0 is set, then switches to animation 6.
void s_dinState1_subid3_hook(GB *gb) {
  BASE(dinState1_subid3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45; // Interaction.substate
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (din_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+28) { goto substate1; }
    else if (jt_ == b_+42) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+10, b_+13); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+16); goto animate; }
  CYC(b_+14, b_+16);
  CALL_C(b_+16, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+19);
  CYC(b_+19, b_+22); SET_BC(0xff00);
  CALL_C(b_+22, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+25);
animate:
  CYC(b_+25, b_+28);
  TAIL(interactionAnimate);

substate1:
  CYC(b_+28, b_+30); C = 0x20;
  CALL_C(b_+30, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CALL_C(b_+34, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+37);
  CYC(b_+37, b_+39); L = 0x46; // Interaction.counter1
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0a);
  RET(b_+41); return;

substate2:
  CALL_C(b_+42, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+45);
  if (!(F & FZ)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); A = 0x06;
  CYC(b_+48, b_+51);
  TAIL(interactionSetAnimation);
}

// Scripted Din who, when her script finishes, jumps up and flies off at angle 8.
void s_dinState1_subid4_hook(GB *gb) {
  BASE(dinState1_subid4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45; // Interaction.substate
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (din_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+39) { goto substate1; }
    else if (jt_ == b_+55) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+10, b_+13); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CALL_C(b_+15, interactionAnimate_hook, SYM(interactionAnimate), b_+18);
  CALL_C(b_+18, interactionRunScript_hook, SYM(interactionRunScript), b_+21);
  if (!(F & FC)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+25); SET_BC(0xff20);
  CALL_C(b_+25, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+28);
  CYC(b_+28, b_+30); L = 0x49; // Interaction.angle
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x08);
  CYC(b_+32, b_+34); L = 0x50; // Interaction.speed
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x37);
  CYC(b_+36, b_+39);
  TAIL(interactionIncSubstate);

substate1:
  CALL_C(b_+39, objectApplySpeed_hook, SYM(objectApplySpeed), b_+42);
  CYC(b_+42, b_+44); C = 0x20;
  CALL_C(b_+44, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+47);
  if (!(F & FZ)) { RET_TAKEN(b_+47); return; }
  CYC(b_+47, b_+48);
  CALL_C(b_+48, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+51);
  CYC(b_+51, b_+53); L = 0x60; // Interaction.animCounter
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x20);
substate2:
  CYC(b_+55, b_+58);
  TAIL(interactionAnimate);
}

void s_dinState1_subid6_hook(GB *gb) {
  BASE(dinState1_subid6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectOscillateZ_hook, SYM(objectOscillateZ), b_+3);
  CYC(b_+3, b_+6);
  TAIL(interactionAnimate);
}

// Din in the final cutscene: at stage 4 she jumps, lands, shows her text, then sets stage 5.
void s_dinState1_subid7_hook(GB *gb) {
  BASE(dinState1_subid7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45; // Interaction.substate
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (din_jump_table(gb));
    if (jt_ == b_+14) { goto substate0; }
    else if (jt_ == b_+38) { goto substate1; }
    else if (jt_ == b_+58) { goto substate2; }
    else if (jt_ == b_+79) { goto substate3; }
    else if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), interactionAnimate_hook)) { interactionAnimate_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+14, interactionAnimate_hook, SYM(interactionAnimate), b_+17);
  CYC(b_+17, b_+20); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+20, b_+22); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CALL_C(b_+23, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+26);
  CYC(b_+26, b_+28); L = 0x46; // Interaction.counter1
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x78);
  CYC(b_+30, b_+32); A = 0x08;
  CALL_C(b_+32, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+35);
  CYC(b_+35, b_+38);
  TAIL_S(seasonsFunc_0a_6717);

substate1:
  CALL_C(b_+38, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+41);
  if (!(F & FZ)) { CYCT(b_+41, b_+44); TAIL_S(seasonsFunc_0a_6710); }
  CYC(b_+41, b_+44);
  CALL_C(b_+44, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+47);
  CYC(b_+47, b_+48); alu_xor(gb, A);
  CYC(b_+48, b_+50); L = 0x4f; // Interaction.zh
  CYC(b_+50, b_+51); mem_wr(gb, HL, A);
  CYC(b_+51, b_+53); L = 0x46; // Interaction.counter1
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x1e);
  CYC(b_+55, b_+58);
  TAIL(interactionAnimate);

substate2:
  CALL_C(b_+58, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+61);
  if (!(F & FZ)) { CYCT(b_+61, b_+63); goto animate2; }
  CYC(b_+61, b_+63);
  CALL_C(b_+63, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+66);
  CYC(b_+66, b_+68); L = 0x46; // Interaction.counter1
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x3c);
  CYC(b_+70, b_+73); SET_BC(0x3d09); // TX_3d09
  CALL_C(b_+73, showText_hook, SYM(showText), b_+76);
animate2:
  CYC(b_+76, b_+79);
  TAIL(interactionAnimate);

substate3:
  CYC(b_+79, b_+82); A = mem_rd(gb, wTextIsActive);
  CYC(b_+82, b_+83); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+83, b_+85); goto animate3; }
  CYC(b_+83, b_+85);
  CALL_C(b_+85, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+88);
  if (!(F & FZ)) { CYCT(b_+88, b_+90); goto animate3; }
  CYC(b_+88, b_+90);
  CALL_C(b_+90, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+93);
  CYC(b_+93, b_+96); SET_HL(wTmpcfc0);
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x05);
animate3:
  CYC(b_+98, b_+101);
  TAIL(interactionAnimate);
}

void s_dinState1_subid8_hook(GB *gb) {
  BASE(dinState1_subid8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+5); E = 0x78; // Interaction.var38
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+7, interactionAnimate_hook, SYM(interactionAnimate), b_+10);
  else CYC(b_+7, b_+10);
  CALL_C(b_+10, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+13);
  CYC(b_+13, b_+16);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

// Din discovering Link collapsed: var38 bit 7 freezes the animation, its low bits double its
// speed. When the script ends the season is locked to spring and Link warps to Din's house.
void s_dinState1_subid9_hook(GB *gb) {
  BASE(dinState1_subid9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x78; // Interaction.var38
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto script; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x7f);
  if (!(F & FZ)) CALL_C_CC(b_+9, interactionAnimate_hook, SYM(interactionAnimate), b_+12);
  else CYC(b_+9, b_+12);
  CALL_C(b_+12, interactionAnimate_hook, SYM(interactionAnimate), b_+15);
script:
  CALL_C(b_+15, interactionRunScript_hook, SYM(interactionRunScript), b_+18);
  if (!(F & FC)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); A = 0x30; // GLOBALFLAG_SEASON_ALWAYS_SPRING
  CALL_C(b_+21, setGlobalFlag_hook, SYM(setGlobalFlag), b_+24);
  CYC(b_+24, b_+27); SET_HL(b_+30); // @warpDestVariables
  CYC(b_+27, b_+30);
  TAIL(setWarpDestVariables);
}

// Applies Din's z-speed with gravity $20; once she lands, restarts the jump.
void s_seasonsFunc_0a_6710_hook(GB *gb) {
  BASE(seasonsFunc_0a_6710);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  TAIL_S(seasonsFunc_0a_6717);
}

void s_seasonsFunc_0a_6717_hook(GB *gb) {
  BASE(seasonsFunc_0a_6717);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xff00);
  CYC(b_+3, b_+6);
  TAIL(objectSetSpeedZ);
}

static uint16_t din_d_jump_table(GB *gb) {
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

// INTERAC_DIN
void s_interactionCodea5_hook(GB *gb) {
  BASE(interactionCodea5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (din_d_jump_table(gb));
    if (jt_ == SYM(dinState0) && hook_is(gb, SYM(dinState0), s_dinState0_hook)) { s_dinState0_hook(gb); return; }
    if (jt_ == SYM(dinState1) && hook_is(gb, SYM(dinState1), s_dinState1_hook)) { s_dinState1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

