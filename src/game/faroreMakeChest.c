#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// objectData.objectData_faroreSparkle (bank $12 object data, referenced by address only).
#define objectData_faroreSparkle_bank12 0x4000
// interac11_subid01@initialAngles: 8 bytes indexed by the sparkle's subid high nibble.
#define initialAngles_bank08 0x427d

// The parent (subid 0) talks to the sparkles through two raw wTmpcfc0 bytes:
// $cfd8 = sparkle distance from the circle center, $cfd9 = "start flying away" signal.
#define faroreSparkleRadius (wTmpcfc0 + 0x18)
#define faroreSparkleFlyAway (wTmpcfc0 + 0x19)

void interac11_subid00_hook(GB *gb);
void interac11_subid01_hook(GB *gb);

static uint16_t faroreMakeChest_jumpTable(GB *gb) {
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

static void faroreMakeChest_addAToHl(GB *gb) {
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

void interactionCode11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x418c, 0x418e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x418e, 0x418f); A = mem_rd(gb, DE);
  CYC(0x418f, 0x4191); alu_and(gb, 0x0f);
  CYC(0x4191, 0x4192); push_effect(gb, 0x4192);
  switch (faroreMakeChest_jumpTable(gb)) {
    case 0x4196: interac11_subid00_hook(gb); return;
    case 0x424f: interac11_subid01_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void interac11_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4196, 0x4198); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4198, 0x4199); A = mem_rd(gb, DE);
  CYC(0x4199, 0x419a); push_effect(gb, 0x419a);
  switch (faroreMakeChest_jumpTable(gb)) {
    case 0x41b0: goto state0;
    case 0x41c7: goto state1;
    case 0x41d6: goto state2;
    case 0x41df: goto state3;
    case 0x41e8: goto state4;
    case 0x41fb: goto state5;
    case 0x421f: goto state678;
    case 0x4230: goto state9;
    case 0x423b: goto stateA;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x41b0, 0x41b2); A = 0x30;
  CYC(0x41b2, 0x41b5); mem_wr(gb, faroreSparkleRadius, A);
  CYC(0x41b5, 0x41b6); alu_xor(gb, A);
  CYC(0x41b6, 0x41b9); mem_wr(gb, faroreSparkleFlyAway, A);
  CALL_C(0x41b9, setCameraFocusedObject_hook, 0x12e5, 0x41bc);
  CYC(0x41bc, 0x41be); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x41be, 0x41c0); A = 0x5a;
  CYC(0x41c0, 0x41c1); mem_wr(gb, DE, A);
  CALL_C(0x41c1, darkenRoomLightly_hook, 0x32f8, 0x41c4);
  CYC(0x41c4, 0x41c7); interactionIncState_hook(gb);
  return;

state1:
  CALL_C(0x41c7, interactionDecCounter1_hook, 0x23cc, 0x41ca);
  if (!(F & FZ)) {
    CYCT(0x41ca, 0x41cb); ret_effect(gb); return;
  }
  CYC(0x41ca, 0x41cb);
  CYC(0x41cb, 0x41cd); mem_wr(gb, HL, 0x30);
  CYC(0x41cd, 0x41d0); SET_HL(objectData_faroreSparkle_bank12);
  CALL_C(0x41d0, parseGivenObjectData_b00_hook, 0x3171, 0x41d3);
  CYC(0x41d3, 0x41d6); interactionIncState_hook(gb);
  return;

state2:
  CALL_C(0x41d6, interactionDecCounter1_hook, 0x23cc, 0x41d9);
  if (!(F & FZ)) {
    CYCT(0x41d9, 0x41da); ret_effect(gb); return;
  }
  CYC(0x41d9, 0x41da);
  CYC(0x41da, 0x41dc); mem_wr(gb, HL, 0x1e);
  CYC(0x41dc, 0x41df); interactionIncState_hook(gb);
  return;

state3:
  CALL_C(0x41df, interactionDecCounter1_hook, 0x23cc, 0x41e2);
  if (!(F & FZ)) {
    CYCT(0x41e2, 0x41e3); ret_effect(gb); return;
  }
  CYC(0x41e2, 0x41e3);
  CYC(0x41e3, 0x41e5); mem_wr(gb, HL, 0x50);
  CYC(0x41e5, 0x41e8); interactionIncState_hook(gb);
  return;

