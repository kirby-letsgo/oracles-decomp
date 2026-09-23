#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/dodongo.s.
// ENEMY_DODONGO
// Variables:
// var30: Animation base?
// var31: Corresponds to direction Link was facing when he picked Dodongo up
// var32: Index in the "attack pattern" ($00-$0f).
// var33: Animation index?

static uint16_t dodongo_jump_table(GB *gb) {
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

static void dodongo_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_DODONGO
// Variables:
// var30: Animation base?
// var31: Corresponds to direction Link was facing when he picked Dodongo up
// var32: Index in the "attack pattern" ($00-$0f).
// var33: Animation index?
void s_enemyCode79_hook(GB *gb) {
  BASE(enemyCode79);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(enemyBoss_dead_b0f); }
  CYC(b_+5, b_+8);
normalStatus:
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (dodongo_jump_table(gb));
    if (jt_ == SYM(dodongo_state_uninitialized) && hook_is(gb, SYM(dodongo_state_uninitialized), s_dodongo_state_uninitialized_hook)) { s_dodongo_state_uninitialized_hook(gb); return; }
    if (jt_ == SYM(dodongo_state_stub) && hook_is(gb, SYM(dodongo_state_stub), s_dodongo_state_stub_hook)) { s_dodongo_state_stub_hook(gb); return; }
    if (jt_ == SYM(dodongo_state_grabbed) && hook_is(gb, SYM(dodongo_state_grabbed), s_dodongo_state_grabbed_hook)) { s_dodongo_state_grabbed_hook(gb); return; }
    if (jt_ == SYM(dodongo_state8) && hook_is(gb, SYM(dodongo_state8), s_dodongo_state8_hook)) { s_dodongo_state8_hook(gb); return; }
    if (jt_ == SYM(dodongo_state9) && hook_is(gb, SYM(dodongo_state9), s_dodongo_state9_hook)) { s_dodongo_state9_hook(gb); return; }
    if (jt_ == SYM(dodongo_stateA) && hook_is(gb, SYM(dodongo_stateA), s_dodongo_stateA_hook)) { s_dodongo_stateA_hook(gb); return; }
    if (jt_ == SYM(dodongo_stateB) && hook_is(gb, SYM(dodongo_stateB), s_dodongo_stateB_hook)) { s_dodongo_stateB_hook(gb); return; }
    if (jt_ == SYM(dodongo_stateC) && hook_is(gb, SYM(dodongo_stateC), s_dodongo_stateC_hook)) { s_dodongo_stateC_hook(gb); return; }
    if (jt_ == SYM(dodongo_stateD) && hook_is(gb, SYM(dodongo_stateD), s_dodongo_stateD_hook)) { s_dodongo_stateD_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_dodongo_state_uninitialized_hook(GB *gb) {
  BASE(dodongo_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0208);
  CALL_C(b_+3, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); A = 0x79;
  CYC(b_+9, b_+11); B = 0x81;
  CALL_C(b_+11, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+16, b_+18); A = 0x04;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, s_enemySetAnimation, SYM(enemySetAnimation), b_+22);
  CALL_C(b_+22, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+25);
  CYC(b_+25, b_+28);
  TAIL(objectSetVisible82);
}

void s_dodongo_state_grabbed_hook(GB *gb) {
  BASE(dodongo_state_grabbed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (dodongo_jump_table(gb));
    if (jt_ == b_+11) goto justGrabbed;
    if (jt_ == b_+42) goto beingHeld;
    if (jt_ == b_+80) goto released;
    if (jt_ == b_+103) goto landed;
    HANDOFF(HL);
  } while (0);
justGrabbed:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+16); A = 0x20;
  CYC(b_+16, b_+19); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+21, b_+24); A = mem_rd(gb, w1Link_direction);
  CYC(b_+24, b_+25); alu_add(gb, A);
  CYC(b_+25, b_+26); alu_add(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+29, b_+31); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x01);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0xff);
  CYC(b_+39, b_+42);
  TAIL(objectSetVisible81);
beingHeld:
  CALL_C(b_+42, s_dodongo_updateAnimationWhileHeld_hook, SYM(dodongo_updateAnimationWhileHeld), b_+45);
  if (F & FZ) { CYCT(b_+45, b_+47); goto dropDodongo; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+50); A = mem_rd(gb, wFrameCounter);
  CYC(b_+50, b_+52); alu_and(gb, 0x03);
  if (F & FZ) { RET_TAKEN(b_+52); return; }
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+56); SET_HL(wLinkImmobilized);
  CYC(b_+56, b_+58); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  RET(b_+58); return;
