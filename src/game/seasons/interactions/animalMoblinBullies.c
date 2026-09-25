#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/animalMoblinBullies.s.
// INTERAC_ANIMAL_MOBLIN_BULLIES

static uint16_t animalMoblinBullies_jump_table(GB *gb) {
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

static void animalMoblinBullies_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode73@func_71c0
static void animalMoblinBullies_func_71c0(GB *gb) {
  BASE(interactionCode73);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+353, b_+355); C = 0x40;
  CALL_C(b_+355, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+358);
  if (F & FZ) { CYCT(b_+358, b_+360); goto L_71c9; }
  CYC(b_+358, b_+360);
  CYC(b_+360, b_+362); A = 0x01;
L_71c9:
  CYC(b_+362, b_+364); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+364, b_+365); mem_wr(gb, DE, A);
  RET(b_+365); return;
}

// interactionCode73@func_71ac
static void animalMoblinBullies_func_71ac(GB *gb) {
  BASE(interactionCode73);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+333, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+336);
  CYC(b_+336, b_+338); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+338, b_+340); A = 0x06;
  CYC(b_+340, b_+341); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+341, b_+343); A = 0x06;
  CYC(b_+343, b_+344); mem_wr(gb, HL, A);
  CYC(b_+344, b_+346); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+346, b_+348); A = 0x32;
  CYC(b_+348, b_+349); mem_wr(gb, HL, A);
  CYC(b_+349, b_+351); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+351, b_+352); A = mem_rd(gb, DE);
  RET(b_+352); return;
}

// interactionCode73@func_7183
static void animalMoblinBullies_func_7183(GB *gb) {
  BASE(interactionCode73);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+292, b_+293); alu_xor(gb, A);
  CYC(b_+293, b_+295); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+295, b_+296); mem_wr(gb, DE, A);
  CYC(b_+296, b_+297); E = alu_inc8(gb, E);
  CYC(b_+297, b_+298); mem_wr(gb, DE, A);
  CYC(b_+298, b_+300); A = 0x07;
  CALL_C(b_+300, s_cpRupeeValue, SYM(cpRupeeValue), b_+303);
  if (!(F & FZ)) { CYCT(b_+303, b_+305); goto L_71a1; }
  CYC(b_+303, b_+305);
  CYC(b_+305, b_+307); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+307, b_+309); A = 0x01;
  CYC(b_+309, b_+310); mem_wr(gb, DE, A);
  CYC(b_+310, b_+312); A = 0x0b;
  CALL_C(b_+312, s_cpRupeeValue, SYM(cpRupeeValue), b_+315);
  if (!(F & FZ)) { CYCT(b_+315, b_+317); goto L_71a1; }
  CYC(b_+315, b_+317);
  CYC(b_+317, b_+319); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+319, b_+321); A = 0x01;
  CYC(b_+321, b_+322); mem_wr(gb, DE, A);
L_71a1:
  CYC(b_+322, b_+323); H = D;
  CYC(b_+323, b_+325); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+325, b_+326); A = mem_rd(gb, HL);
  CYC(b_+326, b_+327); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+327); return; }
  CYC(b_+327, b_+328);
  CYC(b_+328, b_+330); mem_wr(gb, HL, 0x00);
  CYC(b_+330, b_+333);
  TAIL(removeRupeeValue);
}

// INTERAC_ANIMAL_MOBLIN_BULLIES
void s_interactionCode73_hook(GB *gb) {
  BASE(interactionCode73);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto func_7078; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto func_7078; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = mem_rd(gb, wScrollMode);
  CYC(b_+15, b_+17); alu_and(gb, 0x0e);
  if (F & FZ) { CYCT(b_+17, b_+19); goto func_7078; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 0x3c;
  CYC(b_+21, b_+24); mem_wr(gb, wInstrumentsDisabledCounter, A);
  RET(b_+24); return;
func_7078:
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+27, b_+30); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+30, b_+32); alu_cp(gb, 0x0b);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+33, b_+35); goto func_70fd_delete; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(b_+37, b_+39); goto moosh; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); push_effect(gb, b_+41);
  do { uint16_t jt_ = (animalMoblinBullies_jump_table(gb));
    if (jt_ == b_+53) goto state0;
    if (jt_ == b_+161) goto dimitriState1;
    if (jt_ == b_+233) goto dimitriState2;
    HANDOFF(HL);
  } while (0);
