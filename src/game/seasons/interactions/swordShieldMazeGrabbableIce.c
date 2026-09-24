#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/swordShieldMazeGrabbableIce.s.
// INTERAC_D8_GRABBABLE_ICE

static uint16_t swordShieldMazeGrabbableIce_jump_table(GB *gb) {
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

// INTERAC_D8_GRABBABLE_ICE
void s_interactionCode68_hook(GB *gb) {
  BASE(interactionCode68);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (swordShieldMazeGrabbableIce_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+26) goto state1;
    if (jt_ == b_+86) goto state2;
    if (jt_ == b_+144) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+18);
  CYC(b_+18, b_+20); A = 0x06;
  CALL_C(b_+20, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+23);
  CYC(b_+23, b_+26);
  TAIL(objectSetVisiblec2);
state1:
  CYC(b_+26, b_+28); C = 0x20;
  CALL_C(b_+28, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wLinkInAir);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto L_5830; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+40); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+40, b_+41); alu_rrca(gb);
  if (!(F & FC)) CALL_C_CC(b_+41, s_objectPushLinkAwayOnCollision, SYM(objectPushLinkAwayOnCollision), b_+44);
  else CYC(b_+41, b_+44);
L_5830:
  CALL_C(b_+44, s_objectAddToGrabbableObjectBuffer, SYM(objectAddToGrabbableObjectBuffer), b_+47);
func_5833:
  CALL_C(b_+47, s_objectCheckIsOnHazard, SYM(objectCheckIsOnHazard), b_+50);
  if (!(F & FC)) { RET_TAKEN(b_+50); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+53); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+53, b_+55); goto L_5842; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+56); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+56, b_+59); TAIL(objectReplaceWithSplash); }
  CYC(b_+56, b_+59);
  CYC(b_+59, b_+62);
  TAIL(objectReplaceWithFallingDownHoleInteraction);
L_5842:
  CALL_C(b_+62, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+65);
  CYC(b_+65, b_+67); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+67, b_+70); TAIL(objectReplaceWithFallingDownHoleInteraction); }
  CYC(b_+67, b_+70);
  CALL_C(b_+70, s_objectSetInvisible, SYM(objectSetInvisible), b_+73);
  CYC(b_+73, b_+75); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x03);
  CYC(b_+77, b_+79); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x1e);
  CYC(b_+81, b_+83); B = 0x0f;
  CYC(b_+83, b_+86);
  TAIL(objectCreateInteractionWithSubid00);
state2:
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); push_effect(gb, b_+90);
  do { uint16_t jt_ = (swordShieldMazeGrabbableIce_jump_table(gb));
    if (jt_ == b_+98) goto state2_substate0;
    if (jt_ == b_+108) goto state2_substate1;
    if (jt_ == b_+109) goto state2_substate2;
    if (jt_ == b_+130) goto state2_substate3;
    HANDOFF(HL);
  } while (0);
state2_substate0:
  CYC(b_+98, b_+99); H = D;
  CYC(b_+99, b_+100); L = E;
  CYC(b_+100, b_+101); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+101, b_+102); alu_xor(gb, A);
  CYC(b_+102, b_+105); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+105, b_+108);
  TAIL(objectSetVisible81);
state2_substate1:
  RET(b_+108); return;
state2_substate2:
  CALL_C(b_+109, s_objectCheckWithinRoomBoundary, SYM(objectCheckWithinRoomBoundary), b_+112);
  if (!(F & FC)) { CYCT(b_+112, b_+115); TAIL(interactionDelete); }
  CYC(b_+112, b_+115);
  CALL_C(b_+115, s_objectSetVisiblec1, SYM(objectSetVisiblec1), b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+121, b_+123); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+123, b_+125); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+127); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+127, b_+129); goto func_5833; }
  CYC(b_+127, b_+129);
  RET(b_+129); return;
state2_substate3:
  CYC(b_+130, b_+131); H = D;
  CYC(b_+131, b_+133); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+133, b_+135); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+135, b_+137); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+137, b_+138); alu_xor(gb, A);
  CYC(b_+138, b_+139); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+139, b_+140); A = alu_inc8(gb, A);
  CYC(b_+140, b_+141); mem_wr(gb, HL, A);
  CYC(b_+141, b_+144);
  TAIL(objectSetVisible82);
