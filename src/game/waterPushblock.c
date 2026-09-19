#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCode9e_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// 0b:5721, the shared tail of the four setInterleaved*Tile helpers below. Reached only via `jr`
// (no stack effect) from each of them, ends with a tail-jump into the ROM's own setInterleavedTile.
static void interactionCode9e_setInterleavedTile_tail(GB *gb) {
  CYC(0x5721, 0x5723); hram_wr(gb, 0x8c, A);
  CYC(0x5723, 0x5724); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5724, 0x5726); hram_wr(gb, 0x8f, A);
  CYC(0x5726, 0x5727); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5727, 0x5729); hram_wr(gb, 0x8e, A);
  CYC(0x5729, 0x572a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x572a, 0x572d); setInterleavedTile_hook(gb); return; // jp
}

// @param a Position
void interactionCode9e_setHoleTile_hook(GB *gb) {
  CYC(0x56e9, 0x56ea); C = A;
  CYC(0x56ea, 0x56ec); A = 0xf3; // TILEINDEX_HOLE
  CYC(0x56ec, 0x56ef); setTile_hook(gb); return; // jp
}

// @param a Position
void interactionCode9e_setInterleavedPuddleHoleTile_hook(GB *gb) {
  CYC(0x56ef, 0x56f2); SET_HL(0x56f4);
  CYC(0x56f2, 0x56f4); interactionCode9e_setInterleavedTile_tail(gb); // jr
}

// @param a Position
void interactionCode9e_setInterleavedHolePuddleTile_hook(GB *gb) {
  CYC(0x56f7, 0x56fa); SET_HL(0x56fc);
  CYC(0x56fa, 0x56fc); interactionCode9e_setInterleavedTile_tail(gb); // jr
}

// @param a Position
void interactionCode9e_setGroundTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x56ff, 0x5700); push_effect(gb, BC);
  CYC(0x5700, 0x5701); C = A;
  CYC(0x5701, 0x5703); A = 0x1b;
  CALL_C(0x5703, setTile_hook, 0x3a9c, 0x5706);
  CYC(0x5706, 0x5707); SET_BC(pop_effect(gb));
  RET(0x5707); return;
}

// @param a Position
void interactionCode9e_setPuddleTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5708, 0x5709); push_effect(gb, BC);
  CYC(0x5709, 0x570a); C = A;
  CYC(0x570a, 0x570c); A = 0xf9; // TILEINDEX_PUDDLE
  CALL_C(0x570c, setTile_hook, 0x3a9c, 0x570f);
  CYC(0x570f, 0x5710); SET_BC(pop_effect(gb));
  RET(0x5710); return;
}

// @param a Position
void interactionCode9e_setInterleavedHoleGroundTile_hook(GB *gb) {
  CYC(0x5711, 0x5714); SET_HL(0x5716);
  CYC(0x5714, 0x5716); interactionCode9e_setInterleavedTile_tail(gb); // jr
}

// @param a Position
void interactionCode9e_setInterleavedGroundHoleTile_hook(GB *gb) {
  CYC(0x5719, 0x571c); SET_HL(0x571e);
  CYC(0x571c, 0x571e); interactionCode9e_setInterleavedTile_tail(gb); // jr
}

// 0b:5812, called (via a coincidental 12th, ROM-accurate extra invocation baked into the last
// call below) 12 times from interactionCode9e_swapRoomLayouts_hook.
static void interactionCode9e_xor_hook(GB *gb) {
  CYC(0x5812, 0x5813); A = mem_rd(gb, HL);
  CYC(0x5813, 0x5815); alu_xor(gb, 0x01);
  CYC(0x5815, 0x5816); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  RET(0x5816); return;
}

