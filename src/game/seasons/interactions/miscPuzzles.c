#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/miscPuzzles.s (INTERAC_MISC_PUZZLES),
// bank $15: one-off puzzle objects. Subid 0 trigger stairs, 1/2 the linked-game Hero's Cave
// side entrance (subid1@state0 is shared), 3 a trigger mask, 4 the miniboss portal, 5 the
// five-trigger stairs, 6 a small key on toggle blocks, 7 trigger-spawned enemies, 8 the
// chest-order puzzle, 9 its portal, A/B flag setters, C an entrance text, D a dungeon re-init,
// E a layout patch, F a trigger chest. The dispatcher interactionCodedc stays generated (it
// has an Ages namesake).

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
  CALL_C(b_+27, s_getLinkedHerosCaveSideEntranceRoom_hook, SYM(getLinkedHerosCaveSideEntranceRoom), b_+30);
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

static void puzzle_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void puzzle_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// subidF@func_66d2: a puff at the object's position.
static void puzzle_f_puff(GB *gb) {
  BASE(interactionCodedc_subidF);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+69, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+72);
  if (!(F & FZ)) { RET_TAKEN(b_+72); return; }
  CYC(b_+72, b_+73);
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+75, b_+77); L = 0x4b; // Interaction.yh
  CYC(b_+77, b_+80);
  TAIL(setShortPosition_paramC);
}

// A chest (tile $f1) that appears when the triggers match xh, and turns back into the tile
// stored in the tile-index buffer (WRAM bank 3) when they do not.
void s_interactionCodedc_subidF_hook(GB *gb) {
  BASE(interactionCodedc_subidF);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); TAIL(interactionDelete); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = 0x4d; // Interaction.xh
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+18); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+18, b_+19); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto restore; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); E = 0x4b; // Interaction.yh
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+27); B = 0xcf; // wRoomLayout
  CYC(b_+27, b_+28); A = mem_rd(gb, BC);
  CYC(b_+28, b_+30); alu_cp(gb, 0xf1); // TILEINDEX_CHEST
  if (F & FZ) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); A = 0xf1; // TILEINDEX_CHEST
  CALL_C(b_+33, setTile_hook, SYM(setTile), b_+36);
  CALL_L(b_+36, puzzle_f_puff, b_+39);
  CYC(b_+39, b_+41); A = 0x4d;
  CYC(b_+41, b_+44);
  TAIL(playSound_b00);

restore:
  CYC(b_+44, b_+46); E = 0x4b; // Interaction.yh
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); C = A;
  CYC(b_+48, b_+50); B = 0xcf; // wRoomLayout
  CYC(b_+50, b_+51); A = mem_rd(gb, BC);
  CYC(b_+51, b_+53); alu_cp(gb, 0xf1); // TILEINDEX_CHEST
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); A = 0x03;
  CYC(b_+56, b_+58); mem_wr(gb, IO_SVBK, A);
  CYC(b_+58, b_+60); B = 0xdf; // w3TileIndexBuffer (bank 3)
  CYC(b_+60, b_+61); A = mem_rd(gb, BC);
  CYC(b_+61, b_+62); L = A;
  CYC(b_+62, b_+63); alu_xor(gb, A);
  CYC(b_+63, b_+65); mem_wr(gb, IO_SVBK, A);
  CYC(b_+65, b_+66); A = L;
  CALL_C(b_+66, setTile_hook, SYM(setTile), b_+69);
  puzzle_f_puff(gb); return;
}

// Sets tile b at position a with a puff and a 15-frame counter.
void s_func_6744_hook(GB *gb) {
  BASE(func_6744);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+2); A = B;
  CALL_C(b_+2, setTile_hook, SYM(setTile), b_+5);
  CALL_C(b_+5, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+11, b_+13); L = 0x4b; // Interaction.yh
  CALL_C(b_+13, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = 0x46; // Interaction.counter1
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x0f);
  RET(b_+21); return;
}

