#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode97_jump_table(GB *gb) {
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

static void interactionCode97_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void interaction97_subid00_hook(GB *gb);
void interaction97_subid01_hook(GB *gb);

// INTERAC_97
void interactionCode97_hook(GB *gb) {
  CYC(0x7d49, 0x7d4b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d4b, 0x7d4c); A = mem_rd(gb, DE);
  CYC(0x7d4c, 0x7d4d); push_effect(gb, 0x7d4d);
  uint16_t target = interactionCode97_jump_table(gb);
  if (target == 0x7d51) { interaction97_subid00_hook(gb); return; }
  interaction97_subid01_hook(gb); return; // target == 0x7d8a
}

void interaction97_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d51, checkInteractionState_hook, 0x23fe, 0x7d54);
  if (F & FZ) { CYCT(0x7d54, 0x7d56); goto state0; } // jr z
  CYC(0x7d54, 0x7d56);

  // interaction97_subid00@state1
  CALL_C(0x7d56, interactionDecCounter1_hook, 0x23cc, 0x7d59);
  if (F & FZ) { CYCT(0x7d59, 0x7d5c); interactionDelete_hook(gb); return; } // jp z
  CYC(0x7d59, 0x7d5c);
  CYC(0x7d5c, 0x7d5d); L = alu_inc8(gb, L);
  CYC(0x7d5d, 0x7d5e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl), [counter2]--
  if (!(F & FZ)) { RET_TAKEN(0x7d5e); return; } // ret nz
  CYC(0x7d5e, 0x7d5f);
  CALL_C(0x7d5f, getRandomNumber_hook, 0x043e, 0x7d62);
  CYC(0x7d62, 0x7d64); alu_and(gb, 0x03);
  CYC(0x7d64, 0x7d66); A = 0x03;
  CYC(0x7d66, 0x7d67); mem_wr(gb, HL, A);
  CALL_C(0x7d67, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7d6a);
  CYC(0x7d6a, 0x7d6c); alu_and(gb, 0x1f);
  CYC(0x7d6c, 0x7d6e); alu_sub(gb, 0x10);
  CYC(0x7d6e, 0x7d6f); C = A;
  CALL_C(0x7d6f, getRandomNumber_hook, 0x043e, 0x7d72);
  CYC(0x7d72, 0x7d74); alu_and(gb, 0x07);
  CYC(0x7d74, 0x7d76); alu_sub(gb, 0x04);
  CYC(0x7d76, 0x7d77); B = A;
  CALL_C(0x7d77, getFreeInteractionSlot_hook, 0x3aef, 0x7d7a);
  if (!(F & FZ)) { RET_TAKEN(0x7d7a); return; } // ret nz
  CYC(0x7d7a, 0x7d7b);
  CYC(0x7d7b, 0x7d7d); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(0x7d7d, 0x7d80); objectCopyPositionWithOffset_hook(gb); return; // jp

state0:
  CALL_C(0x7d80, interactionIncState_hook, 0x23e0, 0x7d83);
  CYC(0x7d83, 0x7d85); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7d85, 0x7d87); mem_wr(gb, HL, 0x6a);
  CYC(0x7d87, 0x7d88); L = alu_inc8(gb, L);
  CYC(0x7d88, 0x7d89); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x7d89); return; // ret
}

