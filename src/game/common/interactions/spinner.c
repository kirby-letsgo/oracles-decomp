#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode7d_jump_table(GB *gb) {
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

static void interactionCode7d_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

void spinner_setLinkRelativePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4400, 0x4402); B = 0xd0; // >w1Link
  CYC(0x4402, 0x4404); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x4404, 0x4406); C = 0x0b; // <w1Link.yh
  CYC(0x4406, 0x4409); push_effect(gb, 0x4409); goto func;
afterFunc1:
  CYC(0x4409, 0x440b); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x440b, 0x440d); C = 0x0d; // <w1Link.xh

// spinner_setLinkRelativePosition@func: called once via the genuine call above, resuming
// at 0x4409 and staying within this function, then fallen into a second time with no call
// at all -- that ret pops whatever this function's own caller pushed instead.
func:
  CYC(0x440d, 0x440e); A = mem_rd(gb, DE);
  CYC(0x440e, 0x440f); alu_add(gb, mem_rd(gb, HL));
  CYC(0x440f, 0x4410); SET_HL(HL + 1);
  CYC(0x4410, 0x4411); mem_wr(gb, BC, A);
  RET(0x4411);
  if (gb->pc == 0x4409 && gb->sp == sp0_) goto afterFunc1;
  return;
}

void spinner_updateLinkPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x43e7, 0x43e9); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x43e9, 0x43ea); A = mem_rd(gb, DE);
  CYC(0x43ea, 0x43eb); B = A;
  CYC(0x43eb, 0x43ec); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x43ec); return; } // ret z
  CYC(0x43ec, 0x43ed);
  CYC(0x43ed, 0x43ee); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(0x43ee); return; } // ret z
  CYC(0x43ee, 0x43ef);
  CYC(0x43ef, 0x43f0); alu_xor(gb, A);
  CYC(0x43f0, 0x43f1); mem_wr(gb, DE, A);
  CYC(0x43f1, 0x43f3); A = 0x70; // SND_DOORCLOSE
  CALL_C(0x43f3, playSound_b00_hook, 0x0c98, 0x43f6);
  CYC(0x43f6, 0x43f8); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x43f8, 0x43f9); A = mem_rd(gb, DE);
  CYC(0x43f9, 0x43fa); alu_add(gb, B);
  CYC(0x43fa, 0x43fc); alu_and(gb, 0x0f);
  CYC(0x43fc, 0x43ff); SET_HL(0x4432); // spinner_linkRelativePositions
  CYC(0x43ff, 0x4400); interactionCode7d_addDoubleIndexToHl_from_rst(gb, 0x4400);
  spinner_setLinkRelativePosition_hook(gb); return; // falls through
}

void spinner_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x43ac, 0x43ae); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x43ae, 0x43af); A = mem_rd(gb, DE);
  {
    CYC(0x43af, 0x43b0); push_effect(gb, 0x43b0);
    uint16_t target = interactionCode7d_jump_table(gb);
    if (target == 0x43b4) goto state0;
    goto state1;
  }

state0:
  CYC(0x43b4, 0x43b6); A = 0x01;
  CYC(0x43b6, 0x43b7); mem_wr(gb, DE, A);
  CALL_C(0x43b7, interactionInitGraphics_hook, 0x15fb, 0x43ba);
  CYC(0x43ba, 0x43bc); E = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x43bc, 0x43bd); A = mem_rd(gb, DE);
  CYC(0x43bd, 0x43be); H = D;
  CYC(0x43be, 0x43c0); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x43c0, 0x43c1); E = L;
  CYC(0x43c1, 0x43c2); A = mem_rd(gb, HL);
  CYC(0x43c2, 0x43c3); mem_wr(gb, DE, A);
  CALL_C(0x43c3, objectSetVisible82_hook, 0x1e69, 0x43c6);

