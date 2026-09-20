#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode08_hook(GB *gb);
void darkRoomHandler_state0_hook(GB *gb);
void darkRoomHandler_spawnLightableTorch_hook(GB *gb);

void partCode08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46a8, 0x46ab); A = mem_rd(gb, 0xc4ab); // wPaletteThread_mode
  CYC(0x46ab, 0x46ac); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x46ac); return; } // ret nz
  CYC(0x46ac, 0x46ad);
  CYC(0x46ad, 0x46b0); A = mem_rd(gb, 0xcd00); // wScrollMode
  CYC(0x46b0, 0x46b2); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(0x46b2); return; } // ret z
  CYC(0x46b2, 0x46b3);
  CYC(0x46b3, 0x46b5); E = 0xc4; // Part.state
  CYC(0x46b5, 0x46b6); A = mem_rd(gb, DE);
  CYC(0x46b6, 0x46b7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x46b7, 0x46ba); // call z
    darkRoomHandler_state0_hook(gb);
  } else {
    CYC(0x46b7, 0x46ba);
  }

  CYC(0x46ba, 0x46bb); H = D;
  CYC(0x46bb, 0x46bd); L = 0xc7; // Part.counter2
  CYC(0x46bd, 0x46be); B = mem_rd(gb, HL);
  CYC(0x46be, 0x46c1); A = mem_rd(gb, 0xcc8f); // wNumTorchesLit
  CYC(0x46c1, 0x46c2); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x46c2); return; } // ret z
  CYC(0x46c2, 0x46c3);
  CYC(0x46c3, 0x46c4); mem_wr(gb, HL, A); SET_HL(HL - 1); // [counter2]
  CYC(0x46c4, 0x46c5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x46c5, 0x46c8); darkenRoom_hook(gb); return; } // jp z
  CYC(0x46c5, 0x46c8);
  CYC(0x46c8, 0x46c9); alu_cp(gb, mem_rd(gb, HL)); // [counter1]
  if (F & FZ) { CYCT(0x46c9, 0x46cc); brightenRoom_hook(gb); return; } // jp z
  CYC(0x46c9, 0x46cc);
  CYC(0x46cc, 0x46cf); A = mem_rd(gb, 0xc4ae); // wPaletteThread_parameter
  CYC(0x46cf, 0x46d1); alu_cp(gb, 0xf7);
  if (F & FZ) { RET_TAKEN(0x46d1); return; } // ret z
  CYC(0x46d1, 0x46d2);
  CYC(0x46d2, 0x46d5); A = mem_rd(gb, 0xcc8f); // wNumTorchesLit
  CYC(0x46d5, 0x46d6); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(0x46d6, 0x46d9); brightenRoomLightly_hook(gb); return; } // jp nc
  CYC(0x46d6, 0x46d9);
  CYC(0x46d9, 0x46dc); darkenRoomLightly_hook(gb); return; // jp
}

void darkRoomHandler_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4704, 0x4705); A = alu_inc8(gb, A);
  CYC(0x4705, 0x4706); mem_wr(gb, DE, A); // [state] = 1
  CYC(0x4706, 0x4708); E = 0xc6; // Part.counter1
  CYC(0x4708, 0x4709); A = mem_rd(gb, DE);
  CYC(0x4709, 0x470a); C = A;
  CYC(0x470a, 0x470d); SET_HL(0xcf00); // wRoomLayout
  CYC(0x470d, 0x470f); B = 0xb0; // LARGE_ROOM_HEIGHT << 4

L_470f:
  CYC(0x470f, 0x4710); A = mem_rd(gb, HL);
  CYC(0x4710, 0x4712); alu_cp(gb, 0x08); // TILEINDEX_UNLIT_TORCH
  if (F & FZ) {
    CYCT(0x4712, 0x4715); // call z
    darkRoomHandler_spawnLightableTorch_hook(gb);
  } else {
    CYC(0x4712, 0x4715);
  }
  CYC(0x4715, 0x4716); L = alu_inc8(gb, L);
  CYC(0x4716, 0x4717); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x4717, 0x4719); goto L_470f; } // jr nz
  CYC(0x4717, 0x4719);

  CYC(0x4719, 0x471b); E = 0xc6; // Part.counter1
  CYC(0x471b, 0x471c); A = C;
  CYC(0x471c, 0x471d); mem_wr(gb, DE, A);
  CALL_C(0x471d, objectGetShortPosition_hook, 0x2096, 0x4720);
  CYC(0x4720, 0x4722); E = 0xcb; // Part.yh
  CYC(0x4722, 0x4723); mem_wr(gb, DE, A);
  CYC(0x4723, 0x4724); return; // ret
}

void darkRoomHandler_spawnLightableTorch_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46dc, 0x46dd); push_effect(gb, (uint16_t)((H << 8) | L));
  CYC(0x46dd, 0x46de); push_effect(gb, BC);
  CYC(0x46de, 0x46df); C = L;
  CALL_C(0x46df, getFreePartSlot_hook, 0x3e8e, 0x46e2);
  if (!(F & FZ)) { CYCT(0x46e2, 0x46e4); goto L_4700; } // jr nz
  CYC(0x46e2, 0x46e4);
  CYC(0x46e4, 0x46e6); mem_wr(gb, HL, 0x06); // PART_LIGHTABLE_TORCH
  CYC(0x46e6, 0x46e7); L = alu_inc8(gb, L);
  CYC(0x46e7, 0x46e8); E = L;
  CYC(0x46e8, 0x46e9); A = mem_rd(gb, DE);
  CYC(0x46e9, 0x46ea); mem_wr(gb, HL, A); // [child.subid] = [this.subid]
  CYC(0x46ea, 0x46ec); E = 0xcb; // Part.yh
  CYC(0x46ec, 0x46ed); A = mem_rd(gb, DE);
  CYC(0x46ed, 0x46ef); alu_and(gb, 0xf0);
  CYC(0x46ef, 0x46f0); L = A;
  CYC(0x46f0, 0x46f2); E = 0xcd; // Part.xh
  CYC(0x46f2, 0x46f3); A = mem_rd(gb, DE);
  CYC(0x46f3, 0x46f5); alu_and(gb, 0xf0);
  CYC(0x46f5, 0x46f7); A = alu_swap(gb, A);
  CYC(0x46f7, 0x46f8); alu_or(gb, L);
  CYC(0x46f8, 0x46fa); L = 0xc7; // Part.counter2
  CYC(0x46fa, 0x46fb); mem_wr(gb, HL, A);
  CYC(0x46fb, 0x46fd); L = 0xcb; // Part.yh
  CALL_C(0x46fd, setShortPosition_paramC_hook, 0x20b9, 0x4700);

L_4700:
  CYC(0x4700, 0x4701); SET_BC(pop_effect(gb));
  CYC(0x4701, 0x4702); SET_HL(pop_effect(gb));
  CYC(0x4702, 0x4703); C = alu_inc8(gb, C);
  CYC(0x4703, 0x4704); return; // ret
}
