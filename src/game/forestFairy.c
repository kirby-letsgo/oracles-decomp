#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t forest_fairy_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void forest_fairy_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void forestFairy_subid00_hook(GB *gb);
void forestFairy_subid00State0_hook(GB *gb);
void forestFairy_loadMovementPreset_hook(GB *gb);
void forestFairy_subid00State1_hook(GB *gb);
void forestFairy_updateMovement_hook(GB *gb);
void forestFairy_animate_hook(GB *gb);
void forestFairy_subid00State2_hook(GB *gb);
void forestFairy_subid00State3_hook(GB *gb);

void interactionCode49_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c5f, 0x5c61); E = 0x42;
  CYC(0x5c61, 0x5c62); A = mem_rd(gb, DE);
  CYC(0x5c62, 0x5c64); E = 0x44;
  CYC(0x5c64, 0x5c65); push_effect(gb, 0x5c65);
  switch (forest_fairy_jump_table(gb)) {
    case 0x5c87: forestFairy_subid00_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void forestFairy_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c87, 0x5c88); A = mem_rd(gb, DE);
  CYC(0x5c88, 0x5c89); push_effect(gb, 0x5c89);
  switch (forest_fairy_jump_table(gb)) {
    case 0x5c93: forestFairy_subid00State0_hook(gb); return;
    case 0x5d3e: forestFairy_subid00State1_hook(gb); return;
    case 0x5dac: forestFairy_subid00State2_hook(gb); return;
    case 0x5dc7: forestFairy_subid00State3_hook(gb); return;
    case 0x5ddd: interactionDelete_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void forestFairy_subid00State0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c93, interactionInitGraphics_hook, 0x15fb, 0x5c96);
  CALL_ROM(0x5c96, 0x5e37);
  CYC(0x5c99, 0x5c9b); L = 0x50;
  CYC(0x5c9b, 0x5c9d); mem_wr(gb, HL, 0x50);
  CYC(0x5c9d, 0x5c9f); L = 0x7a;
  CYC(0x5c9f, 0x5ca1); mem_wr(gb, HL, 0x5a);
  forestFairy_loadMovementPreset_hook(gb);
}

void forestFairy_loadMovementPreset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ca1, 0x5ca3); E = 0x43;
  CYC(0x5ca3, 0x5ca4); A = mem_rd(gb, DE);
  CYC(0x5ca4, 0x5ca5); alu_add(gb, A);
  CYC(0x5ca5, 0x5ca8); SET_HL(0x5ce6);
  CYC(0x5ca8, 0x5ca9); forest_fairy_add_double_index(gb, 0x5ca9);
  CYC(0x5ca9, 0x5cab); E = 0x4b;
  CYC(0x5cab, 0x5cac); A = mem_rd(gb, HL);
  CYC(0x5cac, 0x5cae); alu_and(gb, 0xf8);
  CYC(0x5cae, 0x5caf); mem_wr(gb, DE, A);
  CYC(0x5caf, 0x5cb1); E = 0x49;
  CYC(0x5cb1, 0x5cb2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5cb2, 0x5cb4); alu_and(gb, 7);
  CYC(0x5cb4, 0x5cb5); alu_add(gb, A);
  CYC(0x5cb5, 0x5cb6); alu_add(gb, A);
  CYC(0x5cb6, 0x5cb7); mem_wr(gb, DE, A);
  CYC(0x5cb7, 0x5cb9); E = 0x4d;
  CYC(0x5cb9, 0x5cba); A = mem_rd(gb, HL);
  CYC(0x5cba, 0x5cbc); alu_and(gb, 0xf8);
  CYC(0x5cbc, 0x5cbd); mem_wr(gb, DE, A);
  CYC(0x5cbd, 0x5cbf); E = 0x46;
  CYC(0x5cbf, 0x5cc0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5cc0, 0x5cc2); alu_and(gb, 7);
  CYC(0x5cc2, 0x5cc3); A = alu_inc8(gb, A);
  CYC(0x5cc3, 0x5cc4); mem_wr(gb, DE, A);
  CYC(0x5cc4, 0x5cc5); E = alu_inc8(gb, E);
  CYC(0x5cc5, 0x5cc6); mem_wr(gb, DE, A);
  CYC(0x5cc6, 0x5cc8); E = 0x78;
  CYC(0x5cc8, 0x5cc9); A = mem_rd(gb, HL);
  CYC(0x5cc9, 0x5ccb); alu_and(gb, 0xf8);
  CYC(0x5ccb, 0x5ccc); mem_wr(gb, DE, A);
  CYC(0x5ccc, 0x5cce); E = 0x48;
  CYC(0x5cce, 0x5ccf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5ccf, 0x5cd1); alu_and(gb, 1);
  CYC(0x5cd1, 0x5cd2); mem_wr(gb, DE, A);
  CYC(0x5cd2, 0x5cd4); E = 0x79;
  CYC(0x5cd4, 0x5cd5); A = mem_rd(gb, HL);
  CYC(0x5cd5, 0x5cd7); alu_and(gb, 0xf8);
  CYC(0x5cd7, 0x5cd8); mem_wr(gb, DE, A);
  CYC(0x5cd8, 0x5cda); E = 0x5c;
  CYC(0x5cda, 0x5cdb); A = mem_rd(gb, HL);
  CYC(0x5cdb, 0x5cdd); alu_and(gb, 7);
  CYC(0x5cdd, 0x5cde); mem_wr(gb, DE, A);
  CYC(0x5cde, 0x5cdf); E = alu_dec8(gb, E);
  CYC(0x5cdf, 0x5ce0); mem_wr(gb, DE, A);
  CYC(0x5ce0, 0x5ce2); E = 0x48;
  CYC(0x5ce2, 0x5ce3); A = mem_rd(gb, DE);
  CYC(0x5ce3, 0x5ce6); interactionSetAnimation_hook(gb);
}

