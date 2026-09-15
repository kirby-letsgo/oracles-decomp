#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t owlStatue_jump_table(GB *gb) {
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

static void owlStatue_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4c94, 0x4c96); goto normalStatus; } // jr z
  CYC(0x4c94, 0x4c96);
  CYC(0x4c96, 0x4c98); E = 0xea; // Part.var2a
  CYC(0x4c98, 0x4c99); A = mem_rd(gb, DE);
  CYC(0x4c99, 0x4c9b); alu_cp(gb, 0x9a);
  if (!(F & FZ)) { CYCT(0x4c9b, 0x4c9d); goto normalStatus; } // jr nz
  CYC(0x4c9b, 0x4c9d);
  CYC(0x4c9d, 0x4c9e); H = D;
  CYC(0x4c9e, 0x4ca0); L = 0xc4; // Part.state
  CYC(0x4ca0, 0x4ca1); A = mem_rd(gb, HL);
  CYC(0x4ca1, 0x4ca3); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x4ca3, 0x4ca5); goto normalStatus; } // jr nc
  CYC(0x4ca3, 0x4ca5);
  CYC(0x4ca5, 0x4ca6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4ca6, 0x4ca8); L = 0xc6; // Part.counter1
  CYC(0x4ca8, 0x4caa); mem_wr(gb, HL, 0x32);

normalStatus:
  CYC(0x4caa, 0x4cac); E = 0xc4; // Part.state
  CYC(0x4cac, 0x4cad); A = mem_rd(gb, DE);
  {
    CYC(0x4cad, 0x4cae); push_effect(gb, 0x4cae);
    uint16_t target = owlStatue_jump_table(gb);
    if (target == 0x4cb6) goto state0;
    if (target == 0x4cc7) goto stateStub;
    if (target == 0x4cc8) goto state2;
    goto state3;
  }

state0:
  CYC(0x4cb6, 0x4cb7); H = D;
  CYC(0x4cb7, 0x4cb8); L = E;
  CYC(0x4cb8, 0x4cb9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4cb9, 0x4cbb); L = 0xff; // Part.var3f
  CYC(0x4cbb, 0x4cbd); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
  CALL_C(0x4cbd, objectMakeTileSolid_hook, 0x20b2, 0x4cc0);
  CYC(0x4cc0, 0x4cc2); H = 0xcf; // >wRoomLayout
  CYC(0x4cc2, 0x4cc4); mem_wr(gb, HL, 0x00);
  CYC(0x4cc4, 0x4cc7); objectSetVisible83_hook(gb); return; // jp

stateStub:
  RET(0x4cc7); return; // ret

state2:
  CALL_C(0x4cc8, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x4ccb);
  if (!(F & FZ)) { CYCT(0x4ccb, 0x4ccd); goto L_4cd6; } // jr nz
  CYC(0x4ccb, 0x4ccd);
  CYC(0x4ccd, 0x4ccf); mem_wr(gb, HL, 0x1e);
  CYC(0x4ccf, 0x4cd0); L = E;
  CYC(0x4cd0, 0x4cd1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4cd1, 0x4cd3); A = 0x01;
  CYC(0x4cd3, 0x4cd6); partSetAnimation_hook(gb); return; // jp

L_4cd6:
  CYC(0x4cd6, 0x4cd7); A = mem_rd(gb, HL);
  CYC(0x4cd7, 0x4cd9); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x4cd9); return; } // ret nz
  CYC(0x4cd9, 0x4cda);
  CYC(0x4cda, 0x4cdb); A = mem_rd(gb, HL);
  CYC(0x4cdb, 0x4cdc); alu_rrca(gb);
  CYC(0x4cdc, 0x4cdd); alu_rrca(gb);
  CYC(0x4cdd, 0x4cdf); alu_sub(gb, 0x02);
  CYC(0x4cdf, 0x4ce2); SET_HL(0x4cef); // @owlStatueSparkleOffset
  CYC(0x4ce2, 0x4ce3); owlStatue_addAToHl_from_rst(gb, 0x4ce3);
  CYC(0x4ce3, 0x4ce4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4ce4, 0x4ce5); B = A;
  CYC(0x4ce5, 0x4ce6); C = mem_rd(gb, HL);
  CALL_C(0x4ce6, getFreeInteractionSlot_hook, 0x3aef, 0x4ce9);
  if (!(F & FZ)) { RET_TAKEN(0x4ce9); return; } // ret nz
  CYC(0x4ce9, 0x4cea);
  CYC(0x4cea, 0x4cec); mem_wr(gb, HL, 0x84); // INTERAC_SPARKLE
  CYC(0x4cec, 0x4cef); objectCopyPositionWithOffset_hook(gb); return; // jp

state3:
  CALL_C(0x4cfb, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x4cfe);
  if (!(F & FZ)) { CYCT(0x4cfe, 0x4d00); goto L_4d07; } // jr nz
  CYC(0x4cfe, 0x4d00);
  CYC(0x4d00, 0x4d01); L = E;
  CYC(0x4d01, 0x4d03); mem_wr(gb, HL, 0x01);
  CYC(0x4d03, 0x4d04); alu_xor(gb, A);
  CYC(0x4d04, 0x4d07); partSetAnimation_hook(gb); return; // jp

L_4d07:
  CYC(0x4d07, 0x4d08); A = mem_rd(gb, HL);
  CYC(0x4d08, 0x4d0a); alu_cp(gb, 0x16);
  if (!(F & FZ)) { RET_TAKEN(0x4d0a); return; } // ret nz
  CYC(0x4d0a, 0x4d0b);
  CYC(0x4d0b, 0x4d0d); L = 0xc2; // Part.subid
  CYC(0x4d0d, 0x4d0e); C = mem_rd(gb, HL);
  CYC(0x4d0e, 0x4d10); B = 0x39;
  CYC(0x4d10, 0x4d13); showText_hook(gb); return; // jp
}
