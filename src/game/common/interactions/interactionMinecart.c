#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define TILEINDEX_MINECART_PLATFORM 0x5f
#define STATICOBJTYPE_INTERACTION 0x03
#define SPECIALOBJECT_MINECART 0x0a

static uint16_t interactionMinecart_jumpTable(GB *gb) {
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

// interactionCode16@state1 (also reached by fallthrough from state0): wait for Link to push
// against the cart for a few frames, then launch him into it.
static void interactionMinecart_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x45bc, objectSetPriorityRelativeToLink_hook, 0x22dc, 0x45bf);
  CYC(0x45bf, 0x45c2); A = mem_rd(gb, wLinkInAir);
  CYC(0x45c2, 0x45c3); alu_add(gb, A);
  if (F & FC) {
    CYCT(0x45c3, 0x45c5); goto linkInAir;
  }
  CYC(0x45c3, 0x45c5);
  CALL_C(0x45c5, objectPreventLinkFromPassing_hook, 0x2680, 0x45c8);
  if (!(F & FC)) {
    CYCT(0x45c8, 0x45c9); ret_effect(gb); return;
  }
  CYC(0x45c8, 0x45c9);
linkInAir:
  CYC(0x45c9, 0x45cc); A = mem_rd(gb, w1Link_zh);
  CYC(0x45cc, 0x45cd); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x45cd, 0x45cf); goto resetCounter;
  }
  CYC(0x45cd, 0x45cf);
  CALL_C(0x45cf, checkLinkID0AndControlNormal_hook, 0x1d18, 0x45d2);
  if (!(F & FC)) {
    CYCT(0x45d2, 0x45d4); goto resetCounter;
  }
  CYC(0x45d2, 0x45d4);
  CALL_C(0x45d4, objectCheckLinkPushingAgainstCenter_hook, 0x2707, 0x45d7);
  if (!(F & FC)) {
    CYCT(0x45d7, 0x45d9); goto resetCounter;
  }
  CYC(0x45d7, 0x45d9);
  CYC(0x45d9, 0x45db); A = 0x01;
  CYC(0x45db, 0x45de); mem_wr(gb, wForceLinkPushAnimation, A);
  CALL_C(0x45de, interactionDecCounter1_hook, 0x23cc, 0x45e1);
  if (!(F & FZ)) {
    CYCT(0x45e1, 0x45e2); ret_effect(gb); return;
  }
  CYC(0x45e1, 0x45e2);
  CALL_C(0x45e2, interactionIncState_hook, 0x23e0, 0x45e5);
  CYC(0x45e5, 0x45e7); A = 0x81;
  CYC(0x45e7, 0x45ea); mem_wr(gb, wLinkInAir, A);
  CYC(0x45ea, 0x45ed); SET_HL(w1Link_speed);
  CYC(0x45ed, 0x45ef); mem_wr(gb, HL, 0x14);
  CYC(0x45ef, 0x45f1); L = (uint8_t)w1Link_speedZ;
  CYC(0x45f1, 0x45f3); mem_wr(gb, HL, 0x40);
  CYC(0x45f3, 0x45f4); L = alu_inc8(gb, L);
  CYC(0x45f4, 0x45f6); mem_wr(gb, HL, 0xfe);
  CALL_C(0x45f6, objectGetAngleTowardLink_hook, 0x1e9c, 0x45f9);
  CYC(0x45f9, 0x45fb); alu_xor(gb, 0x10);
  CYC(0x45fb, 0x45fe); mem_wr(gb, w1Link_angle, A);
  CYC(0x45fe, 0x45ff); ret_effect(gb);
  return;

resetCounter:
  CYC(0x45ff, 0x4601); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4601, 0x4603); A = 0x04;
  CYC(0x4603, 0x4604); mem_wr(gb, DE, A);
  CYC(0x4604, 0x4605); ret_effect(gb);
}

