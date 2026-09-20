#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(spinner_setLinkRelativePosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0xd0; // >w1Link
  CYC(b_+2, b_+4); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+4, b_+6); C = 0x0b; // <w1Link.yh
  CYC(b_+6, b_+9); push_effect(gb, b_+9); goto func;
afterFunc1:
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+11, b_+13); C = 0x0d; // <w1Link.xh

// spinner_setLinkRelativePosition@func: called once via the genuine call above, resuming
// at 0x4409 and staying within this function, then fallen into a second time with no call
// at all -- that ret pops whatever this function's own caller pushed instead.
func:
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+16); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, BC, A);
  RET(b_+17);
  if (gb->pc == b_+9 && gb->sp == sp0_) goto afterFunc1;
  return;
}

void spinner_updateLinkPosition_hook(GB *gb) {
  BASE(spinner_updateLinkPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); A = 0x70; // SND_DOORCLOSE
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_add(gb, B);
  CYC(b_+19, b_+21); alu_and(gb, 0x0f);
  CYC(b_+21, b_+24); SET_HL(SYM(spinner_linkRelativePositions)); // spinner_linkRelativePositions
  CYC(b_+24, b_+25); interactionCode7d_addDoubleIndexToHl_from_rst(gb, SYM(spinner_setLinkRelativePosition));
  spinner_setLinkRelativePosition_hook(gb); return; // falls through
}

void spinner_subid02_hook(GB *gb) {
  BASE(spinner_subid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode7d_jump_table(gb);
    if (target == b_+8) goto state0;
    goto state1;
  }

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+20, b_+21); E = L;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, objectSetVisible82_hook, SYM(objectSetVisible82), b_+26);

state1:
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); H = A;
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+32, b_+33); E = L;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+35, b_+37); goto afterAnim; } // jr z
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+41); A = alu_swap(gb, A);
  CYC(b_+41, b_+42); alu_rlca(gb);
  CYC(b_+42, b_+44); alu_add(gb, 0x02);
  CALL_C(b_+44, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+47);

afterAnim:
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); H = A;
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+53, b_+54); E = L;
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CYC(b_+56, b_+59); interactionAnimate_hook(gb); return; // jp
}

// INTERAC_SPINNER
void interactionCode7d_hook(GB *gb) {
  BASE(interactionCode7d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode7d_jump_table(gb);
    if (target == SYM(spinner_subid02)) { spinner_subid02_hook(gb); return; }
    goto subid00;
  }

subid00:
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  {
    CYC(b_+13, b_+14); push_effect(gb, b_+14);
    uint16_t target = interactionCode7d_jump_table(gb);
    if (target == b_+24) goto state0;
    if (target == SYM(interactionRunScript)) { interactionRunScript_hook(gb); return; }
    if (target == b_+81) goto state2;
    if (target == b_+175) goto state3;
    goto state4;
  }

state0:
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+37); A = W8(wSpinnerState);
  CYC(b_+37, b_+38); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+38, b_+40); A = 0x01;
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto afterSubidCalc; } // jr nz
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = alu_dec8(gb, A);

afterSubidCalc:
  CYC(b_+43, b_+45); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+48); A = alu_swap(gb, A);
  CYC(b_+48, b_+49); alu_rrca(gb);
  CYC(b_+49, b_+51); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
  CYC(b_+52, b_+54); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  CALL_C(b_+55, setShortPosition_hook, SYM(setShortPosition), b_+58);
  CALL_C(b_+58, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+61);
  CYC(b_+61, b_+64); SET_HL((SYM(interactionCode91__subid00__state1) + 15)); // mainScripts.spinnerScript_initialization
  CALL_C(b_+64, interactionSetScript_hook, SYM(interactionSetScript), b_+67);
  CALL_C(b_+67, objectSetVisible82_hook, SYM(objectSetVisible82), b_+70);
  CYC(b_+70, b_+73); SET_BC((SYM(interactionCode95__subid2) + 23)); // INTERAC_SPINNER, $02
  CALL_C(b_+73, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+76);
  if (!(F & FZ)) { RET_TAKEN(b_+76); return; } // ret nz
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+79, b_+80); mem_wr(gb, HL, D);
  RET(b_+80); return; // ret

state2:
  CYC(b_+81, b_+84); SET_HL(wcc95);
  CYC(b_+84, b_+87); A = W8(wLinkInAir);
  CYC(b_+87, b_+88); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+88, b_+90); goto revertToState1; } // jr nz
  CYC(b_+88, b_+90);
  CYC(b_+90, b_+92); alu_bit(gb, 4, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+92, b_+94); goto beginTurning; } // jr nz
  CYC(b_+92, b_+94);

revertToState1:
  CYC(b_+94, b_+96); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 7));
  CYC(b_+96, b_+98); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+98, b_+100); A = 0x01;
  CYC(b_+100, b_+101); mem_wr(gb, DE, A);
  CYC(b_+101, b_+104); SET_HL((SYM(interactionCode91__subid00__state1) + 19)); // mainScripts.spinnerScript_waitForLink
  CYC(b_+104, b_+107); interactionSetScript_hook(gb); return; // jp