// Swap the room layouts in all rooms affected by the flooding.
void interactionCode9e_swapRoomLayouts_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x57e5, getThisRoomFlags_hook, 0x197d, 0x57e8);
  CYC(0x57e8, 0x57ea); L = 0x40; // <ROOM_AGES_140
  CALL_C(0x57ea, interactionCode9e_xor_hook, 0x5812, 0x57ed);
  CALL_C(0x57ed, interactionCode9e_xor_hook, 0x5812, 0x57f0);
  CALL_C(0x57f0, interactionCode9e_xor_hook, 0x5812, 0x57f3);
  CYC(0x57f3, 0x57f5); L = 0x50; // <ROOM_AGES_150
  CALL_C(0x57f5, interactionCode9e_xor_hook, 0x5812, 0x57f8);
  CALL_C(0x57f8, interactionCode9e_xor_hook, 0x5812, 0x57fb);
  CALL_C(0x57fb, interactionCode9e_xor_hook, 0x5812, 0x57fe);
  CYC(0x57fe, 0x57ff); H = alu_dec8(gb, H);
  CYC(0x57ff, 0x5801); L = 0x40; // <ROOM_AGES_040
  CALL_C(0x5801, interactionCode9e_xor_hook, 0x5812, 0x5804);
  CALL_C(0x5804, interactionCode9e_xor_hook, 0x5812, 0x5807);
  CALL_C(0x5807, interactionCode9e_xor_hook, 0x5812, 0x580a);
  CYC(0x580a, 0x580c); L = 0x50; // <ROOM_AGES_050
  CALL_C(0x580c, interactionCode9e_xor_hook, 0x5812, 0x580f);
  // The 11th (final, unconditional) `call $5812` below has a return address that coincidentally
  // equals the callee's own entry point (580f + 3 == 5812): after that call's own ret pops it and
  // resumes at 0x5812, execution simply re-enters @@xor a 12th time, whose ret is what actually
  // returns out of swapRoomLayouts to its real caller. Faithful to the ROM bytes.
  CALL_C(0x580f, interactionCode9e_xor_hook, 0x5812, 0x5812);
  interactionCode9e_xor_hook(gb); return;
}

// ==================================================================================================
// INTERAC_WATER_PUSHBLOCK
// ==================================================================================================
void interactionCode9e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5575, 0x5577); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5577, 0x5578); A = mem_rd(gb, DE);
  CYC(0x5578, 0x5579); push_effect(gb, 0x5579);
  switch (interactionCode9e_jump_table(gb)) {
    case 0x557d: goto subid0;
    case 0x572d: goto subid1;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0:
  CYC(0x557d, 0x557f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x557f, 0x5580); A = mem_rd(gb, DE);
  CYC(0x5580, 0x5581); push_effect(gb, 0x5581);
  switch (interactionCode9e_jump_table(gb)) {
    case 0x558b: goto subid0State0;
    case 0x55ac: goto state1;
    case 0x5601: goto state2;
    case 0x5610: goto subid0State3;
    case 0x2680: objectPreventLinkFromPassing_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0State0:
  CALL_C(0x558b, getThisRoomFlags_hook, 0x197d, 0x558e);
  CYC(0x558e, 0x5590); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x5590, 0x5593); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x5590, 0x5593);

initialize:
  CALL_C(0x5593, interactionInitGraphics_hook, 0x15fb, 0x5596);
  CALL_C(0x5596, objectMarkSolidPosition_hook, 0x24f0, 0x5599);
  CYC(0x5599, 0x559b); A = 0x06;
  CALL_C(0x559b, objectSetCollideRadius_hook, 0x24a1, 0x559e);
  CYC(0x559e, 0x55a0); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x55a0, 0x55a2); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x55a2, 0x55a4); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x55a4, 0x55a6); mem_wr(gb, HL, 30);
  CALL_C(0x55a6, objectSetVisible82_hook, 0x1e69, 0x55a9);
  CYC(0x55a9, 0x55ac); interactionIncState_hook(gb); return; // jp

