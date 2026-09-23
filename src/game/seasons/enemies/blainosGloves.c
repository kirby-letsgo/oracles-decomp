#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/blainosGloves.s.
// ENEMY_BLAINOS_GLOVES

static uint16_t blainosGloves_jump_table(GB *gb) {
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

static void blainosGloves_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void blainosGloves_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// enemyCode5f@seasonsFunc_0d_76fa
static void blainosGloves_seasonsFunc_0d_76fa(GB *gb) {
  BASE(enemyCode5f);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+137, b_+138); H = B;
  CYC(b_+138, b_+141);
  TAIL_S(seasonsFunc_0d_7883);
}

// ENEMY_BLAINOS_GLOVES
void s_enemyCode5f_hook(GB *gb) {
  BASE(enemyCode5f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_76a6; }
  CYC(b_+0, b_+2);
  CALL_C(b_+2, s_seasonsFunc_0d_7986_hook, SYM(seasonsFunc_0d_7986), b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_76a6; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); A = A & 0x7f;
  CYC(b_+17, b_+19); alu_sub(gb, 0x0a);
  CYC(b_+19, b_+21); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto L_76a6; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x02);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x1e);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+38, b_+39); alu_xor(gb, A);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CYC(b_+41, b_+43); A = 0x2b;
  CALL_C(b_+43, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+46);
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0xf8);
  CYC(b_+48, b_+50); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+50, b_+51); alu_xor(gb, A);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
L_76a6:
  CALL_C(b_+53, s_seasonsFunc_0d_785c_hook, SYM(seasonsFunc_0d_785c), b_+56);
  CYC(b_+56, b_+58); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); push_effect(gb, b_+60);
  do { uint16_t jt_ = (blainosGloves_jump_table(gb));
    if (jt_ == b_+68) goto state0;
    if (jt_ == b_+71) goto state1;
    if (jt_ == b_+91) goto state2;
    if (jt_ == b_+125) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+68, b_+70); A = 0x01;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
state1:
  CALL_C(b_+71, s_seasonsFunc_0d_786d_hook, SYM(seasonsFunc_0d_786d), b_+74);
  CYC(b_+74, b_+75); B = H;
  CYC(b_+75, b_+77); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+77, b_+78); A = mem_rd(gb, HL);
  CYC(b_+78, b_+79); push_effect(gb, b_+79);
  do { uint16_t jt_ = (blainosGloves_jump_table(gb));
    if (jt_ == b_+137) goto seasonsFunc_0d_76fa;
    if (jt_ == b_+141) goto state1zh2;
    if (jt_ == b_+202) goto state1zh3;
    if (jt_ == b_+317) goto state1zh4;
    if (jt_ == b_+414) goto state1zh5;
    HANDOFF(HL);
  } while (0);
state2:
  CALL_C(b_+91, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+94);
  if (F & FZ) { CYCT(b_+94, b_+96); goto L_76de; }
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); A = 0x2e;
  CALL_C(b_+98, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+101);
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x02);
  CYC(b_+103, b_+105); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x3b);
  CYC(b_+107, b_+109);
  goto L_76eb;
L_76de:
  CYC(b_+109, b_+110); L = E;
  CYC(b_+110, b_+111); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+111, b_+113); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+113, b_+115); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+115, b_+117); A = 0x25;
  CALL_C(b_+117, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+120);
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x56);
L_76eb:
  CYC(b_+122, b_+125);
  TAIL_S(seasonsFunc_0d_796d);
state3:
  CYC(b_+125, b_+127); A = 0x2b;
  CALL_C(b_+127, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+130);
  CYC(b_+130, b_+132); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+132, b_+133); A = mem_rd(gb, HL);
  CYC(b_+133, b_+134); mem_wr(gb, DE, A);
  CYC(b_+134, b_+137);
  TAIL_S(seasonsFunc_0d_796d);
seasonsFunc_0d_76fa:
  CYC(b_+137, b_+138); H = B;
  CYC(b_+138, b_+141);
  TAIL_S(seasonsFunc_0d_7883);
state1zh2:
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+143); push_effect(gb, b_+143);
  do { uint16_t jt_ = (blainosGloves_jump_table(gb));
    if (jt_ == b_+147) goto state1zh2_substate0;
    if (jt_ == b_+154) goto state1zh2_substate1;
    HANDOFF(HL);
  } while (0);
