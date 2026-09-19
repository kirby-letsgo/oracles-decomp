#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCoded8_jump_table(GB *gb) {
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

// rst_addAToHl (rst $10): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCoded8_addAToHl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// rst_addDoubleIndex (rst $18): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCoded8_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCoded8@updateTile, 0b:7cce -- true local, called via a real `call` twice from
// @toggleLavaSource; always returns via its own ret.
void interactionCoded8_updateTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7cce, 0x7cd0); A = hram_rd(gb, 0x8d); // ldh a,($ff8d)
  CYC(0x7cd0, 0x7cd1); B = mem_rd(gb, HL);
  CYC(0x7cd1, 0x7cd2); alu_add(gb, B);
  CYC(0x7cd2, 0x7cd3); C = L;
  CYC(0x7cd3, 0x7cd4); push_effect(gb, HL);
  CALL_C(0x7cd4, setTile_hook, 0x3a9c, 0x7cd7);
  CYC(0x7cd7, 0x7cd8); SET_HL(pop_effect(gb));
  RET(0x7cd8); return;
}

// interactionCoded8@loadScriptForSubid, 0b:7cd9 -- true local, called via a real `call` from
// @state1 and @state3; never executes its own ret, tail-jumps into interactionSetMiniScript.
static void interactionCoded8_loadScriptForSubid(GB *gb) {
  CYC(0x7cd9, 0x7cdb); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7cdb, 0x7cdc); A = mem_rd(gb, DE);
  CYC(0x7cdc, 0x7cdf); SET_HL(0x7d4a); // @scriptTable
  CYC(0x7cdf, 0x7ce0); interactionCoded8_addDoubleIndex(gb, 0x7ce0);
  CYC(0x7ce0, 0x7ce1); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7ce1, 0x7ce2); H = mem_rd(gb, HL);
  CYC(0x7ce2, 0x7ce3); L = A;
  CYC(0x7ce3, 0x7ce6); interactionSetMiniScript_hook(gb); return; // jp
}

// interactionCoded8@toggleLavaSource, 0b:7c9e -- reached as a plain fallthrough from @state1 --
// no real call frame there, interactionCoded8_hook calls this as a tail and returns -- and by
// a genuine `call` from @state3, which expects a real return to continue with SND_DOORCLOSE.
// Implemented as its own function either way: a plain tail call from @state1 needs no resume
// check of its own, matching the trampoline pattern; @state3's CALL_C wraps it normally instead.
// The resume checks this function's own CALL_C calls need are keyed off its own sp0_, which is
// correct in both cases since a plain tail call pushes nothing extra onto the emulated stack.
void interactionCoded8_toggleLavaSource_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7c9e, 0x7ca0); B = 0x06;
  CYC(0x7ca0, 0x7ca2); A = 0xc3; // TILEINDEX_LAVA_SOURCE_UP_LEFT
  CALL_C(0x7ca2, findTileInRoom_hook, 0x15cc, 0x7ca5);
  if (F & FZ) { CYCT(0x7ca5, 0x7ca7); goto setOrUnsetLavaSource; } // jr z
  CYC(0x7ca5, 0x7ca7);
  CYC(0x7ca7, 0x7ca9); A = 0xc6; // TILEINDEX_LAVA_SOURCE_DOWN_LEFT
  CALL_C(0x7ca9, findTileInRoom_hook, 0x15cc, 0x7cac);
  if (F & FZ) { CYCT(0x7cac, 0x7cae); goto setOrUnsetLavaSource; } // jr z
  CYC(0x7cac, 0x7cae);
  CYC(0x7cae, 0x7cb0); B = 0xfa;
  CYC(0x7cb0, 0x7cb2); A = 0xc9; // TILEINDEX_LAVA_SOURCE_UP_LEFT_EMPTY
  CALL_C(0x7cb2, findTileInRoom_hook, 0x15cc, 0x7cb5);
  if (F & FZ) { CYCT(0x7cb5, 0x7cb7); goto setOrUnsetLavaSource; } // jr z
  CYC(0x7cb5, 0x7cb7);
  CYC(0x7cb7, 0x7cb9); A = 0xcc; // TILEINDEX_LAVA_SOURCE_DOWN_LEFT_EMPTY
  CALL_C(0x7cb9, findTileInRoom_hook, 0x15cc, 0x7cbc);

