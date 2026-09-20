#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x618a, checkInteractionState_hook, 0x23fe, 0x618d);
  if (F & FZ) { CYCT(0x618d, 0x618f); goto state0; } // jr z
  CYC(0x618d, 0x618f);

  // interactionCode80@state1
  CYC(0x618f, 0x6191); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6191, 0x6192); A = mem_rd(gb, DE);
  {
    CYC(0x6192, 0x6193); push_effect(gb, 0x6193);
    uint16_t target = interactionCode80_jump_table(gb);
    if (target == 0x61df) goto deleteIfGotRoomItem;
    if (target == 0x261b) { interactionAnimate_hook(gb); return; }
    HANDOFF(target);
  }

state0:
  CALL_C(0x61a9, interactionInitGraphics_hook, 0x15fb, 0x61ac);
  CALL_C(0x61ac, interactionIncState_hook, 0x23e0, 0x61af);
  CALL_C(0x61af, objectSetVisible83_hook, 0x1e72, 0x61b2);
  CYC(0x61b2, 0x61b4); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x61b4, 0x61b5); A = mem_rd(gb, DE);
  {
    CYC(0x61b5, 0x61b6); push_effect(gb, 0x61b6);
    uint16_t target = interactionCode80_jump_table(gb);
    if (target == 0x61cc) { CYC(0x61cc, 0x61cd); ret_effect(gb); return; } // @stub
    if (target == 0x61cd) goto deleteIfMoblinsKeepDestroyed;
    if (target == 0x61d6) goto deleteIfRoomFlagBit7Unset;
    if (target == 0x61df) goto deleteIfGotRoomItem;
    if (target == 0x61e8) goto subid0a;
    HANDOFF(target);
  }

deleteIfMoblinsKeepDestroyed:
  CYC(0x61cd, 0x61cf); A = 0x1a; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(0x61cf, checkGlobalFlag_hook, 0x31f3, 0x61d2);
  if (F & FZ) { RET_TAKEN(0x61d2); return; } // ret z
  CYC(0x61d2, 0x61d3);
  CYC(0x61d3, 0x61d6); interactionDelete_hook(gb); return; // jp

deleteIfRoomFlagBit7Unset:
  CALL_C(0x61d6, getThisRoomFlags_hook, 0x197d, 0x61d9);
  CYC(0x61d9, 0x61db); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(0x61db); return; } // ret nz
  CYC(0x61db, 0x61dc);
  CYC(0x61dc, 0x61df); interactionDelete_hook(gb); return; // jp

deleteIfGotRoomItem:
  CALL_C(0x61df, getThisRoomFlags_hook, 0x197d, 0x61e2);
  CYC(0x61e2, 0x61e4); alu_bit(gb, 5, A); // ROOMFLAG_BIT_ITEM
  if (F & FZ) { RET_TAKEN(0x61e4); return; } // ret z
  CYC(0x61e4, 0x61e5);
  CYC(0x61e5, 0x61e8); interactionDelete_hook(gb); return; // jp

subid0a:
  CALL_C(0x61e8, objectSetVisible80_hook, 0x1e57, 0x61eb);

  // interactionCode80@isSymmetryCityRoom, entered via `call` from here and
  // ending in a tail `jp lookupKey`; the call's own end address (0x61ee) is
  // the resume point, matched against gb->pc/gb->sp exactly like CALL_C does.
  CYC(0x61eb, 0x61ee); push_effect(gb, 0x61ee);
  CYC(0x6207, 0x620a); A = W8(wActiveRoom);
  CYC(0x620a, 0x620b); E = A;
  CYC(0x620b, 0x620e); SET_HL(0x6211); // @symmetryCityRooms
  CYC(0x620e, 0x6211); lookupKey_hook(gb); // jp lookupKey
  if (!(gb->pc == 0x61ee && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }

  if (F & FC) { CYCT(0x61ee, 0x61f0); goto isSymmetryCity; } // jr c
  CYC(0x61ee, 0x61f0);

normalPalette:
  CYC(0x61f0, 0x61f2); A = 0x7d; // PALH_7d
  CYC(0x61f2, 0x61f5); loadPaletteHeader_hook(gb); return; // jp

isSymmetryCity:
  CYC(0x61f5, 0x61f8); A = W8(wActiveGroup);
  CYC(0x61f8, 0x61f9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x61f9, 0x61fb); goto ruinedSymmetryPalette; } // jr nz
  CYC(0x61f9, 0x61fb);
  CALL_C(0x61fb, getThisRoomFlags_hook, 0x197d, 0x61fe);
  CYC(0x61fe, 0x6200); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x6200, 0x6202); goto normalPalette; } // jr nz
  CYC(0x6200, 0x6202);

ruinedSymmetryPalette:
  CYC(0x6202, 0x6204); A = 0x7c; // PALH_7c
  CYC(0x6204, 0x6207); loadPaletteHeader_hook(gb); return; // jp
}
