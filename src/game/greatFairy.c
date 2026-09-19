#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

void greatFairy_initialize_hook(GB *gb);
void greatFairy_initialize__afterCall7966_hook(GB *gb);
void greatFairy_subid0__afterCall7866_hook(GB *gb);
void greatFairy_subid1__afterCall78b2_hook(GB *gb);

static uint16_t great_fairy_jump_table(GB *gb) {
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

static void great_fairy_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void great_fairy_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

static void great_fairy_initialize_tail(GB *gb, uint16_t sp0_) {
  CALL_C(0x7966, objectMarkSolidPosition_hook, 0x24f0, 0x7969);
  CYC(0x7969, 0x796b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x796b, 0x796c); A = mem_rd(gb, DE);
  CYC(0x796c, 0x796f); SET_HL(0x7979);
  CYC(0x796f, 0x7970); great_fairy_add_double_index(gb, 0x7970);
  CYC(0x7970, 0x7971); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7971, 0x7972); H = mem_rd(gb, HL);
  CYC(0x7972, 0x7973); L = A;
  CALL_C(0x7973, interactionSetScript_hook, 0x2544, 0x7976);
  CYC(0x7976, 0x7979); interactionIncState_hook(gb);
}

void greatFairy_initialize__afterCall7966_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  great_fairy_initialize_tail(gb, sp0_);
}

void greatFairy_initialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7963, interactionInitGraphics_hook, 0x15fb, 0x7966);
  greatFairy_initialize__afterCall7966_hook(gb);
}

static void great_fairy_subid0_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x7874, returnIfScrollMode01Unset_hook, 0x26e4, 0x7877);
  CALL_C(0x7877, interactionRunScript_hook, 0x2552, 0x787a);
  if (F & FC) {
    CYCT(0x787a, 0x787d); interactionDeleteAndUnmarkSolidPosition_hook(gb);
    return;
  }
  CYC(0x787a, 0x787d);
  CYC(0x787d, 0x787f); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x787f, 0x7880); A = mem_rd(gb, DE);
  CYC(0x7880, 0x7881); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7881, 0x7882); ret_effect(gb);
    return;
  }
  CYC(0x7881, 0x7882);
  CALL_C(0x7882, interactionAnimateAsNpc_hook, 0x26db, 0x7885);
  CYC(0x7885, 0x7888); A = W8(wFrameCounter);
  CYC(0x7888, 0x788a); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x788a, 0x788b); ret_effect(gb);
    return;
  }
  CYC(0x788a, 0x788b);
  CYC(0x788b, 0x788e); A = W8(wFrameCounter);
  CYC(0x788e, 0x7890); alu_and(gb, 0x38);
  CYC(0x7890, 0x7892); A = alu_swap(gb, A);
  CYC(0x7892, 0x7893); alu_rlca(gb);
  CYC(0x7893, 0x7896); SET_HL(0x789d);
  CYC(0x7896, 0x7897); great_fairy_add_a_to_hl(gb, 0x7897);
  CYC(0x7897, 0x7899); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x7899, 0x789a); A = mem_rd(gb, DE);
  CYC(0x789a, 0x789b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x789b, 0x789c); mem_wr(gb, DE, A);
  CYC(0x789c, 0x789d); ret_effect(gb);
}

void greatFairy_subid0__afterCall7866_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7866, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7869);
  CYC(0x7869, 0x786b); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x786b, 0x786d); mem_wr(gb, HL, 0xf0);
  CYC(0x786d, 0x786f); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x786f, 0x7871); mem_wr(gb, HL, 0x06);
  CALL_C(0x7871, interactionRunScript_hook, 0x2552, 0x7874);
  great_fairy_subid0_state1(gb, sp0_);
}

void greatFairy_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x785e, checkInteractionState_hook, 0x23fe, 0x7861);
  if (!(F & FZ)) {
    CYCT(0x7861, 0x7863); great_fairy_subid0_state1(gb, sp0_);
    return;
  }
  CYC(0x7861, 0x7863);
  CALL_C(0x7863, greatFairy_initialize_hook, 0x7963, 0x7866);
  greatFairy_subid0__afterCall7866_hook(gb);
}