state1zh2_substate0:
  CYC(b_+147, b_+148); H = D;
  CYC(b_+148, b_+149); L = E;
  CYC(b_+149, b_+150); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+150, b_+152); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+152, b_+154); mem_wr(gb, HL, 0x57);
state1zh2_substate1:
  CALL_L(b_+154, blainosGloves_seasonsFunc_0d_76fa, b_+157);
  CYC(b_+157, b_+159); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+159, b_+160); A = mem_rd(gb, HL);
  CYC(b_+160, b_+161); alu_add(gb, A);
  CYC(b_+161, b_+162); alu_add(gb, A);
  CYC(b_+162, b_+163); B = A;
  CYC(b_+163, b_+166); A = mem_rd(gb, wFrameCounter);
  CYC(b_+166, b_+168); alu_and(gb, 0x04);
  CYC(b_+168, b_+169); alu_rrca(gb);
  CYC(b_+169, b_+170); alu_add(gb, B);
  CYC(b_+170, b_+173); SET_HL(b_+186 /* @seasonsTable_0d_772b */);
  CYC(b_+173, b_+174); blainosGloves_add_a_to_hl(gb, b_+174);
  CYC(b_+174, b_+176); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+178); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+178, b_+179); mem_wr(gb, DE, A);
  CYC(b_+179, b_+180); SET_HL(HL + 1);
  CYC(b_+180, b_+182); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+182, b_+183); A = mem_rd(gb, DE);
  CYC(b_+183, b_+184); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+184, b_+185); mem_wr(gb, DE, A);
  RET(b_+185); return;

state1zh3:
  CYC(b_+202, b_+203); A = mem_rd(gb, DE);
  CYC(b_+203, b_+204); push_effect(gb, b_+204);
  do { uint16_t jt_ = (blainosGloves_jump_table(gb));
    if (jt_ == b_+216) goto state1zh3_substate0;
    if (jt_ == b_+248) goto state1zh3_substate1;
    if (jt_ == b_+258) goto state1zh3_substate2;
    if (jt_ == b_+278) goto state1zh3_substate3;
    if (jt_ == b_+288) goto state1zh3_substate4;
    if (jt_ == b_+308) goto state1zh3_substate5;
    HANDOFF(HL);
  } while (0);
state1zh3_substate0:
  CYC(b_+216, b_+217); H = D;
  CYC(b_+217, b_+218); L = E;
  CYC(b_+218, b_+219); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+219, b_+221); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+221, b_+223); mem_wr(gb, HL, 0x04);
  CYC(b_+223, b_+225); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+225, b_+227); mem_wr(gb, HL, 0x0a);
  CYC(b_+227, b_+229); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+229, b_+231); mem_wr(gb, HL, 0x1e);
  CALL_L(b_+231, blainosGloves_seasonsFunc_0d_76fa, b_+234);
  CYC(b_+234, b_+236); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+236, b_+237); A = mem_rd(gb, HL);
  CYC(b_+237, b_+238); alu_rrca(gb);
  CYC(b_+238, b_+240); A = alu_swap(gb, A);
  CYC(b_+240, b_+242); alu_xor(gb, 0x10);
  CYC(b_+242, b_+244); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+244, b_+245); mem_wr(gb, DE, A);
  CALL_C(b_+245, s_seasonsFunc_0d_78ce_hook, SYM(seasonsFunc_0d_78ce), b_+248);
state1zh3_substate1:
  CALL_C(b_+248, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+251);
  if (!(F & FZ)) { CYCT(b_+251, b_+254); TAIL_S(seasonsFunc_0d_78bc); }
  CYC(b_+251, b_+254);
  CYC(b_+254, b_+256); mem_wr(gb, HL, 0x04);
  CYC(b_+256, b_+257); L = E;
  CYC(b_+257, b_+258); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
state1zh3_substate2:
  CALL_C(b_+258, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+261);
  if (!(F & FZ)) { CYCT(b_+261, b_+264); TAIL_S(seasonsFunc_0d_78ab); }
  CYC(b_+261, b_+264);
  CYC(b_+264, b_+266); mem_wr(gb, HL, 0x0a);
  CYC(b_+266, b_+267); L = E;
  CYC(b_+267, b_+268); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+268, b_+270); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+270, b_+272); mem_wr(gb, HL, 0x78);
  CYC(b_+272, b_+274); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+274, b_+275); A = mem_rd(gb, HL);
  CYC(b_+275, b_+277); alu_xor(gb, 0x10);
  CYC(b_+277, b_+278); mem_wr(gb, HL, A);
