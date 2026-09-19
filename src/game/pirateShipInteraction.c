#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/pirateShip.s (interactionCodec2 /
// INTERAC_PIRATE_SHIP), bank 0x0b. The SAME source file also contains updatePirateShip,
// checkLoadPirateShip and updatePirateShipAngle, which assemble into bank 0x01 and are already
// ported in src/game/pirateShip.c (CYC bound to bank 0x01 there). A single source file can only
// bind one CYC bank per C file, so this bank-0x0b portion (interactionCodec2 and its @-local
// sub-labels) is kept in its own file rather than touching src/game/pirateShip.c.

static uint16_t interactionCodec2_jump_table(GB *gb) {
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

// ==================================================================================================
// INTERAC_PIRATE_SHIP
// ==================================================================================================
void interactionCodec2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x723c, 0x723e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x723e, 0x723f); A = mem_rd(gb, DE);
  {
    CYC(0x723f, 0x7240); push_effect(gb, 0x7240);
    uint16_t target = interactionCodec2_jump_table(gb);
    if (target == 0x72a0) goto subid1;
    if (target == 0x7310) goto subid2;
    // target == 0x7246 falls through to subid0
  }

  // interactionCodec2@subid0
  CYC(0x7246, 0x7248); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7248, 0x7249); A = mem_rd(gb, DE);
  {
    CYC(0x7249, 0x724a); push_effect(gb, 0x724a);
    uint16_t target = interactionCodec2_jump_table(gb);
    if (target == 0x7269) goto subid0State1;
    if (target == 0x261b) { interactionAnimate_hook(gb); return; } // state list's 3rd entry jumps straight into interactionAnimate
    // target == 0x7250 falls through to subid0State0
  }

  // interactionCodec2@subid0State0
  CALL_C(0x7250, interactionInitGraphics_hook, 0x15fb, 0x7253); // SWITCHES THREADS
  CALL_C(0x7253, objectSetVisible82_hook, 0x1e69, 0x7256);
  CYC(0x7256, 0x7259); A = W8(wPirateShipAngle);
  CYC(0x7259, 0x725b); alu_and(gb, 0x03);
  CYC(0x725b, 0x725d); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x725d, 0x725e); mem_wr(gb, DE, A);
  CALL_C(0x725e, interactionSetAnimation_hook, 0x262e, 0x7261);
  CYC(0x7261, 0x7263); A = 0x06;
  CALL_C(0x7263, objectSetCollideRadius_hook, 0x24a1, 0x7266);
  CYC(0x7266, 0x7269); interactionIncState_hook(gb); return; // jp

subid0State1:
  CYC(0x7269, 0x726c); SET_HL(wPirateShipRoom);
  CYC(0x726c, 0x726f); A = W8(wActiveRoom);
  CYC(0x726f, 0x7270); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7270, 0x7273); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x7270, 0x7273);
  CYC(0x7273, 0x7274); L = alu_inc8(gb, L); // inc l
  CYC(0x7274, 0x7275); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl) [wPirateShipY]
  CYC(0x7275, 0x7277); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x7277, 0x7278); mem_wr(gb, DE, A);
  CYC(0x7278, 0x7279); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl) [wPirateShipX]
  CYC(0x7279, 0x727b); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x727b, 0x727c); mem_wr(gb, DE, A);
  CYC(0x727c, 0x727e); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x727e, 0x727f); A = mem_rd(gb, DE);
  CYC(0x727f, 0x7280); alu_cp(gb, mem_rd(gb, HL)); // cp (hl) [wPirateShipAngle]
  CYC(0x7280, 0x7281); A = mem_rd(gb, HL);
  CYC(0x7281, 0x7282); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CALL_C_CC(0x7282, interactionSetAnimation_hook, 0x262e, 0x7285); } else CYC(0x7282, 0x7285); // call nz
  CALL_C(0x7285, objectCheckCollidedWithLink_notDead_hook, 0x1c2e, 0x7288);
  if (!(F & FC)) { CYCT(0x7288, 0x728a); goto animate; } // jr nc
  CYC(0x7288, 0x728a);
  CALL_C(0x728a, checkLinkVulnerable_hook, 0x1d28, 0x728d);
  if (!(F & FC)) { CYCT(0x728d, 0x728f); goto animate; } // jr nc
  CYC(0x728d, 0x728f);
  CYC(0x728f, 0x7292); SET_HL(0x729b); // @warpDest
  CALL_C(0x7292, setWarpDestVariables_hook, 0x1997, 0x7295);
  CYC(0x7295, 0x7298); interactionIncState_hook(gb); return; // jp

animate: // interactionCodec2@animate
  CYC(0x7298, 0x729b); interactionAnimate_hook(gb); return; // jp

  // interactionCodec2@warpDest (0b:729b): pure ROM data (m_HardcodedWarpA table), not ported as
  // code -- referenced only via SET_HL(0x729b) above.

