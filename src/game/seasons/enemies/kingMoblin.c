#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/kingMoblin.s.
// ENEMY_KING_MOBLIN

static uint16_t kingMoblin_jump_table(GB *gb) {
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

static void kingMoblin_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void kingMoblin_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_KING_MOBLIN
void s_enemyCode07_hook(GB *gb) {
  BASE(enemyCode07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_cp(gb, 0x97);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto normalStatus; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+20, s_func_6f20_hook, SYM(func_6f20), b_+23);
  else CYC(b_+20, b_+23);
  CYC(b_+23, b_+25); A = 0x63;
  CYC(b_+25, b_+28);
  TAIL(playSound_b00);
dead:
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x0e);
  CYC(b_+33, b_+34); L = alu_inc8(gb, L);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x00);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+38, b_+40); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
normalStatus:
  CYC(b_+40, b_+42); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); push_effect(gb, b_+44);
  do { uint16_t jt_ = (kingMoblin_jump_table(gb));
    if (jt_ == b_+74) goto state0;
    if (jt_ == b_+100) goto stateStub;
    if (jt_ == b_+101) goto state8;
    if (jt_ == b_+118) goto state9;
    if (jt_ == b_+182) goto stateA;
    if (jt_ == b_+210) goto stateB;
    if (jt_ == b_+235) goto stateC;
    if (jt_ == b_+257) goto stateD;
    if (jt_ == b_+268) goto stateE;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+74, b_+76); A = 0x07;
  CYC(b_+76, b_+79); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);
  CYC(b_+79, b_+81); A = 0x8c;
  CALL_C(b_+81, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+84);
  CYC(b_+84, b_+86); A = 0x14;
  CALL_C(b_+86, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+89);
  CYC(b_+89, b_+91); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+91, b_+93); A = 0x02;
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CALL_C(b_+94, s_enemySetAnimation, SYM(enemySetAnimation), b_+97);
  CYC(b_+97, b_+100);
  TAIL(objectSetVisible83);
stateStub:
  RET(b_+100); return;
state8:
  CYC(b_+101, b_+104); SET_HL(wTmpcfc0);
  CYC(b_+104, b_+106); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+106, b_+109); TAIL(enemyAnimate); }
  CYC(b_+106, b_+109);
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x00);
  CYC(b_+111, b_+112); H = D;
  CYC(b_+112, b_+113); L = E;
  CYC(b_+113, b_+114); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+114, b_+116); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x14);
state9:
  CALL_C(b_+118, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+121);
  CYC(b_+121, b_+123); alu_and(gb, 0x07);
  CYC(b_+123, b_+125); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+125, b_+127); goto state9; }
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+128); H = D;
  CYC(b_+128, b_+130); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+130, b_+131); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+131, b_+133); goto state9; }
  CYC(b_+131, b_+133);
  CYC(b_+133, b_+134); mem_wr(gb, HL, A);
  CYC(b_+134, b_+137); SET_HL(b_+175 /* @table_6e03 */);
  CYC(b_+137, b_+138); kingMoblin_add_a_to_hl(gb, b_+138);
  CYC(b_+138, b_+140); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);
  CYC(b_+141, b_+142); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+142, b_+144); goto state9; }
  CYC(b_+142, b_+144);
  CYC(b_+144, b_+146); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+146, b_+147); A = mem_rd(gb, HL);
  CYC(b_+147, b_+148); mem_wr(gb, DE, A);
  CYC(b_+148, b_+149); H = D;
  CYC(b_+149, b_+151); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+151, b_+152); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+152, b_+154); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+154, b_+155); A = mem_rd(gb, HL);
  CYC(b_+155, b_+157); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+157, b_+158); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+158, b_+160); A = 0x03;
  CYC(b_+160, b_+162); B = 0x18;
  if (!(F & FC)) { CYCT(b_+162, b_+164); goto L_6dfc; }
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+166); A = 0x01;
  CYC(b_+166, b_+168); B = 0x08;
L_6dfc:
  CYC(b_+168, b_+170); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+170, b_+171); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+171, b_+172); mem_wr(gb, HL, B);
  CYC(b_+172, b_+175);
  TAIL(enemySetAnimation);

stateA:
  CALL_C(b_+182, s_objectApplySpeed, SYM(objectApplySpeed), b_+185);
  CYC(b_+185, b_+186); H = D;
  CYC(b_+186, b_+188); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+188, b_+189); A = mem_rd(gb, HL);
  CYC(b_+189, b_+191); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+191, b_+192); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+192, b_+193); A = alu_inc8(gb, A);
  CYC(b_+193, b_+195); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+195, b_+197); goto animate; }
  CYC(b_+195, b_+197);
  CYC(b_+197, b_+199); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+199, b_+200); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+200, s_func_6f2e_hook, SYM(func_6f2e), b_+203);
  CYC(b_+203, b_+205); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+205, b_+206); alu_xor(gb, A);
  CYC(b_+206, b_+207); mem_wr(gb, DE, A);
  CYC(b_+207, b_+210);
  TAIL(enemySetAnimation);
