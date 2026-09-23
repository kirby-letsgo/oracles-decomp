#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/3e.s.
// PART_3e

static uint16_t part3e_jump_table(GB *gb) {
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

static void part3e_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// PART_3e
void s_partCode3e_hook(GB *gb) {
  BASE(partCode3e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x9a);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto normalStatus; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = PART_BASE + OBJ_SUBID;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x01);
  CYC(b_+14, b_+16); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+18, b_+20); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x96);
  CYC(b_+22, b_+24); L = PART_BASE + OBJ_STATE;
  CYC(b_+24, b_+26); A = 0x03;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CALL_C(b_+27, s_partSetAnimation, SYM(partSetAnimation), b_+30);
normalStatus:
  CYC(b_+30, b_+32); E = PART_BASE + OBJ_STATE;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); push_effect(gb, b_+34);
  do { uint16_t jt_ = (part3e_jump_table(gb));
    if (jt_ == b_+44) goto state0;
    if (jt_ == b_+74) goto state1;
    if (jt_ == b_+137) goto state2;
    if (jt_ == b_+193) goto state3;
    if (jt_ == b_+224) goto state4;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+44, b_+45); H = D;
  CYC(b_+45, b_+46); L = E;
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+47, b_+49); L = PART_BASE + OBJ_VAR03;
  CYC(b_+49, b_+50); A = mem_rd(gb, HL);
  CYC(b_+50, b_+51); alu_or(gb, A);
  CYC(b_+51, b_+53); A = 0x1e;
  if (!(F & FZ)) { CYCT(b_+53, b_+55); goto func_6d87; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57); L = PART_BASE + OBJ_VAR30;
  CYC(b_+57, b_+59); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+59, b_+60); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+60, b_+62); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
  CYC(b_+63, b_+65); L = PART_BASE + OBJ_SPEED;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x50);
  CYC(b_+67, b_+69); L = PART_BASE + OBJ_VAR3F;
  CYC(b_+69, b_+71); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+71, b_+74);
  TAIL(objectSetVisible83);
state1:
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+77); L = PART_BASE + OBJ_VAR30;
  CYC(b_+77, b_+78); B = mem_rd(gb, HL);
  CYC(b_+78, b_+79); L = alu_inc8(gb, L);
  CYC(b_+79, b_+80); C = mem_rd(gb, HL);
  CYC(b_+80, b_+82); L = PART_BASE + OBJ_YH;
  CYC(b_+82, b_+83); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+83, b_+85); mem_wr(gb, hFF8F, A);
  CYC(b_+85, b_+86); L = alu_inc8(gb, L);
  CYC(b_+86, b_+87); A = mem_rd(gb, HL);
  CYC(b_+87, b_+89); mem_wr(gb, hFF8E, A);
  CYC(b_+89, b_+90); alu_sub(gb, C);
  CYC(b_+90, b_+91); A = alu_inc8(gb, A);
  CYC(b_+91, b_+93); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+93, b_+95); goto L_6d7c; }
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+97); A = mem_rd(gb, hFF8F);
  CYC(b_+97, b_+98); alu_sub(gb, B);
  CYC(b_+98, b_+99); A = alu_inc8(gb, A);
  CYC(b_+99, b_+101); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+101, b_+103); goto L_6d85; }
  CYC(b_+101, b_+103);
L_6d7c:
  CALL_C(b_+103, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+106);
  CYC(b_+106, b_+108); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+112);
  TAIL(objectApplySpeed);
L_6d85:
  CYC(b_+112, b_+114); A = 0xa0;
func_6d87:
  CYC(b_+114, b_+116); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+116, b_+117); mem_wr(gb, HL, A);
  CYC(b_+117, b_+118); L = E;
  CYC(b_+118, b_+120); mem_wr(gb, HL, 0x03);
  CYC(b_+120, b_+122); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+122, b_+124); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+124, b_+126); A = 0xab;
  CALL_C(b_+126, s_playSound, SYM(playSound_b00), b_+129);
  CYC(b_+129, b_+131); A = 0x01;
  CALL_C(b_+131, s_partSetAnimation, SYM(partSetAnimation), b_+134);
  CYC(b_+134, b_+137);
  TAIL(objectSetVisible81);
state2:
  CYC(b_+137, b_+138); E = alu_inc8(gb, E);
  CYC(b_+138, b_+139); A = mem_rd(gb, DE);
  CYC(b_+139, b_+140); push_effect(gb, b_+140);
  do { uint16_t jt_ = (part3e_jump_table(gb));
    if (jt_ == b_+148) goto substate0;
    if (jt_ == b_+157) goto substate1;
    if (jt_ == b_+167) goto substate2;
    if (jt_ == b_+184) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+148, b_+149); alu_xor(gb, A);
  CYC(b_+149, b_+152); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+152, b_+153); A = alu_inc8(gb, A);
  CYC(b_+153, b_+154); mem_wr(gb, DE, A);
  CYC(b_+154, b_+157);
  TAIL(objectSetVisible81);
substate1:
  CALL_C(b_+157, s_func_6e70_hook, SYM(func_6e70), b_+160);
  if (F & FZ) { RET_TAKEN(b_+160); return; }
  CYC(b_+160, b_+161);
  CALL_C(b_+161, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+164);
  CYC(b_+164, b_+167);
  TAIL(partDelete);