// interactionCode16@state0: init graphics, face the platform, register as a static object.
// Falls through into state1.
static void interactionMinecart_state0(GB *gb, uint16_t sp0_) {
  CYC(0x458d, 0x458f); A = 0x01;
  CYC(0x458f, 0x4590); mem_wr(gb, DE, A);
  CALL_C(0x4590, interactionInitGraphics_hook, 0x15fb, 0x4593);
  CYC(0x4593, 0x4595); A = 0x06;
  CALL_C(0x4595, objectSetCollideRadius_hook, 0x24a1, 0x4598);
  CYC(0x4598, 0x459a); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x459a, 0x459c); mem_wr(gb, HL, 0x04);
  CYC(0x459c, 0x459e); A = TILEINDEX_MINECART_PLATFORM;
  CALL_C(0x459e, objectGetRelativePositionOfTile_hook, 0x1456, 0x45a1);
  CYC(0x45a1, 0x45a2); H = D;
  CYC(0x45a2, 0x45a4); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x45a4, 0x45a6); alu_xor(gb, 0x02);
  CYC(0x45a6, 0x45a7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x45a7, 0x45a9); A = alu_swap(gb, A);
  CYC(0x45a9, 0x45aa); alu_rrca(gb);
  CYC(0x45aa, 0x45ab); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x45ab, 0x45ac); A = mem_rd(gb, HL);
  CYC(0x45ac, 0x45ae); alu_and(gb, 0x01);
  CALL_C(0x45ae, interactionSetAnimation_hook, 0x262e, 0x45b1);
  CALL_C(0x45b1, objectDeleteRelatedObj1AsStaticObject_hook, 0x31b8, 0x45b4);
  CALL_C(0x45b4, findFreeStaticObjectSlot_hook, 0x31a7, 0x45b7);
  CYC(0x45b7, 0x45b9); A = STATICOBJTYPE_INTERACTION;
  if (F & FZ) {
    CALL_C_CC(0x45b9, objectSaveAsStaticObject_hook, 0x31cf, 0x45bc);
  } else {
    CYC(0x45b9, 0x45bc);
  }
  interactionMinecart_state1(gb, sp0_);
}

// interactionCode16@state2: once Link is high enough and falling, spawn the rideable minecart
// special object in the companion slot and hand over direction/angle/position.
static void interactionMinecart_state2(GB *gb, uint16_t sp0_) {
  CYC(0x4605, 0x4608); SET_HL(w1Link_zh);
  CYC(0x4608, 0x4609); A = mem_rd(gb, HL);
  CYC(0x4609, 0x460b); alu_cp(gb, 0xfa);
  if (F & FC) {
    CYCT(0x460b, 0x460c); ret_effect(gb); return;
  }
  CYC(0x460b, 0x460c);
  CYC(0x460c, 0x460e); L = (uint8_t)(w1Link_speedZ + 1);
  CYC(0x460e, 0x4610); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4610, 0x4611); ret_effect(gb); return;
  }
  CYC(0x4610, 0x4611);
  CYC(0x4611, 0x4613); A = 0x03;
  CYC(0x4613, 0x4614); mem_wr(gb, DE, A);
  CYC(0x4614, 0x4617); SET_HL(w1Companion_enabled);
  CYC(0x4617, 0x4618); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4618, 0x461a); mem_wr(gb, HL, SPECIALOBJECT_MINECART);
  CYC(0x461a, 0x461c); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x461c, 0x461e); L = (uint8_t)w1Companion_direction;
  CYC(0x461e, 0x461f); A = mem_rd(gb, DE);
  CYC(0x461f, 0x4620); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4620, 0x4621); E = alu_inc8(gb, E);
  CYC(0x4621, 0x4622); A = mem_rd(gb, DE);
  CYC(0x4622, 0x4623); mem_wr(gb, HL, A);
  CALL_C(0x4623, objectCopyPosition_hook, 0x2242, 0x4626);
  CYC(0x4626, 0x4629); objectDeleteRelatedObj1AsStaticObject_hook(gb);
}

void interactionCode16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4581, 0x4583); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4583, 0x4584); A = mem_rd(gb, DE);
  CYC(0x4584, 0x4585); push_effect(gb, 0x4585);
  switch (interactionMinecart_jumpTable(gb)) {
    case 0x458d: interactionMinecart_state0(gb, sp0_); return;
    case 0x45bc: interactionMinecart_state1(gb, sp0_); return;
    case 0x4605: interactionMinecart_state2(gb, sp0_); return;
    case 0x3b05: interactionDelete_hook(gb); return;
    default: HANDOFF(HL);
  }
}
