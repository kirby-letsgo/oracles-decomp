#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode79_jump_table(GB *gb) {
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

static void interactionCode79_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode79@checkLinkTouching: sets carry if Link is inside the platform's
// hitbox. Reached only by two genuine `call`s from interactionCode79 itself; never
// separately hooked (has no external callers), so calls to it hand-inline the
// push_effect/resume-check dance instead of going through CALL_C.
static void interactionCode79_checkLinkTouching(GB *gb) {
  CYC(0x40da, 0x40dd); SET_HL(w1Link_yh);
  CYC(0x40dd, 0x40de); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x40de, 0x40e0); alu_add(gb, 0x05);
  CYC(0x40e0, 0x40e1); B = A;
  CYC(0x40e1, 0x40e2); L = alu_inc8(gb, L);
  CYC(0x40e2, 0x40e3); C = mem_rd(gb, HL);
  CYC(0x40e3, 0x40e6); interactionCheckContainsPoint_hook(gb); // jp
}

// INTERAC_MOVING_PLATFORM
void interactionCode79_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4065, 0x4067); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4067, 0x4068); A = mem_rd(gb, DE);
  {
    CYC(0x4068, 0x4069); push_effect(gb, 0x4069);
    uint16_t target = interactionCode79_jump_table(gb);
    if (target == 0x406d) goto state0;
    goto state1;
  }

state0:
  CYC(0x406d, 0x406f); A = 0x01;
  CYC(0x406f, 0x4070); mem_wr(gb, DE, A);
  CYC(0x4070, 0x4072); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4072, 0x4073); A = mem_rd(gb, DE);
  CYC(0x4073, 0x4074); B = A;
  CYC(0x4074, 0x4076); alu_and(gb, 0x07);
  CYC(0x4076, 0x4077); mem_wr(gb, DE, A);
  CYC(0x4077, 0x4078); A = B;
  CYC(0x4078, 0x407a); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x407a, 0x407c); A = alu_swap(gb, A);
  CYC(0x407c, 0x407d); alu_rlca(gb);
  CYC(0x407d, 0x407f); alu_and(gb, 0x1f);
  CYC(0x407f, 0x4080); mem_wr(gb, DE, A);
  CALL_C(0x4080, interactionInitGraphics_hook, 0x15fb, 0x4083);
  CYC(0x4083, 0x4085); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x4085, 0x4087); A = 0x14; // SPEED_80
  CYC(0x4087, 0x4088); mem_wr(gb, DE, A);
  CYC(0x4088, 0x408a); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x408a, 0x408b); A = mem_rd(gb, DE);
  CYC(0x408b, 0x408e); SET_HL(0x40a9); // interactionCode79@collisionRadii
  CYC(0x408e, 0x408f); interactionCode79_addDoubleIndexToHl_from_rst(gb, 0x408f);
  CYC(0x408f, 0x4091); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x4091, 0x4092); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4092, 0x4093); mem_wr(gb, DE, A);
  CYC(0x4093, 0x4094); E = alu_inc8(gb, E);
  CYC(0x4094, 0x4095); A = mem_rd(gb, HL);
  CYC(0x4095, 0x4096); mem_wr(gb, DE, A);
  CYC(0x4096, 0x4099); SET_HL(0x4121); // scriptHelp.movingPlatform_loadScript
  CYC(0x4099, 0x409b); E = 0x15;
  CALL_C(0x409b, interBankCall_hook, 0x008a, 0x409e);
  CYC(0x409e, 0x40a1); SET_HL(0x413e); // scriptHelp.movingPlatform_runScript
  CYC(0x40a1, 0x40a3); E = 0x15;
  CALL_C(0x40a3, interBankCall_hook, 0x008a, 0x40a6);
  CYC(0x40a6, 0x40a9); objectSetVisible83_hook(gb); return; // jp

