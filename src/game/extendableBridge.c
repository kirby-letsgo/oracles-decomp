#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Pointer tables indexed by Interaction.xh. Each entry: tile index, then positions
// to write it at, terminated by $ff.
#define extendableBridgeCreationData_bank08 0x525d
#define extendableBridgeRemovalData_bank08 0x5292

static uint16_t extendableBridge_jumpTable(GB *gb) {
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

static void extendableBridge_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode23@checkSwitchStateChanged: nz if the switch this bridge listens to
// (bitmask in var03) has toggled since var30 was recorded.
static void extendableBridge_checkSwitchStateChanged(GB *gb) {
  CYC(0x5240, 0x5243); A = mem_rd(gb, wSwitchState);
  CYC(0x5243, 0x5244); B = A;
  CYC(0x5244, 0x5246); E = INTERACTION_BASE + 0x30; // var30
  CYC(0x5246, 0x5247); A = mem_rd(gb, DE);
  CYC(0x5247, 0x5248); alu_xor(gb, B);
  CYC(0x5248, 0x5249); B = A;
  CYC(0x5249, 0x524b); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x524b, 0x524c); A = mem_rd(gb, DE);
  CYC(0x524c, 0x524d); alu_and(gb, B);
  CYC(0x524d, 0x524e); ret_effect(gb);
}

// interactionCode23@updateNextTile: reads the next position byte into C from the pointer
// in relatedObj2, and advances that pointer.
static void extendableBridge_updateNextTile(GB *gb) {
  CYC(0x524e, 0x524f); H = D;
  CYC(0x524f, 0x5251); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(0x5251, 0x5252); E = L;
  CYC(0x5252, 0x5253); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5253, 0x5254); H = mem_rd(gb, HL);
  CYC(0x5254, 0x5255); L = A;
  CYC(0x5255, 0x5256); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5256, 0x5257); C = A;
  CYC(0x5257, 0x5258); A = L;
  CYC(0x5258, 0x5259); mem_wr(gb, DE, A);
  CYC(0x5259, 0x525a); E = alu_inc8(gb, E);
  CYC(0x525a, 0x525b); A = H;
  CYC(0x525b, 0x525c); mem_wr(gb, DE, A);
  CYC(0x525c, 0x525d); ret_effect(gb);
}

// @startLoadingBridgeData: HL points at a pointer table; select the entry for xh, record
// the tile index in var31 and the position pointer in relatedObj2, then start counting.
static void extendableBridge_startLoadingBridgeData(GB *gb) {
  CYC(0x51cf, 0x51d1); E = INTERACTION_BASE + 0x30; // var30
  CYC(0x51d1, 0x51d4); A = mem_rd(gb, wSwitchState);
  CYC(0x51d4, 0x51d5); mem_wr(gb, DE, A);
  CYC(0x51d5, 0x51d7); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x51d7, 0x51d8); A = mem_rd(gb, DE);
  CYC(0x51d8, 0x51d9); extendableBridge_addDoubleIndex(gb, 0x51d9);
  CYC(0x51d9, 0x51da); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x51da, 0x51db); H = mem_rd(gb, HL);
  CYC(0x51db, 0x51dc); L = A;
  CYC(0x51dc, 0x51dd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x51dd, 0x51df); E = INTERACTION_BASE + 0x31; // var31
  CYC(0x51df, 0x51e0); mem_wr(gb, DE, A);
  CYC(0x51e0, 0x51e2); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(0x51e2, 0x51e3); A = L;
  CYC(0x51e3, 0x51e4); mem_wr(gb, DE, A);
  CYC(0x51e4, 0x51e5); E = alu_inc8(gb, E);
  CYC(0x51e5, 0x51e6); A = H;
  CYC(0x51e6, 0x51e7); mem_wr(gb, DE, A);
  CYC(0x51e7, 0x51e9); A = 0x0a;
  CYC(0x51e9, 0x51eb); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x51eb, 0x51ec); mem_wr(gb, DE, A);
  CYC(0x51ec, 0x51ef); interactionIncSubstate_hook(gb);
}

