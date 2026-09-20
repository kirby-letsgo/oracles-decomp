#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodec2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodec2_jump_table(gb);
    if (target == b_+100) goto subid1;
    if (target == b_+212) goto subid2;
    // target == 0x7246 falls through to subid0
  }

  // interactionCodec2@subid0
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  {
    CYC(b_+13, b_+14); push_effect(gb, b_+14);
    uint16_t target = interactionCodec2_jump_table(gb);
    if (target == b_+45) goto subid0State1;
    if (target == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; } // state list's 3rd entry jumps straight into interactionAnimate
    // target == 0x7250 falls through to subid0State0
  }

  // interactionCodec2@subid0State0
  CALL_C(b_+20, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+23); // SWITCHES THREADS
  CALL_C(b_+23, objectSetVisible82_hook, SYM(objectSetVisible82), b_+26);
  CYC(b_+26, b_+29); A = W8(wPirateShipAngle);
  CYC(b_+29, b_+31); alu_and(gb, 0x03);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CALL_C(b_+34, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+37);
  CYC(b_+37, b_+39); A = 0x06;
  CALL_C(b_+39, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+42);
  CYC(b_+42, b_+45); interactionIncState_hook(gb); return; // jp

subid0State1:
  CYC(b_+45, b_+48); SET_HL(wPirateShipRoom);
  CYC(b_+48, b_+51); A = W8(wActiveRoom);
  CYC(b_+51, b_+52); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+52, b_+55); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+52, b_+55);
  CYC(b_+55, b_+56); L = alu_inc8(gb, L); // inc l
  CYC(b_+56, b_+57); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl) [wPirateShipY]
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CYC(b_+60, b_+61); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl) [wPirateShipX]
  CYC(b_+61, b_+63); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+66); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+68); alu_cp(gb, mem_rd(gb, HL)); // cp (hl) [wPirateShipAngle]
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CALL_C_CC(b_+70, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+73); } else CYC(b_+70, b_+73); // call nz
  CALL_C(b_+73, objectCheckCollidedWithLink_notDead_hook, SYM(objectCheckCollidedWithLink_notDead), b_+76);
  if (!(F & FC)) { CYCT(b_+76, b_+78); goto animate; } // jr nc
  CYC(b_+76, b_+78);
  CALL_C(b_+78, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+81);
  if (!(F & FC)) { CYCT(b_+81, b_+83); goto animate; } // jr nc
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+86); SET_HL(b_+95); // @warpDest
  CALL_C(b_+86, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+89);
  CYC(b_+89, b_+92); interactionIncState_hook(gb); return; // jp

animate: // interactionCodec2@animate
  CYC(b_+92, b_+95); interactionAnimate_hook(gb); return; // jp

  // interactionCodec2@warpDest (0b:729b): pure ROM data (m_HardcodedWarpA table), not ported as
  // code -- referenced only via SET_HL(0x729b) above.

subid1:
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  {
    CYC(b_+103, b_+104); push_effect(gb, b_+104);
    uint16_t target = interactionCodec2_jump_table(gb);
    if (target == b_+163) goto subid1And2State1;
    if (target == b_+181) goto subid1State2;
    // target == 0x72aa falls through to subid1State0
  }

  // interactionCodec2@subid1State0
  CALL_C(b_+110, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+113);
  if (!(F & FZ)) { CYCT(b_+113, b_+116); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+113, b_+116);
  CYC(b_+116, b_+118); A = 0x34; // GLOBALFLAG_PIRATES_GONE
  CALL_C(b_+118, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+121);
  if (F & FZ) { CYCT(b_+121, b_+124); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+121, b_+124);
  CALL_C(b_+124, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+127);
  CYC(b_+127, b_+129); alu_and(gb, 0x40); // ROOMFLAG_40
  if (!(F & FZ)) { CYCT(b_+129, b_+132); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+129, b_+132);
  CALL_C(b_+132, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+135); // SWITCHES THREADS
  CYC(b_+135, b_+137); A = 0x03;
  CALL_C(b_+137, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+140);
  CYC(b_+140, b_+141); alu_xor(gb, A); // xor a (DIR_UP)
  CYC(b_+141, b_+144); W8(w1Link_direction) = A;

