#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodea1_jump_table(GB *gb) {
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

static void interactionCodea1_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:58c4, bare-global (no leading @ in the .s source). Reached by the last entry of
// interactionCodea1@updateSubid's jump table (see interactionCodea1_hook below).
void movingPlatform_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x58c4, interactionDecCounter1_hook, 0x23cc, 0x58c7);
  if (!(F & FZ)) { CYCT(0x58c7, 0x58c8); ret_effect(gb); return; } // ret nz
  CYC(0x58c7, 0x58c8);
  CYC(0x58c8, 0x58cb); sidescrollPlatformFunc_5bfc_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_MOVING_SIDESCROLL_PLATFORM
// ==================================================================================================
// interactionCodea1@updateSubid is inlined below (single caller, "callers: 1"): it's entered by a
// genuine `call` from this routine's own body, and every one of its branches either performs a
// real "ret" (movingPlatform_stateC's "ret nz") or tail-jumps into an already-hooked leaf whose own
// ret consumes the very same manually-pushed return address (0x581d) -- and this routine still has
// more work to do afterward ("jp sidescrollingPlatformCommon"), so the resume is verified exactly
// like CALL_C does, at the single shared `afterUpdateSubid` label (same idiom as
// src/game/decoration.c's @isSymmetryCityRoom).
void interactionCodea1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5817, sidescrollPlatform_checkLinkOnPlatform_hook, 0x5b7f, 0x581a);
  CYC(0x581a, 0x581d); push_effect(gb, 0x581d);

  CYC(0x5820, 0x5822); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5822, 0x5823); A = mem_rd(gb, DE);
  CYC(0x5823, 0x5825); alu_sub(gb, 0x08);
  if (F & FC) { CYCT(0x5825, 0x5827); goto state0To7; } // jr c
  CYC(0x5825, 0x5827);
  CYC(0x5827, 0x5828); push_effect(gb, 0x5828);
  switch (interactionCodea1_jump_table(gb)) {
    case 0x585c: goto state8;
    case 0x586d: goto state9;
    case 0x588a: goto stateA;
    case 0x58a7: goto stateB;
    case 0x58c4: movingPlatform_stateC_hook(gb); goto afterUpdateSubid;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0To7:
  CYC(0x5832, 0x5835); SET_HL(0x7eaf); // bank0e.movingSidescrollPlatformScriptTable
  CALL_C(0x5835, objectLoadMovementScript_hook, 0x3035, 0x5838);
  CALL_C(0x5838, interactionInitGraphics_hook, 0x15fb, 0x583b);
  CYC(0x583b, 0x583d); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x583d, 0x583e); A = mem_rd(gb, DE);
  CYC(0x583e, 0x5841); SET_HL(0x5852); // @collisionRadii
  CYC(0x5841, 0x5842); interactionCodea1_addDoubleIndex(gb, 0x5842);
  CYC(0x5842, 0x5844); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5844, 0x5845); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5845, 0x5846); mem_wr(gb, DE, A);
  CYC(0x5846, 0x5847); E = alu_inc8(gb, E);
  CYC(0x5847, 0x5848); A = mem_rd(gb, HL);
  CYC(0x5848, 0x5849); mem_wr(gb, DE, A);
  CYC(0x5849, 0x584b); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x584b, 0x584c); A = mem_rd(gb, DE);
  CALL_C(0x584c, interactionSetAnimation_hook, 0x262e, 0x584f);
  CYC(0x584f, 0x5852); objectSetVisible82_hook(gb); goto afterUpdateSubid; // jp

state8:
  CYC(0x585c, 0x585e); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x585e, 0x585f); A = mem_rd(gb, DE);
  CYC(0x585f, 0x5860); H = D;
  CYC(0x5860, 0x5862); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5862, 0x5863); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x5863, 0x5865); goto l_5868; } // jr nc
  CYC(0x5863, 0x5865);
  CYC(0x5865, 0x5868); objectApplySpeed_hook(gb); goto afterUpdateSubid; // jp

l_5868:
  CYC(0x5868, 0x5869); A = mem_rd(gb, DE);
  CYC(0x5869, 0x586a); mem_wr(gb, HL, A);
  CYC(0x586a, 0x586d); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateSubid; // jp

