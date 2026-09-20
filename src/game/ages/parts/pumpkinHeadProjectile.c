#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t pumpkinHeadProjectile_jump_table(GB *gb) {
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

static void pumpkinHeadProjectile_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_PUMPKIN_HEAD_PROJECTILE
void partCode42_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (!(F & FZ)) { CYCT(0x73ba, 0x73bd); partDelete_hook(gb); return; } // jp nz
  CYC(0x73ba, 0x73bd);
  CYC(0x73bd, 0x73bf); E = 0xc4; // Part.state
  CYC(0x73bf, 0x73c0); A = mem_rd(gb, DE);
  {
    CYC(0x73c0, 0x73c1); push_effect(gb, 0x73c1);
    uint16_t target = pumpkinHeadProjectile_jump_table(gb);
    if (target == 0x742c) goto state1;
    if (target == 0x7436) goto state2;
    goto state0;
  }

state0:
  CYC(0x73c7, 0x73c8); H = D;
  CYC(0x73c8, 0x73c9); L = E;
  CYC(0x73c9, 0x73ca); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x73ca, 0x73cc); L = 0xc6; // Part.counter1
  CYC(0x73cc, 0x73ce); mem_wr(gb, HL, 0x08);
  CYC(0x73ce, 0x73d0); L = 0xd0; // Part.speed
  CYC(0x73d0, 0x73d2); mem_wr(gb, HL, 0x3c); // SPEED_0f0
  CYC(0x73d2, 0x73d4); E = 0xcb; // Part.yh
  CYC(0x73d4, 0x73d6); L = 0xcf; // Part.zh
  CYC(0x73d6, 0x73d7); A = mem_rd(gb, DE);
  CYC(0x73d7, 0x73d8); alu_add(gb, mem_rd(gb, HL));
  CYC(0x73d8, 0x73d9); mem_wr(gb, DE, A);
  CYC(0x73d9, 0x73db); mem_wr(gb, HL, 0x00);
  CYC(0x73db, 0x73dd); E = 0xc2; // Part.subid
  CYC(0x73dd, 0x73de); A = mem_rd(gb, DE);
  CYC(0x73de, 0x73e1); SET_BC(0x7421); // table_7421
  CALL_C(0x73e1, addAToBc_hook, 0x006d, 0x73e4);
  CYC(0x73e4, 0x73e6); L = 0xc9; // Part.angle
  CYC(0x73e6, 0x73e7); A = mem_rd(gb, BC);
  CYC(0x73e7, 0x73e8); alu_add(gb, mem_rd(gb, HL));
  CYC(0x73e8, 0x73ea); alu_and(gb, 0x1f);
  CYC(0x73ea, 0x73eb); mem_wr(gb, HL, A);
  CYC(0x73eb, 0x73ec); A = mem_rd(gb, DE);
  CYC(0x73ec, 0x73ed); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x73ed, 0x73ef); goto L_7417; } // jr nz
  CYC(0x73ed, 0x73ef);
  CYC(0x73ef, 0x73f0); A = mem_rd(gb, HL);
  CYC(0x73f0, 0x73f1); alu_rrca(gb);
  CYC(0x73f1, 0x73f2); alu_rrca(gb);
  CYC(0x73f2, 0x73f5); SET_HL(0x7424); // table_7424
  CYC(0x73f5, 0x73f6); pumpkinHeadProjectile_addAToHl_from_rst(gb, 0x73f6);
  CYC(0x73f6, 0x73f8); E = 0xcb; // Part.yh
  CYC(0x73f8, 0x73f9); A = mem_rd(gb, DE);
  CYC(0x73f9, 0x73fa); alu_add(gb, mem_rd(gb, HL));
  CYC(0x73fa, 0x73fb); mem_wr(gb, DE, A);
  CYC(0x73fb, 0x73fd); E = 0xcd; // Part.xh
  CYC(0x73fd, 0x73fe); SET_HL(HL + 1); // inc hl
  CYC(0x73fe, 0x73ff); A = mem_rd(gb, DE);
  CYC(0x73ff, 0x7400); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7400, 0x7401); mem_wr(gb, DE, A);
  CYC(0x7401, 0x7403); B = 0x02;

L_7403:
  CALL_C(0x7403, getFreePartSlot_hook, 0x3e8e, 0x7406);
  if (!(F & FZ)) { CYCT(0x7406, 0x7408); goto L_7417; } // jr nz
  CYC(0x7406, 0x7408);
  CYC(0x7408, 0x740a); mem_wr(gb, HL, 0x42); // PART_PUMPKIN_HEAD_PROJECTILE
  CYC(0x740a, 0x740b); L = alu_inc8(gb, L);
  CYC(0x740b, 0x740c); mem_wr(gb, HL, B);
  CYC(0x740c, 0x740e); L = 0xc9; // Part.angle
  CYC(0x740e, 0x740f); E = L;
  CYC(0x740f, 0x7410); A = mem_rd(gb, DE);
  CYC(0x7410, 0x7411); mem_wr(gb, HL, A);
  CALL_C(0x7411, objectCopyPosition_hook, 0x2242, 0x7414);
  CYC(0x7414, 0x7415); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x7415, 0x7417); goto L_7403; } // jr nz
  CYC(0x7415, 0x7417);

L_7417:
  CYC(0x7417, 0x7419); E = 0xc9; // Part.angle
  CYC(0x7419, 0x741a); A = mem_rd(gb, DE);
  CYC(0x741a, 0x741b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x741b, 0x741e); objectSetVisible82_hook(gb); return; } // jp z
  CYC(0x741b, 0x741e);
  CYC(0x741e, 0x7421); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(0x742c, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x742f);
  if (!(F & FZ)) { CYCT(0x742f, 0x7431); goto state2; } // jr nz
  CYC(0x742f, 0x7431);
  CYC(0x7431, 0x7432); L = E;
  CYC(0x7432, 0x7433); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x7433, objectSetVisible82_hook, 0x1e69, 0x7436);

state2:
  CALL_C(0x7436, partAnimate_hook, 0x2978, 0x7439);
  CALL_C(0x7439, objectApplySpeed_hook, 0x201d, 0x743c);
  CALL_C(0x743c, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x743f);
  if (!(F & FC)) { RET_TAKEN(0x743f); return; } // ret nc
  CYC(0x743f, 0x7440);
  CYC(0x7440, 0x7443); partDelete_hook(gb); return; // jp
}
