#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCoded8), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCoded8), (from), (to), true)

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
  BASE(interactionCoded8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+105, b_+107); A = hram_rd(gb, 0x8d); // ldh a,($ff8d)
  CYC(b_+107, b_+108); B = mem_rd(gb, HL);
  CYC(b_+108, b_+109); alu_add(gb, B);
  CYC(b_+109, b_+110); C = L;
  CYC(b_+110, b_+111); push_effect(gb, HL);
  CALL_C(b_+111, setTile_hook, SYM(setTile), b_+114);
  CYC(b_+114, b_+115); SET_HL(pop_effect(gb));
  RET(b_+115); return;
}

// interactionCoded8@loadScriptForSubid, 0b:7cd9 -- true local, called via a real `call` from
// @state1 and @state3; never executes its own ret, tail-jumps into interactionSetMiniScript.
static void interactionCoded8_loadScriptForSubid(GB *gb) {
  BASE(interactionCoded8);
  CYC(b_+116, b_+118); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+118, b_+119); A = mem_rd(gb, DE);
  CYC(b_+119, b_+122); SET_HL(b_+229); // @scriptTable
  CYC(b_+122, b_+123); interactionCoded8_addDoubleIndex(gb, b_+123);
  CYC(b_+123, b_+124); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+124, b_+125); H = mem_rd(gb, HL);
  CYC(b_+125, b_+126); L = A;
  CYC(b_+126, b_+129); interactionSetMiniScript_hook(gb); return; // jp
}

// interactionCoded8@toggleLavaSource, 0b:7c9e -- reached as a plain fallthrough from @state1 --
// no real call frame there, interactionCoded8_hook calls this as a tail and returns -- and by
// a genuine `call` from @state3, which expects a real return to continue with SND_DOORCLOSE.
// Implemented as its own function either way: a plain tail call from @state1 needs no resume
// check of its own, matching the trampoline pattern; @state3's CALL_C wraps it normally instead.
// The resume checks this function's own CALL_C calls need are keyed off its own sp0_, which is
// correct in both cases since a plain tail call pushes nothing extra onto the emulated stack.
void interactionCoded8_toggleLavaSource_hook(GB *gb) {
  BASE(interactionCoded8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+57, b_+59); B = 0x06;
  CYC(b_+59, b_+61); A = 0xc3; // TILEINDEX_LAVA_SOURCE_UP_LEFT
  CALL_C(b_+61, findTileInRoom_hook, SYM(findTileInRoom), b_+64);
  if (F & FZ) { CYCT(b_+64, b_+66); goto setOrUnsetLavaSource; } // jr z
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); A = 0xc6; // TILEINDEX_LAVA_SOURCE_DOWN_LEFT
  CALL_C(b_+68, findTileInRoom_hook, SYM(findTileInRoom), b_+71);
  if (F & FZ) { CYCT(b_+71, b_+73); goto setOrUnsetLavaSource; } // jr z
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+75); B = 0xfa;
  CYC(b_+75, b_+77); A = 0xc9; // TILEINDEX_LAVA_SOURCE_UP_LEFT_EMPTY
  CALL_C(b_+77, findTileInRoom_hook, SYM(findTileInRoom), b_+80);
  if (F & FZ) { CYCT(b_+80, b_+82); goto setOrUnsetLavaSource; } // jr z
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+84); A = 0xcc; // TILEINDEX_LAVA_SOURCE_DOWN_LEFT_EMPTY
  CALL_C(b_+84, findTileInRoom_hook, SYM(findTileInRoom), b_+87);

setOrUnsetLavaSource:
  CYC(b_+87, b_+88); A = B;
  CYC(b_+88, b_+90); hram_wr(gb, 0x8d, A); // ldh ($ff8d),a
  CALL_C(b_+90, interactionCoded8_updateTile_hook, b_+105, b_+93);

tileLoop:
  CYC(b_+93, b_+94); L = alu_inc8(gb, L);
  CYC(b_+94, b_+95); A = mem_rd(gb, HL);
  CYC(b_+95, b_+97); alu_sub(gb, 0xc3); // TILEINDEX_LAVA_SOURCE_UP_LEFT
  CYC(b_+97, b_+99); alu_cp(gb, 0x0c);
  if (!(F & FC)) { CYCT(b_+99, b_+100); ret_effect(gb); return; } // ret nc
  CYC(b_+99, b_+100);
  CALL_C(b_+100, interactionCoded8_updateTile_hook, b_+105, b_+103);
  CYC(b_+103, b_+105); goto tileLoop; // jr $7cc2
}

