#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode68_jump_table(GB *gb) {
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

static void interactionCode68_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_ROSA
//
// interactionCode68@initGraphicsAndIncState (0x4ce6-0x4cee) is dead code in the ROM -- no call,
// jump, or fallthrough reaches it from anywhere in this routine -- so its bytes are omitted here.
//
// initGraphicsAndLoadScript and loadScriptFromTableAndInitGraphics are each called once (from
// subid00@state0 and subid01@state0), then jr into loadScriptAndIncState / loadScriptFromTable-
// AndIncState at the same stack depth as their caller. Both of those tail-jump into
// interactionIncState_hook without ever executing their own ret, so it is that external hook's
// own return that eventually pops the still-outstanding outer call and resumes this function --
// handled below with the same tail-jump-then-check pattern used for external hooks elsewhere.
// getScript is called from both of those (loadScriptAndIncState, loadScriptFromTableAndIncState),
// each one level deeper than the top level because of that same still-outstanding outer call, so
// its own two resume checks compare against sp0_-2, not bare sp0_.
void interactionCode68_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c84, 0x4c86); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4c86, 0x4c87); A = mem_rd(gb, DE);
  {
    CYC(0x4c87, 0x4c88); push_effect(gb, 0x4c88);
    uint16_t target = interactionCode68_jump_table(gb);
    if (target == 0x4c8c) goto subid00;
    goto subid01;
  }

subid00:
  CALL_C(0x4c8c, checkInteractionState_hook, 0x23fe, 0x4c8f);
  if (!(F & FZ)) { CYCT(0x4c8f, 0x4c91); goto subid00_state1; } // jr nz
  CYC(0x4c8f, 0x4c91);

  // interactionCode68@subid00@state0
  CALL_C(0x4c91, checkIsLinkedGame_hook, 0x1992, 0x4c94);
  if (F & FZ) { CYCT(0x4c94, 0x4c97); interactionDelete_hook(gb); return; } // jp z
  CYC(0x4c94, 0x4c97);
  CYC(0x4c97, 0x4c9a); A = W8(wEssencesObtained);
  CYC(0x4c9a, 0x4c9c); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(0x4c9c, 0x4c9f); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x4c9c, 0x4c9f);
  CYC(0x4c9f, 0x4ca2); push_effect(gb, 0x4ca2); goto initGraphicsAndLoadScript;
afterInitGraphicsAndLoadScript:
  CALL_C(0x4ca2, objectSetVisiblec2_hook, 0x1e45, 0x4ca5);
  CALL_C(0x4ca5, getThisRoomFlags_hook, 0x197d, 0x4ca8);
  CYC(0x4ca8, 0x4caa); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x4caa, 0x4cac); goto subid00_alreadyGaveShovel; } // jr nz
  CYC(0x4caa, 0x4cac);
  CALL_C(0x4cac, getFreeInteractionSlot_hook, 0x3aef, 0x4caf);
  if (!(F & FZ)) { RET_TAKEN(0x4caf); return; } // ret nz
  CYC(0x4caf, 0x4cb0);
  CYC(0x4cb0, 0x4cb2); mem_wr(gb, HL, 0x6b); // INTERAC_MISCELLANEOUS_1
  CYC(0x4cb2, 0x4cb3); L = alu_inc8(gb, L);
  CYC(0x4cb3, 0x4cb5); mem_wr(gb, HL, 0x09);
  CYC(0x4cb5, 0x4cb7); L = INTERACTION_BASE + OBJ_RELATED1 + 1; // Interaction.relatedObj1+1
  CYC(0x4cb7, 0x4cb8); A = D;
  CYC(0x4cb8, 0x4cb9); mem_wr(gb, HL, A);
  RET(0x4cb9); return; // ret

subid00_alreadyGaveShovel:
  CYC(0x4cba, 0x4cbd); SET_HL(0x7108); // mainScripts.rosa_subid00Script_alreadyGaveShovel
  CYC(0x4cbd, 0x4cc0); interactionSetScript_hook(gb); return; // jp

subid00_state1:
  CALL_C(0x4cc0, interactionRunScript_hook, 0x2552, 0x4cc3);
  CYC(0x4cc3, 0x4cc5); A = 0x15; // TREASURE_SHOVEL
  CALL_C(0x4cc5, checkTreasureObtained_hook, 0x1748, 0x4cc8);
  if (F & FC) { CYCT(0x4cc8, 0x4ccb); npcFaceLinkAndAnimate_hook(gb); return; } // jp c
  CYC(0x4cc8, 0x4ccb);
  CYC(0x4ccb, 0x4cce); interactionAnimateAsNpc_hook(gb); return; // jp

