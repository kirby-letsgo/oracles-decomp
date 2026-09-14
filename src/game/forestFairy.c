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
void forestFairy_deleteSelf_hook(GB *gb);
void forestFairy_subid01_hook(GB *gb);
void forestFairy_initCollisionRadiusAndSetZAndIncState_hook(GB *gb);
void forestFairy_subid02_hook(GB *gb);
void forestFairy_subid03_hook(GB *gb);
void forestFairy_subid04_hook(GB *gb);
void forestFairy_subid03State1_hook(GB *gb);
void forestFairy_subid03State2_hook(GB *gb);
void forestFairy_subid03State3_hook(GB *gb);
void forestFairy_subid04State1_hook(GB *gb);
void forestFairy_subid05_hook(GB *gb);
void forestFairy_initNpcFromData_hook(GB *gb);
void forestFairy_standardUpdate_hook(GB *gb);
void forestFairy_subid08_hook(GB *gb);
void forestFairy_subid0b_hook(GB *gb);
void forestFairy_subid0c_hook(GB *gb);
void forestFairy_standardUpdate_2_hook(GB *gb);
void forestFairy_subid0e_hook(GB *gb);

void interactionCode49_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c5f, 0x5c61); E = 0x42;
  CYC(0x5c61, 0x5c62); A = mem_rd(gb, DE);
  CYC(0x5c62, 0x5c64); E = 0x44;
  CYC(0x5c64, 0x5c65); push_effect(gb, 0x5c65);
  switch (forest_fairy_jump_table(gb)) {
    case 0x5c87: forestFairy_subid00_hook(gb); return;
    case 0x5de0: forestFairy_subid01_hook(gb); return;
    case 0x5e53: forestFairy_subid02_hook(gb); return;
    case 0x5e56: forestFairy_subid03_hook(gb); return;
    case 0x5e62: forestFairy_subid04_hook(gb); return;
    case 0x5ed7: forestFairy_subid05_hook(gb); return;
    case 0x5f3c: forestFairy_subid08_hook(gb); return;
    case 0x5f73: forestFairy_subid0b_hook(gb); return;
    case 0x5f9c: forestFairy_subid0c_hook(gb); return;
    case 0x5fb9: forestFairy_subid0e_hook(gb); return;
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

void forestFairy_deleteSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ddd, 0x5de0); interactionDelete_hook(gb);
}

void forestFairy_initCollisionRadiusAndSetZAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5e37, interactionIncState_hook, 0x23e0, 0x5e3a);
  CYC(0x5e3a, 0x5e3c); L = 0x66;
  CYC(0x5e3c, 0x5e3e); A = 4;
  CYC(0x5e3e, 0x5e3f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5e3f, 0x5e40); mem_wr(gb, HL, A);
  CYC(0x5e40, 0x5e42); L = 0x4f;
  CYC(0x5e42, 0x5e44); mem_wr(gb, HL, 0xfc);
  CYC(0x5e44, 0x5e47); objectSetVisiblec1_hook(gb);
}

void forestFairy_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e53, 0x5e56); interactionDelete_hook(gb);
}

void forestFairy_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e56, 0x5e57); A = mem_rd(gb, DE);
  CYC(0x5e57, 0x5e58); push_effect(gb, 0x5e58);
  switch (forest_fairy_jump_table(gb)) {
    case 0x5c93: forestFairy_subid00State0_hook(gb); return;
    case 0x5e6a: forestFairy_subid03State1_hook(gb); return;
    case 0x5e7c: forestFairy_subid03State2_hook(gb); return;
    case 0x5eaa: forestFairy_subid03State3_hook(gb); return;
    case 0x5dc7: forestFairy_subid00State3_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void forestFairy_subid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e62, 0x5e63); A = mem_rd(gb, DE);
  CYC(0x5e63, 0x5e64); push_effect(gb, 0x5e64);
  switch (forest_fairy_jump_table(gb)) {
    case 0x5c93: forestFairy_subid00State0_hook(gb); return;
    case 0x5eca: forestFairy_subid04State1_hook(gb); return;
    case 0x5dc7: forestFairy_subid00State3_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void forestFairy_subid03State1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e6a, 0x5e6d); push_effect(gb, 0x5e6d); forestFairy_subid00State1_hook(gb);
  if (!(F & FC)) { CYCT(0x5e6d, 0x5e6e); ret_effect(gb); return; }
  CYC(0x5e6d, 0x5e6e);
  CALL_C(0x5e6e, interactionIncState_hook, 0x23e0, 0x5e71);
  CYC(0x5e71, 0x5e73); A = 2;
  CYC(0x5e73, 0x5e75); L = 0x46;
  CYC(0x5e75, 0x5e76); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5e76, 0x5e77); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5e77, 0x5e79); L = 0x7b;
  CYC(0x5e79, 0x5e7b); mem_wr(gb, HL, 0x20);
  CYC(0x5e7b, 0x5e7c); ret_effect(gb);
}

