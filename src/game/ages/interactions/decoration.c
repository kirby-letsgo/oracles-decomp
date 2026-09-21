#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode80_jump_table(GB *gb) {
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

// INTERAC_DECORATION
void interactionCode80_hook(GB *gb) {
  BASE(interactionCode80);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; } // jr z
  CYC(b_+3, b_+5);

  // interactionCode80@state1
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  {
    CYC(b_+8, b_+9); push_effect(gb, b_+9);
    uint16_t target = interactionCode80_jump_table(gb);
    if (target == b_+85) goto deleteIfGotRoomItem;
    if (target == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; }
    HANDOFF(target);
  }

state0:
  CALL_C(b_+31, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+34);
  CALL_C(b_+34, interactionIncState_hook, SYM(interactionIncState), b_+37);
  CALL_C(b_+37, objectSetVisible83_hook, SYM(objectSetVisible83), b_+40);
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  {
    CYC(b_+43, b_+44); push_effect(gb, b_+44);
    uint16_t target = interactionCode80_jump_table(gb);
    if (target == b_+66) { CYC(b_+66, b_+67); ret_effect(gb); return; } // @stub
    if (target == b_+67) goto deleteIfMoblinsKeepDestroyed;
    if (target == b_+76) goto deleteIfRoomFlagBit7Unset;
    if (target == b_+85) goto deleteIfGotRoomItem;
    if (target == b_+94) goto subid0a;
    HANDOFF(target);
  }

deleteIfMoblinsKeepDestroyed:
  CYC(b_+67, b_+69); A = 0x1a; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(b_+69, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+72);
  if (F & FZ) { RET_TAKEN(b_+72); return; } // ret z
  CYC(b_+72, b_+73);
  CYC(b_+73, b_+76); TAIL(interactionDelete); // jp

deleteIfRoomFlagBit7Unset:
  CALL_C(b_+76, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+79);
  CYC(b_+79, b_+81); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; } // ret nz
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+85); TAIL(interactionDelete); // jp

deleteIfGotRoomItem:
  CALL_C(b_+85, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+88);
  CYC(b_+88, b_+90); alu_bit(gb, 5, A); // ROOMFLAG_BIT_ITEM
  if (F & FZ) { RET_TAKEN(b_+90); return; } // ret z
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+94); TAIL(interactionDelete); // jp

subid0a:
  CALL_C(b_+94, objectSetVisible80_hook, SYM(objectSetVisible80), b_+97);

  // interactionCode80@isSymmetryCityRoom, entered via `call` from here and
  // ending in a tail `jp lookupKey`; the call's own end address (0x61ee) is
  // the resume point, matched against gb->pc/gb->sp exactly like CALL_C does.
  CYC(b_+97, b_+100); push_effect(gb, b_+100);
  CYC(b_+125, b_+128); A = W8(wActiveRoom);
  CYC(b_+128, b_+129); E = A;
  CYC(b_+129, b_+132); SET_HL(b_+135); // @symmetryCityRooms
  CYC(b_+132, b_+135); lookupKey_hook(gb); // jp lookupKey
  if (!(gb->pc == b_+100 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }

  if (F & FC) { CYCT(b_+100, b_+102); goto isSymmetryCity; } // jr c
  CYC(b_+100, b_+102);

normalPalette:
  CYC(b_+102, b_+104); A = 0x7d; // PALH_7d
  CYC(b_+104, b_+107); TAIL(loadPaletteHeader); // jp

isSymmetryCity:
  CYC(b_+107, b_+110); A = W8(wActiveGroup);
  CYC(b_+110, b_+111); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+111, b_+113); goto ruinedSymmetryPalette; } // jr nz
  CYC(b_+111, b_+113);
  CALL_C(b_+113, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+116);
  CYC(b_+116, b_+118); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+118, b_+120); goto normalPalette; } // jr nz
  CYC(b_+118, b_+120);

ruinedSymmetryPalette:
  CYC(b_+120, b_+122); A = 0x7c; // PALH_7c
  CYC(b_+122, b_+125); TAIL(loadPaletteHeader); // jp
}
