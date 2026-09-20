#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode8b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode8b_jump_table(gb);
    if (target == b_+34) goto subid2;
    // subid 0 and subid 1 both target 0x6a77
  }

  // interactionCode8b@subid0 / @subid1
  CALL_C(b_+10, checkInteractionState_hook, SYM(checkInteractionState), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto L_6a7f; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); push_effect(gb, b_+18); goto loadScriptAndInitGraphics;

L_6a7f:
  CALL_C(b_+18, interactionRunScript_hook, SYM(interactionRunScript), b_+21);
  if (F & FC) { CYCT(b_+21, b_+24); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+31); npcFaceLinkAndAnimate_hook(gb); return; } // jp z
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+34); interactionAnimateAsNpc_hook(gb); return; // jp

subid2:
  CYC(b_+34, b_+36); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+36, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+39);
  if (F & FZ) { CYCT(b_+39, b_+42); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+45); SET_HL((SYM(interaction6e_subid02__state1) + 12)); // agesInteractionsBank08.shootingGalleryNpc
  CYC(b_+45, b_+47); E = 0x08;
  CYC(b_+47, b_+50); interBankCall_hook(gb); return; // jp

// interactionCode8b@loadScriptAndInitGraphics
loadScriptAndInitGraphics:
  CALL_C(b_+56, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+59);
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+65); SET_HL(b_+75); // @scriptTable
  CYC(b_+65, b_+66); interactionCode8b_add_double_index(gb, b_+66);
  CYC(b_+66, b_+67); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+67, b_+68); H = mem_rd(gb, HL);
  CYC(b_+68, b_+69); L = A;
  CALL_C(b_+69, interactionSetScript_hook, SYM(interactionSetScript), b_+72);
  CYC(b_+72, b_+75); interactionIncState_hook(gb);
  if (gb->pc == b_+18 && gb->sp == sp0_) goto L_6a7f;
  return; // jp
}