void forestFairy_subid03State2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e7c, 0x5e7d); H = D;
  CYC(0x5e7d, 0x5e7f); L = 0x7a;
  CYC(0x5e7f, 0x5e80); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5e80, 0x5e81); A = mem_rd(gb, HL);
  CYC(0x5e81, 0x5e83); alu_and(gb, 7);
  if (F & FZ) {
    CYC(0x5e83, 0x5e85);
    CYC(0x5e85, 0x5e86); push_effect(gb, BC);
    CYC(0x5e86, 0x5e89); SET_BC(0x8402);
    CALL_C(0x5e89, objectCreateInteraction_hook, 0x24c5, 0x5e8c);
    CYC(0x5e8c, 0x5e8d); SET_BC(pop_effect(gb));
  } else CYCT(0x5e83, 0x5e85);
  CALL_C(0x5e8d, interactionDecCounter2_hook, 0x23d1, 0x5e90);
  if (F & FZ) {
    CYC(0x5e90, 0x5e92);
    CYC(0x5e92, 0x5e93); L = alu_dec8(gb, L);
    CYC(0x5e93, 0x5e94); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5e94, 0x5e95); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x5e95, 0x5e96); L = alu_inc8(gb, L);
    CYC(0x5e96, 0x5e97); A = mem_rd(gb, HL);
    CYC(0x5e97, 0x5e98); A = alu_inc8(gb, A);
    CYC(0x5e98, 0x5e9a); alu_and(gb, 0x1f);
    CYC(0x5e9a, 0x5e9b); mem_wr(gb, HL, A);
    CYC(0x5e9b, 0x5e9d); L = 0x7b;
    CYC(0x5e9d, 0x5e9e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) {
      CYC(0x5e9e, 0x5ea0);
      CYC(0x5ea0, 0x5ea1); L = E;
      CYC(0x5ea1, 0x5ea2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(0x5ea2, 0x5ea5); SET_HL(0xcfd2);
      CYC(0x5ea5, 0x5ea6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(0x5ea6, 0x5ea7); ret_effect(gb); return;
    }
    CYCT(0x5e9e, 0x5ea0);
  } else CYCT(0x5e90, 0x5e92);
  CYC(0x5ea7, 0x5eaa); forestFairy_updateMovement_hook(gb);
}

void forestFairy_subid03State3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5eaa, 0x5ead); A = mem_rd(gb, 0xcfd2);
  CYC(0x5ead, 0x5eae); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5eae, 0x5eb1); forestFairy_animate_hook(gb); return; }
  CYC(0x5eae, 0x5eb1);
  CALL_C(0x5eb1, interactionIncState_hook, 0x23e0, 0x5eb4);
  CYC(0x5eb4, 0x5eb6); L = 0x43;
  CYC(0x5eb6, 0x5eb7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5eb7, 0x5eb9); L = 0x4b;
  CYC(0x5eb9, 0x5eba); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5eba, 0x5ebb); L = alu_inc8(gb, L);
  CYC(0x5ebb, 0x5ebc); C = mem_rd(gb, HL);
  CYC(0x5ebc, 0x5ebd); B = A;
  CYC(0x5ebd, 0x5ebe); push_effect(gb, BC);
  CALL_C(0x5ebe, forestFairy_loadMovementPreset_hook, 0x5ca1, 0x5ec1);
  CYC(0x5ec1, 0x5ec2); SET_BC(pop_effect(gb));
  CYC(0x5ec2, 0x5ec3); H = D;
  CYC(0x5ec3, 0x5ec5); L = 0x4b;
  CYC(0x5ec5, 0x5ec6); mem_wr(gb, HL, B);
  CYC(0x5ec6, 0x5ec8); L = 0x4d;
  CYC(0x5ec8, 0x5ec9); mem_wr(gb, HL, C);
  CYC(0x5ec9, 0x5eca); ret_effect(gb);
}