setOrUnsetLavaSource:
  CYC(0x7cbc, 0x7cbd); A = B;
  CYC(0x7cbd, 0x7cbf); hram_wr(gb, 0x8d, A); // ldh ($ff8d),a
  CALL_C(0x7cbf, interactionCoded8_updateTile_hook, 0x7cce, 0x7cc2);

tileLoop:
  CYC(0x7cc2, 0x7cc3); L = alu_inc8(gb, L);
  CYC(0x7cc3, 0x7cc4); A = mem_rd(gb, HL);
  CYC(0x7cc4, 0x7cc6); alu_sub(gb, 0xc3); // TILEINDEX_LAVA_SOURCE_UP_LEFT
  CYC(0x7cc6, 0x7cc8); alu_cp(gb, 0x0c);
  if (!(F & FC)) { CYCT(0x7cc8, 0x7cc9); ret_effect(gb); return; } // ret nc
  CYC(0x7cc8, 0x7cc9);
  CALL_C(0x7cc9, interactionCoded8_updateTile_hook, 0x7cce, 0x7ccc);
  CYC(0x7ccc, 0x7cce); goto tileLoop; // jr $7cc2
}

// ==================================================================================================
// INTERAC_LEVER_LAVA_FILLER
//
// Variables:
//   counter2: Number of frames between two lava tiles being filled. Effectively this sets the
//             "speed" of the lava filler (lower is faster).
// ==================================================================================================
void interactionCoded8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7c65, 0x7c67); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7c67, 0x7c68); A = mem_rd(gb, DE);
  CYC(0x7c68, 0x7c69); push_effect(gb, 0x7c69);
  switch (interactionCoded8_jump_table(gb)) {
    case 0x7c73: goto state0;
    case 0x7c89: goto state1;
    case 0x7ce6: goto state2;
    case 0x7d07: goto state3;
    case 0x7d1a: goto state4;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x7c73, 0x7c75); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7c75, 0x7c76); A = mem_rd(gb, DE);
  CYC(0x7c76, 0x7c79); SET_HL(0x7c81); // @counter2Vals
  CYC(0x7c79, 0x7c7a); interactionCoded8_addAToHl(gb, 0x7c7a);
  CYC(0x7c7a, 0x7c7b); A = mem_rd(gb, HL);
  CYC(0x7c7b, 0x7c7d); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7c7d, 0x7c7e); mem_wr(gb, DE, A);
  CYC(0x7c7e, 0x7c81); interactionIncState_hook(gb); return; // jp

state1:
  // Waiting for lever to be pulled
  CYC(0x7c89, 0x7c8c); A = mem_rd(gb, wLever1PullDistance);
  CYC(0x7c8c, 0x7c8e); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x7c8e, 0x7c8f); ret_effect(gb); return; } // ret z
  CYC(0x7c8e, 0x7c8f);
  // Lever has been pulled all the way.
  CALL_C(0x7c8f, interactionIncState_hook, 0x23e0, 0x7c92);
  CYC(0x7c92, 0x7c94); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c94, 0x7c96); mem_wr(gb, HL, 30);
  CYC(0x7c96, 0x7c98); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x7c98, playSound_b00_hook, 0x0c98, 0x7c9b);
  CALL_C(0x7c9b, interactionCoded8_loadScriptForSubid, 0x7cd9, 0x7c9e);
  // falls through into @toggleLavaSource
  interactionCoded8_toggleLavaSource_hook(gb); return;

state2:
  // Floor is being filled
  CALL_C(0x7ce6, interactionDecCounter1_hook, 0x23cc, 0x7ce9);
  if (!(F & FZ)) { CYCT(0x7ce9, 0x7cea); ret_effect(gb); return; } // ret nz
  CYC(0x7ce9, 0x7cea);
  // Fill next group of tiles
  CYC(0x7cea, 0x7ceb); L = alu_inc8(gb, L);
  CYC(0x7ceb, 0x7cec); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x7cec, 0x7ced); mem_wr(gb, HL, A); // [counter1] = [counter2]
  CALL_C(0x7ced, interactionGetMiniScript_hook, 0x2791, 0x7cf0);
  CYC(0x7cf0, 0x7cf1); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7cf1, 0x7cf2); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7cf2, 0x7cf5); interactionIncState_hook(gb); return; } // jp z
  CYC(0x7cf2, 0x7cf5);

