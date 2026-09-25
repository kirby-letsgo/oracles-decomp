#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/springBloomFlower.s.
// INTERAC_SPRINGBLOOM_FLOWER

static uint16_t springBloomFlower_jump_table(GB *gb) {
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

static void springBloomFlower_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SPRINGBLOOM_FLOWER
void s_interactionCode9c_hook(GB *gb) {
  BASE(interactionCode9c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (springBloomFlower_jump_table(gb));
    if (jt_ == b_+20) goto state0;
    if (jt_ == b_+41) goto state1;
    if (jt_ == b_+78) goto state2;
    if (jt_ == b_+160) goto state3;
    if (jt_ == b_+202) goto state4;
    if (jt_ == b_+212) goto state5;
    if (jt_ == b_+233) goto state6;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+26); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+27, b_+30); TAIL(interactionDelete); }
  CYC(b_+27, b_+30);
  CYC(b_+30, b_+32); A = 0x06;
  CALL_C(b_+32, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+35);
  CALL_C(b_+35, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+38);
  CALL_C(b_+38, s_objectSetVisible83, SYM(objectSetVisible83), b_+41);
state1:
  CYC(b_+41, b_+44); A = mem_rd(gb, wUnknown);
  CYC(b_+44, b_+45); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+45, b_+47); goto L_5c1f; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); A = 0x05;
  CYC(b_+49, b_+51);
  goto L_5c32;
L_5c1f:
  CYC(b_+51, b_+54); A = mem_rd(gb, wWarpsDisabled);
  CYC(b_+54, b_+55); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+55); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+59); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+59, b_+60); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+60); return; }
  CYC(b_+60, b_+61);
  CALL_C(b_+61, s_objectCheckCollidedWithLink, SYM(objectCheckCollidedWithLink), b_+64);
  if (!(F & FC)) { RET_TAKEN(b_+64); return; }
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+67); A = 0x02;
  CYC(b_+67, b_+70); mem_wr(gb, wUnknown, A);
L_5c32:
  CYC(b_+70, b_+72); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  CYC(b_+73, b_+75); A = 0x01;
  CYC(b_+75, b_+78);
  TAIL(interactionSetAnimation);
state2:
  CALL_C(b_+78, s_interactionAnimate, SYM(interactionAnimate), b_+81);
  CYC(b_+81, b_+83); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+85); return; }
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+89); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+89, b_+91); alu_cp(gb, 0xd0);
  if (!(F & FZ)) { CYCT(b_+91, b_+94); TAIL_S(seasonsFunc_0a_5d18); }
  CYC(b_+91, b_+94);
  CALL_C(b_+94, s_checkLinkID0AndControlNormal_hook, SYM(checkLinkID0AndControlNormal), b_+97);
  if (!(F & FC)) { CYCT(b_+97, b_+100); TAIL_S(seasonsFunc_0a_5d18); }
  CYC(b_+97, b_+100);
  CALL_C(b_+100, s_objectCheckCollidedWithLink, SYM(objectCheckCollidedWithLink), b_+103);
  if (!(F & FC)) { CYCT(b_+103, b_+106); TAIL_S(seasonsFunc_0a_5d18); }
  CYC(b_+103, b_+106);
  CYC(b_+106, b_+108); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+108, b_+110); A = 0x03;
  CYC(b_+110, b_+111); mem_wr(gb, DE, A);
  CALL_C(b_+111, s_clearAllParentItems, SYM(clearAllParentItems), b_+114);
  CALL_C(b_+114, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+117);
  CALL_C(b_+117, s_resetLinkInvincibility, SYM(resetLinkInvincibility), b_+120);
  CYC(b_+120, b_+122); A = 0x83;
  CYC(b_+122, b_+125); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+125, b_+128); mem_wr(gb, wWarpsDisabled, A);
  CALL_C(b_+128, s_setLinkForceStateToState08, SYM(setLinkForceStateToState08), b_+131);
  CALL_C(b_+131, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+134);
  CYC(b_+134, b_+135); alu_xor(gb, A);
  CYC(b_+135, b_+137); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CALL_C(b_+137, s_func_5cf2_hook, SYM(func_5cf2), b_+140);
  CYC(b_+140, b_+142); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+146); mem_wr(gb, w1Link_xh, A);
  CYC(b_+146, b_+147); alu_xor(gb, A);
  CYC(b_+147, b_+150); mem_wr(gb, w1Link_zh, A);
  CYC(b_+150, b_+152); A = 0x52;
  CALL_C(b_+152, s_playSound, SYM(playSound_b00), b_+155);
  CYC(b_+155, b_+157); A = 0x02;
  CYC(b_+157, b_+160);
  TAIL(interactionSetAnimation);
