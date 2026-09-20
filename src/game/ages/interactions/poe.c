#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

void interactionCode59__afterCall6c0b_hook(GB *gb);

static uint16_t poe_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A); burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L); burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL); burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A; burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

static void poe_add_double_index(GB *gb, uint16_t ra) {
  push_effect(gb, ra); burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC); burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0; burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode59__loadScriptAndInitGraphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c08, interactionInitGraphics_hook, 0x15fb, 0x6c0b);
  interactionCode59__afterCall6c0b_hook(gb);
}

void interactionCode59__afterCall6c0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c0b, objectMarkSolidPosition_hook, 0x24f0, 0x6c0e);
  CYC(0x6c0e, 0x6c10); E = OBJ_SUBID; CYC(0x6c10, 0x6c11); A = mem_rd(gb, DE); CYC(0x6c11, 0x6c14); SET_HL(0x6c1e); CYC(0x6c14, 0x6c15); poe_add_double_index(gb, 0x6c15);
  CYC(0x6c15, 0x6c16); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x6c16, 0x6c17); H = mem_rd(gb, HL); CYC(0x6c17, 0x6c18); L = A; CALL_C(0x6c18, interactionSetScript_hook, 0x2544, 0x6c1b); CYC(0x6c1b, 0x6c1e); interactionIncState_hook(gb);
}

void interactionCode59__afterCall6be7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6be7, interactionRunScript_hook, 0x2552, 0x6bea); if (F & FC) { CYCT(0x6bea, 0x6bed); interactionDelete_hook(gb); return; } CYC(0x6bea, 0x6bed);
  CYC(0x6bed, 0x6bef); E = OBJ_VAR3E; CYC(0x6bef, 0x6bf0); A = mem_rd(gb, DE); CYC(0x6bf0, 0x6bf1); alu_or(gb, A); if (!(F & FZ)) { CYCT(0x6bf1, 0x6bf2); ret_effect(gb); return; } CYC(0x6bf1, 0x6bf2);
  CYC(0x6bf2, 0x6bf4); E = OBJ_VAR3F; CYC(0x6bf4, 0x6bf5); A = mem_rd(gb, DE); CYC(0x6bf5, 0x6bf6); alu_or(gb, A); if (F & FZ) { CYCT(0x6bf6, 0x6bf9); npcFaceLinkAndAnimate_hook(gb); return; } CYC(0x6bf6, 0x6bf9); CALL_C(0x6bf9, interactionAnimate_hook, 0x261b, 0x6bfc); CYC(0x6bfc, 0x6bff); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

void interactionCode59_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6b9c, checkInteractionState_hook, 0x23fe, 0x6b9f);
  if (F & FZ) {
    CYC(0x6b9f, 0x6ba1); CYC(0x6ba1, 0x6ba3); E = INTERACTION_BASE + OBJ_VAR03; CYC(0x6ba3, 0x6ba4); A = mem_rd(gb, DE); CYC(0x6ba4, 0x6ba5); push_effect(gb, 0x6ba5);
    switch (poe_jump_table(gb)) {
      case 0x6bab:
        CALL_C(0x6bab, getThisRoomFlags_hook, 0x197d, 0x6bae); CYC(0x6bae, 0x6bb0); alu_bit(gb, 6, mem_rd(gb, HL)); if (!(F & FZ)) { CYCT(0x6bb0, 0x6bb3); interactionDelete_hook(gb); return; } CYC(0x6bb0, 0x6bb3); CYC(0x6bb3, 0x6bb6); SET_HL(wGroup0RoomFlags + 0x2e); CYC(0x6bb6, 0x6bb8); alu_bit(gb, 6, mem_rd(gb, HL)); if (!(F & FZ)) { CYCT(0x6bb8, 0x6bbb); interactionDelete_hook(gb); return; } CYC(0x6bb8, 0x6bbb); CYC(0x6bbb, 0x6bbd); break;
      case 0x6bbd:
        CALL_C(0x6bbd, getThisRoomFlags_hook, 0x197d, 0x6bc0); CYC(0x6bc0, 0x6bc2); alu_bit(gb, 5, mem_rd(gb, HL)); if (!(F & FZ)) { CYCT(0x6bc2, 0x6bc5); interactionDelete_hook(gb); return; } CYC(0x6bc2, 0x6bc5); CYC(0x6bc5, 0x6bc7); alu_bit(gb, 6, mem_rd(gb, HL)); if (F & FZ) { CYCT(0x6bc7, 0x6bca); interactionDelete_hook(gb); return; } CYC(0x6bc7, 0x6bca); CYC(0x6bca, 0x6bcd); SET_HL(wGroup0RoomFlags + 0x2e); CYC(0x6bcd, 0x6bcf); alu_bit(gb, 6, mem_rd(gb, HL)); if (F & FZ) { CYCT(0x6bcf, 0x6bd2); interactionDelete_hook(gb); return; } CYC(0x6bcf, 0x6bd2); CYC(0x6bd2, 0x6bd4); break;
      case 0x6bd4:
        CYC(0x6bd4, 0x6bd7); SET_HL(wGroup0RoomFlags + 0x7c); CYC(0x6bd7, 0x6bd9); alu_bit(gb, 6, mem_rd(gb, HL)); if (F & FZ) { CYCT(0x6bd9, 0x6bdc); interactionDelete_hook(gb); return; } CYC(0x6bd9, 0x6bdc); CALL_C(0x6bdc, getThisRoomFlags_hook, 0x197d, 0x6bdf); CYC(0x6bdf, 0x6be1); alu_bit(gb, 6, mem_rd(gb, HL)); if (!(F & FZ)) { CYCT(0x6be1, 0x6be4); interactionDelete_hook(gb); return; } CYC(0x6be1, 0x6be4); break;
      default: HANDOFF(HL);
    }
    CALL_C(0x6be4, interactionCode59__loadScriptAndInitGraphics_hook, 0x6c08, 0x6be7);
  } else CYCT(0x6b9f, 0x6ba1);
  interactionCode59__afterCall6be7_hook(gb);
}