moosh:
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); push_effect(gb, b_+49);
  do { uint16_t jt_ = (animalMoblinBullies_jump_table(gb));
    if (jt_ == b_+53) goto state0;
    if (jt_ == b_+248) goto mooshState1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+53, b_+55); A = 0x01;
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CALL_C(b_+56, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+59);
  CYC(b_+59, b_+62); SET_HL(w1Companion_id);
  CYC(b_+62, b_+65); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+65, b_+67); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(b_+67, b_+69); goto func_70c1; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+71, b_+73); goto func_70fd_delete; }
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+74); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+74, b_+76); goto func_70fd_delete; }
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+79); A = mem_rd(gb, wDimitriState);
  CYC(b_+79, b_+81); alu_and(gb, 0x88);
  if (!(F & FZ)) { CYCT(b_+81, b_+83); goto func_70fd_delete; }
  CYC(b_+81, b_+83);
  CALL_L(b_+83, animalMoblinBullies_func_71ac, b_+86);
  CYC(b_+86, b_+89); SET_HL(b_+366 /* @table_71cd */);
  CYC(b_+89, b_+90); animalMoblinBullies_add_double_index(gb, b_+90);
  CYC(b_+90, b_+91); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+91, b_+92); H = mem_rd(gb, HL);
  CYC(b_+92, b_+93); L = A;
  CALL_C(b_+93, s_interactionSetScript, SYM(interactionSetScript), b_+96);
  CYC(b_+96, b_+98);
  goto func_70f3;
func_70c1:
  CYC(b_+98, b_+99); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+99, b_+101); goto func_70fd_delete; }
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+104); A = mem_rd(gb, wMooshState);
  CYC(b_+104, b_+106); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+106, b_+108); goto func_70fd_delete; }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+110, b_+112); goto func_70fd_delete; }
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+114); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+114, b_+116); goto func_70fd_delete; }
  CYC(b_+114, b_+116);
  CYC(b_+116, b_+118); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(b_+118, b_+120); goto L_70e6; }
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+121); H = D;
  CYC(b_+121, b_+123); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+123, b_+124); A = mem_rd(gb, HL);
  CYC(b_+124, b_+125); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+125, b_+127); goto L_70e6; }
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+129); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+129, b_+131); mem_wr(gb, HL, 0x28);
  CYC(b_+131, b_+133); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0xa8);
L_70e6:
  CALL_L(b_+135, animalMoblinBullies_func_71ac, b_+138);
  CYC(b_+138, b_+141); SET_HL(b_+378 /* @table_71d9 */);
  CYC(b_+141, b_+142); animalMoblinBullies_add_double_index(gb, b_+142);
  CYC(b_+142, b_+143); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+143, b_+144); H = mem_rd(gb, HL);
  CYC(b_+144, b_+145); L = A;
  CALL_C(b_+145, s_interactionSetScript, SYM(interactionSetScript), b_+148);
func_70f3:
  CALL_C(b_+148, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+151);
  CALL_C(b_+151, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+154);
  if (F & FC) { RET_TAKEN(b_+154); return; }
  CYC(b_+154, b_+155);
  CYC(b_+155, b_+158);
  TAIL(objectSetInvisible);
func_70fd_delete:
  CYC(b_+158, b_+161);
  TAIL(interactionDelete);