state1:
  CYC(0x40b5, 0x40b8); A = W8(wLinkRidingObject);
  CYC(0x40b8, 0x40b9); alu_cp(gb, D);
  if (F & FZ) { CYCT(0x40b9, 0x40bb); goto linkOnPlatform; } // jr z
  CYC(0x40b9, 0x40bb);
  CYC(0x40bb, 0x40bc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x40bc, 0x40be); goto updateSubstate; } // jr nz
  CYC(0x40bc, 0x40be);
  CYC(0x40be, 0x40c1); push_effect(gb, 0x40c1); interactionCode79_checkLinkTouching(gb);
  if (gb->pc == 0x40c1 && gb->sp == sp0_) goto afterCheck1;
  return;
afterCheck1:
  if (!(F & FC)) { CYCT(0x40c1, 0x40c3); goto updateSubstate; } // jr nc
  CYC(0x40c1, 0x40c3);
  CYC(0x40c3, 0x40c4); A = D;
  CYC(0x40c4, 0x40c7); W8(wLinkRidingObject) = A;
  CYC(0x40c7, 0x40c9); goto updateSubstate; // jr

linkOnPlatform:
  CYC(0x40c9, 0x40cc); push_effect(gb, 0x40cc); interactionCode79_checkLinkTouching(gb);
  if (gb->pc == 0x40cc && gb->sp == sp0_) goto afterCheck2;
  return;
afterCheck2:
  if (F & FC) { CYCT(0x40cc, 0x40ce); goto updateSubstate; } // jr c
  CYC(0x40cc, 0x40ce);
  CYC(0x40ce, 0x40cf); alu_xor(gb, A);
  CYC(0x40cf, 0x40d2); W8(wLinkRidingObject) = A;

updateSubstate:
  CYC(0x40d2, 0x40d4); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x40d4, 0x40d5); A = mem_rd(gb, DE);
  {
    CYC(0x40d5, 0x40d6); push_effect(gb, 0x40d6);
    uint16_t target = interactionCode79_jump_table(gb);
    if (target == 0x40e6) goto substate0;
    goto substate1;
  }

substate0:
  CALL_C(0x40e6, interactionDecCounter1_hook, 0x23cc, 0x40e9);
  if (!(F & FZ)) { RET_TAKEN(0x40e9); return; } // ret nz
  CYC(0x40e9, 0x40ea);
  CYC(0x40ea, 0x40ed); SET_HL(0x413e); // scriptHelp.movingPlatform_runScript
  CYC(0x40ed, 0x40ef); E = 0x15;
  CALL_C(0x40ef, interBankCall_hook, 0x008a, 0x40f2);
  RET(0x40f2); return; // ret

substate1:
  CYC(0x40f3, 0x40f6); A = W8(wLinkPlayingInstrument);
  CYC(0x40f6, 0x40f7); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x40f7); return; } // ret nz
  CYC(0x40f7, 0x40f8);
  CALL_C(0x40f8, objectApplySpeed_hook, 0x201d, 0x40fb);
  CYC(0x40fb, 0x40fe); A = W8(wLinkRidingObject);
  CYC(0x40fe, 0x40ff); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(0x40ff, 0x4101); goto substate0; } // jr nz
  CYC(0x40ff, 0x4101);
  CYC(0x4101, 0x4104); A = W8(w1Link_state);
  CYC(0x4104, 0x4106); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x4106, 0x4108); goto substate0; } // jr nz
  CYC(0x4106, 0x4108);
  CYC(0x4108, 0x410a); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x410a, 0x410b); A = mem_rd(gb, DE);
  CYC(0x410b, 0x410c); B = A;
  CYC(0x410c, 0x410e); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x410e, 0x410f); A = mem_rd(gb, DE);
  CYC(0x410f, 0x4110); C = A;
  CALL_C(0x4110, updateLinkPositionGivenVelocity_hook, 0x231e, 0x4113);
  CYC(0x4113, 0x4115); goto substate0; // jr
}
