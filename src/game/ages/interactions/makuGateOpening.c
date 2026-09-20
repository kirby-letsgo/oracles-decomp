#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode76), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode76), (from), (to), true)

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
  BASE(interactionCode76);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    interactionCode76_jump_table(gb); // subid0 and subid1 both target 0x5f0a
  }

  // interactionCode76@subid0 / @subid1
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  {
    CYC(b_+11, b_+12); push_effect(gb, b_+12);
    uint16_t target = interactionCode76_jump_table(gb);
    if (target == b_+59) goto state1;
    if (target == b_+87) goto state2;
    if (target == b_+115) goto state3;
  }

  // interactionCode76@state0
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 30);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+32); alu_or(gb, A);
  CYC(b_+32, b_+35); SET_HL(b_+142); // @frame0And1_subid0
  if (F & FZ) { CYCT(b_+35, b_+37); goto l5f2a; } // jr z
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+40); SET_HL(b_+151); // @frame0And1_subid1

l5f2a:
  CYC(b_+40, b_+43); push_effect(gb, b_+43); goto loadInterleavedTiles; // call
after1_loadInterleavedTiles:
  CYC(b_+43, b_+46); SET_BC(b_+169); // @frame0_poof
  CYC(b_+46, b_+49); push_effect(gb, b_+49); goto loadPoofs; // call
after1_loadPoofs:
  // falls through into shakeScreen (no push_effect for this path)

shakeScreen:
  CYC(b_+49, b_+51); A = 0x06;
  CALL_C(b_+51, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+54);
  CYC(b_+54, b_+56); A = 0x70; // SND_DOORCLOSE
  CYC(b_+56, b_+59); playSound_b00_hook(gb);
  if (gb->pc == b_+78 && gb->sp == sp0_) goto after1_shakeScreen;
  if (gb->pc == b_+106 && gb->sp == sp0_) goto after2_shakeScreen;
  if (gb->pc == b_+134 && gb->sp == sp0_) goto after3_shakeScreen;
  return; // jp

state1:
  CALL_C(b_+59, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+62);
  if (!(F & FZ)) { RET_TAKEN(b_+62); return; } // ret nz
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+66); SET_HL(b_+142); // @frame0And1_subid0
  CYC(b_+66, b_+69); push_effect(gb, b_+69); goto loadTiles; // call
after1_loadTiles:
  CYC(b_+69, b_+72); SET_BC(b_+178); // @frame1_poof
  CYC(b_+72, b_+75); push_effect(gb, b_+75); goto loadPoofs; // call
after2_loadPoofs:
  CYC(b_+75, b_+78); push_effect(gb, b_+78); goto shakeScreen; // call
after1_shakeScreen:
  CYC(b_+78, b_+79); H = D;
  CYC(b_+79, b_+81); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+81, b_+82); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+82, b_+84); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 30);
  RET(b_+86); return; // ret

state2:
  CALL_C(b_+87, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+90);
  if (!(F & FZ)) { RET_TAKEN(b_+90); return; } // ret nz
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+94); SET_HL(b_+160); // @frame2And3
  CYC(b_+94, b_+97); push_effect(gb, b_+97); goto loadInterleavedTiles; // call
after2_loadInterleavedTiles:
  CYC(b_+97, b_+100); SET_BC(b_+187); // @frame2_poof
  CYC(b_+100, b_+103); push_effect(gb, b_+103); goto loadPoofs; // call
after3_loadPoofs:
  CYC(b_+103, b_+106); push_effect(gb, b_+106); goto shakeScreen; // call
after2_shakeScreen:
  CYC(b_+106, b_+107); H = D;
  CYC(b_+107, b_+109); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+109, b_+110); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 30);
  RET(b_+114); return; // ret

state3:
  CALL_C(b_+115, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+118);
  if (!(F & FZ)) { RET_TAKEN(b_+118); return; } // ret nz
  CYC(b_+118, b_+119);
  CYC(b_+119, b_+122); SET_HL(b_+160); // @frame2And3
  CYC(b_+122, b_+125); push_effect(gb, b_+125); goto loadTiles; // call
after2_loadTiles:
  CYC(b_+125, b_+128); SET_BC(b_+196); // @frame3_poof
  CYC(b_+128, b_+131); push_effect(gb, b_+131); goto loadPoofs; // call
after4_loadPoofs:
  CYC(b_+131, b_+134); push_effect(gb, b_+134); goto shakeScreen; // call
after3_shakeScreen:
  CALL_C(b_+134, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+137);
  CYC(b_+137, b_+139); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  CYC(b_+139, b_+142); interactionDelete_hook(gb); return; // jp

// @param hl Pointer to data
loadInterleavedTiles:
  CYC(b_+205, b_+206); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+206, b_+207); B = A;

