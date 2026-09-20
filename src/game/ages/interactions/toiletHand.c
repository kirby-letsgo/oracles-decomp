#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t toilet_hand_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A); burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb)); burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L); burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL); burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A; burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

static void toilet_hand_add_double_index(GB *gb, uint16_t ra) {
  push_effect(gb, ra); burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC); burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0; burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void toilet_hand_load_script(GB *gb, uint16_t sp0_) {
  CYC(0x6cbe, 0x6cc0); E = OBJ_SUBID; CYC(0x6cc0, 0x6cc1); A = mem_rd(gb, DE); CYC(0x6cc1, 0x6cc4); SET_HL(0x6d0c); CYC(0x6cc4, 0x6cc5); toilet_hand_add_double_index(gb, 0x6cc5); CYC(0x6cc5, 0x6cc6); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x6cc6, 0x6cc7); H = mem_rd(gb, HL); CYC(0x6cc7, 0x6cc8); L = A; CYC(0x6cc8, 0x6ccb); interactionSetScript_hook(gb);
}

static void toilet_hand_load_script_and_init_graphics(GB *gb, uint16_t sp0_) {
  CALL_C(0x6cb3, interactionInitGraphics_hook, 0x15fb, 0x6cb6); CYC(0x6cb6, 0x6cb8); A = 0x0b; CALL_C(0x6cb8, interactionSetHighTextIndex_hook, 0x253b, 0x6cbb); CALL_C(0x6cbb, interactionIncState_hook, 0x23e0, 0x6cbe); toilet_hand_load_script(gb, sp0_);
}

static void toilet_hand_respond_to_object_in_hole(GB *gb, uint16_t sp0_) {
  CYC(0x6ccb, 0x6cce); A = W8(wTextIsActive); CYC(0x6cce, 0x6ccf); alu_or(gb, A); if (!(F & FZ)) { CYCT(0x6ccf, 0x6cd0); ret_effect(gb); return; } CYC(0x6ccf, 0x6cd0);
  CYC(0x6cd0, 0x6cd3); A = mem_rd(gb, wTmpcfc0_fallDownHoleEvent + 0x18); CYC(0x6cd3, 0x6cd4); A = alu_inc8(gb, A); CYC(0x6cd4, 0x6cd5); E = A; CYC(0x6cd5, 0x6cd8); SET_HL(0x6cef); CALL_C(0x6cd8, lookupKey_hook, 0x1e06, 0x6cdb); if (!(F & FC)) { CYCT(0x6cdb, 0x6cdc); ret_effect(gb); return; } CYC(0x6cdb, 0x6cdc);
  CYC(0x6cdc, 0x6cdf); SET_HL(0x6cf4); CYC(0x6cdf, 0x6ce0); toilet_hand_add_double_index(gb, 0x6ce0); CYC(0x6ce0, 0x6ce1); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x6ce1, 0x6ce2); H = mem_rd(gb, HL); CYC(0x6ce2, 0x6ce3); L = A;
  CYC(0x6ce3, 0x6ce6); A = mem_rd(gb, wTmpcfc0_fallDownHoleEvent + 0x19); CYC(0x6ce6, 0x6ce7); E = A; CALL_C(0x6ce7, lookupKey_hook, 0x1e06, 0x6cea); if (!(F & FC)) { CYCT(0x6cea, 0x6ceb); ret_effect(gb); return; } CYC(0x6cea, 0x6ceb); CYC(0x6ceb, 0x6ced); E = OBJ_VAR38; CYC(0x6ced, 0x6cee); mem_wr(gb, DE, A); CYC(0x6cee, 0x6cef); ret_effect(gb);
}

void interactionCode5b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c54, 0x6c56); E = OBJ_STATE; CYC(0x6c56, 0x6c57); A = mem_rd(gb, DE); CYC(0x6c57, 0x6c58); push_effect(gb, 0x6c58);
  switch (toilet_hand_jump_table(gb)) { case 0x6c5e: goto state0; case 0x6c6c: goto state1; case 0x6c86: goto state2; default: HANDOFF(HL); }
state0:
  CYC(0x6c5e, 0x6c61); push_effect(gb, 0x6c61); toilet_hand_load_script_and_init_graphics(gb, sp0_); CALL_C(0x6c61, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6c64); CYC(0x6c64, 0x6c67); SET_HL(0x414b); CYC(0x6c67, 0x6c69); E = 0x08; CALL_C(0x6c69, interBankCall_hook, 0x008a, 0x6c6c);
state1:
  CYC(0x6c6c, 0x6c6f); push_effect(gb, 0x6c6f); toilet_hand_respond_to_object_in_hole(gb, sp0_); if (F & FC) goto dropped;
  CALL_C(0x6c71, interactionRunScript_hook, 0x2552, 0x6c74); CYC(0x6c74, 0x6c75); H = D; CYC(0x6c75, 0x6c77); L = OBJ_VISIBLE; CYC(0x6c77, 0x6c79); alu_bit(gb, 7, mem_rd(gb, HL)); if (F & FZ) { CYCT(0x6c79, 0x6c7a); ret_effect(gb); return; } CYC(0x6c79, 0x6c7a); CYC(0x6c7a, 0x6c7d); interactionAnimateAsNpc_hook(gb); return;
dropped:
  CYC(0x6c7d, 0x6c80); SET_HL(0x66d9); CALL_C(0x6c80, interactionSetScript_hook, 0x2544, 0x6c83); CYC(0x6c83, 0x6c86); interactionIncState_hook(gb); return;
state2:
  CYC(0x6c86, 0x6c89); A = W8(wTextIsActive); CYC(0x6c89, 0x6c8a); alu_or(gb, A); if (!(F & FZ)) { CYCT(0x6c8a, 0x6c8b); ret_effect(gb); return; } CYC(0x6c8a, 0x6c8b); CALL_C(0x6c8b, interactionRunScript_hook, 0x2552, 0x6c8e); if (F & FC) goto script_ended; CYC(0x6c8e, 0x6c90); CYC(0x6c90, 0x6c91); H = D; CYC(0x6c91, 0x6c93); L = OBJ_VISIBLE; CYC(0x6c93, 0x6c95); alu_bit(gb, 7, mem_rd(gb, HL)); if (F & FZ) { CYCT(0x6c95, 0x6c96); ret_effect(gb); return; } CYC(0x6c95, 0x6c96); CALL_C(0x6c96, interactionAnimateAsNpc_hook, 0x26db, 0x6c99); CYC(0x6c99, 0x6c9c); interactionAnimate_hook(gb); return;
script_ended:
  CYCT(0x6c8e, 0x6c90); CYC(0x6c9c, 0x6c9f); push_effect(gb, 0x6c9f); toilet_hand_load_script(gb, sp0_); CYC(0x6c9f, 0x6ca2); SET_HL(0x414b); CYC(0x6ca2, 0x6ca4); E = 0x08; CALL_C(0x6ca4, interBankCall_hook, 0x008a, 0x6ca7); CYC(0x6ca7, 0x6ca9); E = OBJ_STATE; CYC(0x6ca9, 0x6cab); A = 1; CYC(0x6cab, 0x6cac); mem_wr(gb, DE, A); CYC(0x6cac, 0x6cad); ret_effect(gb);
}
