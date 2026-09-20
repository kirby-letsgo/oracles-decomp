#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Per-direction y/x offsets applied when the cube rolls one tile.
#define coloredCubeDirectionOffsets_bank08 SYM(interactionCode19__directionOffsets)
// Rows: cube orientation (direction); columns: push angle.
#define coloredCubeAnimations_bank08 SYM(interactionCode19__animations)
// Cube orientation -> wRotatingCubeColor.
#define coloredCubeColors_bank08 SYM(interactionCode19__colors)

#define TILEINDEX_CRACKED_FLOOR 0x4d
#define TILEINDEX_HOLE 0xf3
#define PALH_89 0x89
#define SND_MOVE_BLOCK_2 0x7f

static uint16_t coloredCube_jumpTable(GB *gb) {
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

static void coloredCube_addAToHl(GB *gb) {
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

// interactionCode19@setColor: sets wRotatingCubeColor and the animation from the cube's
// orientation in A. Tails into interactionSetAnimation, whose ret pops our return address.
static void coloredCube_setColor(GB *gb) {
  BASE(interactionCode19);
  CYC(b_+271, b_+272); B = A;
  CYC(b_+272, b_+275); SET_HL(coloredCubeColors_bank08);
  CYC(b_+275, b_+276); push_effect(gb, b_+276); coloredCube_addAToHl(gb);
  CYC(b_+276, b_+277); A = mem_rd(gb, HL);
  CYC(b_+277, b_+280); mem_wr(gb, wRotatingCubeColor, A);
  CYC(b_+280, b_+281); A = B;
  CYC(b_+281, b_+284); interactionSetAnimation_hook(gb);
}

// interactionCode19@checkLinkPushingTowardBlock: b = push direction, zflag set if Link is
// pushing toward the block.
static void coloredCube_checkLinkPushingTowardBlock(GB *gb, uint16_t sp0_) {
  BASE(interactionCode19);
  CYC(b_+207, b_+210); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+210, b_+211); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+211, b_+212); ret_effect(gb); return;
  }
  CYC(b_+211, b_+212);
  CYC(b_+212, b_+215); A = mem_rd(gb, wLinkAngle);
  CYC(b_+215, b_+216); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+216, b_+217); ret_effect(gb); return;
  }
  CYC(b_+216, b_+217);
  CYC(b_+217, b_+220); A = W8(w1Link_zh);
  CYC(b_+220, b_+221); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+221, b_+222); ret_effect(gb); return;
  }
  CYC(b_+221, b_+222);
  CYC(b_+222, b_+225); A = mem_rd(gb, wGameKeysPressed);
  CYC(b_+225, b_+227); alu_and(gb, 0x03); // BTN_A | BTN_B
  if (!(F & FZ)) {
    CYCT(b_+227, b_+228); ret_effect(gb); return;
  }
  CYC(b_+227, b_+228);
  CYC(b_+228, b_+230); C = 0x14;
  CALL_C(b_+230, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+233);
  if (!(F & FC)) {
    CYCT(b_+233, b_+235); goto notWithinDistance;
  }
  CYC(b_+233, b_+235);
  CYC(b_+235, b_+237); A = alu_srl(gb, A);
  CYC(b_+237, b_+239); alu_xor(gb, 0x02);
  CYC(b_+239, b_+240); B = A;
  CYC(b_+240, b_+243); A = W8(w1Link_direction);
  CYC(b_+243, b_+244); alu_cp(gb, B);
  CYC(b_+244, b_+245); ret_effect(gb);
  return;
notWithinDistance:
  CYC(b_+245, b_+246); alu_or(gb, D);
  CYC(b_+246, b_+247); ret_effect(gb);
}

