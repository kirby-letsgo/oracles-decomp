#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Pointer tables indexed by Interaction.xh. Each entry: tile index, then positions
// to write it at, terminated by $ff.
#define extendableBridgeCreationData_bank08 SYM(interactionCode23__bridgeCreationData)
#define extendableBridgeRemovalData_bank08 SYM(interactionCode23__bridgeRemovalData)

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
  BASE(interactionCode23);
  CYC(b_+178, b_+181); A = mem_rd(gb, wSwitchState);
  CYC(b_+181, b_+182); B = A;
  CYC(b_+182, b_+184); E = INTERACTION_BASE + 0x30; // var30
  CYC(b_+184, b_+185); A = mem_rd(gb, DE);
  CYC(b_+185, b_+186); alu_xor(gb, B);
  CYC(b_+186, b_+187); B = A;
  CYC(b_+187, b_+189); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+189, b_+190); A = mem_rd(gb, DE);
  CYC(b_+190, b_+191); alu_and(gb, B);
  CYC(b_+191, b_+192); ret_effect(gb);
}

// interactionCode23@updateNextTile: reads the next position byte into C from the pointer
// in relatedObj2, and advances that pointer.
static void extendableBridge_updateNextTile(GB *gb) {
  BASE(interactionCode23);
  CYC(b_+192, b_+193); H = D;
  CYC(b_+193, b_+195); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+195, b_+196); E = L;
  CYC(b_+196, b_+197); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+197, b_+198); H = mem_rd(gb, HL);
  CYC(b_+198, b_+199); L = A;
  CYC(b_+199, b_+200); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+200, b_+201); C = A;
  CYC(b_+201, b_+202); A = L;
  CYC(b_+202, b_+203); mem_wr(gb, DE, A);
  CYC(b_+203, b_+204); E = alu_inc8(gb, E);
  CYC(b_+204, b_+205); A = H;
  CYC(b_+205, b_+206); mem_wr(gb, DE, A);
  CYC(b_+206, b_+207); ret_effect(gb);
}

// @startLoadingBridgeData: HL points at a pointer table; select the entry for xh, record
// the tile index in var31 and the position pointer in relatedObj2, then start counting.
static void extendableBridge_startLoadingBridgeData(GB *gb) {
  BASE(interactionCode23);
  CYC(b_+65, b_+67); E = INTERACTION_BASE + 0x30; // var30
  CYC(b_+67, b_+70); A = mem_rd(gb, wSwitchState);
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+73); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); extendableBridge_addDoubleIndex(gb, b_+75);
  CYC(b_+75, b_+76); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+76, b_+77); H = mem_rd(gb, HL);
  CYC(b_+77, b_+78); L = A;
  CYC(b_+78, b_+79); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+79, b_+81); E = INTERACTION_BASE + 0x31; // var31
  CYC(b_+81, b_+82); mem_wr(gb, DE, A);
  CYC(b_+82, b_+84); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+84, b_+85); A = L;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+87); E = alu_inc8(gb, E);
  CYC(b_+87, b_+88); A = H;
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+91); A = 0x0a;
  CYC(b_+91, b_+93); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, b_+97); interactionIncSubstate_hook(gb);
}

// @state0: var03 = bitmask for subid; go to state 2 if a bridge is already here,
// state 1 otherwise; remember the current switch state.
static void extendableBridge_state0(GB *gb) {
  BASE(interactionCode23);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); alu_and(gb, 0x07);
  CYC(b_+16, b_+19); SET_HL(bitTable);
  CYC(b_+19, b_+20); alu_add(gb, L);
  CYC(b_+20, b_+21); L = A;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); E = alu_inc8(gb, E);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); C = A;
  CYC(b_+28, b_+30); B = wRoomLayout >> 8;
  CYC(b_+30, b_+31); A = mem_rd(gb, BC);
  CYC(b_+31, b_+33); alu_sub(gb, 0x6a); // TILEINDEX_VERTICAL_BRIDGE
  CYC(b_+33, b_+35); alu_sub(gb, 0x06);
  CYC(b_+35, b_+37); A = 0x02;
  if (F & FC) {
    CYCT(b_+37, b_+39);
  } else {
    CYC(b_+37, b_+39);
    CYC(b_+39, b_+40); A = alu_dec8(gb, A);
  }
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + 0x30; // var30
  CYC(b_+45, b_+48); A = mem_rd(gb, wSwitchState);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); ret_effect(gb);
}

