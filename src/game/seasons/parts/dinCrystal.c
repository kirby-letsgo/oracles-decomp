#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/dinCrystal.s.
// PART_DIN_CRYSTAL

static uint16_t dinCrystal_jump_table(GB *gb) {
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

// PART_DIN_CRYSTAL
void s_partCode4f_hook(GB *gb) {
  BASE(partCode4f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_STATE;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto normalStatus; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); A = A & 0x7f;
  CYC(b_+14, b_+16); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+16, b_+18); goto normalStatus; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+20, b_+23); TAIL_S(seasonsFunc_10_7bc2); }
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+25); alu_cp(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto L_79e1; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); A = 0x24;
  CALL_C(b_+29, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+34, b_+36); E = PART_BASE + OBJ_VAR33;
  CYC(b_+36, b_+38); A = 0x01;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
L_79e1:
  CYC(b_+39, b_+40); H = D;
  CYC(b_+40, b_+42); L = PART_BASE + OBJ_HEALTH;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x40);
  CYC(b_+44, b_+46); L = PART_BASE + OBJ_VAR32;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x3c);
normalStatus:
  CYC(b_+48, b_+50); E = PART_BASE + OBJ_SUBID;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+53); E = PART_BASE + OBJ_STATE;
  CYC(b_+53, b_+54); push_effect(gb, b_+54);
  do { uint16_t jt_ = (dinCrystal_jump_table(gb));
    if (jt_ == b_+58) goto subid0;
    if (jt_ == b_+490) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+58, b_+59); H = D;
  CYC(b_+59, b_+61); L = PART_BASE + OBJ_VAR32;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+63); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+63, b_+65); goto L_7a02; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+66); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+66, b_+68); goto L_7a02; }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+70); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+70, b_+72); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
L_7a02:
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+74); push_effect(gb, b_+74);
  do { uint16_t jt_ = (dinCrystal_jump_table(gb));
    if (jt_ == b_+90) goto state0;
    if (jt_ == b_+132) goto state1;
    if (jt_ == b_+232) goto stateStub;
    if (jt_ == b_+233) goto state3;
    if (jt_ == b_+255) goto state4;
    if (jt_ == b_+355) goto state5;
    if (jt_ == b_+380) goto state6;
    if (jt_ == b_+431) goto state7;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+90, s_getFreePartSlot, SYM(getFreePartSlot), b_+93);
  if (!(F & FZ)) { RET_TAKEN(b_+93); return; }
  CYC(b_+93, b_+94);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x07);
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x00);
  CYC(b_+99, b_+100); L = alu_inc8(gb, L);
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x08);
  CYC(b_+102, b_+104); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+104, b_+106); A = 0xc0;
  CYC(b_+106, b_+107); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+107, b_+108); mem_wr(gb, HL, D);
  CYC(b_+108, b_+110); A = 0x0f;
  CYC(b_+110, b_+113); mem_wr(gb, wLinkForceState, A);
  CYC(b_+113, b_+114); H = D;
  CYC(b_+114, b_+116); L = PART_BASE + OBJ_STATE;
  CYC(b_+116, b_+117); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+117, b_+119); L = PART_BASE + OBJ_YH;
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x50);
  CYC(b_+121, b_+123); L = PART_BASE + OBJ_XH;
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x78);
  CYC(b_+125, b_+127); L = PART_BASE + OBJ_ZH;
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0xfc);
  CYC(b_+129, b_+132);
  TAIL(objectSetVisible82);
state1:
  CYC(b_+132, b_+133); E = alu_inc8(gb, E);
  CYC(b_+133, b_+134); A = mem_rd(gb, DE);
  CYC(b_+134, b_+135); push_effect(gb, b_+135);
  do { uint16_t jt_ = (dinCrystal_jump_table(gb));
    if (jt_ == b_+141) goto state1_substate0;
    if (jt_ == b_+157) goto state1_substate1;
    if (jt_ == b_+200) goto state1_substate2;
    HANDOFF(HL);
  } while (0);
