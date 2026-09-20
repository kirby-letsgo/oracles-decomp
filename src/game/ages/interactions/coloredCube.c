#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Per-direction y/x offsets applied when the cube rolls one tile.
#define coloredCubeDirectionOffsets_bank08 0x49ce
// Rows: cube orientation (direction); columns: push angle.
#define coloredCubeAnimations_bank08 0x49fe
// Cube orientation -> wRotatingCubeColor.
#define coloredCubeColors_bank08 0x4a23

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
  CYC(0x4a16, 0x4a17); B = A;
  CYC(0x4a17, 0x4a1a); SET_HL(coloredCubeColors_bank08);
  CYC(0x4a1a, 0x4a1b); push_effect(gb, 0x4a1b); coloredCube_addAToHl(gb);
  CYC(0x4a1b, 0x4a1c); A = mem_rd(gb, HL);
  CYC(0x4a1c, 0x4a1f); mem_wr(gb, wRotatingCubeColor, A);
  CYC(0x4a1f, 0x4a20); A = B;
  CYC(0x4a20, 0x4a23); interactionSetAnimation_hook(gb);
}

// interactionCode19@checkLinkPushingTowardBlock: b = push direction, zflag set if Link is
// pushing toward the block.
static void coloredCube_checkLinkPushingTowardBlock(GB *gb, uint16_t sp0_) {
  CYC(0x49d6, 0x49d9); A = mem_rd(gb, wLinkGrabState);
  CYC(0x49d9, 0x49da); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x49da, 0x49db); ret_effect(gb); return;
  }
  CYC(0x49da, 0x49db);
  CYC(0x49db, 0x49de); A = mem_rd(gb, wLinkAngle);
  CYC(0x49de, 0x49df); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x49df, 0x49e0); ret_effect(gb); return;
  }
  CYC(0x49df, 0x49e0);
  CYC(0x49e0, 0x49e3); A = W8(w1Link_zh);
  CYC(0x49e3, 0x49e4); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x49e4, 0x49e5); ret_effect(gb); return;
  }
  CYC(0x49e4, 0x49e5);
  CYC(0x49e5, 0x49e8); A = mem_rd(gb, wGameKeysPressed);
  CYC(0x49e8, 0x49ea); alu_and(gb, 0x03); // BTN_A | BTN_B
  if (!(F & FZ)) {
    CYCT(0x49ea, 0x49eb); ret_effect(gb); return;
  }
  CYC(0x49ea, 0x49eb);
  CYC(0x49eb, 0x49ed); C = 0x14;
  CALL_C(0x49ed, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x49f0);
  if (!(F & FC)) {
    CYCT(0x49f0, 0x49f2); goto notWithinDistance;
  }
  CYC(0x49f0, 0x49f2);
  CYC(0x49f2, 0x49f4); A = alu_srl(gb, A);
  CYC(0x49f4, 0x49f6); alu_xor(gb, 0x02);
  CYC(0x49f6, 0x49f7); B = A;
  CYC(0x49f7, 0x49fa); A = W8(w1Link_direction);
  CYC(0x49fa, 0x49fb); alu_cp(gb, B);
  CYC(0x49fb, 0x49fc); ret_effect(gb);
  return;
notWithinDistance:
  CYC(0x49fc, 0x49fd); alu_or(gb, D);
  CYC(0x49fd, 0x49fe); ret_effect(gb);
}

// interactionCode19@updatePosition: marks the cube's tile solid in wRoomCollisions and records
// it in wRotatingCubePos. Reached by a `call` from state 0 and by fallthrough from
// @finishMovement; tails into objectPreventLinkFromPassing.
static void coloredCube_updatePosition(GB *gb, uint16_t sp0_) {
  CALL_C(0x49c0, objectGetShortPosition_hook, 0x2096, 0x49c3);
  CYC(0x49c3, 0x49c5); H = wRoomCollisions >> 8;
  CYC(0x49c5, 0x49c6); L = A;
  CYC(0x49c6, 0x49c8); mem_wr(gb, HL, 0x0f);
  CYC(0x49c8, 0x49cb); mem_wr(gb, wRotatingCubePos, A);
  CYC(0x49cb, 0x49ce); objectPreventLinkFromPassing_hook(gb);
}

// interactionCode19@fallDownHole: the cracked floor under the cube gave way.
static void coloredCube_fallDownHole(GB *gb, uint16_t sp0_) {
  CYC(0x4a29, 0x4a2a); C = L;
  CYC(0x4a2a, 0x4a2c); A = TILEINDEX_HOLE;
  CALL_C(0x4a2c, setTile_hook, 0x3a9c, 0x4a2f);
  CALL_C(0x4a2f, objectCreateFallingDownHoleInteraction_hook, 0x24d1, 0x4a32);
  CYC(0x4a32, 0x4a35); interactionDelete_hook(gb);
}

