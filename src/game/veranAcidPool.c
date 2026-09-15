#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t veranAcidPool_jump_table(GB *gb) {
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

// PART_VERAN_ACID_POOL
void partCode57_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d6c, 0x7d6e); E = 0xc4; // Part.state
  CYC(0x7d6e, 0x7d6f); A = mem_rd(gb, DE);
  {
    CYC(0x7d6f, 0x7d70); push_effect(gb, 0x7d70);
    uint16_t target = veranAcidPool_jump_table(gb);
    if (target == 0x7d9c) goto state1;
    if (target == 0x7dad) goto state2;
    if (target == 0x7dc7) goto state3;
    if (target == 0x7dcf) goto state4;
    if (target == 0x7dd6) goto state5;
    if (target == 0x7e10) goto state6;
    goto state0;
  }

state0:
  CALL_C(0x7d7e, objectCenterOnTile_hook, 0x20db, 0x7d81);
  CALL_C(0x7d81, objectGetShortPosition_hook, 0x2096, 0x7d84);
  CYC(0x7d84, 0x7d86); E = 0xf0; // Part.var30
  CYC(0x7d86, 0x7d87); mem_wr(gb, DE, A);
  CYC(0x7d87, 0x7d89); E = 0xc6; // Part.counter1
  CYC(0x7d89, 0x7d8b); A = 0x04;
  CYC(0x7d8b, 0x7d8c); mem_wr(gb, DE, A);
  CYC(0x7d8c, 0x7d8e); A = 0x68; // SND_UNKNOWN3
  CALL_C(0x7d8e, playSound_b00_hook, 0x0c98, 0x7d91);
  CYC(0x7d91, 0x7d94); SET_HL(0x7d98); // table_7d98
  CYC(0x7d94, 0x7d96); A = 0x60;
  CYC(0x7d96, 0x7d98); goto func_7de1; // jr

state1:
  CALL_C(0x7d9c, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7d9f);
  if (!(F & FZ)) { RET_TAKEN(0x7d9f); return; } // ret nz
  CYC(0x7d9f, 0x7da0);
  CYC(0x7da0, 0x7da2); mem_wr(gb, HL, 0x04);
  CYC(0x7da2, 0x7da5); SET_HL(0x7da9); // table_7da9
  CYC(0x7da5, 0x7da7); A = 0x60;
  CYC(0x7da7, 0x7da9); goto func_7de1; // jr

state2:
  CALL_C(0x7dad, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7db0);
  if (!(F & FZ)) { RET_TAKEN(0x7db0); return; } // ret nz
  CYC(0x7db0, 0x7db1);
  CYC(0x7db1, 0x7db3); mem_wr(gb, HL, 0x2d);
  CYC(0x7db3, 0x7db4); L = E;
  CYC(0x7db4, 0x7db5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7db5, 0x7db7); L = 0x60;

func_7db7:
  CYC(0x7db7, 0x7db9); E = 0xf0; // Part.var30
  CYC(0x7db9, 0x7dba); A = mem_rd(gb, DE);
  CYC(0x7dba, 0x7dbb); C = A;
  CYC(0x7dbb, 0x7dbd); B = 0xcf;
  CYC(0x7dbd, 0x7dbe); A = mem_rd(gb, BC);
  CYC(0x7dbe, 0x7dc0); alu_sub(gb, 0x02);
  CYC(0x7dc0, 0x7dc2); alu_cp(gb, 0x03);
  if (F & FC) { // ret c
    RET_TAKEN(0x7dc2);
    if (gb->pc == 0x7e19 && gb->sp == sp0_) goto state6_afterFunc7db7;
    return;
  }
  CYC(0x7dc2, 0x7dc3);
  CYC(0x7dc3, 0x7dc4); A = L;
  CYC(0x7dc4, 0x7dc7); setTile_hook(gb); // jp (own burn; local, multi-entry tail-chain)
  if (gb->pc == 0x7e19 && gb->sp == sp0_) goto state6_afterFunc7db7;
  return;

