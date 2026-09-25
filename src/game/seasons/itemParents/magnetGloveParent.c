#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/itemParents/magnetGloveParent.s.
// Variables:
// var37: ?

static uint16_t magnetGloveParent_jump_table(GB *gb) {
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

static void magnetGloveParent_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// parentItemCode_magnetGloves@searchForTile
static void magnetGloveParent_searchForTile(GB *gb) {
  BASE(parentItemCode_magnetGloves);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+167, b_+170); A = mem_rd(gb, w1Link_yh);
  CYC(b_+170, b_+172); mem_wr(gb, hFF8F, A);
  CYC(b_+172, b_+173); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+173, b_+174); B = A;
  CYC(b_+174, b_+175); SET_HL(HL + 1);
  CYC(b_+175, b_+178); A = mem_rd(gb, w1Link_xh);
  CYC(b_+178, b_+180); mem_wr(gb, hFF8E, A);
  CYC(b_+180, b_+181); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+181, b_+182); C = A;
  CYC(b_+182, b_+183); SET_HL(HL + 1);
  CYC(b_+183, b_+184); push_effect(gb, HL);
  CALL_C(b_+184, s_getTileAtPosition, SYM(getTileAtPosition), b_+187);
  CYC(b_+187, b_+188); C = L;
  CYC(b_+188, b_+189); B = H;
  CYC(b_+189, b_+190); SET_HL(pop_effect(gb));
checkNextTile:
  CYC(b_+190, b_+191); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+191, b_+193); goto ret; }
  CYC(b_+191, b_+193);
  CYC(b_+193, b_+194); alu_cp(gb, D);
  if (F & FZ) { CYCT(b_+194, b_+196); goto foundTile; }
  CYC(b_+194, b_+196);
  CYC(b_+196, b_+197); A = C;
  CYC(b_+197, b_+198); alu_add(gb, E);
  CYC(b_+198, b_+199); C = A;
  CYC(b_+199, b_+200); A = mem_rd(gb, BC);
  CYC(b_+200, b_+202);
  goto checkNextTile;
foundTile:
  CYC(b_+202, b_+203); A = C;
  CYC(b_+203, b_+205); alu_and(gb, 0xf0);
  CYC(b_+205, b_+207); alu_or(gb, 0x08);
  CYC(b_+207, b_+208); B = A;
  CYC(b_+208, b_+209); A = C;
  CYC(b_+209, b_+211); A = alu_swap(gb, A);
  CYC(b_+211, b_+213); alu_and(gb, 0xf0);
  CYC(b_+213, b_+215); alu_or(gb, 0x08);
  CYC(b_+215, b_+216); C = A;
ret:
  RET(b_+216); return;
}

// parentItemCode_magnetGloves@checkLatchedOntoTile
static void magnetGloveParent_checkLatchedOntoTile(GB *gb) {
  BASE(parentItemCode_magnetGloves);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+126, b_+129); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+129, b_+131); alu_xor(gb, 0x01);
  CYC(b_+131, b_+133); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+133); return; }
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+137); A = mem_rd(gb, wActiveGroup);
  CYC(b_+137, b_+140); SET_HL(SYM(magnetTilesTable));
  CYC(b_+140, b_+141); magnetGloveParent_add_a_to_hl(gb, b_+141);
  CYC(b_+141, b_+142); A = mem_rd(gb, HL);
  CYC(b_+142, b_+143); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+143); return; }
  CYC(b_+143, b_+144);
  CYC(b_+144, b_+145); push_effect(gb, DE);
  CYC(b_+145, b_+146); D = A;
  CYC(b_+146, b_+149); A = mem_rd(gb, w1Link_direction);
  CYC(b_+149, b_+150); E = A;
  CYC(b_+150, b_+151); alu_add(gb, A);
  CYC(b_+151, b_+152); alu_add(gb, A);
  CYC(b_+152, b_+153); alu_add(gb, E);
  CYC(b_+153, b_+156); SET_HL(b_+217 /* @offsetsToCheck */);
  CYC(b_+156, b_+157); magnetGloveParent_add_a_to_hl(gb, b_+157);
  CYC(b_+157, b_+158); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+158, b_+159); E = A;
  CALL_L(b_+159, magnetGloveParent_searchForTile, b_+162);
  if (F & FZ) CALL_L_CC(b_+162, magnetGloveParent_searchForTile, b_+165);
  else CYC(b_+162, b_+165);
  CYC(b_+165, b_+166); SET_DE(pop_effect(gb));
  RET(b_+166); return;
}

