#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodea6_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// 0b:5dbd, called once from interactionCodea6@state0.
void interactionCodea6_createSparkle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5dbd, 0x5dc0); SET_BC(0x840f); // INTERAC_SPARKLE, $0f
  CALL_C(0x5dc0, objectCreateInteraction_hook, 0x24c5, 0x5dc3);
  if (!(F & FZ)) { CYCT(0x5dc3, 0x5dc4); ret_effect(gb); return; } // ret nz
  CYC(0x5dc3, 0x5dc4);
  CYC(0x5dc4, 0x5dc6); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x5dc6, 0x5dc8); A = INTERACTION_BASE; // Interaction.start
  CYC(0x5dc8, 0x5dc9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5dc9, 0x5dca); mem_wr(gb, HL, D);
  RET(0x5dca); return;
}

// 0b:5d87. Marked "Unused function?" in the disassembly source -- callers: 0, unreachable from
// interactionCodea6 or anywhere else, but ported here for completeness since it's part of the
// same source file.
void interactionCodea6_func_5d87_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d87, 0x5d8a); SET_BC(0x840b); // INTERAC_SPARKLE, $0b
  CALL_C(0x5d8a, objectCreateInteraction_hook, 0x24c5, 0x5d8d);
  if (!(F & FZ)) { CYCT(0x5d8d, 0x5d8e); ret_effect(gb); return; } // ret nz
  CYC(0x5d8d, 0x5d8e);
  CYC(0x5d8e, 0x5d90); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5d90, 0x5d92); mem_wr(gb, HL, 0xc2); // 194
  CALL_C(0x5d92, objectCopyPosition_hook, 0x2242, 0x5d95);
  CALL_C(0x5d95, getRandomNumber_hook, 0x043e, 0x5d98);
  CYC(0x5d98, 0x5d9a); alu_and(gb, 0x07);
  CYC(0x5d9a, 0x5d9b); alu_add(gb, A);
  CYC(0x5d9b, 0x5d9e); SET_BC(0x5dad); // @offsets
  CALL_C(0x5d9e, addAToBc_hook, 0x006d, 0x5da1);
  CYC(0x5da1, 0x5da2); A = mem_rd(gb, BC);
  CYC(0x5da2, 0x5da4); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5da4, 0x5da5); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5da5, 0x5da6); mem_wr(gb, HL, A);
  CYC(0x5da6, 0x5da7); SET_BC(BC + 1); // inc bc
  CYC(0x5da7, 0x5da8); A = mem_rd(gb, BC);
  CYC(0x5da8, 0x5daa); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5daa, 0x5dab); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5dab, 0x5dac); mem_wr(gb, HL, A);
  RET(0x5dac); return;

  // @offsets (0b:5dad): pure ROM data (8 signed (dy,dx) byte pairs), referenced only via
  // SET_BC(0x5dad) above.
}

// ==================================================================================================
// INTERAC_MAKU_SEED
//
// Variables:
//   var38: ?
// ==================================================================================================
void interactionCodea6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d31, 0x5d33); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5d33, 0x5d34); A = mem_rd(gb, DE);
  CYC(0x5d34, 0x5d35); push_effect(gb, 0x5d35);
  switch (interactionCodea6_jump_table(gb)) {
    case 0x5d39: goto state0;
    case 0x5d60: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x5d39, interactionIncState_hook, 0x23e0, 0x5d3c);
  CYC(0x5d3c, 0x5d3e); A = 0xab; // PALH_ab
  CALL_C(0x5d3e, loadPaletteHeader_hook, 0x050b, 0x5d41);
  CALL_C(0x5d41, interactionInitGraphics_hook, 0x15fb, 0x5d44);
  CYC(0x5d44, 0x5d47); SET_HL(w1Link_yh);
  CYC(0x5d47, 0x5d48); B = mem_rd(gb, HL);
  CYC(0x5d48, 0x5d4a); L = (uint8_t)w1Link_xh;
  CYC(0x5d4a, 0x5d4b); C = mem_rd(gb, HL);
  CALL_C(0x5d4b, interactionSetPosition_hook, 0x2773, 0x5d4e);
  CYC(0x5d4e, 0x5d50); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x5d50, 0x5d52); mem_wr(gb, HL, 0x8b);
  CYC(0x5d52, 0x5d55); A = mem_rd(gb, wFrameCounter);
  CYC(0x5d55, 0x5d56); alu_cpl(gb);
  CYC(0x5d56, 0x5d57); A = alu_inc8(gb, A);
  CYC(0x5d57, 0x5d59); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5d59, 0x5d5a); mem_wr(gb, DE, A);
  CALL_C(0x5d5a, objectSetVisible82_hook, 0x1e69, 0x5d5d);
  CALL_C(0x5d5d, interactionCodea6_createSparkle_hook, 0x5dbd, 0x5d60);
  // falls through into @state1 (also a direct jump-table target)

state1:
  CYC(0x5d60, 0x5d61); H = D;
  CYC(0x5d61, 0x5d63); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x5d63, 0x5d64); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x5d64, 0x5d66); alu_cp(gb, 0xf3);
  if (F & FC) { CYCT(0x5d66, 0x5d68); goto l_5d70; } // jr c
  CYC(0x5d66, 0x5d68);
  CYC(0x5d68, 0x5d6a); A = 0x01;
  CYC(0x5d6a, 0x5d6d); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(0x5d6d, 0x5d70); interactionDelete_hook(gb); return; // jp

l_5d70:
  CYC(0x5d70, 0x5d73); SET_BC(0x0080);
  CYC(0x5d73, 0x5d74); A = C;
  CYC(0x5d74, 0x5d75); alu_add(gb, mem_rd(gb, HL)); // [zh]
  CYC(0x5d75, 0x5d76); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5d76, 0x5d77); A = B;
  CYC(0x5d77, 0x5d78); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x5d78, 0x5d79); mem_wr(gb, HL, A);
  CYC(0x5d79, 0x5d7c); A = mem_rd(gb, wFrameCounter);
  CYC(0x5d7c, 0x5d7e); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5d7e, 0x5d7f); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5d7f, 0x5d81); alu_and(gb, 0x3f);
  CYC(0x5d81, 0x5d83); A = 0x83; // SND_MAGIC_POWDER
  if (F & FZ) { CALL_C_CC(0x5d83, playSound_b00_hook, 0x0c98, 0x5d86); } else { CYC(0x5d83, 0x5d86); } // call z
  RET(0x5d86); return;
}
