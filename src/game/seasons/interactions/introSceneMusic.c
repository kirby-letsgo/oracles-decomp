#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/introSceneMusic.s (INTERAC_INTRO_SCENE_MUSIC),
// bank $09: starts the carnival music in the room where Din dances and fades it with Link's
// x position.

static uint16_t intro_scene_music_jump_table(GB *gb) {
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

void s_interactionCode85_hook(GB *gb) {
  BASE(interactionCode85);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (intro_scene_music_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+37) { goto state1; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x98; // <ROOM_SEASONS_098, where Din dances
  CALL_C(b_+13, getARoomFlags_hook, SYM(getARoomFlags), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+18, b_+21); TAIL(interactionDelete); }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+24); SET_HL(wTmpcfc0 + 0x17);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); A = alu_inc8(gb, A);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+32); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+32, b_+34); A = 0x08; // MUS_CARNIVAL
  CALL_C(b_+34, playSound_b00_hook, SYM(playSound_b00), b_+37);

state1:
  CYC(b_+37, b_+40); A = mem_rd(gb, w1Link_xh);
  CYC(b_+40, b_+42); alu_cp(gb, 0x70);
  CYC(b_+42, b_+44); A = 0x01;
  if (F & FC) { CYCT(b_+44, b_+46); goto set_volume; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
set_volume:
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); L = 0x77;
  CYC(b_+50, b_+51); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  CYC(b_+53, b_+56);
  TAIL(setMusicVolume);
}
