#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode7b_jump_table(GB *gb) {
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

// INTERAC_STONE_PANEL
void interactionCode7b_hook(GB *gb) {
  BASE(interactionCode7b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode7b_jump_table(gb);
    if (target == b_+104) goto state1;
    if (target == b_+134) goto state2;
    if (target == SYM(objectPreventLinkFromPassing)) { objectPreventLinkFromPassing_hook(gb); return; }
  }

  // @state0
  CYC(b_+12, b_+15); SET_BC(0x0e08);
  CALL_C(b_+15, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+18);
  CALL_C(b_+18, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+21);
  CALL_C(b_+21, objectSetVisible83_hook, SYM(objectSetVisible83), b_+24);
  CYC(b_+24, b_+26); A = 0x7e; // PALH_7e
  CALL_C(b_+26, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+29);
  CALL_C(b_+29, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+32);
  CYC(b_+32, b_+34); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto initializeOpenedState; } // jr nz
  CYC(b_+34, b_+36);

  // Closed
  CYC(b_+36, b_+39); SET_HL(wRoomCollisions + 0x66);
  CYC(b_+39, b_+41); A = 0x0f;
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CYC(b_+44, b_+47); TAIL(interactionIncState); // jp

initializeOpenedState:
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+49, b_+51); A = 0x03;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);

  // Move position 10 left or right
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+56); alu_or(gb, A);
  CYC(b_+56, b_+58); B = 0x10;
  if (!(F & FZ)) { CYCT(b_+58, b_+60); goto L_60c0; } // jr nz
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); B = 0xf0;

L_60c0:
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); alu_add(gb, B);
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+69, b_+71); A = 0x03;
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);

updateSolidityUponOpening:
  CYC(b_+72, b_+75); SET_HL(wRoomCollisions + 0x66);
  CYC(b_+75, b_+76); alu_xor(gb, A);
  CYC(b_+76, b_+77); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+77, b_+78); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
  CYC(b_+79, b_+81); L = 0x46;
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x02);
  CYC(b_+83, b_+85); L = 0x56;
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x0a);
  CYC(b_+87, b_+89); L = 0x66;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x08);
  CYC(b_+91, b_+93); L = 0x48;
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x01);
  CYC(b_+95, b_+97); L = 0x58;
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x05);
  CYC(b_+99, b_+101); L = 0x68;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x04);
  RET(b_+103);
  if (gb->pc == b_+220 && gb->sp == sp0_) goto after1_updateSolidityUponOpening;
  return; // ret

// Wait for bit 7 of wActiveTriggers to open the panel.
state1:
  CALL_C(b_+104, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+107);
  CYC(b_+107, b_+110); A = W8(wActiveTriggers);
  CYC(b_+110, b_+112); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+112); return; } // ret z
  CYC(b_+112, b_+113);
  CYC(b_+113, b_+115); A = 0x81;
  CYC(b_+115, b_+118); W8(wDisabledObjects) = A;
  CYC(b_+118, b_+121); W8(wMenuDisabled) = A;
  CYC(b_+121, b_+123); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+123, b_+125); A = 0x3c;
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+128); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+128, playSound_b00_hook, SYM(playSound_b00), b_+131);
  CYC(b_+131, b_+134); TAIL(interactionIncState); // jp

state2:
  CALL_C(b_+134, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+137);
  CYC(b_+137, b_+139); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+139, b_+140); A = mem_rd(gb, DE);
  {
    CYC(b_+140, b_+141); push_effect(gb, b_+141);
    uint16_t target = interactionCode7b_jump_table(gb);
    if (target == b_+179) goto substate1;
    if (target == b_+196) goto substate2;
  }

  // @substate0: delay before opening
  CALL_C(b_+147, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+150);
  if (!(F & FZ)) { RET_TAKEN(b_+150); return; } // ret nz
  CYC(b_+150, b_+151);
  CYC(b_+151, b_+153); mem_wr(gb, HL, 0x80);
  CYC(b_+153, b_+155); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+155, b_+156); A = mem_rd(gb, DE);
  CYC(b_+156, b_+157); alu_or(gb, A);
  CYC(b_+157, b_+159); A = 0x18; // ANGLE_LEFT
  if (F & FZ) { CYCT(b_+159, b_+161); goto L_6125; } // jr z
  CYC(b_+159, b_+161);
  CYC(b_+161, b_+163); A = 0x08; // ANGLE_RIGHT

L_6125:
  CYC(b_+163, b_+165); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+165, b_+166); mem_wr(gb, DE, A);
  CYC(b_+166, b_+168); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+168, b_+170); A = 0x05; // SPEED_20
  CYC(b_+170, b_+171); mem_wr(gb, DE, A);
  CYC(b_+171, b_+173); A = 0xb0; // SND_OPENING
  CALL_C(b_+173, playSound_b00_hook, SYM(playSound_b00), b_+176);
  CYC(b_+176, b_+179); TAIL(interactionIncSubstate); // jp

// @substate1: currently opening
substate1:
  CYC(b_+179, b_+182); A = W8(wFrameCounter);
  CYC(b_+182, b_+183); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+183); return; } // ret nc
  CYC(b_+183, b_+184);
  CALL_C(b_+184, objectApplySpeed_hook, SYM(objectApplySpeed), b_+187);
  CALL_C(b_+187, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+190);
  if (!(F & FZ)) { RET_TAKEN(b_+190); return; } // ret nz
  CYC(b_+190, b_+191);
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x1e);
  CYC(b_+193, b_+196); TAIL(interactionIncSubstate); // jp

// @substate2: done opening
substate2:
  CALL_C(b_+196, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+199);
  if (!(F & FZ)) { RET_TAKEN(b_+199); return; } // ret nz
  CYC(b_+199, b_+200);
  CYC(b_+200, b_+201); alu_xor(gb, A);
  CYC(b_+201, b_+204); W8(wDisabledObjects) = A;
  CYC(b_+204, b_+207); W8(wMenuDisabled) = A;
  CYC(b_+207, b_+210); SET_HL(wActiveTriggers);
  CYC(b_+210, b_+212); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(b_+212, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+215);
  CYC(b_+215, b_+217); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)

  CYC(b_+217, b_+220); push_effect(gb, b_+220); goto updateSolidityUponOpening;
after1_updateSolidityUponOpening:
  CYC(b_+220, b_+223); A = W8(wActiveMusic);
  CALL_C(b_+223, playSound_b00_hook, SYM(playSound_b00), b_+226);
  CYC(b_+226, b_+229); TAIL(interactionIncState); // jp
}