void forestFairy_subid04State1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5eca, 0x5ecd); A = mem_rd(gb, 0xcfd2);
  CYC(0x5ecd, 0x5ece); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5ece, 0x5ed1); forestFairy_animate_hook(gb); return; }
  CYC(0x5ece, 0x5ed1);
  CALL_C(0x5ed1, interactionIncState_hook, 0x23e0, 0x5ed4);
  CYC(0x5ed4, 0x5ed7); forestFairy_loadMovementPreset_hook(gb);
}

void forestFairy_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5de0, 0x5de1); A = mem_rd(gb, DE);
  CYC(0x5de1, 0x5de2); alu_or(gb, A);
  if (F & FZ) goto state_zero;
  CYC(0x5de2, 0x5de4);
  CYC(0x5de4, 0x5de7); A = mem_rd(gb, 0xcfd0);
  CYC(0x5de7, 0x5de8); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5de8, 0x5deb); interactionDelete_hook(gb); return; }
  CYC(0x5de8, 0x5deb);
  CYC(0x5deb, 0x5dee); SET_HL(0xd000);
  CALL_C(0x5dee, preventObjectHFromPassingObjectD_hook, 0x1d7f, 0x5df1);
  CALL_C(0x5df1, interactionAnimate_hook, 0x261b, 0x5df4);
  CYC(0x5df4, 0x5df7); interactionRunScript_hook(gb); return;
state_zero:
  CYCT(0x5de2, 0x5de4);
  CYC(0x5df7, 0x5df9); E = 0x43;
  CYC(0x5df9, 0x5dfa); A = mem_rd(gb, DE);
  CYC(0x5dfa, 0x5dfd); SET_HL(0xcfd1);
  CALL_C(0x5dfd, checkFlag_hook, 0x0205, 0x5e00);
  if (F & FZ) { CYCT(0x5e00, 0x5e03); interactionDelete_hook(gb); return; }
  CYC(0x5e00, 0x5e03);
  CYC(0x5e03, 0x5e06); A = mem_rd(gb, 0xcfd1);
  CALL_C(0x5e06, getNumSetBits_hook, 0x0176, 0x5e09);
  CYC(0x5e09, 0x5e0a); A = alu_dec8(gb, A);
  CYC(0x5e0a, 0x5e0d); SET_HL(0x5e47);
  CYC(0x5e0d, 0x5e0e); forest_fairy_add_double_index(gb, 0x5e0e);
  CYC(0x5e0e, 0x5e0f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5e0f, 0x5e10); H = mem_rd(gb, HL);
  CYC(0x5e10, 0x5e11); L = A;
  CALL_C(0x5e11, interactionSetScript_hook, 0x2544, 0x5e14);
  CALL_C(0x5e14, interactionInitGraphics_hook, 0x15fb, 0x5e17);
  CYC(0x5e17, 0x5e19); E = 0x43;
  CYC(0x5e19, 0x5e1a); A = mem_rd(gb, DE);
  CYC(0x5e1a, 0x5e1b); B = A;
  CYC(0x5e1b, 0x5e1c); A = alu_inc8(gb, A);
  CYC(0x5e1c, 0x5e1e); E = 0x5c;
  CYC(0x5e1e, 0x5e1f); mem_wr(gb, DE, A);
  CYC(0x5e1f, 0x5e20); E = alu_dec8(gb, E);
  CYC(0x5e20, 0x5e21); mem_wr(gb, DE, A);
  CYC(0x5e21, 0x5e22); A = B;
  CYC(0x5e22, 0x5e25); SET_HL(0x5e4d);
  CYC(0x5e25, 0x5e26); forest_fairy_add_double_index(gb, 0x5e26);
  CYC(0x5e26, 0x5e28); E = 0x4b;
  CYC(0x5e28, 0x5e29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5e29, 0x5e2a); mem_wr(gb, DE, A);
  CYC(0x5e2a, 0x5e2c); E = 0x4d;
  CYC(0x5e2c, 0x5e2d); A = mem_rd(gb, HL);
  CYC(0x5e2d, 0x5e2e); mem_wr(gb, DE, A);
  CYC(0x5e2e, 0x5e2f); A = B;
  CYC(0x5e2f, 0x5e30); alu_or(gb, A);
  if (F & FZ) CYC(0x5e30, 0x5e32);
  else { CYCT(0x5e30, 0x5e32); CYC(0x5e32, 0x5e34); A = 1; }
  CALL_C(0x5e34, interactionSetAnimation_hook, 0x262e, 0x5e37);
  forestFairy_initCollisionRadiusAndSetZAndIncState_hook(gb);
}

