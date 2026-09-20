#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6082, 0x6084); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6084, 0x6085); A = mem_rd(gb, DE);
  {
    CYC(0x6085, 0x6086); push_effect(gb, 0x6086);
    uint16_t target = interactionCode7b_jump_table(gb);
    if (target == 0x60ea) goto state1;
    if (target == 0x6108) goto state2;
    if (target == 0x2680) { objectPreventLinkFromPassing_hook(gb); return; }
  }

  // @state0
  CYC(0x608e, 0x6091); SET_BC(0x0e08);
  CALL_C(0x6091, objectSetCollideRadii_hook, 0x24a9, 0x6094);
  CALL_C(0x6094, interactionInitGraphics_hook, 0x15fb, 0x6097);
  CALL_C(0x6097, objectSetVisible83_hook, 0x1e72, 0x609a);
  CYC(0x609a, 0x609c); A = 0x7e; // PALH_7e
  CALL_C(0x609c, loadPaletteHeader_hook, 0x050b, 0x609f);
  CALL_C(0x609f, getThisRoomFlags_hook, 0x197d, 0x60a2);
  CYC(0x60a2, 0x60a4); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x60a4, 0x60a6); goto initializeOpenedState; } // jr nz
  CYC(0x60a4, 0x60a6);

  // Closed
  CYC(0x60a6, 0x60a9); SET_HL(wRoomCollisions + 0x66);
  CYC(0x60a9, 0x60ab); A = 0x0f;
  CYC(0x60ab, 0x60ac); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x60ac, 0x60ad); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x60ad, 0x60ae); mem_wr(gb, HL, A);
  CYC(0x60ae, 0x60b1); interactionIncState_hook(gb); return; // jp

initializeOpenedState:
  CYC(0x60b1, 0x60b3); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x60b3, 0x60b5); A = 0x03;
  CYC(0x60b5, 0x60b6); mem_wr(gb, DE, A);

  // Move position 10 left or right
  CYC(0x60b6, 0x60b8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x60b8, 0x60b9); A = mem_rd(gb, DE);
  CYC(0x60b9, 0x60ba); alu_or(gb, A);
  CYC(0x60ba, 0x60bc); B = 0x10;
  if (!(F & FZ)) { CYCT(0x60bc, 0x60be); goto L_60c0; } // jr nz
  CYC(0x60bc, 0x60be);
  CYC(0x60be, 0x60c0); B = 0xf0;

L_60c0:
  CYC(0x60c0, 0x60c2); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x60c2, 0x60c3); A = mem_rd(gb, DE);
  CYC(0x60c3, 0x60c4); alu_add(gb, B);
  CYC(0x60c4, 0x60c5); mem_wr(gb, DE, A);
  CYC(0x60c5, 0x60c7); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x60c7, 0x60c9); A = 0x03;
  CYC(0x60c9, 0x60ca); mem_wr(gb, DE, A);

updateSolidityUponOpening:
  CYC(0x60ca, 0x60cd); SET_HL(wRoomCollisions + 0x66);
  CYC(0x60cd, 0x60ce); alu_xor(gb, A);
  CYC(0x60ce, 0x60cf); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x60cf, 0x60d0); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x60d0, 0x60d1); mem_wr(gb, HL, A);
  CYC(0x60d1, 0x60d3); L = 0x46;
  CYC(0x60d3, 0x60d5); mem_wr(gb, HL, 0x02);
  CYC(0x60d5, 0x60d7); L = 0x56;
  CYC(0x60d7, 0x60d9); mem_wr(gb, HL, 0x0a);
  CYC(0x60d9, 0x60db); L = 0x66;
  CYC(0x60db, 0x60dd); mem_wr(gb, HL, 0x08);
  CYC(0x60dd, 0x60df); L = 0x48;
  CYC(0x60df, 0x60e1); mem_wr(gb, HL, 0x01);
  CYC(0x60e1, 0x60e3); L = 0x58;
  CYC(0x60e3, 0x60e5); mem_wr(gb, HL, 0x05);
  CYC(0x60e5, 0x60e7); L = 0x68;
  CYC(0x60e7, 0x60e9); mem_wr(gb, HL, 0x04);
  RET(0x60e9);
  if (gb->pc == 0x615e && gb->sp == sp0_) goto after1_updateSolidityUponOpening;
  return; // ret

