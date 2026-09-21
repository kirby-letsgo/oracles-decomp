#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodeb9_jump_table(GB *gb) {
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

static void interactionCodeb9_addAToHl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void interactionCodeb9_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:6cb2, interactionCodeb9@jump. Reached by fallthrough from @subid0Init, by a tail `jp` from
// @subid0/@subid1/@subid2 -- goto, same routine -- and by a genuine `call` from @subid3Init and
// @subid4Init, which both still have work to do afterward -- their call sites push their own
// return address and check the resulting pc/sp exactly like CALL_C would, since this plain C function
// itself ends with a bare tail-call into objectSetSpeedZ_hook with no push of its own.
static void interactionCodeb9_jump(GB *gb) {
  BASE(interactionCodeb9);
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+94); SET_HL(b_+167); // @speedZVals
  CYC(b_+94, b_+95); interactionCodeb9_addDoubleIndex(gb, b_+95);
  CYC(b_+95, b_+96); C = mem_rd(gb, HL);
  CYC(b_+96, b_+97); SET_HL(HL + 1); // inc hl
  CYC(b_+97, b_+98); B = mem_rd(gb, HL);
  CYC(b_+98, b_+101); TAIL(objectSetSpeedZ); // jp
}

// ==================================================================================================
// INTERAC_HORON_DOG_CREDITS
//
// Variables:
//   subid: Used as a sort of "state" variable?
//   var36: Target x-position
//   var37: ?
// ==================================================================================================
void interactionCodeb9_hook(GB *gb) {
  BASE(interactionCodeb9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodeb9_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+189) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CALL_C(b_+17, objectSetInvisible_hook, SYM(objectSetInvisible), b_+20);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); B = A;
  CYC(b_+24, b_+27); SET_HL(b_+143); // @counter1Vals
  CYC(b_+27, b_+28); interactionCodeb9_addAToHl(gb, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+33); A = B;
  CYC(b_+33, b_+36); SET_HL(b_+151); // @positions
  CYC(b_+36, b_+37); interactionCodeb9_addDoubleIndex(gb, b_+37);
  CYC(b_+37, b_+38); B = mem_rd(gb, HL);
  CYC(b_+38, b_+39); SET_HL(HL + 1); // inc hl
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); C = A;
  CYC(b_+41, b_+43); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CALL_C(b_+44, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+47);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+52, b_+54); A = 0x28; // SPEED_100
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+59); push_effect(gb, b_+59);
  do { uint16_t jt_ = (interactionCodeb9_jump_table(gb));
    if (jt_ == b_+75) { goto subid0Init; }
    else if (jt_ == b_+88) { interactionCodeb9_jump(gb); return; }
    else if (jt_ == b_+101) { goto subid3Init; }
    else if (jt_ == b_+112) { goto subid4Init; }
    else if (jt_ == b_+137) { goto subid7Init; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0Init:
  CYC(b_+75, b_+77); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+77, b_+79); A = 0x04;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+81); H = D;
  CYC(b_+81, b_+83); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0xe0);
  CYC(b_+85, b_+86); SET_HL(HL + 1); // inc hl
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x01); // [counter2]
  interactionCodeb9_jump(gb); return; // falls through into @jump

subid3Init:
  CYC(b_+101, b_+104); push_effect(gb, b_+104); interactionCodeb9_jump(gb);
  if (!(gb->pc == b_+104 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(b_+104, b_+106); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+106, b_+108); A = 0x3c; // SPEED_180
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+112); goto setZPosition; // jp

subid4Init: // also @subid5Init, @subid6Init -- aliased jump-table entry
  CYC(b_+112, b_+115); push_effect(gb, b_+115); interactionCodeb9_jump(gb);
  if (!(gb->pc == b_+115 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(b_+115, b_+117); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+117, b_+119); A = 0x0a; // SPEED_40
  CYC(b_+119, b_+120); mem_wr(gb, DE, A);
  // falls through into @setZPosition

setZPosition:
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+122, b_+123); A = mem_rd(gb, DE);
  CYC(b_+123, b_+125); alu_sub(gb, 0x03);
  CYC(b_+125, b_+128); SET_HL(b_+181); // @zPositions
  CYC(b_+128, b_+129); interactionCodeb9_addDoubleIndex(gb, b_+129);
  CYC(b_+129, b_+131); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+131, b_+132); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+132, b_+133); mem_wr(gb, DE, A);
  CYC(b_+133, b_+134); E = alu_dec8(gb, E);
  CYC(b_+134, b_+135); A = mem_rd(gb, HL);
  CYC(b_+135, b_+136); mem_wr(gb, DE, A);
  RET(b_+136); return;

subid7Init:
  CYC(b_+137, b_+140); SET_HL(GV((SYM(interactionCoded8__subid0Script) + 37), 0x7a81)); // mainScripts.horonDogCreditsScript
  CYC(b_+140, b_+143); TAIL(interactionSetScript); // jp

