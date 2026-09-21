#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode9e);
  CYC(b_+428, b_+430); mem_wr(gb, hFF8C, A);
  CYC(b_+430, b_+431); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+431, b_+433); mem_wr(gb, hFF8F, A);
  CYC(b_+433, b_+434); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+434, b_+436); mem_wr(gb, hFF8E, A);
  CYC(b_+436, b_+437); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+437, b_+440); TAIL(setInterleavedTile); // jp
}

// @param a Position
void interactionCode9e_setHoleTile_hook(GB *gb) {
  BASE(interactionCode9e);
  CYC(b_+372, b_+373); C = A;
  CYC(b_+373, b_+375); A = 0xf3; // TILEINDEX_HOLE
  CYC(b_+375, b_+378); TAIL(setTile); // jp
}

// @param a Position
void interactionCode9e_setInterleavedPuddleHoleTile_hook(GB *gb) {
  BASE(interactionCode9e);
  CYC(b_+378, b_+381); SET_HL(b_+383);
  CYC(b_+381, b_+383); interactionCode9e_setInterleavedTile_tail(gb); // jr
}

// @param a Position
void interactionCode9e_setInterleavedHolePuddleTile_hook(GB *gb) {
  BASE(interactionCode9e);
  CYC(b_+386, b_+389); SET_HL(b_+391);
  CYC(b_+389, b_+391); interactionCode9e_setInterleavedTile_tail(gb); // jr
}

// @param a Position
void interactionCode9e_setGroundTile_hook(GB *gb) {
  BASE(interactionCode9e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+394, b_+395); push_effect(gb, BC);
  CYC(b_+395, b_+396); C = A;
  CYC(b_+396, b_+398); A = 0x1b;
  CALL_C(b_+398, setTile_hook, SYM(setTile), b_+401);
  CYC(b_+401, b_+402); SET_BC(pop_effect(gb));
  RET(b_+402); return;
}

// @param a Position
void interactionCode9e_setPuddleTile_hook(GB *gb) {
  BASE(interactionCode9e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+403, b_+404); push_effect(gb, BC);
  CYC(b_+404, b_+405); C = A;
  CYC(b_+405, b_+407); A = 0xf9; // TILEINDEX_PUDDLE
  CALL_C(b_+407, setTile_hook, SYM(setTile), b_+410);
  CYC(b_+410, b_+411); SET_BC(pop_effect(gb));
  RET(b_+411); return;
}

// @param a Position
void interactionCode9e_setInterleavedHoleGroundTile_hook(GB *gb) {
  BASE(interactionCode9e);
  CYC(b_+412, b_+415); SET_HL(b_+417);
  CYC(b_+415, b_+417); interactionCode9e_setInterleavedTile_tail(gb); // jr
}

// @param a Position
void interactionCode9e_setInterleavedGroundHoleTile_hook(GB *gb) {
  BASE(interactionCode9e);
  CYC(b_+420, b_+423); SET_HL(b_+425);
  CYC(b_+423, b_+425); interactionCode9e_setInterleavedTile_tail(gb); // jr
}

// 0b:5812, called (via a coincidental 12th, ROM-accurate extra invocation baked into the last
// call below) 12 times from interactionCode9e_swapRoomLayouts_hook.
static void interactionCode9e_xor_hook(GB *gb) {
  BASE(interactionCode9e);
  CYC(b_+669, b_+670); A = mem_rd(gb, HL);
  CYC(b_+670, b_+672); alu_xor(gb, 0x01);
  CYC(b_+672, b_+673); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  RET(b_+673); return;
}