state1zh3_substate3:
  CALL_C(b_+278, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+281);
  if (!(F & FZ)) { CYCT(b_+281, b_+284); TAIL_S(seasonsFunc_0d_78bc); }
  CYC(b_+281, b_+284);
  CYC(b_+284, b_+286); mem_wr(gb, HL, 0x14);
  CYC(b_+286, b_+287); L = E;
  CYC(b_+287, b_+288); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
state1zh3_substate4:
  CALL_C(b_+288, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+291);
  if (!(F & FZ)) { CYCT(b_+291, b_+294); TAIL_S(seasonsFunc_0d_78ab); }
  CYC(b_+291, b_+294);
  CYC(b_+294, b_+296); mem_wr(gb, HL, 0x14);
  CYC(b_+296, b_+297); L = E;
  CYC(b_+297, b_+298); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+298, b_+300); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+300, b_+302); mem_wr(gb, HL, 0x28);
  CYC(b_+302, b_+304); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+304, b_+305); A = mem_rd(gb, HL);
  CYC(b_+305, b_+307); alu_xor(gb, 0x10);
  CYC(b_+307, b_+308); mem_wr(gb, HL, A);
state1zh3_substate5:
  CALL_C(b_+308, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+311);
  if (!(F & FZ)) { CYCT(b_+311, b_+314); TAIL_S(seasonsFunc_0d_78bc); }
  CYC(b_+311, b_+314);
  CYC(b_+314, b_+317);
  goto seasonsFunc_0d_76fa;
state1zh4:
  CYC(b_+317, b_+318); A = mem_rd(gb, DE);
  CYC(b_+318, b_+319); push_effect(gb, b_+319);
  do { uint16_t jt_ = (blainosGloves_jump_table(gb));
    if (jt_ == b_+327) goto state1zh4_substate0;
    if (jt_ == b_+334) goto state1zh4_substate1;
    if (jt_ == b_+376) goto state1zh4_substate2;
    if (jt_ == b_+401) goto state1zh4_substate3;
    HANDOFF(HL);
  } while (0);
state1zh4_substate0:
  CYC(b_+327, b_+328); H = D;
  CYC(b_+328, b_+329); L = E;
  CYC(b_+329, b_+330); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+330, b_+332); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+332, b_+334); mem_wr(gb, HL, 0x57);
state1zh4_substate1:
  CALL_L(b_+334, blainosGloves_seasonsFunc_0d_76fa, b_+337);
  CYC(b_+337, b_+339); A = 0x05;
  CALL_C(b_+339, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+342);
  CYC(b_+342, b_+343); A = mem_rd(gb, HL);
  CYC(b_+343, b_+345); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+345); return; }
  CYC(b_+345, b_+346);
  CYC(b_+346, b_+348); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+348, b_+350); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+350, b_+351); A = mem_rd(gb, HL);
  CYC(b_+351, b_+352); mem_wr(gb, DE, A);
  CALL_C(b_+352, s_seasonsFunc_0d_78ce_hook, SYM(seasonsFunc_0d_78ce), b_+355);
  CYC(b_+355, b_+358); SET_BC(hOamFunc);
  CALL_C(b_+358, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+361);
  CYC(b_+361, b_+363); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+363, b_+365); mem_wr(gb, HL, 0x58);
  CYC(b_+365, b_+367); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+367, b_+368); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+368, b_+370); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+370, b_+372); mem_wr(gb, HL, 0x0a);
  CYC(b_+372, b_+374); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+374, b_+376); mem_wr(gb, HL, 0x50);
state1zh4_substate2:
  CALL_C(b_+376, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+379);
  if (!(F & FZ)) { CYCT(b_+379, b_+381); goto L_77fc; }
  CYC(b_+379, b_+381);
  CYC(b_+381, b_+383); mem_wr(gb, HL, 0x08);
  CYC(b_+383, b_+384); L = E;
  CYC(b_+384, b_+385); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+385, b_+387); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x64);
  CYC(b_+389, b_+391); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+391, b_+392); A = mem_rd(gb, HL);
  CYC(b_+392, b_+394); alu_xor(gb, 0x10);
  CYC(b_+394, b_+395); mem_wr(gb, HL, A);
L_77fc:
  CALL_C(b_+395, s_seasonsFunc_0d_78bc_hook, SYM(seasonsFunc_0d_78bc), b_+398);
  CYC(b_+398, b_+401);
  TAIL_S(seasonsFunc_0d_78e1);