void forestFairy_subid00State1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d3e, 0x5d3f); H = D;
  CYC(0x5d3f, 0x5d41); L = 0x78;
  CYC(0x5d41, 0x5d42); B = mem_rd(gb, HL);
  CYC(0x5d42, 0x5d43); L = alu_inc8(gb, L);
  CYC(0x5d43, 0x5d44); C = mem_rd(gb, HL);
  CYC(0x5d44, 0x5d46); L = 0x4b;
  CYC(0x5d46, 0x5d47); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5d47, 0x5d49); mem_wr(gb, 0xff8f, A);
  CYC(0x5d49, 0x5d4a); L = alu_inc8(gb, L);
  CYC(0x5d4a, 0x5d4b); A = mem_rd(gb, HL);
  CYC(0x5d4b, 0x5d4d); mem_wr(gb, 0xff8e, A);
  CYC(0x5d4d, 0x5d4e); alu_sub(gb, C);
  CYC(0x5d4e, 0x5d50); alu_add(gb, 4);
  CYC(0x5d50, 0x5d52); alu_cp(gb, 9);
  if (!(F & FC)) { CYCT(0x5d52, 0x5d54); goto outside; }
  CYC(0x5d52, 0x5d54);
  CYC(0x5d54, 0x5d56); A = mem_rd(gb, 0xff8f);
  CYC(0x5d56, 0x5d57); alu_sub(gb, B);
  CYC(0x5d57, 0x5d59); alu_add(gb, 4);
  CYC(0x5d59, 0x5d5b); alu_cp(gb, 9);
  if (!(F & FC)) { CYCT(0x5d5b, 0x5d5d); goto outside; }
  CYC(0x5d5b, 0x5d5d);
  CYC(0x5d5d, 0x5d5f); E = 0x42;
  CYC(0x5d5f, 0x5d60); A = mem_rd(gb, DE);
  CYC(0x5d60, 0x5d62); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(0x5d62, 0x5d64); goto found; }
  CYC(0x5d62, 0x5d64);
  CYC(0x5d64, 0x5d65); mem_wr(gb, HL, C);
  CYC(0x5d65, 0x5d67); L = 0x4b;
  CYC(0x5d67, 0x5d68); mem_wr(gb, HL, B);
  CYC(0x5d68, 0x5d6a); L = 0x44;
  CYC(0x5d6a, 0x5d6b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
found:
  CYC(0x5d6b, 0x5d6e); SET_HL(0xcfd2);
  CYC(0x5d6e, 0x5d6f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d6f, 0x5d70); alu_scf(gb);
  CYC(0x5d70, 0x5d71); ret_effect(gb);
  return;
outside:
  CYC(0x5d71, 0x5d73); L = 0x7a;
  CYC(0x5d73, 0x5d74); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5d74, 0x5d75); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(0x5d75, 0x5d77); goto animation; }
  CYC(0x5d75, 0x5d77);
  CYC(0x5d77, 0x5d7a); mem_wr(gb, HL, 0x5a);
  CYC(0x5d7a, 0x5d7c); L = 0x47;
  CYC(0x5d7c, 0x5d7e); mem_wr(gb, HL, alu_srl(gb, mem_rd(gb, HL)));
  if (!(F & FC)) { CYCT(0x5d7e, 0x5d80); goto decrement; }
  CYC(0x5d7e, 0x5d80);
  CYC(0x5d7f, 0x5d80); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