beginTurning:
  CYC(b_+107, b_+109); A = 0x03;
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  CALL_C(b_+110, clearAllParentItems_hook, SYM(clearAllParentItems), b_+113);
  CYC(b_+113, b_+115); C = 0x28;
  CALL_C(b_+115, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+118);
  CYC(b_+118, b_+120); A = alu_sra(gb, A);
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  CYC(b_+123, b_+124); B = A;
  CYC(b_+124, b_+125); E = alu_inc8(gb, E);
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+127); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+127, b_+129); goto clockwise; } // jr nz
  CYC(b_+127, b_+129);

  // interactionCode7d@counterClockwise
  CYC(b_+129, b_+130); A = B;
  CYC(b_+130, b_+131); alu_add(gb, A);
  CYC(b_+131, b_+134); SET_HL(SYM(spinner_counterClockwiseData)); // spinner_counterClockwiseData
  CYC(b_+134, b_+135); interactionCode7d_addDoubleIndexToHl_from_rst(gb, b_+135);
  CYC(b_+135, b_+137); goto afterDirectionTable; // jr

clockwise:
  CYC(b_+137, b_+138); A = B;
  CYC(b_+138, b_+139); alu_add(gb, A);
  CYC(b_+139, b_+142); SET_HL(SYM(spinner_clockwiseData)); // spinner_clockwiseData
  CYC(b_+142, b_+143); interactionCode7d_addDoubleIndexToHl_from_rst(gb, b_+143);

afterDirectionTable:
  CALL_C(b_+143, spinner_setLinkRelativePosition_hook, SYM(spinner_setLinkRelativePosition), b_+146);
  CYC(b_+146, b_+147); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+147, b_+149); C = 0x08; // <w1Link.direction
  CYC(b_+149, b_+150); mem_wr(gb, BC, A);
  CYC(b_+150, b_+152); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+152, b_+153); A = mem_rd(gb, HL);
  CYC(b_+153, b_+154); mem_wr(gb, DE, A);
  CALL_C(b_+154, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+157);
  CYC(b_+157, b_+160); A = W8(wDisabledObjects);
  CYC(b_+160, b_+162); alu_or(gb, 0x80);
  CYC(b_+162, b_+165); W8(wDisabledObjects) = A;
  CYC(b_+165, b_+167); A = 0x04;
  CALL_C(b_+167, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+170);
  CYC(b_+170, b_+172); A = 0x6c; // SND_OPENCHEST
  CYC(b_+172, b_+175); playSound_b00_hook(gb); return; // jp

state3:
  CALL_C(b_+175, spinner_updateLinkPosition_hook, SYM(spinner_updateLinkPosition), b_+178);
  CYC(b_+178, b_+180); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+180, b_+181); A = mem_rd(gb, DE);
  CYC(b_+181, b_+182); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+182, b_+185); interactionAnimate_hook(gb); return; } // jp nz
  CYC(b_+182, b_+185);
  CYC(b_+185, b_+186); H = D;
  CYC(b_+186, b_+188); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+188, b_+190); mem_wr(gb, HL, 0x04);
  CYC(b_+190, b_+192); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+192, b_+194); mem_wr(gb, HL, 0x10);
  CYC(b_+194, b_+195); alu_xor(gb, A);
  CYC(b_+195, b_+198); W8(wDisabledObjects) = A;
  CYC(b_+198, b_+201); SET_HL(w1Link_direction);
  CYC(b_+201, b_+202); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+202, b_+204); A = alu_swap(gb, A);
  CYC(b_+204, b_+205); alu_rrca(gb);
  CYC(b_+205, b_+206); mem_wr(gb, HL, A);
  CYC(b_+206, b_+209); SET_HL(wLinkForceState);
  CYC(b_+209, b_+211); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(b_+211, b_+212); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+212, b_+213); L = alu_inc8(gb, L);
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0x10);
  CYC(b_+215, b_+218); SET_HL(wcc95);
  CYC(b_+218, b_+220); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 7));
  RET(b_+220); return; // ret

state4:
  CALL_C(b_+221, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+224);
  if (!(F & FZ)) { RET_TAKEN(b_+224); return; } // ret nz
  CYC(b_+224, b_+225);
  CYC(b_+225, b_+227); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+227, b_+230); A = W8(wSpinnerState);
  CYC(b_+230, b_+231); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+231, b_+234); W8(wSpinnerState) = A;
  CYC(b_+234, b_+236); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+236, b_+237); A = mem_rd(gb, HL);
  CYC(b_+237, b_+239); alu_xor(gb, 0x01);
  CYC(b_+239, b_+240); mem_wr(gb, HL, A);
  CYC(b_+240, b_+242); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+242, b_+243); A = mem_rd(gb, HL);
  CYC(b_+243, b_+245); alu_xor(gb, 0x08);
  CYC(b_+245, b_+246); mem_wr(gb, HL, A);
  CYC(b_+246, b_+248); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+248, b_+250); mem_wr(gb, HL, 0x01);
  CYC(b_+250, b_+253); SET_HL((SYM(interactionCode91__subid00__state1) + 18)); // mainScripts.spinnerScript_waitForLinkAfterDelay
  CYC(b_+253, b_+256); interactionSetScript_hook(gb); return; // jp
}
