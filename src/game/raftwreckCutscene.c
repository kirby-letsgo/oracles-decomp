#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCode9b_jump_table(GB *gb) {
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

static void interactionCode9b_add_a_to_hl(GB *gb) {
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

// 0b:530f, called once from interactionCode9b@state1.
void interactionCode9b_updateSubstate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x530f, 0x5311); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5311, 0x5312); A = mem_rd(gb, DE);
  CYC(0x5312, 0x5314); alu_cp(gb, 0x02);
  if (!(F & FC)) { CALL_C_CC(0x5314, interactionRunScript_hook, 0x2552, 0x5317); } else { CYC(0x5314, 0x5317); } // call nc
  CYC(0x5317, 0x5319); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5319, 0x531a); A = mem_rd(gb, DE);
  CYC(0x531a, 0x531b); push_effect(gb, 0x531b);
  switch (interactionCode9b_jump_table(gb)) {
    case 0x532d: goto substate0;
    case 0x5356: goto substate1;
    case 0x536a: goto substate2;
    case 0x537d: goto substate3;
    case 0x539b: goto substate4;
    case 0x53a1: goto substate6;
    case 0x53ad: goto substate7;
    case 0x53c4: goto substate8;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x532d, 0x5330); A = mem_rd(gb, wScrollMode);
  CYC(0x5330, 0x5332); alu_and(gb, 0x01); // SCROLLMODE_01
  if (F & FZ) { CYCT(0x5332, 0x5333); ret_effect(gb); return; } // ret z
  CYC(0x5332, 0x5333);
  CALL_C(0x5333, interactionIncSubstate_hook, 0x23e5, 0x5336);
  CYC(0x5336, 0x5338); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5338, 0x533a); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x533a, 0x533c); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x533c, 0x533d); A = mem_rd(gb, HL);
  CYC(0x533d, 0x533f); alu_sub(gb, 0x50);
  CYC(0x533f, 0x5341); C = 0x03; // DIR_LEFT
  CYC(0x5341, 0x5343); B = 0x18; // ANGLE_LEFT
  if (!(F & FC)) { CYCT(0x5343, 0x5345); goto l_534b; } // jr nc
  CYC(0x5343, 0x5345);
  CYC(0x5345, 0x5347); C = 0x01; // DIR_RIGHT
  CYC(0x5347, 0x5349); B = 0x08; // ANGLE_RIGHT
  CYC(0x5349, 0x534a); alu_cpl(gb);
  CYC(0x534a, 0x534b); A = alu_inc8(gb, A);

l_534b:
  CYC(0x534b, 0x534d); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x534d, 0x534e); mem_wr(gb, HL, B);
  CYC(0x534e, 0x5350); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5350, 0x5351); alu_add(gb, A);
  CYC(0x5351, 0x5352); mem_wr(gb, HL, A);
  CYC(0x5352, 0x5353); A = C;
  CYC(0x5353, 0x5356); setLinkDirection_hook(gb); return; // jp

substate1:
  CYC(0x5356, 0x5357); H = D;
  CYC(0x5357, 0x5359); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5359, 0x535a); A = mem_rd(gb, HL);
  CYC(0x535a, 0x535b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x535b, 0x535d); goto l_5361; } // jr z
  CYC(0x535b, 0x535d);
  CYC(0x535d, 0x535e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x535e, 0x5361); objectApplySpeed_hook(gb); return; // jp

l_5361:
  CALL_C(0x5361, interactionIncSubstate_hook, 0x23e5, 0x5364);
  CYC(0x5364, 0x5367); SET_HL(0x7ac6); // mainScripts.raftwreckCutsceneScript
  CYC(0x5367, 0x536a); interactionSetScript_hook(gb); return; // jp

substate2:
  CYC(0x536a, 0x536d); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x536d, 0x536f); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x536f, 0x5370); ret_effect(gb); return; } // ret nz
  CYC(0x536f, 0x5370);