state4:
  CYC(0x41e8, 0x41eb); A = mem_rd(gb, wFrameCounter);
  CYC(0x41eb, 0x41ec); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x41ec, 0x41ee);
  } else {
    CYC(0x41ec, 0x41ee);
    CYC(0x41ee, 0x41f1); SET_HL(faroreSparkleRadius);
    CYC(0x41f1, 0x41f2); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CALL_C(0x41f2, interactionDecCounter1_hook, 0x23cc, 0x41f5);
  if (!(F & FZ)) {
    CYCT(0x41f5, 0x41f6); ret_effect(gb); return;
  }
  CYC(0x41f5, 0x41f6);
  CYC(0x41f6, 0x41f8); mem_wr(gb, HL, 0x28);
  CYC(0x41f8, 0x41fb); interactionIncState_hook(gb);
  return;

state5:
  CALL_C(0x41fb, interactionDecCounter1_hook, 0x23cc, 0x41fe);
  if (!(F & FZ)) {
    CYCT(0x41fe, 0x41ff); ret_effect(gb); return;
  }
  CYC(0x41fe, 0x41ff);
  CYC(0x41ff, 0x4201); mem_wr(gb, HL, 0x08);
  CYC(0x4201, 0x4203); A = 0x01;
  CYC(0x4203, 0x4206); mem_wr(gb, faroreSparkleFlyAway, A);
  CYC(0x4206, 0x4209); SET_BC(0x840c);
  CALL_C(0x4209, objectCreateInteraction_hook, 0x24c5, 0x420c);
  CYC(0x420c, 0x420e); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x420e, 0x4210); mem_wr(gb, HL, INTERACTION_BASE);
  CYC(0x4210, 0x4211); L = alu_inc8(gb, L);
  CYC(0x4211, 0x4212); mem_wr(gb, HL, D);
  CALL_C(0x4212, objectCreatePuff_hook, 0x24c1, 0x4215);
  CYC(0x4215, 0x4217); A = 0xf1;
  CYC(0x4217, 0x4219); C = 0x75;
  CALL_C(0x4219, setTile_hook, 0x3a9c, 0x421c);
  CYC(0x421c, 0x421f); interactionIncState_hook(gb);
  return;

state678:
  CALL_C(0x421f, interactionDecCounter1_hook, 0x23cc, 0x4222);
  if (!(F & FZ)) {
    CYCT(0x4222, 0x4223); ret_effect(gb); return;
  }
  CYC(0x4222, 0x4223);
  CYC(0x4223, 0x4225); mem_wr(gb, HL, 0x10);
  CALL_C(0x4225, fadeinFromWhite_hook, 0x3299, 0x4228);

playFadeoutSound:
  CYC(0x4228, 0x422a); A = 0xb4;
  CALL_C(0x422a, playSound_b00_hook, 0x0c98, 0x422d);
  CYC(0x422d, 0x4230); interactionIncState_hook(gb);
  return;

state9:
  CALL_C(0x4230, interactionDecCounter1_hook, 0x23cc, 0x4233);
  if (!(F & FZ)) {
    CYCT(0x4233, 0x4234); ret_effect(gb); return;
  }
  CYC(0x4233, 0x4234);
  CYC(0x4234, 0x4236); A = 0x04;
  CALL_C(0x4236, fadeinFromWhiteWithDelay_hook, 0x3284, 0x4239);
  CYC(0x4239, 0x423b); goto playFadeoutSound;

stateA:
  CYC(0x423b, 0x423e); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x423e, 0x423f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x423f, 0x4240); ret_effect(gb); return;
  }
  CYC(0x423f, 0x4240);
  CYC(0x4240, 0x4242); A = 0x01;
  CYC(0x4242, 0x4245); mem_wr(gb, wTmpcfc0, A);
  CYC(0x4245, 0x4246); alu_xor(gb, A);
  CYC(0x4246, 0x4249); mem_wr(gb, wPaletteThread_parameter, A);
  CALL_C(0x4249, setCameraFocusedObjectToLink_hook, 0x12f0, 0x424c);
  CYC(0x424c, 0x424f); interactionDelete_hook(gb);
}

