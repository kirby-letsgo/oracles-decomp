#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t boy2_jump_table(GB *gb) {
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

static void boy2_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void boy2_initialize_script(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3f);
  CYC(b_+189, b_+191); A = 0x29;
  CALL_C(b_+191, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+194);
  CYC(b_+194, b_+196); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+196, b_+197); A = mem_rd(gb, DE);
  CYC(b_+197, b_+200); SET_HL(b_+210);
  CYC(b_+200, b_+201); boy2_add_double_index(gb, b_+201);
  CYC(b_+201, b_+202); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+202, b_+203); H = mem_rd(gb, HL);
  CYC(b_+203, b_+204); L = A;
  CALL_C(b_+204, interactionSetScript_hook, SYM(interactionSetScript), b_+207);
  CYC(b_+207, b_+210); interactionIncState_hook(gb);
}

static void boy2_init_graphics_and_inc_state(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3f);
  CALL_C(b_+174, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+177);
  CALL_C(b_+177, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+180);
  CYC(b_+180, b_+183); interactionIncState_hook(gb);
}

static void boy2_initialize_graphics_and_script(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3f);
  CALL_C(b_+183, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+186);
  CALL_C(b_+186, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+189);
  boy2_initialize_script(gb, sp0_);
}

void interactionCode3f_hook(GB *gb) {
  BASE(interactionCode3f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boy2_jump_table(gb));
    if (jt_ == b_+12) { goto subid0; }
    else if (jt_ == b_+42) { goto subid1; }
    else if (jt_ == b_+70) { goto subid2; }
    else if (jt_ == b_+147) { goto subid3; }
    else { HANDOFF(HL); }
  } while (0);

subid0:
  CALL_C(b_+12, checkInteractionState_hook, SYM(checkInteractionState), b_+15);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17); goto subid0_state1;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x14;
  CALL_C(b_+19, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+22);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+25); TAIL(interactionDelete);
  }
  CYC(b_+22, b_+25);
  CYC(b_+25, b_+27); A = 0x0b;
  CALL_C(b_+27, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+30);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+33); TAIL(interactionDelete);
  }
  CYC(b_+30, b_+33);
  CYC(b_+33, b_+36); push_effect(gb, b_+36);
  boy2_initialize_graphics_and_script(gb, sp0_);
subid0_state1:
  CALL_C(b_+36, interactionRunScript_hook, SYM(interactionRunScript), b_+39);
  CYC(b_+39, b_+42); npcFaceLinkAndAnimate_hook(gb);
  return;

subid1:
  CALL_C(b_+42, checkInteractionState_hook, SYM(checkInteractionState), b_+45);
  if (!(F & FZ)) {
    CYCT(b_+45, b_+47); goto subid1_state1;
  }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+50); SET_HL(SYM(getGameProgress_1));
  CYC(b_+50, b_+52); E = 0x09;
  CALL_C(b_+52, interBankCall_hook, 0x008a, b_+55);
  CYC(b_+55, b_+56); A = B;
  CYC(b_+56, b_+58); alu_cp(gb, 3);
  if (!(F & FZ)) {
    CYCT(b_+58, b_+61); TAIL(interactionDelete);
  }
  CYC(b_+58, b_+61);
  CYC(b_+61, b_+64); push_effect(gb, b_+64);
  boy2_initialize_graphics_and_script(gb, sp0_);
subid1_state1:
  CALL_C(b_+64, interactionRunScript_hook, SYM(interactionRunScript), b_+67);
  CYC(b_+67, b_+70); npcFaceLinkAndAnimate_hook(gb);
  return;

subid2:
  CALL_C(b_+70, checkInteractionState_hook, SYM(checkInteractionState), b_+73);
  if (!(F & FZ)) {
    CYCT(b_+73, b_+75); goto subid2_state1;
  }
  CYC(b_+73, b_+75);
  CALL_C(b_+75, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+78);
  CYC(b_+78, b_+80); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+80, b_+83); TAIL(interactionDelete);
  }
  CYC(b_+80, b_+83);
  CYC(b_+83, b_+86); push_effect(gb, b_+86);
  boy2_init_graphics_and_inc_state(gb, sp0_);
  CYC(b_+86, b_+88); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); mem_wr(gb, HL, A);
  CYC(b_+92, b_+95); objectSetVisiblec2_hook(gb);
  return;

subid2_state1:
  CYC(b_+95, b_+97); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  CYC(b_+98, b_+99); push_effect(gb, b_+99);
  do { uint16_t jt_ = (boy2_jump_table(gb));
    if (jt_ == b_+105) { goto substate0; }
    else if (jt_ == b_+125) { goto substate1; }
    else if (jt_ == b_+139) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+105, interactionAnimate_hook, SYM(interactionAnimate), b_+108);
  CYC(b_+108, b_+111); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd1);
  CYC(b_+111, b_+113); alu_cp(gb, 1);
  if (!(F & FZ)) {
    CYCT(b_+113, b_+114); ret_effect(gb); return;
  }
  CYC(b_+113, b_+114);
  CALL_C(b_+114, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+117);
  CYC(b_+117, b_+120); SET_HL((SYM(interactionCode64__state0) + 5));
  CYC(b_+120, b_+122); E = 0x08;
  CYC(b_+122, b_+125); interBankCall_hook(gb);
  return;

substate1:
  CYC(b_+125, b_+127); C = 0x20;
  CALL_C(b_+127, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+130);
  if (!(F & FZ)) {
    CYCT(b_+130, b_+131); ret_effect(gb); return;
  }
  CYC(b_+130, b_+131);
  CALL_C(b_+131, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+134);
  CYC(b_+134, b_+137); push_effect(gb, b_+137);
  boy2_initialize_script(gb, sp0_);
  CYC(b_+137, b_+138); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+138, b_+139); ret_effect(gb);
  return;

substate2:
  CYC(b_+139, b_+142); SET_HL((SYM(goronDance_sequenceData__platinum) + 20));
  CYC(b_+142, b_+144); E = 0x08;
  CYC(b_+144, b_+147); interBankCall_hook(gb);
  return;

subid3:
  CALL_C(b_+147, checkInteractionState_hook, SYM(checkInteractionState), b_+150);
  if (F & FZ) {
    CYCT(b_+150, b_+152); goto subid3_state0;
  }
  CYC(b_+150, b_+152);
  CYC(b_+152, b_+155); SET_HL((SYM(goronDance_sequenceData__gold) + 14));
  CYC(b_+155, b_+157); E = 0x08;
  CYC(b_+157, b_+160); interBankCall_hook(gb);
  return;

subid3_state0:
  CYC(b_+160, b_+163); push_effect(gb, b_+163);
  boy2_init_graphics_and_inc_state(gb, sp0_);
  CYC(b_+163, b_+165); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+165, b_+167); mem_wr(gb, HL, 0x78);
  CYC(b_+167, b_+169); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x02);
  CYC(b_+171, b_+174); TAIL(objectSetVisiblec1);
}
