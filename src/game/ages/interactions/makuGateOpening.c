#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode76_jump_table(GB *gb) {
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

// INTERAC_MAKU_GATE_OPENING
void interactionCode76_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f02, 0x5f04); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5f04, 0x5f05); A = mem_rd(gb, DE);
  {
    CYC(0x5f05, 0x5f06); push_effect(gb, 0x5f06);
    interactionCode76_jump_table(gb); // subid0 and subid1 both target 0x5f0a
  }

  // interactionCode76@subid0 / @subid1
  CYC(0x5f0a, 0x5f0c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5f0c, 0x5f0d); A = mem_rd(gb, DE);
  {
    CYC(0x5f0d, 0x5f0e); push_effect(gb, 0x5f0e);
    uint16_t target = interactionCode76_jump_table(gb);
    if (target == 0x5f3d) goto state1;
    if (target == 0x5f59) goto state2;
    if (target == 0x5f75) goto state3;
  }

  // interactionCode76@state0
  CYC(0x5f16, 0x5f17); H = D;
  CYC(0x5f17, 0x5f19); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5f19, 0x5f1a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x5f1a, 0x5f1c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5f1c, 0x5f1e); mem_wr(gb, HL, 30);
  CYC(0x5f1e, 0x5f20); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5f20, 0x5f21); A = mem_rd(gb, HL);
  CYC(0x5f21, 0x5f22); alu_or(gb, A);
  CYC(0x5f22, 0x5f25); SET_HL(0x5f90); // @frame0And1_subid0
  if (F & FZ) { CYCT(0x5f25, 0x5f27); goto l5f2a; } // jr z
  CYC(0x5f25, 0x5f27);
  CYC(0x5f27, 0x5f2a); SET_HL(0x5f99); // @frame0And1_subid1

l5f2a:
  CYC(0x5f2a, 0x5f2d); push_effect(gb, 0x5f2d); goto loadInterleavedTiles; // call
after1_loadInterleavedTiles:
  CYC(0x5f2d, 0x5f30); SET_BC(0x5fab); // @frame0_poof
  CYC(0x5f30, 0x5f33); push_effect(gb, 0x5f33); goto loadPoofs; // call
after1_loadPoofs:
  // falls through into shakeScreen (no push_effect for this path)

shakeScreen:
  CYC(0x5f33, 0x5f35); A = 0x06;
  CALL_C(0x5f35, setScreenShakeCounter_hook, 0x24bb, 0x5f38);
  CYC(0x5f38, 0x5f3a); A = 0x70; // SND_DOORCLOSE
  CYC(0x5f3a, 0x5f3d); playSound_b00_hook(gb);
  if (gb->pc == 0x5f50 && gb->sp == sp0_) goto after1_shakeScreen;
  if (gb->pc == 0x5f6c && gb->sp == sp0_) goto after2_shakeScreen;
  if (gb->pc == 0x5f88 && gb->sp == sp0_) goto after3_shakeScreen;
  return; // jp

state1:
  CALL_C(0x5f3d, interactionDecCounter1_hook, 0x23cc, 0x5f40);
  if (!(F & FZ)) { RET_TAKEN(0x5f40); return; } // ret nz
  CYC(0x5f40, 0x5f41);
  CYC(0x5f41, 0x5f44); SET_HL(0x5f90); // @frame0And1_subid0
  CYC(0x5f44, 0x5f47); push_effect(gb, 0x5f47); goto loadTiles; // call
after1_loadTiles:
  CYC(0x5f47, 0x5f4a); SET_BC(0x5fb4); // @frame1_poof
  CYC(0x5f4a, 0x5f4d); push_effect(gb, 0x5f4d); goto loadPoofs; // call
after2_loadPoofs:
  CYC(0x5f4d, 0x5f50); push_effect(gb, 0x5f50); goto shakeScreen; // call