state1:
  CYC(b_+189, b_+191); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+191, b_+192); A = mem_rd(gb, DE);
  CYC(b_+192, b_+193); push_effect(gb, b_+193);
  do { uint16_t jt_ = (interactionCodeb9_jump_table(gb));
    if (jt_ == b_+199) { goto substate0; }
    else if (jt_ == b_+209) { goto substate1; }
    else if (jt_ == b_+290) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+199, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+202);
  if (!(F & FZ)) { CYCT(b_+202, b_+203); ret_effect(gb); return; } // ret nz
  CYC(b_+202, b_+203);
  CALL_C(b_+203, objectSetVisible_hook, SYM(objectSetVisible), b_+206);
  CYC(b_+206, b_+209); TAIL(interactionIncSubstate); // jp

substate1:
  CALL_C(b_+209, interactionAnimate_hook, SYM(interactionAnimate), b_+212);
  CALL_C(b_+212, objectApplySpeed_hook, SYM(objectApplySpeed), b_+215);
  CYC(b_+215, b_+216); H = D;
  CYC(b_+216, b_+218); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+218, b_+219); A = mem_rd(gb, HL);
  CYC(b_+219, b_+221); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+221, b_+222); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+222, b_+224); goto reachedTargetXPosition; } // jr nz
  CYC(b_+222, b_+224);
  CALL_C(b_+224, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+227);
  CYC(b_+227, b_+229); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+229, b_+231); mem_wr(gb, HL, 0x00);
  CYC(b_+231, b_+233); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+233, b_+234); A = mem_rd(gb, HL);
  CYC(b_+234, b_+235); alu_add(gb, A);
  CYC(b_+235, b_+236); A = alu_inc8(gb, A);
  CYC(b_+236, b_+239); TAIL(interactionSetAnimation); // jp

reachedTargetXPosition:
  CYC(b_+239, b_+241); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+241, b_+242); A = mem_rd(gb, DE);
  CYC(b_+242, b_+243); push_effect(gb, b_+243);
  do { uint16_t jt_ = (interactionCodeb9_jump_table(gb));
    if (jt_ == b_+259) { goto subid0_1_2; }
    else if (jt_ == b_+270) { goto subid3; }
    else if (jt_ == b_+286) { goto subid4_5_6; }
    else if (jt_ == b_+285) { goto subid7; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0_1_2:
  CYC(b_+259, b_+261); C = 0x20;
  CALL_C(b_+261, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+264);
  if (!(F & FZ)) { CYCT(b_+264, b_+265); ret_effect(gb); return; } // ret nz
  CYC(b_+264, b_+265);
  CYC(b_+265, b_+267); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+267, b_+270); interactionCodeb9_jump(gb); return; // jp

subid3:
  CYC(b_+270, b_+272); C = 0x10;
  // falls through into @label_0b_293

label_0b_293:
  CYC(b_+272, b_+274); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+274, b_+275); A = mem_rd(gb, DE);
  CYC(b_+275, b_+276); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+276, b_+277); ret_effect(gb); return; } // ret nz
  CYC(b_+276, b_+277);
  CALL_C(b_+277, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+280);
  if (!(F & FZ)) { CYCT(b_+280, b_+281); ret_effect(gb); return; } // ret nz
  CYC(b_+280, b_+281);
  CYC(b_+281, b_+282); H = D;
  CYC(b_+282, b_+284); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+284, b_+285); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  // falls through into @subid7

subid7:
  RET(b_+285); return;

subid4_5_6:
  CYC(b_+286, b_+288); C = 0x01;
  CYCT(b_+288, b_+290); goto label_0b_293; // jr

substate2:
  CYC(b_+290, b_+292); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+292, b_+293); A = mem_rd(gb, DE);
  CYC(b_+293, b_+294); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+294, b_+296); goto substate2_subidNot0; } // jr nz
  CYC(b_+294, b_+296);

  // @substate2_subid0
  CYC(b_+296, b_+297); B = A;
  CYC(b_+297, b_+298); H = D;
  CYC(b_+298, b_+300); L = INTERACTION_BASE + OBJ_COUNTER1;
  CALL_C(b_+300, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+303);
  if (!(F & FZ)) { CYCT(b_+303, b_+305); goto animate; } // jr nz
  CYC(b_+303, b_+305);
  CYC(b_+305, b_+308); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+308, b_+310); mem_wr(gb, HL, 0x01);
  RET(b_+310); return;

substate2_subidNot0:
  CYC(b_+311, b_+313); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+313, b_+315); goto animate; } // jr nz
  CYC(b_+313, b_+315);
  CALL_C(b_+315, interactionRunScript_hook, SYM(interactionRunScript), b_+318);
  CYC(b_+318, b_+320); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+320, b_+321); A = mem_rd(gb, DE);
  CYC(b_+321, b_+322); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+322, b_+323); ret_effect(gb); return; } // ret z
  CYC(b_+322, b_+323);
  // falls through into @animate

animate:
  CYC(b_+323, b_+326); TAIL(interactionAnimate); // jp
}