void forestFairy_initNpcFromData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5efd, 0x5efe); push_effect(gb, HL);
  CALL_C(0x5efe, interactionInitGraphics_hook, 0x15fb, 0x5f01);
  CYC(0x5f01, 0x5f02); SET_HL(pop_effect(gb));
  CYC(0x5f02, 0x5f04); E = 0x72;
  CYC(0x5f04, 0x5f05); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5f05, 0x5f06); mem_wr(gb, DE, A);
  CYC(0x5f06, 0x5f08); E = 0x5b;
  CYC(0x5f08, 0x5f09); A = mem_rd(gb, HL);
  CYC(0x5f09, 0x5f0b); alu_and(gb, 0x0f);
  CYC(0x5f0b, 0x5f0c); mem_wr(gb, DE, A);
  CYC(0x5f0c, 0x5f0d); E = alu_inc8(gb, E);
  CYC(0x5f0d, 0x5f0e); mem_wr(gb, DE, A);
  CYC(0x5f0e, 0x5f0f); A = mem_rd(gb, HL);
  CYC(0x5f0f, 0x5f11); alu_and(gb, 0xf0);
  CYC(0x5f11, 0x5f13); A = alu_swap(gb, A);
  CALL_C(0x5f13, interactionSetAnimation_hook, 0x262e, 0x5f16);
  CALL_C(0x5f16, objectMarkSolidPosition_hook, 0x24f0, 0x5f19);
  CALL_C(0x5f19, interactionIncState_hook, 0x23e0, 0x5f1c);
  CYC(0x5f1c, 0x5f1e); L = 0x4f;
  CYC(0x5f1e, 0x5f20); mem_wr(gb, HL, 0xfc);
  CYC(0x5f20, 0x5f22); L = 0x73;
  CYC(0x5f22, 0x5f24); mem_wr(gb, HL, 0x11);
  CYC(0x5f24, 0x5f27); SET_HL(0x6303);
  CALL_C(0x5f27, interactionSetScript_hook, 0x2544, 0x5f2a);
  CYC(0x5f2a, 0x5f2d); objectSetVisiblec1_hook(gb);
}

void forestFairy_standardUpdate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f33, interactionRunScript_hook, 0x2552, 0x5f36);
  CALL_C(0x5f36, interactionAnimate_hook, 0x261b, 0x5f39);
  CYC(0x5f39, 0x5f3c); objectPreventLinkFromPassing_hook(gb);
}