// Three stairs (table_675a positions) that rise one by one while the triggers are held and
// sink back when they are released.
void s_interactionCodedc_subid0_hook(GB *gb) {
  BASE(interactionCodedc_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (puzzle_jump_table(gb));
    if (jt_ == SYM(interactionIncState) && hook_is(gb, SYM(interactionIncState), interactionIncState_hook)) { interactionIncState_hook(gb); return; }
    else if (jt_ == b_+16) { goto state0; }
    else if (jt_ == b_+29) { goto state1; }
    else if (jt_ == b_+57) { goto state2; }
    else if (jt_ == b_+66) { goto state3; }
    else if (jt_ == b_+92) { goto state4; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+16, b_+19); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); A = 0x01;
  CYC(b_+23, b_+25); E = 0x46; // Interaction.counter1
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+29);
  TAIL(interactionIncState);

state1:
  CALL_C(b_+29, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+32);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); L = 0x45; // Interaction.substate
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+38); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+38, b_+40); goto all_up; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+41, b_+44); SET_HL(SYM(table_675a));
  CYC(b_+44, b_+45); puzzle_add_a_to_hl(gb, b_+45);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+48); B = 0x6d;
  CYCT(b_+48, b_+50);
  TAIL_S(func_6744);
all_up:
  CALL_C(b_+50, interactionIncState_hook, SYM(interactionIncState), b_+53);
  CYC(b_+53, b_+55); L = 0x46; // Interaction.counter1
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x43);
state2:
  CALL_C(b_+57, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+60);
  if (!(F & FZ)) { RET_TAKEN(b_+60); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x01);
  CYC(b_+63, b_+66);
  TAIL(interactionIncState);

state3:
  CALL_C(b_+66, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+69);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+72); L = 0x45; // Interaction.substate
  CYC(b_+72, b_+73); A = mem_rd(gb, HL);
  CYC(b_+73, b_+74); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+74, b_+77); TAIL(interactionIncState); }
  CYC(b_+74, b_+77);
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CYC(b_+79, b_+82); SET_HL(SYM(table_675a));
  CYC(b_+82, b_+83); puzzle_add_a_to_hl(gb, b_+83);
  CYC(b_+83, b_+84); A = mem_rd(gb, HL);
  CYC(b_+84, b_+86); B = 0xfd;
  CALL_C(b_+86, s_func_6744_hook, SYM(func_6744), b_+89);
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x1e);
  RET(b_+91); return;

state4:
  CYC(b_+92, b_+95); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+95, b_+96); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+96); return; }
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+99); A = 0x01;
  CYC(b_+99, b_+101); E = 0x44; // Interaction.state
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  RET(b_+102); return;
}

// Sets bit 7 of wActiveTriggers while every trigger in the yh mask is pressed.
void s_interactionCodedc_subid3_hook(GB *gb) {
  BASE(interactionCodedc_subid3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x4b; // Interaction.yh
  CYC(b_+3, b_+6); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+6, b_+7); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+11); SET_HL(wActiveTriggers);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto clear; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+15); return;
clear:
  CYC(b_+16, b_+19); SET_HL(wActiveTriggers);
  CYC(b_+19, b_+21); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  RET(b_+21); return;
}

// subid4@func_681b: records the miniboss portal (var30) in wc64a if it is a later one.
static void puzzle_record_portal(GB *gb) {
  BASE(interactionCodedc_subid4);
  CYC(b_+42, b_+44); E = 0x70; // Interaction.var30
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+48); SET_HL(wc64a);
  CYC(b_+48, b_+49); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { RET_TAKEN(b_+49); return; }
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A);
  RET(b_+51); return;
}

// The miniboss portal spawner: once the room flag is set, a puff and the portal at the
// object's short position.
void s_interactionCodedc_subid4_hook(GB *gb) {
  BASE(interactionCodedc_subid4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (puzzle_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+52) { goto state1; }
    else if (jt_ == b_+68) { goto state2; }
    else if (jt_ == b_+83) { goto state3; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+12, b_+14); E = 0x4d; // Interaction.xh
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); E = 0x70; // Interaction.var30
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); B = A;
  CALL_C(b_+19, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+22);
  CYC(b_+22, b_+24); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(b_+24, b_+26); goto place; }
  CYC(b_+24, b_+26);
  CALL_L(b_+26, puzzle_record_portal, b_+29);
  CYC(b_+29, b_+32);
  TAIL(interactionDelete);
place:
  CYC(b_+32, b_+34); E = 0x4b; // Interaction.yh
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); C = A;
  CALL_C(b_+36, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+39);
  CYC(b_+39, b_+42);
  TAIL(interactionIncState);

