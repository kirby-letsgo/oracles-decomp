#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/treasureAndDrops.s, the Seasons-only pieces.

static void treasure_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

// Finds treasure A in treasureDisplayData1 and returns its display indices in DE. Seasons
// shows the slingshot's (treasure $13) second entry once it is the hyper slingshot (level 2).
void s_loadTreasureDisplayData__getTableIndices_b3f_hook(GB *gb) {
  BASE(loadTreasureDisplayData_b3f);
  CYC(b_+51, b_+52); D = A;
  CYC(b_+52, b_+55); SET_HL(SYM(treasureDisplayData1));
  for (;;) {
    CYC(b_+55, b_+56); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+56, b_+57); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+57, b_+59);
      break;
    }
    CYC(b_+57, b_+59);
    CYC(b_+59, b_+60); alu_cp(gb, D);
    if (F & FZ) {
      CYCT(b_+60, b_+62);
      break;
    }
    CYC(b_+60, b_+62);
    CYC(b_+62, b_+63); SET_HL(HL + 1);
    CYC(b_+63, b_+64); SET_HL(HL + 1);
    CYC(b_+64, b_+66);
  }
  CYC(b_+66, b_+68); alu_cp(gb, 0x13);
  if (!(F & FZ)) {
    CYCT(b_+68, b_+70);
  } else {
    CYC(b_+68, b_+70);
    CYC(b_+70, b_+73); A = mem_rd(gb, wSlingshotLevel);
    CYC(b_+73, b_+75); alu_cp(gb, 0x02);
    if (!(F & FZ)) {
      CYCT(b_+75, b_+77);
    } else {
      CYC(b_+75, b_+77);
      CYC(b_+77, b_+78); A = alu_inc8(gb, A);
      CYC(b_+78, b_+79); treasure_add_a_to_hl_from_rst(gb, b_+79);
    }
  }
  CYC(b_+79, b_+80); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+80, b_+81); E = mem_rd(gb, HL);
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+82, b_+84);
  } else {
    CYC(b_+82, b_+84);
    CYC(b_+84, b_+85); L = A;
    CYC(b_+85, b_+87); H = 0xc6;
    CYC(b_+87, b_+88); D = mem_rd(gb, HL);
  }
  CYC(b_+88, b_+89); ret_effect(gb);
}
