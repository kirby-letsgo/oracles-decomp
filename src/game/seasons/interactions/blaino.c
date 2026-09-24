#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/blaino.s.
// INTERAC_BLAINO
// var37 - 0 if enough rupees, else 1
// var38 - RUPEEVAL_10 if cheated, otherwise RUPEEVAL_20
// var39 - pointer to Blaino / script ???
// $ccec - result of fight - $01 if won, $02 if lost, $03 if cheated
// $cced - $00 on init, $01 when starting fight, $03 when fight done

static uint16_t blaino_jump_table(GB *gb) {
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

// INTERAC_BLAINO
// var37 - 0 if enough rupees, else 1
// var38 - RUPEEVAL_10 if cheated, otherwise RUPEEVAL_20
// var39 - pointer to Blaino / script ???
// $ccec - result of fight - $01 if won, $02 if lost, $03 if cheated
// $cced - $00 on init, $01 when starting fight, $03 when fight done
void s_interactionCode72_hook(GB *gb) {
  BASE(interactionCode72);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); TAIL_S(blainoSubid01); }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+16) goto state0;
    if (jt_ == b_+69) goto state1;
    if (jt_ == SYM(interactionDelete) && hook_is(gb, SYM(interactionDelete), s_interactionDelete)) { s_interactionDelete(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+16, s_interactionIncState, SYM(interactionIncState), b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wShootingGalleryBallStatus);
  CYC(b_+22, b_+24); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+24, b_+26); goto L_6fae; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+28, b_+30); goto L_6fc3; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+32, b_+34); goto L_6fae; }
  CYC(b_+32, b_+34);
L_6fae:
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x01);
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x02);
  CYC(b_+42, b_+44); A = 0x06;
  CALL_C(b_+44, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+47);
  CALL_C(b_+47, s_seasonsFunc_09_7055_hook, SYM(seasonsFunc_09_7055), b_+50);
  CALL_C(b_+50, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+53);
  CYC(b_+53, b_+55);
  goto animate;
L_6fc3:
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x00);
  CALL_C(b_+59, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+62);
  CYC(b_+62, b_+64); A = 0x01;
  CYC(b_+64, b_+67);
  TAIL(interactionSetAnimation);

state1:
  CYC(b_+69, b_+71); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+73, b_+75); goto L_6fe0; }
  CYC(b_+73, b_+75);
  CALL_C(b_+75, s_seasonsFunc_09_7036_hook, SYM(seasonsFunc_09_7036), b_+78);
  CALL_C(b_+78, s_seasonsFunc_09_704f_hook, SYM(seasonsFunc_09_704f), b_+81);
animate:
  CALL_C(b_+81, s_interactionAnimate, SYM(interactionAnimate), b_+84);
L_6fe0:
  CALL_C(b_+84, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+87);
  CYC(b_+87, b_+90);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

void s_blainoSubid01_hook(GB *gb) {
  BASE(blainoSubid01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+22) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); A = 0x04;
  CALL_C(b_+16, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+19);
  CYC(b_+19, b_+22);
  TAIL(objectSetVisiblec1);
state1:
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, b_+26);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+32) goto substate0;
    if (jt_ == b_+54) goto substate1;
    if (jt_ == b_+67) goto substate2;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+32, b_+35); A = mem_rd(gb, wTmpcbb5);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+36, b_+38); goto substate2; }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); H = D;
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+41, b_+42); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x01);
  CYC(b_+46, b_+47); alu_xor(gb, A);
  CYC(b_+47, b_+49); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A);
  CYC(b_+51, b_+54);
  TAIL(interactionAnimate);
substate1:
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+59); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+59, b_+61); goto L_7026; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_7026:
  CYC(b_+64, b_+67);
  TAIL(interactionAnimate);
substate2:
  CYC(b_+67, b_+69); C = 0x20;
  CALL_C(b_+69, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+72);
  if (!(F & FZ)) { RET_TAKEN(b_+72); return; }
  CYC(b_+72, b_+73);
  CYC(b_+73, b_+74); H = D;
  CYC(b_+74, b_+77); SET_BC(IO_LCDC);
  CYC(b_+77, b_+80);
  TAIL(objectSetSpeedZ);
}

void s_seasonsFunc_09_7036_hook(GB *gb) {
  BASE(seasonsFunc_09_7036);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+9);
  CYC(b_+9, b_+11); alu_add(gb, 0x04);
  CYC(b_+11, b_+13); alu_and(gb, 0x18);
  CYC(b_+13, b_+15); A = alu_swap(gb, A);
  CYC(b_+15, b_+16); alu_rlca(gb);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+19, b_+20); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+25);
  TAIL(interactionSetAnimation);
}

void s_seasonsFunc_09_704f_hook(GB *gb) {
  BASE(seasonsFunc_09_704f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x0e;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  s_seasonsFunc_09_7055_hook(gb); return; // falls through
}

void s_seasonsFunc_09_7055_hook(GB *gb) {
  BASE(seasonsFunc_09_7055);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+2, b_+4); A = 0x80;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); E = alu_inc8(gb, E);
  CYC(b_+6, b_+8); A = 0xff;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  RET(b_+9); return;
}