state1_substate0:
  CYC(b_+141, b_+144); A = mem_rd(gb, w1Link_yh);
  CYC(b_+144, b_+146); alu_cp(gb, 0x78);
  if (!(F & FC)) { CYCT(b_+146, b_+149); TAIL(partAnimate); }
  CYC(b_+146, b_+149);
  CYC(b_+149, b_+151); A = 0x01;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CYC(b_+152, b_+154); A = 0x8d;
  CALL_C(b_+154, s_playSound, SYM(playSound_b00), b_+157);
state1_substate1:
  CYC(b_+157, b_+159); B = 0x04;
  CALL_C(b_+159, s_checkBPartSlotsAvailable, SYM(checkBPartSlotsAvailable), b_+162);
  if (!(F & FZ)) { RET_TAKEN(b_+162); return; }
  CYC(b_+162, b_+163);
  CYC(b_+163, b_+166); SET_BC(0x0404);
L_7a60:
  CALL_C(b_+166, s_getFreePartSlot, SYM(getFreePartSlot), b_+169);
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x4f);
  CYC(b_+171, b_+172); L = alu_inc8(gb, L);
  CYC(b_+172, b_+173); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+173, b_+175); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+175, b_+176); mem_wr(gb, HL, C);
  CALL_C(b_+176, s_objectCopyPosition, SYM(objectCopyPosition), b_+179);
  CYC(b_+179, b_+180); A = C;
  CYC(b_+180, b_+182); alu_add(gb, 0x08);
  CYC(b_+182, b_+183); C = A;
  CYC(b_+183, b_+184); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+184, b_+186); goto L_7a60; }
  CYC(b_+184, b_+186);
  CYC(b_+186, b_+187); H = D;
  CYC(b_+187, b_+189); L = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+189, b_+190); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+190, b_+191); L = alu_inc8(gb, L);
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x5a);
  CYC(b_+193, b_+195); L = PART_BASE + OBJ_ZH;
  CYC(b_+195, b_+197); mem_wr(gb, HL, 0x00);
  CYC(b_+197, b_+200);
  TAIL(objectSetInvisible);
state1_substate2:
  CALL_C(b_+200, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+203);
  if (!(F & FZ)) { RET_TAKEN(b_+203); return; }
  CYC(b_+203, b_+204);
  CALL_C(b_+204, s_getFreeEnemySlot, SYM(getFreeEnemySlot), b_+207);
  if (!(F & FZ)) { RET_TAKEN(b_+207); return; }
  CYC(b_+207, b_+208);
  CYC(b_+208, b_+210); mem_wr(gb, HL, 0x02);
  CYC(b_+210, b_+212); E = PART_BASE + OBJ_RELATED2;
  CYC(b_+212, b_+214); A = 0x80;
  CYC(b_+214, b_+215); mem_wr(gb, DE, A);
  CYC(b_+215, b_+216); E = alu_inc8(gb, E);
  CYC(b_+216, b_+217); A = H;
  CYC(b_+217, b_+218); mem_wr(gb, DE, A);
  CYC(b_+218, b_+220); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+220, b_+222); A = 0xc0;
  CYC(b_+222, b_+223); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+223, b_+224); mem_wr(gb, HL, D);
  CYC(b_+224, b_+225); H = D;
  CYC(b_+225, b_+227); L = PART_BASE + OBJ_STATE;
  CYC(b_+227, b_+228); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+228, b_+229); L = alu_inc8(gb, L);
  CYC(b_+229, b_+231); mem_wr(gb, HL, 0x00);
  RET(b_+231); return;
stateStub:
  RET(b_+232); return;
