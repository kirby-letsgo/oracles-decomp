#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

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
  CYC(0x738c, 0x738d); push_effect(gb, 0x738d);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode63_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7352, 0x7354); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7354, 0x7355); A = mem_rd(gb, DE);
  CYC(0x7355, 0x7356); push_effect(gb, 0x7356);
  switch (accessory_jump_table(gb)) {
    case 0x735a: goto state0;
    case 0x7360: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x735a, 0x735c); A = 1;
  CYC(0x735c, 0x735d); mem_wr(gb, DE, A);
  CALL_C(0x735d, interactionInitGraphics_hook, 0x15fb, 0x7360);

state1:
  CYC(0x7360, 0x7362); A = 0;
  CALL_C(0x7362, objectGetRelatedObject1Var_hook, 0x2160, 0x7365);
  CYC(0x7365, 0x7367); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x7367, 0x7368); A = mem_rd(gb, HL);
  CYC(0x7368, 0x7369); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7369, 0x736b);
    goto delete;
  }
  CYC(0x7369, 0x736b);
  CYC(0x736b, 0x736d); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x736d, 0x736e); A = mem_rd(gb, HL);
  CYC(0x736e, 0x736f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x736f, 0x7371);
    goto delete;
  }
  CYC(0x736f, 0x7371);
  CYC(0x7371, 0x7373); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x7373, 0x7375); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x7375, 0x7378);
    objectSetInvisible_hook(gb);
    return;
  }
  CYC(0x7375, 0x7378);
  CALL_C(0x7378, objectSetVisible80_hook, 0x1e57, 0x737b);
  CYC(0x737b, 0x737e); SET_BC(0xf400);
  CYC(0x737e, 0x7380); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7380, 0x7381); A = mem_rd(gb, DE);
  CYC(0x7381, 0x7382); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7382, 0x7384);
    goto take_position_with_offset;
  }
  CYC(0x7382, 0x7384);
  CYC(0x7384, 0x7386); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x7386, 0x7387); A = mem_rd(gb, HL);
  CYC(0x7387, 0x7388); push_effect(gb, HL);
  CYC(0x7388, 0x7389); alu_add(gb, A);
  CYC(0x7389, 0x738c); SET_HL(0x73aa);
  accessory_add_double_index(gb);
  CYC(0x738d, 0x738e); B = mem_rd(gb, HL);
  CYC(0x738e, 0x738f); SET_HL(HL + 1);
  CYC(0x738f, 0x7390); C = mem_rd(gb, HL);
  CYC(0x7390, 0x7391); SET_HL(HL + 1);
  CYC(0x7391, 0x7392); A = mem_rd(gb, HL);
  CYC(0x7392, 0x7394); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x7394, 0x7395); mem_wr(gb, DE, A);
  CYC(0x7395, 0x7396); SET_HL(HL + 1);
  CYC(0x7396, 0x7398); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x7398, 0x7399); A = mem_rd(gb, DE);
  CYC(0x7399, 0x739a); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x739a, 0x739c);
    goto animation_done;
  }
  CYC(0x739a, 0x739c);
  CYC(0x739c, 0x739d); A = mem_rd(gb, HL);
  CYC(0x739d, 0x739e); mem_wr(gb, DE, A);
  CYC(0x739e, 0x739f); push_effect(gb, BC);
  CALL_C(0x739f, interactionSetAnimation_hook, 0x262e, 0x73a2);
  CYC(0x73a2, 0x73a3); SET_BC(pop_effect(gb));

animation_done:
  CYC(0x73a3, 0x73a4); SET_HL(pop_effect(gb));

take_position_with_offset:
  CYC(0x73a4, 0x73a7);
  objectTakePositionWithOffset_hook(gb);
  return;

delete:
  CYC(0x73a7, 0x73aa);
  interactionDelete_hook(gb);
}
