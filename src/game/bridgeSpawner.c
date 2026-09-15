#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static void bridgeSpawner_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void partCode0c_hook(GB *gb);
void bridgeSpawner_state0_hook(GB *gb);

void partCode0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x485b, 0x485d); E = 0xc4; // Part.state
  CYC(0x485d, 0x485e); A = mem_rd(gb, DE);
  CYC(0x485e, 0x485f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x485f, 0x4862); // call z
    bridgeSpawner_state0_hook(gb);
  } else {
    CYC(0x485f, 0x4862);
  }
  CALL_C(0x4862, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x4865);
  if (!(F & FZ)) { RET_TAKEN(0x4865); return; } // ret nz
  CYC(0x4865, 0x4866);
  CYC(0x4866, 0x4868); L = 0xc9; // Part.angle
  CYC(0x4868, 0x4869); A = mem_rd(gb, HL);
  CYC(0x4869, 0x486c); SET_HL(0x48a4); // @tileValues
  CYC(0x486c, 0x486d); bridgeSpawner_addDoubleIndexToHl_from_rst(gb, 0x486d);
  CYC(0x486d, 0x486f); E = 0xc7; // Part.counter2
  CYC(0x486f, 0x4870); A = mem_rd(gb, DE);
  CYC(0x4870, 0x4871); alu_rrca(gb);
  CYC(0x4871, 0x4872); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FC)) { CYCT(0x4872, 0x4874); goto L_4875; } // jr nc
  CYC(0x4872, 0x4874);
  CYC(0x4874, 0x4875); A = mem_rd(gb, HL);

L_4875:
  CYC(0x4875, 0x4876); B = A;
  CYC(0x4876, 0x4878); E = 0xcb; // Part.yh
  CYC(0x4878, 0x4879); A = mem_rd(gb, DE);
  CYC(0x4879, 0x487a); C = A;
  CYC(0x487a, 0x487b); push_effect(gb, BC);
  CALL_C(0x487b, setTileInRoomLayoutBuffer_hook, 0x1426, 0x487e);
  CYC(0x487e, 0x487f); SET_BC(pop_effect(gb));
  CYC(0x487f, 0x4880); A = B;
  CALL_C(0x4880, setTile_hook, 0x3a9c, 0x4883);
  CYC(0x4883, 0x4885); A = 0x70; // SND_DOORCLOSE
  CALL_C(0x4885, playSound_b00_hook, 0x0c98, 0x4888);
  CYC(0x4888, 0x4889); H = D;
  CYC(0x4889, 0x488b); L = 0xc6; // Part.counter1
  CYC(0x488b, 0x488d); mem_wr(gb, HL, 0x08);
  CYC(0x488d, 0x488e); L = alu_inc8(gb, L);
  CYC(0x488e, 0x488f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter2]
  if (F & FZ) { CYCT(0x488f, 0x4892); partDelete_hook(gb); return; } // jp z
  CYC(0x488f, 0x4892);
  CYC(0x4892, 0x4893); A = mem_rd(gb, HL); // [counter1]
  CYC(0x4893, 0x4894); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x4894); return; } // ret c
  CYC(0x4894, 0x4895);
  CYC(0x4895, 0x4897); L = 0xc9; // Part.angle
  CYC(0x4897, 0x4898); A = mem_rd(gb, HL);
  CYC(0x4898, 0x489b); SET_BC(0x48ac); // @directionVals
  CALL_C(0x489b, addAToBc_hook, 0x006d, 0x489e);
  CYC(0x489e, 0x489f); A = mem_rd(gb, BC);
  CYC(0x489f, 0x48a1); L = 0xcb; // Part.yh
  CYC(0x48a1, 0x48a2); alu_add(gb, mem_rd(gb, HL));
  CYC(0x48a2, 0x48a3); mem_wr(gb, HL, A);
  RET(0x48a3); return; // ret
}

void bridgeSpawner_state0_hook(GB *gb) {
  CYC(0x48b0, 0x48b1); H = D;
  CYC(0x48b1, 0x48b2); L = E;
  CYC(0x48b2, 0x48b3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 1
  CYC(0x48b3, 0x48b5); L = 0xc6; // Part.counter1
  CYC(0x48b5, 0x48b7); mem_wr(gb, HL, 0x08);
  CYC(0x48b7, 0x48b8); return; // ret
}