state3:
  CYC(b_+233, b_+234); H = D;
  CYC(b_+234, b_+236); L = PART_BASE + OBJ_ZH;
  CYC(b_+236, b_+237); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+237, b_+238); A = mem_rd(gb, HL);
  CYC(b_+238, b_+240); alu_cp(gb, 0xfc);
  if (F & FC) { CYCT(b_+240, b_+242); goto animate; }
  CYC(b_+240, b_+242);
  CYC(b_+242, b_+243); L = E;
  CYC(b_+243, b_+244); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+244, b_+246); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+246, b_+248); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+248, b_+250); L = PART_BASE + OBJ_SPEED;
  CYC(b_+250, b_+252); mem_wr(gb, HL, 0x14);
animate:
  CYC(b_+252, b_+255);
  TAIL(partAnimate);
state4:
  CYC(b_+255, b_+257); A = 0x01;
  CALL_C(b_+257, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+260);
  CYC(b_+260, b_+261); A = mem_rd(gb, HL);
  CYC(b_+261, b_+263); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+263, b_+265); goto L_7b0b; }
  CYC(b_+263, b_+265);
  CALL_C(b_+265, s_seasonsFunc_10_7bd6_hook, SYM(seasonsFunc_10_7bd6), b_+268);
  CYC(b_+268, b_+270); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+270, b_+271); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+271, b_+273); A = alu_srl(gb, A);
  CYC(b_+273, b_+274); B = A;
  CYC(b_+274, b_+277); A = mem_rd(gb, w1Link_yh);
  CYC(b_+277, b_+279); A = alu_srl(gb, A);
  CYC(b_+279, b_+280); alu_add(gb, B);
  CYC(b_+280, b_+281); B = A;
  CYC(b_+281, b_+282); L = alu_inc8(gb, L);
  CYC(b_+282, b_+283); A = mem_rd(gb, HL);
  CYC(b_+283, b_+285); A = alu_srl(gb, A);
  CYC(b_+285, b_+286); C = A;
  CYC(b_+286, b_+289); A = mem_rd(gb, w1Link_xh);
  CYC(b_+289, b_+291); A = alu_srl(gb, A);
  CYC(b_+291, b_+292); alu_add(gb, C);
  CYC(b_+292, b_+293); C = A;
  CYC(b_+293, b_+295); E = PART_BASE + OBJ_XH;
  CYC(b_+295, b_+296); A = mem_rd(gb, DE);
  CYC(b_+296, b_+298); mem_wr(gb, hFF8E, A);
  CYC(b_+298, b_+300); E = PART_BASE + OBJ_YH;
  CYC(b_+300, b_+301); A = mem_rd(gb, DE);
  CYC(b_+301, b_+303); mem_wr(gb, hFF8F, A);
  CYC(b_+303, b_+304); alu_sub(gb, B);
  CYC(b_+304, b_+306); alu_add(gb, 0x04);
  CYC(b_+306, b_+308); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+308, b_+310); goto L_7b00; }
  CYC(b_+308, b_+310);
  CYC(b_+310, b_+312); A = mem_rd(gb, hFF8E);
  CYC(b_+312, b_+313); alu_sub(gb, C);
  CYC(b_+313, b_+315); alu_add(gb, 0x04);
  CYC(b_+315, b_+317); alu_cp(gb, 0x09);
  if (F & FC) { CYCT(b_+317, b_+319); goto animate; }
  CYC(b_+317, b_+319);
  CYC(b_+319, b_+322); A = mem_rd(gb, wFrameCounter);
  CYC(b_+322, b_+324); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(b_+324, b_+326); goto L_7b06; }
  CYC(b_+324, b_+326);
L_7b00:
  CALL_C(b_+326, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+329);
  CYC(b_+329, b_+331); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+331, b_+332); mem_wr(gb, DE, A);
L_7b06:
  CALL_C(b_+332, s_objectApplySpeed, SYM(objectApplySpeed), b_+335);
  CYC(b_+335, b_+337);
  goto animate;
