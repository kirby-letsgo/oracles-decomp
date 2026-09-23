#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_DEKU_SCRUB), bank 0x15.

static void deku_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// var38 = 1 when Link has all five seed types and each is at the satchel's capacity.
void s_dekuScrub_upgradeSatchel_hook(GB *gb) {
  BASE(dekuScrub_upgradeSatchel);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x20; // TREASURE_EMBER_SEEDS
  for (;;) {
    CYC(b_+2, b_+3); A = E;
    CALL_C(b_+3, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+6);
    if (!(F & FC)) { RET_TAKEN(b_+6); return; }
    CYC(b_+6, b_+7);
    CYC(b_+7, b_+8); E = alu_inc8(gb, E);
    CYC(b_+8, b_+9); A = E;
    CYC(b_+9, b_+11); alu_cp(gb, 0x25); // TREASURE_MYSTERY_SEEDS+1
    if (F & FC) { CYCT(b_+11, b_+13); continue; }
    CYC(b_+11, b_+13);
    break;
  }
  CYC(b_+13, b_+16); A = mem_rd(gb, wSeedSatchelLevel);
  CYC(b_+16, b_+19); SET_HL(SYM(table_65cf) - 1);
  CYC(b_+19, b_+20); deku_add_a_to_hl(gb, b_+20);
  CYC(b_+20, b_+21); B = mem_rd(gb, HL);
  CYC(b_+21, b_+24); SET_HL(wNumEmberSeeds);
  for (;;) {
    CYC(b_+24, b_+25); A = B;
    CYC(b_+25, b_+26); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FZ)) { RET_TAKEN(b_+26); return; }
    CYC(b_+26, b_+27);
    CYC(b_+27, b_+28); L = alu_inc8(gb, L);
    CYC(b_+28, b_+29); A = L;
    CYC(b_+29, b_+31); alu_cp(gb, (uint8_t)(wNumMysterySeeds + 1));
    if (F & FC) { CYCT(b_+31, b_+33); continue; }
    CYC(b_+31, b_+33);
    break;
  }
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = 0x78; // Interaction.var38
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x01);
  RET(b_+38); return;
}