after1_shakeScreen:
  CYC(0x5f50, 0x5f51); H = D;
  CYC(0x5f51, 0x5f53); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5f53, 0x5f54); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x5f54, 0x5f56); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5f56, 0x5f58); mem_wr(gb, HL, 30);
  RET(0x5f58); return; // ret

state2:
  CALL_C(0x5f59, interactionDecCounter1_hook, 0x23cc, 0x5f5c);
  if (!(F & FZ)) { RET_TAKEN(0x5f5c); return; } // ret nz
  CYC(0x5f5c, 0x5f5d);
  CYC(0x5f5d, 0x5f60); SET_HL(0x5fa2); // @frame2And3
  CYC(0x5f60, 0x5f63); push_effect(gb, 0x5f63); goto loadInterleavedTiles; // call
after2_loadInterleavedTiles:
  CYC(0x5f63, 0x5f66); SET_BC(0x5fbd); // @frame2_poof
  CYC(0x5f66, 0x5f69); push_effect(gb, 0x5f69); goto loadPoofs; // call
after3_loadPoofs:
  CYC(0x5f69, 0x5f6c); push_effect(gb, 0x5f6c); goto shakeScreen; // call
after2_shakeScreen:
  CYC(0x5f6c, 0x5f6d); H = D;
  CYC(0x5f6d, 0x5f6f); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5f6f, 0x5f70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x5f70, 0x5f72); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5f72, 0x5f74); mem_wr(gb, HL, 30);
  RET(0x5f74); return; // ret

state3:
  CALL_C(0x5f75, interactionDecCounter1_hook, 0x23cc, 0x5f78);
  if (!(F & FZ)) { RET_TAKEN(0x5f78); return; } // ret nz
  CYC(0x5f78, 0x5f79);
  CYC(0x5f79, 0x5f7c); SET_HL(0x5fa2); // @frame2And3
  CYC(0x5f7c, 0x5f7f); push_effect(gb, 0x5f7f); goto loadTiles; // call
after2_loadTiles:
  CYC(0x5f7f, 0x5f82); SET_BC(0x5fc6); // @frame3_poof
  CYC(0x5f82, 0x5f85); push_effect(gb, 0x5f85); goto loadPoofs; // call
after4_loadPoofs:
  CYC(0x5f85, 0x5f88); push_effect(gb, 0x5f88); goto shakeScreen; // call
after3_shakeScreen:
  CALL_C(0x5f88, getThisRoomFlags_hook, 0x197d, 0x5f8b);
  CYC(0x5f8b, 0x5f8d); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(0x5f8d, 0x5f90); interactionDelete_hook(gb); return; // jp

// @param hl Pointer to data
loadInterleavedTiles:
  CYC(0x5fcf, 0x5fd0); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5fd0, 0x5fd1); B = A;

next_loadInterleavedTiles:
  CYC(0x5fd1, 0x5fd2); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5fd2, 0x5fd4); H8(hFF8C) = A;
  CYC(0x5fd4, 0x5fd5); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5fd5, 0x5fd7); H8(hFF8F) = A;
  CYC(0x5fd7, 0x5fd8); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5fd8, 0x5fda); H8(hFF8E) = A;
  CYC(0x5fda, 0x5fdb); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5fdb, 0x5fdc); push_effect(gb, HL);
  CYC(0x5fdc, 0x5fdd); push_effect(gb, BC);
  CALL_C(0x5fdd, setInterleavedTile_hook, 0x3acf, 0x5fe0);
  CYC(0x5fe0, 0x5fe1); SET_BC(pop_effect(gb));
  CYC(0x5fe1, 0x5fe2); SET_HL(pop_effect(gb));
  CYC(0x5fe2, 0x5fe3); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x5fe3, 0x5fe5); goto next_loadInterleavedTiles; } // jr nz
  CYC(0x5fe3, 0x5fe5);
  RET(0x5fe5);
  if (gb->pc == 0x5f2d && gb->sp == sp0_) goto after1_loadInterleavedTiles;
  if (gb->pc == 0x5f63 && gb->sp == sp0_) goto after2_loadInterleavedTiles;
  return; // ret