state1:
  CALL_C(b_+52, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+55);
  CYC(b_+55, b_+57); alu_and(gb, 0x20);
  if (F & FZ) { RET_TAKEN(b_+57); return; }
  CYC(b_+57, b_+58);
  CALL_L(b_+58, puzzle_record_portal, b_+61);
  CALL_C(b_+61, interactionIncState_hook, SYM(interactionIncState), b_+64);
  CYC(b_+64, b_+66); L = 0x46; // Interaction.counter1
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x28);
state2:
  CALL_C(b_+68, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+71);
  CALL_C(b_+71, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+74);
  if (!(F & FZ)) { RET_TAKEN(b_+74); return; }
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+77, objectCreatePuff_hook, SYM(objectCreatePuff), b_+80);
  CYC(b_+80, b_+83);
  TAIL(interactionIncState);

state3:
  CALL_C(b_+83, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+86);
  if (!(F & FZ)) { RET_TAKEN(b_+86); return; }
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+89); A = 0x4d;
  CALL_C(b_+89, playSound_b00_hook, SYM(playSound_b00), b_+92);
  CYC(b_+92, b_+95); SET_BC(0x7e02); // INTERAC_MINIBOSS_PORTAL, subid 2
  CALL_C(b_+95, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+98);
  if (!(F & FZ)) { RET_TAKEN(b_+98); return; }
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+102);
  TAIL(interactionDelete);
}

// With all five triggers ($1f) pressed, raises tiles $6d from position $73 to $7d one per 15
// frames; when a trigger is released they sink back ($f4) in reverse.
void s_interactionCodedc_subid5_hook(GB *gb) {
  BASE(interactionCodedc_subid5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (puzzle_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == b_+17) { goto state1; }
    else if (jt_ == b_+36) { goto state2; }
    else if (jt_ == b_+57) { goto state3; }
    else if (jt_ == b_+66) { goto state4; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
state1:
  CYC(b_+17, b_+20); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+20, b_+22); alu_cp(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); A = 0x0f;
  CYC(b_+26, b_+28); L = 0x46; // Interaction.counter1
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x0f);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x73);
  CYC(b_+33, b_+36);
  TAIL(interactionIncState);

state2:
  CALL_C(b_+36, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+39);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; }
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CYC(b_+42, b_+44); B = 0x6d;
  CALL_C(b_+44, s_func_6744_hook, SYM(func_6744), b_+47);
  CYC(b_+47, b_+48); A = C;
  CYC(b_+48, b_+50); alu_cp(gb, 0x7d);
  if (F & FZ) { CYCT(b_+50, b_+53); TAIL(interactionIncState); }
  CYC(b_+50, b_+53);
  CYC(b_+53, b_+55); L = 0x47; // Interaction.counter2
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+56); return;

state3:
  CYC(b_+57, b_+60); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+60, b_+62); alu_cp(gb, 0x1f);
  if (F & FZ) { RET_TAKEN(b_+62); return; }
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+66);
  TAIL(interactionIncState);

state4:
  CALL_C(b_+66, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+69);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+71); L = alu_inc8(gb, L);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  CYC(b_+72, b_+74); B = 0xf4;
  CALL_C(b_+74, s_func_6744_hook, SYM(func_6744), b_+77);
  CYC(b_+77, b_+78); A = C;
  CYC(b_+78, b_+80); alu_cp(gb, 0x73);
  if (F & FZ) { CYCT(b_+80, b_+82); goto restart; }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+84); L = 0x47; // Interaction.counter2
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+85); return;
restart:
  CYC(b_+86, b_+87); H = D;
  CYC(b_+87, b_+89); L = 0x44; // Interaction.state
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x01);
  RET(b_+91); return;
}

// Drops a small key at the object once the toggle blocks have been switched (bit 6).
void s_interactionCodedc_subid6_hook(GB *gb) {
  BASE(interactionCodedc_subid6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto check; }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); TAIL(interactionDelete); }
  CYC(b_+11, b_+14);
  CALL_C(b_+14, interactionIncState_hook, SYM(interactionIncState), b_+17);