state3:
  CALL_C(0x7dc7, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7dca);
  if (!(F & FZ)) { RET_TAKEN(0x7dca); return; } // ret nz
  CYC(0x7dca, 0x7dcb);
  CYC(0x7dcb, 0x7dcd); mem_wr(gb, HL, 0x04);
  CYC(0x7dcd, 0x7dce); L = E;
  CYC(0x7dce, 0x7dcf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state4:
  CYC(0x7dcf, 0x7dd2); SET_HL(0x7da9); // table_7da9
  CYC(0x7dd2, 0x7dd4); A = 0xa0;
  CYC(0x7dd4, 0x7dd6); goto func_7de1; // jr

state5:
  CALL_C(0x7dd6, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7dd9);
  if (!(F & FZ)) { RET_TAKEN(0x7dd9); return; } // ret nz
  CYC(0x7dd9, 0x7dda);
  CYC(0x7dda, 0x7ddc); mem_wr(gb, HL, 0x04);
  CYC(0x7ddc, 0x7ddf); SET_HL(0x7d98); // table_7d98
  CYC(0x7ddf, 0x7de1); A = 0xa0;

func_7de1:
  CYC(0x7de1, 0x7de3); mem_wr(gb, 0xff8b, A); // hFF8B
  CYC(0x7de3, 0x7de5); E = 0xf0; // Part.var30
  CYC(0x7de5, 0x7de6); A = mem_rd(gb, DE);
  CYC(0x7de6, 0x7de7); C = A;
  CYC(0x7de7, 0x7de9); B = 0x04;

L_7de9:
  CYC(0x7de9, 0x7dea); push_effect(gb, BC);
  CYC(0x7dea, 0x7deb); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7deb, 0x7dec); alu_add(gb, C);
  CYC(0x7dec, 0x7ded); C = A;
  CYC(0x7ded, 0x7def); B = 0xcf;
  CYC(0x7def, 0x7df0); A = mem_rd(gb, BC);
  CYC(0x7df0, 0x7df2); alu_cp(gb, 0xda);
  if (F & FZ) { CYCT(0x7df2, 0x7df4); goto L_7e00; } // jr z
  CYC(0x7df2, 0x7df4);
  CYC(0x7df4, 0x7df6); alu_sub(gb, 0x02);
  CYC(0x7df6, 0x7df8); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x7df8, 0x7dfa); goto L_7e07; } // jr c
  CYC(0x7df8, 0x7dfa);
  CYC(0x7dfa, 0x7dfc); B = 0xce;
  CYC(0x7dfc, 0x7dfd); A = mem_rd(gb, BC);
  CYC(0x7dfd, 0x7dfe); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7dfe, 0x7e00); goto L_7e07; } // jr nz
  CYC(0x7dfe, 0x7e00);

L_7e00:
  CYC(0x7e00, 0x7e02); A = mem_rd(gb, 0xff8b);
  CYC(0x7e02, 0x7e03); push_effect(gb, HL);
  CALL_C(0x7e03, setTile_hook, 0x3a9c, 0x7e06);
  CYC(0x7e06, 0x7e07); SET_HL(pop_effect(gb));

L_7e07:
  CYC(0x7e07, 0x7e08); SET_BC(pop_effect(gb));
  CYC(0x7e08, 0x7e09); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x7e09, 0x7e0b); goto L_7de9; } // jr nz
  CYC(0x7e09, 0x7e0b);
  CYC(0x7e0b, 0x7e0c); H = D;
  CYC(0x7e0c, 0x7e0e); L = 0xc4; // Part.state
  CYC(0x7e0e, 0x7e0f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x7e0f); return; // ret

state6:
  CALL_C(0x7e10, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7e13);
  if (!(F & FZ)) { RET_TAKEN(0x7e13); return; } // ret nz
  CYC(0x7e13, 0x7e14);
  CYC(0x7e14, 0x7e16); L = 0xa0;
  CYC(0x7e16, 0x7e19); push_effect(gb, 0x7e19); goto func_7db7; // call $7db7 (own burn; local, inlined)

state6_afterFunc7db7:
  CYC(0x7e19, 0x7e1c); partDelete_hook(gb); return; // jp
}
