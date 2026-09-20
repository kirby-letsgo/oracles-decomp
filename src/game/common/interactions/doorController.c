#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Data format: b0 = tile to transition into, b1 = tile to transition from.
#define doorControllerShutterTiles_bank08 SYM(interactionCode1e__shutterTiles)
#define doorControllerScriptSubidTable_bank08 SYM(interactionCode1e__scriptSubidTable)

static uint16_t doorController_jumpTable(GB *gb) {
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

static void doorController_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void doorController_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode1e@playSoundIfInScreenBoundary: plays sound A if the door is on screen.
// Reached only by static `call`s from interactionCode1e itself.
static void doorController_playSoundIfInScreenBoundary(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+297, b_+299); H8(hFF8B) = A;
  CALL_C(b_+299, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+302);
  if (!(F & FC)) {
    CYCT(b_+302, b_+303); ret_effect(gb); return;
  }
  CYC(b_+302, b_+303);
  CYC(b_+303, b_+305); A = H8(hFF8B);
  CYC(b_+305, b_+308); playSound_b00_hook(gb);
}

// interactionCode1e@checkRespawnLink: force Link to respawn if he's on the same tile as
// this object.
static void doorController_checkRespawnLink(GB *gb) {
  BASE(interactionCode1e);
  CYC(b_+229, b_+232); A = W8(w1Link_yh);
  CYC(b_+232, b_+234); alu_and(gb, 0xf0);
  CYC(b_+234, b_+235); B = A;
  CYC(b_+235, b_+238); A = W8(w1Link_xh);
  CYC(b_+238, b_+240); A = alu_swap(gb, A);
  CYC(b_+240, b_+242); alu_and(gb, 0x0f);
  CYC(b_+242, b_+243); alu_or(gb, B);
  CYC(b_+243, b_+244); B = A;
  CYC(b_+244, b_+246); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+246, b_+247); A = mem_rd(gb, DE);
  CYC(b_+247, b_+248); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+248, b_+249); ret_effect(gb); return;
  }
  CYC(b_+248, b_+249);
  CYC(b_+249, b_+251); A = 0x02;
  CYC(b_+251, b_+254); mem_wr(gb, wScreenTransitionDelay, A);
  CYC(b_+254, b_+257); respawnLink_hook(gb);
}

// interactionCode1e@func_47ee: for subids >= 4, count this door as one more closed door.
static void doorController_func_47ee(GB *gb) {
  BASE(interactionCode1e);
  CYC(b_+266, b_+268); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+268, b_+269); A = mem_rd(gb, DE);
  CYC(b_+269, b_+271); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+271, b_+272); ret_effect(gb); return;
  }
  CYC(b_+271, b_+272);
  CYC(b_+272, b_+275); SET_HL(wcc93);
  CYC(b_+275, b_+276); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+276, b_+277); ret_effect(gb);
}

// interactionCode1e@func_47e5: if the door's tile is not solid, fall through into func_47ee.
static void doorController_func_47e5(GB *gb) {
  BASE(interactionCode1e);
  CYC(b_+257, b_+259); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+259, b_+260); A = mem_rd(gb, DE);
  CYC(b_+260, b_+261); C = A;
  CYC(b_+261, b_+263); B = wRoomCollisions >> 8;
  CYC(b_+263, b_+264); A = mem_rd(gb, BC);
  CYC(b_+264, b_+265); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+265, b_+266); ret_effect(gb); return;
  }
  CYC(b_+265, b_+266);
  doorController_func_47ee(gb);
}

// interactionCode1e@func_47f9: for subids >= 4, count one closed door fewer.
static void doorController_func_47f9(GB *gb) {
  BASE(interactionCode1e);
  CYC(b_+277, b_+279); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+279, b_+280); A = mem_rd(gb, DE);
  CYC(b_+280, b_+282); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+282, b_+283); ret_effect(gb); return;
  }
  CYC(b_+282, b_+283);
  CYC(b_+283, b_+286); SET_HL(wcc93);
  CYC(b_+286, b_+287); A = mem_rd(gb, HL);
  CYC(b_+287, b_+288); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+288, b_+289); ret_effect(gb); return;
  }
  CYC(b_+288, b_+289);
  CYC(b_+289, b_+290); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+290, b_+291); A = mem_rd(gb, HL);
  CYC(b_+291, b_+293); alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(b_+293, b_+294); ret_effect(gb); return;
  }
  CYC(b_+293, b_+294);
  CYC(b_+294, b_+296); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+296, b_+297); ret_effect(gb);
}