state1:
  CALL_C(0x55ac, objectPreventLinkFromPassing_hook, 0x2680, 0x55af);
  if (!(F & FC)) { CYCT(0x55af, 0x55b1); goto notPushing; } // jr nc
  CYC(0x55af, 0x55b1);
  CALL_C(0x55b1, objectCheckLinkPushingAgainstCenter_hook, 0x2707, 0x55b4);
  if (!(F & FC)) { CYCT(0x55b4, 0x55b6); goto notPushing; } // jr nc
  CYC(0x55b4, 0x55b6);
  CYC(0x55b6, 0x55b8); A = 0x01;
  CYC(0x55b8, 0x55bb); mem_wr(gb, wForceLinkPushAnimation, A);
  CALL_C(0x55bb, interactionDecCounter1_hook, 0x23cc, 0x55be);
  if (!(F & FZ)) { CYCT(0x55be, 0x55bf); ret_effect(gb); return; } // ret nz
  CYC(0x55be, 0x55bf);
  CYC(0x55bf, 0x55c1); goto pushedLongEnough; // jr

notPushing:
  CYC(0x55c1, 0x55c3); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x55c3, 0x55c5); A = 30;
  CYC(0x55c5, 0x55c6); mem_wr(gb, DE, A);
  RET(0x55c6); return;

pushedLongEnough:
  CYC(0x55c7, 0x55c9); C = 0x28;
  CALL_C(0x55c9, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x55cc);
  CYC(0x55cc, 0x55cd); B = A;
  CYC(0x55cd, 0x55cf); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x55cf, 0x55d0); A = mem_rd(gb, DE);
  CYC(0x55d0, 0x55d1); alu_or(gb, A);
  CYC(0x55d1, 0x55d3); C = 0x02;
  if (F & FZ) { CYCT(0x55d3, 0x55d5); goto l_55d7; } // jr z
  CYC(0x55d3, 0x55d5);
  CYC(0x55d5, 0x55d7); C = 0x06;

l_55d7:
  CYC(0x55d7, 0x55d8); A = B;
  CYC(0x55d8, 0x55d9); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x55d9, 0x55da); ret_effect(gb); return; } // ret nz
  CYC(0x55d9, 0x55da);
  CYC(0x55da, 0x55dc); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x55dc, 0x55de); A = (uint8_t)(mem_rd(gb, DE) ^ 0x04);
  CYC(0x55de, 0x55df); mem_wr(gb, DE, A);
  CYC(0x55df, 0x55e0); H = D;
  CYC(0x55e0, 0x55e2); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x55e2, 0x55e3); A = mem_rd(gb, HL);
  CYC(0x55e3, 0x55e4); alu_add(gb, A);
  CYC(0x55e4, 0x55e5); alu_add(gb, A);
  CYC(0x55e5, 0x55e7); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x55e7, 0x55e8); mem_wr(gb, HL, A);
  CYC(0x55e8, 0x55ea); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x55ea, 0x55ec); mem_wr(gb, HL, 0x40);
  CYC(0x55ec, 0x55ee); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(0x55ee, 0x55f1); mem_wr(gb, wDisabledObjects, A);
  CYC(0x55f1, 0x55f4); mem_wr(gb, wMenuDisabled, A);
  CYC(0x55f4, 0x55f6); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(0x55f6, playSound_b00_hook, 0x0c98, 0x55f9);
  CYC(0x55f9, 0x55fb); A = 0x71; // SND_MOVEBLOCK
  CALL_C(0x55fb, playSound_b00_hook, 0x0c98, 0x55fe);
  CYC(0x55fe, 0x5601); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(0x5601, objectApplySpeed_hook, 0x201d, 0x5604);
  CALL_C(0x5604, objectPreventLinkFromPassing_hook, 0x2680, 0x5607);
  CALL_C(0x5607, interactionDecCounter1_hook, 0x23cc, 0x560a);
  if (!(F & FZ)) { CYCT(0x560a, 0x560b); ret_effect(gb); return; } // ret nz
  CYC(0x560a, 0x560b);
  CYC(0x560b, 0x560d); mem_wr(gb, HL, 70);
  CYC(0x560d, 0x5610); interactionIncState_hook(gb); return; // jp

