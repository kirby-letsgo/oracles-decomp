#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode0c), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode0c), (from), (to), true)

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
  BASE(partCode0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+7); // call z
    bridgeSpawner_state0_hook(gb);
  } else {
    CYC(b_+4, b_+7);
  }
  CALL_C(b_+7, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); L = 0xc9; // Part.angle
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+17); SET_HL(b_+73); // @tileValues
  CYC(b_+17, b_+18); bridgeSpawner_addDoubleIndexToHl_from_rst(gb, b_+18);
  CYC(b_+18, b_+20); E = 0xc7; // Part.counter2
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_rrca(gb);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FC)) { CYCT(b_+23, b_+25); goto L_4875; } // jr nc
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);

L_4875:
  CYC(b_+26, b_+27); B = A;
  CYC(b_+27, b_+29); E = 0xcb; // Part.yh
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); C = A;
  CYC(b_+31, b_+32); push_effect(gb, BC);
  CALL_C(b_+32, setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+35);
  CYC(b_+35, b_+36); SET_BC(pop_effect(gb));
  CYC(b_+36, b_+37); A = B;
  CALL_C(b_+37, setTile_hook, SYM(setTile), b_+40);
  CYC(b_+40, b_+42); A = 0x70; // SND_DOORCLOSE
  CALL_C(b_+42, playSound_b00_hook, SYM(playSound_b00), b_+45);
  CYC(b_+45, b_+46); H = D;
  CYC(b_+46, b_+48); L = 0xc6; // Part.counter1
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x08);
  CYC(b_+50, b_+51); L = alu_inc8(gb, L);
  CYC(b_+51, b_+52); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter2]
  if (F & FZ) { CYCT(b_+52, b_+55); partDelete_hook(gb); return; } // jp z
  CYC(b_+52, b_+55);
  CYC(b_+55, b_+56); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+56, b_+57); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+57); return; } // ret c
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); L = 0xc9; // Part.angle
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+64); SET_BC(b_+81); // @directionVals
  CALL_C(b_+64, addAToBc_hook, 0x006d, b_+67);
  CYC(b_+67, b_+68); A = mem_rd(gb, BC);
  CYC(b_+68, b_+70); L = 0xcb; // Part.yh
  CYC(b_+70, b_+71); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+71, b_+72); mem_wr(gb, HL, A);
  RET(b_+72); return; // ret
}

void bridgeSpawner_state0_hook(GB *gb) {
  BASE(partCode0c);
  CYC(b_+85, b_+86); H = D;
  CYC(b_+86, b_+87); L = E;
  CYC(b_+87, b_+88); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 1
  CYC(b_+88, b_+90); L = 0xc6; // Part.counter1
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x08);
  CYC(b_+92, SYM(partCode0e)); return; // ret
}