stateB:
  CALL_C(b_+210, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+213);
  if (!(F & FZ)) { CYCT(b_+213, b_+215); goto animate; }
  CYC(b_+213, b_+215);
  CYC(b_+215, b_+216); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+216, b_+218); B = 0x3f;
  CALL_C(b_+218, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+221);
  if (!(F & FZ)) { RET_TAKEN(b_+221); return; }
  CYC(b_+221, b_+222);
  CYC(b_+222, b_+224); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+224, b_+226); A = 0x0c;
  CYC(b_+226, b_+227); mem_wr(gb, DE, A);
  CYC(b_+227, b_+229); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+229, b_+231); A = 0x04;
  CYC(b_+231, b_+232); mem_wr(gb, DE, A);
  CYC(b_+232, b_+235);
  TAIL(enemySetAnimation);
stateC:
  CALL_C(b_+235, s_func_6f40_hook, SYM(func_6f40), b_+238);
  if (!(F & FC)) { RET_TAKEN(b_+238); return; }
  CYC(b_+238, b_+239);
  CYC(b_+239, b_+240); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+240); return; }
  CYC(b_+240, b_+241);
  CYC(b_+241, b_+243); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+243, b_+245); A = 0x0d;
  CYC(b_+245, b_+246); mem_wr(gb, DE, A);
  CALL_C(b_+246, s_func_6f2e_hook, SYM(func_6f2e), b_+249);
  CYC(b_+249, b_+251); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+251, b_+253); A = 0x02;
  CYC(b_+253, b_+254); mem_wr(gb, DE, A);
  CYC(b_+254, b_+257);
  TAIL(enemySetAnimation);
stateD:
  CALL_C(b_+257, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+260);
  if (!(F & FZ)) { CYCT(b_+260, b_+262); goto animate; }
  CYC(b_+260, b_+262);
  CYC(b_+262, b_+263); L = E;
  CYC(b_+263, b_+265); mem_wr(gb, HL, 0x09);
animate:
  CYC(b_+265, b_+268);
  TAIL(enemyAnimate);
stateE:
  CYC(b_+268, b_+269); E = alu_inc8(gb, E);
  CYC(b_+269, b_+270); A = mem_rd(gb, DE);
  CYC(b_+270, b_+271); push_effect(gb, b_+271);
  do { uint16_t jt_ = (kingMoblin_jump_table(gb));
    if (jt_ == b_+281) goto substate0;
    if (jt_ == b_+339) goto substate1;
    if (jt_ == b_+372) goto substate2;
    if (jt_ == b_+392) goto substate3;
    if (jt_ == b_+459) goto substateStub;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+281, s_checkLinkCollisionsEnabled, SYM(checkLinkCollisionsEnabled), b_+284);
  if (!(F & FC)) { RET_TAKEN(b_+284); return; }
  CYC(b_+284, b_+285);
  CYC(b_+285, b_+286); H = D;
  CYC(b_+286, b_+288); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+288, b_+289); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+289, b_+291); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+291, b_+293); mem_wr(gb, HL, 0x3c);
  CYC(b_+293, b_+295); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+295, b_+297); mem_wr(gb, HL, 0x01);
  CYC(b_+297, b_+299); A = 0x01;
  CALL_C(b_+299, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+302);
  CYC(b_+302, b_+303); A = mem_rd(gb, HL);
  CYC(b_+303, b_+305); alu_cp(gb, 0x3f);
  if (!(F & FZ)) { CYCT(b_+305, b_+307); goto L_6ea2; }
  CYC(b_+305, b_+307);
  CYC(b_+307, b_+309); L = PART_BASE + OBJ_STATE;
  CYC(b_+309, b_+310); A = mem_rd(gb, HL);
  CYC(b_+310, b_+311); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+311, b_+313); goto L_6ea2; }
  CYC(b_+311, b_+313);
  CYC(b_+313, b_+315); mem_wr(gb, HL, 0x06);
  CYC(b_+315, b_+317); L = PART_BASE + OBJ_YH;
  CYC(b_+317, b_+319); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+319, b_+320); A = mem_rd(gb, DE);
  CYC(b_+320, b_+322); alu_sub(gb, 0x10);
  CYC(b_+322, b_+323); mem_wr(gb, HL, A);
  CYC(b_+323, b_+325); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+325, b_+327); A = 0x01;
  CYC(b_+327, b_+328); mem_wr(gb, DE, A);
  CYC(b_+328, b_+331); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+331, b_+334); mem_wr(gb, wDisabledObjects, A);
L_6ea2:
  CYC(b_+334, b_+336); A = 0x05;
  CYC(b_+336, b_+339);
  TAIL(enemySetAnimation);