// interactionCode19@checkAnimParameter: advance the cube one tile on animation parameter 2-5,
// finish the roll on bit 7 (@finishMovement). Reached by jr from state 1 and by fallthrough
// from state 2.
static void coloredCube_checkAnimParameter(GB *gb, uint16_t sp0_) {
  CYC(0x4986, 0x4988); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4988, 0x4989); A = mem_rd(gb, DE);
  CYC(0x4989, 0x498a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x498a, 0x498b); ret_effect(gb); return;
  }
  CYC(0x498a, 0x498b);
  CYC(0x498b, 0x498c); B = A;
  CYC(0x498c, 0x498d); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x498d, 0x498f); goto finishMovement;
  }
  CYC(0x498d, 0x498f);
  CYC(0x498f, 0x4990); alu_xor(gb, A);
  CYC(0x4990, 0x4991); mem_wr(gb, DE, A);
  CYC(0x4991, 0x4992); A = B;
  CYC(0x4992, 0x4994); alu_sub(gb, 0x02);
  CYC(0x4994, 0x4997); SET_HL(coloredCubeDirectionOffsets_bank08);
  CYC(0x4997, 0x4998); push_effect(gb, 0x4998); coloredCube_addAToHl(gb);
  CYC(0x4998, 0x499a); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x499a, 0x499b); A = mem_rd(gb, DE);
  CYC(0x499b, 0x499c); alu_add(gb, mem_rd(gb, HL));
  CYC(0x499c, 0x499d); mem_wr(gb, DE, A);
  CYC(0x499d, 0x499e); SET_HL(HL + 1);
  CYC(0x499e, 0x49a0); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x49a0, 0x49a1); A = mem_rd(gb, DE);
  CYC(0x49a1, 0x49a2); alu_add(gb, mem_rd(gb, HL));
  CYC(0x49a2, 0x49a3); mem_wr(gb, DE, A);
  CYC(0x49a3, 0x49a4); ret_effect(gb);
  return;

finishMovement:
  CYC(0x49a4, 0x49a5); H = D;
  CYC(0x49a5, 0x49a7); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x49a7, 0x49a8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x49a8, 0x49aa); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x49aa, 0x49ac); mem_wr(gb, HL, 0x14);
  CYC(0x49ac, 0x49ad); L = alu_inc8(gb, L);
  CYC(0x49ad, 0x49af); mem_wr(gb, HL, 0x0a);
  CYC(0x49af, 0x49b0); A = B;
  CYC(0x49b0, 0x49b2); alu_and(gb, 0x7f);
  CYC(0x49b2, 0x49b4); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x49b4, 0x49b5); mem_wr(gb, DE, A);
  CYC(0x49b5, 0x49b8); push_effect(gb, 0x49b8); coloredCube_setColor(gb);
  CALL_C(0x49b8, objectCenterOnTile_hook, 0x20db, 0x49bb);
  CYC(0x49bb, 0x49bd); A = SND_MOVE_BLOCK_2;
  CALL_C(0x49bd, playSound_b00_hook, 0x0c98, 0x49c0);
  coloredCube_updatePosition(gb, sp0_);
}

// @state0: initialization
static void coloredCube_state0(GB *gb, uint16_t sp0_) {
  CYC(0x4915, 0x4916); H = D;
  CYC(0x4916, 0x4917); L = E;
  CYC(0x4917, 0x4918); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4918, 0x491a); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x491a, 0x491c); mem_wr(gb, HL, 0x14); // frames it takes to push it
  CYC(0x491c, 0x491d); L = alu_inc8(gb, L);
  CYC(0x491d, 0x491f); mem_wr(gb, HL, 0x0a); // frames it takes to fall into a hole
  CYC(0x491f, 0x4921); A = 0x06;
  CALL_C(0x4921, objectSetCollideRadius_hook, 0x24a1, 0x4924);
  CALL_C(0x4924, interactionInitGraphics_hook, 0x15fb, 0x4927);
  CYC(0x4927, 0x4929); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4929, 0x492a); A = mem_rd(gb, DE);
  CYC(0x492a, 0x492c); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x492c, 0x492d); mem_wr(gb, DE, A);
  CYC(0x492d, 0x4930); push_effect(gb, 0x4930); coloredCube_setColor(gb);
  // Palette 6 is used, so nothing else may be pushed/held while a cube is on screen
  // (it would interfere with objectMimicBgTile).
  CYC(0x4930, 0x4932); A = PALH_89;
  CALL_C(0x4932, loadPaletteHeader_hook, 0x050b, 0x4935);
  CYC(0x4935, 0x4938); push_effect(gb, 0x4938); coloredCube_updatePosition(gb, sp0_);
  CYC(0x4938, 0x493b); objectSetVisible82_hook(gb);
}

