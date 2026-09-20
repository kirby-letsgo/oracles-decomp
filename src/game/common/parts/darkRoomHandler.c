#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode08_hook(GB *gb);
void darkRoomHandler_state0_hook(GB *gb);
void darkRoomHandler_spawnLightableTorch_hook(GB *gb);

void partCode08_hook(GB *gb) {
  BASE(partCode08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode); // wPaletteThread_mode
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wScreenVariables); // wScrollMode
  CYC(b_+8, b_+10); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+10); return; } // ret z
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = 0xc4; // Part.state
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+15, b_+18); // call z
    darkRoomHandler_state0_hook(gb);
  } else {
    CYC(b_+15, b_+18);
  }

  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = 0xc7; // Part.counter2
  CYC(b_+21, b_+22); B = mem_rd(gb, HL);
  CYC(b_+22, b_+25); A = mem_rd(gb, wNumTorchesLit); // wNumTorchesLit
  CYC(b_+25, b_+26); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+26); return; } // ret z
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL - 1); // [counter2]
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+29, b_+32); darkenRoom_hook(gb); return; } // jp z
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+33); alu_cp(gb, mem_rd(gb, HL)); // [counter1]
  if (F & FZ) { CYCT(b_+33, b_+36); brightenRoom_hook(gb); return; } // jp z
  CYC(b_+33, b_+36);
  CYC(b_+36, b_+39); A = mem_rd(gb, wPaletteThread_parameter); // wPaletteThread_parameter
  CYC(b_+39, b_+41); alu_cp(gb, 0xf7);
  if (F & FZ) { RET_TAKEN(b_+41); return; } // ret z
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wNumTorchesLit); // wNumTorchesLit
  CYC(b_+45, b_+46); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+46, b_+49); brightenRoomLightly_hook(gb); return; } // jp nc
  CYC(b_+46, b_+49);
  CYC(b_+49, b_+52); darkenRoomLightly_hook(gb); return; // jp
}

void darkRoomHandler_state0_hook(GB *gb) {
  BASE(partCode08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+92, b_+93); A = alu_inc8(gb, A);
  CYC(b_+93, b_+94); mem_wr(gb, DE, A); // [state] = 1
  CYC(b_+94, b_+96); E = 0xc6; // Part.counter1
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); C = A;
  CYC(b_+98, b_+101); SET_HL(wRoomLayout); // wRoomLayout
  CYC(b_+101, b_+103); B = 0xb0; // LARGE_ROOM_HEIGHT << 4

L_470f:
  CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+106); alu_cp(gb, 0x08); // TILEINDEX_UNLIT_TORCH
  if (F & FZ) {
    CYCT(b_+106, b_+109); // call z
    darkRoomHandler_spawnLightableTorch_hook(gb);
  } else {
    CYC(b_+106, b_+109);
  }
  CYC(b_+109, b_+110); L = alu_inc8(gb, L);
  CYC(b_+110, b_+111); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+111, b_+113); goto L_470f; } // jr nz
  CYC(b_+111, b_+113);

  CYC(b_+113, b_+115); E = 0xc6; // Part.counter1
  CYC(b_+115, b_+116); A = C;
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CALL_C(b_+117, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+120);
  CYC(b_+120, b_+122); E = 0xcb; // Part.yh
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  CYC(b_+123, b_+124); return; // ret
}

void darkRoomHandler_spawnLightableTorch_hook(GB *gb) {
  BASE(partCode08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+52, b_+53); push_effect(gb, (uint16_t)((H << 8) | L));
  CYC(b_+53, b_+54); push_effect(gb, BC);
  CYC(b_+54, b_+55); C = L;
  CALL_C(b_+55, getFreePartSlot_hook, SYM(getFreePartSlot), b_+58);
  if (!(F & FZ)) { CYCT(b_+58, b_+60); goto L_4700; } // jr nz
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x06); // PART_LIGHTABLE_TORCH
  CYC(b_+62, b_+63); L = alu_inc8(gb, L);
  CYC(b_+63, b_+64); E = L;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); mem_wr(gb, HL, A); // [child.subid] = [this.subid]
  CYC(b_+66, b_+68); E = 0xcb; // Part.yh
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+71); alu_and(gb, 0xf0);
  CYC(b_+71, b_+72); L = A;
  CYC(b_+72, b_+74); E = 0xcd; // Part.xh
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+77); alu_and(gb, 0xf0);
  CYC(b_+77, b_+79); A = alu_swap(gb, A);
  CYC(b_+79, b_+80); alu_or(gb, L);
  CYC(b_+80, b_+82); L = 0xc7; // Part.counter2
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
  CYC(b_+83, b_+85); L = 0xcb; // Part.yh
  CALL_C(b_+85, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+88);

L_4700:
  CYC(b_+88, b_+89); SET_BC(pop_effect(gb));
  CYC(b_+89, b_+90); SET_HL(pop_effect(gb));
  CYC(b_+90, b_+91); C = alu_inc8(gb, C);
  CYC(b_+91, b_+92); return; // ret
}