state1zh4_substate3:
  CALL_C(b_+401, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+404);
  if (F & FZ) { CYCT(b_+404, b_+407); goto seasonsFunc_0d_76fa; }
  CYC(b_+404, b_+407);
  CYC(b_+407, b_+409); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+409, b_+410); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+410, b_+411); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+411, b_+414);
  TAIL_S(seasonsFunc_0d_78bc);
state1zh5:
  CYC(b_+414, b_+415); A = mem_rd(gb, DE);
  CYC(b_+415, b_+416); push_effect(gb, b_+416);
  do { uint16_t jt_ = (blainosGloves_jump_table(gb));
    if (jt_ == b_+422) goto state1zh5_substate0;
    if (jt_ == b_+433) goto state1zh5_substate1;
    if (jt_ == b_+469) goto state1zh5_substate2;
    HANDOFF(HL);
  } while (0);
state1zh5_substate0:
  CYC(b_+422, b_+423); H = D;
  CYC(b_+423, b_+425); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+425, b_+426); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+426, b_+428); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+428, b_+430); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+430, b_+431); L = alu_inc8(gb, L);
  CYC(b_+431, b_+433); mem_wr(gb, HL, 0x58);
state1zh5_substate1:
  CYC(b_+433, b_+435); A = 0x05;
  CALL_C(b_+435, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+438);
  CYC(b_+438, b_+439); A = mem_rd(gb, HL);
  CYC(b_+439, b_+441); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+441, b_+444); TAIL_S(seasonsFunc_0d_78f8); }
  CYC(b_+441, b_+444);
  CYC(b_+444, b_+445); H = D;
  CYC(b_+445, b_+447); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+447, b_+449); mem_wr(gb, HL, 0x1e);
  CYC(b_+449, b_+451); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+451, b_+453); A = 0x08;
  CYC(b_+453, b_+454); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+454, b_+455); mem_wr(gb, HL, A);
  CYC(b_+455, b_+457); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+457, b_+459); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+459, b_+461); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+461, b_+462); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+462, b_+464); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+464, b_+466); A = 0xc0;
  CYC(b_+466, b_+467); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+467, b_+469); mem_wr(gb, HL, 0xfb);
state1zh5_substate2:
  CALL_C(b_+469, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+472);
  if (F & FZ) { CYCT(b_+472, b_+474); goto L_7851; }
  CYC(b_+472, b_+474);
  CYC(b_+474, b_+476); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+476, b_+478); A = 0x06;
  CYC(b_+478, b_+479); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+479, b_+480); mem_wr(gb, HL, A);
L_7851:
  CALL_C(b_+480, s_seasonsFunc_0d_7915_hook, SYM(seasonsFunc_0d_7915), b_+483);
  CALL_C(b_+483, s_seasonsFunc_0d_78f8_hook, SYM(seasonsFunc_0d_78f8), b_+486);
  CYC(b_+486, b_+488); C = 0x34;
  CYC(b_+488, b_+491);
  TAIL(objectUpdateSpeedZ_paramC);
}

void s_seasonsFunc_0d_785c_hook(GB *gb) {
  BASE(seasonsFunc_0d_785c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CALL_C(b_+7, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x54);
  if (F & FZ) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+17);
  TAIL(enemyDelete);
}

void s_seasonsFunc_0d_786d_hook(GB *gb) {
  BASE(seasonsFunc_0d_786d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x31;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+14, b_+16); A = 0x40;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return;
}

void s_seasonsFunc_0d_7883_hook(GB *gb) {
  BASE(seasonsFunc_0d_7883);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+4); push_effect(gb, HL);
  CYC(b_+4, b_+7); SET_HL(SYM(seasonsTable_0d_78a3));
  CYC(b_+7, b_+8); blainosGloves_add_double_index(gb, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  s_seasonsFunc_0d_7895_hook(gb); return; // falls through
}

void s_seasonsFunc_0d_7895_hook(GB *gb) {
  BASE(seasonsFunc_0d_7895);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+3);
  s_seasonsFunc_0d_7898_hook(gb); return; // falls through
}

void s_seasonsFunc_0d_7898_hook(GB *gb) {
  BASE(seasonsFunc_0d_7898);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+5, b_+8); TAIL(objectSetVisible82); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11);
  TAIL(objectSetVisible81);
}

