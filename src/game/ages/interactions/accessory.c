#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t accessory_jump_table(GB *gb) {
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

static void accessory_add_double_index(GB *gb) {
  BASE(interactionCode63);
  CYC(b_+58, b_+59); push_effect(gb, b_+59);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode63_hook(GB *gb) {
  BASE(interactionCode63);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (accessory_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+14) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);

state1:
  CYC(b_+14, b_+16); A = 0;
  CALL_C(b_+16, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+19);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+23, b_+25);
    goto delete;
  }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+31);
    goto delete;
  }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+33, b_+35); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+35, b_+38);
    TAIL(objectSetInvisible);
  }
  CYC(b_+35, b_+38);
  CALL_C(b_+38, objectSetVisible80_hook, SYM(objectSetVisible80), b_+41);
  CYC(b_+41, b_+44); SET_BC(0xf400);
  CYC(b_+44, b_+46); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+48, b_+50);
    goto take_position_with_offset;
  }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+54); push_effect(gb, HL);
  CYC(b_+54, b_+55); alu_add(gb, A);
  CYC(b_+55, b_+58); SET_HL(b_+88);
  accessory_add_double_index(gb);
  CYC(b_+59, b_+60); B = mem_rd(gb, HL);
  CYC(b_+60, b_+61); SET_HL(HL + 1);
  CYC(b_+61, b_+62); C = mem_rd(gb, HL);
  CYC(b_+62, b_+63); SET_HL(HL + 1);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+66); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+68); SET_HL(HL + 1);
  CYC(b_+68, b_+70); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+70, b_+71); A = mem_rd(gb, DE);
  CYC(b_+71, b_+72); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+72, b_+74);
    goto animation_done;
  }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CYC(b_+75, b_+76); mem_wr(gb, DE, A);
  CYC(b_+76, b_+77); push_effect(gb, BC);
  CALL_C(b_+77, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+80);
  CYC(b_+80, b_+81); SET_BC(pop_effect(gb));

animation_done:
  CYC(b_+81, b_+82); SET_HL(pop_effect(gb));

take_position_with_offset:
  CYC(b_+82, b_+85);
  TAIL(objectTakePositionWithOffset);

delete:
  CYC(b_+85, b_+88);
  interactionDelete_hook(gb);
}
