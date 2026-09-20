#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/din.s (interactionCodeaa / INTERAC_DIN), bank
// 0x0b.

static uint16_t interactionCodeaa_jump_table(GB *gb) {
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

// interactionCodeaa@runSubid0Substates (0b:5fec), plus its internal @substate0-3, @updateSpeedZ
// and @beginJump: only reached via the single `call` at 0b:5fdf inside interactionCodeaa@runSubid0.
// The caller pushes the real return address (0b:5fe2) before invoking this as a plain C function
// call. Two paths (@substate0's "ret nz" and @updateSpeedZ's "ret nz") are real ret_effect pops of
// that exact value; every other path tail-jumps into an already-hooked leaf (interactionAnimate,
// showText+fallthrough, objectSetSpeedZ) whose own ret_effect consumes it instead. Either way the
// stack ends up balanced with no resume check needed.
//
// interactionCodeaa@loadScript (0b:604b) and @scriptTable (0b:6058): dead code, `callers: 0` in
// the readiness report ("Scripts unused?" per the disassembly comment) -- not ported.
static void interactionCodeaa_runSubid0Substates(GB *gb, uint16_t sp0_) {
  CYC(0x5fec, 0x5fee); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5fee, 0x5fef); A = mem_rd(gb, DE);
  {
    CYC(0x5fef, 0x5ff0); push_effect(gb, 0x5ff0);
    uint16_t target = interactionCodeaa_jump_table(gb);
    if (target == 0x6012) goto substate1;
    if (target == 0x6026) goto substate2;
    if (target == 0x603b) goto substate3;
    if (target == 0x261b) { interactionAnimate_hook(gb); return; } // substate list's 5th entry jumps straight into interactionAnimate
    // target == 0x5ffa falls through to substate0
  }

  // interactionCodeaa@substate0
  CALL_C(0x5ffa, interactionAnimate_hook, 0x261b, 0x5ffd);
  CYC(0x5ffd, 0x6000); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x6000, 0x6002); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x6002, 0x6003); ret_effect(gb); return; } // ret nz
  CYC(0x6002, 0x6003);
  CALL_C(0x6003, interactionIncSubstate_hook, 0x23e5, 0x6006);
  CYC(0x6006, 0x6008); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6008, 0x600a); mem_wr(gb, HL, 120);
  CYC(0x600a, 0x600c); A = 0x05;
  CALL_C(0x600c, interactionSetAnimation_hook, 0x262e, 0x600f);
  CYC(0x600f, 0x6012); goto beginJump; // jp

substate1: // interactionCodeaa@substate1
  CALL_C(0x6012, interactionDecCounter1_hook, 0x23cc, 0x6015);
  if (!(F & FZ)) { CYCT(0x6015, 0x6018); goto updateSpeedZ; } // jp nz
  CYC(0x6015, 0x6018);
  CALL_C(0x6018, interactionIncSubstate_hook, 0x23e5, 0x601b);
  CYC(0x601b, 0x601c); alu_xor(gb, A);
  CYC(0x601c, 0x601e); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x601e, 0x601f); mem_wr(gb, HL, A);
  CYC(0x601f, 0x6021); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6021, 0x6023); mem_wr(gb, HL, 30);
  CYC(0x6023, 0x6026); interactionAnimate_hook(gb); return; // jp

substate2: // interactionCodeaa@substate2
  CALL_C(0x6026, interactionDecCounter1_hook, 0x23cc, 0x6029);
  if (!(F & FZ)) { CYCT(0x6029, 0x602b); goto l6038; } // jr nz
  CYC(0x6029, 0x602b);
  CALL_C(0x602b, interactionIncSubstate_hook, 0x23e5, 0x602e);
  CYC(0x602e, 0x6030); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6030, 0x6032); mem_wr(gb, HL, 60);
  CYC(0x6032, 0x6035); SET_BC(0x3d09); // TX_3d09
  CALL_C(0x6035, showText_hook, 0x1872, 0x6038);

l6038: // L_6038
  CYC(0x6038, 0x603b); interactionAnimate_hook(gb); return; // jp