// Swap the room layouts in all rooms affected by the flooding.
void interactionCode9e_swapRoomLayouts_hook(GB *gb) {
  BASE(interactionCode9e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+624, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+627);
  CYC(b_+627, b_+629); L = 0x40; // <ROOM_AGES_140
  CALL_L(b_+629, interactionCode9e_xor_hook, b_+632);
  CALL_L(b_+632, interactionCode9e_xor_hook, b_+635);
  CALL_L(b_+635, interactionCode9e_xor_hook, b_+638);
  CYC(b_+638, b_+640); L = 0x50; // <ROOM_AGES_150
  CALL_L(b_+640, interactionCode9e_xor_hook, b_+643);
  CALL_L(b_+643, interactionCode9e_xor_hook, b_+646);
  CALL_L(b_+646, interactionCode9e_xor_hook, b_+649);
  CYC(b_+649, b_+650); H = alu_dec8(gb, H);
  CYC(b_+650, b_+652); L = 0x40; // <ROOM_AGES_040
  CALL_L(b_+652, interactionCode9e_xor_hook, b_+655);
  CALL_L(b_+655, interactionCode9e_xor_hook, b_+658);
  CALL_L(b_+658, interactionCode9e_xor_hook, b_+661);
  CYC(b_+661, b_+663); L = 0x50; // <ROOM_AGES_050
  CALL_L(b_+663, interactionCode9e_xor_hook, b_+666);
  // The 11th (final, unconditional) `call $5812` below has a return address that coincidentally
  // equals the callee's own entry point (580f + 3 == 5812): after that call's own ret pops it and
  // resumes at 0x5812, execution simply re-enters @@xor a 12th time, whose ret is what actually
  // returns out of swapRoomLayouts to its real caller. Faithful to the ROM bytes.
  CALL_L(b_+666, interactionCode9e_xor_hook, b_+669);
  interactionCode9e_xor_hook(gb); return;
}

// ==================================================================================================
// INTERAC_WATER_PUSHBLOCK
// ==================================================================================================
void interactionCode9e_hook(GB *gb) {
  BASE(interactionCode9e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCode9e_jump_table(gb));
    if (jt_ == b_+8) { goto subid0; }
    else if (jt_ == b_+440) { goto subid1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (interactionCode9e_jump_table(gb));
    if (jt_ == b_+22) { goto subid0State0; }
    else if (jt_ == b_+55) { goto state1; }
    else if (jt_ == b_+140) { goto state2; }
    else if (jt_ == b_+155) { goto subid0State3; }
    else if (jt_ == SYM(objectPreventLinkFromPassing) && hook_enabled_at(gb, SYM(objectPreventLinkFromPassing))) { objectPreventLinkFromPassing_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0State0:
  CALL_C(b_+22, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+25);
  CYC(b_+25, b_+27); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+27, b_+30); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+27, b_+30);

initialize:
  CALL_C(b_+30, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+33);
  CALL_C(b_+33, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+36);
  CYC(b_+36, b_+38); A = 0x06;
  CALL_C(b_+38, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+41);
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+45, b_+47); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 30);
  CALL_C(b_+49, objectSetVisible82_hook, SYM(objectSetVisible82), b_+52);
  CYC(b_+52, b_+55); TAIL(interactionIncState); // jp

state1:
  CALL_C(b_+55, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+58);
  if (!(F & FC)) { CYCT(b_+58, b_+60); goto notPushing; } // jr nc
  CYC(b_+58, b_+60);
  CALL_C(b_+60, objectCheckLinkPushingAgainstCenter_hook, SYM(objectCheckLinkPushingAgainstCenter), b_+63);
  if (!(F & FC)) { CYCT(b_+63, b_+65); goto notPushing; } // jr nc
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); A = 0x01;
  CYC(b_+67, b_+70); mem_wr(gb, wForceLinkPushAnimation, A);
  CALL_C(b_+70, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+73);
  if (!(F & FZ)) { CYCT(b_+73, b_+74); ret_effect(gb); return; } // ret nz
  CYC(b_+73, b_+74);
  CYC(b_+74, b_+76); goto pushedLongEnough; // jr

notPushing:
  CYC(b_+76, b_+78); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+78, b_+80); A = 30;
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  RET(b_+81); return;

pushedLongEnough:
  CYC(b_+82, b_+84); C = 0x28;
  CALL_C(b_+84, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+87);
  CYC(b_+87, b_+88); B = A;
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); alu_or(gb, A);
  CYC(b_+92, b_+94); C = 0x02;
  if (F & FZ) { CYCT(b_+94, b_+96); goto l_55d7; } // jr z
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); C = 0x06;