// @state0: var03 = bitmask for subid; go to state 2 if a bridge is already here,
// state 1 otherwise; remember the current switch state.
static void extendableBridge_state0(GB *gb) {
  CYC(0x5198, 0x519a); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x519a, 0x519b); A = mem_rd(gb, DE);
  CYC(0x519b, 0x519c); B = A;
  CYC(0x519c, 0x519e); alu_and(gb, 0x07);
  CYC(0x519e, 0x51a1); SET_HL(bitTable);
  CYC(0x51a1, 0x51a2); alu_add(gb, L);
  CYC(0x51a2, 0x51a3); L = A;
  CYC(0x51a3, 0x51a4); A = mem_rd(gb, HL);
  CYC(0x51a4, 0x51a5); E = alu_inc8(gb, E);
  CYC(0x51a5, 0x51a6); mem_wr(gb, DE, A);
  CYC(0x51a6, 0x51a8); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x51a8, 0x51a9); A = mem_rd(gb, DE);
  CYC(0x51a9, 0x51aa); C = A;
  CYC(0x51aa, 0x51ac); B = wRoomLayout >> 8;
  CYC(0x51ac, 0x51ad); A = mem_rd(gb, BC);
  CYC(0x51ad, 0x51af); alu_sub(gb, 0x6a); // TILEINDEX_VERTICAL_BRIDGE
  CYC(0x51af, 0x51b1); alu_sub(gb, 0x06);
  CYC(0x51b1, 0x51b3); A = 0x02;
  if (F & FC) {
    CYCT(0x51b3, 0x51b5);
  } else {
    CYC(0x51b3, 0x51b5);
    CYC(0x51b5, 0x51b6); A = alu_dec8(gb, A);
  }
  CYC(0x51b6, 0x51b8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x51b8, 0x51b9); mem_wr(gb, DE, A);
  CYC(0x51b9, 0x51bb); E = INTERACTION_BASE + 0x30; // var30
  CYC(0x51bb, 0x51be); A = mem_rd(gb, wSwitchState);
  CYC(0x51be, 0x51bf); mem_wr(gb, DE, A);
  CYC(0x51bf, 0x51c0); ret_effect(gb);
}

// @state1: waiting for the switch to toggle to create the bridge.
static void extendableBridge_state1(GB *gb, uint16_t sp0_) {
  CYC(0x51c0, 0x51c2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x51c2, 0x51c3); A = mem_rd(gb, DE);
  CYC(0x51c3, 0x51c4); push_effect(gb, 0x51c4);
  switch (extendableBridge_jumpTable(gb)) {
    case 0x51c8: goto substate0;
    case 0x51ef: goto substate1;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x51c8, 0x51cb); push_effect(gb, 0x51cb); extendableBridge_checkSwitchStateChanged(gb);
  if (F & FZ) {
    CYCT(0x51cb, 0x51cc); ret_effect(gb); return;
  }
  CYC(0x51cb, 0x51cc);
  CYC(0x51cc, 0x51cf); SET_HL(extendableBridgeCreationData_bank08);
  extendableBridge_startLoadingBridgeData(gb);
  return;

substate1:
  CALL_C(0x51ef, interactionDecCounter1_hook, 0x23cc, 0x51f2);
  if (!(F & FZ)) {
    CYCT(0x51f2, 0x51f3); ret_effect(gb); return;
  }
  CYC(0x51f2, 0x51f3);
  CYC(0x51f3, 0x51f5); mem_wr(gb, HL, 0x0a);
  CYC(0x51f5, 0x51f8); push_effect(gb, 0x51f8); extendableBridge_updateNextTile(gb);
  CYC(0x51f8, 0x51f9); A = C;
  CYC(0x51f9, 0x51fa); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(0x51fa, 0x51fc); goto gotoNextState;
  }
  CYC(0x51fa, 0x51fc);
  CYC(0x51fc, 0x51fe); E = INTERACTION_BASE + 0x31; // var31
  CYC(0x51fe, 0x51ff); A = mem_rd(gb, DE);
  CALL_C(0x51ff, setTile_hook, 0x3a9c, 0x5202);
  CYC(0x5202, 0x5204); A = 0x70; // SND_DOORCLOSE
  CYC(0x5204, 0x5207); playSound_b00_hook(gb);
  return;

