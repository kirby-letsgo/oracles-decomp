#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode1f), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode1f), (from), (to), true)

// Subid 0, indexed by var03 (the original xh): b0 = dest room, b1 = dest position.
#define specialWarpSubid0WarpData_bank08 SYM(interactionCode1f__subid0__warpData)
// Subid 1: m_HardcodedWarpA ROOM_AGES_5b8, $00, $93, $03
#define specialWarpSubid1WarpDestVariables_bank08 SYM(interactionCode1f__subid1State2__warpDestVariables)
// Subid 2: m_HardcodedWarpB ROOM_AGES_037, $0e, $22, $03
#define specialWarpSubid2WarpDestVariables_bank08 SYM(interactionCode1f__subid2__warpDestVariables)

#define SPECIALOBJECT_DIMITRI 0x0c

static uint16_t specialWarp_jumpTable(GB *gb) {
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

static void specialWarp_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @subid0: trigger a warp when Link dives while touching this object.
static void specialWarp_subid0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1f);
  CALL_C(b_+10, checkInteractionState_hook, SYM(checkInteractionState), b_+13);
  if (F & FZ) {
    CYCT(b_+13, b_+15); goto initialize;
  }
  CYC(b_+13, b_+15);
  // Link must be diving, and have collided with this object without holding anything
  CYC(b_+15, b_+18); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+18, b_+19); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+19, b_+20); ret_effect(gb); return;
  }
  CYC(b_+19, b_+20);
  CALL_C(b_+20, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+23);
  if (!(F & FC)) {
    CYCT(b_+23, b_+24); ret_effect(gb); return;
  }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+30); SET_HL(specialWarpSubid0WarpData_bank08);
  CYC(b_+30, b_+31); specialWarp_addDoubleIndex(gb, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+32, b_+35); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+39); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+39, b_+41); A = 0x87;
  CYC(b_+41, b_+44); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+44, b_+46); A = 0x01;
  CYC(b_+46, b_+49); mem_wr(gb, wWarpTransition, A);
  CYC(b_+49, b_+51); A = 0x03;
  CYC(b_+51, b_+54); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+54, b_+57); interactionDelete_hook(gb);
  return;

initialize:
  // @subid0@initialize: xh is a parameter (warp index) saved to var03 before the
  // short-form position in yh is expanded.
  CYC(b_+61, b_+63); A = 0x01;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+66); A = 0x02;
  CALL_C(b_+66, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+69);
  CYC(b_+69, b_+71); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+74, b_+75); mem_wr(gb, HL, A);
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+77, b_+78); C = mem_rd(gb, HL);
  CYC(b_+78, b_+81); setShortPosition_paramC_hook(gb);
}

// @subid1State0: only exists while riding Dimitri.
static void specialWarp_subid1State0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1f);
  CYC(b_+91, b_+94); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+94, b_+96); alu_cp(gb, SPECIALOBJECT_DIMITRI);
  if (!(F & FZ)) {
    CYCT(b_+96, b_+99); interactionDelete_hook(gb); return;
  }
  CYC(b_+96, b_+99);
  CYC(b_+99, b_+102); SET_BC((SYM(loadTilesetHlpr) + 18));
  CALL_C(b_+102, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+105);
  CALL_C(b_+105, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+108);
  if (!(F & FC)) {
    CALL_C_CC(b_+108, interactionIncState_hook, SYM(interactionIncState), b_+111);
  } else {
    CYC(b_+108, b_+111);
  }
  CYC(b_+111, b_+114); interactionIncState_hook(gb);
}

// @subid1State1: wait for Link to stop touching the object.
static void specialWarp_subid1State1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1f);
  CALL_C(b_+114, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+117);
  if (F & FC) {
    CYCT(b_+117, b_+118); ret_effect(gb); return;
  }
  CYC(b_+117, b_+118);
  CYC(b_+118, b_+121); interactionIncState_hook(gb);
}

// @subid1State2: warp when Link (riding the companion) touches the object.
static void specialWarp_subid1State2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1f);
  CYC(b_+121, b_+122); A = D;
  CYC(b_+122, b_+125); mem_wr(gb, wDisableWarpTiles, A);
  CYC(b_+125, b_+128); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+128, b_+130); alu_cp(gb, w1Companion >> 8);
  if (!(F & FZ)) {
    CYCT(b_+130, b_+131); ret_effect(gb); return;
  }
  CYC(b_+130, b_+131);
  CALL_C(b_+131, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+134);
  if (!(F & FC)) {
    CYCT(b_+134, b_+135); ret_effect(gb); return;
  }
  CYC(b_+134, b_+135);
  CYC(b_+135, b_+138); SET_HL(specialWarpSubid1WarpDestVariables_bank08);
  CYC(b_+138, b_+141); setWarpDestVariables_hook(gb);
}

// @subid1: a warp at the top of a waterfall.
static void specialWarp_subid1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1f);
  CYC(b_+81, b_+83); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); push_effect(gb, b_+85);
  do { uint16_t jt_ = (specialWarp_jumpTable(gb));
    if (jt_ == b_+91) { specialWarp_subid1State0(gb, sp0_); return; }
    else if (jt_ == b_+114) { specialWarp_subid1State1(gb, sp0_); return; }
    else if (jt_ == b_+121) { specialWarp_subid1State2(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// @subid2: a warp in a cave in a waterfall; triggers once the companion's y reaches $a8.
static void specialWarp_subid2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1f);
  CYC(b_+146, b_+147); A = D;
  CYC(b_+147, b_+150); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(b_+150, checkInteractionState_hook, SYM(checkInteractionState), b_+153);
  if (F & FZ) {
    CYCT(b_+153, b_+155); goto initialize;
  }
  CYC(b_+153, b_+155);
  CALL_C(b_+155, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+158);
  if (!(F & FC)) {
    CYCT(b_+158, b_+159); ret_effect(gb); return;
  }
  CYC(b_+158, b_+159);
  CYC(b_+159, b_+162); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+162, b_+164); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(b_+164, b_+165); ret_effect(gb); return;
  }
  CYC(b_+164, b_+165);
  CYC(b_+165, b_+166); H = A;
  CYC(b_+166, b_+168); L = (uint8_t)w1Companion_yh;
  CYC(b_+168, b_+169); A = mem_rd(gb, HL);
  CYC(b_+169, b_+171); alu_cp(gb, 0xa8);
  if (F & FC) {
    CYCT(b_+171, b_+172); ret_effect(gb); return;
  }
  CYC(b_+171, b_+172);
  CYC(b_+172, b_+174); A = 0xff;
  CYC(b_+174, b_+177); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+177, b_+180); SET_HL(specialWarpSubid2WarpDestVariables_bank08);
  CALL_C(b_+180, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+183);
  CYC(b_+183, b_+186); interactionDelete_hook(gb);
  return;

initialize:
  // @subid2@initialize
  CALL_C(b_+191, interactionIncState_hook, SYM(interactionIncState), b_+194);
  CYC(b_+194, SYM(interactionCode20)); interactionSetAlwaysUpdateBit_hook(gb);
}

// INTERAC_SPECIAL_WARP: hardcoded warps that are not part of the room's warp data.
void interactionCode1f_hook(GB *gb) {
  BASE(interactionCode1f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (specialWarp_jumpTable(gb));
    if (jt_ == b_+10) { specialWarp_subid0(gb, sp0_); return; }
    else if (jt_ == b_+81) { specialWarp_subid1(gb, sp0_); return; }
    else if (jt_ == b_+146) { specialWarp_subid2(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