state3:
  CALL_C(b_+144, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+147);
  if (!(F & FZ)) { RET_TAKEN(b_+147); return; }
  CYC(b_+147, b_+148);
  CYC(b_+148, b_+151); A = mem_rd(gb, w1Link_state);
  CYC(b_+151, b_+153); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+153, b_+155); TAIL_S(delete); }
  CYC(b_+153, b_+155);
  CYC(b_+155, b_+158); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+158, b_+159); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+159, b_+161); TAIL_S(delete); }
  CYC(b_+159, b_+161);
  CYC(b_+161, b_+164); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+164, b_+166); alu_cp(gb, 0xd0);
  if (!(F & FZ)) { CYCT(b_+166, b_+168); TAIL_S(delete); }
  CYC(b_+166, b_+168);
  CALL_C(b_+168, s_resetLinkInvincibility, SYM(resetLinkInvincibility), b_+171);
  CYC(b_+171, b_+173); A = 0x80;
  CYC(b_+173, b_+176); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+176, b_+179); mem_wr(gb, wDisableWarpTiles, A);
  CYC(b_+179, b_+182); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(b_+182, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+185);
  CYC(b_+185, b_+187); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CALL_C(b_+187, s_func_58e4_hook, SYM(func_58e4), b_+190);
  CYC(b_+190, b_+192); A = mem_rd(gb, hActiveObject);
  CYC(b_+192, b_+193); D = A;
  CYC(b_+193, b_+196); A = mem_rd(gb, wDungeonFloor);
  CYC(b_+196, b_+197); A = alu_dec8(gb, A);
  CYC(b_+197, b_+200); mem_wr(gb, wDungeonFloor, A);
  CALL_C(b_+200, s_getActiveRoomFromDungeonMapPosition, SYM(getActiveRoomFromDungeonMapPosition), b_+203);
  CYC(b_+203, b_+206); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+206, b_+208); A = 0x85;
  CYC(b_+208, b_+211); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+211, b_+213); A = 0x0f;
  CYC(b_+213, b_+216); mem_wr(gb, wWarpTransition, A);
  CYC(b_+216, b_+218); A = 0x03;
  CYC(b_+218, b_+221); mem_wr(gb, wWarpTransition2, A);
  s_delete_hook(gb); return; // falls through
}

void s_delete_hook(GB *gb) {
  BASE(delete);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(interactionDelete);
}

void s_func_58e4_hook(GB *gb) {
  BASE(func_58e4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+4); H = alu_dec8(gb, H);
  CYC(b_+4, b_+5); B = L;
  CYC(b_+5, b_+8); A = mem_rd(gb, wActiveTileIndex);
  CYC(b_+8, b_+10); alu_cp(gb, 0xd0);
  CYC(b_+10, b_+13); A = mem_rd(gb, wActiveTilePos);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); TAIL_S(func_590c); }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); A = B;
  CYC(b_+16, b_+18); alu_sub(gb, 0x10);
  CALL_C(b_+18, s_func_5907_hook, SYM(func_5907), b_+21);
  if (F & FZ) { CYCT(b_+21, b_+23); TAIL_S(func_590b); }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); A = B;
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  CALL_C(b_+25, s_func_5907_hook, SYM(func_5907), b_+28);
  if (F & FZ) { CYCT(b_+28, b_+30); TAIL_S(func_590b); }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+33); alu_add(gb, 0x10);
  CYC(b_+33, b_+35);
  TAIL_S(func_590c);
}

void s_func_5907_hook(GB *gb) {
  BASE(func_5907);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); L = A;
  CYC(b_+1, b_+2); A = mem_rd(gb, HL);
  CYC(b_+2, b_+3); alu_or(gb, A);
  RET(b_+3); return;
}

void s_func_590b_hook(GB *gb) {
  BASE(func_590b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = L;
  s_func_590c_hook(gb); return; // falls through
}

void s_func_590c_hook(GB *gb) {
  BASE(func_590c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+3, b_+6); A = mem_rd(gb, wActiveRoom);
  CYC(b_+6, b_+8); alu_cp(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_5918; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); B = 0x27;
L_5918:
  CYC(b_+12, b_+13); A = B;
  CYC(b_+13, b_+16); mem_wr(gb, wWarpDestPos, A);
  RET(b_+16); return;
}