static void great_fairy_update_sparkles_and_sound(GB *gb, uint16_t sp0_) {
  CYC(0x7923, 0x7926); A = W8(wFrameCounter);
  CYC(0x7926, 0x7928); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x7928, 0x7929); ret_effect(gb);
    return;
  }
  CYC(0x7928, 0x7929);
  CYC(0x7929, 0x792c); SET_BC(0x8402);
  CALL_C(0x792c, objectCreateInteraction_hook, 0x24c5, 0x792f);
  CYC(0x792f, 0x7932); A = W8(wFrameCounter);
  CYC(0x7932, 0x7934); alu_and(gb, 0x1f);
  CYC(0x7934, 0x7936); A = 0x83;
  if (F & FZ) CALL_C_CC(0x7936, playSound_b00_hook, 0x0c98, 0x7939);
  else CYC(0x7936, 0x7939);
  CYC(0x7939, 0x793a); ret_effect(gb);
}

static void great_fairy_subid1_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x78e3, interactionAnimate_hook, 0x261b, 0x78e6);
  CYC(0x78e6, 0x78e8); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x78e8, 0x78e9); A = mem_rd(gb, DE);
  CYC(0x78e9, 0x78ea); push_effect(gb, 0x78ea);
  switch (great_fairy_jump_table(gb)) {
    case 0x78f2: goto substate0;
    case 0x7909: goto substate1;
    case 0x793a: goto substate2;
    case 0x794d: goto substate3;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x78f2, interactionDecCounter1_hook, 0x23cc, 0x78f5);
  if (!(F & FZ)) {
    CYCT(0x78f5, 0x78f6); ret_effect(gb);
    return;
  }
  CYC(0x78f5, 0x78f6);
  CYC(0x78f6, 0x78f8); mem_wr(gb, HL, 0x40);
  CYC(0x78f8, 0x78fa); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x78fa, 0x78fc); mem_wr(gb, HL, 0x08);
  CYC(0x78fc, 0x78fe); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x78fe, 0x7900); mem_wr(gb, HL, 0x78);
  CYC(0x7900, 0x7903); SET_BC(0x4109);
  CALL_C(0x7903, showText_hook, 0x1872, 0x7906);
  CYC(0x7906, 0x7909); interactionIncSubstate_hook(gb);
  return;

substate1:
  CALL_C(0x7909, retIfTextIsActive_hook, 0x1859, 0x790c);
  CALL_C(0x790c, objectApplySpeed_hook, 0x201d, 0x790f);
  CALL_C(0x790f, interactionDecCounter2_hook, 0x23d1, 0x7912);
  if (!(F & FZ)) {
    CYCT(0x7912, 0x7914);
    goto update_sparkles;
  }
  CYC(0x7912, 0x7914);
  CYC(0x7914, 0x7916); mem_wr(gb, HL, 0x02);
  CYC(0x7916, 0x7918); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7918, 0x7919); A = mem_rd(gb, HL);
  CYC(0x7919, 0x791a); A = alu_inc8(gb, A);
  CYC(0x791a, 0x791c); alu_and(gb, 0x1f);
  CYC(0x791c, 0x791d); mem_wr(gb, HL, A);
  CALL_C(0x791d, interactionDecCounter1_hook, 0x23cc, 0x7920);
  if (F & FZ) {
    CYCT(0x7920, 0x7923); interactionIncSubstate_hook(gb);
    return;
  }
  CYC(0x7920, 0x7923);

update_sparkles:
  great_fairy_update_sparkles_and_sound(gb, sp0_);
  return;