animation:
  CYC(0x5d80, 0x5d82); alu_and(gb, 7);
  if (!(F & FZ)) { CYCT(0x5d82, 0x5d84); goto decrement; }
  CYC(0x5d82, 0x5d84);
  CYC(0x5d84, 0x5d85); push_effect(gb, BC);
  CYC(0x5d85, 0x5d88); SET_BC(0x8402);
  CALL_C(0x5d88, objectCreateInteraction_hook, 0x24c5, 0x5d8b);
  CYC(0x5d8b, 0x5d8c); SET_BC(pop_effect(gb));
decrement:
  CALL_C(0x5d8c, interactionDecCounter1_hook, 0x23cc, 0x5d8f);
  if (!(F & FZ)) { CYCT(0x5d8f, 0x5d91); forestFairy_updateMovement_hook(gb); return; }
  CYC(0x5d8f, 0x5d91);
  CYC(0x5d91, 0x5d92); L = alu_inc8(gb, L);
  CYC(0x5d92, 0x5d93); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5d93, 0x5d94); mem_wr(gb, HL, A);
  CALL_C(0x5d94, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x5d97);
  CALL_C(0x5d97, objectNudgeAngleTowards_hook, 0x1fd4, 0x5d9a);
  forestFairy_updateMovement_hook(gb);
}

void forestFairy_updateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d9a, objectApplySpeed_hook, 0x201d, 0x5d9d);
  CYC(0x5d9d, 0x5da0); A = mem_rd(gb, 0xcc00);
  CYC(0x5da0, 0x5da2); alu_and(gb, 0x1f);
  CYC(0x5da2, 0x5da4); A = 0x83;
  if (F & FZ) CALL_C_CC(0x5da4, playSound_b00_hook, 0x0c98, 0x5da7);
  else CYC(0x5da4, 0x5da7);
  forestFairy_animate_hook(gb);
}

void forestFairy_animate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5da7, interactionAnimate_hook, 0x261b, 0x5daa);
  CYC(0x5daa, 0x5dab); alu_or(gb, D);
  CYC(0x5dab, 0x5dac); ret_effect(gb);
}

void forestFairy_subid00State2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5dac, 0x5daf); A = mem_rd(gb, 0xcfd2);
  CYC(0x5daf, 0x5db0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5db0, 0x5db2); forestFairy_animate_hook(gb); return; }
  CYC(0x5db0, 0x5db2);
  CYC(0x5db2, 0x5db4); E = 0x43;
  CYC(0x5db4, 0x5db5); A = mem_rd(gb, DE);
  CYC(0x5db5, 0x5db7); alu_cp(gb, 6);
  if (!(F & FC)) { CYCT(0x5db7, 0x5db9); goto delete; }
  CYC(0x5db7, 0x5db9);
  CYC(0x5db9, 0x5dbb); alu_add(gb, 6);
  CYC(0x5dbb, 0x5dbc); mem_wr(gb, DE, A);
  CALL_C(0x5dbc, interactionIncState_hook, 0x23e0, 0x5dbf);
  CYC(0x5dbf, 0x5dc2); forestFairy_loadMovementPreset_hook(gb);
  return;
delete:
  CALL_C(0x5dc2, objectCreatePuff_hook, 0x24c1, 0x5dc5);
  CYC(0x5dc5, 0x5dc7); interactionDelete_hook(gb);
}

void forestFairy_subid00State3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5dc7, 0x5dca); push_effect(gb, 0x5dca); forestFairy_subid00State1_hook(gb);
  if (F & FC) { CYCT(0x5dca, 0x5dcc); interactionDelete_hook(gb); return; }
  CYC(0x5dca, 0x5dcc);
  CYC(0x5dcc, 0x5dce); E = 0x4b;
  CYC(0x5dce, 0x5dcf); A = mem_rd(gb, DE);
  CYC(0x5dcf, 0x5dd1); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(0x5dd1, 0x5dd3); goto done; }
  CYC(0x5dd1, 0x5dd3);
  CYC(0x5dd3, 0x5dd5); E = 0x4d;
  CYC(0x5dd5, 0x5dd6); A = mem_rd(gb, DE);
  CYC(0x5dd6, 0x5dd8); alu_cp(gb, 0xa0);
  if (F & FC) { CYCT(0x5dd8, 0x5dd9); ret_effect(gb); return; }
  CYC(0x5dd8, 0x5dd9);
done:
  CYC(0x5dd9, 0x5ddc); SET_HL(0xcfd2);
  CYC(0x5ddc, 0x5ddd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5ddd, 0x5de0); interactionDelete_hook(gb);
}