subid1And2State0Common:
  CALL_C(b_+144, objectSetVisible82_hook, SYM(objectSetVisible82), b_+147);
  CYC(b_+147, b_+149); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+149, b_+151); A = 60;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CYC(b_+152, b_+154); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(b_+154, b_+157); W8(wDisabledObjects) = A;
  CYC(b_+157, b_+160); W8(wMenuDisabled) = A;
  CYC(b_+160, b_+163); interactionIncState_hook(gb); return; // jp

subid1And2State1:
  CALL_C(b_+163, interactionAnimate_hook, SYM(interactionAnimate), b_+166);
  CALL_C(b_+166, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+169);
  if (!(F & FZ)) { RET_TAKEN(b_+169); return; } // ret nz
  CYC(b_+169, b_+170);
  CYC(b_+170, b_+172); mem_wr(gb, HL, 0x80);
  CYC(b_+172, b_+175); SET_BC(0x360c); // TX_360c
  CALL_C(b_+175, showText_hook, SYM(showText), b_+178);
  CYC(b_+178, b_+181); interactionIncState_hook(gb); return; // jp

subid1State2:
  CYC(b_+181, b_+183); C = 0x18; // ANGLE_LEFT

moveOffScreen:
  CYC(b_+183, b_+185); B = 0x28; // SPEED_100
  CYC(b_+185, b_+187); E = INTERACTION_BASE + OBJ_ANGLE;
  CALL_C(b_+187, objectApplyGivenSpeed_hook, SYM(objectApplyGivenSpeed), b_+190);
  CALL_C(b_+190, interactionAnimate_hook, SYM(interactionAnimate), b_+193);
  CALL_C(b_+193, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+196);
  if (!(F & FZ)) { RET_TAKEN(b_+196); return; } // ret nz
  CYC(b_+196, b_+197);
  CALL_C(b_+197, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+200);
  CYC(b_+200, b_+202); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)
  CYC(b_+202, b_+203); alu_xor(gb, A); // xor a
  CYC(b_+203, b_+206); W8(wDisabledObjects) = A;
  CYC(b_+206, b_+209); W8(wMenuDisabled) = A;
  CYC(b_+209, b_+212); interactionDelete_hook(gb); return; // jp

subid2:
  CYC(b_+212, b_+214); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+214, b_+215); A = mem_rd(gb, DE);
  {
    CYC(b_+215, b_+216); push_effect(gb, b_+216);
    uint16_t target = interactionCodec2_jump_table(gb);
    if (target == b_+163) goto subid1And2State1;
    if (target == b_+259) goto subid2State2;
    // target == 0x731a falls through to subid2State0
  }

  // interactionCodec2@subid2State0
  CALL_C(b_+222, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+225);
  if (F & FZ) { CYCT(b_+225, b_+228); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+225, b_+228);
  CYC(b_+228, b_+230); A = 0x34; // GLOBALFLAG_PIRATES_GONE
  CALL_C(b_+230, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+233);
  if (F & FZ) { CYCT(b_+233, b_+236); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+233, b_+236);
  CALL_C(b_+236, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+239);
  CYC(b_+239, b_+241); alu_and(gb, 0x40); // ROOMFLAG_40
  if (!(F & FZ)) { CYCT(b_+241, b_+244); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+241, b_+244);
  CALL_C(b_+244, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+247); // SWITCHES THREADS
  CYC(b_+247, b_+248); alu_xor(gb, A); // xor a
  CALL_C(b_+248, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+251);
  CYC(b_+251, b_+253); A = 0x01;
  CYC(b_+253, b_+256); W8(w1Link_direction) = A;
  CYC(b_+256, b_+259); goto subid1And2State0Common; // jp

subid2State2:
  CYC(b_+259, b_+261); C = 0x00; // ANGLE_UP
  CYC(b_+261, b_+263); goto moveOffScreen; // jr
}
