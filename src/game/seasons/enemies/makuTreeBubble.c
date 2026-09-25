#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/makuTreeBubble.s.
// ENEMY_MAKU_TREE_BUBBLE
// Variables:
// $cfc0: bit 7 set when popped

static uint16_t makuTreeBubble_jump_table(GB *gb) {
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

static void makuTreeBubble_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_MAKU_TREE_BUBBLE
// Variables:
// $cfc0: bit 7 set when popped
void s_enemyCode56_hook(GB *gb) {
  BASE(enemyCode56);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+4, b_+6); A = 0x02;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
normalStatus:
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (makuTreeBubble_jump_table(gb));
    if (jt_ == b_+19) goto state0;
    if (jt_ == b_+27) goto state1;
    if (jt_ == b_+135) goto state2;
    if (jt_ == b_+187) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+19, b_+21); A = 0x01;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, s_objectSetVisible80, SYM(objectSetVisible80), b_+25);
  CYC(b_+25, b_+27);
  goto snore;
state1:
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_757e; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+35); SET_HL(wTmpcfc0);
  CYC(b_+35, b_+37); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+37, b_+39); goto L_757e; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+41, b_+43); A = 0x02;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
L_757e:
  CALL_C(b_+44, s_enemyAnimate, SYM(enemyAnimate), b_+47);
snore:
  CYC(b_+47, b_+49); A = 0x0b;
  CALL_C(b_+49, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+52);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+54); B = A;
  CYC(b_+54, b_+55); L = alu_inc8(gb, L);
  CYC(b_+55, b_+56); C = mem_rd(gb, HL);
  CYC(b_+56, b_+58); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+61); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+61, b_+63); goto L_75a3; }
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+65); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+67, b_+69); goto L_75a6; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); E = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+74); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+74, b_+76); goto L_75a3; }
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); A = 0x92;
  CALL_C(b_+78, s_playSound, SYM(playSound_b00), b_+81);
L_75a3:
  CYC(b_+81, b_+83); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
L_75a6:
  CYC(b_+84, b_+85); alu_add(gb, A);
  CYC(b_+85, b_+88); SET_HL(b_+111 /* @bubbleOffsetAndCollisionRadius */);
  CYC(b_+88, b_+89); makuTreeBubble_add_double_index(gb, b_+89);
  CYC(b_+89, b_+91); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+91, b_+92); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+92, b_+93); alu_add(gb, B);
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, b_+96); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+96, b_+97); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+97, b_+98); alu_add(gb, C);
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+101); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+101, b_+103); A = 0xf8;
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CYC(b_+104, b_+106); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CYC(b_+108, b_+109); E = alu_inc8(gb, E);
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  RET(b_+110); return;

state2:
  CYC(b_+135, b_+136); H = D;
  CYC(b_+136, b_+138); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+138, b_+139); A = mem_rd(gb, HL);
  CYC(b_+139, b_+140); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+140, b_+142); goto L_75fd; }
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+143); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+143, b_+145); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+145, b_+147); B = 0x0b;
  CALL_C(b_+147, s_clearMemory, SYM(clearMemory), b_+150);
  CYC(b_+150, b_+152); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+152, b_+153); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+153, b_+155); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+155, b_+157); A = 0x01;
  CYC(b_+157, b_+158); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+158, b_+159); mem_wr(gb, HL, A);
  CYC(b_+159, b_+161); A = 0x01;
  CALL_C(b_+161, s_enemySetAnimation, SYM(enemySetAnimation), b_+164);
  CYC(b_+164, b_+167); SET_HL(wTmpcfc0);
  CYC(b_+167, b_+169); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+169, b_+171);
  goto snore;
L_75fd:
  CYC(b_+171, b_+173); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+173, b_+175); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+175, b_+178); TAIL(enemyAnimate); }
  CYC(b_+175, b_+178);
  CYC(b_+178, b_+179); alu_xor(gb, A);
  CYC(b_+179, b_+180); mem_wr(gb, HL, A);
  CYC(b_+180, b_+182); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+182, b_+183); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+183, b_+184); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+184, b_+187);
  goto snore;
state3:
  CALL_C(b_+187, s_enemyAnimate, SYM(enemyAnimate), b_+190);
  CYC(b_+190, b_+192); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+192, b_+193); A = mem_rd(gb, DE);
  CYC(b_+193, b_+194); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+194); return; }
  CYC(b_+194, b_+195);
  CYC(b_+195, b_+196); alu_rlca(gb);
  if (F & FC) { CYCT(b_+196, b_+199); TAIL(enemyDelete); }
  CYC(b_+196, b_+199);
  CYC(b_+199, b_+200); H = D;
  CYC(b_+200, b_+202); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+202, b_+203); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+203); return;
}