L_7b0b:
  CYC(b_+337, b_+338); H = D;
  CYC(b_+338, b_+340); L = PART_BASE + OBJ_STATE;
  CYC(b_+340, b_+341); E = L;
  CYC(b_+341, b_+343); mem_wr(gb, HL, 0x06);
  CYC(b_+343, b_+344); L = alu_inc8(gb, L);
  CYC(b_+344, b_+346); mem_wr(gb, HL, 0x00);
  CYC(b_+346, b_+348); L = PART_BASE + OBJ_VAR32;
  CYC(b_+348, b_+350); mem_wr(gb, HL, 0x00);
  CYC(b_+350, b_+352); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+352, b_+354); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  RET(b_+354); return;
state5:
  CALL_C(b_+355, s_seasonsFunc_10_7bd6_hook, SYM(seasonsFunc_10_7bd6), b_+358);
  CALL_C(b_+358, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+361);
  if (F & FZ) { CYCT(b_+361, b_+363); goto L_7b2d; }
  CYC(b_+361, b_+363);
  CALL_C(b_+363, s_objectCheckTileCollision_allowHoles, SYM(objectCheckTileCollision_allowHoles), b_+366);
  if (!(F & FC)) CALL_C_CC(b_+366, s_objectApplySpeed, SYM(objectApplySpeed), b_+369);
  else CYC(b_+366, b_+369);
  CYC(b_+369, b_+371);
  goto animate;
L_7b2d:
  CYC(b_+371, b_+373); L = PART_BASE + OBJ_STATE;
  CYC(b_+373, b_+374); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+374, b_+376); L = PART_BASE + OBJ_SPEED;
  CYC(b_+376, b_+378); mem_wr(gb, HL, 0x14);
  CYC(b_+378, b_+380);
  goto animate;
state6:
  CYC(b_+380, b_+381); E = alu_inc8(gb, E);
  CYC(b_+381, b_+382); A = mem_rd(gb, DE);
  CYC(b_+382, b_+383); push_effect(gb, b_+383);
  do { uint16_t jt_ = (dinCrystal_jump_table(gb));
    if (jt_ == b_+391) goto state6_substate0;
    if (jt_ == b_+401) goto state6_substate1;
    if (jt_ == b_+157) goto state1_substate1;
    if (jt_ == b_+419) goto state6_substate3;
    HANDOFF(HL);
  } while (0);
state6_substate0:
  CYC(b_+391, b_+392); H = D;
  CYC(b_+392, b_+393); L = E;
  CYC(b_+393, b_+394); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+394, b_+396); L = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+396, b_+398); A = 0x10;
  CYC(b_+398, b_+399); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+399, b_+400); mem_wr(gb, HL, A);
  RET(b_+400); return;
state6_substate1:
  CALL_C(b_+401, s_objectCheckCollidedWithLink, SYM(objectCheckCollidedWithLink), b_+404);
  if (!(F & FC)) { CYCT(b_+404, b_+406); goto L_7b5a; }
  CYC(b_+404, b_+406);
  CYC(b_+406, b_+408); E = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+408, b_+410); A = 0x02;
  CYC(b_+410, b_+411); mem_wr(gb, DE, A);
  CYC(b_+411, b_+413); A = 0x8d;
  CALL_C(b_+413, s_playSound, SYM(playSound_b00), b_+416);
L_7b5a:
  CYC(b_+416, b_+419);
  TAIL(partAnimate);
state6_substate3:
  CYC(b_+419, b_+420); H = D;
  CYC(b_+420, b_+422); L = PART_BASE + OBJ_STATE;
  CYC(b_+422, b_+423); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+423, b_+425); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+425, b_+427); A = 0x3c;
  CYC(b_+427, b_+428); mem_wr(gb, HL, A);
  CALL_C(b_+428, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+431);
state7:
  CALL_C(b_+431, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+434);
  if (!(F & FZ)) { RET_TAKEN(b_+434); return; }
  CYC(b_+434, b_+435);
  CYC(b_+435, b_+437); A = 0x0f;
  CYC(b_+437, b_+438); mem_wr(gb, HL, A);
  CALL_C(b_+438, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+441);
