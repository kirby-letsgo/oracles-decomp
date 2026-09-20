#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode97), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode97), (from), (to), true)

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
  BASE(interactionCode97);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = interactionCode97_jump_table(gb);
  if (target == SYM(interaction97_subid00)) { interaction97_subid00_hook(gb); return; }
  interaction97_subid01_hook(gb); return; // target == 0x7d8a
}

void interaction97_subid00_hook(GB *gb) {
  BASE(interaction97_subid00);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; } // jr z
  CYC(b_+3, b_+5);

  // interaction97_subid00@state1
  CALL_C(b_+5, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl), [counter2]--
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } // ret nz
  CYC(b_+13, b_+14);
  CALL_C(b_+14, getRandomNumber_hook, SYM(getRandomNumber), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x03);
  CYC(b_+19, b_+21); A = 0x03;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CALL_C(b_+22, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+25);
  CYC(b_+25, b_+27); alu_and(gb, 0x1f);
  CYC(b_+27, b_+29); alu_sub(gb, 0x10);
  CYC(b_+29, b_+30); C = A;
  CALL_C(b_+30, getRandomNumber_hook, SYM(getRandomNumber), b_+33);
  CYC(b_+33, b_+35); alu_and(gb, 0x07);
  CYC(b_+35, b_+37); alu_sub(gb, 0x04);
  CYC(b_+37, b_+38); B = A;
  CALL_C(b_+38, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+41);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; } // ret nz
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+44, b_+47); objectCopyPositionWithOffset_hook(gb); return; // jp

state0:
  CALL_C(b_+47, interactionIncState_hook, SYM(interactionIncState), b_+50);
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x6a);
  CYC(b_+54, b_+55); L = alu_inc8(gb, L);
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+56); return; // ret
}

void interaction97_subid01_hook(GB *gb) {
  BASE(interaction97_subid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; } // jr z
  CYC(b_+3, b_+5);

  // interaction97_subid01@state1
  CALL_C(b_+5, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x12);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (F & FZ) { CYCT(b_+13, b_+16); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+13, b_+16);
  CALL_C(b_+16, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+19);
  CYC(b_+19, b_+21); alu_and(gb, 0x03);
  CYC(b_+21, b_+23); alu_add(gb, 0x0c);
  CYC(b_+23, b_+24); B = A;

  // interaction97_subid01@spawnBubble; reached here by fallthrough (top-level, sp==sp0_),
  // and also by a genuine `call` from the loop in @state0 below (sp==sp0_-2 there).
spawnBubble:
  CYC(b_+24, b_+25); alu_add(gb, A); // add a
  CYC(b_+25, b_+26); alu_add(gb, B);
  CYC(b_+26, b_+29); SET_HL(b_+77); // @positions
  CYC(b_+29, b_+30); interactionCode97_addAToHl_from_rst(gb, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+31, b_+32); B = A;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+33, b_+34); C = A;
  CYC(b_+34, b_+35); E = mem_rd(gb, HL);
  CALL_C(b_+35, getFreePartSlot_hook, SYM(getFreePartSlot), b_+38);
  if (!(F & FZ)) {
    RET_TAKEN(b_+38);
    if (gb->pc == b_+72 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterSpawnBubble_fromLoop;
    return;
  } // ret nz
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x16); // PART_JABU_JABUS_BUBBLES
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+43); mem_wr(gb, HL, E);
  CYC(b_+43, b_+45); L = PART_BASE + OBJ_YH;
  CYC(b_+45, b_+46); mem_wr(gb, HL, B);
  CYC(b_+46, b_+48); L = PART_BASE + OBJ_XH;
  CYC(b_+48, b_+49); mem_wr(gb, HL, C);
  RET(b_+49);
  if (gb->pc == b_+72 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterSpawnBubble_fromLoop;
  return; // ret

state0:
  CALL_C(b_+50, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+53);
  CALL_C(b_+53, interactionIncState_hook, SYM(interactionIncState), b_+56);
  CYC(b_+56, b_+58); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 30);
  CYC(b_+60, b_+61); L = alu_inc8(gb, L);
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x04); // [counter2]
  CYC(b_+63, b_+65); B = 0x0c;

  // interaction97_subid01@state0_loop
loop:
  CYC(b_+65, b_+66); push_effect(gb, BC); // push bc
  CYC(b_+66, b_+67); A = B;
  CYC(b_+67, b_+68); B = alu_dec8(gb, B);
  CYC(b_+68, b_+69); A = alu_dec8(gb, A);
  CYC(b_+69, b_+72); push_effect(gb, b_+72); goto spawnBubble;

afterSpawnBubble_fromLoop:
  CYC(b_+72, b_+73); SET_BC(pop_effect(gb)); // pop bc
  CYC(b_+73, b_+74); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+74, b_+76); goto loop; } // jr nz
  CYC(b_+74, b_+76);
  RET(b_+76); return; // ret
}
