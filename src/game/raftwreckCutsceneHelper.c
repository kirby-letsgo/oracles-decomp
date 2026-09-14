#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t raftwreck_jump_table(GB *gb) {
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

static void raftwreck_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void raftwreck_load_angle_and_counter_preset(GB *gb, uint16_t sp0_) {
  CYC(0x73ef, 0x73f1); B = 3;
  CYC(0x73f1, 0x73f4); SET_HL(0x76b8);
  CYC(0x73f4, 0x73f6); E = 0x0a;
  CALL_C(0x73f6, interBankCall_hook, 0x008a, 0x73f9);
  CYC(0x73f9, 0x73fa); A = B;
  CYC(0x73fa, 0x73fb); alu_or(gb, A);
  CYC(0x73fb, 0x73fc); ret_effect(gb);
}

static void raftwreck_read_spawn_data(GB *gb) {
  CYC(0x73fd, 0x73ff); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x73ff, 0x7400); A = mem_rd(gb, DE);
  CYC(0x7400, 0x7401); alu_add(gb, A);
  CYC(0x7401, 0x7402); raftwreck_add_double_index(gb, 0x7402);
  CYC(0x7402, 0x7403); B = mem_rd(gb, HL);
  CYC(0x7403, 0x7404); SET_HL(HL + 1);
  CYC(0x7404, 0x7405); C = mem_rd(gb, HL);
  CYC(0x7405, 0x7406); SET_HL(HL + 1);
  CYC(0x7406, 0x7407); E = mem_rd(gb, HL);
  CYC(0x7407, 0x7408); SET_HL(HL + 1);
  CYC(0x7408, 0x7409); A = mem_rd(gb, HL);
  CYC(0x7409, 0x740a); H = D;
  CYC(0x740a, 0x740c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x740c, 0x740d); mem_wr(gb, HL, A);
  CYC(0x740d, 0x740e); ret_effect(gb);
}

static void raftwreck_finish_initialization(GB *gb, uint16_t sp0_) {
  CALL_C(0x73ec, objectSetVisible82_hook, 0x1e69, 0x73ef);
  raftwreck_load_angle_and_counter_preset(gb, sp0_);
}

void interactionCode64__afterCall73ec_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  raftwreck_finish_initialization(gb, sp0_);
}

static void raftwreck_state0(GB *gb, uint16_t sp0_) {
  CALL_C(0x73d6, interactionIncState_hook, 0x23e0, 0x73d9);
  CYC(0x73d9, 0x73db); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x73db, 0x73dc); A = mem_rd(gb, DE);
  CYC(0x73dc, 0x73dd); push_effect(gb, 0x73dd);
  switch (raftwreck_jump_table(gb)) {
    case 0x73e9:
      CALL_C(0x73e9, interactionInitGraphics_hook, 0x15fb, 0x73ec);
      raftwreck_finish_initialization(gb, sp0_);
      return;
    case 0x73fc:
      CYC(0x73fc, 0x73fd); ret_effect(gb);
      return;
    default:
      hook_continue(gb, HL, sp0_);
      return;
  }
}

static void raftwreck_run_subid00(GB *gb, uint16_t sp0_) {
  CALL_C(0x741e, interactionAnimate_hook, 0x261b, 0x7421);
  CALL_C(0x7421, objectApplySpeed_hook, 0x201d, 0x7424);
  CYC(0x7424, 0x7426); alu_cp(gb, 0xf0);
  if (!(F & FC)) {
    CYCT(0x7426, 0x7429); interactionDelete_hook(gb);
    return;
  }
  CYC(0x7426, 0x7429);
  CALL_C(0x7429, interactionDecCounter1_hook, 0x23cc, 0x742c);
  if (F & FZ) {
    CYCT(0x742c, 0x742f); push_effect(gb, 0x742f);
    raftwreck_load_angle_and_counter_preset(gb, sp0_);
  } else {
    CYC(0x742c, 0x742f);
  }
  if (F & FZ) {
    CYCT(0x742f, 0x7432); interactionDelete_hook(gb);
    return;
  }
  CYC(0x742f, 0x7432);
  CYC(0x7432, 0x7433); ret_effect(gb);
}