dropDodongo:
  CALL_C(b_+59, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+62);
  CYC(b_+62, b_+63); H = D;
  CYC(b_+63, b_+65); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x03);
  CYC(b_+67, b_+68); L = alu_inc8(gb, L);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x3c);
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+72, b_+74); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+74, b_+76); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+80);
  TAIL(enemySetAnimation);
released:
  CYC(b_+80, b_+81); H = D;
  CYC(b_+81, b_+83); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+83, b_+85); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+85, b_+87); goto L_67ca; }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+89); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+89, b_+91); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+94); alu_add(gb, 0x02);
  CALL_C(b_+94, s_enemySetAnimation, SYM(enemySetAnimation), b_+97);
L_67ca:
  CALL_C(b_+97, s_dodongo_setInvincibilityAndPlaySoundIfInSpikes_hook, SYM(dodongo_setInvincibilityAndPlaySoundIfInSpikes), b_+100);
  if (!(F & FZ)) { RET_TAKEN(b_+100); return; }
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+103);
  goto inSpikes;
landed:
  CYC(b_+103, b_+105); C = 0x20;
  CALL_C(b_+105, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+108);
  if (!(F & FZ)) { RET_TAKEN(b_+108); return; }
  CYC(b_+108, b_+109);
  CALL_C(b_+109, s_dodongo_setInvincibilityAndPlaySoundIfInSpikes_hook, SYM(dodongo_setInvincibilityAndPlaySoundIfInSpikes), b_+112);
  if (!(F & FZ)) { CYCT(b_+112, b_+115); TAIL_S(dodongo_resetMovement); }
  CYC(b_+112, b_+115);
inSpikes:
  CYC(b_+115, b_+116); H = D;
  CYC(b_+116, b_+118); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+118, b_+119); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) CALL_C_CC(b_+119, s_ecom_killObjectH, SYM(ecom_killObjectH_b0d), b_+122);
  else CYC(b_+119, b_+122);
  CYC(b_+122, b_+124); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0x0c);
  CYC(b_+126, b_+128); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x04);
  CYC(b_+130, b_+131); L = alu_dec8(gb, L);
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x1e);
  CYC(b_+133, b_+135); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+135, b_+137); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+137, b_+139); goto L_67fa; }
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+140); L = alu_dec8(gb, L);
  CYC(b_+140, b_+141); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+141, b_+142); alu_add(gb, A);
  CYC(b_+142, b_+144); alu_xor(gb, 0x10);
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);
L_67fa:
  CYC(b_+145, b_+148);
  TAIL(objectSetVisible82);
}

void s_dodongo_state_stub_hook(GB *gb) {
  BASE(dodongo_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

// Waiting for Link to enter room
void s_dodongo_state8_hook(GB *gb) {
  BASE(dodongo_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wcc93);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x09;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); A = 0x2e;
  CYC(b_+10, b_+13); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+13, s_playSound, SYM(playSound_b00), b_+16);
  s_dodongo_state9_hook(gb); return; // falls through
}

// Deciding what direction to walk in
void s_dodongo_state9_hook(GB *gb) {
  BASE(dodongo_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_dodongo_turnTowardLinkIfPossible_hook, SYM(dodongo_turnTowardLinkIfPossible), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x0a);
  CALL_C(b_+12, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x07);
  CYC(b_+17, b_+20); SET_HL(b_+57 /* @counter1Vals */);
  CYC(b_+20, b_+21); dodongo_add_a_to_hl(gb, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_rrca(gb);
  CYC(b_+29, b_+30); E = alu_dec8(gb, E);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CALL_C(b_+31, s_enemySetAnimation, SYM(enemySetAnimation), b_+34);
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+39); alu_and(gb, 0x08);
  CYC(b_+39, b_+40); alu_rrca(gb);
  CYC(b_+40, b_+41); alu_rrca(gb);
  CYC(b_+41, b_+44); SET_HL(b_+53 /* @collisionRadii */);
  CYC(b_+44, b_+45); dodongo_add_a_to_hl(gb, b_+45);
  CYC(b_+45, b_+47); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+47, b_+48); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); E = alu_inc8(gb, E);
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  RET(b_+52); return;
}

// Walking
void s_dodongo_stateA_hook(GB *gb) {
  BASE(dodongo_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_dodongo_playStompSoundAtInterval_hook, SYM(dodongo_playStompSoundAtInterval), b_+3);
  CALL_C(b_+3, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto walking; }
  CYC(b_+6, b_+8);
  CALL_C(b_+8, s_dodongo_updateAngleTowardLink_hook, SYM(dodongo_updateAngleTowardLink), b_+11);
  if (F & FC) { CYCT(b_+11, b_+14); TAIL_S(dodongo_initiateNextAttack); }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17);
  TAIL_S(dodongo_resetMovement);