// @initScreenFlashing (falls into from substate2 above; also reached via goto from substate4)
initScreenFlashing:
  CYC(0x5370, 0x5373); SET_HL(wGenericCutscene_cbb3);
  CYC(0x5373, 0x5375); mem_wr(gb, HL, 0x00);
  CYC(0x5375, 0x5378); SET_HL(wGenericCutscene_cbba);
  CYC(0x5378, 0x537a); mem_wr(gb, HL, 0xff);
  CYC(0x537a, 0x537d); interactionIncSubstate_hook(gb); return; // jp

substate3:
  CYC(0x537d, 0x5380); SET_HL(wGenericCutscene_cbb3);
  CYC(0x5380, 0x5382); B = 0x01;
  CALL_C(0x5382, flashScreen_hook, 0x2d73, 0x5385);
  if (F & FZ) { CYCT(0x5385, 0x5386); ret_effect(gb); return; } // ret z
  CYC(0x5385, 0x5386);
  CALL_C(0x5386, interactionIncSubstate_hook, 0x23e5, 0x5389);
  CYC(0x5389, 0x538a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x538a, 0x538c); alu_cp(gb, 0x03);
  CYC(0x538c, 0x538e); A = 0x5a;
  if (F & FZ) { CYCT(0x538e, 0x5390); goto l_5392; } // jr z
  CYC(0x538e, 0x5390);
  CYC(0x5390, 0x5392); A = 0x78;

l_5392:
  CYC(0x5392, 0x5393); mem_wr(gb, HL, A);
  CYC(0x5393, 0x5395); A = 0xf1;
  CYC(0x5395, 0x5398); mem_wr(gb, wPaletteThread_parameter, A);
  CYC(0x5398, 0x539b); darkenRoom_hook(gb); return; // jp

substate4:
  CALL_C(0x539b, interactionDecCounter1_hook, 0x23cc, 0x539e);
  if (!(F & FZ)) { CYCT(0x539e, 0x539f); ret_effect(gb); return; } // ret nz
  CYC(0x539e, 0x539f);
  CYC(0x539f, 0x53a1); goto initScreenFlashing; // jr

substate6:
  CALL_C(0x53a1, interactionDecCounter1_hook, 0x23cc, 0x53a4);
  if (!(F & FZ)) { CYCT(0x53a4, 0x53a5); ret_effect(gb); return; } // ret nz
  CYC(0x53a4, 0x53a5);
  CYC(0x53a5, 0x53a7); A = 0x02;
  CYC(0x53a7, 0x53aa); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(0x53aa, 0x53ad); interactionIncSubstate_hook(gb); return; // jp

substate7:
  CYC(0x53ad, 0x53b0); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x53b0, 0x53b2); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x53b2, 0x53b4); goto l_53bc; } // jr nz
  CYC(0x53b2, 0x53b4);
  CALL_C(0x53b4, interactionIncSubstate_hook, 0x23e5, 0x53b7);
  CYC(0x53b7, 0x53b9); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x53b9, 0x53bb); mem_wr(gb, HL, 20);
  RET(0x53bb); return;

l_53bc:
  CYC(0x53bc, 0x53be); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x53be, 0x53bf); A = mem_rd(gb, DE);
  CYC(0x53bf, 0x53c0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x53c0, 0x53c1); ret_effect(gb); return; } // ret z
  CYC(0x53c0, 0x53c1);
  CYC(0x53c1, 0x53c4); goto oscillateY; // jp