// interactionCode19@updatePosition: marks the cube's tile solid in wRoomCollisions and records
// it in wRotatingCubePos. Reached by a `call` from state 0 and by fallthrough from
// @finishMovement; tails into objectPreventLinkFromPassing.
static void coloredCube_updatePosition(GB *gb, uint16_t sp0_) {
  BASE(interactionCode19);
  CALL_C(b_+185, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+188);
  CYC(b_+188, b_+190); H = wRoomCollisions >> 8;
  CYC(b_+190, b_+191); L = A;
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x0f);
  CYC(b_+193, b_+196); mem_wr(gb, wRotatingCubePos, A);
  CYC(b_+196, b_+199); objectPreventLinkFromPassing_hook(gb);
}

// interactionCode19@fallDownHole: the cracked floor under the cube gave way.
static void coloredCube_fallDownHole(GB *gb, uint16_t sp0_) {
  BASE(interactionCode19);
  CYC(b_+290, b_+291); C = L;
  CYC(b_+291, b_+293); A = TILEINDEX_HOLE;
  CALL_C(b_+293, setTile_hook, SYM(setTile), b_+296);
  CALL_C(b_+296, objectCreateFallingDownHoleInteraction_hook, SYM(objectCreateFallingDownHoleInteraction), b_+299);
  CYC(b_+299, b_+302); interactionDelete_hook(gb);
}

// interactionCode19@checkAnimParameter: advance the cube one tile on animation parameter 2-5,
// finish the roll on bit 7 (@finishMovement). Reached by jr from state 1 and by fallthrough
// from state 2.
static void coloredCube_checkAnimParameter(GB *gb, uint16_t sp0_) {
  BASE(interactionCode19);
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+131); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+131, b_+132); ret_effect(gb); return;
  }
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+133); B = A;
  CYC(b_+133, b_+134); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+134, b_+136); goto finishMovement;
  }
  CYC(b_+134, b_+136);
  CYC(b_+136, b_+137); alu_xor(gb, A);
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+139); A = B;
  CYC(b_+139, b_+141); alu_sub(gb, 0x02);
  CYC(b_+141, b_+144); SET_HL(coloredCubeDirectionOffsets_bank08);
  CYC(b_+144, b_+145); push_effect(gb, b_+145); coloredCube_addAToHl(gb);
  CYC(b_+145, b_+147); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+147, b_+148); A = mem_rd(gb, DE);
  CYC(b_+148, b_+149); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+149, b_+150); mem_wr(gb, DE, A);
  CYC(b_+150, b_+151); SET_HL(HL + 1);
  CYC(b_+151, b_+153); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+153, b_+154); A = mem_rd(gb, DE);
  CYC(b_+154, b_+155); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+155, b_+156); mem_wr(gb, DE, A);
  CYC(b_+156, b_+157); ret_effect(gb);
  return;

finishMovement:
  CYC(b_+157, b_+158); H = D;
  CYC(b_+158, b_+160); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+160, b_+161); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+161, b_+163); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x14);
  CYC(b_+165, b_+166); L = alu_inc8(gb, L);
  CYC(b_+166, b_+168); mem_wr(gb, HL, 0x0a);
  CYC(b_+168, b_+169); A = B;
  CYC(b_+169, b_+171); alu_and(gb, 0x7f);
  CYC(b_+171, b_+173); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+173, b_+174); mem_wr(gb, DE, A);
  CYC(b_+174, b_+177); push_effect(gb, b_+177); coloredCube_setColor(gb);
  CALL_C(b_+177, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+180);
  CYC(b_+180, b_+182); A = SND_MOVE_BLOCK_2;
  CALL_C(b_+182, playSound_b00_hook, SYM(playSound_b00), b_+185);
  coloredCube_updatePosition(gb, sp0_);
}

// @state0: initialization
static void coloredCube_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode19);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x14); // frames it takes to push it
  CYC(b_+21, b_+22); L = alu_inc8(gb, L);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x0a); // frames it takes to fall into a hole
  CYC(b_+24, b_+26); A = 0x06;
  CALL_C(b_+26, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+29);
  CALL_C(b_+29, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+32);
  CYC(b_+32, b_+34); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+41); push_effect(gb, b_+41); coloredCube_setColor(gb);
  // Palette 6 is used, so nothing else may be pushed/held while a cube is on screen
  // (it would interfere with objectMimicBgTile).
  CYC(b_+41, b_+43); A = PALH_89;
  CALL_C(b_+43, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+46);
  CYC(b_+46, b_+49); push_effect(gb, b_+49); coloredCube_updatePosition(gb, sp0_);
  CYC(b_+49, b_+52); objectSetVisible82_hook(gb);
}

