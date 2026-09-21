#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCoded0_jump_table(GB *gb) {
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

// rst_addAToHl (rst $10): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCoded0_addAToHl(GB *gb, uint16_t return_address) {
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

// rst_addDoubleIndex (rst $18): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCoded0_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCoded0@cpYToCompanion (0b:4bee): true local, called (real `call`) twice from
// @setFlagAndDeleteWhenCompanionIsAbove and @setFlagAndDeleteWhenCompanionIsBelowOrRight.
static void interactionCoded0_cpYToCompanion(GB *gb) {
  BASE(interactionCoded0);
  CYC(b_+186, b_+188); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+188, b_+189); A = mem_rd(gb, DE);
  CYC(b_+189, b_+192); SET_HL(w1Companion_yh);
  CYC(b_+192, b_+193); alu_cp(gb, mem_rd(gb, HL));
  RET(b_+193); return;
}

// interactionCoded0@checkLinkInXRange (0b:4bf6): true local, called (real `call`) once from
// @setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange.
// @param[out] zflag z if Link is within a certain range of X-positions for certain rooms?
static void interactionCoded0_checkLinkInXRange(GB *gb) {
  BASE(interactionCoded0);
  CYC(b_+194, b_+197); A = mem_rd(gb, wActiveRoom);
  CYC(b_+197, b_+200); SET_HL(b_+228); // @rooms
  CYC(b_+200, b_+202); B = 0x00;

roomLoop:
  CYC(b_+202, b_+203); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+203, b_+205); goto foundRoom; } // jr z
  CYC(b_+203, b_+205);
  CYC(b_+205, b_+206); B = alu_inc8(gb, B);
  CYC(b_+206, b_+207); SET_HL(HL + 1);
  CYC(b_+207, b_+209); goto roomLoop; // jr $4bfe

foundRoom:
  CYC(b_+209, b_+210); A = B;
  CYC(b_+210, b_+213); SET_HL(b_+231); // @xRanges
  CYC(b_+213, b_+214); interactionCoded0_addDoubleIndex(gb, b_+214);
  CYC(b_+214, b_+217); A = mem_rd(gb, w1Link_xh);
  CYC(b_+217, b_+218); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+218, b_+220); goto notInRange; } // jr c
  CYC(b_+218, b_+220);
  CYC(b_+220, b_+221); SET_HL(HL + 1);
  CYC(b_+221, b_+222); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+222, b_+224); goto notInRange; } // jr nc
  CYC(b_+222, b_+224);
  CYC(b_+224, b_+225); alu_xor(gb, A);
  RET(b_+225); return;

notInRange:
  CYC(b_+226, b_+227); alu_or(gb, D);
  RET(b_+227); return;
}

// ==================================================================================================
// INTERAC_COMPANION_TUTORIAL
//
// @state2's inner jump table has 6 entries for the ages build; two of them (index 2 and 5) both
// target @setFlagAndDeleteWhenCompanionIsBelow, which collapses to a single switch case here.
// @setFlagAndDeleteWhenCompanionIsAboveAndVar38NonZero is dead in the ages build (no jump-table
// entry reaches it) but its bytes are still present in ROM, falling straight through into
// @setFlagAndDeleteWhenCompanionIsAbove; ported below for completeness even though unreachable.
// ==================================================================================================
void interactionCoded0_hook(GB *gb) {
  BASE(interactionCoded0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCoded0_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+14) { goto state1; }
    else if (jt_ == b_+99) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;

state1:
  CYC(b_+14, b_+16); A = 0x02;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+20); A = mem_rd(gb, w1Companion_enabled);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+21, b_+23); goto deleteIfSubid2Or5; } // jr z
  CYC(b_+21, b_+23);

  // Verify that the correct companion is on-screen, otherwise delete self
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+28); A = alu_srl(gb, A);
  CYC(b_+28, b_+30); alu_add(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(b_+30, b_+32); alu_cp(gb, 0x0e); // SPECIALOBJECT_LAST_COMPANION+1
  if (F & FC) { CYCT(b_+32, b_+34); goto afterMoosh; } // jr c
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); A = 0x0d; // SPECIALOBJECT_MOOSH

afterMoosh:
  CYC(b_+36, b_+39); SET_HL(w1Companion_id);
  CYC(b_+39, b_+40); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto delete; } // jr nz

  // Delete self if tutorial text was already shown
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+46); SET_HL(b_+249); // @flagNumbers
  CYC(b_+46, b_+47); interactionCoded0_addAToHl(gb, b_+47);
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+51); SET_HL(wCompanionTutorialTextShown);
  CALL_C(b_+51, checkFlag_hook, SYM(checkFlag), b_+54);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto delete; } // jr nz
  CYC(b_+54, b_+56);

  // Check whether to dismount? (subid 2 only)
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+59); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto afterDismountCheck; } // jr nz
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+64); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+64, b_+65); alu_rra(gb);
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  if (!(F & FC)) { CYCT(b_+66, b_+68); goto afterDismountCheck; } // jr nc
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+71); mem_wr(gb, wForceCompanionDismount, A);