L_7b73:
  CALL_C(b_+441, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+444);
  CYC(b_+444, b_+446); alu_and(gb, 0x0f);
  CYC(b_+446, b_+448); alu_cp(gb, 0x0d);
  if (!(F & FC)) { CYCT(b_+448, b_+450); goto L_7b73; }
  CYC(b_+448, b_+450);
  CYC(b_+450, b_+451); A = alu_inc8(gb, A);
  CYC(b_+451, b_+452); C = A;
  CYC(b_+452, b_+453); push_effect(gb, BC);
L_7b7f:
  CALL_C(b_+453, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+456);
  CYC(b_+456, b_+458); alu_and(gb, 0x0f);
  CYC(b_+458, b_+460); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+460, b_+462); goto L_7b7f; }
  CYC(b_+460, b_+462);
  CYC(b_+462, b_+463); SET_BC(pop_effect(gb));
  CYC(b_+463, b_+464); A = alu_inc8(gb, A);
  CYC(b_+464, b_+466); A = alu_swap(gb, A);
  CYC(b_+466, b_+467); alu_or(gb, C);
  CYC(b_+467, b_+468); C = A;
  CYC(b_+468, b_+470); B = 0xce;
  CYC(b_+470, b_+471); A = mem_rd(gb, BC);
  CYC(b_+471, b_+472); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+472, b_+474); goto L_7b73; }
  CYC(b_+472, b_+474);
  CYC(b_+474, b_+476); A = 0x48;
  CALL_C(b_+476, s_breakCrackedFloor, SYM(breakCrackedFloor), b_+479);
  CYC(b_+479, b_+481); E = PART_BASE + OBJ_COUNTER2;
  CYC(b_+481, b_+482); A = mem_rd(gb, DE);
  CYC(b_+482, b_+483); A = alu_inc8(gb, A);
  CYC(b_+483, b_+485); alu_cp(gb, 0x75);
  if (F & FZ) { CYCT(b_+485, b_+488); TAIL(partDelete); }
  CYC(b_+485, b_+488);
  CYC(b_+488, b_+489); mem_wr(gb, DE, A);
  RET(b_+489); return;
subid1:
  CYC(b_+490, b_+491); A = mem_rd(gb, DE);
  CYC(b_+491, b_+492); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+492, b_+494); goto L_7bb3; }
  CYC(b_+492, b_+494);
  CYC(b_+494, b_+495); H = D;
  CYC(b_+495, b_+496); L = E;
  CYC(b_+496, b_+497); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+497, b_+499); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+499, b_+501); mem_wr(gb, HL, 0x5a);
  CYC(b_+501, b_+503); L = PART_BASE + OBJ_SPEED;
  CYC(b_+503, b_+505); mem_wr(gb, HL, 0x0f);
L_7bb3:
  CALL_C(b_+505, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+508);
  if (F & FZ) { CYCT(b_+508, b_+511); TAIL(partDelete); }
  CYC(b_+508, b_+511);
  CYC(b_+511, b_+513); L = PART_BASE + OBJ_VISIBLE;
  CYC(b_+513, b_+514); A = mem_rd(gb, HL);
  CYC(b_+514, b_+516); alu_xor(gb, 0x80);
  CYC(b_+516, b_+517); mem_wr(gb, HL, A);
  CYC(b_+517, b_+520);
  TAIL(objectApplySpeed);
}

void s_seasonsFunc_10_7bc2_hook(GB *gb) {
  BASE(seasonsFunc_10_7bc2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x78);
  CYC(b_+5, b_+7); L = PART_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_STATE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x05);
  CYC(b_+15, b_+17); L = PART_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x50);
  RET(b_+19); return;
}

void s_seasonsFunc_10_7bd6_hook(GB *gb) {
  BASE(seasonsFunc_10_7bd6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_VAR33;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wIsLinkBeingShocked);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x29;
  CALL_C(b_+13, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+21, b_+23); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+23); return;
}