// @state1: run the script; it decides when to move to state 2 (open) or 3 (close).
static void doorController_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CALL_C(b_+72, interactionRunScript_hook, SYM(interactionRunScript), b_+75);
  if (F & FC) {
    CYCT(b_+75, b_+78); interactionDelete_hook(gb); return;
  }
  CYC(b_+75, b_+78);
  CYC(b_+78, b_+80); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+80, b_+81); alu_xor(gb, A);
  CYC(b_+81, b_+82); mem_wr(gb, DE, A);
  CYC(b_+82, b_+83); ret_effect(gb);
}

// @gotoState1: back to state 1 / substate 0, then run state 1 immediately.
static void doorController_gotoState1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+218, b_+220); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+220, b_+222); A = 0x01;
  CYC(b_+222, b_+223); mem_wr(gb, DE, A);
  CYC(b_+223, b_+224); E = alu_inc8(gb, E);
  CYC(b_+224, b_+225); alu_xor(gb, A);
  CYC(b_+225, b_+226); mem_wr(gb, DE, A);
  CYC(b_+226, b_+229);
  doorController_state1(gb, sp0_);
}

// @setTileAndPlaySound: HL points at the final tile; write it and play the door sound.
static void doorController_setTileAndPlaySound(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+205, b_+207); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+207, b_+208); A = mem_rd(gb, DE);
  CYC(b_+208, b_+209); C = A;
  CYC(b_+209, b_+210); A = mem_rd(gb, HL);
  CALL_C(b_+210, setTile_hook, SYM(setTile), b_+213);
  CYC(b_+213, b_+215); A = 0x70; // SND_DOORCLOSE
  CYC(b_+215, b_+218); push_effect(gb, b_+218); doorController_playSoundIfInScreenBoundary(gb, sp0_);
  doorController_gotoState1(gb, sp0_);
}

// @interleaveDoorTile: start the half-open/half-closed door animation tile.
static void doorController_interleaveDoorTile(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+101, b_+103); A = 0x70; // SND_DOORCLOSE
  CYC(b_+103, b_+106); push_effect(gb, b_+106); doorController_playSoundIfInScreenBoundary(gb, sp0_);
  CYC(b_+106, b_+108); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+108, b_+109); A = mem_rd(gb, DE);
  CYC(b_+109, b_+112); SET_HL(doorControllerShutterTiles_bank08);
  CYC(b_+112, b_+113); push_effect(gb, b_+113); doorController_addAToHl(gb);
  CYC(b_+113, b_+115); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+118); H8(hFF8C) = A;
  CYC(b_+118, b_+119); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+119, b_+121); H8(hFF8F) = A;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+122, b_+124); H8(hFF8E) = A;
  CYC(b_+124, b_+126); alu_and(gb, 0x03);
  CALL_C(b_+126, setInterleavedTile_hook, SYM(setInterleavedTile), b_+129);
  CYC(b_+129, b_+131); A = H8(hActiveObject);
  CYC(b_+131, b_+132); D = A;
  CYC(b_+132, b_+133); H = D;
  CYC(b_+133, b_+135); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+136, b_+138); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x06);
  // Set the new tile in the room layout (without calling setTile, so no visual update yet).
  CYC(b_+140, b_+142); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+142, b_+143); C = mem_rd(gb, HL);
  CYC(b_+143, b_+145); B = wRoomLayout >> 8;
  CYC(b_+145, b_+147); A = H8(hFF8F);
  CYC(b_+147, b_+148); mem_wr(gb, BC, A);
  CYC(b_+148, b_+149); ret_effect(gb);
}

// @state0: init. "xh" is a parameter (0-7): a bit index for wActiveTriggers.
static void doorController_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+36); alu_and(gb, 0x07);
  CYC(b_+36, b_+39); SET_BC(bitTable);
  CYC(b_+39, b_+40); alu_add(gb, C);
  CYC(b_+40, b_+41); C = A;
  CYC(b_+41, b_+42); A = mem_rd(gb, BC);
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  // Convert short-form position in yh to a full y/x position
  CYC(b_+45, b_+47); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+49, b_+50); A = mem_rd(gb, HL);
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+53, setShortPosition_hook, SYM(setShortPosition), b_+56);
  // Pick the script from the subid
  CYC(b_+56, b_+58); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+62); SET_HL(doorControllerScriptSubidTable_bank08);
  CYC(b_+62, b_+63); doorController_addDoubleIndex(gb, b_+63);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+64, b_+65); H = mem_rd(gb, HL);
  CYC(b_+65, b_+66); L = A;
  CALL_C(b_+66, interactionSetScript_hook, SYM(interactionSetScript), b_+69);
  CYC(b_+69, b_+72); push_effect(gb, b_+72); doorController_func_47e5(gb);
  doorController_state1(gb, sp0_);
}