substate2:
  CYC(0x793a, 0x793d); push_effect(gb, 0x793d);
  great_fairy_update_sparkles_and_sound(gb, sp0_);
  CYC(0x793d, 0x793e); H = D;
  CYC(0x793e, 0x7940); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x7940, 0x7941); A = mem_rd(gb, HL);
  CYC(0x7941, 0x7943); alu_sub(gb, 0x02);
  CYC(0x7943, 0x7944); mem_wr(gb, HL, A);
  CYC(0x7944, 0x7946); alu_cp(gb, 0xb0);
  if (!(F & FC)) {
    CYCT(0x7946, 0x7947); ret_effect(gb);
    return;
  }
  CYC(0x7946, 0x7947);
  CALL_C(0x7947, fadeoutToWhite_hook, 0x326c, 0x794a);
  CYC(0x794a, 0x794d); interactionIncSubstate_hook(gb);
  return;

substate3:
  CYC(0x794d, 0x7950); A = W8(wPaletteThread_mode);
  CYC(0x7950, 0x7951); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7951, 0x7952); ret_effect(gb);
    return;
  }
  CYC(0x7951, 0x7952);
  CYC(0x7952, 0x7954); A = 0x1e;
  CYC(0x7954, 0x7957); W8(wCutsceneTrigger) = A;
  CYC(0x7957, 0x795a); interactionDelete_hook(gb);
}

void greatFairy_subid1__afterCall78b2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78b2, objectSetVisiblec1_hook, 0x1e3c, 0x78b5);
  CALL_C(0x78b5, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x78b8);
  CYC(0x78b8, 0x78ba); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x78ba, 0x78bc); mem_wr(gb, HL, 0xf0);
  CYC(0x78bc, 0x78be); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x78be, 0x78c0); A = 0xb4;
  CYC(0x78c0, 0x78c1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x78c1, 0x78c3); mem_wr(gb, HL, 0x02);
  CYC(0x78c3, 0x78c6); SET_BC(0x8404);
  CALL_C(0x78c6, objectCreateInteraction_hook, 0x24c5, 0x78c9);
  CYC(0x78c9, 0x78cb); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x78cb, 0x78cd); mem_wr(gb, HL, 0x78);
  CYC(0x78cd, 0x78cf); B = 0x00;

  do {
    CYC(0x78cf, 0x78d0); push_effect(gb, BC);
    CYC(0x78d0, 0x78d3); SET_BC(0x840a);
    CALL_C(0x78d3, objectCreateInteraction_hook, 0x24c5, 0x78d6);
    CYC(0x78d6, 0x78d7); SET_BC(pop_effect(gb));
    CYC(0x78d7, 0x78d9); L = INTERACTION_BASE + OBJ_ANGLE;
    CYC(0x78d9, 0x78da); mem_wr(gb, HL, B);
    CYC(0x78da, 0x78db); A = B;
    CYC(0x78db, 0x78dd); alu_add(gb, 0x04);
    CYC(0x78dd, 0x78de); B = A;
    CYC(0x78de, 0x78e0); alu_bit(gb, 5, A);
    if (!(F & FZ)) {
      CYC(0x78e0, 0x78e2);
      break;
    }
    CYCT(0x78e0, 0x78e2);
  } while (true);
  CYC(0x78e2, 0x78e3); ret_effect(gb);
}

void greatFairy_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78a5, checkInteractionState_hook, 0x23fe, 0x78a8);
  if (!(F & FZ)) {
    CYCT(0x78a8, 0x78aa); great_fairy_subid1_state1(gb, sp0_);
    return;
  }
  CYC(0x78a8, 0x78aa);
  CYC(0x78aa, 0x78ac); A = 0x8a;
  CALL_C(0x78ac, playSound_b00_hook, 0x0c98, 0x78af);
  CALL_C(0x78af, greatFairy_initialize_hook, 0x7963, 0x78b2);
  greatFairy_subid1__afterCall78b2_hook(gb);
}

void interactionCoded5_hook(GB *gb) {
  CYC(0x7856, 0x7858); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7858, 0x7859); A = mem_rd(gb, DE);
  CYC(0x7859, 0x785a); push_effect(gb, 0x785a);
  uint16_t target = great_fairy_jump_table(gb);
  if (target == 0x785e) { greatFairy_subid0_hook(gb); return; }
  greatFairy_subid1_hook(gb); return; // target == 0x78a5
}