check:
  CYC(b_+17, b_+20); A = mem_rd(gb, wToggleBlocksState);
  CYC(b_+20, b_+22); alu_bit(gb, 6, A);
  if (F & FZ) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CALL_C(b_+23, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+26);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x30); // TREASURE_SMALL_KEY
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x01);
  CALL_C(b_+35, objectCopyPosition_hook, SYM(objectCopyPosition), b_+38);
  CYC(b_+38, b_+41);
  TAIL(interactionDelete);
}

// subid7@func_6923: c = the b-th (1..4) spawn position from @table_692b.
static void puzzle_7_position(GB *gb) {
  BASE(interactionCodedc_subid7);
  CYC(b_+71, b_+72); A = B;
  CYC(b_+72, b_+73); A = alu_dec8(gb, A);
  CYC(b_+73, b_+76); SET_HL(b_+79); // @table_692b
  CYC(b_+76, b_+77); puzzle_add_a_to_hl(gb, b_+77);
  CYC(b_+77, b_+78); C = mem_rd(gb, HL);
  RET(b_+78); return;
}

// subid7@func_692f: de = enemy id and subid for the trigger in var31, from @table_693a.
static void puzzle_7_enemy(GB *gb) {
  BASE(interactionCodedc_subid7);
  CYC(b_+83, b_+85); E = 0x71; // Interaction.var31
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+89); SET_HL(b_+94); // @table_693a
  CYC(b_+89, b_+90); puzzle_add_double_index(gb, b_+90);
  CYC(b_+90, b_+91); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+91, b_+92); E = mem_rd(gb, HL);
  CYC(b_+92, b_+93); D = A;
  RET(b_+93); return;
}

// Each newly pressed trigger spawns four puffs, then four enemies of the kind that trigger
// selects; once they are all beaten the puzzle waits for the next trigger.
void s_interactionCodedc_subid7_hook(GB *gb) {
  BASE(interactionCodedc_subid7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (puzzle_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == b_+17) { goto state1; }
    else if (jt_ == b_+44) { goto state2; }
    else if (jt_ == b_+110) { goto state3; }
    else if (jt_ == b_+148) { goto state4; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
state1:
  CYC(b_+17, b_+19); E = 0x70; // Interaction.var30
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+24); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+24, b_+25); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+30); mem_wr(gb, wccb1, A);
  CYC(b_+30, b_+31); C = A;
  CYC(b_+31, b_+32); A = B;
  CYC(b_+32, b_+33); alu_cpl(gb);
  CYC(b_+33, b_+34); alu_and(gb, C);
  CALL_C(b_+34, getHighestSetBit_hook, SYM(getHighestSetBit), b_+37);
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+40); L = 0x71; // Interaction.var31
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CYC(b_+41, b_+44);
  TAIL(interactionIncState);

state2:
  CYC(b_+44, b_+46); B = 0x04;
  for (;;) {
    CALL_L(b_+46, puzzle_7_position, b_+49);
    CALL_C(b_+49, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+52);
    if (!(F & FZ)) { RET_TAKEN(b_+52); return; }
    CYC(b_+52, b_+53);
    CYC(b_+53, b_+55); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
    CYC(b_+55, b_+57); L = 0x4b; // Interaction.yh
    CALL_C(b_+57, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+60);
    CYC(b_+60, b_+61); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+61, b_+63); continue; }
    CYC(b_+61, b_+63);
    break;
  }
  CALL_C(b_+63, interactionIncState_hook, SYM(interactionIncState), b_+66);
  CYC(b_+66, b_+68); L = 0x46; // Interaction.counter1
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x1e);
  RET(b_+70); return;

state3:
  CALL_C(b_+110, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+113);
  if (!(F & FZ)) { RET_TAKEN(b_+113); return; }
  CYC(b_+113, b_+114);
  CYC(b_+114, b_+116); A = 0x01;
  CYC(b_+116, b_+119); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CALL_L(b_+119, puzzle_7_enemy, b_+122);
  CYC(b_+122, b_+124); B = 0x04;
  for (;;) {
    CALL_L(b_+124, puzzle_7_position, b_+127);
    CALL_C(b_+127, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+130);
    if (!(F & FZ)) { RET_TAKEN(b_+130); return; }
    CYC(b_+130, b_+131);
    CYC(b_+131, b_+132); mem_wr(gb, HL, D);
    CYC(b_+132, b_+133); L = alu_inc8(gb, L);
    CYC(b_+133, b_+134); mem_wr(gb, HL, E);
    CYC(b_+134, b_+136); L = 0x8b; // Enemy.yh
    CALL_C(b_+136, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+139);
    CYC(b_+139, b_+140); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+140, b_+142); continue; }
    CYC(b_+140, b_+142);
    break;
  }
  CYC(b_+142, b_+144); A = mem_rd(gb, hActiveObject);
  CYC(b_+144, b_+145); D = A;
  CYC(b_+145, b_+148);
  TAIL(interactionIncState);

