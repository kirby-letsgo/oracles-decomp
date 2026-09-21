#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/ages/interactions/timeportal.s (INTERAC_TIMEPORTAL), bank $10.

static uint16_t timeportal_jump_table(GB *gb) {
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

void timeportal_updatePalette_hook(GB *gb) {
  BASE(timeportal_updatePalette);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto animate;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  CYC(b_+11, b_+13); alu_and(gb, 0x0b);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
animate:
  CYC(b_+14, b_+17);
  interactionAnimate_hook(gb);
}

void interactionBeginTimewarp_hook(GB *gb) {
  BASE(interactionBeginTimewarp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+3);
  CYC(b_+3, b_+6); SET_HL(w1Link);
  CALL_C(b_+6, objectCopyPosition_hook, SYM(objectCopyPosition), b_+9);
  CYC(b_+9, b_+11); L = 0x08;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x02);
  CYC(b_+13, b_+15); A = 0x81;
  CYC(b_+15, b_+18); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+18, b_+21); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(b_+21, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+24);
  CYC(b_+24, b_+27); mem_wr(gb, wActiveTileIndex, A);
  CYC(b_+27, b_+28); A = L;
  CYC(b_+28, b_+31); mem_wr(gb, wActiveTilePos, A);
  CYC(b_+31, b_+32); A = alu_inc8(gb, A);
  CYC(b_+32, b_+35); mem_wr(gb, wLinkTimeWarpTile, A);
  CYC(b_+35, b_+38); mem_wr(gb, wcde0, A);
  CYC(b_+38, b_+40); A = 0x1b;
  CYC(b_+40, b_+43); mem_wr(gb, wCutsceneTrigger, A);
  CALL_C(b_+43, restartSound_hook, SYM(restartSound), b_+46);
  CYC(b_+46, b_+49);
  interactionDelete_hook(gb);
}

void interactionCodede_hook(GB *gb) {
  BASE(interactionCodede);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+5); mem_wr(gb, wcddd, A);
  CYC(b_+5, b_+8); A = mem_rd(gb, wMenuDisabled);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+12);
    TAIL(objectSetInvisible);
  }
  CYC(b_+9, b_+12);
  CALL_C(b_+12, objectSetVisible_hook, SYM(objectSetVisible), b_+15);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); push_effect(gb, b_+19);
  do { uint16_t jt_ = (timeportal_jump_table(gb));
    if (jt_ == b_+25) { goto state0; }
    else if (jt_ == b_+62) { goto state1; }
    else if (jt_ == b_+70) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CYC(b_+25, b_+27); C = 0xde;
  CALL_C(b_+27, objectFindSameTypeObjectWithID_hook, SYM(objectFindSameTypeObjectWithID), b_+30);
  CYC(b_+30, b_+31); A = H;
  CYC(b_+31, b_+32); alu_cp(gb, D);
  if (!(F & FZ)) {
    CYCT(b_+32, b_+35);
    TAIL(interactionDelete);
  }
  CYC(b_+32, b_+35);
  CYC(b_+35, b_+37); A = 0x03;
  CALL_C(b_+37, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+40);
  CALL_C(b_+40, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+43);
  CYC(b_+43, b_+44); C = A;
  CALL_C(b_+44, interactionIncState_hook, SYM(interactionIncState), b_+47);
  CYC(b_+47, b_+49); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+49, b_+50); mem_wr(gb, HL, C);
  CALL_C(b_+50, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+53);
  if (!(F & FC)) {
    CALL_C_CC(b_+53, interactionIncState_hook, SYM(interactionIncState), b_+56);
  } else {
    CYC(b_+53, b_+56);
  }
  CALL_C(b_+56, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+59);
  CYC(b_+59, b_+62);
  TAIL(objectSetVisible83);
state1:
  CALL_C(b_+62, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+65);
  if (!(F & FC)) {
    CYCT(b_+65, b_+68);
    TAIL(interactionIncState);
  }
  CYC(b_+65, b_+68);
  CYC(b_+68, b_+70);
  TAIL(timeportal_updatePalette);
state2:
  CYC(b_+70, b_+72); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+74); B = A;
  CYC(b_+74, b_+77); A = mem_rd(gb, wPortalPos);
  CYC(b_+77, b_+78); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+78, b_+81);
    TAIL(interactionDelete);
  }
  CYC(b_+78, b_+81);
  CALL_C(b_+81, timeportal_updatePalette_hook, SYM(timeportal_updatePalette), b_+84);
  CYC(b_+84, b_+87); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+87, b_+88); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+88); return; }
  CYC(b_+88, b_+89);
  CALL_C(b_+89, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+92);
  if (!(F & FC)) { RET_TAKEN(b_+92); return; }
  CYC(b_+92, b_+93);
  CALL_C(b_+93, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+96);
  if (!(F & FC)) { RET_TAKEN(b_+96); return; }
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+99); A = 0xff;
  CYC(b_+99, b_+102); mem_wr(gb, wPortalGroup, A);
  interactionBeginTimewarp_hook(gb);
}