subid0State3:
  CYC(0x5610, 0x5612); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5612, 0x5613); A = mem_rd(gb, DE);
  CYC(0x5613, 0x5614); push_effect(gb, 0x5614);
  switch (interactionCode9e_jump_table(gb)) {
    case 0x562c: goto subid0Substate0;
    case 0x5644: goto subid0Substate1;
    case 0x5658: goto subid0Substate2;
    case 0x566c: goto subid0Substate3;
    case 0x5671: goto subid0Substate4;
    case 0x5676: goto subid0Substate5;
    case 0x567b: goto subid0Substate6;
    case 0x568e: goto subid0Substate7;
    case 0x56a1: goto subid0Substate8;
    case 0x56af: goto subid0Substate9;
    case 0x56bf: goto substateA;
    case 0x56d2: goto substateB;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0Substate0:
  CALL_C(0x562c, interactionDecCounter1_hook, 0x23cc, 0x562f);
  if (!(F & FZ)) { CYCT(0x562f, 0x5630); ret_effect(gb); return; } // ret nz
  CYC(0x562f, 0x5630);
  CYC(0x5630, 0x5632); mem_wr(gb, HL, 0x08);
  CYC(0x5632, 0x5634); A = 0xc2; // SND_FLOODGATES
  CALL_C(0x5634, playSound_b00_hook, 0x0c98, 0x5637);
  CYC(0x5637, 0x5639); A = 0x63;
  CALL_C(0x5639, interactionCode9e_setInterleavedHoleGroundTile_hook, 0x5711, 0x563c);
  CYC(0x563c, 0x563e); A = 0x65;
  CALL_C(0x563e, interactionCode9e_setInterleavedHoleGroundTile_hook, 0x5711, 0x5641);
  CYC(0x5641, 0x5644); interactionIncSubstate_hook(gb); return; // jp

subid0Substate1:
  CYC(0x5644, 0x5647); SET_BC(0x6365);

setGroundTilesWhenCounterIsZero:
  CALL_C(0x5647, interactionDecCounter1_hook, 0x23cc, 0x564a);
  if (!(F & FZ)) { CYCT(0x564a, 0x564b); ret_effect(gb); return; } // ret nz
  CYC(0x564a, 0x564b);
  CYC(0x564b, 0x564d); mem_wr(gb, HL, 0x08);
  CYC(0x564d, 0x564e); A = B;
  CALL_C(0x564e, interactionCode9e_setGroundTile_hook, 0x56ff, 0x5651);
  CYC(0x5651, 0x5652); A = C;
  CALL_C(0x5652, interactionCode9e_setPuddleTile_hook, 0x5708, 0x5655);
  CYC(0x5655, 0x5658); interactionIncSubstate_hook(gb); return; // jp

subid0Substate2:
  CYC(0x5658, 0x565b); SET_BC(0x6266);

setHoleTilesWhenCounterIsZero:
  CALL_C(0x565b, interactionDecCounter1_hook, 0x23cc, 0x565e);
  if (!(F & FZ)) { CYCT(0x565e, 0x565f); ret_effect(gb); return; } // ret nz
  CYC(0x565e, 0x565f);
  CYC(0x565f, 0x5661); mem_wr(gb, HL, 0x08);
  CYC(0x5661, 0x5662); A = B;
  CALL_C(0x5662, interactionCode9e_setInterleavedHoleGroundTile_hook, 0x5711, 0x5665);
  CYC(0x5665, 0x5666); A = C;
  CALL_C(0x5666, interactionCode9e_setInterleavedHoleGroundTile_hook, 0x5711, 0x5669);
  CYC(0x5669, 0x566c); interactionIncSubstate_hook(gb); return; // jp

subid0Substate3:
  CYC(0x566c, 0x566f); SET_BC(0x6266);
  CYC(0x566f, 0x5671); goto setGroundTilesWhenCounterIsZero; // jr

subid0Substate4:
  CYC(0x5671, 0x5674); SET_BC(0x6167);
  CYC(0x5674, 0x5676); goto setHoleTilesWhenCounterIsZero; // jr

subid0Substate5:
  CYC(0x5676, 0x5679); SET_BC(0x6167);
  CYC(0x5679, 0x567b); goto setGroundTilesWhenCounterIsZero; // jr

subid0Substate6:
  CALL_C(0x567b, interactionDecCounter1_hook, 0x23cc, 0x567e);
  if (!(F & FZ)) { CYCT(0x567e, 0x567f); ret_effect(gb); return; } // ret nz
  CYC(0x567e, 0x567f);
  CYC(0x567f, 0x5681); mem_wr(gb, HL, 0x08);
  CYC(0x5681, 0x5683); A = 0x60;
  CALL_C(0x5683, interactionCode9e_setInterleavedPuddleHoleTile_hook, 0x56ef, 0x5686);
  CYC(0x5686, 0x5688); A = 0x68;
  CALL_C(0x5688, interactionCode9e_setInterleavedHoleGroundTile_hook, 0x5711, 0x568b);
  CYC(0x568b, 0x568e); interactionIncSubstate_hook(gb); return; // jp

subid0Substate7:
  CALL_C(0x568e, interactionDecCounter1_hook, 0x23cc, 0x5691);
  if (!(F & FZ)) { CYCT(0x5691, 0x5692); ret_effect(gb); return; } // ret nz
  CYC(0x5691, 0x5692);
  CYC(0x5692, 0x5694); mem_wr(gb, HL, 0x08);
  CYC(0x5694, 0x5696); A = 0x60;
  CALL_C(0x5696, interactionCode9e_setHoleTile_hook, 0x56e9, 0x5699);
  CYC(0x5699, 0x569b); A = 0x68;
  CALL_C(0x569b, interactionCode9e_setPuddleTile_hook, 0x5708, 0x569e);
  CYC(0x569e, 0x56a1); interactionIncSubstate_hook(gb); return; // jp

subid0Substate8:
  CALL_C(0x56a1, interactionDecCounter1_hook, 0x23cc, 0x56a4);
  if (!(F & FZ)) { CYCT(0x56a4, 0x56a5); ret_effect(gb); return; } // ret nz
  CYC(0x56a4, 0x56a5);
  CYC(0x56a5, 0x56a7); mem_wr(gb, HL, 0x08);
  CYC(0x56a7, 0x56a9); A = 0x69;
  CALL_C(0x56a9, interactionCode9e_setInterleavedPuddleHoleTile_hook, 0x56ef, 0x56ac);
  CYC(0x56ac, 0x56af); interactionIncSubstate_hook(gb); return; // jp

subid0Substate9:
  CALL_C(0x56af, interactionDecCounter1_hook, 0x23cc, 0x56b2);
  if (!(F & FZ)) { CYCT(0x56b2, 0x56b3); ret_effect(gb); return; } // ret nz
  CYC(0x56b2, 0x56b3);
  CYC(0x56b3, 0x56b5); mem_wr(gb, HL, 90);
  CYC(0x56b5, 0x56b7); C = 0x69;

setWaterTileAndIncSubstate:
  CYC(0x56b7, 0x56b9); A = 0xfa; // TILEINDEX_WATER
  CALL_C(0x56b9, setTile_hook, 0x3a9c, 0x56bc);
  CYC(0x56bc, 0x56bf); interactionIncSubstate_hook(gb); return; // jp

substateA:
  CALL_C(0x56bf, interactionDecCounter1_hook, 0x23cc, 0x56c2);
  if (!(F & FZ)) { CYCT(0x56c2, 0x56c3); ret_effect(gb); return; } // ret nz
  CYC(0x56c2, 0x56c3);
  CYC(0x56c3, 0x56c5); mem_wr(gb, HL, 72);
  CYC(0x56c5, 0x56c7); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(0x56c7, playSound_b00_hook, 0x0c98, 0x56ca);
  CYC(0x56ca, 0x56cc); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x56cc, playSound_b00_hook, 0x0c98, 0x56cf);
  CYC(0x56cf, 0x56d2); interactionIncSubstate_hook(gb); return; // jp