// @state1: waiting for the switch to toggle to create the bridge.
static void extendableBridge_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode23);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); push_effect(gb, b_+54);
  do { uint16_t jt_ = (extendableBridge_jumpTable(gb));
    if (jt_ == b_+58) { goto substate0; }
    else if (jt_ == b_+97) { goto substate1; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+58, b_+61); push_effect(gb, b_+61); extendableBridge_checkSwitchStateChanged(gb);
  if (F & FZ) {
    CYCT(b_+61, b_+62); ret_effect(gb); return;
  }
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+65); SET_HL(extendableBridgeCreationData_bank08);
  extendableBridge_startLoadingBridgeData(gb);
  return;

substate1:
  CALL_C(b_+97, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+100);
  if (!(F & FZ)) {
    CYCT(b_+100, b_+101); ret_effect(gb); return;
  }
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x0a);
  CYC(b_+103, b_+106); push_effect(gb, b_+106); extendableBridge_updateNextTile(gb);
  CYC(b_+106, b_+107); A = C;
  CYC(b_+107, b_+108); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+108, b_+110); goto gotoNextState;
  }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); E = INTERACTION_BASE + 0x31; // var31
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  CALL_C(b_+113, setTile_hook, SYM(setTile), b_+116);
  CYC(b_+116, b_+118); A = 0x70; // SND_DOORCLOSE
  CYC(b_+118, b_+121); playSound_b00_hook(gb);
  return;

gotoNextState:
  CALL_C(b_+121, interactionIncState_hook, SYM(interactionIncState), b_+124);
  CYC(b_+124, b_+125); L = alu_inc8(gb, L);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+128); ret_effect(gb);
}

// @state2: waiting for the switch to toggle to remove the bridge.
static void extendableBridge_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode23);
  CYC(b_+128, b_+130); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+132); push_effect(gb, b_+132);
  do { uint16_t jt_ = (extendableBridge_jumpTable(gb));
    if (jt_ == b_+136) { goto substate0; }
    else if (jt_ == b_+145) { goto substate1; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+136, b_+139); push_effect(gb, b_+139); extendableBridge_checkSwitchStateChanged(gb);
  if (F & FZ) {
    CYCT(b_+139, b_+140); ret_effect(gb); return;
  }
  CYC(b_+139, b_+140);
  CYC(b_+140, b_+143); SET_HL(extendableBridgeRemovalData_bank08);
  CYC(b_+143, b_+145);
  extendableBridge_startLoadingBridgeData(gb);
  return;

substate1:
  CALL_C(b_+145, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+148);
  if (!(F & FZ)) {
    CYCT(b_+148, b_+149); ret_effect(gb); return;
  }
  CYC(b_+148, b_+149);
  CYC(b_+149, b_+151); mem_wr(gb, HL, 0x0a);
  CYC(b_+151, b_+154); push_effect(gb, b_+154); extendableBridge_updateNextTile(gb);
  CYC(b_+154, b_+155); A = C;
  CYC(b_+155, b_+156); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+156, b_+158); goto gotoState1;
  }
  CYC(b_+156, b_+158);
  CYC(b_+158, b_+160); E = INTERACTION_BASE + 0x31; // var31
  CYC(b_+160, b_+161); A = mem_rd(gb, DE);
  CALL_C(b_+161, setTile_hook, SYM(setTile), b_+164);
  CYC(b_+164, b_+166); A = 0x70; // SND_DOORCLOSE
  CYC(b_+166, b_+169); playSound_b00_hook(gb);
  return;

gotoState1:
  CYC(b_+169, b_+170); H = D;
  CYC(b_+170, b_+172); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+172, b_+174); mem_wr(gb, HL, 0x01);
  CYC(b_+174, b_+175); L = alu_inc8(gb, L);
  CYC(b_+175, b_+177); mem_wr(gb, HL, 0x00);
  CYC(b_+177, b_+178); ret_effect(gb);
}

// INTERAC_EXTENDABLE_BRIDGE: extends or retracts a bridge tile by tile when a switch toggles.
void interactionCode23_hook(GB *gb) {
  BASE(interactionCode23);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (extendableBridge_jumpTable(gb));
    if (jt_ == b_+10) { extendableBridge_state0(gb); return; }
    else if (jt_ == b_+50) { extendableBridge_state1(gb, sp0_); return; }
    else if (jt_ == b_+128) { extendableBridge_state2(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