l_55d7:
  CYC(b_+98, b_+99); A = B;
  CYC(b_+99, b_+100); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+100, b_+101); ret_effect(gb); return; } // ret nz
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+103); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+103, b_+105); A = (uint8_t)(mem_rd(gb, DE) ^ 0x04);
  CYC(b_+105, b_+106); mem_wr(gb, DE, A);
  CYC(b_+106, b_+107); H = D;
  CYC(b_+107, b_+109); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+109, b_+110); A = mem_rd(gb, HL);
  CYC(b_+110, b_+111); alu_add(gb, A);
  CYC(b_+111, b_+112); alu_add(gb, A);
  CYC(b_+112, b_+114); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+114, b_+115); mem_wr(gb, HL, A);
  CYC(b_+115, b_+117); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x40);
  CYC(b_+119, b_+121); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(b_+121, b_+124); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+124, b_+127); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+127, b_+129); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+129, playSound_b00_hook, SYM(playSound_b00), b_+132);
  CYC(b_+132, b_+134); A = 0x71; // SND_MOVEBLOCK
  CALL_C(b_+134, playSound_b00_hook, SYM(playSound_b00), b_+137);
  CYC(b_+137, b_+140); TAIL(interactionIncState); // jp

state2:
  CALL_C(b_+140, objectApplySpeed_hook, SYM(objectApplySpeed), b_+143);
  CALL_C(b_+143, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+146);
  CALL_C(b_+146, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+149);
  if (!(F & FZ)) { CYCT(b_+149, b_+150); ret_effect(gb); return; } // ret nz
  CYC(b_+149, b_+150);
  CYC(b_+150, b_+152); mem_wr(gb, HL, 70);
  CYC(b_+152, b_+155); TAIL(interactionIncState); // jp