dimitriState1:
  CALL_C(b_+161, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+164);
  CYC(b_+164, b_+166); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+166, b_+167); A = mem_rd(gb, DE);
  CYC(b_+167, b_+169); alu_and(gb, 0x1f);
  if (F & FZ) CALL_L_CC(b_+169, animalMoblinBullies_func_7183, b_+172);
  else CYC(b_+169, b_+172);
  CYC(b_+172, b_+175); A = mem_rd(gb, wDimitriState);
  CYC(b_+175, b_+177); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+177, b_+179); goto func_7131; }
  CYC(b_+177, b_+179);
  CYC(b_+179, b_+182); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+182, b_+183); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+183); return; }
  CYC(b_+183, b_+184);
  CALL_L(b_+184, animalMoblinBullies_func_71c0, b_+187);
  CYC(b_+187, b_+189); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+189, b_+190); A = mem_rd(gb, DE);
  CYC(b_+190, b_+191); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+191, b_+193); goto L_712e; }
  CYC(b_+191, b_+193);
  CALL_C(b_+193, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+196);
  CYC(b_+196, b_+198); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+198, b_+199); mem_wr(gb, DE, A);
  CALL_C(b_+199, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+202);
  CYC(b_+202, b_+203); E = alu_dec8(gb, E);
  CYC(b_+203, b_+204); mem_wr(gb, DE, A);
  CALL_C(b_+204, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+207);
L_712e:
  CYC(b_+207, b_+210);
  TAIL(interactionRunScript);
func_7131:
  CYC(b_+210, b_+212); A = 0x01;
  CYC(b_+212, b_+215); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+215, b_+217); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+217, b_+219); A = 0x02;
  CYC(b_+219, b_+220); mem_wr(gb, DE, A);
  CYC(b_+220, b_+222); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+222, b_+223); A = mem_rd(gb, DE);
  CYC(b_+223, b_+226); SET_HL(b_+372 /* @table_71d3 */);
  CYC(b_+226, b_+227); animalMoblinBullies_add_double_index(gb, b_+227);
  CYC(b_+227, b_+228); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+228, b_+229); H = mem_rd(gb, HL);
  CYC(b_+229, b_+230); L = A;
  CYC(b_+230, b_+233);
  TAIL(interactionSetScript);
dimitriState2:
  CALL_L(b_+233, animalMoblinBullies_func_71c0, b_+236);
  CALL_C(b_+236, s_interactionAnimate, SYM(interactionAnimate), b_+239);
  CALL_C(b_+239, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+242);
  CALL_C(b_+242, s_interactionRunScript, SYM(interactionRunScript), b_+245);
  if (!(F & FC)) { RET_TAKEN(b_+245); return; }
  CYC(b_+245, b_+246);
func_7155:
  CYC(b_+246, b_+248);
  goto func_70fd_delete;
mooshState1:
  CALL_C(b_+248, s_interactionAnimate, SYM(interactionAnimate), b_+251);
  CALL_C(b_+251, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+254);
  CYC(b_+254, b_+257); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+257, b_+258); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+258); return; }
  CYC(b_+258, b_+259);
  CYC(b_+259, b_+262); A = mem_rd(gb, wNumEnemies);
  CYC(b_+262, b_+263); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+263, b_+265); goto L_716a; }
  CYC(b_+263, b_+265);
  CYC(b_+265, b_+267); A = 0x01;
L_716a:
  CYC(b_+267, b_+269); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+269, b_+270); mem_wr(gb, DE, A);
  CALL_L(b_+270, animalMoblinBullies_func_71c0, b_+273);
  CALL_C(b_+273, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+276);
  if (!(F & FC)) { CYCT(b_+276, b_+278); goto L_717a; }
  CYC(b_+276, b_+278);
  CALL_C(b_+278, s_objectSetVisible, SYM(objectSetVisible), b_+281);
  CYC(b_+281, b_+283);
  goto L_717d;
L_717a:
  CALL_C(b_+283, s_objectSetInvisible, SYM(objectSetInvisible), b_+286);
L_717d:
  CALL_C(b_+286, s_interactionRunScript, SYM(interactionRunScript), b_+289);
  if (F & FC) { CYCT(b_+289, b_+291); goto func_7155; }
  CYC(b_+289, b_+291);
  RET(b_+291); return;
}

