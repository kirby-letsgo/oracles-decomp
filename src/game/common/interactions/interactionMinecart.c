#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode16);
  CALL_C(b_+59, objectSetPriorityRelativeToLink_hook, SYM(objectSetPriorityRelativeToLink), b_+62);
  CYC(b_+62, b_+65); A = mem_rd(gb, wLinkInAir);
  CYC(b_+65, b_+66); alu_add(gb, A);
  if (F & FC) {
    CYCT(b_+66, b_+68); goto linkInAir;
  }
  CYC(b_+66, b_+68);
  CALL_C(b_+68, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+71);
  if (!(F & FC)) {
    CYCT(b_+71, b_+72); ret_effect(gb); return;
  }
  CYC(b_+71, b_+72);
linkInAir:
  CYC(b_+72, b_+75); A = mem_rd(gb, w1Link_zh);
  CYC(b_+75, b_+76); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+76, b_+78); goto resetCounter;
  }
  CYC(b_+76, b_+78);
  CALL_C(b_+78, checkLinkID0AndControlNormal_hook, SYM(checkLinkID0AndControlNormal), b_+81);
  if (!(F & FC)) {
    CYCT(b_+81, b_+83); goto resetCounter;
  }
  CYC(b_+81, b_+83);
  CALL_C(b_+83, objectCheckLinkPushingAgainstCenter_hook, SYM(objectCheckLinkPushingAgainstCenter), b_+86);
  if (!(F & FC)) {
    CYCT(b_+86, b_+88); goto resetCounter;
  }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+90); A = 0x01;
  CYC(b_+90, b_+93); mem_wr(gb, wForceLinkPushAnimation, A);
  CALL_C(b_+93, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+96);
  if (!(F & FZ)) {
    CYCT(b_+96, b_+97); ret_effect(gb); return;
  }
  CYC(b_+96, b_+97);
  CALL_C(b_+97, interactionIncState_hook, SYM(interactionIncState), b_+100);
  CYC(b_+100, b_+102); A = 0x81;
  CYC(b_+102, b_+105); mem_wr(gb, wLinkInAir, A);
  CYC(b_+105, b_+108); SET_HL(w1Link_speed);
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x14);
  CYC(b_+110, b_+112); L = (uint8_t)w1Link_speedZ;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x40);
  CYC(b_+114, b_+115); L = alu_inc8(gb, L);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0xfe);
  CALL_C(b_+117, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+120);
  CYC(b_+120, b_+122); alu_xor(gb, 0x10);
  CYC(b_+122, b_+125); mem_wr(gb, w1Link_angle, A);
  CYC(b_+125, b_+126); ret_effect(gb);
  return;

resetCounter:
  CYC(b_+126, b_+128); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+128, b_+130); A = 0x04;
  CYC(b_+130, b_+131); mem_wr(gb, DE, A);
  CYC(b_+131, b_+132); ret_effect(gb);
}

// interactionCode16@state0: init graphics, face the platform, register as a static object.
// Falls through into state1.
static void interactionMinecart_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode16);
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+18);
  CYC(b_+18, b_+20); A = 0x06;
  CALL_C(b_+20, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+23);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x04);
  CYC(b_+27, b_+29); A = TILEINDEX_MINECART_PLATFORM;
  CALL_C(b_+29, objectGetRelativePositionOfTile_hook, SYM(objectGetRelativePositionOfTile), b_+32);
  CYC(b_+32, b_+33); H = D;
  CYC(b_+33, b_+35); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+35, b_+37); alu_xor(gb, 0x02);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+40); A = alu_swap(gb, A);
  CYC(b_+40, b_+41); alu_rrca(gb);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_and(gb, 0x01);
  CALL_C(b_+45, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+48);
  CALL_C(b_+48, objectDeleteRelatedObj1AsStaticObject_hook, SYM(objectDeleteRelatedObj1AsStaticObject), b_+51);
  CALL_C(b_+51, findFreeStaticObjectSlot_hook, SYM(findFreeStaticObjectSlot), b_+54);
  CYC(b_+54, b_+56); A = STATICOBJTYPE_INTERACTION;
  if (F & FZ) {
    CALL_C_CC(b_+56, objectSaveAsStaticObject_hook, SYM(objectSaveAsStaticObject), b_+59);
  } else {
    CYC(b_+56, b_+59);
  }
  interactionMinecart_state1(gb, sp0_);
}

// interactionCode16@state2: once Link is high enough and falling, spawn the rideable minecart
// special object in the companion slot and hand over direction/angle/position.
static void interactionMinecart_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode16);
  CYC(b_+132, b_+135); SET_HL(w1Link_zh);
  CYC(b_+135, b_+136); A = mem_rd(gb, HL);
  CYC(b_+136, b_+138); alu_cp(gb, 0xfa);
  if (F & FC) {
    CYCT(b_+138, b_+139); ret_effect(gb); return;
  }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+141); L = (uint8_t)(w1Link_speedZ + 1);
  CYC(b_+141, b_+143); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+143, b_+144); ret_effect(gb); return;
  }
  CYC(b_+143, b_+144);
  CYC(b_+144, b_+146); A = 0x03;
  CYC(b_+146, b_+147); mem_wr(gb, DE, A);
  CYC(b_+147, b_+150); SET_HL(w1Companion_enabled);
  CYC(b_+150, b_+151); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+151, b_+153); mem_wr(gb, HL, SPECIALOBJECT_MINECART);
  CYC(b_+153, b_+155); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+155, b_+157); L = (uint8_t)w1Companion_direction;
  CYC(b_+157, b_+158); A = mem_rd(gb, DE);
  CYC(b_+158, b_+159); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+159, b_+160); E = alu_inc8(gb, E);
  CYC(b_+160, b_+161); A = mem_rd(gb, DE);
  CYC(b_+161, b_+162); mem_wr(gb, HL, A);
  CALL_C(b_+162, objectCopyPosition_hook, SYM(objectCopyPosition), b_+165);
  CYC(b_+165, b_+168); objectDeleteRelatedObj1AsStaticObject_hook(gb);
}

void interactionCode16_hook(GB *gb) {
  BASE(interactionCode16);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionMinecart_jumpTable(gb));
    if (jt_ == b_+12) { interactionMinecart_state0(gb, sp0_); return; }
    else if (jt_ == b_+59) { interactionMinecart_state1(gb, sp0_); return; }
    else if (jt_ == b_+132) { interactionMinecart_state2(gb, sp0_); return; }
    else if (jt_ == SYM(interactionDelete) && hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}
