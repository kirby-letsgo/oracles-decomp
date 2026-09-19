#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// Bare-global, implemented in src/game/movingSidescrollPlatform.c; shared by both interactions'
// updateState jump tables (both @-locals dispatch their last entry into it identically).
void movingPlatform_stateC_hook(GB *gb);

static uint16_t interactionCodea2_jump_table(GB *gb) {
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

static void interactionCodea2_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_MOVING_SIDESCROLL_CONVEYOR
// ==================================================================================================
// interactionCodea2@updateState is inlined below (single caller): entered by a genuine `call`
// from this routine's own body, its branches all funnel into @applySpeed (a real "ret nz" or a
// tail `jp updateLinkPositionGivenVelocity` whose own ret consumes the same manually-pushed
// return address), or into the shared movingPlatform_stateC_hook -- and this routine still has
// "jp sidescrollingPlatformCommon" to run afterward, so the resume is verified like CALL_C at the
// shared `afterUpdateState` label (same idiom as movingSidescrollPlatform.c's interactionCodea1).
void interactionCodea2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x58cb, interactionAnimate_hook, 0x261b, 0x58ce);
  CALL_C(0x58ce, sidescrollPlatform_checkLinkOnPlatform_hook, 0x5b7f, 0x58d1);
  if (!(F & FZ)) { CALL_C_CC(0x58d1, sidescrollPlatform_updateLinkKnockbackForConveyor_hook, 0x5bb4, 0x58d4); } else { CYC(0x58d1, 0x58d4); } // call nz
  CYC(0x58d4, 0x58d7); push_effect(gb, 0x58d7);

  CYC(0x58da, 0x58dc); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x58dc, 0x58dd); A = mem_rd(gb, DE);
  CYC(0x58dd, 0x58df); alu_sub(gb, 0x08);
  if (F & FC) { CYCT(0x58df, 0x58e1); goto state0To7; } // jr c
  CYC(0x58df, 0x58e1);
  CYC(0x58e1, 0x58e2); push_effect(gb, 0x58e2);
  switch (interactionCodea2_jump_table(gb)) {
    case 0x5906: goto state8;
    case 0x5914: goto state9;
    case 0x5922: goto stateA;
    case 0x593e: goto stateB;
    case 0x58c4: movingPlatform_stateC_hook(gb); goto afterUpdateState;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0To7:
  CYC(0x58ec, 0x58ef); SET_HL(0x7f0b); // bank0e.movingSidescrollConveyorScriptTable
  CALL_C(0x58ef, objectLoadMovementScript_hook, 0x3035, 0x58f2);
  CALL_C(0x58f2, interactionInitGraphics_hook, 0x15fb, 0x58f5);
  CYC(0x58f5, 0x58f6); H = D;
  CYC(0x58f6, 0x58f8); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x58f8, 0x58fa); mem_wr(gb, HL, 0x08);
  CYC(0x58fa, 0x58fb); L = alu_inc8(gb, L);
  CYC(0x58fb, 0x58fd); mem_wr(gb, HL, 0x0c);
  CYC(0x58fd, 0x58ff); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x58ff, 0x5900); A = mem_rd(gb, DE);
  CALL_C(0x5900, interactionSetAnimation_hook, 0x262e, 0x5903);
  CYC(0x5903, 0x5906); objectSetVisible82_hook(gb); goto afterUpdateState; // jp

state8:
  CYC(0x5906, 0x5908); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x5908, 0x5909); A = mem_rd(gb, DE);
  CYC(0x5909, 0x590a); H = D;
  CYC(0x590a, 0x590c); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x590c, 0x590d); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x590d, 0x590f); goto applySpeed; } // jr c
  CYC(0x590d, 0x590f);
  CYC(0x590f, 0x5910); A = mem_rd(gb, DE);
  CYC(0x5910, 0x5911); mem_wr(gb, HL, A);
  CYC(0x5911, 0x5914); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateState; // jp

state9:
  CYC(0x5914, 0x5916); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5916, 0x5917); A = mem_rd(gb, DE);
  CYC(0x5917, 0x5918); H = D;
  CYC(0x5918, 0x591a); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x591a, 0x591b); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x591b, 0x591d); goto applySpeed; } // jr c
  CYC(0x591b, 0x591d);
  CYC(0x591d, 0x591e); A = mem_rd(gb, HL);
  CYC(0x591e, 0x591f); mem_wr(gb, DE, A);
  CYC(0x591f, 0x5922); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateState; // jp

stateA:
  CYC(0x5922, 0x5924); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5924, 0x5925); A = mem_rd(gb, DE);
  CYC(0x5925, 0x5926); H = D;
  CYC(0x5926, 0x5928); L = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x5928, 0x5929); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x5929, 0x592b); goto l_5939; } // jr nc
  CYC(0x5929, 0x592b);
  CYC(0x592b, 0x592d); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x592d, 0x592e); B = mem_rd(gb, HL);
  CYC(0x592e, 0x5930); C = 0x10; // ANGLE_DOWN
  CYC(0x5930, 0x5933); A = mem_rd(gb, wLinkRidingObject);
  CYC(0x5933, 0x5934); alu_cp(gb, D);
  if (F & FZ) { CALL_C_CC(0x5934, updateLinkPositionGivenVelocity_hook, 0x231e, 0x5937); } else { CYC(0x5934, 0x5937); } // call z
  CYCT(0x5937, 0x5939); goto applySpeed; // jr

l_5939:
  CYC(0x5939, 0x593a); A = mem_rd(gb, HL);
  CYC(0x593a, 0x593b); mem_wr(gb, DE, A);
  CYC(0x593b, 0x593e); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateState; // jp

stateB:
  CYC(0x593e, 0x5940); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x5940, 0x5941); A = mem_rd(gb, DE);
  CYC(0x5941, 0x5942); H = D;
  CYC(0x5942, 0x5944); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5944, 0x5945); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x5945, 0x5947); goto applySpeed; } // jr c
  CYC(0x5945, 0x5947);
  CYC(0x5947, 0x5948); A = mem_rd(gb, DE);
  CYC(0x5948, 0x5949); mem_wr(gb, HL, A);
  CYC(0x5949, 0x594c); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateState; // jp

applySpeed:
  CALL_C(0x594c, objectApplySpeed_hook, 0x201d, 0x594f);
  CYC(0x594f, 0x5952); A = mem_rd(gb, wLinkRidingObject);
  CYC(0x5952, 0x5953); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(0x5953, 0x5954); ret_effect(gb); return; } // ret nz
  CYC(0x5953, 0x5954);
  CYC(0x5954, 0x5956); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5956, 0x5957); A = mem_rd(gb, DE);
  CYC(0x5957, 0x5958); alu_rrca(gb);
  CYC(0x5958, 0x5959); alu_rrca(gb);
  CYC(0x5959, 0x595a); B = A;
  CYC(0x595a, 0x595c); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x595c, 0x595d); A = mem_rd(gb, DE);
  CYC(0x595d, 0x595e); alu_add(gb, B);
  CYC(0x595e, 0x5961); SET_HL(0x5968); // @directions
  CYC(0x5961, 0x5962); interactionCodea2_addDoubleIndex(gb, 0x5962);
  CYC(0x5962, 0x5963); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5963, 0x5964); C = A;
  CYC(0x5964, 0x5965); B = mem_rd(gb, HL);
  CYC(0x5965, 0x5968); updateLinkPositionGivenVelocity_hook(gb); goto afterUpdateState; // jp

afterUpdateState:
  if (!(gb->pc == 0x58d7 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(0x58d7, 0x58da); sidescrollingPlatformCommon_hook(gb); return; // jp
}