state1:
  CYC(0x43c6, 0x43c8); E = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x43c8, 0x43c9); A = mem_rd(gb, DE);
  CYC(0x43c9, 0x43ca); H = A;
  CYC(0x43ca, 0x43cc); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x43cc, 0x43cd); E = L;
  CYC(0x43cd, 0x43ce); A = mem_rd(gb, DE);
  CYC(0x43ce, 0x43cf); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x43cf, 0x43d1); goto afterAnim; } // jr z
  CYC(0x43cf, 0x43d1);
  CYC(0x43d1, 0x43d2); A = mem_rd(gb, HL);
  CYC(0x43d2, 0x43d3); mem_wr(gb, DE, A);
  CYC(0x43d3, 0x43d5); A = alu_swap(gb, A);
  CYC(0x43d5, 0x43d6); alu_rlca(gb);
  CYC(0x43d6, 0x43d8); alu_add(gb, 0x02);
  CALL_C(0x43d8, interactionSetAnimation_hook, 0x262e, 0x43db);

afterAnim:
  CYC(0x43db, 0x43dd); E = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x43dd, 0x43de); A = mem_rd(gb, DE);
  CYC(0x43de, 0x43df); H = A;
  CYC(0x43df, 0x43e1); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x43e1, 0x43e2); E = L;
  CYC(0x43e2, 0x43e3); A = mem_rd(gb, HL);
  CYC(0x43e3, 0x43e4); mem_wr(gb, DE, A);
  CYC(0x43e4, 0x43e7); interactionAnimate_hook(gb); return; // jp
}

// INTERAC_SPINNER
void interactionCode7d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42ac, 0x42ae); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x42ae, 0x42af); A = mem_rd(gb, DE);
  {
    CYC(0x42af, 0x42b0); push_effect(gb, 0x42b0);
    uint16_t target = interactionCode7d_jump_table(gb);
    if (target == 0x43ac) { spinner_subid02_hook(gb); return; }
    goto subid00;
  }

subid00:
  CYC(0x42b6, 0x42b8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x42b8, 0x42b9); A = mem_rd(gb, DE);
  {
    CYC(0x42b9, 0x42ba); push_effect(gb, 0x42ba);
    uint16_t target = interactionCode7d_jump_table(gb);
    if (target == 0x42c4) goto state0;
    if (target == 0x2552) { interactionRunScript_hook(gb); return; }
    if (target == 0x42fd) goto state2;
    if (target == 0x435b) goto state3;
    goto state4;
  }

state0:
  CYC(0x42c4, 0x42c6); A = 0x01;
  CYC(0x42c6, 0x42c7); mem_wr(gb, DE, A);
  CYC(0x42c7, 0x42c8); H = D;
  CYC(0x42c8, 0x42ca); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x42ca, 0x42cb); A = mem_rd(gb, HL);
  CYC(0x42cb, 0x42cd); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x42cd, 0x42ce); mem_wr(gb, HL, A);
  CYC(0x42ce, 0x42d1); A = W8(wSpinnerState);
  CYC(0x42d1, 0x42d2); alu_and(gb, mem_rd(gb, HL));
  CYC(0x42d2, 0x42d4); A = 0x01;
  if (!(F & FZ)) { CYCT(0x42d4, 0x42d6); goto afterSubidCalc; } // jr nz
  CYC(0x42d4, 0x42d6);
  CYC(0x42d6, 0x42d7); A = alu_dec8(gb, A);