// @param hl Pointer to data
loadTiles:
  CYC(0x5fe6, 0x5fe7); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5fe7, 0x5fe8); B = A;

next_loadTiles:
  CYC(0x5fe8, 0x5fe9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5fe9, 0x5fea); C = A;
  CYC(0x5fea, 0x5feb); A = mem_rd(gb, HL); // ld a,(hl)
  CYC(0x5feb, 0x5fec); push_effect(gb, HL);
  CYC(0x5fec, 0x5fed); push_effect(gb, BC);
  CALL_C(0x5fed, setTile_hook, 0x3a9c, 0x5ff0);
  CYC(0x5ff0, 0x5ff1); SET_BC(pop_effect(gb));
  CYC(0x5ff1, 0x5ff2); SET_HL(pop_effect(gb));
  CYC(0x5ff2, 0x5ff3); SET_HL(HL + 1); // inc hl
  CYC(0x5ff3, 0x5ff4); SET_HL(HL + 1); // inc hl
  CYC(0x5ff4, 0x5ff5); SET_HL(HL + 1); // inc hl
  CYC(0x5ff5, 0x5ff6); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x5ff6, 0x5ff8); goto next_loadTiles; } // jr nz
  CYC(0x5ff6, 0x5ff8);
  RET(0x5ff8);
  if (gb->pc == 0x5f47 && gb->sp == sp0_) goto after1_loadTiles;
  if (gb->pc == 0x5f7f && gb->sp == sp0_) goto after2_loadTiles;
  return; // ret

// @param bc Pointer to poof position data
loadPoofs:
  CYC(0x5ff9, 0x5ffa); A = mem_rd(gb, BC);
  CYC(0x5ffa, 0x5ffb); SET_BC(BC + 1); // inc bc

next_loadPoofs:
  CYC(0x5ffb, 0x5ffd); H8(hFF8B) = A;
  CALL_C(0x5ffd, getFreeInteractionSlot_hook, 0x3aef, 0x6000);
  if (!(F & FZ)) {
    RET_TAKEN(0x6000);
    if (gb->pc == 0x5f33 && gb->sp == sp0_) goto after1_loadPoofs;
    if (gb->pc == 0x5f4d && gb->sp == sp0_) goto after2_loadPoofs;
    if (gb->pc == 0x5f69 && gb->sp == sp0_) goto after3_loadPoofs;
    if (gb->pc == 0x5f85 && gb->sp == sp0_) goto after4_loadPoofs;
    return;
  } // ret nz
  CYC(0x6000, 0x6001);
  CYC(0x6001, 0x6003); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(0x6003, 0x6005); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x6005, 0x6006); A = mem_rd(gb, BC);
  CYC(0x6006, 0x6007); mem_wr(gb, HL, A);
  CYC(0x6007, 0x6008); SET_BC(BC + 1); // inc bc
  CYC(0x6008, 0x600a); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x600a, 0x600b); A = mem_rd(gb, BC);
  CYC(0x600b, 0x600c); mem_wr(gb, HL, A);
  CYC(0x600c, 0x600d); SET_BC(BC + 1); // inc bc
  CYC(0x600d, 0x600f); A = H8(hFF8B);
  CYC(0x600f, 0x6010); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6010, 0x6012); goto next_loadPoofs; } // jr nz
  CYC(0x6010, 0x6012);
  CYC(0x6012, 0x6014); A = 0x73; // SND_KILLENEMY
  CYC(0x6014, 0x6017); playSound_b00_hook(gb);
  if (gb->pc == 0x5f33 && gb->sp == sp0_) goto after1_loadPoofs;
  if (gb->pc == 0x5f4d && gb->sp == sp0_) goto after2_loadPoofs;
  if (gb->pc == 0x5f69 && gb->sp == sp0_) goto after3_loadPoofs;
  if (gb->pc == 0x5f85 && gb->sp == sp0_) goto after4_loadPoofs;
  return; // jp
}
