#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// interactionCode0f@specialHoleRooms: lookupKey table (room, group pairs, $00 terminated).
#define specialHoleRooms_bank08 0x4146

static uint16_t fallDownHole_jumpTable(GB *gb) {
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

// interactionCode0f@checkUpdateHoleEvent: in a few special rooms, record the id of the
// object that fell in the hole into the wTmpcfc0.fallDownHoleEvent buffer. Reached only by
// a static `call` from interactionCode0f state 0; not independently hookable.
static void fallDownHole_checkUpdateHoleEvent(GB *gb, uint16_t sp0_) {
  CYC(0x411e, 0x4121); A = mem_rd(gb, wActiveRoom);
  CYC(0x4121, 0x4122); E = A;
  CYC(0x4122, 0x4125); SET_HL(specialHoleRooms_bank08);
  CALL_C(0x4125, lookupKey_hook, 0x1e06, 0x4128);
  if (!(F & FC)) {
    CYCT(0x4128, 0x4129); ret_effect(gb); return;
  }
  CYC(0x4128, 0x4129);
  CYC(0x4129, 0x412a); B = A;
  CYC(0x412a, 0x412d); A = mem_rd(gb, wActiveGroup);
  CYC(0x412d, 0x412e); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x412e, 0x412f); ret_effect(gb); return;
  }
  CYC(0x412e, 0x412f);
  CYC(0x412f, 0x4132); SET_HL(wTmpcfc0_fallDownHoleEvent_cfd8);
  CYC(0x4132, 0x4134); B = 0x04;
  for (;;) {
    CYC(0x4134, 0x4135); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4135, 0x4137); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYC(0x4137, 0x4139);
      CYC(0x4139, 0x413b); E = INTERACTION_BASE + OBJ_COUNTER2;
      CYC(0x413b, 0x413c); A = mem_rd(gb, DE);
      CYC(0x413c, 0x413d); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(0x413d, 0x413e); E = alu_dec8(gb, E);
      CYC(0x413e, 0x413f); A = mem_rd(gb, DE);
      CYC(0x413f, 0x4140); mem_wr(gb, HL, A);
      CYC(0x4140, 0x4141); ret_effect(gb);
      return;
    }
    CYCT(0x4137, 0x4139);
    CYC(0x4141, 0x4142); L = alu_inc8(gb, L);
    CYC(0x4142, 0x4143); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(0x4143, 0x4145);
      break;
    }
    CYCT(0x4143, 0x4145);
  }
  CYC(0x4145, 0x4146); ret_effect(gb);
}

void interactionCode0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40ab, 0x40ad); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x40ad, 0x40ae); A = mem_rd(gb, DE);
  CYC(0x40ae, 0x40af); push_effect(gb, 0x40af);
  switch (fallDownHole_jumpTable(gb)) {
    case 0x40b5: goto state0;
    case 0x40de: goto state1;
    case 0x410e: goto state2;
    default: HANDOFF(HL);
  }

state0:
  CALL_C(0x40b5, interactionInitGraphics_hook, 0x15fb, 0x40b8);
  CALL_C(0x40b8, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x40bb);
  CALL_C(0x40bb, interactionIncState_hook, 0x23e0, 0x40be);
  CYC(0x40be, 0x40c0); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x40c0, 0x40c1); A = mem_rd(gb, DE);
  CYC(0x40c1, 0x40c2); alu_add(gb, mem_rd(gb, HL));
  CYC(0x40c2, 0x40c3); mem_wr(gb, HL, A);
  CYC(0x40c3, 0x40c5); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x40c5, 0x40c7); mem_wr(gb, HL, 0x0f);
  CYC(0x40c7, 0x40c8); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x40c8, 0x40ca); goto fallDownHole;
  }
  CYC(0x40c8, 0x40ca);
  // @dust
  CALL_C(0x40ca, interactionSetAnimation_hook, 0x262e, 0x40cd);
  CYC(0x40cd, 0x40d0); objectSetVisible80_hook(gb);
  return;

fallDownHole:
  CYC(0x40d0, 0x40d1); E = alu_inc8(gb, E);
  CYC(0x40d1, 0x40d2); A = mem_rd(gb, DE);
  CYC(0x40d2, 0x40d3); alu_rlca(gb);
  CYC(0x40d3, 0x40d5); A = 0x59;
  if (!(F & FC)) {
    CALL_C_CC(0x40d5, playSound_b00_hook, 0x0c98, 0x40d8);
  } else {
    CYC(0x40d5, 0x40d8);
  }
  CYC(0x40d8, 0x40db); push_effect(gb, 0x40db); fallDownHole_checkUpdateHoleEvent(gb, sp0_);
  CYC(0x40db, 0x40de); objectSetVisible83_hook(gb);
  return;

state1:
  CYC(0x40de, 0x40df); H = D;
  CYC(0x40df, 0x40e1); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x40e1, 0x40e3); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x40e3, 0x40e5); goto delete;
  }
  CYC(0x40e3, 0x40e5);
  CYC(0x40e5, 0x40e7); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x40e7, 0x40e8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x40e8, 0x40ea); H8(hFF8F) = A;
  CYC(0x40ea, 0x40ec); alu_add(gb, 0x05);
  CYC(0x40ec, 0x40ee); alu_and(gb, 0xf0);
  CYC(0x40ee, 0x40f0); alu_add(gb, 0x08);
  CYC(0x40f0, 0x40f1); B = A;
  CYC(0x40f1, 0x40f2); L = alu_inc8(gb, L);
  CYC(0x40f2, 0x40f3); A = mem_rd(gb, HL);
  CYC(0x40f3, 0x40f5); H8(hFF8E) = A;
  CYC(0x40f5, 0x40f7); alu_and(gb, 0xf0);
  CYC(0x40f7, 0x40f9); alu_add(gb, 0x08);
  CYC(0x40f9, 0x40fa); C = A;
  CYC(0x40fa, 0x40fb); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x40fb, 0x40fd); goto moveTowardsCenter;
  }
  CYC(0x40fb, 0x40fd);
  CYC(0x40fd, 0x40ff); A = H8(hFF8F);
  CYC(0x40ff, 0x4100); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x4100, 0x4102); goto animate;
  }
  CYC(0x4100, 0x4102);

moveTowardsCenter:
  CALL_C(0x4102, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x4105);
  CYC(0x4105, 0x4107); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x4107, 0x4108); mem_wr(gb, DE, A);
  CALL_C(0x4108, objectApplySpeed_hook, 0x201d, 0x410b);

animate:
  CYC(0x410b, 0x410e); interactionAnimate_hook(gb);
  return;

state2:
  CYC(0x410e, 0x410f); H = D;
  CYC(0x410f, 0x4111); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x4111, 0x4112); A = mem_rd(gb, HL);
  CYC(0x4112, 0x4114); alu_xor(gb, 0x80);
  CYC(0x4114, 0x4115); mem_wr(gb, HL, A);
  CYC(0x4115, 0x4117); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4117, 0x4119); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4119, 0x411b); goto animate;
  }
  CYC(0x4119, 0x411b);

delete:
  CYC(0x411b, 0x411e); interactionDelete_hook(gb);
}

void clearFallDownHoleEventBuffer_hook(GB *gb) {
  CYC(0x414b, 0x414e); SET_HL(wTmpcfc0_fallDownHoleEvent_cfd8);
  CYC(0x414e, 0x4150); B = 0x08;
  CYC(0x4150, 0x4152); A = 0xff;
  CYC(0x4152, 0x4155); fillMemory_hook(gb);
}