walking:
  CALL_C(b_+17, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+20);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); TAIL_S(dodongo_doubleAnimate); }
  CYC(b_+20, b_+22);
  CALL_C(b_+22, s_dodongo_checkTileInFront_hook, SYM(dodongo_checkTileInFront), b_+25);
  if (!(F & FC)) { CYCT(b_+25, b_+28); TAIL_S(dodongo_resetMovement); }
  CYC(b_+25, b_+28);
  CALL_C(b_+28, s_objectApplySpeed, SYM(objectApplySpeed), b_+31);
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+36); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+36, b_+38); TAIL_S(dodongo_animate); }
  CYC(b_+36, b_+38);
  s_dodongo_doubleAnimate_hook(gb); return; // falls through
}

void s_dodongo_doubleAnimate_hook(GB *gb) {
  BASE(dodongo_doubleAnimate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_enemyAnimate, SYM(enemyAnimate), b_+3);
  s_dodongo_animate_hook(gb); return; // falls through
}

void s_dodongo_animate_hook(GB *gb) {
  BASE(dodongo_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(enemyAnimate);
}

// Opening mouth, preparing to fire
void s_dodongo_stateB_hook(GB *gb) {
  BASE(dodongo_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+7); TAIL_S(dodongo_checkEatBomb); }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto animate; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); B = 0x41;
  CALL_C(b_+13, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+16);
  CYC(b_+16, b_+18); A = 0xbb;
  CALL_C(b_+18, s_playSound, SYM(playSound_b00), b_+21);
  CYC(b_+21, b_+23);
  TAIL_S(dodongo_animate);
animate:
  CYC(b_+23, b_+25); alu_add(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); TAIL_S(dodongo_animate); }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29);
  TAIL_S(dodongo_resetMovement);
}

// In spikes?
void s_dodongo_stateC_hook(GB *gb) {
  BASE(dodongo_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_cp(gb, 0x1e);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto moveBack; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x32);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_xor(gb, 0x10);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+27, b_+28); alu_rrca(gb);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CALL_C(b_+29, s_enemySetAnimation, SYM(enemySetAnimation), b_+32);
moveBack:
  CALL_C(b_+32, s_dodongo_playStompSoundAtInterval_hook, SYM(dodongo_playStompSoundAtInterval), b_+35);
  CALL_C(b_+35, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+38);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto L_68ca; }
  CYC(b_+38, b_+40);
  CALL_C(b_+40, s_dodongo_checkInSpikes_hook, SYM(dodongo_checkInSpikes), b_+43);
  if (!(F & FZ)) { CYCT(b_+43, b_+45); TAIL_S(dodongo_resetMovement); }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+47, b_+49); A = 0x0a;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
L_68ca:
  CALL_C(b_+50, s_objectApplySpeed, SYM(objectApplySpeed), b_+53);
  CYC(b_+53, b_+55);
  TAIL_S(dodongo_doubleAnimate);
}

// Just ate a bomb
void s_dodongo_stateD_hook(GB *gb) {
  BASE(dodongo_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectAddToGrabbableObjectBuffer, SYM(objectAddToGrabbableObjectBuffer), b_+3);
  CALL_C(b_+3, s_objectPushLinkAwayOnCollision, SYM(objectPushLinkAwayOnCollision), b_+6);
  CALL_C(b_+6, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, s_dodongo_updateAnimationWhileSlimmingDown_hook, SYM(dodongo_updateAnimationWhileSlimmingDown), b_+13);
  if (F & FZ) { CYCT(b_+13, b_+15); TAIL_S(dodongo_resetMovement); }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_DIRECTION;
  s_dodongo_updateAnimation_hook(gb); return; // falls through
}

// @param	hl	Pointer to Enemy.direction
void s_dodongo_updateAnimation_hook(GB *gb) {
  BASE(dodongo_updateAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  CYC(b_+5, b_+7); alu_add(gb, 0x02);
  CYC(b_+7, b_+8); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+11);
  TAIL(enemySetAnimation);
}

void s_dodongo_resetMovement_hook(GB *gb) {
  BASE(dodongo_resetMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x09);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+7, b_+9); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+20);
  TAIL(enemySetAnimation);
}

