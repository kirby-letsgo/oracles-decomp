#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCode99_jump_table(GB *gb) {
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

static void interactionCode99_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_EXPLOSION_WITH_DEBRIS
// ==================================================================================================
void interactionCode99_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x501c, 0x501e); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x501e, 0x501f); A = mem_rd(gb, DE);
  CYC(0x501f, 0x5020); push_effect(gb, 0x5020);
  switch (interactionCode99_jump_table(gb)) {
    case 0x5024: goto state0;
    case 0x508e: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x5024, 0x5026); A = 0x01;
  CYC(0x5026, 0x5027); mem_wr(gb, DE, A);
  CALL_C(0x5027, interactionInitGraphics_hook, 0x15fb, 0x502a);
  CALL_C(0x502a, objectSetVisible81_hook, 0x1e60, 0x502d);
  CYC(0x502d, 0x502f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x502f, 0x5030); A = mem_rd(gb, DE);
  CYC(0x5030, 0x5031); push_effect(gb, 0x5031);
  switch (interactionCode99_jump_table(gb)) {
    case 0x5037: goto initSubid00;
    case 0x5062: goto initSubid01;
    case 0x505a: goto initSubid02;
    default: hook_continue(gb, HL, sp0_); return;
  }

initSubid00:
  CYC(0x5037, 0x5038); E = alu_inc8(gb, E);
  CYC(0x5038, 0x5039); A = mem_rd(gb, DE);
  CYC(0x5039, 0x503a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x503a, 0x503b); ret_effect(gb); return; } // ret z
  CYC(0x503a, 0x503b);
  CALL_C(0x503b, getRandomNumber_noPreserveVars_hook, 0x0453, 0x503e);
  CYC(0x503e, 0x5040); alu_and(gb, 0x03);
  CYC(0x5040, 0x5043); SET_HL(0x50cf); // @subid0Positions
  CYC(0x5043, 0x5044); interactionCode99_add_double_index_from_rst(gb, 0x5044);
  CALL_C(0x5044, getRandomNumber_hook, 0x043e, 0x5047);
  CYC(0x5047, 0x5049); alu_and(gb, 0x07);
  CYC(0x5049, 0x504b); alu_sub(gb, 0x03);
  CYC(0x504b, 0x504c); alu_add(gb, mem_rd(gb, HL));
  CYC(0x504c, 0x504d); B = A;
  CYC(0x504d, 0x504e); SET_HL(HL + 1);
  CALL_C(0x504e, getRandomNumber_hook, 0x043e, 0x5051);
  CYC(0x5051, 0x5053); alu_and(gb, 0x07);
  CYC(0x5053, 0x5055); alu_sub(gb, 0x03);
  CYC(0x5055, 0x5056); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5056, 0x5057); C = A;
  CYC(0x5057, 0x505a); interactionSetPosition_hook(gb); return; // jp

initSubid02:
  CYC(0x505a, 0x505c); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x505c, 0x505d); A = mem_rd(gb, DE);
  CYC(0x505d, 0x505f); A = (uint8_t)(A & ~(1 << 6));
  CYC(0x505f, 0x5061); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x5061, 0x5062); mem_wr(gb, DE, A);

