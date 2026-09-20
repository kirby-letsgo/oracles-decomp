#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodeaa);
  CYC(b_+60, b_+62); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  {
    CYC(b_+63, b_+64); push_effect(gb, b_+64);
    uint16_t target = interactionCodeaa_jump_table(gb);
    if (target == b_+98) goto substate1;
    if (target == b_+118) goto substate2;
    if (target == b_+139) goto substate3;
    if (target == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; } // substate list's 5th entry jumps straight into interactionAnimate
    // target == 0x5ffa falls through to substate0
  }

  // interactionCodeaa@substate0
  CALL_C(b_+74, interactionAnimate_hook, SYM(interactionAnimate), b_+77);
  CYC(b_+77, b_+80); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+80, b_+82); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+82, b_+83); ret_effect(gb); return; } // ret nz
  CYC(b_+82, b_+83);
  CALL_C(b_+83, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+86);
  CYC(b_+86, b_+88); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+88, b_+90); mem_wr(gb, HL, 120);
  CYC(b_+90, b_+92); A = 0x05;
  CALL_C(b_+92, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+95);
  CYC(b_+95, b_+98); goto beginJump; // jp

substate1: // interactionCodeaa@substate1
  CALL_C(b_+98, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+101);
  if (!(F & FZ)) { CYCT(b_+101, b_+104); goto updateSpeedZ; } // jp nz
  CYC(b_+101, b_+104);
  CALL_C(b_+104, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+107);
  CYC(b_+107, b_+108); alu_xor(gb, A);
  CYC(b_+108, b_+110); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+110, b_+111); mem_wr(gb, HL, A);
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+113, b_+115); mem_wr(gb, HL, 30);
  CYC(b_+115, b_+118); interactionAnimate_hook(gb); return; // jp

substate2: // interactionCodeaa@substate2
  CALL_C(b_+118, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+121);
  if (!(F & FZ)) { CYCT(b_+121, b_+123); goto l6038; } // jr nz
  CYC(b_+121, b_+123);
  CALL_C(b_+123, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+126);
  CYC(b_+126, b_+128); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+128, b_+130); mem_wr(gb, HL, 60);
  CYC(b_+130, b_+133); SET_BC(0x3d09); // TX_3d09
  CALL_C(b_+133, showText_hook, SYM(showText), b_+136);

l6038: // L_6038
  CYC(b_+136, b_+139); interactionAnimate_hook(gb); return; // jp

substate3: // interactionCodeaa@substate3
  CALL_C(b_+139, interactionDecCounter1IfTextNotActive_hook, SYM(interactionDecCounter1IfTextNotActive), b_+142);
  if (!(F & FZ)) { CYCT(b_+142, b_+144); goto l6048; } // jr nz
  CYC(b_+142, b_+144);
  CALL_C(b_+144, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+147);
  CYC(b_+147, b_+150); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0x05);

l6048: // L_6048
  CYC(b_+152, b_+155); interactionAnimate_hook(gb); return; // jp

updateSpeedZ: // interactionCodeaa@updateSpeedZ
  CYC(b_+170, b_+172); C = 0x20;
  CALL_C(b_+172, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+175);
  if (!(F & FZ)) { CYCT(b_+175, b_+176); ret_effect(gb); return; } // ret nz
  CYC(b_+175, b_+176);
  CYC(b_+176, b_+177); H = D;

beginJump: // interactionCodeaa@beginJump
  CYC(b_+177, b_+180); SET_BC(0xff00); // -$100
  CYC(b_+180, b_+183); objectSetSpeedZ_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_DIN
// ==================================================================================================
void interactionCodeaa_hook(GB *gb) {
  BASE(interactionCodeaa);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodeaa_jump_table(gb);
    if (target == b_+37) goto state1;
    // target == 0x5fb8 falls through to state0
  }

  // interactionCodeaa@state0
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14); // SWITCHES THREADS
  CALL_C(b_+14, objectSetVisible82_hook, SYM(objectSetVisible82), b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  {
    CYC(b_+20, b_+21); push_effect(gb, b_+21);
    uint16_t target = interactionCodeaa_jump_table(gb);
    if (target == b_+28) goto initSubid2;
    // target == 0x5fcb falls through to initSubid0/initSubid1 (aliased)
  }

  // interactionCodeaa@initSubid0 / @initSubid1
  CYC(b_+27, b_+28); ret_effect(gb); return; // ret

initSubid2: // interactionCodeaa@initSubid2
  CALL_C(b_+28, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+31);
  CYC(b_+31, b_+34); SET_BC((SYM(interactionCodeb6__frame1) + 9));
  CYC(b_+34, b_+37); interactionSetPosition_hook(gb); return; // jp

state1: // interactionCodeaa@state1
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  {
    CYC(b_+40, b_+41); push_effect(gb, b_+41);
    uint16_t target = interactionCodeaa_jump_table(gb);
    if (target == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; } // state list's 2nd/3rd entries jump straight into interactionAnimate
    // target == 0x5fdf falls through to runSubid0
  }

  // interactionCodeaa@runSubid0
  CYC(b_+47, b_+50); push_effect(gb, b_+50); interactionCodeaa_runSubid0Substates(gb, sp0_); // call
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+54, b_+57); objectSetVisiblec2_hook(gb); return; } // jp nz
  CYC(b_+54, b_+57);
  CYC(b_+57, b_+60); objectSetVisible82_hook(gb); return; // jp
}
