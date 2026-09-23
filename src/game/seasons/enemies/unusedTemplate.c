#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/unusedTemplate.s.
// Leftover template code maybe? Nothing here is referenced by anything.

static uint16_t unusedTemplate_jump_table(GB *gb) {
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

void s_enemyCodeTemplate0_hook(GB *gb) {
  BASE(enemyCodeTemplate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (unusedTemplate_jump_table(gb));
    if (jt_ == b_+22) goto state0;
    if (jt_ == b_+25) goto state_stub;
    if (jt_ == b_+26) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+22, b_+25);
  TAIL(ecom_setSpeedAndState8AndVisible_b0d);
state_stub:
  RET(b_+25); return;
state2:
  CYC(b_+26, b_+29);
  TAIL(enemyAnimate);
}

void s_enemyCodeTemplate1_hook(GB *gb) {
  BASE(enemyCodeTemplate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (unusedTemplate_jump_table(gb));
    if (jt_ == b_+20) goto state0;
    if (jt_ == b_+23) goto state_stub;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+20, b_+23);
  TAIL(enemyDelete);
state_stub:
  RET(b_+23); return;
}