substate3: // interactionCodeaa@substate3
  CALL_C(0x603b, interactionDecCounter1IfTextNotActive_hook, 0x273c, 0x603e);
  if (!(F & FZ)) { CYCT(0x603e, 0x6040); goto l6048; } // jr nz
  CYC(0x603e, 0x6040);
  CALL_C(0x6040, interactionIncSubstate_hook, 0x23e5, 0x6043);
  CYC(0x6043, 0x6046); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x6046, 0x6048); mem_wr(gb, HL, 0x05);

l6048: // L_6048
  CYC(0x6048, 0x604b); interactionAnimate_hook(gb); return; // jp

updateSpeedZ: // interactionCodeaa@updateSpeedZ
  CYC(0x605a, 0x605c); C = 0x20;
  CALL_C(0x605c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x605f);
  if (!(F & FZ)) { CYCT(0x605f, 0x6060); ret_effect(gb); return; } // ret nz
  CYC(0x605f, 0x6060);
  CYC(0x6060, 0x6061); H = D;

beginJump: // interactionCodeaa@beginJump
  CYC(0x6061, 0x6064); SET_BC(0xff00); // -$100
  CYC(0x6064, 0x6067); objectSetSpeedZ_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_DIN
// ==================================================================================================
void interactionCodeaa_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5fb0, 0x5fb2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5fb2, 0x5fb3); A = mem_rd(gb, DE);
  {
    CYC(0x5fb3, 0x5fb4); push_effect(gb, 0x5fb4);
    uint16_t target = interactionCodeaa_jump_table(gb);
    if (target == 0x5fd5) goto state1;
    // target == 0x5fb8 falls through to state0
  }

  // interactionCodeaa@state0
  CYC(0x5fb8, 0x5fba); A = 0x01;
  CYC(0x5fba, 0x5fbb); mem_wr(gb, DE, A); // [state]
  CALL_C(0x5fbb, interactionInitGraphics_hook, 0x15fb, 0x5fbe); // SWITCHES THREADS
  CALL_C(0x5fbe, objectSetVisible82_hook, 0x1e69, 0x5fc1);
  CYC(0x5fc1, 0x5fc3); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5fc3, 0x5fc4); A = mem_rd(gb, DE);
  {
    CYC(0x5fc4, 0x5fc5); push_effect(gb, 0x5fc5);
    uint16_t target = interactionCodeaa_jump_table(gb);
    if (target == 0x5fcc) goto initSubid2;
    // target == 0x5fcb falls through to initSubid0/initSubid1 (aliased)
  }

  // interactionCodeaa@initSubid0 / @initSubid1
  CYC(0x5fcb, 0x5fcc); ret_effect(gb); return; // ret

initSubid2: // interactionCodeaa@initSubid2
  CALL_C(0x5fcc, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x5fcf);
  CYC(0x5fcf, 0x5fd2); SET_BC(0x4830);
  CYC(0x5fd2, 0x5fd5); interactionSetPosition_hook(gb); return; // jp

state1: // interactionCodeaa@state1
  CYC(0x5fd5, 0x5fd7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5fd7, 0x5fd8); A = mem_rd(gb, DE);
  {
    CYC(0x5fd8, 0x5fd9); push_effect(gb, 0x5fd9);
    uint16_t target = interactionCodeaa_jump_table(gb);
    if (target == 0x261b) { interactionAnimate_hook(gb); return; } // state list's 2nd/3rd entries jump straight into interactionAnimate
    // target == 0x5fdf falls through to runSubid0
  }

  // interactionCodeaa@runSubid0
  CYC(0x5fdf, 0x5fe2); push_effect(gb, 0x5fe2); interactionCodeaa_runSubid0Substates(gb, sp0_); // call
  CYC(0x5fe2, 0x5fe4); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x5fe4, 0x5fe5); A = mem_rd(gb, DE);
  CYC(0x5fe5, 0x5fe6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5fe6, 0x5fe9); objectSetVisiblec2_hook(gb); return; } // jp nz
  CYC(0x5fe6, 0x5fe9);
  CYC(0x5fe9, 0x5fec); objectSetVisible82_hook(gb); return; // jp
}