subid1:
  CYC(0x72a0, 0x72a2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x72a2, 0x72a3); A = mem_rd(gb, DE);
  {
    CYC(0x72a3, 0x72a4); push_effect(gb, 0x72a4);
    uint16_t target = interactionCodec2_jump_table(gb);
    if (target == 0x72df) goto subid1And2State1;
    if (target == 0x72f1) goto subid1State2;
    // target == 0x72aa falls through to subid1State0
  }

  // interactionCodec2@subid1State0
  CALL_C(0x72aa, checkIsLinkedGame_hook, 0x1992, 0x72ad);
  if (!(F & FZ)) { CYCT(0x72ad, 0x72b0); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x72ad, 0x72b0);
  CYC(0x72b0, 0x72b2); A = 0x34; // GLOBALFLAG_PIRATES_GONE
  CALL_C(0x72b2, checkGlobalFlag_hook, 0x31f3, 0x72b5);
  if (F & FZ) { CYCT(0x72b5, 0x72b8); interactionDelete_hook(gb); return; } // jp z
  CYC(0x72b5, 0x72b8);
  CALL_C(0x72b8, getThisRoomFlags_hook, 0x197d, 0x72bb);
  CYC(0x72bb, 0x72bd); alu_and(gb, 0x40); // ROOMFLAG_40
  if (!(F & FZ)) { CYCT(0x72bd, 0x72c0); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x72bd, 0x72c0);
  CALL_C(0x72c0, interactionInitGraphics_hook, 0x15fb, 0x72c3); // SWITCHES THREADS
  CYC(0x72c3, 0x72c5); A = 0x03;
  CALL_C(0x72c5, interactionSetAnimation_hook, 0x262e, 0x72c8);
  CYC(0x72c8, 0x72c9); alu_xor(gb, A); // xor a (DIR_UP)
  CYC(0x72c9, 0x72cc); W8(w1Link_direction) = A;

subid1And2State0Common:
  CALL_C(0x72cc, objectSetVisible82_hook, 0x1e69, 0x72cf);
  CYC(0x72cf, 0x72d1); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x72d1, 0x72d3); A = 60;
  CYC(0x72d3, 0x72d4); mem_wr(gb, DE, A);
  CYC(0x72d4, 0x72d6); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(0x72d6, 0x72d9); W8(wDisabledObjects) = A;
  CYC(0x72d9, 0x72dc); W8(wMenuDisabled) = A;
  CYC(0x72dc, 0x72df); interactionIncState_hook(gb); return; // jp

subid1And2State1:
  CALL_C(0x72df, interactionAnimate_hook, 0x261b, 0x72e2);
  CALL_C(0x72e2, interactionDecCounter1_hook, 0x23cc, 0x72e5);
  if (!(F & FZ)) { RET_TAKEN(0x72e5); return; } // ret nz
  CYC(0x72e5, 0x72e6);
  CYC(0x72e6, 0x72e8); mem_wr(gb, HL, 0x80);
  CYC(0x72e8, 0x72eb); SET_BC(0x360c); // TX_360c
  CALL_C(0x72eb, showText_hook, 0x1872, 0x72ee);
  CYC(0x72ee, 0x72f1); interactionIncState_hook(gb); return; // jp

subid1State2:
  CYC(0x72f1, 0x72f3); C = 0x18; // ANGLE_LEFT

moveOffScreen:
  CYC(0x72f3, 0x72f5); B = 0x28; // SPEED_100
  CYC(0x72f5, 0x72f7); E = INTERACTION_BASE + OBJ_ANGLE;
  CALL_C(0x72f7, objectApplyGivenSpeed_hook, 0x2029, 0x72fa);
  CALL_C(0x72fa, interactionAnimate_hook, 0x261b, 0x72fd);
  CALL_C(0x72fd, interactionDecCounter1_hook, 0x23cc, 0x7300);
  if (!(F & FZ)) { RET_TAKEN(0x7300); return; } // ret nz
  CYC(0x7300, 0x7301);
  CALL_C(0x7301, getThisRoomFlags_hook, 0x197d, 0x7304);
  CYC(0x7304, 0x7306); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)
  CYC(0x7306, 0x7307); alu_xor(gb, A); // xor a
  CYC(0x7307, 0x730a); W8(wDisabledObjects) = A;
  CYC(0x730a, 0x730d); W8(wMenuDisabled) = A;
  CYC(0x730d, 0x7310); interactionDelete_hook(gb); return; // jp

subid2:
  CYC(0x7310, 0x7312); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7312, 0x7313); A = mem_rd(gb, DE);
  {
    CYC(0x7313, 0x7314); push_effect(gb, 0x7314);
    uint16_t target = interactionCodec2_jump_table(gb);
    if (target == 0x72df) goto subid1And2State1;
    if (target == 0x733f) goto subid2State2;
    // target == 0x731a falls through to subid2State0
  }

  // interactionCodec2@subid2State0
  CALL_C(0x731a, checkIsLinkedGame_hook, 0x1992, 0x731d);
  if (F & FZ) { CYCT(0x731d, 0x7320); interactionDelete_hook(gb); return; } // jp z
  CYC(0x731d, 0x7320);
  CYC(0x7320, 0x7322); A = 0x34; // GLOBALFLAG_PIRATES_GONE
  CALL_C(0x7322, checkGlobalFlag_hook, 0x31f3, 0x7325);
  if (F & FZ) { CYCT(0x7325, 0x7328); interactionDelete_hook(gb); return; } // jp z
  CYC(0x7325, 0x7328);
  CALL_C(0x7328, getThisRoomFlags_hook, 0x197d, 0x732b);
  CYC(0x732b, 0x732d); alu_and(gb, 0x40); // ROOMFLAG_40
  if (!(F & FZ)) { CYCT(0x732d, 0x7330); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x732d, 0x7330);
  CALL_C(0x7330, interactionInitGraphics_hook, 0x15fb, 0x7333); // SWITCHES THREADS
  CYC(0x7333, 0x7334); alu_xor(gb, A); // xor a
  CALL_C(0x7334, interactionSetAnimation_hook, 0x262e, 0x7337);
  CYC(0x7337, 0x7339); A = 0x01;
  CYC(0x7339, 0x733c); W8(w1Link_direction) = A;
  CYC(0x733c, 0x733f); goto subid1And2State0Common; // jp

subid2State2:
  CYC(0x733f, 0x7341); C = 0x00; // ANGLE_UP
  CYC(0x7341, 0x7343); goto moveOffScreen; // jr
}