afterDismountCheck:
  CYC(b_+71, b_+74); SET_HL(b_+237); // @tutorialTextToShow
  CYC(b_+74, b_+75); interactionCoded0_addDoubleIndex(gb, b_+75);
  CYC(b_+75, b_+76); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+76, b_+77); C = A;
  CYC(b_+77, b_+78); B = mem_rd(gb, HL);
  CYC(b_+78, b_+81); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+81, b_+83); alu_bit(gb, 0, A);
  if (!(F & FZ)) CALL_C_CC(b_+83, showText_hook, SYM(showText), b_+86); else CYC(b_+83, b_+86); // call nz

deleteIfSubid2Or5:
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+91); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+91, b_+93); goto delete; } // jr z
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+95); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+95, b_+96); ret_effect(gb); return; } // ret nz
  CYC(b_+95, b_+96);

delete:
  CYC(b_+96, b_+99); TAIL(interactionDelete); // jp

state2:
  CYC(b_+99, b_+102); A = mem_rd(gb, w1Companion_enabled);
  CYC(b_+102, b_+103); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+103, b_+104); ret_effect(gb); return; } // ret z
  CYC(b_+103, b_+104);
  CYC(b_+104, b_+106); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); push_effect(gb, b_+108);
  do { uint16_t jt_ = (interactionCoded0_jump_table(gb));
    if (jt_ == b_+171) { goto setFlagAndDeleteWhenCompanionIsBelowOrRight; }
    else if (jt_ == b_+135) { goto setFlagAndDeleteWhenCompanionIsAbove; }
    else if (jt_ == b_+120) { goto setFlagAndDeleteWhenCompanionIsBelow; }
    else if (jt_ == b_+155) { goto setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange; }
    else if (jt_ == b_+161) { goto setFlagAndDeleteWhenCompanionIsLeft; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

setFlagAndDeleteWhenCompanionIsBelow:
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+122, b_+123); A = mem_rd(gb, DE);
  CYC(b_+123, b_+126); SET_HL(w1Companion_yh);
  CYC(b_+126, b_+127); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+127, b_+128); ret_effect(gb); return; } // ret nc
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+130); goto setFlagAndDelete; // jr $4bbf

  // setFlagAndDeleteWhenCompanionIsAboveAndVar38NonZero (0b:4bb6): dead in ages, falls through.
  CYC(b_+130, b_+133); A = mem_rd(gb, w1Companion_var38);
  CYC(b_+133, b_+134); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+134, b_+135); ret_effect(gb); return; } // ret z
  CYC(b_+134, b_+135);

setFlagAndDeleteWhenCompanionIsAbove:
  CALL_C(b_+135, interactionCoded0_cpYToCompanion, b_+186, b_+138);
  if (F & FC) { CYCT(b_+138, b_+139); ret_effect(gb); return; } // ret c
  CYC(b_+138, b_+139);

setFlagAndDelete:
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+145); SET_HL(b_+249); // @flagNumbers
  CYC(b_+145, b_+146); interactionCoded0_addAToHl(gb, b_+146);
  CYC(b_+146, b_+147); A = mem_rd(gb, HL);
  CYC(b_+147, b_+150); SET_HL(wCompanionTutorialTextShown);
  CALL_C(b_+150, setFlag_hook, SYM(setFlag), b_+153);

  // goToDelete (falls through, never a goto target)
  CYC(b_+153, b_+155); goto delete; // jr $4b94

setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange:
  CALL_C(b_+155, interactionCoded0_checkLinkInXRange, b_+194, b_+158);
  if (!(F & FZ)) { CYCT(b_+158, b_+159); ret_effect(gb); return; } // ret nz
  CYC(b_+158, b_+159);
  CYC(b_+159, b_+161); goto setFlagAndDeleteWhenCompanionIsAbove; // jr $4bbb

setFlagAndDeleteWhenCompanionIsLeft:
  CYC(b_+161, b_+163); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+163, b_+164); A = mem_rd(gb, DE);
  CYC(b_+164, b_+167); SET_HL(w1Companion_xh);
  CYC(b_+167, b_+168); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+168, b_+169); ret_effect(gb); return; } // ret nc
  CYC(b_+168, b_+169);
  CYC(b_+169, b_+171); goto setFlagAndDelete; // jr $4bbf

setFlagAndDeleteWhenCompanionIsBelowOrRight:
  CALL_C(b_+171, interactionCoded0_cpYToCompanion, b_+186, b_+174);
  if (F & FC) { CYCT(b_+174, b_+176); goto setFlagAndDelete; } // jr c
  CYC(b_+174, b_+176);
  CYC(b_+176, b_+178); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+178, b_+179); A = mem_rd(gb, DE);
  CYC(b_+179, b_+182); SET_HL(w1Companion_xh);
  CYC(b_+182, b_+183); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+183, b_+184); ret_effect(gb); return; } // ret c
  CYC(b_+183, b_+184);
  CYC(b_+184, b_+186); goto setFlagAndDelete; // jr $4bbf
}