void forestFairy_subid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5ed7, checkInteractionState_hook, 0x23fe, 0x5eda);
  if (!(F & FZ)) { CYCT(0x5eda, 0x5edc); forestFairy_standardUpdate_hook(gb); return; }
  CYC(0x5eda, 0x5edc);
  CYC(0x5edc, 0x5ede); A = 0x0e;
  CALL_C(0x5ede, checkGlobalFlag_hook, 0x31f3, 0x5ee1);
  if (F & FZ) { CYCT(0x5ee1, 0x5ee4); interactionDelete_hook(gb); return; }
  CYC(0x5ee1, 0x5ee4);
  CYC(0x5ee4, 0x5ee6); A = 0x2b;
  CALL_C(0x5ee6, checkGlobalFlag_hook, 0x31f3, 0x5ee9);
  if (F & FZ) { CYCT(0x5ee9, 0x5eec); interactionDelete_hook(gb); return; }
  CYC(0x5ee9, 0x5eec);
  CYC(0x5eec, 0x5eef); A = mem_rd(gb, 0xc790);
  CYC(0x5eef, 0x5ef1); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x5ef1, 0x5ef4); interactionDelete_hook(gb); return; }
  CYC(0x5ef1, 0x5ef4);
  CYC(0x5ef4, 0x5ef6); E = 0x42;
  CYC(0x5ef6, 0x5ef7); A = mem_rd(gb, DE);
  CYC(0x5ef7, 0x5ef9); alu_sub(gb, 5);
  CYC(0x5ef9, 0x5efc); SET_HL(0x5f2d);
  CYC(0x5efc, 0x5efd); forest_fairy_add_double_index(gb, 0x5efd);
  forestFairy_initNpcFromData_hook(gb);
}

void forestFairy_subid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f3c, checkInteractionState_hook, 0x23fe, 0x5f3f);
  if (!(F & FZ)) { CYCT(0x5f3f, 0x5f41); forestFairy_standardUpdate_hook(gb); return; }
  CYC(0x5f3f, 0x5f41);
  CYC(0x5f41, 0x5f43); A = 0x0e;
  CALL_C(0x5f43, checkGlobalFlag_hook, 0x31f3, 0x5f46);
  if (F & FZ) { CYCT(0x5f46, 0x5f49); interactionDelete_hook(gb); return; }
  CYC(0x5f46, 0x5f49);
  CYC(0x5f49, 0x5f4b); A = 0x2b;
  CALL_C(0x5f4b, checkGlobalFlag_hook, 0x31f3, 0x5f4e);
  if (F & FZ) { CYCT(0x5f4e, 0x5f51); interactionDelete_hook(gb); return; }
  CYC(0x5f4e, 0x5f51);
  CYC(0x5f51, 0x5f54); A = mem_rd(gb, 0xc790);
  CYC(0x5f54, 0x5f56); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(0x5f56, 0x5f59); interactionDelete_hook(gb); return; }
  CYC(0x5f56, 0x5f59);
  CYC(0x5f59, 0x5f5b); A = 0x14;
  CALL_C(0x5f5b, checkGlobalFlag_hook, 0x31f3, 0x5f5e);
  if (!(F & FZ)) { CYCT(0x5f5e, 0x5f61); interactionDelete_hook(gb); return; }
  CYC(0x5f5e, 0x5f61);
  CYC(0x5f61, 0x5f63); E = 0x42;
  CYC(0x5f63, 0x5f64); A = mem_rd(gb, DE);
  CYC(0x5f64, 0x5f66); alu_sub(gb, 8);
  CYC(0x5f66, 0x5f69); SET_HL(0x5f6d);
  CYC(0x5f69, 0x5f6a); forest_fairy_add_double_index(gb, 0x5f6a);
  CYC(0x5f6a, 0x5f6d); forestFairy_initNpcFromData_hook(gb);
}