void interaction97_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d8a, checkInteractionState_hook, 0x23fe, 0x7d8d);
  if (F & FZ) { CYCT(0x7d8d, 0x7d8f); goto state0; } // jr z
  CYC(0x7d8d, 0x7d8f);

  // interaction97_subid01@state1
  CALL_C(0x7d8f, interactionDecCounter1_hook, 0x23cc, 0x7d92);
  if (!(F & FZ)) { RET_TAKEN(0x7d92); return; } // ret nz
  CYC(0x7d92, 0x7d93);
  CYC(0x7d93, 0x7d95); mem_wr(gb, HL, 0x12);
  CYC(0x7d95, 0x7d96); L = alu_inc8(gb, L);
  CYC(0x7d96, 0x7d97); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (F & FZ) { CYCT(0x7d97, 0x7d9a); interactionDelete_hook(gb); return; } // jp z
  CYC(0x7d97, 0x7d9a);
  CALL_C(0x7d9a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7d9d);
  CYC(0x7d9d, 0x7d9f); alu_and(gb, 0x03);
  CYC(0x7d9f, 0x7da1); alu_add(gb, 0x0c);
  CYC(0x7da1, 0x7da2); B = A;

  // interaction97_subid01@spawnBubble; reached here by fallthrough (top-level, sp==sp0_),
  // and also by a genuine `call` from the loop in @state0 below (sp==sp0_-2 there).
spawnBubble:
  CYC(0x7da2, 0x7da3); alu_add(gb, A); // add a
  CYC(0x7da3, 0x7da4); alu_add(gb, B);
  CYC(0x7da4, 0x7da7); SET_HL(0x7dd7); // @positions
  CYC(0x7da7, 0x7da8); interactionCode97_addAToHl_from_rst(gb, 0x7da8);
  CYC(0x7da8, 0x7da9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7da9, 0x7daa); B = A;
  CYC(0x7daa, 0x7dab); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7dab, 0x7dac); C = A;
  CYC(0x7dac, 0x7dad); E = mem_rd(gb, HL);
  CALL_C(0x7dad, getFreePartSlot_hook, 0x3e8e, 0x7db0);
  if (!(F & FZ)) {
    RET_TAKEN(0x7db0);
    if (gb->pc == 0x7dd2 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterSpawnBubble_fromLoop;
    return;
  } // ret nz
  CYC(0x7db0, 0x7db1);
  CYC(0x7db1, 0x7db3); mem_wr(gb, HL, 0x16); // PART_JABU_JABUS_BUBBLES
  CYC(0x7db3, 0x7db4); L = alu_inc8(gb, L);
  CYC(0x7db4, 0x7db5); mem_wr(gb, HL, E);
  CYC(0x7db5, 0x7db7); L = PART_BASE + OBJ_YH;
  CYC(0x7db7, 0x7db8); mem_wr(gb, HL, B);
  CYC(0x7db8, 0x7dba); L = PART_BASE + OBJ_XH;
  CYC(0x7dba, 0x7dbb); mem_wr(gb, HL, C);
  RET(0x7dbb);
  if (gb->pc == 0x7dd2 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterSpawnBubble_fromLoop;
  return; // ret

state0:
  CALL_C(0x7dbc, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7dbf);
  CALL_C(0x7dbf, interactionIncState_hook, 0x23e0, 0x7dc2);
  CYC(0x7dc2, 0x7dc4); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7dc4, 0x7dc6); mem_wr(gb, HL, 30);
  CYC(0x7dc6, 0x7dc7); L = alu_inc8(gb, L);
  CYC(0x7dc7, 0x7dc9); mem_wr(gb, HL, 0x04); // [counter2]
  CYC(0x7dc9, 0x7dcb); B = 0x0c;

  // interaction97_subid01@state0_loop
loop:
  CYC(0x7dcb, 0x7dcc); push_effect(gb, BC); // push bc
  CYC(0x7dcc, 0x7dcd); A = B;
  CYC(0x7dcd, 0x7dce); B = alu_dec8(gb, B);
  CYC(0x7dce, 0x7dcf); A = alu_dec8(gb, A);
  CYC(0x7dcf, 0x7dd2); push_effect(gb, 0x7dd2); goto spawnBubble;

afterSpawnBubble_fromLoop:
  CYC(0x7dd2, 0x7dd3); SET_BC(pop_effect(gb)); // pop bc
  CYC(0x7dd3, 0x7dd4); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x7dd4, 0x7dd6); goto loop; } // jr nz
  CYC(0x7dd4, 0x7dd6);
  RET(0x7dd6); return; // ret
}