state9:
  CYC(0x586d, 0x586f); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x586f, 0x5870); A = mem_rd(gb, DE);
  CYC(0x5870, 0x5871); H = D;
  CYC(0x5871, 0x5873); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x5873, 0x5874); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x5874, 0x5876); goto l_5885; } // jr nc
  CYC(0x5874, 0x5876);
  CYC(0x5876, 0x5878); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5878, 0x5879); B = mem_rd(gb, HL);
  CYC(0x5879, 0x587b); C = 0x08; // ANGLE_RIGHT
  CYC(0x587b, 0x587e); A = mem_rd(gb, wLinkRidingObject);
  CYC(0x587e, 0x587f); alu_cp(gb, D);
  if (F & FZ) { CALL_C_CC(0x587f, updateLinkPositionGivenVelocity_hook, 0x231e, 0x5882); } else { CYC(0x587f, 0x5882); } // call z
  CYC(0x5882, 0x5885); objectApplySpeed_hook(gb); goto afterUpdateSubid; // jp

l_5885:
  CYC(0x5885, 0x5886); A = mem_rd(gb, HL);
  CYC(0x5886, 0x5887); mem_wr(gb, DE, A);
  CYC(0x5887, 0x588a); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateSubid; // jp

stateA:
  CYC(0x588a, 0x588c); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x588c, 0x588d); A = mem_rd(gb, DE);
  CYC(0x588d, 0x588e); H = D;
  CYC(0x588e, 0x5890); L = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x5890, 0x5891); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x5891, 0x5893); goto l_58a2; } // jr nc
  CYC(0x5891, 0x5893);
  CYC(0x5893, 0x5895); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5895, 0x5896); B = mem_rd(gb, HL);
  CYC(0x5896, 0x5898); C = 0x10; // ANGLE_DOWN
  CYC(0x5898, 0x589b); A = mem_rd(gb, wLinkRidingObject);
  CYC(0x589b, 0x589c); alu_cp(gb, D);
  if (F & FZ) { CALL_C_CC(0x589c, updateLinkPositionGivenVelocity_hook, 0x231e, 0x589f); } else { CYC(0x589c, 0x589f); } // call z
  CYC(0x589f, 0x58a2); objectApplySpeed_hook(gb); goto afterUpdateSubid; // jp

l_58a2:
  CYC(0x58a2, 0x58a3); A = mem_rd(gb, HL);
  CYC(0x58a3, 0x58a4); mem_wr(gb, DE, A);
  CYC(0x58a4, 0x58a7); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateSubid; // jp

stateB:
  CYC(0x58a7, 0x58a9); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x58a9, 0x58aa); A = mem_rd(gb, DE);
  CYC(0x58aa, 0x58ab); H = D;
  CYC(0x58ab, 0x58ad); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x58ad, 0x58ae); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x58ae, 0x58b0); goto l_58bf; } // jr nc
  CYC(0x58ae, 0x58b0);
  CYC(0x58b0, 0x58b2); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x58b2, 0x58b3); B = mem_rd(gb, HL);
  CYC(0x58b3, 0x58b5); C = 0x18; // ANGLE_LEFT
  CYC(0x58b5, 0x58b8); A = mem_rd(gb, wLinkRidingObject);
  CYC(0x58b8, 0x58b9); alu_cp(gb, D);
  if (F & FZ) { CALL_C_CC(0x58b9, updateLinkPositionGivenVelocity_hook, 0x231e, 0x58bc); } else { CYC(0x58b9, 0x58bc); } // call z
  CYC(0x58bc, 0x58bf); objectApplySpeed_hook(gb); goto afterUpdateSubid; // jp

l_58bf:
  CYC(0x58bf, 0x58c0); A = mem_rd(gb, DE);
  CYC(0x58c0, 0x58c1); mem_wr(gb, HL, A);
  CYC(0x58c1, 0x58c4); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateSubid; // jp

afterUpdateSubid:
  if (!(gb->pc == 0x581d && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(0x581d, 0x5820); sidescrollingPlatformCommon_hook(gb); return; // jp
}

// 0b:7fa1, bare-global. Genuinely dead code -- "Garbage function here (partial repeat of the
// above function)" per ref/oracles-disasm/code/ages/garbage/bank0bEnd.s -- with zero callers and
// call/jump targets (0x258f, 0x3b5c) that land mid-instruction inside unrelated routines, so
// neither has a known _hook. Ported byte-for-byte via the raw interpreter primitives.
void func_7fa1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7fa1, 0x7fa4); push_effect(gb, 0x7fa4); asm_call(gb, 0x258f, 0x7fa4);
  if (!(F & FC)) { CYCT(0x7fa4, 0x7fa5); ret_effect(gb); return; } // ret nc
  CYC(0x7fa4, 0x7fa5);
  CYC(0x7fa5, 0x7fa8); hook_continue(gb, 0x3b5c, sp0_); return; // jp
}