// Wait for bit 7 of wActiveTriggers to open the panel.
state1:
  CALL_C(0x60ea, objectPreventLinkFromPassing_hook, 0x2680, 0x60ed);
  CYC(0x60ed, 0x60f0); A = W8(wActiveTriggers);
  CYC(0x60f0, 0x60f2); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(0x60f2); return; } // ret z
  CYC(0x60f2, 0x60f3);
  CYC(0x60f3, 0x60f5); A = 0x81;
  CYC(0x60f5, 0x60f8); W8(wDisabledObjects) = A;
  CYC(0x60f8, 0x60fb); W8(wMenuDisabled) = A;
  CYC(0x60fb, 0x60fd); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x60fd, 0x60ff); A = 0x3c;
  CYC(0x60ff, 0x6100); mem_wr(gb, DE, A);
  CYC(0x6100, 0x6102); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(0x6102, playSound_b00_hook, 0x0c98, 0x6105);
  CYC(0x6105, 0x6108); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(0x6108, objectPreventLinkFromPassing_hook, 0x2680, 0x610b);
  CYC(0x610b, 0x610d); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x610d, 0x610e); A = mem_rd(gb, DE);
  {
    CYC(0x610e, 0x610f); push_effect(gb, 0x610f);
    uint16_t target = interactionCode7b_jump_table(gb);
    if (target == 0x6135) goto substate1;
    if (target == 0x6146) goto substate2;
  }

  // @substate0: delay before opening
  CALL_C(0x6115, interactionDecCounter1_hook, 0x23cc, 0x6118);
  if (!(F & FZ)) { RET_TAKEN(0x6118); return; } // ret nz
  CYC(0x6118, 0x6119);
  CYC(0x6119, 0x611b); mem_wr(gb, HL, 0x80);
  CYC(0x611b, 0x611d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x611d, 0x611e); A = mem_rd(gb, DE);
  CYC(0x611e, 0x611f); alu_or(gb, A);
  CYC(0x611f, 0x6121); A = 0x18; // ANGLE_LEFT
  if (F & FZ) { CYCT(0x6121, 0x6123); goto L_6125; } // jr z
  CYC(0x6121, 0x6123);
  CYC(0x6123, 0x6125); A = 0x08; // ANGLE_RIGHT

L_6125:
  CYC(0x6125, 0x6127); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6127, 0x6128); mem_wr(gb, DE, A);
  CYC(0x6128, 0x612a); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x612a, 0x612c); A = 0x05; // SPEED_20
  CYC(0x612c, 0x612d); mem_wr(gb, DE, A);
  CYC(0x612d, 0x612f); A = 0xb0; // SND_OPENING
  CALL_C(0x612f, playSound_b00_hook, 0x0c98, 0x6132);
  CYC(0x6132, 0x6135); interactionIncSubstate_hook(gb); return; // jp

// @substate1: currently opening
substate1:
  CYC(0x6135, 0x6138); A = W8(wFrameCounter);
  CYC(0x6138, 0x6139); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x6139); return; } // ret nc
  CYC(0x6139, 0x613a);
  CALL_C(0x613a, objectApplySpeed_hook, 0x201d, 0x613d);
  CALL_C(0x613d, interactionDecCounter1_hook, 0x23cc, 0x6140);
  if (!(F & FZ)) { RET_TAKEN(0x6140); return; } // ret nz
  CYC(0x6140, 0x6141);
  CYC(0x6141, 0x6143); mem_wr(gb, HL, 0x1e);
  CYC(0x6143, 0x6146); interactionIncSubstate_hook(gb); return; // jp

// @substate2: done opening
substate2:
  CALL_C(0x6146, interactionDecCounter1_hook, 0x23cc, 0x6149);
  if (!(F & FZ)) { RET_TAKEN(0x6149); return; } // ret nz
  CYC(0x6149, 0x614a);
  CYC(0x614a, 0x614b); alu_xor(gb, A);
  CYC(0x614b, 0x614e); W8(wDisabledObjects) = A;
  CYC(0x614e, 0x6151); W8(wMenuDisabled) = A;
  CYC(0x6151, 0x6154); SET_HL(wActiveTriggers);
  CYC(0x6154, 0x6156); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(0x6156, getThisRoomFlags_hook, 0x197d, 0x6159);
  CYC(0x6159, 0x615b); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)

  CYC(0x615b, 0x615e); push_effect(gb, 0x615e); goto updateSolidityUponOpening;
after1_updateSolidityUponOpening:
  CYC(0x615e, 0x6161); A = W8(wActiveMusic);
  CALL_C(0x6161, playSound_b00_hook, 0x0c98, 0x6164);
  CYC(0x6164, 0x6167); interactionIncState_hook(gb); return; // jp
}