subid01:
  CALL_C(0x4cce, checkInteractionState_hook, 0x23fe, 0x4cd1);
  if (!(F & FZ)) { CYCT(0x4cd1, 0x4cd3); goto subid01_state1; } // jr nz
  CYC(0x4cd1, 0x4cd3);

  // interactionCode68@subid01@state0
  CYC(0x4cd3, 0x4cd6); push_effect(gb, 0x4cd6); goto loadScriptFromTableAndInitGraphics;
afterLoadScriptFromTableAndInitGraphics:
  CYC(0x4cd6, 0x4cd8); L = INTERACTION_BASE + 0x37; // Interaction.var37
  CYC(0x4cd8, 0x4cda); mem_wr(gb, HL, 0x04);
  CALL_C(0x4cda, interactionRunScript_hook, 0x2552, 0x4cdd);

subid01_state1:
  CALL_C(0x4cdd, interactionRunScript_hook, 0x2552, 0x4ce0);
  if (F & FC) { CYCT(0x4ce0, 0x4ce3); interactionDelete_hook(gb); return; } // jp c
  CYC(0x4ce0, 0x4ce3);
  CYC(0x4ce3, 0x4ce6); npcFaceLinkAndAnimate_hook(gb); return; // jp

initGraphicsAndLoadScript:
  CALL_C(0x4cef, interactionInitGraphics_hook, 0x15fb, 0x4cf2);
  CALL_C(0x4cf2, objectMarkSolidPosition_hook, 0x24f0, 0x4cf5);
  CYC(0x4cf5, 0x4cf7); goto loadScriptAndIncState;

loadScriptFromTableAndInitGraphics:
  CALL_C(0x4cf7, interactionInitGraphics_hook, 0x15fb, 0x4cfa);
  CALL_C(0x4cfa, objectMarkSolidPosition_hook, 0x24f0, 0x4cfd);
  CYC(0x4cfd, 0x4cff); goto loadScriptFromTableAndIncState;

loadScriptAndIncState:
  CYC(0x4cff, 0x4d02); push_effect(gb, 0x4d02); goto getScript;
afterGetScript1:
  CALL_C(0x4d02, interactionSetScript_hook, 0x2544, 0x4d05);
  CYC(0x4d05, 0x4d08); interactionIncState_hook(gb);
  if (gb->pc == 0x4ca2 && gb->sp == sp0_) goto afterInitGraphicsAndLoadScript;
  return; // jp

loadScriptFromTableAndIncState:
  CYC(0x4d08, 0x4d0b); push_effect(gb, 0x4d0b); goto getScript;
afterGetScript2:
  CYC(0x4d0b, 0x4d0c); E = alu_inc8(gb, E); // Interaction.var03
  CYC(0x4d0c, 0x4d0d); A = mem_rd(gb, DE);
  CYC(0x4d0d, 0x4d0e); interactionCode68_addDoubleIndexToHl_from_rst(gb, 0x4d0e);
  CYC(0x4d0e, 0x4d0f); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4d0f, 0x4d10); H = mem_rd(gb, HL);
  CYC(0x4d10, 0x4d11); L = A;
  CALL_C(0x4d11, interactionSetScript_hook, 0x2544, 0x4d14);
  CYC(0x4d14, 0x4d17); interactionIncState_hook(gb);
  if (gb->pc == 0x4cd6 && gb->sp == sp0_) goto afterLoadScriptFromTableAndInitGraphics;
  return; // jp

getScript:
  CYC(0x4d17, 0x4d19); A = 0x1c; // >TX_1c00
  CALL_C(0x4d19, interactionSetHighTextIndex_hook, 0x253b, 0x4d1c);
  CYC(0x4d1c, 0x4d1e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4d1e, 0x4d1f); A = mem_rd(gb, DE);
  CYC(0x4d1f, 0x4d22); SET_HL(0x4d27); // interactionCode68@scriptTable
  CYC(0x4d22, 0x4d23); interactionCode68_addDoubleIndexToHl_from_rst(gb, 0x4d23);
  CYC(0x4d23, 0x4d24); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4d24, 0x4d25); H = mem_rd(gb, HL);
  CYC(0x4d25, 0x4d26); L = A;
  RET(0x4d26);
  if (gb->pc == 0x4d02 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterGetScript1;
  if (gb->pc == 0x4d0b && gb->sp == (uint16_t)(sp0_ - 2)) goto afterGetScript2;
  return; // ret
}
