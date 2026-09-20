#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t subrosian_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void subrosian_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void subrosian_subid00_hook(GB *gb);
void subrosian_subid01_hook(GB *gb);
void subrosian_subid02_hook(GB *gb);
void subrosian_subid03_hook(GB *gb);
void subrosian_subid04_hook(GB *gb);
void subrosian_initGraphicsAndIncState_hook(GB *gb);
void subrosian_unused_63ec_hook(GB *gb);
void subrosian_initSubid02_hook(GB *gb);
void subrosian_loadScript_hook(GB *gb);
void subrosian_loadScriptIndex_hook(GB *gb);
void subrosian_getScriptPtr_hook(GB *gb);

void interactionCode4e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6354, 0x6356); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6356, 0x6357); A = mem_rd(gb, DE);
  CYC(0x6357, 0x6358); push_effect(gb, 0x6358);
  switch (subrosian_jump_table(gb)) {
    case 0x6362: subrosian_subid00_hook(gb); return;
    case 0x639c: subrosian_subid01_hook(gb); return;
    case 0x63a4: subrosian_subid02_hook(gb); return;
    case 0x63b8: subrosian_subid03_hook(gb); return;
    case 0x63c4: subrosian_subid04_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void subrosian_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6362, checkInteractionState_hook, 0x23fe, 0x6365);
  if (!(F & FZ)) { CYCT(0x6365, 0x6367); goto state1; }
  CYC(0x6365, 0x6367);
  CALL_C(0x6367, interactionIncState_hook, 0x23e0, 0x636a);
  CALL_C(0x636a, interactionInitGraphics_hook, 0x15fb, 0x636d);
  CALL_C(0x636d, objectSetVisiblec2_hook, 0x1e45, 0x6370);
  CYC(0x6370, 0x6372); A = 0x1c;
  CALL_C(0x6372, interactionSetHighTextIndex_hook, 0x253b, 0x6375);
  CALL_C(0x6375, checkIsLinkedGame_hook, 0x1992, 0x6378);
  if (F & FZ) { CYCT(0x6378, 0x637b); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(0x6378, 0x637b);
  CYC(0x637b, 0x637e); SET_HL(0x5559);
  CYC(0x637e, 0x6380); E = 0x09;
  CALL_C(0x6380, interBankCall_hook, 0x008a, 0x6383);
  CYC(0x6383, 0x6384); A = B;
  CYC(0x6384, 0x6386); alu_cp(gb, 5);
  CYC(0x6386, 0x6389); SET_HL(0x64c4);
  if (F & FZ) { CYCT(0x6389, 0x638b); goto set_script; }
  CYC(0x6389, 0x638b);
  CYC(0x638b, 0x638d); alu_cp(gb, 7);
  if (!(F & FZ)) { CYCT(0x638d, 0x6390); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(0x638d, 0x6390);
  CYC(0x6390, 0x6393); SET_HL(0x64c6);
set_script:
  CALL_C(0x6393, interactionSetScript_hook, 0x2544, 0x6396);
state1:
  CALL_C(0x6396, interactionRunScript_hook, 0x2552, 0x6399);
  CYC(0x6399, 0x639c); npcFaceLinkAndAnimate_hook(gb);
}

void subrosian_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x639c, 0x639f); SET_HL(0x776b);
  CYC(0x639f, 0x63a1); E = 9;
  CYC(0x63a1, 0x63a4); interBankCall_hook(gb);
}

void subrosian_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63a4, checkInteractionState_hook, 0x23fe, 0x63a7);
  if (!(F & FZ)) { CYCT(0x63a7, 0x63a9); goto state1; }
  CYC(0x63a7, 0x63a9);
  CALL_C(0x63a9, subrosian_initSubid02_hook, 0x63f4, 0x63ac);
  CALL_C(0x63ac, interactionRunScript_hook, 0x2552, 0x63af);
state1:
  CALL_C(0x63af, interactionRunScript_hook, 0x2552, 0x63b2);
  if (F & FC) { CYCT(0x63b2, 0x63b5); interactionDelete_hook(gb); return; }
  CYC(0x63b2, 0x63b5);
  CYC(0x63b5, 0x63b8); npcFaceLinkAndAnimate_hook(gb);
}