// Variables:
// var37: ?
void s_parentItemCode_magnetGloves_hook(GB *gb) {
  BASE(parentItemCode_magnetGloves);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkClimbingVine);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto deleteSelf; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = 0x04;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (magnetGloveParent_jump_table(gb));
    if (jt_ == b_+14) goto state0;
    if (jt_ == b_+42) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+14, b_+17); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto deleteSelf; }
  CYC(b_+18, b_+20);
  CALL_C(b_+20, s_itemIncState, SYM(itemIncState), b_+23);
  CYC(b_+23, b_+25); L = 0x37;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xff);
  CYC(b_+27, b_+29); L = 0x18;
  CYC(b_+29, b_+30); alu_xor(gb, A);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0xd6);
  CALL_C(b_+33, s_itemCreateChild, SYM(itemCreateChild), b_+36);
  CALL_C(b_+36, s_updateLinkDirectionFromAngle, SYM(updateLinkDirectionFromAngle), b_+39);
  CALL_C(b_+39, s_setStatusBarNeedsRefreshBit1, SYM(setStatusBarNeedsRefreshBit1), b_+42);
state1:
  CYC(b_+42, b_+45); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto invertPolarityAndStop; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+51); A = mem_rd(gb, wLinkInAir);
  CYC(b_+51, b_+52); alu_rlca(gb);
  if (F & FC) { CYCT(b_+52, b_+54); goto invertPolarityAndStop; }
  CYC(b_+52, b_+54);
  CALL_C(b_+54, s_parentItemCheckButtonPressed, SYM(parentItemCheckButtonPressed), b_+57);
  if (F & FZ) { CYCT(b_+57, b_+59); goto invertPolarityAndStop; }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); A = 0xaf;
  CALL_C(b_+61, s_playSound, SYM(playSound_b00), b_+64);
  CYC(b_+64, b_+67); A = mem_rd(gb, wMagnetGlovePolarity);
  CYC(b_+67, b_+68); alu_scf(gb);
  CYC(b_+68, b_+69); alu_adc(gb, A);
  CYC(b_+69, b_+72); mem_wr(gb, wMagnetGloveState, A);
  CALL_C(b_+72, s_itemDisableLinkTurning, SYM(itemDisableLinkTurning), b_+75);
  CALL_L(b_+75, magnetGloveParent_checkLatchedOntoTile, b_+78);
  if (F & FZ) { RET_TAKEN(b_+78); return; }
  CYC(b_+78, b_+79);
  CALL_C(b_+79, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+82);
  CYC(b_+82, b_+85); SET_HL(wMagnetGloveState);
  CYC(b_+85, b_+87); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+87, b_+89); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+89, b_+91); goto L_522a; }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93); alu_xor(gb, 0x10);
L_522a:
  CYC(b_+93, b_+95); E = 0x09;
  CYC(b_+95, b_+96); mem_wr(gb, DE, A);
  CYC(b_+96, b_+97); C = A;
  CYC(b_+97, b_+99); A = 0xff;
  CYC(b_+99, b_+102); mem_wr(gb, w1Link_angle, A);
  CYC(b_+102, b_+104); B = 0x3c;
  CYC(b_+104, b_+107);
  TAIL(updateLinkPositionGivenVelocity);
invertPolarityAndStop:
  CYC(b_+107, b_+110); SET_HL(wMagnetGlovePolarity);
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CYC(b_+111, b_+113); alu_xor(gb, 0x01);
  CYC(b_+113, b_+114); mem_wr(gb, HL, A);
  CYC(b_+114, b_+117); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+117, b_+119); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
deleteSelf:
  CYC(b_+119, b_+120); alu_xor(gb, A);
  CYC(b_+120, b_+123); mem_wr(gb, wMagnetGloveState, A);
  CYC(b_+123, b_+126);
  TAIL(clearParentItem);
}