state4:
  CYC(b_+148, b_+151); A = mem_rd(gb, wNumEnemies);
  CYC(b_+151, b_+152); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+152); return; }
  CYC(b_+152, b_+153);
  CYC(b_+153, b_+156); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+156, b_+157); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+157, b_+160); TAIL(interactionDelete); }
  CYC(b_+157, b_+160);
  CYC(b_+160, b_+161); alu_xor(gb, A);
  CYC(b_+161, b_+164); mem_wr(gb, wccb1, A);
  CYC(b_+164, b_+166); E = 0x44; // Interaction.state
  CYC(b_+166, b_+168); A = 0x01;
  CYC(b_+168, b_+169); mem_wr(gb, DE, A);
  RET(b_+169); return;
}

// The chest-order puzzle: opening the chests in @dungeonsInOrder pays rupees, the eighth in
// order opens the portal (tile $4f) and sets the room flag; a wrong chest gives less.
void s_interactionCodedc_subid8_hook(GB *gb) {
  BASE(interactionCodedc_subid8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (puzzle_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+11) { goto state1; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
state1:
  CYC(b_+11, b_+14); A = mem_rd(gb, wcca2);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+19); E = 0x47; // Interaction.counter2
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+23); SET_HL(b_+124); // @dungeonsInOrder
  CYC(b_+23, b_+24); puzzle_add_a_to_hl(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto wrong; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYCT(b_+31, b_+33); goto reward;
wrong:
  CYC(b_+33, b_+35); E = 0x70; // Interaction.var30
  CYC(b_+35, b_+37); A = 0x01;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
reward:
  CYC(b_+38, b_+41); SET_BC(0x2809); // TREASURE_RUPEES $09
  CYC(b_+41, b_+43); E = 0x70; // Interaction.var30
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto wrong_chest; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+50); SET_BC(0x280d); // TREASURE_RUPEES $0d
  CYC(b_+50, b_+52); E = 0x47; // Interaction.counter2
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+55); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(b_+55, b_+57); goto spawn; }
  CYC(b_+55, b_+57);
  CALL_C(b_+57, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+60);
  CYC(b_+60, b_+62); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+62, b_+64); goto spawn; }
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+66); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+66, s_func_6a18_hook, SYM(func_6a18), b_+69);
  CYC(b_+69, b_+71); A = 0x4f;
  CALL_C(b_+71, setTile_hook, SYM(setTile), b_+74);
  CYC(b_+74, b_+76); A = 0x4d; // SND_SOLVEPUZZLE
  CYC(b_+76, b_+79); SET_BC(0x280d); // TREASURE_RUPEES $0d
  CYCT(b_+79, b_+81); goto success;
wrong_chest:
  CYC(b_+81, b_+83); A = 0x5a; // SND_ERROR
success:
  CALL_C(b_+83, playSound_b00_hook, SYM(playSound_b00), b_+86);
spawn:
  CALL_C(b_+86, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+89);
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+92, b_+93); L = alu_inc8(gb, L);
  CYC(b_+93, b_+94); mem_wr(gb, HL, B);
  CYC(b_+94, b_+95); L = alu_inc8(gb, L);
  CYC(b_+95, b_+96); mem_wr(gb, HL, C);
  CYC(b_+96, b_+98); L = 0x4b; // Interaction.yh
  CYC(b_+98, b_+101); A = mem_rd(gb, wcca2);
  CYC(b_+101, b_+102); B = A;
  CYC(b_+102, b_+104); alu_and(gb, 0xf0);
  CYC(b_+104, b_+105); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+105, b_+106); L = alu_inc8(gb, L);
  CYC(b_+106, b_+107); A = B;
  CYC(b_+107, b_+109); A = alu_swap(gb, A);
  CYC(b_+109, b_+111); alu_and(gb, 0xf0);
  CYC(b_+111, b_+113); alu_or(gb, 0x08);
  CYC(b_+113, b_+114); mem_wr(gb, HL, A);
  CYC(b_+114, b_+116); A = 0x81;
  CYC(b_+116, b_+119); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+119, b_+120); alu_xor(gb, A);
  CYC(b_+120, b_+123); mem_wr(gb, wcca2, A);
  RET(b_+123); return;
}