// @state2: a door is opening
static void doorController_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+83, b_+86); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+87, b_+88); ret_effect(gb); return;
  }
  CYC(b_+87, b_+88);
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); push_effect(gb, b_+92);
  do { uint16_t jt_ = (doorController_jumpTable(gb));
    if (jt_ == b_+96) { goto substate0; }
    else if (jt_ == b_+149) { goto substate1; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  // The tile at this position must be solid
  CALL_C(b_+96, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+99);
  if (!(F & FC)) {
    CYCT(b_+99, b_+101); doorController_gotoState1(gb, sp0_); return;
  }
  CYC(b_+99, b_+101);
  doorController_interleaveDoorTile(gb, sp0_);
  return;

substate1:
  CALL_C(b_+149, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+152);
  if (!(F & FZ)) {
    CYCT(b_+152, b_+153); ret_effect(gb); return;
  }
  CYC(b_+152, b_+153);
  // Door will now open fully
  CYC(b_+153, b_+156); push_effect(gb, b_+156); doorController_func_47ee(gb);
  CYC(b_+156, b_+158); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+158, b_+159); A = mem_rd(gb, DE);
  CYC(b_+159, b_+162); SET_HL(doorControllerShutterTiles_bank08);
  CYC(b_+162, b_+163); push_effect(gb, b_+163); doorController_addAToHl(gb);
  CYC(b_+163, b_+165);
  doorController_setTileAndPlaySound(gb, sp0_);
}

// @state3: a door is closing
static void doorController_state3(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+165, b_+167); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+169); push_effect(gb, b_+169);
  do { uint16_t jt_ = (doorController_jumpTable(gb));
    if (jt_ == b_+173) { goto substate0; }
    else if (jt_ == b_+187) { goto substate1; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  // The tile at this position must not be solid (a somaria block counts as not solid)
  CALL_C(b_+173, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+176);
  CYC(b_+176, b_+178); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
  if (F & FZ) {
    CYCT(b_+178, b_+180); doorController_interleaveDoorTile(gb, sp0_); return;
  }
  CYC(b_+178, b_+180);
  CALL_C(b_+180, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+183);
  if (F & FC) {
    CYCT(b_+183, b_+185); doorController_gotoState1(gb, sp0_); return;
  }
  CYC(b_+183, b_+185);
  CYC(b_+185, b_+187);
  doorController_interleaveDoorTile(gb, sp0_);
  return;

substate1:
  CALL_C(b_+187, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+190);
  if (!(F & FZ)) {
    CYCT(b_+190, b_+191); ret_effect(gb); return;
  }
  CYC(b_+190, b_+191);
  // Door will now close fully
  CYC(b_+191, b_+194); push_effect(gb, b_+194); doorController_checkRespawnLink(gb);
  CYC(b_+194, b_+197); push_effect(gb, b_+197); doorController_func_47f9(gb);
  CYC(b_+197, b_+199); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+199, b_+200); A = mem_rd(gb, DE);
  CYC(b_+200, b_+203); SET_HL(doorControllerShutterTiles_bank08);
  CYC(b_+203, b_+204); push_effect(gb, b_+204); doorController_addAToHl(gb);
  CYC(b_+204, b_+205); SET_HL(HL + 1);
  doorController_setTileAndPlaySound(gb, sp0_);
}

// INTERAC_DOOR_CONTROLLER: opens/closes shutter, key, boss and minecart doors under
// script control.
void interactionCode1e_hook(GB *gb) {
  BASE(interactionCode1e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wSwitchHookState);
  CYC(b_+9, b_+11); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (doorController_jumpTable(gb));
    if (jt_ == b_+24) { doorController_state0(gb, sp0_); return; }
    else if (jt_ == b_+72) { doorController_state1(gb, sp0_); return; }
    else if (jt_ == b_+83) { doorController_state2(gb, sp0_); return; }
    else if (jt_ == b_+165) { doorController_state3(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