next_loadInterleavedTiles:
  CYC(b_+207, b_+208); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+208, b_+210); H8(hFF8C) = A;
  CYC(b_+210, b_+211); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+211, b_+213); H8(hFF8F) = A;
  CYC(b_+213, b_+214); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+214, b_+216); H8(hFF8E) = A;
  CYC(b_+216, b_+217); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+217, b_+218); push_effect(gb, HL);
  CYC(b_+218, b_+219); push_effect(gb, BC);
  CALL_C(b_+219, setInterleavedTile_hook, SYM(setInterleavedTile), b_+222);
  CYC(b_+222, b_+223); SET_BC(pop_effect(gb));
  CYC(b_+223, b_+224); SET_HL(pop_effect(gb));
  CYC(b_+224, b_+225); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+225, b_+227); goto next_loadInterleavedTiles; } // jr nz
  CYC(b_+225, b_+227);
  RET(b_+227);
  if (gb->pc == b_+43 && gb->sp == sp0_) goto after1_loadInterleavedTiles;
  if (gb->pc == b_+97 && gb->sp == sp0_) goto after2_loadInterleavedTiles;
  return; // ret

// @param hl Pointer to data
loadTiles:
  CYC(b_+228, b_+229); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+229, b_+230); B = A;

next_loadTiles:
  CYC(b_+230, b_+231); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+231, b_+232); C = A;
  CYC(b_+232, b_+233); A = mem_rd(gb, HL); // ld a,(hl)
  CYC(b_+233, b_+234); push_effect(gb, HL);
  CYC(b_+234, b_+235); push_effect(gb, BC);
  CALL_C(b_+235, setTile_hook, SYM(setTile), b_+238);
  CYC(b_+238, b_+239); SET_BC(pop_effect(gb));
  CYC(b_+239, b_+240); SET_HL(pop_effect(gb));
  CYC(b_+240, b_+241); SET_HL(HL + 1); // inc hl
  CYC(b_+241, b_+242); SET_HL(HL + 1); // inc hl
  CYC(b_+242, b_+243); SET_HL(HL + 1); // inc hl
  CYC(b_+243, b_+244); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+244, b_+246); goto next_loadTiles; } // jr nz
  CYC(b_+244, b_+246);
  RET(b_+246);
  if (gb->pc == b_+69 && gb->sp == sp0_) goto after1_loadTiles;
  if (gb->pc == b_+125 && gb->sp == sp0_) goto after2_loadTiles;
  return; // ret

// @param bc Pointer to poof position data
loadPoofs:
  CYC(b_+247, b_+248); A = mem_rd(gb, BC);
  CYC(b_+248, b_+249); SET_BC(BC + 1); // inc bc

next_loadPoofs:
  CYC(b_+249, b_+251); H8(hFF8B) = A;
  CALL_C(b_+251, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+254);
  if (!(F & FZ)) {
    RET_TAKEN(b_+254);
    if (gb->pc == b_+49 && gb->sp == sp0_) goto after1_loadPoofs;
    if (gb->pc == b_+75 && gb->sp == sp0_) goto after2_loadPoofs;
    if (gb->pc == b_+103 && gb->sp == sp0_) goto after3_loadPoofs;
    if (gb->pc == b_+131 && gb->sp == sp0_) goto after4_loadPoofs;
    return;
  } // ret nz
  CYC(b_+254, b_+255);
  CYC(b_+255, b_+257); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+257, b_+259); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+259, b_+260); A = mem_rd(gb, BC);
  CYC(b_+260, b_+261); mem_wr(gb, HL, A);
  CYC(b_+261, b_+262); SET_BC(BC + 1); // inc bc
  CYC(b_+262, b_+264); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+264, b_+265); A = mem_rd(gb, BC);
  CYC(b_+265, b_+266); mem_wr(gb, HL, A);
  CYC(b_+266, b_+267); SET_BC(BC + 1); // inc bc
  CYC(b_+267, b_+269); A = H8(hFF8B);
  CYC(b_+269, b_+270); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+270, b_+272); goto next_loadPoofs; } // jr nz
  CYC(b_+270, b_+272);
  CYC(b_+272, b_+274); A = 0x73; // SND_KILLENEMY
  CYC(b_+274, SYM(interactionCode77)); playSound_b00_hook(gb);
  if (gb->pc == b_+49 && gb->sp == sp0_) goto after1_loadPoofs;
  if (gb->pc == b_+75 && gb->sp == sp0_) goto after2_loadPoofs;
  if (gb->pc == b_+103 && gb->sp == sp0_) goto after3_loadPoofs;
  if (gb->pc == b_+131 && gb->sp == sp0_) goto after4_loadPoofs;
  return; // jp
}