gotoNextState:
  CALL_C(0x5207, interactionIncState_hook, 0x23e0, 0x520a);
  CYC(0x520a, 0x520b); L = alu_inc8(gb, L);
  CYC(0x520b, 0x520d); mem_wr(gb, HL, 0x00);
  CYC(0x520d, 0x520e); ret_effect(gb);
}

// @state2: waiting for the switch to toggle to remove the bridge.
static void extendableBridge_state2(GB *gb, uint16_t sp0_) {
  CYC(0x520e, 0x5210); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5210, 0x5211); A = mem_rd(gb, DE);
  CYC(0x5211, 0x5212); push_effect(gb, 0x5212);
  switch (extendableBridge_jumpTable(gb)) {
    case 0x5216: goto substate0;
    case 0x521f: goto substate1;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x5216, 0x5219); push_effect(gb, 0x5219); extendableBridge_checkSwitchStateChanged(gb);
  if (F & FZ) {
    CYCT(0x5219, 0x521a); ret_effect(gb); return;
  }
  CYC(0x5219, 0x521a);
  CYC(0x521a, 0x521d); SET_HL(extendableBridgeRemovalData_bank08);
  CYC(0x521d, 0x521f);
  extendableBridge_startLoadingBridgeData(gb);
  return;

substate1:
  CALL_C(0x521f, interactionDecCounter1_hook, 0x23cc, 0x5222);
  if (!(F & FZ)) {
    CYCT(0x5222, 0x5223); ret_effect(gb); return;
  }
  CYC(0x5222, 0x5223);
  CYC(0x5223, 0x5225); mem_wr(gb, HL, 0x0a);
  CYC(0x5225, 0x5228); push_effect(gb, 0x5228); extendableBridge_updateNextTile(gb);
  CYC(0x5228, 0x5229); A = C;
  CYC(0x5229, 0x522a); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(0x522a, 0x522c); goto gotoState1;
  }
  CYC(0x522a, 0x522c);
  CYC(0x522c, 0x522e); E = INTERACTION_BASE + 0x31; // var31
  CYC(0x522e, 0x522f); A = mem_rd(gb, DE);
  CALL_C(0x522f, setTile_hook, 0x3a9c, 0x5232);
  CYC(0x5232, 0x5234); A = 0x70; // SND_DOORCLOSE
  CYC(0x5234, 0x5237); playSound_b00_hook(gb);
  return;

gotoState1:
  CYC(0x5237, 0x5238); H = D;
  CYC(0x5238, 0x523a); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x523a, 0x523c); mem_wr(gb, HL, 0x01);
  CYC(0x523c, 0x523d); L = alu_inc8(gb, L);
  CYC(0x523d, 0x523f); mem_wr(gb, HL, 0x00);
  CYC(0x523f, 0x5240); ret_effect(gb);
}

// INTERAC_EXTENDABLE_BRIDGE: extends or retracts a bridge tile by tile when a switch toggles.
void interactionCode23_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x518e, 0x5190); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5190, 0x5191); A = mem_rd(gb, DE);
  CYC(0x5191, 0x5192); push_effect(gb, 0x5192);
  switch (extendableBridge_jumpTable(gb)) {
    case 0x5198: extendableBridge_state0(gb); return;
    case 0x51c0: extendableBridge_state1(gb, sp0_); return;
    case 0x520e: extendableBridge_state2(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