state3:
  CYC(b_+160, b_+162); A = 0x10;
  CYC(b_+162, b_+165); mem_wr(gb, wcc50, A);
  CALL_C(b_+165, s_interactionAnimate, SYM(interactionAnimate), b_+168);
  CYC(b_+168, b_+170); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+170, b_+171); A = mem_rd(gb, DE);
  CYC(b_+171, b_+172); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+172, b_+174); goto func_5ca0; }
  CYC(b_+172, b_+174);
  CYC(b_+174, b_+176); alu_cp(gb, 0x02);
  if (!(F & FC)) CALL_C_CC(b_+176, s_func_5cf2_hook, SYM(func_5cf2), b_+179);
  else CYC(b_+176, b_+179);
  RET(b_+179); return;
func_5ca0:
  CYC(b_+180, b_+182); A = 0x06;
  CALL_C(b_+182, s_func_5cf2_hook, SYM(func_5cf2), b_+185);
  CYC(b_+185, b_+186); alu_xor(gb, A);
  CYC(b_+186, b_+189); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+189, b_+191); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+191, b_+193); A = 0x04;
  CYC(b_+193, b_+194); mem_wr(gb, DE, A);
  CYC(b_+194, b_+196); A = 0x06;
  CYC(b_+196, b_+199); mem_wr(gb, wLinkForceState, A);
  CYC(b_+199, b_+202);
  TAIL(objectSetVisible83);
state4:
  CALL_C(b_+202, s_interactionAnimate, SYM(interactionAnimate), b_+205);
  CYC(b_+205, b_+207); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+207, b_+208); A = mem_rd(gb, DE);
  CYC(b_+208, b_+209); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+209); return; }
  CYC(b_+209, b_+210);
  CYC(b_+210, b_+212);
  TAIL_S(seasonsFunc_0a_5d18);
state5:
  CALL_C(b_+212, s_interactionAnimate, SYM(interactionAnimate), b_+215);
  CYC(b_+215, b_+217); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+217, b_+218); A = mem_rd(gb, DE);
  CYC(b_+218, b_+219); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+219); return; }
  CYC(b_+219, b_+220);
  CYC(b_+220, b_+222); A = 0x52;
  CALL_C(b_+222, s_playSound, SYM(playSound_b00), b_+225);
  CALL_C(b_+225, s_interactionIncState, SYM(interactionIncState), b_+228);
  CYC(b_+228, b_+230); A = 0x02;
  CYC(b_+230, b_+233);
  TAIL(interactionSetAnimation);
state6:
  CALL_C(b_+233, s_interactionAnimate, SYM(interactionAnimate), b_+236);
  CYC(b_+236, b_+238); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+238, b_+239); A = mem_rd(gb, DE);
  CYC(b_+239, b_+240); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+240, b_+242); goto func_5ce8; }
  CYC(b_+240, b_+242);
  CYC(b_+242, b_+243); mem_wr(gb, DE, A);
  CYC(b_+243, b_+246); mem_wr(gb, wUnknown, A);
  CALL_C(b_+246, s_objectSetVisible83, SYM(objectSetVisible83), b_+249);
  CYC(b_+249, b_+252);
  TAIL(interactionIncState);
func_5ce8:
  CYC(b_+252, b_+253); A = alu_dec8(gb, A);
  CYC(b_+253, b_+256); mem_wr(gb, wUnknown, A);
  CYC(b_+256, b_+258); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+258); return; }
  CYC(b_+258, b_+259);
  CYC(b_+259, b_+262);
  TAIL(objectSetVisible82);
}

void s_func_5cf2_hook(GB *gb) {
  BASE(func_5cf2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(table_5d08));
  CYC(b_+3, b_+4); springBloomFlower_add_double_index(gb, b_+4);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+13); mem_wr(gb, w1Link_yh, A);
  CYC(b_+13, b_+14); SET_HL(HL + 1);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); alu_and(gb, 0xf0);
  CYC(b_+19, b_+20); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return;
}

void s_seasonsFunc_0a_5d18_hook(GB *gb) {
  BASE(seasonsFunc_0a_5d18);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wUnknown, A);
  CALL_C(b_+9, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+14, s_objectSetVisible83, SYM(objectSetVisible83), b_+17);
  CYC(b_+17, b_+19); A = 0x00;
  CYC(b_+19, b_+22);
  TAIL(interactionSetAnimation);
}