subid0State3:
  CYC(b_+155, b_+157); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+157, b_+158); A = mem_rd(gb, DE);
  CYC(b_+158, b_+159); push_effect(gb, b_+159);
  do { uint16_t jt_ = (interactionCode9e_jump_table(gb));
    if (jt_ == b_+183) { goto subid0Substate0; }
    else if (jt_ == b_+207) { goto subid0Substate1; }
    else if (jt_ == b_+227) { goto subid0Substate2; }
    else if (jt_ == b_+247) { goto subid0Substate3; }
    else if (jt_ == b_+252) { goto subid0Substate4; }
    else if (jt_ == b_+257) { goto subid0Substate5; }
    else if (jt_ == b_+262) { goto subid0Substate6; }
    else if (jt_ == b_+281) { goto subid0Substate7; }
    else if (jt_ == b_+300) { goto subid0Substate8; }
    else if (jt_ == b_+314) { goto subid0Substate9; }
    else if (jt_ == b_+330) { goto substateA; }
    else if (jt_ == b_+349) { goto substateB; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0Substate0:
  CALL_C(b_+183, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+186);
  if (!(F & FZ)) { CYCT(b_+186, b_+187); ret_effect(gb); return; } // ret nz
  CYC(b_+186, b_+187);
  CYC(b_+187, b_+189); mem_wr(gb, HL, 0x08);
  CYC(b_+189, b_+191); A = 0xc2; // SND_FLOODGATES
  CALL_C(b_+191, playSound_b00_hook, SYM(playSound_b00), b_+194);
  CYC(b_+194, b_+196); A = 0x63;
  CALL_L(b_+196, interactionCode9e_setInterleavedHoleGroundTile_hook, b_+199);
  CYC(b_+199, b_+201); A = 0x65;
  CALL_L(b_+201, interactionCode9e_setInterleavedHoleGroundTile_hook, b_+204);
  CYC(b_+204, b_+207); TAIL(interactionIncSubstate); // jp

subid0Substate1:
  CYC(b_+207, b_+210); SET_BC((SYM(zelda_state0__initSubid00) + 5));

setGroundTilesWhenCounterIsZero:
  CALL_C(b_+210, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+213);
  if (!(F & FZ)) { CYCT(b_+213, b_+214); ret_effect(gb); return; } // ret nz
  CYC(b_+213, b_+214);
  CYC(b_+214, b_+216); mem_wr(gb, HL, 0x08);
  CYC(b_+216, b_+217); A = B;
  CALL_L(b_+217, interactionCode9e_setGroundTile_hook, b_+220);
  CYC(b_+220, b_+221); A = C;
  CALL_L(b_+221, interactionCode9e_setPuddleTile_hook, b_+224);
  CYC(b_+224, b_+227); TAIL(interactionIncSubstate); // jp

subid0Substate2:
  CYC(b_+227, b_+230); SET_BC((SYM(zora_subid1B__textTable) + 17));

setHoleTilesWhenCounterIsZero:
  CALL_C(b_+230, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+233);
  if (!(F & FZ)) { CYCT(b_+233, b_+234); ret_effect(gb); return; } // ret nz
  CYC(b_+233, b_+234);
  CYC(b_+234, b_+236); mem_wr(gb, HL, 0x08);
  CYC(b_+236, b_+237); A = B;
  CALL_L(b_+237, interactionCode9e_setInterleavedHoleGroundTile_hook, b_+240);
  CYC(b_+240, b_+241); A = C;
  CALL_L(b_+241, interactionCode9e_setInterleavedHoleGroundTile_hook, b_+244);
  CYC(b_+244, b_+247); TAIL(interactionIncSubstate); // jp

subid0Substate3:
  CYC(b_+247, b_+250); SET_BC((SYM(zora_subid1B__textTable) + 17));
  CYC(b_+250, b_+252); goto setGroundTilesWhenCounterIsZero; // jr

subid0Substate4:
  CYC(b_+252, b_+255); SET_BC((SYM(zora_subid0B__animationTable) + 9));
  CYC(b_+255, b_+257); goto setHoleTilesWhenCounterIsZero; // jr

subid0Substate5:
  CYC(b_+257, b_+260); SET_BC((SYM(zora_subid0B__animationTable) + 9));
  CYC(b_+260, b_+262); goto setGroundTilesWhenCounterIsZero; // jr

subid0Substate6:
  CALL_C(b_+262, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+265);
  if (!(F & FZ)) { CYCT(b_+265, b_+266); ret_effect(gb); return; } // ret nz
  CYC(b_+265, b_+266);
  CYC(b_+266, b_+268); mem_wr(gb, HL, 0x08);
  CYC(b_+268, b_+270); A = 0x60;
  CALL_L(b_+270, interactionCode9e_setInterleavedPuddleHoleTile_hook, b_+273);
  CYC(b_+273, b_+275); A = 0x68;
  CALL_L(b_+275, interactionCode9e_setInterleavedHoleGroundTile_hook, b_+278);
  CYC(b_+278, b_+281); TAIL(interactionIncSubstate); // jp

subid0Substate7:
  CALL_C(b_+281, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+284);
  if (!(F & FZ)) { CYCT(b_+284, b_+285); ret_effect(gb); return; } // ret nz
  CYC(b_+284, b_+285);
  CYC(b_+285, b_+287); mem_wr(gb, HL, 0x08);
  CYC(b_+287, b_+289); A = 0x60;
  CALL_L(b_+289, interactionCode9e_setHoleTile_hook, b_+292);
  CYC(b_+292, b_+294); A = 0x68;
  CALL_L(b_+294, interactionCode9e_setPuddleTile_hook, b_+297);
  CYC(b_+297, b_+300); TAIL(interactionIncSubstate); // jp

subid0Substate8:
  CALL_C(b_+300, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+303);
  if (!(F & FZ)) { CYCT(b_+303, b_+304); ret_effect(gb); return; } // ret nz
  CYC(b_+303, b_+304);
  CYC(b_+304, b_+306); mem_wr(gb, HL, 0x08);
  CYC(b_+306, b_+308); A = 0x69;
  CALL_L(b_+308, interactionCode9e_setInterleavedPuddleHoleTile_hook, b_+311);
  CYC(b_+311, b_+314); TAIL(interactionIncSubstate); // jp

subid0Substate9:
  CALL_C(b_+314, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+317);
  if (!(F & FZ)) { CYCT(b_+317, b_+318); ret_effect(gb); return; } // ret nz
  CYC(b_+317, b_+318);
  CYC(b_+318, b_+320); mem_wr(gb, HL, 90);
  CYC(b_+320, b_+322); C = 0x69;

setWaterTileAndIncSubstate:
  CYC(b_+322, b_+324); A = 0xfa; // TILEINDEX_WATER
  CALL_C(b_+324, setTile_hook, SYM(setTile), b_+327);
  CYC(b_+327, b_+330); TAIL(interactionIncSubstate); // jp

substateA:
  CALL_C(b_+330, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+333);
  if (!(F & FZ)) { CYCT(b_+333, b_+334); ret_effect(gb); return; } // ret nz
  CYC(b_+333, b_+334);
  CYC(b_+334, b_+336); mem_wr(gb, HL, 72);
  CYC(b_+336, b_+338); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+338, playSound_b00_hook, SYM(playSound_b00), b_+341);
  CYC(b_+341, b_+343); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+343, playSound_b00_hook, SYM(playSound_b00), b_+346);
  CYC(b_+346, b_+349); TAIL(interactionIncSubstate); // jp

