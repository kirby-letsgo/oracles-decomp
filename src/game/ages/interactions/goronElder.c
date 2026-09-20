#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode8b_jump_table(GB *gb) {
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

static void interactionCode8b_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_GORON_ELDER
void interactionCode8b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a6d, 0x6a6f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6a6f, 0x6a70); A = mem_rd(gb, DE);
  {
    CYC(0x6a70, 0x6a71); push_effect(gb, 0x6a71);
    uint16_t target = interactionCode8b_jump_table(gb);
    if (target == 0x6a8f) goto subid2;
    // subid 0 and subid 1 both target 0x6a77
  }

  // interactionCode8b@subid0 / @subid1
  CALL_C(0x6a77, checkInteractionState_hook, 0x23fe, 0x6a7a);
  if (!(F & FZ)) { CYCT(0x6a7a, 0x6a7c); goto L_6a7f; } // jr nz
  CYC(0x6a7a, 0x6a7c);
  CYC(0x6a7c, 0x6a7f); push_effect(gb, 0x6a7f); goto loadScriptAndInitGraphics;

L_6a7f:
  CALL_C(0x6a7f, interactionRunScript_hook, 0x2552, 0x6a82);
  if (F & FC) { CYCT(0x6a82, 0x6a85); interactionDelete_hook(gb); return; } // jp c
  CYC(0x6a82, 0x6a85);
  CYC(0x6a85, 0x6a87); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6a87, 0x6a88); A = mem_rd(gb, DE);
  CYC(0x6a88, 0x6a89); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6a89, 0x6a8c); npcFaceLinkAndAnimate_hook(gb); return; } // jp z
  CYC(0x6a89, 0x6a8c);
  CYC(0x6a8c, 0x6a8f); interactionAnimateAsNpc_hook(gb); return; // jp

subid2:
  CYC(0x6a8f, 0x6a91); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x6a91, checkGlobalFlag_hook, 0x31f3, 0x6a94);
  if (F & FZ) { CYCT(0x6a94, 0x6a97); interactionDelete_hook(gb); return; } // jp z
  CYC(0x6a94, 0x6a97);
  CYC(0x6a97, 0x6a9a); SET_HL(0x5626); // agesInteractionsBank08.shootingGalleryNpc
  CYC(0x6a9a, 0x6a9c); E = 0x08;
  CYC(0x6a9c, 0x6a9f); interBankCall_hook(gb); return; // jp

// interactionCode8b@loadScriptAndInitGraphics
loadScriptAndInitGraphics:
  CALL_C(0x6aa5, interactionInitGraphics_hook, 0x15fb, 0x6aa8);
  CYC(0x6aa8, 0x6aaa); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6aaa, 0x6aab); A = mem_rd(gb, DE);
  CYC(0x6aab, 0x6aae); SET_HL(0x6ab8); // @scriptTable
  CYC(0x6aae, 0x6aaf); interactionCode8b_add_double_index(gb, 0x6aaf);
  CYC(0x6aaf, 0x6ab0); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6ab0, 0x6ab1); H = mem_rd(gb, HL);
  CYC(0x6ab1, 0x6ab2); L = A;
  CALL_C(0x6ab2, interactionSetScript_hook, 0x2544, 0x6ab5);
  CYC(0x6ab5, 0x6ab8); interactionIncState_hook(gb);
  if (gb->pc == 0x6a7f && gb->sp == sp0_) goto L_6a7f;
  return; // jp
}