substate1:
  CALL_C(b_+339, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+342);
  if (!(F & FZ)) { RET_TAKEN(b_+342); return; }
  CYC(b_+342, b_+343);
  CYC(b_+343, b_+345); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+345, b_+347); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+347, b_+349); goto L_6eb4; }
  CYC(b_+347, b_+349);
  CYC(b_+349, b_+350); L = E;
  CYC(b_+350, b_+351); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+351); return;
L_6eb4:
  CYC(b_+352, b_+354); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+354, b_+356); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+356, b_+358); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+358, b_+360); mem_wr(gb, HL, 0x09);
  CYC(b_+360, b_+362); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+362, b_+364); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+364); return; }
  CYC(b_+364, b_+365);
  CYC(b_+365, b_+366); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+366, b_+369); SET_BC(0x3f06);
  CYC(b_+369, b_+372);
  TAIL(showText);
substate2:
  CYC(b_+372, b_+374); A = 0x04;
  CALL_C(b_+374, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+377);
  CYC(b_+377, b_+378); A = mem_rd(gb, HL);
  CYC(b_+378, b_+380); alu_cp(gb, 0x05);
  if (!(F & FZ)) { RET_TAKEN(b_+380); return; }
  CYC(b_+380, b_+381);
  CYC(b_+381, b_+382); H = D;
  CYC(b_+382, b_+384); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+384, b_+385); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+385, b_+386); L = alu_inc8(gb, L);
  CYC(b_+386, b_+388); mem_wr(gb, HL, 0x01);
  CYC(b_+388, b_+389); L = alu_inc8(gb, L);
  CYC(b_+389, b_+391); mem_wr(gb, HL, 0x06);
  RET(b_+391); return;
substate3:
  CALL_C(b_+392, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+395);
  if (!(F & FZ)) { RET_TAKEN(b_+395); return; }
  CYC(b_+395, b_+396);
  CYC(b_+396, b_+397); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+397, b_+398); L = alu_inc8(gb, L);
  CYC(b_+398, b_+399); A = mem_rd(gb, HL);
  CYC(b_+399, b_+400); A = alu_dec8(gb, A);
  CYC(b_+400, b_+403); SET_HL(b_+447 /* @table_6f13 */);
  CYC(b_+403, b_+404); kingMoblin_add_double_index(gb, b_+404);
  CYC(b_+404, b_+405); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+405, b_+406); C = mem_rd(gb, HL);
  CYC(b_+406, b_+407); B = A;
  CALL_C(b_+407, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+410);
  if (!(F & FZ)) { RET_TAKEN(b_+410); return; }
  CYC(b_+410, b_+411);
  CYC(b_+411, b_+413); mem_wr(gb, HL, 0x56);
  CYC(b_+413, b_+415); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+415, b_+416); mem_wr(gb, HL, B);
  CYC(b_+416, b_+418); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+418, b_+419); mem_wr(gb, HL, C);
  CYC(b_+419, b_+420); A = C;
  CYC(b_+420, b_+422); alu_and(gb, 0xf0);
  CYC(b_+422, b_+424); A = alu_swap(gb, A);
  CYC(b_+424, b_+425); C = A;
  CYC(b_+425, b_+427); A = 0xac;
  CALL_C(b_+427, s_setTile, SYM(setTile), b_+430);
  CALL_C(b_+430, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+433);
  CYC(b_+433, b_+435); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+435, b_+437); mem_wr(gb, HL, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+437); return; }
  CYC(b_+437, b_+438);
  CYC(b_+438, b_+440); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+440, b_+441); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+441, b_+443); A = 0x01;
  CYC(b_+443, b_+446); mem_wr(gb, wTmpcfc0, A);
  RET(b_+446); return;

substateStub:
  RET(b_+459); return;
}

void s_func_6f20_hook(GB *gb) {
  BASE(func_6f20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  CYC(b_+1, b_+4); SET_HL(SYM(table_6f20));
  CYC(b_+4, b_+5); kingMoblin_add_a_to_hl(gb, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  RET(b_+9); return;
}

void s_func_6f2e_hook(GB *gb) {
  BASE(func_6f2e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+7); SET_HL(SYM(table_6f3b));
  CYC(b_+7, b_+8); kingMoblin_add_a_to_hl(gb, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  RET(b_+12); return;
}

void s_func_6f40_hook(GB *gb) {
  BASE(func_6f40);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_enemyAnimate, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_6f5b; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0x80;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); A = 0x04;
  CALL_C(b_+17, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+20);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x03);
  CYC(b_+22, b_+24); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+24, b_+26); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+26); return;
L_6f5b:
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+33); SET_HL(SYM(table_6f6f));
  CYC(b_+33, b_+34); kingMoblin_add_double_index(gb, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+35, b_+36); B = A;
  CYC(b_+36, b_+37); C = mem_rd(gb, HL);
  CYC(b_+37, b_+39); A = 0x0b;
  CALL_C(b_+39, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+42);
  CALL_C(b_+42, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+45);
  CYC(b_+45, b_+46); alu_or(gb, D);
  RET(b_+46); return;
}