afterSubidCalc:
  CYC(0x42d7, 0x42d9); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x42d9, 0x42da); mem_wr(gb, HL, A);
  CYC(0x42da, 0x42dc); A = alu_swap(gb, A);
  CYC(0x42dc, 0x42dd); alu_rrca(gb);
  CYC(0x42dd, 0x42df); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x42df, 0x42e0); mem_wr(gb, HL, A);
  CYC(0x42e0, 0x42e2); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x42e2, 0x42e3); A = mem_rd(gb, HL);
  CALL_C(0x42e3, setShortPosition_hook, 0x20b8, 0x42e6);
  CALL_C(0x42e6, interactionInitGraphics_hook, 0x15fb, 0x42e9);
  CYC(0x42e9, 0x42ec); SET_HL(0x49b8); // mainScripts.spinnerScript_initialization
  CALL_C(0x42ec, interactionSetScript_hook, 0x2544, 0x42ef);
  CALL_C(0x42ef, objectSetVisible82_hook, 0x1e69, 0x42f2);
  CYC(0x42f2, 0x42f5); SET_BC(0x7d02); // INTERAC_SPINNER, $02
  CALL_C(0x42f5, objectCreateInteraction_hook, 0x24c5, 0x42f8);
  if (!(F & FZ)) { RET_TAKEN(0x42f8); return; } // ret nz
  CYC(0x42f8, 0x42f9);
  CYC(0x42f9, 0x42fb); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x42fb, 0x42fc); mem_wr(gb, HL, D);
  RET(0x42fc); return; // ret

state2:
  CYC(0x42fd, 0x4300); SET_HL(wcc95);
  CYC(0x4300, 0x4303); A = W8(wLinkInAir);
  CYC(0x4303, 0x4304); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4304, 0x4306); goto revertToState1; } // jr nz
  CYC(0x4304, 0x4306);
  CYC(0x4306, 0x4308); alu_bit(gb, 4, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4308, 0x430a); goto beginTurning; } // jr nz
  CYC(0x4308, 0x430a);

revertToState1:
  CYC(0x430a, 0x430c); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 7));
  CYC(0x430c, 0x430e); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x430e, 0x4310); A = 0x01;
  CYC(0x4310, 0x4311); mem_wr(gb, DE, A);
  CYC(0x4311, 0x4314); SET_HL(0x49bc); // mainScripts.spinnerScript_waitForLink
  CYC(0x4314, 0x4317); interactionSetScript_hook(gb); return; // jp

beginTurning:
  CYC(0x4317, 0x4319); A = 0x03;
  CYC(0x4319, 0x431a); mem_wr(gb, DE, A);
  CALL_C(0x431a, clearAllParentItems_hook, 0x2c10, 0x431d);
  CYC(0x431d, 0x431f); C = 0x28;
  CALL_C(0x431f, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x4322);
  CYC(0x4322, 0x4324); A = alu_sra(gb, A);
  CYC(0x4324, 0x4326); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x4326, 0x4327); mem_wr(gb, DE, A);
  CYC(0x4327, 0x4328); B = A;
  CYC(0x4328, 0x4329); E = alu_inc8(gb, E);
  CYC(0x4329, 0x432a); A = mem_rd(gb, DE);
  CYC(0x432a, 0x432b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x432b, 0x432d); goto clockwise; } // jr nz
  CYC(0x432b, 0x432d);

  // interactionCode7d@counterClockwise
  CYC(0x432d, 0x432e); A = B;
  CYC(0x432e, 0x432f); alu_add(gb, A);
  CYC(0x432f, 0x4332); SET_HL(0x4412); // spinner_counterClockwiseData
  CYC(0x4332, 0x4333); interactionCode7d_addDoubleIndexToHl_from_rst(gb, 0x4333);
  CYC(0x4333, 0x4335); goto afterDirectionTable; // jr

clockwise:
  CYC(0x4335, 0x4336); A = B;
  CYC(0x4336, 0x4337); alu_add(gb, A);
  CYC(0x4337, 0x433a); SET_HL(0x4422); // spinner_clockwiseData
  CYC(0x433a, 0x433b); interactionCode7d_addDoubleIndexToHl_from_rst(gb, 0x433b);

