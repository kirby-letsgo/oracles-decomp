#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode79);
  CYC(b_+117, b_+120); SET_HL(w1Link_yh);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+121, b_+123); alu_add(gb, 0x05);
  CYC(b_+123, b_+124); B = A;
  CYC(b_+124, b_+125); L = alu_inc8(gb, L);
  CYC(b_+125, b_+126); C = mem_rd(gb, HL);
  CYC(b_+126, b_+129); interactionCheckContainsPoint_hook(gb); // jp
}

// INTERAC_MOVING_PLATFORM
void interactionCode79_hook(GB *gb) {
  BASE(interactionCode79);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode79_jump_table(gb);
    if (target == b_+8) goto state0;
    goto state1;
  }

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+17); alu_and(gb, 0x07);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); A = B;
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+21, b_+23); A = alu_swap(gb, A);
  CYC(b_+23, b_+24); alu_rlca(gb);
  CYC(b_+24, b_+26); alu_and(gb, 0x1f);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CALL_C(b_+27, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+30);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+32, b_+34); A = 0x14; // SPEED_80
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+41); SET_HL(b_+68); // interactionCode79@collisionRadii
  CYC(b_+41, b_+42); interactionCode79_addDoubleIndexToHl_from_rst(gb, b_+42);
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+47); E = alu_inc8(gb, E);
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+52); SET_HL(GV((SYM(interactionCode7a) + 12), 0x4680)); // scriptHelp.movingPlatform_loadScript
  CYC(b_+52, b_+54); E = 0x15;
  CALL_C(b_+54, interBankCall_hook, 0x008a, b_+57);
  CYC(b_+57, b_+60); SET_HL(GV((SYM(interactionCode7a__state0) + 28), 0x469d)); // scriptHelp.movingPlatform_runScript
  CYC(b_+60, b_+62); E = 0x15;
  CALL_C(b_+62, interBankCall_hook, 0x008a, b_+65);
  CYC(b_+65, b_+68); TAIL(objectSetVisible83); // jp

state1:
  CYC(b_+80, b_+83); A = W8(wLinkRidingObject);
  CYC(b_+83, b_+84); alu_cp(gb, D);
  if (F & FZ) { CYCT(b_+84, b_+86); goto linkOnPlatform; } // jr z
  CYC(b_+84, b_+86);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+87, b_+89); goto updateSubstate; } // jr nz
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+92); push_effect(gb, b_+92); interactionCode79_checkLinkTouching(gb);
  if (gb->pc == b_+92 && gb->sp == sp0_) goto afterCheck1;
  return;
afterCheck1:
  if (!(F & FC)) { CYCT(b_+92, b_+94); goto updateSubstate; } // jr nc
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+95); A = D;
  CYC(b_+95, b_+98); W8(wLinkRidingObject) = A;
  CYC(b_+98, b_+100); goto updateSubstate; // jr

linkOnPlatform:
  CYC(b_+100, b_+103); push_effect(gb, b_+103); interactionCode79_checkLinkTouching(gb);
  if (gb->pc == b_+103 && gb->sp == sp0_) goto afterCheck2;
  return;
afterCheck2:
  if (F & FC) { CYCT(b_+103, b_+105); goto updateSubstate; } // jr c
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+106); alu_xor(gb, A);
  CYC(b_+106, b_+109); W8(wLinkRidingObject) = A;

updateSubstate:
  CYC(b_+109, b_+111); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+111, b_+112); A = mem_rd(gb, DE);
  {
    CYC(b_+112, b_+113); push_effect(gb, b_+113);
    uint16_t target = interactionCode79_jump_table(gb);
    if (target == b_+129) goto substate0;
    goto substate1;
  }

substate0:
  CALL_C(b_+129, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+132);
  if (!(F & FZ)) { RET_TAKEN(b_+132); return; } // ret nz
  CYC(b_+132, b_+133);
  CYC(b_+133, b_+136); SET_HL(GV((SYM(interactionCode7a__state0) + 28), 0x469d)); // scriptHelp.movingPlatform_runScript
  CYC(b_+136, b_+138); E = 0x15;
  CALL_C(b_+138, interBankCall_hook, 0x008a, b_+141);
  RET(b_+141); return; // ret

substate1:
  CYC(b_+142, b_+145); A = W8(wLinkPlayingInstrument);
  CYC(b_+145, b_+146); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+146); return; } // ret nz
  CYC(b_+146, b_+147);
  CALL_C(b_+147, objectApplySpeed_hook, SYM(objectApplySpeed), b_+150);
  CYC(b_+150, b_+153); A = W8(wLinkRidingObject);
  CYC(b_+153, b_+154); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(b_+154, b_+156); goto substate0; } // jr nz
  CYC(b_+154, b_+156);
  CYC(b_+156, b_+159); A = W8(w1Link_state);
  CYC(b_+159, b_+161); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+161, b_+163); goto substate0; } // jr nz
  CYC(b_+161, b_+163);
  CYC(b_+163, b_+165); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+165, b_+166); A = mem_rd(gb, DE);
  CYC(b_+166, b_+167); B = A;
  CYC(b_+167, b_+169); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+169, b_+170); A = mem_rd(gb, DE);
  CYC(b_+170, b_+171); C = A;
  CALL_C(b_+171, updateLinkPositionGivenVelocity_hook, SYM(updateLinkPositionGivenVelocity), b_+174);
  CYC(b_+174, b_+176); goto substate0; // jr
}
