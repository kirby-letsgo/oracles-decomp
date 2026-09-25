#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/lostWoodsDekuScrub.s.
// INTERAC_LOST_WOODS_DEKU_SCRUB

static uint16_t lostWoodsDekuScrub_jump_table(GB *gb) {
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

// interactionCode5b@func_7f55
static void lostWoodsDekuScrub_func_7f55(GB *gb) {
  BASE(interactionCode5b);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+48, b_+50); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); push_effect(gb, b_+52);
  do { uint16_t jt_ = (lostWoodsDekuScrub_jump_table(gb));
    if (jt_ == b_+56) goto func_7f55_var39_00;
    if (jt_ == b_+72) goto func_7f55_var39_01;
    HANDOFF(HL);
  } while (0);
func_7f55_var39_00:
  CYC(b_+56, b_+57); H = D;
  CYC(b_+57, b_+59); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+62); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+62); return; }
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+65); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x01);
  CYC(b_+67, b_+69); A = 0x3d;
  CYC(b_+69, b_+72);
  TAIL(playSound_b00);
func_7f55_var39_01:
  RET(b_+72); return;
}

// INTERAC_LOST_WOODS_DEKU_SCRUB
void s_interactionCode5b_hook(GB *gb) {
  BASE(interactionCode5b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (lostWoodsDekuScrub_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+39) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+13); A = 0x86;
  CALL_C(b_+13, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+16);
  CALL_C(b_+16, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+19);
  CYC(b_+19, b_+21); A = 0x0b;
  CALL_C(b_+21, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+24);
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x01);
  CYC(b_+29, b_+31); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x04);
  CYC(b_+33, b_+36); SET_HL(SYM(lostWoodsDekuScrubScript));
  CALL_C(b_+36, s_interactionSetScript, SYM(interactionSetScript), b_+39);
state1:
  CALL_C(b_+39, s_interactionRunScript, SYM(interactionRunScript), b_+42);
  CALL_L(b_+42, lostWoodsDekuScrub_func_7f55, b_+45);
  CYC(b_+45, b_+48);
  TAIL(interactionAnimateAsNpc);
}