// The portal spawner for a solved chest-order room.
void s_interactionCodedc_subid9_hook(GB *gb) {
  BASE(interactionCodedc_subid9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(interactionDelete); }
  CYC(b_+5, b_+8);
  CALL_C(b_+8, s_func_6a18_hook, SYM(func_6a18), b_+11);
  CYC(b_+11, b_+14);
  TAIL(interactionDelete);
}

// Spawns the portal spawner (subid 1) at position $57.
void s_func_6a18_hook(GB *gb) {
  BASE(func_6a18);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xe1); // INTERAC_PORTAL_SPAWNER
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x01);
  CYC(b_+9, b_+11); C = 0x57;
  CYC(b_+11, b_+13); L = 0x4b; // Interaction.yh
  CYC(b_+13, b_+16);
  TAIL(setShortPosition_paramC);
}

void s_interactionCodedc_subidA_hook(GB *gb) {
  BASE(interactionCodedc_subidA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wGroup4RoomFlags + 0x04);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+5, b_+8);
  TAIL(interactionDelete);
}

void s_interactionCodedc_subidB_hook(GB *gb) {
  BASE(interactionCodedc_subidB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wToggleBlocksState, A);
  CYC(b_+4, b_+7);
  TAIL(interactionDelete);
}

// Shows TX_0202 when Link enters on the object's tile, with objects and the menu disabled
// until the text closes.
void s_interactionCodedc_subidC_hook(GB *gb) {
  BASE(interactionCodedc_subidC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto text; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wEnteredWarpPosition);
  CYC(b_+11, b_+12); alu_cp(gb, L);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); TAIL(interactionDelete); }
  CYC(b_+12, b_+15);
  CALL_C(b_+15, interactionIncState_hook, SYM(interactionIncState), b_+18);
  CALL_C(b_+18, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+21);
  CYC(b_+21, b_+23); A = 0x81;
  CYC(b_+23, b_+26); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+26, b_+29); mem_wr(gb, wMenuDisabled, A);
text:
  CYC(b_+29, b_+32); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+37); SET_BC(0x0202); // TX_0202
  CALL_C(b_+37, showText_hook, SYM(showText), b_+40);
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CYC(b_+41, b_+44); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+44, b_+47); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+47, b_+50);
  TAIL(interactionDelete);
}

// Entering at warp position $22 re-initializes the dungeon state.
void s_interactionCodedc_subidD_hook(GB *gb) {
  BASE(interactionCodedc_subidD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wWarpDestPos);
  CYC(b_+3, b_+5); alu_cp(gb, 0x22);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto done; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); mem_wr(gb, wWarpDestPos, A);
  CALL_C(b_+11, initializeDungeonStuff_hook, SYM(initializeDungeonStuff), b_+14);
done:
  CYC(b_+14, b_+17);
  TAIL(interactionDelete);
}

// Once scrolling starts, replaces every $fe/$ff tile of the room layout with $7b.
void s_interactionCodedc_subidE_hook(GB *gb) {
  BASE(interactionCodedc_subidE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScrollMode);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x79);
  for (;;) {
    CYC(b_+9, b_+10); A = mem_rd(gb, HL);
    CYC(b_+10, b_+12); alu_cp(gb, 0xfe);
    if (F & FZ) { CYCT(b_+12, b_+14); goto replace; }
    CYC(b_+12, b_+14);
    CYC(b_+14, b_+16); alu_cp(gb, 0xff);
    if (!(F & FZ)) { CYCT(b_+16, b_+18); goto next; }
    CYC(b_+16, b_+18);
replace:
    CYC(b_+18, b_+20); mem_wr(gb, HL, 0x7b);
next:
    CYC(b_+20, b_+21); L = alu_dec8(gb, L);
    if (!(F & FZ)) { CYCT(b_+21, b_+23); continue; }
    CYC(b_+21, b_+23);
    break;
  }
  CYC(b_+23, b_+26);
  TAIL(interactionDelete);
}