// Either turns toward Link or, if facing a wall, turns in some other random direction.
// @param[out]	c	c if wasn't able to turn in any valid direction
void s_dodongo_turnTowardLinkIfPossible_hook(GB *gb) {
  BASE(dodongo_turnTowardLinkIfPossible);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_updateCardinalAngleTowardTarget, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+3);
  CALL_C(b_+3, s_dodongo_checkTileInFront_hook, SYM(dodongo_checkTileInFront), b_+6);
  if (F & FC) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CALL_C(b_+7, s_ecom_setRandomCardinalAngle, SYM(ecom_setRandomCardinalAngle_b0d), b_+10);
  s_dodongo_checkTileInFront_hook(gb); return; // falls through
}

// @param[out]	cflag	c if tile in front of dodongo is not a spike
void s_dodongo_checkTileInFront_hook(GB *gb) {
  BASE(dodongo_checkTileInFront);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_rrca(gb);
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+15); SET_HL(b_+34 /* @positionOffsets */);
  CYC(b_+15, b_+16); dodongo_add_a_to_hl(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); alu_add(gb, B);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_add(gb, C);
  CYC(b_+21, b_+22); C = A;
  CALL_C(b_+22, s_getTileAtPosition, SYM(getTileAtPosition), b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0xa4);
  CYC(b_+27, b_+28); alu_scf(gb);
  if (F & FZ) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); alu_sub(gb, 0x02);
  CYC(b_+31, b_+33); alu_cp(gb, 0x03);
  RET(b_+33); return;
}

void s_dodongo_playStompSoundAtInterval_hook(GB *gb) {
  BASE(dodongo_playStompSoundAtInterval);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0a);
  CYC(b_+5, b_+7); B = 0x1f;
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_693e; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); B = 0x0f;
L_693e:
  CYC(b_+11, b_+14); A = mem_rd(gb, wFrameCounter);
  CYC(b_+14, b_+15); alu_and(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); A = 0x85;
  CYC(b_+18, b_+21);
  TAIL(playSound_b00);
}

// @param[out]	c	c if Link is at a good angle to charge him
void s_dodongo_updateAngleTowardLink_hook(GB *gb) {
  BASE(dodongo_updateAngleTowardLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x0c;
  CALL_C(b_+2, s_objectCheckCenteredWithLink, SYM(objectCheckCenteredWithLink), b_+5);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+9);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_sub(gb, B);
  CYC(b_+14, b_+16); alu_add(gb, 0x04);
  CYC(b_+16, b_+18); alu_cp(gb, 0x09);
  RET(b_+18); return;
}

// @param[out]	zflag	z if dodongo is ready to continue moving
void s_dodongo_updateAnimationWhileSlimmingDown_hook(GB *gb) {
  BASE(dodongo_updateAnimationWhileSlimmingDown);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); alu_cp(gb, 0x11);
  if (F & FZ) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+10, b_+12); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(b_+12, b_+14); goto L_696d; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x08);
  CYC(b_+16, b_+17); alu_or(gb, D);
  RET(b_+17); return;
L_696d:
  CYC(b_+18, b_+21); SET_BC(b_+28 /* @counter2Vals */);
  CALL_C(b_+21, s_addAToBc, SYM(addAToBc), b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, BC);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+27); alu_or(gb, D);
  RET(b_+27); return;
}

// dodongo_checkEatBomb@checkBombInRangeToEat
static void dodongo_checkBombInRangeToEat(GB *gb) {
  BASE(dodongo_checkEatBomb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+59, b_+61); L = 0x2f;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+64, b_+66); goto notEaten; }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); alu_and(gb, 0xb0);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto notEaten; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); L = 0x05;
  CYC(b_+72, b_+73); A = mem_rd(gb, HL);
  CYC(b_+73, b_+75); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+75, b_+77); goto notEaten; }
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+78); push_effect(gb, HL);
  CYC(b_+78, b_+80); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);
  CYC(b_+81, b_+82); alu_rrca(gb);
  CYC(b_+82, b_+83); alu_rrca(gb);
  CYC(b_+83, b_+86); SET_HL(b_+124 /* @positionOffsets */);
  CYC(b_+86, b_+87); dodongo_add_a_to_hl(gb, b_+87);
  CYC(b_+87, b_+88); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+88, b_+89); C = mem_rd(gb, HL);
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+92, b_+93); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+93, b_+94); B = A;
  CYC(b_+94, b_+96); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);
  CYC(b_+97, b_+98); alu_add(gb, C);
  CYC(b_+98, b_+99); C = A;
  CYC(b_+99, b_+100); SET_HL(pop_effect(gb));
  CYC(b_+100, b_+102); L = 0x0b;
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+104); alu_sub(gb, B);
  CYC(b_+104, b_+106); alu_add(gb, 0x0c);
  CYC(b_+106, b_+108); alu_cp(gb, 0x19);
  if (!(F & FC)) { CYCT(b_+108, b_+110); goto notEaten; }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); L = 0x0d;
  CYC(b_+112, b_+113); A = mem_rd(gb, HL);
  CYC(b_+113, b_+114); alu_sub(gb, C);
  CYC(b_+114, b_+116); alu_add(gb, 0x08);
  CYC(b_+116, b_+118); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(b_+118, b_+120); goto notEaten; }
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+121); alu_xor(gb, A);
  RET(b_+121); return;