substate2:
  CALL_C(b_+167, s_func_6e37_hook, SYM(func_6e37), b_+170);
  if (F & FC) { CYCT(b_+170, b_+173); TAIL(partDelete); }
  CYC(b_+170, b_+173);
  CYC(b_+173, b_+175); E = PART_BASE + OBJ_ZH;
  CYC(b_+175, b_+176); A = mem_rd(gb, DE);
  CYC(b_+176, b_+177); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+177); return; }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+180); E = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+180, b_+182); A = 0x03;
  CYC(b_+182, b_+183); mem_wr(gb, DE, A);
  RET(b_+183); return;
substate3:
  CYC(b_+184, b_+186); B = 0x09;
  CALL_C(b_+186, s_objectCreateInteractionWithSubid00, SYM(objectCreateInteractionWithSubid00), b_+189);
  if (!(F & FZ)) { RET_TAKEN(b_+189); return; }
  CYC(b_+189, b_+190);
  CYC(b_+190, b_+193);
  TAIL(partDelete);
state3:
  CALL_C(b_+193, s_func_6e70_hook, SYM(func_6e70), b_+196);
  if (!(F & FZ)) { CYCT(b_+196, b_+199); TAIL(partDelete); }
  CYC(b_+196, b_+199);
  CALL_C(b_+199, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+202);
  if (F & FZ) { CYCT(b_+202, b_+204); goto L_6ded; }
  CYC(b_+202, b_+204);
  CYC(b_+204, b_+206); E = PART_BASE + OBJ_SUBID;
  CYC(b_+206, b_+207); A = mem_rd(gb, DE);
  CYC(b_+207, b_+208); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+208, b_+211); TAIL_S(seasonsFunc_10_6e6a); }
  CYC(b_+208, b_+211);
  CALL_C(b_+211, s_partAnimate, SYM(partAnimate), b_+214);
  CYC(b_+214, b_+216);
  goto L_6dff;
L_6ded:
  CYC(b_+216, b_+218); L = PART_BASE + OBJ_STATE;
  CYC(b_+218, b_+219); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+219, b_+221); A = 0x02;
  CYC(b_+221, b_+224);
  TAIL(partSetAnimation);
state4:
  CYC(b_+224, b_+226); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+226, b_+227); A = mem_rd(gb, DE);
  CYC(b_+227, b_+228); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+228, b_+231); TAIL(partDelete); }
  CYC(b_+228, b_+231);
  CALL_C(b_+231, s_partAnimate, SYM(partAnimate), b_+234);
L_6dff:
  CYC(b_+234, b_+236); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+236, b_+237); A = mem_rd(gb, DE);
  CYC(b_+237, b_+239); alu_cp(gb, 0xff);
  if (F & FZ) { RET_TAKEN(b_+239); return; }
  CYC(b_+239, b_+240);
  CYC(b_+240, b_+243); SET_HL(SYM(table_6e10));
  CYC(b_+243, b_+244); part3e_add_a_to_hl(gb, b_+244);
  CYC(b_+244, b_+246); E = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+246, b_+247); A = mem_rd(gb, HL);
  CYC(b_+247, b_+248); mem_wr(gb, DE, A);
  CYC(b_+248, b_+249); E = alu_inc8(gb, E);
  CYC(b_+249, b_+250); mem_wr(gb, DE, A);
  RET(b_+250); return;
}

void s_func_6e13_hook(GB *gb) {
  BASE(func_6e13);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_getFreePartSlot, SYM(getFreePartSlot), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x3d);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); E = L;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); E = L;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); L = PART_BASE + OBJ_SPEED;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x3c);
  CYC(b_+26, b_+28); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+28, b_+30); A = 0xc0;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0xff);
  CYC(b_+33, b_+36);
  TAIL(objectCopyPosition);
}

void s_func_6e37_hook(GB *gb) {
  BASE(func_6e37);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x00;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CALL_C(b_+5, s_checkObjectsCollided, SYM(checkObjectsCollided), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x3c);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x1e);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); alu_sub(gb, 0x06);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(b_+31, b_+33); goto L_6e5e; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x00);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+37, b_+39); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
L_6e5e:
  CYC(b_+39, b_+41); A = 0x63;
  CALL_C(b_+41, s_playSound, SYM(playSound_b00), b_+44);
  CYC(b_+44, b_+46); A = 0x83;
  CALL_C(b_+46, s_playSound, SYM(playSound_b00), b_+49);
  CYC(b_+49, b_+50); alu_scf(gb);
  RET(b_+50); return;
}

void s_seasonsFunc_10_6e6a_hook(GB *gb) {
  BASE(seasonsFunc_10_6e6a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectAddToGrabbableObjectBuffer, SYM(objectAddToGrabbableObjectBuffer), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectPushLinkAwayOnCollision);
}

void s_func_6e70_hook(GB *gb) {
  BASE(func_6e70);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x77);
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, s_objectCreatePuff, SYM(objectCreatePuff), b_+12);
  CYC(b_+12, b_+13); alu_or(gb, D);
  RET(b_+13); return;
}

