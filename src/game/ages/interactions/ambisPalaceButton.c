#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/ambisPalaceButton.s (interactionCodebe /
// INTERAC_AMBIS_PALACE_BUTTON), bank 0x0b.

static uint16_t interactionCodebe_jump_table(GB *gb) {
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
// INTERAC_AMBIS_PALACE_BUTTON
// ==================================================================================================
void interactionCodebe_hook(GB *gb) {
  BASE(interactionCodebe);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodebe_jump_table(gb);
    if (target == b_+26) goto state1;
    if (target == b_+77) goto state2;
    // target == 0x70df falls through to state0
  }

  // interactionCodebe@state0
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+20); A = 0x02;
  CALL_C(b_+20, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+23);
  CYC(b_+23, b_+26); interactionIncState_hook(gb); return; // jp

state1: // interactionCodebe@state1
  CALL_C(b_+26, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+29);
  if (!(F & FC)) { CYCT(b_+29, b_+30); ret_effect(gb); return; } // ret nc
  CYC(b_+29, b_+30);
  CALL_C(b_+30, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+33);
  CYC(b_+33, b_+36); A = W8(wActiveTilePos);
  CYC(b_+36, b_+37); alu_cp(gb, L);
  if (!(F & FZ)) { CYCT(b_+37, b_+38); ret_effect(gb); return; } // ret nz
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); A = W8(wLinkInAir);
  CYC(b_+41, b_+42); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+42, b_+43); ret_effect(gb); return; } // ret nz
  CYC(b_+42, b_+43);
  CALL_C(b_+43, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+46);
  if (!(F & FC)) { CYCT(b_+46, b_+47); ret_effect(gb); return; } // ret nc
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(b_+49, b_+52); W8(wDisabledObjects) = A;
  CYC(b_+52, b_+55); W8(wMenuDisabled) = A;
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+57, b_+59); A = 0x2d;
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CALL_C(b_+60, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+63);
  CYC(b_+63, b_+64); C = L;
  CYC(b_+64, b_+66); A = 0x9e;
  CALL_C(b_+66, setTile_hook, SYM(setTile), b_+69);
  CYC(b_+69, b_+71); A = 0x6c;
  CALL_C(b_+71, playSound_b00_hook, SYM(playSound_b00), b_+74);
  CYC(b_+74, b_+77); interactionIncState_hook(gb); return; // jp

state2: // interactionCodebe@state2
  CALL_C(b_+77, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+80);
  if (!(F & FZ)) { CYCT(b_+80, b_+81); ret_effect(gb); return; } // ret nz
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); A = 0x1c;
  CYC(b_+83, b_+86); W8(wCutsceneTrigger) = A;
  CYC(b_+86, b_+89); A = W8(wActiveRoom);
  CYC(b_+89, b_+92); W8(wGenericCutscene_cbbb) = A;
  CYC(b_+92, b_+95); A = W8(wActiveTilePos);
  CYC(b_+95, b_+98); W8(wGenericCutscene_cbbc) = A;
  CYC(b_+98, b_+100); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+104); W8(wGenericCutscene_cbbd) = A;
  CALL_C(b_+104, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+107);
  CYC(b_+107, b_+110); interactionDelete_hook(gb); return; // jp
}