afterDirectionTable:
  CALL_C(0x433b, spinner_setLinkRelativePosition_hook, 0x4400, 0x433e);
  CYC(0x433e, 0x433f); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x433f, 0x4341); C = 0x08; // <w1Link.direction
  CYC(0x4341, 0x4342); mem_wr(gb, BC, A);
  CYC(0x4342, 0x4344); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x4344, 0x4345); A = mem_rd(gb, HL);
  CYC(0x4345, 0x4346); mem_wr(gb, DE, A);
  CALL_C(0x4346, setLinkForceStateToState08_hook, 0x2aad, 0x4349);
  CYC(0x4349, 0x434c); A = W8(wDisabledObjects);
  CYC(0x434c, 0x434e); alu_or(gb, 0x80);
  CYC(0x434e, 0x4351); W8(wDisabledObjects) = A;
  CYC(0x4351, 0x4353); A = 0x04;
  CALL_C(0x4353, setScreenShakeCounter_hook, 0x24bb, 0x4356);
  CYC(0x4356, 0x4358); A = 0x6c; // SND_OPENCHEST
  CYC(0x4358, 0x435b); playSound_b00_hook(gb); return; // jp

state3:
  CALL_C(0x435b, spinner_updateLinkPosition_hook, 0x43e7, 0x435e);
  CYC(0x435e, 0x4360); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4360, 0x4361); A = mem_rd(gb, DE);
  CYC(0x4361, 0x4362); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x4362, 0x4365); interactionAnimate_hook(gb); return; } // jp nz
  CYC(0x4362, 0x4365);
  CYC(0x4365, 0x4366); H = D;
  CYC(0x4366, 0x4368); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4368, 0x436a); mem_wr(gb, HL, 0x04);
  CYC(0x436a, 0x436c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x436c, 0x436e); mem_wr(gb, HL, 0x10);
  CYC(0x436e, 0x436f); alu_xor(gb, A);
  CYC(0x436f, 0x4372); W8(wDisabledObjects) = A;
  CYC(0x4372, 0x4375); SET_HL(w1Link_direction);
  CYC(0x4375, 0x4376); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4376, 0x4378); A = alu_swap(gb, A);
  CYC(0x4378, 0x4379); alu_rrca(gb);
  CYC(0x4379, 0x437a); mem_wr(gb, HL, A);
  CYC(0x437a, 0x437d); SET_HL(wLinkForceState);
  CYC(0x437d, 0x437f); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(0x437f, 0x4380); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4380, 0x4381); L = alu_inc8(gb, L);
  CYC(0x4381, 0x4383); mem_wr(gb, HL, 0x10);
  CYC(0x4383, 0x4386); SET_HL(wcc95);
  CYC(0x4386, 0x4388); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 7));
  RET(0x4388); return; // ret

state4:
  CALL_C(0x4389, interactionDecCounter1_hook, 0x23cc, 0x438c);
  if (!(F & FZ)) { RET_TAKEN(0x438c); return; } // ret nz
  CYC(0x438c, 0x438d);
  CYC(0x438d, 0x438f); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x438f, 0x4392); A = W8(wSpinnerState);
  CYC(0x4392, 0x4393); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x4393, 0x4396); W8(wSpinnerState) = A;
  CYC(0x4396, 0x4398); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x4398, 0x4399); A = mem_rd(gb, HL);
  CYC(0x4399, 0x439b); alu_xor(gb, 0x01);
  CYC(0x439b, 0x439c); mem_wr(gb, HL, A);
  CYC(0x439c, 0x439e); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x439e, 0x439f); A = mem_rd(gb, HL);
  CYC(0x439f, 0x43a1); alu_xor(gb, 0x08);
  CYC(0x43a1, 0x43a2); mem_wr(gb, HL, A);
  CYC(0x43a2, 0x43a4); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x43a4, 0x43a6); mem_wr(gb, HL, 0x01);
  CYC(0x43a6, 0x43a9); SET_HL(0x49bb); // mainScripts.spinnerScript_waitForLinkAfterDelay
  CYC(0x43a9, 0x43ac); interactionSetScript_hook(gb); return; // jp
}