initSubid01:
  CALL_C(0x5062, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5065);
  CYC(0x5065, 0x5067); alu_and(gb, 0x03);
  CYC(0x5067, 0x5069); alu_add(gb, 0x02);
  CYC(0x5069, 0x506a); C = A;
  CYC(0x506a, 0x506b); H = D;
  CYC(0x506b, 0x506d); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x506d, 0x506e); A = mem_rd(gb, HL);
  CYC(0x506e, 0x506f); alu_add(gb, A);
  CYC(0x506f, 0x5070); alu_add(gb, A);
  CYC(0x5070, 0x5071); alu_add(gb, A);
  CYC(0x5071, 0x5072); alu_add(gb, C);
  CYC(0x5072, 0x5074); alu_and(gb, 0x1f);
  CYC(0x5074, 0x5076); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5076, 0x5077); mem_wr(gb, HL, A);
  CALL_C(0x5077, getRandomNumber_hook, 0x043e, 0x507a);
  CYC(0x507a, 0x507c); alu_and(gb, 0x03);
  CYC(0x507c, 0x507f); SET_BC(0x50bf); // @subid1And2Speeds
  CALL_C(0x507f, addAToBc_hook, 0x006d, 0x5082);
  CYC(0x5082, 0x5083); A = mem_rd(gb, BC);
  CYC(0x5083, 0x5085); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5085, 0x5086); mem_wr(gb, HL, A);
  CYC(0x5086, 0x5088); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x5088, 0x508a); mem_wr(gb, HL, 0x80);
  CYC(0x508a, 0x508b); L = alu_inc8(gb, L);
  CYC(0x508b, 0x508d); mem_wr(gb, HL, 0xfe);
  RET(0x508d); return;

state1:
  CYC(0x508e, 0x5090); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5090, 0x5091); A = mem_rd(gb, DE);
  CYC(0x5091, 0x5092); push_effect(gb, 0x5092);
  switch (interactionCode99_jump_table(gb)) {
    case 0x5098: goto runSubid0;
    case 0x50c3: goto runSubid1Or2;
    default: hook_continue(gb, HL, sp0_); return;
  }

runSubid0:
  CALL_C(0x5098, interactionAnimate_hook, 0x261b, 0x509b);
  CYC(0x509b, 0x509d); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x509d, 0x509e); A = mem_rd(gb, DE);
  CYC(0x509e, 0x509f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x509f, 0x50a0); ret_effect(gb); return; } // ret z
  CYC(0x509f, 0x50a0);
  CYC(0x50a0, 0x50a1); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x50a1, 0x50a4); interactionDelete_hook(gb); return; } // jp z
  CYC(0x50a1, 0x50a4);
  CYC(0x50a4, 0x50a5); alu_xor(gb, A);
  CYC(0x50a5, 0x50a6); mem_wr(gb, DE, A);
  CYC(0x50a6, 0x50a8); hram_wr(gb, 0x8b, A);
  CYC(0x50a8, 0x50aa); hram_wr(gb, 0x8d, A);
  CYC(0x50aa, 0x50ac); hram_wr(gb, 0x8c, A);
  CYC(0x50ac, 0x50ae); B = 0x04;

spawnDebrisLoop:
  CALL_C(0x50ae, getFreeInteractionSlot_hook, 0x3aef, 0x50b1);
  if (!(F & FZ)) { CYCT(0x50b1, 0x50b2); ret_effect(gb); return; } // ret nz
  CYC(0x50b1, 0x50b2);
  CYC(0x50b2, 0x50b4); mem_wr(gb, HL, 0x99); // INTERAC_EXPLOSION_WITH_DEBRIS
  CYC(0x50b4, 0x50b5); L = alu_inc8(gb, L);
  CYC(0x50b5, 0x50b6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x50b6, 0x50b7); L = alu_inc8(gb, L);
  CYC(0x50b7, 0x50b8); mem_wr(gb, HL, B);
  CALL_C(0x50b8, objectCopyPosition_hook, 0x2242, 0x50bb);
  CYC(0x50bb, 0x50bc); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x50bc, 0x50be); goto spawnDebrisLoop; } // jr nz
  CYC(0x50bc, 0x50be);
  RET(0x50be); return;

runSubid1Or2:
  CALL_C(0x50c3, objectApplySpeed_hook, 0x201d, 0x50c6);
  CYC(0x50c6, 0x50c8); C = 0x28;
  CALL_C(0x50c8, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x50cb);
  if (F & FZ) { CYCT(0x50cb, 0x50ce); interactionDelete_hook(gb); return; } // jp z
  CYC(0x50cb, 0x50ce);
  RET(0x50ce); return;
}