substateB:
  CALL_C(b_+349, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+352);
  if (!(F & FZ)) { CYCT(b_+352, b_+353); ret_effect(gb); return; } // ret nz
  CYC(b_+352, b_+353);
  CYC(b_+353, b_+356); A = mem_rd(gb, wActiveMusic);
  CALL_C(b_+356, playSound_b00_hook, SYM(playSound_b00), b_+359);
  CYC(b_+359, b_+360); alu_xor(gb, A);
  CYC(b_+360, b_+363); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+363, b_+366); mem_wr(gb, wMenuDisabled, A);
  CALL_L(b_+366, interactionCode9e_swapRoomLayouts_hook, b_+369);
  CYC(b_+369, b_+372); TAIL(interactionIncState); // jp

subid1:
  CYC(b_+440, b_+442); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+442, b_+443); A = mem_rd(gb, DE);
  CYC(b_+443, b_+444); push_effect(gb, b_+444);
  do { uint16_t jt_ = (interactionCode9e_jump_table(gb));
    if (jt_ == b_+454) { goto subid1State0; }
    else if (jt_ == b_+55) { goto state1; }
    else if (jt_ == b_+140) { goto state2; }
    else if (jt_ == b_+465) { goto subid1State3; }
    else if (jt_ == SYM(objectPreventLinkFromPassing) && hook_enabled_at(gb, SYM(objectPreventLinkFromPassing))) { objectPreventLinkFromPassing_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid1State0:
  CALL_C(b_+454, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+457);
  CYC(b_+457, b_+459); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+459, b_+462); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+459, b_+462);
  CYC(b_+462, b_+465); goto initialize; // jp (local)