// ==================================================================================================
// INTERAC_LEVER_LAVA_FILLER
//
// Variables:
//   counter2: Number of frames between two lava tiles being filled. Effectively this sets the
//             "speed" of the lava filler (lower is faster).
// ==================================================================================================
void interactionCoded8_hook(GB *gb) {
  BASE(interactionCoded8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCoded8_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == b_+36) { goto state1; }
    else if (jt_ == b_+129) { goto state2; }
    else if (jt_ == b_+162) { goto state3; }
    else if (jt_ == b_+181) { goto state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+20); SET_HL(b_+28); // @counter2Vals
  CYC(b_+20, b_+21); interactionCoded8_addAToHl(gb, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28); interactionIncState_hook(gb); return; // jp

state1:
  // Waiting for lever to be pulled
  CYC(b_+36, b_+39); A = mem_rd(gb, wLever1PullDistance);
  CYC(b_+39, b_+41); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+41, b_+42); ret_effect(gb); return; } // ret z
  CYC(b_+41, b_+42);
  // Lever has been pulled all the way.
  CALL_C(b_+42, interactionIncState_hook, SYM(interactionIncState), b_+45);
  CYC(b_+45, b_+47); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 30);
  CYC(b_+49, b_+51); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+51, playSound_b00_hook, SYM(playSound_b00), b_+54);
  CALL_C(b_+54, interactionCoded8_loadScriptForSubid, b_+116, b_+57);
  // falls through into @toggleLavaSource
  interactionCoded8_toggleLavaSource_hook(gb); return;

state2:
  // Floor is being filled
  CALL_C(b_+129, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } // ret nz
  CYC(b_+132, b_+133);
  // Fill next group of tiles
  CYC(b_+133, b_+134); L = alu_inc8(gb, L);
  CYC(b_+134, b_+135); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+135, b_+136); mem_wr(gb, HL, A); // [counter1] = [counter2]
  CALL_C(b_+136, interactionGetMiniScript_hook, SYM(interactionGetMiniScript), b_+139);
  CYC(b_+139, b_+140); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+140, b_+141); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+141, b_+144); interactionIncState_hook(gb); return; } // jp z
  CYC(b_+141, b_+144);

fillTilesLoop:
  CYC(b_+144, b_+145); C = A;
  CYC(b_+145, b_+147); A = 0x01; // TILEINDEX_DRIED_LAVA
  CYC(b_+147, b_+148); push_effect(gb, HL);
  CALL_C(b_+148, setTileInAllBuffers_hook, SYM(setTileInAllBuffers), b_+151);
  CYC(b_+151, b_+152); SET_HL(pop_effect(gb));
  CYC(b_+152, b_+153); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+153, b_+154); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+154, b_+156); goto fillTilesLoop; } // jr nz
  CYC(b_+154, b_+156);
  CALL_C(b_+156, interactionSetMiniScript_hook, SYM(interactionSetMiniScript), b_+159);
  CYC(b_+159, b_+162); goto playRumbleSound; // jp $7d45

state3:
  // Tiles have been filled. Waiting for lever to revert to starting position.
  CYC(b_+162, b_+165); A = mem_rd(gb, wLever1PullDistance);
  CYC(b_+165, b_+166); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+166, b_+167); ret_effect(gb); return; } // ret nz
  CYC(b_+166, b_+167);
  CALL_C(b_+167, interactionIncState_hook, SYM(interactionIncState), b_+170);
  CALL_C(b_+170, interactionCoded8_loadScriptForSubid, b_+116, b_+173);
  CALL_C(b_+173, interactionCoded8_toggleLavaSource_hook, b_+57, b_+176);
  CYC(b_+176, b_+178); A = 0x70; // SND_DOORCLOSE
  CYC(b_+178, b_+181); playSound_b00_hook(gb); return; // jp

state4:
  // Tiles are being filled with lava again.
  CALL_C(b_+181, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+184);
  if (!(F & FZ)) { CYCT(b_+184, b_+185); ret_effect(gb); return; } // ret nz
  CYC(b_+184, b_+185);
  CYC(b_+185, b_+186); L = alu_inc8(gb, L);
  CYC(b_+186, b_+187); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+187, b_+188); mem_wr(gb, HL, A); // [counter1] = [counter2]
  CALL_C(b_+188, interactionGetMiniScript_hook, SYM(interactionGetMiniScript), b_+191);
  CYC(b_+191, b_+192); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+192, b_+193); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+193, b_+195); goto fillNextGroupWithLava; } // jr nz
  CYC(b_+193, b_+195);
  // Done filling the lava back.
  CYC(b_+195, b_+197); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+197, b_+199); A = 0x01;
  CYC(b_+199, b_+200); mem_wr(gb, DE, A);
  RET(b_+200); return;

fillNextGroupWithLava:
  CYC(b_+201, b_+202); C = A;
  CALL_C(b_+202, getRandomNumber_hook, SYM(getRandomNumber), b_+205);
  CYC(b_+205, b_+207); alu_and(gb, 0x03);
  CYC(b_+207, b_+209); alu_add(gb, 0x61); // TILEINDEX_DUNGEON_LAVA_1
  CYC(b_+209, b_+210); push_effect(gb, HL);
  CALL_C(b_+210, setTileInAllBuffers_hook, SYM(setTileInAllBuffers), b_+213);
  CYC(b_+213, b_+214); SET_HL(pop_effect(gb));
  CYC(b_+214, b_+215); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+215, b_+216); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+216, b_+218); goto fillNextGroupWithLava; } // jr nz
  CYC(b_+216, b_+218);
  CALL_C(b_+218, interactionSetMiniScript_hook, SYM(interactionSetMiniScript), b_+221);
  CYC(b_+221, b_+224); goto playRumbleSound; // jp $7d45

playRumbleSound:
  CYC(b_+224, b_+226); A = 0xb8; // SND_RUMBLE2
  CYC(b_+226, b_+229); playSound_b00_hook(gb); return; // jp
}
