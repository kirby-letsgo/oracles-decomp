#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/miscPuzzles.s (INTERAC_MISC_PUZZLES),
// bank $15: the linked-game Hero's Cave side entrance. Subid 1 is the exit on the far side
// (warps Link back once he steps on the tile he came in on), subid 2 the entrance in the
// Hero's Cave. The dispatcher interactionCodedc stays generated (it has an Ages namesake).

static uint16_t puzzle_jump_table(GB *gb) {
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

// subid1@state0, shared with subid 2: only exists in a linked game; disables warp tiles.
static void puzzle_dc_state0(GB *gb) {
  BASE(interactionCodedc_subid1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+16);
  if (F & FZ) { CYCT(b_+16, b_+19); TAIL(interactionDelete); }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+22); mem_wr(gb, wDisableWarpTiles, A);
  RET(b_+22); return;
}

void s_interactionCodedc_subid1_hook(GB *gb) {
  BASE(interactionCodedc_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (puzzle_jump_table(gb));
    if (jt_ == b_+10) { puzzle_dc_state0(gb); return; }
    else if (jt_ == b_+23) { goto state1; }
    else if (jt_ == b_+38) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state1:
  CALL_C(b_+23, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+26);
  CYC(b_+26, b_+29); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+33); A = mem_rd(gb, wEnteredWarpPosition);
  CYC(b_+33, b_+34); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+38);
  TAIL(interactionIncState);

state2:
  CALL_C(b_+38, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+41);
  CYC(b_+41, b_+42); B = A;
  CYC(b_+42, b_+45); A = mem_rd(gb, wActiveTileIndex);
  CYC(b_+45, b_+46); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+46); return; }
  CYC(b_+46, b_+47);
  CALL_C(b_+47, checkLinkID0AndControlNormal_hook, SYM(checkLinkID0AndControlNormal), b_+50);
  if (!(F & FC)) { RET_TAKEN(b_+50); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+54); SET_HL(wWarpDestGroup);
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x85);
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x30);
  CYC(b_+59, b_+60); L = alu_inc8(gb, L);
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x93);
  CYC(b_+62, b_+63); L = alu_inc8(gb, L);
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0xff);
  CYC(b_+65, b_+67); A = 0x01;
  CYC(b_+67, b_+70); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+70, b_+73);
  TAIL(interactionDelete);
}

void s_interactionCodedc_subid2_hook(GB *gb) {
  BASE(interactionCodedc_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (puzzle_jump_table(gb));
    if (jt_ == SYM(interactionCodedc_subid1) + 10) { puzzle_dc_state0(gb); return; }
    else if (jt_ == b_+8) { goto state1; }
    else { HANDOFF(HL); }
  } while (0);

state1:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+13); mem_wr(gb, wDisableWarpTiles, A);
  CALL_C(b_+13, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+16);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+20); A = mem_rd(gb, wActiveTileIndex);
  CYC(b_+20, b_+21); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+25); A = mem_rd(gb, wLinkInAir);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; }
  CYC(b_+26, b_+27);
  CALL_C(b_+27, s_getLinkedHerosCaveSideEntranceRoom, SYM(getLinkedHerosCaveSideEntranceRoom), b_+30);
  CYC(b_+30, b_+32); A = 0x05;
  CYC(b_+32, b_+35); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+35, b_+37); A = 0x09;
  CYC(b_+37, b_+40); mem_wr(gb, wWarpTransition, A);
  CYC(b_+40, b_+42); A = 0x00;
  CYC(b_+42, b_+45); mem_wr(gb, wScrollMode, A);
  CYC(b_+45, b_+47); A = 0x0a; // LINK_STATE_WARPING
  CYC(b_+47, b_+50); mem_wr(gb, wLinkForceState, A);
  CYC(b_+50, b_+53);
  TAIL(interactionDelete);
}