substate8:
  CALL_C(0x53c4, interactionDecCounter1_hook, 0x23cc, 0x53c7);
  if (!(F & FZ)) { CYCT(0x53c7, 0x53c8); ret_effect(gb); return; } // ret nz
  CYC(0x53c7, 0x53c8);
  CYC(0x53c8, 0x53ca); A = 0xfa; // SNDCTRL_FAST_FADEOUT
  CALL_C(0x53ca, playSound_b00_hook, 0x0c98, 0x53cd);
  CALL_C(0x53cd, getThisRoomFlags_hook, 0x197d, 0x53d0);
  CYC(0x53d0, 0x53d2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6))); // ROOMFLAG_BIT_40
  CYC(0x53d2, 0x53d5); SET_HL(w1Companion_enabled);
  CYC(0x53d5, 0x53d7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(0x53d7, 0x53d9); A = 0xd0; // >w1Link
  CYC(0x53d9, 0x53dc); mem_wr(gb, wLinkObjectIndex, A);
  CYC(0x53dc, 0x53df); SET_HL(0x53e2); // @tokayWarpDest
  CYC(0x53df, 0x53e2); setWarpDestVariables_hook(gb); return; // jp

oscillateY:
  CYC(0x53e7, 0x53ea); A = mem_rd(gb, wFrameCounter);
  CYC(0x53ea, 0x53ec); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x53ec, 0x53ed); ret_effect(gb); return; } // ret nz
  CYC(0x53ec, 0x53ed);
  CYC(0x53ed, 0x53f0); A = mem_rd(gb, wFrameCounter);
  CYC(0x53f0, 0x53f2); alu_and(gb, 0x38);
  CYC(0x53f2, 0x53f4); A = alu_swap(gb, A);
  CYC(0x53f4, 0x53f5); alu_rlca(gb);
  CYC(0x53f5, 0x53f8); SET_HL(0x53ff); // @yOscillation
  CYC(0x53f8, 0x53f9); push_effect(gb, 0x53f9); interactionCode9b_add_a_to_hl(gb); // rst $10
  CYC(0x53f9, 0x53fb); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x53fb, 0x53fc); A = mem_rd(gb, DE);
  CYC(0x53fc, 0x53fd); alu_add(gb, mem_rd(gb, HL));
  CYC(0x53fd, 0x53fe); mem_wr(gb, DE, A);
  RET(0x53fe); return;

  // @tokayWarpDest (0b:53e2): m_HardcodedWarpA ROOM_AGES_1aa, $00, $42, $03 -- pure ROM data
  // referenced only via SET_HL(0x53e2) above.
  // @yOscillation (0b:53ff): pure ROM data referenced only via SET_HL(0x53ff) above.
}

// ==================================================================================================
// INTERAC_RAFTWRECK_CUTSCENE
// ==================================================================================================
void interactionCode9b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x52d6, 0x52d8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x52d8, 0x52d9); A = mem_rd(gb, DE);
  CYC(0x52d9, 0x52da); push_effect(gb, 0x52da);
  switch (interactionCode9b_jump_table(gb)) {
    case 0x52de: goto state0;
    case 0x5303: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x52de, getThisRoomFlags_hook, 0x197d, 0x52e1);
  CYC(0x52e1, 0x52e3); alu_bit(gb, 6, A); // ROOMFLAG_BIT_40
  if (!(F & FZ)) { CYCT(0x52e3, 0x52e6); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x52e3, 0x52e6);
  CYC(0x52e6, 0x52e8); A = 0x01;
  CYC(0x52e8, 0x52eb); mem_wr(gb, wDisabledObjects, A);
  CYC(0x52eb, 0x52ee); mem_wr(gb, wMenuDisabled, A);
  CYC(0x52ee, 0x52f1); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x52f1, 0x52f2); H = A;
  CYC(0x52f2, 0x52f4); L = 0x0d; // SpecialObject.xh
  CYC(0x52f4, 0x52f5); C = mem_rd(gb, HL);
  CYC(0x52f5, 0x52f7); B = 0x76;
  CALL_C(0x52f7, interactionSetPosition_hook, 0x2773, 0x52fa);
  CALL_C(0x52fa, setLinkForceStateToState08_hook, 0x2aad, 0x52fd);
  CYC(0x52fd, 0x5300); mem_wr(gb, wTmpcfc0_genericCutscene_cfd0, A);
  CYC(0x5300, 0x5303); interactionIncState_hook(gb); return; // jp

state1:
  CALL_C(0x5303, interactionCode9b_updateSubstate_hook, 0x530f, 0x5306);
  CYC(0x5306, 0x5309); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x5309, 0x530a); H = A;
  CYC(0x530a, 0x530c); L = 0x0b; // SpecialObject.yh
  CYC(0x530c, 0x530f); objectCopyPosition_hook(gb); return; // jp
}