// @state1: waiting to be pushed
static void coloredCube_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode19);
  CALL_C(b_+52, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+55);
  if (!(F & FZ)) {
    CYCT(b_+55, b_+57); goto checkPush;
  }
  CYC(b_+55, b_+57);
  CALL_C(b_+57, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+60);
  CYC(b_+60, b_+62); alu_cp(gb, TILEINDEX_CRACKED_FLOOR);
  if (F & FZ) {
    CYCT(b_+62, b_+65); coloredCube_fallDownHole(gb, sp0_); return;
  }
  CYC(b_+62, b_+65);
checkPush:
  CYC(b_+65, b_+68); push_effect(gb, b_+68); coloredCube_checkLinkPushingTowardBlock(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(b_+68, b_+70); goto resetCounter1;
  }
  CYC(b_+68, b_+70);
  CALL_C(b_+70, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+73);
  if (!(F & FZ)) {
    CYCT(b_+73, b_+74); ret_effect(gb); return;
  }
  CYC(b_+73, b_+74);
  // Block has been pushed for 20 frames
  CYC(b_+74, b_+75); A = B;
  CYC(b_+75, b_+77); A = alu_swap(gb, A);
  CYC(b_+77, b_+78); alu_rrca(gb);
  CYC(b_+78, b_+80); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  CALL_C(b_+81, interactionCheckAdjacentTileIsSolid_hook, SYM(interactionCheckAdjacentTileIsSolid), b_+84);
  if (!(F & FZ)) {
    CYCT(b_+84, b_+86); goto resetCounter1;
  }
  CYC(b_+84, b_+86);
  // Clear collisions at the current position
  CALL_C(b_+86, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+89);
  CYC(b_+89, b_+91); H = wRoomCollisions >> 8;
  CYC(b_+91, b_+92); L = A;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x00);
  CALL_C(b_+94, interactionIncState_hook, SYM(interactionIncState), b_+97);
  CYC(b_+97, b_+99); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+99, b_+100); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+100, b_+101); alu_add(gb, A);
  CYC(b_+101, b_+102); alu_add(gb, A);
  CYC(b_+102, b_+103); B = A;
  CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+106); A = alu_swap(gb, A);
  CYC(b_+106, b_+107); alu_rlca(gb);
  CYC(b_+107, b_+108); alu_add(gb, B);
  CYC(b_+108, b_+111); SET_HL(coloredCubeAnimations_bank08);
  CYC(b_+111, b_+112); push_effect(gb, b_+112); coloredCube_addAToHl(gb);
  CYC(b_+112, b_+113); A = mem_rd(gb, HL);
  CALL_C(b_+113, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+116);
  CYC(b_+116, b_+118);
  coloredCube_checkAnimParameter(gb, sp0_);
  return;

resetCounter1:
  CYC(b_+118, b_+120); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+120, b_+122); A = 0x14;
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  CYC(b_+123, b_+124); ret_effect(gb);
}

// @state2: being pushed. Falls through into @checkAnimParameter.
static void coloredCube_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode19);
  CALL_C(b_+124, interactionAnimate_hook, SYM(interactionAnimate), b_+127);
  coloredCube_checkAnimParameter(gb, sp0_);
}

// INTERAC_COLORED_CUBE: a cube that rolls one tile when pushed, changing its top color.
void interactionCode19_hook(GB *gb) {
  BASE(interactionCode19);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); push_effect(gb, b_+8);
  do { uint16_t jt_ = (coloredCube_jumpTable(gb));
    if (jt_ == b_+14) { coloredCube_state0(gb, sp0_); return; }
    else if (jt_ == b_+52) { coloredCube_state1(gb, sp0_); return; }
    else if (jt_ == b_+124) { coloredCube_state2(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