void s_seasonsFunc_0d_78ab_hook(GB *gb) {
  BASE(seasonsFunc_0d_78ab);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+2, b_+3); B = mem_rd(gb, HL);
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+7); A = 0x0b;
  CALL_C(b_+7, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+11); push_effect(gb, HL);
  CALL_C(b_+11, s_seasonsFunc_0d_7895_hook, SYM(seasonsFunc_0d_7895), b_+14);
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CYC(b_+15, b_+17);
  TAIL_S(seasonsFunc_0d_78ce);
}

void s_seasonsFunc_0d_78bc_hook(GB *gb) {
  BASE(seasonsFunc_0d_78bc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+2, b_+3); B = mem_rd(gb, HL);
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+7); A = 0x0b;
  CALL_C(b_+7, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+11); push_effect(gb, HL);
  CALL_C(b_+11, s_seasonsFunc_0d_7895_hook, SYM(seasonsFunc_0d_7895), b_+14);
  CALL_C(b_+14, s_objectApplySpeed, SYM(objectApplySpeed), b_+17);
  CYC(b_+17, b_+18); SET_HL(pop_effect(gb));
  s_seasonsFunc_0d_78ce_hook(gb); return; // falls through
}

void s_seasonsFunc_0d_78ce_hook(GB *gb) {
  BASE(seasonsFunc_0d_78ce);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  RET(b_+18); return;
}

void s_seasonsFunc_0d_78e1_hook(GB *gb) {
  BASE(seasonsFunc_0d_78e1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_Z;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); E = alu_inc8(gb, E);
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_sbc(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14); L = alu_dec8(gb, L);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_add(gb, 0x80);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_adc(gb, 0x00);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  RET(b_+22); return;
}

void s_seasonsFunc_0d_78f8_hook(GB *gb) {
  BASE(seasonsFunc_0d_78f8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x21;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+10); SET_HL(SYM(seasonsTable_0d_78a3));
  CYC(b_+10, b_+11); blainosGloves_add_a_to_hl(gb, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); C = mem_rd(gb, HL);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CALL_C(b_+15, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+18);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+23, b_+26); TAIL(objectSetVisible82); }
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+29);
  TAIL(objectSetVisible81);
}

void s_seasonsFunc_0d_7915_hook(GB *gb) {
  BASE(seasonsFunc_0d_7915);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_0d_795c_hook, SYM(seasonsFunc_0d_795c), b_+3);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x80);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x08);
  CYC(b_+13, b_+16); SET_HL(w1Link_zh);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_sub(gb, 0x08);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); L = 0x2b;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x00);
  CYC(b_+24, b_+26); L = 0x2d;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00);
  CYC(b_+28, b_+30); A = 0x09;
  CALL_C(b_+30, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+33);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+37); mem_wr(gb, w1Link_knockbackAngle, A);
  CYC(b_+37, b_+39); alu_add(gb, 0x04);
  CYC(b_+39, b_+41); alu_and(gb, 0x18);
  CYC(b_+41, b_+42); alu_rrca(gb);
  CYC(b_+42, b_+43); alu_rrca(gb);
  CYC(b_+43, b_+46); SET_HL(SYM(seasonsTable_0d_7954));
  CYC(b_+46, b_+47); blainosGloves_add_a_to_hl(gb, b_+47);
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+51, b_+54); mem_wr(gb, w1Link_yh, A);
  CYC(b_+54, b_+55); SET_HL(HL + 1);
  CYC(b_+55, b_+57); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+59); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+59, b_+62); mem_wr(gb, w1Link_xh, A);
  RET(b_+62); return;
}

void s_seasonsFunc_0d_795c_hook(GB *gb) {
  BASE(seasonsFunc_0d_795c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = 0x14;
  CYC(b_+10, b_+13); mem_wr(gb, w1Link_invincibilityCounter, A);
  CYC(b_+13, b_+16); mem_wr(gb, w1Link_knockbackCounter, A);
  RET(b_+16); return;
}

void s_seasonsFunc_0d_796d_hook(GB *gb) {
  BASE(seasonsFunc_0d_796d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x30;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+10); SET_HL(SYM(seasonsTable_0d_7982));
  CYC(b_+10, b_+11); blainosGloves_add_a_to_hl(gb, b_+11);
  CYC(b_+11, b_+12); C = mem_rd(gb, HL);
  CYC(b_+12, b_+14); B = 0xf8;
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CALL_C(b_+15, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+18);
  CYC(b_+18, b_+21);
  TAIL_S(seasonsFunc_0d_7898);
}

void s_seasonsFunc_0d_7986_hook(GB *gb) {
  BASE(seasonsFunc_0d_7986);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}