static void raftwreck_run_subid03(GB *gb, uint16_t sp0_) {
  CYC(0x7433, 0x7434); H = D;
  CYC(0x7434, 0x7436); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7436, 0x7437); A = mem_rd(gb, HL);
  CYC(0x7437, 0x7438); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(0x7438, 0x743a);
    CYC(0x743a, 0x743b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x743b, 0x743c); ret_effect(gb);
    return;
  }
  CYCT(0x7438, 0x743a);
  CYC(0x743c, 0x743f); SET_HL(0x7475);
  CYC(0x743f, 0x7441); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7441, 0x7442); A = mem_rd(gb, DE);
  CYC(0x7442, 0x7444); alu_cp(gb, 3);
  if (F & FZ) {
    CYCT(0x7444, 0x7446);
  } else {
    CYC(0x7444, 0x7446);
    CYC(0x7446, 0x7449); SET_HL(0x7489);
  }
  CYC(0x7449, 0x744c); push_effect(gb, 0x744c);
  raftwreck_read_spawn_data(gb);
  CALL_C(0x744c, getFreeInteractionSlot_hook, 0x3aef, 0x744f);
  if (!(F & FZ)) {
    CYCT(0x744f, 0x7450); ret_effect(gb);
    return;
  }
  CYC(0x744f, 0x7450);
  CYC(0x7450, 0x7452); mem_wr(gb, HL, 0x64);
  CYC(0x7452, 0x7453); L = alu_inc8(gb, L);
  CYC(0x7453, 0x7454); mem_wr(gb, HL, E);
  CYC(0x7454, 0x7455); L = alu_inc8(gb, L);
  CYC(0x7455, 0x7457); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7457, 0x7458); A = mem_rd(gb, DE);
  CYC(0x7458, 0x7459); mem_wr(gb, HL, A);
  CYC(0x7459, 0x745b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x745b, 0x745c); A = mem_rd(gb, DE);
  CYC(0x745c, 0x745e); alu_cp(gb, 3);
  CYC(0x745e, 0x7460); A = 0x50;
  if (F & FZ) {
    CYCT(0x7460, 0x7462);
  } else {
    CYC(0x7460, 0x7462);
    CYC(0x7462, 0x7464); A = 0x78;
  }
  CYC(0x7464, 0x7466); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7466, 0x7467); mem_wr(gb, HL, A);
  CALL_C(0x7467, interactionHSetPosition_hook, 0x2774, 0x746a);
  CYC(0x746a, 0x746b); H = D;
  CYC(0x746b, 0x746d); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x746d, 0x746e); A = mem_rd(gb, HL);
  CYC(0x746e, 0x746f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x746f, 0x7472); interactionDelete_hook(gb);
    return;
  }
  CYC(0x746f, 0x7472);
  CYC(0x7472, 0x7473); L = alu_inc8(gb, L);
  CYC(0x7473, 0x7474); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7474, 0x7475); ret_effect(gb);
}

static void raftwreck_run_subid05(GB *gb, uint16_t sp0_) {
  CYC(0x74c9, 0x74ca); H = D;
  CYC(0x74ca, 0x74cc); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x74cc, 0x74cd); A = mem_rd(gb, HL);
  CYC(0x74cd, 0x74ce); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(0x74ce, 0x74d0);
    CYC(0x74d0, 0x74d1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x74d1, 0x74d2); ret_effect(gb);
    return;
  }
  CYCT(0x74ce, 0x74d0);
  CYC(0x74d2, 0x74d5); SET_HL(0x74fa);
  CYC(0x74d5, 0x74d8); push_effect(gb, 0x74d8);
  raftwreck_read_spawn_data(gb);
  CALL_C(0x74d8, getFreePartSlot_hook, 0x3e8e, 0x74db);
  if (!(F & FZ)) {
    CYCT(0x74db, 0x74dc); ret_effect(gb);
    return;
  }
  CYC(0x74db, 0x74dc);
  CYC(0x74dc, 0x74de); mem_wr(gb, HL, 0x27);
  CYC(0x74de, 0x74df); L = alu_inc8(gb, L);
  CYC(0x74df, 0x74e0); mem_wr(gb, HL, E);
  CYC(0x74e0, 0x74e1); L = alu_inc8(gb, L);
  CYC(0x74e1, 0x74e2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x74e2, 0x74e4); L = PART_BASE + OBJ_YH;
  CYC(0x74e4, 0x74e5); mem_wr(gb, HL, B);
  CYC(0x74e5, 0x74e7); L = PART_BASE + OBJ_XH;
  CYC(0x74e7, 0x74e8); mem_wr(gb, HL, C);
  CYC(0x74e8, 0x74e9); H = D;
  CYC(0x74e9, 0x74eb); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x74eb, 0x74ec); A = mem_rd(gb, HL);
  CYC(0x74ec, 0x74ed); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x74ed, 0x74ef);
    CYC(0x74f2, 0x74f4); A = 3;
    CYC(0x74f4, 0x74f7); W8(wTmpcfc0) = A;
    CYC(0x74f7, 0x74fa); interactionDelete_hook(gb);
    return;
  }
  CYC(0x74ed, 0x74ef);
  CYC(0x74ef, 0x74f0); L = alu_inc8(gb, L);
  CYC(0x74f0, 0x74f1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x74f1, 0x74f2); ret_effect(gb);
}

static void raftwreck_state1(GB *gb, uint16_t sp0_) {
  CYC(0x740e, 0x7410); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7410, 0x7411); A = mem_rd(gb, DE);
  CYC(0x7411, 0x7412); push_effect(gb, 0x7412);
  switch (raftwreck_jump_table(gb)) {
    case 0x741e: raftwreck_run_subid00(gb, sp0_); return;
    case 0x7433: raftwreck_run_subid03(gb, sp0_); return;
    case 0x74c9: raftwreck_run_subid05(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void interactionCode64_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x73ce, 0x73d0); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x73d0, 0x73d1); A = mem_rd(gb, DE);
  CYC(0x73d1, 0x73d2); push_effect(gb, 0x73d2);
  switch (raftwreck_jump_table(gb)) {
    case 0x73d6: raftwreck_state0(gb, sp0_); return;
    case 0x740e: raftwreck_state1(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