substateB:
  CALL_C(0x56d2, interactionDecCounter1_hook, 0x23cc, 0x56d5);
  if (!(F & FZ)) { CYCT(0x56d5, 0x56d6); ret_effect(gb); return; } // ret nz
  CYC(0x56d5, 0x56d6);
  CYC(0x56d6, 0x56d9); A = mem_rd(gb, wActiveMusic);
  CALL_C(0x56d9, playSound_b00_hook, 0x0c98, 0x56dc);
  CYC(0x56dc, 0x56dd); alu_xor(gb, A);
  CYC(0x56dd, 0x56e0); mem_wr(gb, wDisabledObjects, A);
  CYC(0x56e0, 0x56e3); mem_wr(gb, wMenuDisabled, A);
  CALL_C(0x56e3, interactionCode9e_swapRoomLayouts_hook, 0x57e5, 0x56e6);
  CYC(0x56e6, 0x56e9); interactionIncState_hook(gb); return; // jp

subid1:
  CYC(0x572d, 0x572f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x572f, 0x5730); A = mem_rd(gb, DE);
  CYC(0x5730, 0x5731); push_effect(gb, 0x5731);
  switch (interactionCode9e_jump_table(gb)) {
    case 0x573b: goto subid1State0;
    case 0x55ac: goto state1;
    case 0x5601: goto state2;
    case 0x5746: goto subid1State3;
    case 0x2680: objectPreventLinkFromPassing_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid1State0:
  CALL_C(0x573b, getThisRoomFlags_hook, 0x197d, 0x573e);
  CYC(0x573e, 0x5740); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x5740, 0x5743); interactionDelete_hook(gb); return; } // jp z
  CYC(0x5740, 0x5743);
  CYC(0x5743, 0x5746); goto initialize; // jp (local)