// @state1: waiting to be pushed
static void coloredCube_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x493b, interactionDecCounter2_hook, 0x23d1, 0x493e);
  if (!(F & FZ)) {
    CYCT(0x493e, 0x4940); goto checkPush;
  }
  CYC(0x493e, 0x4940);
  CALL_C(0x4940, objectGetTileAtPosition_hook, 0x1444, 0x4943);
  CYC(0x4943, 0x4945); alu_cp(gb, TILEINDEX_CRACKED_FLOOR);
  if (F & FZ) {
    CYCT(0x4945, 0x4948); coloredCube_fallDownHole(gb, sp0_); return;
  }
  CYC(0x4945, 0x4948);
checkPush:
  CYC(0x4948, 0x494b); push_effect(gb, 0x494b); coloredCube_checkLinkPushingTowardBlock(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(0x494b, 0x494d); goto resetCounter1;
  }
  CYC(0x494b, 0x494d);
  CALL_C(0x494d, interactionDecCounter1_hook, 0x23cc, 0x4950);
  if (!(F & FZ)) {
    CYCT(0x4950, 0x4951); ret_effect(gb); return;
  }
  CYC(0x4950, 0x4951);
  // Block has been pushed for 20 frames
  CYC(0x4951, 0x4952); A = B;
  CYC(0x4952, 0x4954); A = alu_swap(gb, A);
  CYC(0x4954, 0x4955); alu_rrca(gb);
  CYC(0x4955, 0x4957); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x4957, 0x4958); mem_wr(gb, DE, A);
  CALL_C(0x4958, interactionCheckAdjacentTileIsSolid_hook, 0x271d, 0x495b);
  if (!(F & FZ)) {
    CYCT(0x495b, 0x495d); goto resetCounter1;
  }
  CYC(0x495b, 0x495d);
  // Clear collisions at the current position
  CALL_C(0x495d, objectGetShortPosition_hook, 0x2096, 0x4960);
  CYC(0x4960, 0x4962); H = wRoomCollisions >> 8;
  CYC(0x4962, 0x4963); L = A;
  CYC(0x4963, 0x4965); mem_wr(gb, HL, 0x00);
  CALL_C(0x4965, interactionIncState_hook, 0x23e0, 0x4968);
  CYC(0x4968, 0x496a); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x496a, 0x496b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x496b, 0x496c); alu_add(gb, A);
  CYC(0x496c, 0x496d); alu_add(gb, A);
  CYC(0x496d, 0x496e); B = A;
  CYC(0x496e, 0x496f); A = mem_rd(gb, HL);
  CYC(0x496f, 0x4971); A = alu_swap(gb, A);
  CYC(0x4971, 0x4972); alu_rlca(gb);
  CYC(0x4972, 0x4973); alu_add(gb, B);
  CYC(0x4973, 0x4976); SET_HL(coloredCubeAnimations_bank08);
  CYC(0x4976, 0x4977); push_effect(gb, 0x4977); coloredCube_addAToHl(gb);
  CYC(0x4977, 0x4978); A = mem_rd(gb, HL);
  CALL_C(0x4978, interactionSetAnimation_hook, 0x262e, 0x497b);
  CYC(0x497b, 0x497d);
  coloredCube_checkAnimParameter(gb, sp0_);
  return;

resetCounter1:
  CYC(0x497d, 0x497f); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x497f, 0x4981); A = 0x14;
  CYC(0x4981, 0x4982); mem_wr(gb, DE, A);
  CYC(0x4982, 0x4983); ret_effect(gb);
}

// @state2: being pushed. Falls through into @checkAnimParameter.
static void coloredCube_state2(GB *gb, uint16_t sp0_) {
  CALL_C(0x4983, interactionAnimate_hook, 0x261b, 0x4986);
  coloredCube_checkAnimParameter(gb, sp0_);
}

// INTERAC_COLORED_CUBE: a cube that rolls one tile when pushed, changing its top color.
void interactionCode19_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4907, objectReplaceWithAnimationIfOnHazard_hook, 0x2225, 0x490a);
  if (F & FC) {
    CYCT(0x490a, 0x490b); ret_effect(gb); return;
  }
  CYC(0x490a, 0x490b);
  CYC(0x490b, 0x490d); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x490d, 0x490e); A = mem_rd(gb, DE);
  CYC(0x490e, 0x490f); push_effect(gb, 0x490f);
  switch (coloredCube_jumpTable(gb)) {
    case 0x4915: coloredCube_state0(gb, sp0_); return;
    case 0x493b: coloredCube_state1(gb, sp0_); return;
    case 0x4983: coloredCube_state2(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