subid1State3:
  CYC(b_+465, b_+467); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+467, b_+468); A = mem_rd(gb, DE);
  CYC(b_+468, b_+469); push_effect(gb, b_+469);
  do { uint16_t jt_ = (interactionCode9e_jump_table(gb));
    if (jt_ == b_+493) { goto subid1Substate0; }
    else if (jt_ == b_+517) { goto subid1Substate1; }
    else if (jt_ == b_+523) { goto subid1Substate2; }
    else if (jt_ == b_+543) { goto subid1Substate3; }
    else if (jt_ == b_+549) { goto subid1Substate4; }
    else if (jt_ == b_+555) { goto subid1Substate5; }
    else if (jt_ == b_+561) { goto subid1Substate6; }
    else if (jt_ == b_+580) { goto subid1Substate7; }
    else if (jt_ == b_+596) { goto subid1Substate8; }
    else if (jt_ == b_+610) { goto subid1Substate9; }
    else if (jt_ == b_+330) { goto substateA; }
    else if (jt_ == b_+349) { goto substateB; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid1Substate0:
  CALL_C(b_+493, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+496);
  if (!(F & FZ)) { CYCT(b_+496, b_+497); ret_effect(gb); return; } // ret nz
  CYC(b_+496, b_+497);
  CYC(b_+497, b_+499); mem_wr(gb, HL, 0x08);
  CYC(b_+499, b_+501); A = 0xc2; // SND_FLOODGATES
  CALL_C(b_+501, playSound_b00_hook, SYM(playSound_b00), b_+504);
  CYC(b_+504, b_+506); A = 0x63;
  CALL_L(b_+506, interactionCode9e_setInterleavedGroundHoleTile_hook, b_+509);
  CYC(b_+509, b_+511); A = 0x65;
  CALL_L(b_+511, interactionCode9e_setInterleavedGroundHoleTile_hook, b_+514);
  CYC(b_+514, b_+517); TAIL(interactionIncSubstate); // jp

subid1Substate1:
  CYC(b_+517, b_+520); SET_BC((SYM(creditsTextHorizontal_6559) + 10));
  CYC(b_+520, b_+523); goto setGroundTilesWhenCounterIsZero; // jp (local)

subid1Substate2:
  CYC(b_+523, b_+526); SET_BC((SYM(interactionCodeaf__state1) + 1));

setHoleTilesWhenCounterZero_2:
  CALL_C(b_+526, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+529);
  if (!(F & FZ)) { CYCT(b_+529, b_+530); ret_effect(gb); return; } // ret nz
  CYC(b_+529, b_+530);
  CYC(b_+530, b_+532); mem_wr(gb, HL, 0x08);
  CYC(b_+532, b_+533); A = B;
  CALL_L(b_+533, interactionCode9e_setInterleavedGroundHoleTile_hook, b_+536);
  CYC(b_+536, b_+537); A = C;
  CALL_L(b_+537, interactionCode9e_setInterleavedGroundHoleTile_hook, b_+540);
  CYC(b_+540, b_+543); TAIL(interactionIncSubstate); // jp

subid1Substate3:
  CYC(b_+543, b_+546); SET_BC((SYM(interactionCodeaf__state1) + 1));
  CYC(b_+546, b_+549); goto setGroundTilesWhenCounterIsZero; // jp (local)

subid1Substate4:
  CYC(b_+549, b_+552); SET_BC((SYM(zora_subid0B__animationTable) + 9));
  CYC(b_+552, b_+555); goto setHoleTilesWhenCounterZero_2; // jp (local)

subid1Substate5:
  CYC(b_+555, b_+558); SET_BC((SYM(twinrovaInCutscene_loadScript) + 1));
  CYC(b_+558, b_+561); goto setGroundTilesWhenCounterIsZero; // jp (local)

subid1Substate6:
  CALL_C(b_+561, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+564);
  if (!(F & FZ)) { CYCT(b_+564, b_+565); ret_effect(gb); return; } // ret nz
  CYC(b_+564, b_+565);
  CYC(b_+565, b_+567); mem_wr(gb, HL, 0x08);
  CYC(b_+567, b_+569); A = 0x60;
  CALL_L(b_+569, interactionCode9e_setInterleavedHolePuddleTile_hook, b_+572);
  CYC(b_+572, b_+574); A = 0x68;
  CALL_L(b_+574, interactionCode9e_setInterleavedGroundHoleTile_hook, b_+577);
  CYC(b_+577, b_+580); TAIL(interactionIncSubstate); // jp

subid1Substate7:
  CALL_C(b_+580, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+583);
  if (!(F & FZ)) { CYCT(b_+583, b_+584); ret_effect(gb); return; } // ret nz
  CYC(b_+583, b_+584);
  CYC(b_+584, b_+586); mem_wr(gb, HL, 0x08);
  CYC(b_+586, b_+588); A = 0x68;
  CALL_L(b_+588, interactionCode9e_setGroundTile_hook, b_+591);
  CYC(b_+591, b_+593); C = 0x60;
  CYC(b_+593, b_+596); goto setWaterTileAndIncSubstate; // jp (local)

subid1Substate8:
  CALL_C(b_+596, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+599);
  if (!(F & FZ)) { CYCT(b_+599, b_+600); ret_effect(gb); return; } // ret nz
  CYC(b_+599, b_+600);
  CYC(b_+600, b_+602); mem_wr(gb, HL, 0x08);
  CYC(b_+602, b_+604); A = 0x69;
  CALL_L(b_+604, interactionCode9e_setInterleavedHolePuddleTile_hook, b_+607);
  CYC(b_+607, b_+610); TAIL(interactionIncSubstate); // jp

subid1Substate9:
  CALL_C(b_+610, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+613);
  if (!(F & FZ)) { CYCT(b_+613, b_+614); ret_effect(gb); return; } // ret nz
  CYC(b_+613, b_+614);
  CYC(b_+614, b_+616); mem_wr(gb, HL, 90);
  CYC(b_+616, b_+618); A = 0x69;
  CALL_L(b_+618, interactionCode9e_setHoleTile_hook, b_+621);
  CYC(b_+621, b_+624); TAIL(interactionIncSubstate); // jp
}