// interac11_subid01@interac11_updateSparkle: rotate the sparkle around the circle center
// at the radius given by the parent. Reached only by a static `call` from subid 1 state 2;
// not independently hookable.
static void faroreMakeChest_updateSparkle(GB *gb, uint16_t sp0_) {
  CYC(0x42af, 0x42b2); A = mem_rd(gb, wFrameCounter);
  CYC(0x42b2, 0x42b3); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x42b3, 0x42b5);
  } else {
    CYC(0x42b3, 0x42b5);
    CYC(0x42b5, 0x42b6); H = D;
    CYC(0x42b6, 0x42b8); L = INTERACTION_BASE + OBJ_ANGLE;
    CYC(0x42b8, 0x42b9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x42b9, 0x42ba); A = mem_rd(gb, HL);
    CYC(0x42ba, 0x42bc); alu_and(gb, 0x1f);
    CYC(0x42bc, 0x42bd); mem_wr(gb, HL, A);
    CYC(0x42bd, 0x42bf); A = 0xc9;
    if (F & FZ) {
      CALL_C_CC(0x42bf, playSound_b00_hook, 0x0c98, 0x42c2);
    } else {
      CYC(0x42bf, 0x42c2);
    }
  }
  CYC(0x42c2, 0x42c4); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x42c4, 0x42c7); SET_BC(0x7858);
  CYC(0x42c7, 0x42ca); A = mem_rd(gb, faroreSparkleRadius);
  CALL_C(0x42ca, objectSetPositionInCircleArc_hook, 0x210e, 0x42cd);
  CYC(0x42cd, 0x42d0); interactionAnimate_hook(gb);
}

void interac11_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x424f, 0x4251); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4251, 0x4252); A = mem_rd(gb, DE);
  CYC(0x4252, 0x4253); push_effect(gb, 0x4253);
  switch (faroreMakeChest_jumpTable(gb)) {
    case 0x425b: goto state0;
    case 0x4285: goto state1;
    case 0x4292: goto state2;
    case 0x42a2: goto state3;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x425b, 0x425d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x425d, 0x425e); A = mem_rd(gb, DE);
  CYC(0x425e, 0x4260); A = alu_swap(gb, A);
  CYC(0x4260, 0x4262); alu_and(gb, 0x0f);
  CYC(0x4262, 0x4265); SET_HL(initialAngles_bank08);
  CYC(0x4265, 0x4266); push_effect(gb, 0x4266); faroreMakeChest_addAToHl(gb);
  CYC(0x4266, 0x4267); A = mem_rd(gb, HL);
  CYC(0x4267, 0x4269); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x4269, 0x426a); mem_wr(gb, DE, A);
  CYC(0x426a, 0x426c); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x426c, 0x426e); A = 0x28;
  CYC(0x426e, 0x426f); mem_wr(gb, DE, A);
  CYC(0x426f, 0x4271); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4271, 0x4273); A = 0x30;
  CYC(0x4273, 0x4274); mem_wr(gb, DE, A);
  CALL_C(0x4274, interactionInitGraphics_hook, 0x15fb, 0x4277);
  CALL_C(0x4277, objectSetVisible80_hook, 0x1e57, 0x427a);
  CYC(0x427a, 0x427d); interactionIncState_hook(gb);
  return;

state1:
  CALL_C(0x4285, objectApplySpeed_hook, 0x201d, 0x4288);
  CALL_C(0x4288, interactionAnimate_hook, 0x261b, 0x428b);
  CALL_C(0x428b, interactionDecCounter1_hook, 0x23cc, 0x428e);
  if (!(F & FZ)) {
    CYCT(0x428e, 0x428f); ret_effect(gb); return;
  }
  CYC(0x428e, 0x428f);
  CYC(0x428f, 0x4292); interactionIncState_hook(gb);
  return;

state2:
  CYC(0x4292, 0x4295); push_effect(gb, 0x4295); faroreMakeChest_updateSparkle(gb, sp0_);
  CYC(0x4295, 0x4298); A = mem_rd(gb, faroreSparkleFlyAway);
  CYC(0x4298, 0x4299); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4299, 0x429a); ret_effect(gb); return;
  }
  CYC(0x4299, 0x429a);
  CYC(0x429a, 0x429c); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x429c, 0x429e); A = 0x50;
  CYC(0x429e, 0x429f); mem_wr(gb, DE, A);
  CYC(0x429f, 0x42a2); interactionIncState_hook(gb);
  return;

state3:
  CALL_C(0x42a2, objectApplySpeed_hook, 0x201d, 0x42a5);
  CALL_C(0x42a5, interactionAnimate_hook, 0x261b, 0x42a8);
  CALL_C(0x42a8, objectCheckWithinScreenBoundary_hook, 0x2184, 0x42ab);
  if (F & FC) {
    CYCT(0x42ab, 0x42ac); ret_effect(gb); return;
  }
  CYC(0x42ab, 0x42ac);
  CYC(0x42ac, 0x42af); interactionDelete_hook(gb);
}