void subrosian_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63b8, checkInteractionState_hook, 0x23fe, 0x63bb);
  if (!(F & FZ)) { CYCT(0x63bb, 0x63bd); goto state1; }
  CYC(0x63bb, 0x63bd);
  CALL_C(0x63bd, subrosian_initGraphicsAndIncState_hook, 0x63e3, 0x63c0);
  CYC(0x63c0, 0x63c2); A = 2;
  CYC(0x63c2, 0x63c4);
  CYC(0x63ce, 0x63d0); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x63d0, 0x63d1); mem_wr(gb, DE, A);
  CYC(0x63d1, 0x63d4); SET_HL(0x7ed9);
  CALL_C(0x63d4, interactionSetScript_hook, 0x2544, 0x63d7);
  CALL_C(0x63d7, interactionRunScript_hook, 0x2552, 0x63da);
state1:
  CALL_C(0x63da, interactionRunScript_hook, 0x2552, 0x63dd);
  if (F & FC) { CYCT(0x63dd, 0x63e0); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(0x63dd, 0x63e0);
  CYC(0x63e0, 0x63e3); npcFaceLinkAndAnimate_hook(gb);
}

void subrosian_subid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63c4, checkInteractionState_hook, 0x23fe, 0x63c7);
  if (!(F & FZ)) { CYCT(0x63c7, 0x63c9); goto state1; }
  CYC(0x63c7, 0x63c9);
  CALL_C(0x63c9, subrosian_initGraphicsAndIncState_hook, 0x63e3, 0x63cc);
  CYC(0x63cc, 0x63ce); A = 4;
  CYC(0x63ce, 0x63d0); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x63d0, 0x63d1); mem_wr(gb, DE, A);
  CYC(0x63d1, 0x63d4); SET_HL(0x7ed9);
  CALL_C(0x63d4, interactionSetScript_hook, 0x2544, 0x63d7);
  CALL_C(0x63d7, interactionRunScript_hook, 0x2552, 0x63da);
state1:
  CALL_C(0x63da, interactionRunScript_hook, 0x2552, 0x63dd);
  if (F & FC) { CYCT(0x63dd, 0x63e0); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(0x63dd, 0x63e0);
  CYC(0x63e0, 0x63e3); npcFaceLinkAndAnimate_hook(gb);
}

void subrosian_initGraphicsAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63e3, interactionInitGraphics_hook, 0x15fb, 0x63e6);
  CALL_C(0x63e6, objectMarkSolidPosition_hook, 0x24f0, 0x63e9);
  CYC(0x63e9, 0x63ec); interactionIncState_hook(gb);
}

void subrosian_unused_63ec_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63ec, interactionInitGraphics_hook, 0x15fb, 0x63ef);
  CALL_C(0x63ef, objectMarkSolidPosition_hook, 0x24f0, 0x63f2);
  CYC(0x63f2, 0x63f4); subrosian_loadScript_hook(gb);
}

void subrosian_initSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63f4, interactionInitGraphics_hook, 0x15fb, 0x63f7);
  CALL_C(0x63f7, objectMarkSolidPosition_hook, 0x24f0, 0x63fa);
  CYC(0x63fa, 0x63fc); subrosian_loadScriptIndex_hook(gb);
}

void subrosian_loadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63fc, subrosian_getScriptPtr_hook, 0x6414, 0x63ff);
  CALL_C(0x63ff, interactionSetScript_hook, 0x2544, 0x6402);
  CYC(0x6402, 0x6405); interactionIncState_hook(gb);
}

void subrosian_loadScriptIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6405, subrosian_getScriptPtr_hook, 0x6414, 0x6408);
  CYC(0x6408, 0x6409); E = alu_inc8(gb, E);
  CYC(0x6409, 0x640a); A = mem_rd(gb, DE);
  CYC(0x640a, 0x640b); subrosian_add_double_index(gb, 0x640b);
  CYC(0x640b, 0x640c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x640c, 0x640d); H = mem_rd(gb, HL);
  CYC(0x640d, 0x640e); L = A;
  CALL_C(0x640e, interactionSetScript_hook, 0x2544, 0x6411);
  CYC(0x6411, 0x6414); interactionIncState_hook(gb);
}

void subrosian_getScriptPtr_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6414, 0x6416); A = 0x1c;
  CALL_C(0x6416, interactionSetHighTextIndex_hook, 0x253b, 0x6419);
  CYC(0x6419, 0x641b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x641b, 0x641c); A = mem_rd(gb, DE);
  CYC(0x641c, 0x641f); SET_HL(0x6424);
  CYC(0x641f, 0x6420); subrosian_add_double_index(gb, 0x6420);
  CYC(0x6420, 0x6421); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6421, 0x6422); H = mem_rd(gb, HL);
  CYC(0x6422, 0x6423); L = A;
  CYC(0x6423, 0x6424); ret_effect(gb);
}