fillTilesLoop:
  CYC(0x7cf5, 0x7cf6); C = A;
  CYC(0x7cf6, 0x7cf8); A = 0x01; // TILEINDEX_DRIED_LAVA
  CYC(0x7cf8, 0x7cf9); push_effect(gb, HL);
  CALL_C(0x7cf9, setTileInAllBuffers_hook, 0x3ac6, 0x7cfc);
  CYC(0x7cfc, 0x7cfd); SET_HL(pop_effect(gb));
  CYC(0x7cfd, 0x7cfe); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7cfe, 0x7cff); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7cff, 0x7d01); goto fillTilesLoop; } // jr nz
  CYC(0x7cff, 0x7d01);
  CALL_C(0x7d01, interactionSetMiniScript_hook, 0x2798, 0x7d04);
  CYC(0x7d04, 0x7d07); goto playRumbleSound; // jp $7d45

state3:
  // Tiles have been filled. Waiting for lever to revert to starting position.
  CYC(0x7d07, 0x7d0a); A = mem_rd(gb, wLever1PullDistance);
  CYC(0x7d0a, 0x7d0b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7d0b, 0x7d0c); ret_effect(gb); return; } // ret nz
  CYC(0x7d0b, 0x7d0c);
  CALL_C(0x7d0c, interactionIncState_hook, 0x23e0, 0x7d0f);
  CALL_C(0x7d0f, interactionCoded8_loadScriptForSubid, 0x7cd9, 0x7d12);
  CALL_C(0x7d12, interactionCoded8_toggleLavaSource_hook, 0x7c9e, 0x7d15);
  CYC(0x7d15, 0x7d17); A = 0x70; // SND_DOORCLOSE
  CYC(0x7d17, 0x7d1a); playSound_b00_hook(gb); return; // jp

state4:
  // Tiles are being filled with lava again.
  CALL_C(0x7d1a, interactionDecCounter1_hook, 0x23cc, 0x7d1d);
  if (!(F & FZ)) { CYCT(0x7d1d, 0x7d1e); ret_effect(gb); return; } // ret nz
  CYC(0x7d1d, 0x7d1e);
  CYC(0x7d1e, 0x7d1f); L = alu_inc8(gb, L);
  CYC(0x7d1f, 0x7d20); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x7d20, 0x7d21); mem_wr(gb, HL, A); // [counter1] = [counter2]
  CALL_C(0x7d21, interactionGetMiniScript_hook, 0x2791, 0x7d24);
  CYC(0x7d24, 0x7d25); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7d25, 0x7d26); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7d26, 0x7d28); goto fillNextGroupWithLava; } // jr nz
  CYC(0x7d26, 0x7d28);
  // Done filling the lava back.
  CYC(0x7d28, 0x7d2a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7d2a, 0x7d2c); A = 0x01;
  CYC(0x7d2c, 0x7d2d); mem_wr(gb, DE, A);
  RET(0x7d2d); return;

fillNextGroupWithLava:
  CYC(0x7d2e, 0x7d2f); C = A;
  CALL_C(0x7d2f, getRandomNumber_hook, 0x043e, 0x7d32);
  CYC(0x7d32, 0x7d34); alu_and(gb, 0x03);
  CYC(0x7d34, 0x7d36); alu_add(gb, 0x61); // TILEINDEX_DUNGEON_LAVA_1
  CYC(0x7d36, 0x7d37); push_effect(gb, HL);
  CALL_C(0x7d37, setTileInAllBuffers_hook, 0x3ac6, 0x7d3a);
  CYC(0x7d3a, 0x7d3b); SET_HL(pop_effect(gb));
  CYC(0x7d3b, 0x7d3c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7d3c, 0x7d3d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7d3d, 0x7d3f); goto fillNextGroupWithLava; } // jr nz
  CYC(0x7d3d, 0x7d3f);
  CALL_C(0x7d3f, interactionSetMiniScript_hook, 0x2798, 0x7d42);
  CYC(0x7d42, 0x7d45); goto playRumbleSound; // jp $7d45

playRumbleSound:
  CYC(0x7d45, 0x7d47); A = 0xb8; // SND_RUMBLE2
  CYC(0x7d47, 0x7d4a); playSound_b00_hook(gb); return; // jp
}