notEaten:
  CYC(b_+122, b_+123); alu_or(gb, D);
  RET(b_+123); return;
}

void s_dodongo_checkEatBomb_hook(GB *gb) {
  BASE(dodongo_checkEatBomb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x03;
  CALL_C(b_+2, s_findItemWithID, SYM(findItemWithID), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto animate; }
  CYC(b_+5, b_+7);
  CALL_L(b_+7, dodongo_checkBombInRangeToEat, b_+10);
  if (F & FZ) { CYCT(b_+10, b_+12); goto eatBomb; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); C = 0x03;
  CALL_C(b_+14, s_findItemWithID_startingAfterH, SYM(findItemWithID_startingAfterH), b_+17);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto animate; }
  CYC(b_+17, b_+19);
  CALL_L(b_+19, dodongo_checkBombInRangeToEat, b_+22);
  if (F & FZ) { CYCT(b_+22, b_+24); goto eatBomb; }
  CYC(b_+22, b_+24);
animate:
  CYC(b_+24, b_+27);
  TAIL(enemyAnimate);
eatBomb:
  CYC(b_+27, b_+29); L = 0x2f;
  CYC(b_+29, b_+31); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+31, b_+32); H = D;
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x00);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+38, b_+40); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x0d);
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x78);
  CYC(b_+49, b_+51); alu_add(gb, 0x02);
  CALL_C(b_+51, s_enemySetAnimation, SYM(enemySetAnimation), b_+54);
  CYC(b_+54, b_+56); A = 0x94;
  CYC(b_+56, b_+59);
  TAIL(playSound_b00);
}

// Determines next attack.
void s_dodongo_initiateNextAttack_hook(GB *gb) {
  BASE(dodongo_initiateNextAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x0f);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+10); SET_HL(b_+44 /* @attackPattern */);
  CALL_C(b_+10, s_checkFlag, SYM(checkFlag), b_+13);
  CYC(b_+13, b_+14); H = D;
  if (F & FZ) { CYCT(b_+14, b_+16); goto chargeAttack; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x0b);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); A = alu_inc8(gb, A);
  CALL_C(b_+24, s_enemySetAnimation, SYM(enemySetAnimation), b_+27);
  CYC(b_+27, b_+29); A = 0x7f;
  CYC(b_+29, b_+32);
  TAIL(playSound_b00);
chargeAttack:
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x32);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x2d);
  CYC(b_+40, b_+41); L = alu_dec8(gb, L);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x80);
  RET(b_+43); return;
}

// @param[out]	zflag	z if in spikes
void s_dodongo_setInvincibilityAndPlaySoundIfInSpikes_hook(GB *gb) {
  BASE(dodongo_setInvincibilityAndPlaySoundIfInSpikes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_dodongo_checkInSpikes_hook, SYM(dodongo_checkInSpikes), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+6, b_+8); A = 0x30;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); A = 0x63;
  CALL_C(b_+11, s_playSound, SYM(playSound_b00), b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  RET(b_+15); return;
}

// @param[out]	zflag	z if in spikes
void s_dodongo_checkInSpikes_hook(GB *gb) {
  BASE(dodongo_checkInSpikes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+11); alu_add(gb, 0x05);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); C = mem_rd(gb, HL);
  CALL_C(b_+14, s_getTileAtPosition, SYM(getTileAtPosition), b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0x60);
  RET(b_+19); return;
}

// @param[out]	zflag	z if king dodongo has regained normal weight and is ready to move
void s_dodongo_updateAnimationWhileHeld_hook(GB *gb) {
  BASE(dodongo_updateAnimationWhileHeld);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_6a5e; }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, s_dodongo_updateAnimationWhileSlimmingDown_hook, SYM(dodongo_updateAnimationWhileSlimmingDown), b_+9);
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
L_6a5e:
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+12, b_+15); A = mem_rd(gb, w1Link_direction);
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+19); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, HL, B);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+25); alu_and(gb, 0x0c);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CALL_C(b_+26, s_dodongo_updateAnimation_hook, SYM(dodongo_updateAnimation), b_+29);
  CYC(b_+29, b_+30); alu_or(gb, D);
  RET(b_+30); return;
}
