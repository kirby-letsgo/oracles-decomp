#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x70d5, 0x70d7); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x70d7, 0x70d8); A = mem_rd(gb, DE);
  {
    CYC(0x70d8, 0x70d9); push_effect(gb, 0x70d9);
    uint16_t target = interactionCodebe_jump_table(gb);
    if (target == 0x70ef) goto state1;
    if (target == 0x7122) goto state2;
    // target == 0x70df falls through to state0
  }

  // interactionCodebe@state0
  CALL_C(0x70df, getThisRoomFlags_hook, 0x197d, 0x70e2);
  CYC(0x70e2, 0x70e4); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x70e4, 0x70e7); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x70e4, 0x70e7);
  CYC(0x70e7, 0x70e9); A = 0x02;
  CALL_C(0x70e9, objectSetCollideRadius_hook, 0x24a1, 0x70ec);
  CYC(0x70ec, 0x70ef); interactionIncState_hook(gb); return; // jp

state1: // interactionCodebe@state1
  CALL_C(0x70ef, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x70f2);
  if (!(F & FC)) { CYCT(0x70f2, 0x70f3); ret_effect(gb); return; } // ret nc
  CYC(0x70f2, 0x70f3);
  CALL_C(0x70f3, objectGetTileAtPosition_hook, 0x1444, 0x70f6);
  CYC(0x70f6, 0x70f9); A = W8(wActiveTilePos);
  CYC(0x70f9, 0x70fa); alu_cp(gb, L);
  if (!(F & FZ)) { CYCT(0x70fa, 0x70fb); ret_effect(gb); return; } // ret nz
  CYC(0x70fa, 0x70fb);
  CYC(0x70fb, 0x70fe); A = W8(wLinkInAir);
  CYC(0x70fe, 0x70ff); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x70ff, 0x7100); ret_effect(gb); return; } // ret nz
  CYC(0x70ff, 0x7100);
  CALL_C(0x7100, checkLinkVulnerable_hook, 0x1d28, 0x7103);
  if (!(F & FC)) { CYCT(0x7103, 0x7104); ret_effect(gb); return; } // ret nc
  CYC(0x7103, 0x7104);
  CYC(0x7104, 0x7106); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(0x7106, 0x7109); W8(wDisabledObjects) = A;
  CYC(0x7109, 0x710c); W8(wMenuDisabled) = A;
  CYC(0x710c, 0x710e); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x710e, 0x7110); A = 0x2d;
  CYC(0x7110, 0x7111); mem_wr(gb, DE, A);
  CALL_C(0x7111, objectGetTileAtPosition_hook, 0x1444, 0x7114);
  CYC(0x7114, 0x7115); C = L;
  CYC(0x7115, 0x7117); A = 0x9e;
  CALL_C(0x7117, setTile_hook, 0x3a9c, 0x711a);
  CYC(0x711a, 0x711c); A = 0x6c;
  CALL_C(0x711c, playSound_b00_hook, 0x0c98, 0x711f);
  CYC(0x711f, 0x7122); interactionIncState_hook(gb); return; // jp

state2: // interactionCodebe@state2
  CALL_C(0x7122, interactionDecCounter1_hook, 0x23cc, 0x7125);
  if (!(F & FZ)) { CYCT(0x7125, 0x7126); ret_effect(gb); return; } // ret nz
  CYC(0x7125, 0x7126);
  CYC(0x7126, 0x7128); A = 0x1c;
  CYC(0x7128, 0x712b); W8(wCutsceneTrigger) = A;
  CYC(0x712b, 0x712e); A = W8(wActiveRoom);
  CYC(0x712e, 0x7131); W8(wGenericCutscene_cbbb) = A;
  CYC(0x7131, 0x7134); A = W8(wActiveTilePos);
  CYC(0x7134, 0x7137); W8(wGenericCutscene_cbbc) = A;
  CYC(0x7137, 0x7139); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7139, 0x713a); A = mem_rd(gb, DE);
  CYC(0x713a, 0x713d); W8(wGenericCutscene_cbbd) = A;
  CALL_C(0x713d, fadeoutToWhite_hook, 0x326c, 0x7140);
  CYC(0x7140, 0x7143); interactionDelete_hook(gb); return; // jp
}