void forestFairy_subid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f73, checkInteractionState_hook, 0x23fe, 0x5f76);
  if (!(F & FZ)) { CYCT(0x5f76, 0x5f78); forestFairy_standardUpdate_hook(gb); return; }
  CYC(0x5f76, 0x5f78);
  CYC(0x5f78, 0x5f7a); A = 0x14;
  CALL_C(0x5f7a, checkGlobalFlag_hook, 0x31f3, 0x5f7d);
  if (F & FZ) { CYCT(0x5f7d, 0x5f80); interactionDelete_hook(gb); return; }
  CYC(0x5f7d, 0x5f80);
  CALL_C(0x5f80, interactionInitGraphics_hook, 0x15fb, 0x5f83);
  CALL_C(0x5f83, objectMarkSolidPosition_hook, 0x24f0, 0x5f86);
  CALL_C(0x5f86, interactionIncState_hook, 0x23e0, 0x5f89);
  CYC(0x5f89, 0x5f8b); L = 0x4f;
  CYC(0x5f8b, 0x5f8d); mem_wr(gb, HL, 0xfc);
  CYC(0x5f8d, 0x5f8f); L = 0x5c;
  CYC(0x5f8f, 0x5f91); A = 1;
  CYC(0x5f91, 0x5f92); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5f92, 0x5f93); mem_wr(gb, HL, A);
  CYC(0x5f93, 0x5f96); SET_HL(0x630b);
  CALL_C(0x5f96, interactionSetScript_hook, 0x2544, 0x5f99);
  CYC(0x5f99, 0x5f9c); objectSetVisiblec1_hook(gb);
}

void forestFairy_subid0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f9c, checkInteractionState_hook, 0x23fe, 0x5f9f);
  forestFairy_standardUpdate_2_hook(gb);
}

void forestFairy_standardUpdate_2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(0x5f9f, 0x5fa1); forestFairy_standardUpdate_hook(gb); return; }
  CYC(0x5f9f, 0x5fa1);
  CYC(0x5fa1, 0x5fa3); A = 0x14;
  CALL_C(0x5fa3, checkGlobalFlag_hook, 0x31f3, 0x5fa6);
  if (F & FZ) { CYCT(0x5fa6, 0x5fa9); interactionDelete_hook(gb); return; }
  CYC(0x5fa6, 0x5fa9);
  CYC(0x5fa9, 0x5fab); E = 0x42;
  CYC(0x5fab, 0x5fac); A = mem_rd(gb, DE);
  CYC(0x5fac, 0x5fae); alu_sub(gb, 0x0c);
  CYC(0x5fae, 0x5fb1); SET_HL(0x5fb5);
  CYC(0x5fb1, 0x5fb2); forest_fairy_add_double_index(gb, 0x5fb2);
  CYC(0x5fb2, 0x5fb5); forestFairy_initNpcFromData_hook(gb);
}

void forestFairy_subid0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5fb9, checkInteractionState_hook, 0x23fe, 0x5fbc);
  if (!(F & FZ)) { CYCT(0x5fbc, 0x5fbe); forestFairy_standardUpdate_2_hook(gb); return; }
  CYC(0x5fbc, 0x5fbe);
  CYC(0x5fbe, 0x5fc0); A = 0x23;
  CALL_C(0x5fc0, checkGlobalFlag_hook, 0x31f3, 0x5fc3);
  if (!(F & FZ)) { CYCT(0x5fc3, 0x5fc6); interactionDelete_hook(gb); return; }
  CYC(0x5fc3, 0x5fc6);
  CYC(0x5fc6, 0x5fc8); A = 0x2b;
  CALL_C(0x5fc8, checkGlobalFlag_hook, 0x31f3, 0x5fcb);
  if (!(F & FZ)) { CYCT(0x5fcb, 0x5fce); interactionDelete_hook(gb); return; }
  CYC(0x5fcb, 0x5fce);
  CYC(0x5fce, 0x5fd0); A = 0x42;
  CALL_C(0x5fd0, checkGlobalFlag_hook, 0x31f3, 0x5fd3);
  if (F & FZ) { CYCT(0x5fd3, 0x5fd6); interactionDelete_hook(gb); return; }
  CYC(0x5fd3, 0x5fd6);
  CYC(0x5fd6, 0x5fd8); E = 0x42;
  CYC(0x5fd8, 0x5fd9); A = mem_rd(gb, DE);
  CYC(0x5fd9, 0x5fdb); alu_sub(gb, 0x0e);
  CYC(0x5fdb, 0x5fde); SET_HL(0x5fe2);
  CYC(0x5fde, 0x5fdf); forest_fairy_add_double_index(gb, 0x5fdf);
  CYC(0x5fdf, 0x5fe2); forestFairy_initNpcFromData_hook(gb);
}