subid1State3:
  CYC(0x5746, 0x5748); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5748, 0x5749); A = mem_rd(gb, DE);
  CYC(0x5749, 0x574a); push_effect(gb, 0x574a);
  switch (interactionCode9e_jump_table(gb)) {
    case 0x5762: goto subid1Substate0;
    case 0x577a: goto subid1Substate1;
    case 0x5780: goto subid1Substate2;
    case 0x5794: goto subid1Substate3;
    case 0x579a: goto subid1Substate4;
    case 0x57a0: goto subid1Substate5;
    case 0x57a6: goto subid1Substate6;
    case 0x57b9: goto subid1Substate7;
    case 0x57c9: goto subid1Substate8;
    case 0x57d7: goto subid1Substate9;
    case 0x56bf: goto substateA;
    case 0x56d2: goto substateB;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid1Substate0:
  CALL_C(0x5762, interactionDecCounter1_hook, 0x23cc, 0x5765);
  if (!(F & FZ)) { CYCT(0x5765, 0x5766); ret_effect(gb); return; } // ret nz
  CYC(0x5765, 0x5766);
  CYC(0x5766, 0x5768); mem_wr(gb, HL, 0x08);
  CYC(0x5768, 0x576a); A = 0xc2; // SND_FLOODGATES
  CALL_C(0x576a, playSound_b00_hook, 0x0c98, 0x576d);
  CYC(0x576d, 0x576f); A = 0x63;
  CALL_C(0x576f, interactionCode9e_setInterleavedGroundHoleTile_hook, 0x5719, 0x5772);
  CYC(0x5772, 0x5774); A = 0x65;
  CALL_C(0x5774, interactionCode9e_setInterleavedGroundHoleTile_hook, 0x5719, 0x5777);
  CYC(0x5777, 0x577a); interactionIncSubstate_hook(gb); return; // jp

subid1Substate1:
  CYC(0x577a, 0x577d); SET_BC(0x6563);
  CYC(0x577d, 0x5780); goto setGroundTilesWhenCounterIsZero; // jp (local)

subid1Substate2:
  CYC(0x5780, 0x5783); SET_BC(0x6662);

setHoleTilesWhenCounterZero_2:
  CALL_C(0x5783, interactionDecCounter1_hook, 0x23cc, 0x5786);
  if (!(F & FZ)) { CYCT(0x5786, 0x5787); ret_effect(gb); return; } // ret nz
  CYC(0x5786, 0x5787);
  CYC(0x5787, 0x5789); mem_wr(gb, HL, 0x08);
  CYC(0x5789, 0x578a); A = B;
  CALL_C(0x578a, interactionCode9e_setInterleavedGroundHoleTile_hook, 0x5719, 0x578d);
  CYC(0x578d, 0x578e); A = C;
  CALL_C(0x578e, interactionCode9e_setInterleavedGroundHoleTile_hook, 0x5719, 0x5791);
  CYC(0x5791, 0x5794); interactionIncSubstate_hook(gb); return; // jp

subid1Substate3:
  CYC(0x5794, 0x5797); SET_BC(0x6662);
  CYC(0x5797, 0x579a); goto setGroundTilesWhenCounterIsZero; // jp (local)

subid1Substate4:
  CYC(0x579a, 0x579d); SET_BC(0x6167);
  CYC(0x579d, 0x57a0); goto setHoleTilesWhenCounterZero_2; // jp (local)

subid1Substate5:
  CYC(0x57a0, 0x57a3); SET_BC(0x6761);
  CYC(0x57a3, 0x57a6); goto setGroundTilesWhenCounterIsZero; // jp (local)

subid1Substate6:
  CALL_C(0x57a6, interactionDecCounter1_hook, 0x23cc, 0x57a9);
  if (!(F & FZ)) { CYCT(0x57a9, 0x57aa); ret_effect(gb); return; } // ret nz
  CYC(0x57a9, 0x57aa);
  CYC(0x57aa, 0x57ac); mem_wr(gb, HL, 0x08);
  CYC(0x57ac, 0x57ae); A = 0x60;
  CALL_C(0x57ae, interactionCode9e_setInterleavedHolePuddleTile_hook, 0x56f7, 0x57b1);
  CYC(0x57b1, 0x57b3); A = 0x68;
  CALL_C(0x57b3, interactionCode9e_setInterleavedGroundHoleTile_hook, 0x5719, 0x57b6);
  CYC(0x57b6, 0x57b9); interactionIncSubstate_hook(gb); return; // jp

subid1Substate7:
  CALL_C(0x57b9, interactionDecCounter1_hook, 0x23cc, 0x57bc);
  if (!(F & FZ)) { CYCT(0x57bc, 0x57bd); ret_effect(gb); return; } // ret nz
  CYC(0x57bc, 0x57bd);
  CYC(0x57bd, 0x57bf); mem_wr(gb, HL, 0x08);
  CYC(0x57bf, 0x57c1); A = 0x68;
  CALL_C(0x57c1, interactionCode9e_setGroundTile_hook, 0x56ff, 0x57c4);
  CYC(0x57c4, 0x57c6); C = 0x60;
  CYC(0x57c6, 0x57c9); goto setWaterTileAndIncSubstate; // jp (local)

subid1Substate8:
  CALL_C(0x57c9, interactionDecCounter1_hook, 0x23cc, 0x57cc);
  if (!(F & FZ)) { CYCT(0x57cc, 0x57cd); ret_effect(gb); return; } // ret nz
  CYC(0x57cc, 0x57cd);
  CYC(0x57cd, 0x57cf); mem_wr(gb, HL, 0x08);
  CYC(0x57cf, 0x57d1); A = 0x69;
  CALL_C(0x57d1, interactionCode9e_setInterleavedHolePuddleTile_hook, 0x56f7, 0x57d4);
  CYC(0x57d4, 0x57d7); interactionIncSubstate_hook(gb); return; // jp

subid1Substate9:
  CALL_C(0x57d7, interactionDecCounter1_hook, 0x23cc, 0x57da);
  if (!(F & FZ)) { CYCT(0x57da, 0x57db); ret_effect(gb); return; } // ret nz
  CYC(0x57da, 0x57db);
  CYC(0x57db, 0x57dd); mem_wr(gb, HL, 90);
  CYC(0x57dd, 0x57df); A = 0x69;
  CALL_C(0x57df, interactionCode9e_setHoleTile_hook, 0x56e9, 0x57e2);
  CYC(0x57e2, 0x57e5); interactionIncSubstate_hook(gb); return; // jp
}
