#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/danceHallMinigame.s.
// INTERAC_DANCE_HALL_MINIGAME

static uint16_t danceHallMinigame_jump_table(GB *gb) {
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

static void danceHallMinigame_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void danceHallMinigame_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode6a@func_60a4
static void danceHallMinigame_func_60a4(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+1214, s_objectCheckCollidedWithLink, SYM(objectCheckCollidedWithLink), b_+1217);
  if (!(F & FC)) { RET_TAKEN(b_+1217); return; }
  CYC(b_+1217, b_+1218);
  CYC(b_+1218, b_+1220); A = 0x01;
  CYC(b_+1220, b_+1223); mem_wr(gb, wTmpcfc0 + 0x1d, A);
  RET(b_+1223); return;
}

// interactionCode6a@func_6061
static void danceHallMinigame_func_6061(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+1147, b_+1149); A = 0x17;
  CYC(b_+1149, b_+1150); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+1150, b_+1151); A = alu_inc8(gb, A);
  if (!(F & FC)) { CYCT(b_+1151, b_+1153); goto L_606b; }
  CYC(b_+1151, b_+1153);
  CYC(b_+1153, b_+1155); A = 0x68;
  CYC(b_+1155, b_+1156); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+1156); return; }
  CYC(b_+1156, b_+1157);
L_606b:
  CYC(b_+1157, b_+1158); mem_wr(gb, HL, A);
  CYC(b_+1158, b_+1161); A = mem_rd(gb, wTmpcfc0 + 0x12);
  CYC(b_+1161, b_+1163); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+1163, b_+1164); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+1164, b_+1166); alu_and(gb, 0x03);
  CYC(b_+1166, b_+1167); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1167, b_+1168); B = A;
  CYC(b_+1168, b_+1170); A = alu_swap(gb, A);
  CYC(b_+1170, b_+1171); alu_rrca(gb);
  CYC(b_+1171, b_+1172); mem_wr(gb, HL, A);
  CYC(b_+1172, b_+1173); A = B;
  CYC(b_+1173, b_+1176);
  TAIL(interactionSetAnimation);
}

// interactionCode6a@func_6058
static void danceHallMinigame_func_6058(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+1138, b_+1139); H = D;
  CYC(b_+1139, b_+1141); L = INTERACTION_BASE + OBJ_YH;
  CALL_L(b_+1141, danceHallMinigame_func_6061, b_+1144);
  CYC(b_+1144, b_+1145); H = D;
  CYC(b_+1145, b_+1147); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+1147, b_+1149); A = 0x17;
  CYC(b_+1149, b_+1150); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+1150, b_+1151); A = alu_inc8(gb, A);
  if (!(F & FC)) { CYCT(b_+1151, b_+1153); goto L_606b; }
  CYC(b_+1151, b_+1153);
  CYC(b_+1153, b_+1155); A = 0x68;
  CYC(b_+1155, b_+1156); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+1156); return; }
  CYC(b_+1156, b_+1157);
L_606b:
  CYC(b_+1157, b_+1158); mem_wr(gb, HL, A);
  CYC(b_+1158, b_+1161); A = mem_rd(gb, wTmpcfc0 + 0x12);
  CYC(b_+1161, b_+1163); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+1163, b_+1164); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+1164, b_+1166); alu_and(gb, 0x03);
  CYC(b_+1166, b_+1167); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1167, b_+1168); B = A;
  CYC(b_+1168, b_+1170); A = alu_swap(gb, A);
  CYC(b_+1170, b_+1171); alu_rrca(gb);
  CYC(b_+1171, b_+1172); mem_wr(gb, HL, A);
  CYC(b_+1172, b_+1173); A = B;
  CYC(b_+1173, b_+1176);
  TAIL(interactionSetAnimation);
}

// interactionCode6a@func_603f
static void danceHallMinigame_func_603f(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+1113, b_+1114); H = D;
  CYC(b_+1114, b_+1116); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+1116, b_+1118); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+1118, b_+1119); A = mem_rd(gb, DE);
  CYC(b_+1119, b_+1120); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1120, b_+1122); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+1122, b_+1123); A = mem_rd(gb, DE);
  CYC(b_+1123, b_+1124); mem_wr(gb, HL, A);
  CYC(b_+1124, b_+1127); A = mem_rd(gb, wTmpcfc0 + 0x13);
  CYC(b_+1127, b_+1129); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+1129, b_+1130); mem_wr(gb, DE, A);
  CALL_C(b_+1130, s_objectApplySpeed, SYM(objectApplySpeed), b_+1133);
  CALL_L(b_+1133, danceHallMinigame_func_6058, b_+1136);
  CYC(b_+1136, b_+1138);
  goto func_607e;

func_607e:
  CYC(b_+1176, b_+1178); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+1178, b_+1179); A = mem_rd(gb, DE);
  CYC(b_+1179, b_+1180); B = A;
  CYC(b_+1180, b_+1182); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+1182, b_+1183); A = mem_rd(gb, DE);
  CYC(b_+1183, b_+1184); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(b_+1184, b_+1186); goto L_608a; }
  CYC(b_+1184, b_+1186);
  CYC(b_+1186, b_+1187); alu_cpl(gb);
  CYC(b_+1187, b_+1188); A = alu_inc8(gb, A);
L_608a:
  CYC(b_+1188, b_+1189); C = A;
  CYC(b_+1189, b_+1191); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+1191, b_+1192); A = mem_rd(gb, DE);
  CYC(b_+1192, b_+1193); B = A;
  CYC(b_+1193, b_+1195); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+1195, b_+1196); A = mem_rd(gb, DE);
  CYC(b_+1196, b_+1197); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(b_+1197, b_+1199); goto L_6097; }
  CYC(b_+1197, b_+1199);
  CYC(b_+1199, b_+1200); alu_cpl(gb);
  CYC(b_+1200, b_+1201); A = alu_inc8(gb, A);
L_6097:
  CYC(b_+1201, b_+1202); alu_add(gb, C);
  CYC(b_+1202, b_+1203); B = A;
  CYC(b_+1203, b_+1205); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+1205, b_+1206); A = mem_rd(gb, DE);
  CYC(b_+1206, b_+1207); alu_add(gb, B);
  CYC(b_+1207, b_+1208); mem_wr(gb, DE, A);
  CYC(b_+1208, b_+1210); alu_cp(gb, 0x10);
  if (F & FC) { RET_TAKEN(b_+1210); return; }
  CYC(b_+1210, b_+1211);
  CYC(b_+1211, b_+1214);
  TAIL(objectCenterOnTile);
}

// interactionCode6a@subid2@func_5fc3
static void danceHallMinigame_subid2_func_5fc3(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+989, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+992);
  CYC(b_+992, b_+993); C = A;
  CYC(b_+993, b_+996); SET_HL(b_+1035 /* @subid2@table_5ff1 */);
L_5fca:
  CYC(b_+996, b_+997); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+997, b_+998); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+998, b_+1000); goto L_5fd1; }
  CYC(b_+998, b_+1000);
  CYC(b_+1000, b_+1001); SET_HL(HL + 1);
  CYC(b_+1001, b_+1003);
  goto L_5fca;
L_5fd1:
  CYC(b_+1003, b_+1006); A = mem_rd(gb, wTmpcfc0 + 0x12);
  CYC(b_+1006, b_+1008); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+1008, b_+1010); goto L_5fdb; }
  CYC(b_+1008, b_+1010);
  CYC(b_+1010, b_+1011); A = mem_rd(gb, HL);
  CYC(b_+1011, b_+1013);
  goto L_5fde;
L_5fdb:
  CYC(b_+1013, b_+1014); A = mem_rd(gb, HL);
  CYC(b_+1014, b_+1016); A = alu_swap(gb, A);
L_5fde:
  CYC(b_+1016, b_+1018); alu_and(gb, 0x0f);
  CYC(b_+1018, b_+1020); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+1020, b_+1021); mem_wr(gb, DE, A);
  CYC(b_+1021, b_+1023); mem_wr(gb, hFF8B, A);
  CALL_C(b_+1023, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+1026);
  CYC(b_+1026, b_+1028); A = mem_rd(gb, hFF8B);
  CYC(b_+1028, b_+1030); A = alu_swap(gb, A);
  CYC(b_+1030, b_+1031); alu_rrca(gb);
  CYC(b_+1031, b_+1033); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+1033, b_+1034); mem_wr(gb, DE, A);
  RET(b_+1034); return;
}

// interactionCode6a@func_5f1d
static void danceHallMinigame_func_5f1d(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+823, b_+825); C = 0x28;
  CALL_C(b_+825, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+828);
  CALL_C(b_+828, s_interactionAnimate, SYM(interactionAnimate), b_+831);
  CYC(b_+831, b_+832); H = D;
  CYC(b_+832, b_+834); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+834, b_+835); A = mem_rd(gb, HL);
  CYC(b_+835, b_+836); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+836, b_+838); goto L_5f36; }
  CYC(b_+836, b_+838);
  CYC(b_+838, b_+839); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+839, b_+841); goto L_5f36; }
  CYC(b_+839, b_+841);
  CYC(b_+841, b_+842); A = alu_dec8(gb, A);
  CYC(b_+842, b_+844); mem_wr(gb, HL, 0x00);
  CYC(b_+844, b_+846); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+846, b_+847); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+847, b_+848); mem_wr(gb, HL, A);
L_5f36:
  CYC(b_+848, b_+850); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+850, b_+851); A = mem_rd(gb, HL);
  CYC(b_+851, b_+852); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+852); return; }
  CYC(b_+852, b_+853);
  CYC(b_+853, b_+854); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+854); return;
}

// interactionCode6a@func_5f10
static void danceHallMinigame_func_5f10(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto func_5f10;
subid1_func_5c6f:
  CYC(b_+137, b_+139); A = 0x01;
  CYC(b_+139, b_+142); mem_wr(gb, wTmpcfc0 + 0x12, A);
  CYC(b_+142, b_+144); A = 0x04;
  CYC(b_+144, b_+146);
  goto L_5c87;
subid1_func_5c78:
  CYC(b_+146, b_+148); A = 0xff;
  CYC(b_+148, b_+151); mem_wr(gb, wTmpcfc0 + 0x12, A);
  CYC(b_+151, b_+153); A = 0x04;
  CYC(b_+153, b_+155);
  goto L_5c87;
subid1_func_5c81:
  CYC(b_+155, b_+157); A = 0x05;
  CYC(b_+157, b_+159);
  goto L_5c87;

L_5c87:
  CYC(b_+161, b_+164); mem_wr(gb, wTmpcfc0 + 0x14, A);
  CYC(b_+164, b_+166); A = 0x09;
  CYC(b_+166, b_+169); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(b_+169, b_+172); SET_HL(wTmpcfc0 + 0x1a);
  CYC(b_+172, b_+173); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+173); return;

func_5f10:
  CYC(b_+810, b_+811); push_effect(gb, b_+811);
  do { uint16_t jt_ = (danceHallMinigame_jump_table(gb));
    if (jt_ == b_+137) goto subid1_func_5c6f;
    if (jt_ == b_+146) goto subid1_func_5c78;
    if (jt_ == b_+155) goto subid1_func_5c81;
    HANDOFF(HL);
  } while (0);
}

// interactionCode6a@func_5efd
static void danceHallMinigame_func_5efd(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+791, b_+794); A = mem_rd(gb, wTmpcfc0 + 0x0b);
  CYC(b_+794, b_+797); SET_HL(wTmpcfc0 + 0x08);
  CYC(b_+797, b_+798); danceHallMinigame_add_a_to_hl(gb, b_+798);
  CYC(b_+798, b_+799); A = mem_rd(gb, HL);
  CYC(b_+799, b_+801); mem_wr(gb, hFF8B, A);
  CYC(b_+801, b_+804); SET_HL(b_+817 /* @table_5f17 */);
  CYC(b_+804, b_+805); danceHallMinigame_add_double_index(gb, b_+805);
  CYC(b_+805, b_+806); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+806, b_+807); E = mem_rd(gb, HL);
  CYC(b_+807, b_+810);
  TAIL(playSound_b00);
}

// interactionCode6a@func_5ec4
static void danceHallMinigame_func_5ec4(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+734, s_getRandomNumber, SYM(getRandomNumber), b_+737);
  CYC(b_+737, b_+739); alu_and(gb, 0x0f);
  CYC(b_+739, b_+742); SET_BC(b_+747 /* @table_5ed1 */);
  CALL_C(b_+742, s_addAToBc, SYM(addAToBc), b_+745);
  CYC(b_+745, b_+746); A = mem_rd(gb, BC);
  RET(b_+746); return;
}

// interactionCode6a@func_5eb9
static void danceHallMinigame_func_5eb9(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+723, b_+725); A = 0x02;
  CALL_C(b_+725, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+728);
  CYC(b_+728, b_+731); SET_BC(0xfe80);
  CYC(b_+731, b_+734);
  TAIL(objectSetSpeedZ);
}

// interactionCode6a@func_5e97
static void danceHallMinigame_func_5e97(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+689, b_+692); A = mem_rd(gb, wTmpcfc0 + 0x1d);
  CYC(b_+692, b_+693); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+693); return; }
  CYC(b_+693, b_+694);
  CYC(b_+694, b_+697); A = mem_rd(gb, wTmpcfc0 + 0x18);
  CYC(b_+697, b_+698); B = A;
  CYC(b_+698, b_+699); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+699); return; }
  CYC(b_+699, b_+700);
  CYC(b_+700, b_+703); A = mem_rd(gb, wTmpcfc0 + 0x19);
  CYC(b_+703, b_+705); alu_cp(gb, 0x03);
  if (F & FZ) { RET_TAKEN(b_+705); return; }
  CYC(b_+705, b_+706);
  CYC(b_+706, b_+709); SET_HL(wTmpcfc0 + 0x19);
  CYC(b_+709, b_+710); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+710, b_+713); SET_HL(wTmpcfc0 + 0x08);
  CYC(b_+713, b_+714); danceHallMinigame_add_a_to_hl(gb, b_+714);
  CYC(b_+714, b_+715); A = mem_rd(gb, HL);
  CYC(b_+715, b_+716); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+716); return; }
  CYC(b_+716, b_+717);
  CYC(b_+717, b_+719); A = 0xff;
  CYC(b_+719, b_+722); mem_wr(gb, wTmpcfc0 + 0x18, A);
  RET(b_+722); return;
}

// interactionCode6a@subid1@func_5e77
static void danceHallMinigame_subid1_func_5e77(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto subid1_func_5e77;
subid1_state4_func_5cdd:
  CYC(b_+247, b_+250); A = mem_rd(gb, wTmpcfc0 + 0x17);
  CYC(b_+250, b_+253); SET_HL(b_+263 /* @subid1@state4@table_5ced */);
  CYC(b_+253, b_+254); danceHallMinigame_add_double_index(gb, b_+254);
  CYC(b_+254, b_+255); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+255, b_+258); mem_wr(gb, wTmpcfc0 + 0x13, A);
  CYC(b_+258, b_+259); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+259, b_+262); mem_wr(gb, wTmpcfc0 + 0x16, A);
  RET(b_+262); return;

subid1_func_5e77:
  CYC(b_+657, b_+660); SET_HL(wTmpcfc0 + 0x1b);
  CYC(b_+660, b_+661); A = mem_rd(gb, HL);
  CYC(b_+661, b_+663); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(b_+663, b_+665); goto L_5e81; }
  CYC(b_+663, b_+665);
  CYC(b_+665, b_+667); A = 0x08;
L_5e81:
  CYC(b_+667, b_+668); A = alu_inc8(gb, A);
  CYC(b_+668, b_+669); mem_wr(gb, HL, A);
  CYC(b_+669, b_+670); B = A;
  CYC(b_+670, b_+672); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+672); return; }
  CYC(b_+672, b_+673);
  CYC(b_+673, b_+674); A = B;
  CYC(b_+674, b_+675); alu_rrca(gb);
  CYC(b_+675, b_+676); alu_rrca(gb);
  CYC(b_+676, b_+678); alu_and(gb, 0x03);
  CYC(b_+678, b_+679); B = A;
  CYC(b_+679, b_+682); A = mem_rd(gb, wTmpcfc0 + 0x17);
  CYC(b_+682, b_+683); alu_add(gb, B);
  CYC(b_+683, b_+686); mem_wr(gb, wTmpcfc0 + 0x17, A);
  CYC(b_+686, b_+689);
  goto subid1_state4_func_5cdd;
}

// interactionCode6a@subid1@state4@func_5cdd
static void danceHallMinigame_subid1_state4_func_5cdd(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+247, b_+250); A = mem_rd(gb, wTmpcfc0 + 0x17);
  CYC(b_+250, b_+253); SET_HL(b_+263 /* @subid1@state4@table_5ced */);
  CYC(b_+253, b_+254); danceHallMinigame_add_double_index(gb, b_+254);
  CYC(b_+254, b_+255); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+255, b_+258); mem_wr(gb, wTmpcfc0 + 0x13, A);
  CYC(b_+258, b_+259); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+259, b_+262); mem_wr(gb, wTmpcfc0 + 0x16, A);
  RET(b_+262); return;
}

// interactionCode6a@subid1@func_5c21
static void danceHallMinigame_subid1_func_5c21(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+59, b_+61); A = 0x01;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CALL_C(b_+62, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+65);
  CYC(b_+65, b_+67); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x02);
  CYC(b_+69, b_+70); L = alu_inc8(gb, L);
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x10);
  CALL_C(b_+72, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+75);
  CYC(b_+75, b_+78);
  TAIL(objectSetVisiblec2);
}

// INTERAC_DANCE_HALL_MINIGAME
void s_interactionCode6a_hook(GB *gb) {
  BASE(interactionCode6a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (danceHallMinigame_jump_table(gb));
    if (jt_ == b_+12) goto subid0;
    if (jt_ == b_+34) goto subid1;
    if (jt_ == b_+855) goto subid2;
    if (jt_ == b_+1224) goto subid3;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+17); mem_wr(gb, wInShop, A);
  CYC(b_+17, b_+19); B = 0x20;
  CYC(b_+19, b_+22); SET_HL(wTmpcfc0);
  CALL_C(b_+22, s_clearMemory, SYM(clearMemory), b_+25);
  CYC(b_+25, b_+28); SET_HL(SYM(masterDiverScript_secretDone));
  CALL_C(b_+28, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+31);
  CYC(b_+31, b_+34);
  TAIL(interactionDelete);
subid1:
  CYC(b_+34, b_+36); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); push_effect(gb, b_+38);
  do { uint16_t jt_ = (danceHallMinigame_jump_table(gb));
    if (jt_ == b_+50) goto subid1_state0;
    if (jt_ == b_+651) goto subid1_state1;
    if (jt_ == b_+78) goto subid1_state2;
    if (jt_ == b_+89) goto subid1_state3;
    if (jt_ == b_+174) goto subid1_state4;
    if (jt_ == b_+489) goto subid1_state5;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CALL_L(b_+50, danceHallMinigame_subid1_func_5c21, b_+53);
  CYC(b_+53, b_+56); SET_HL(SYM(dancecLeaderScript_promptToStartDancing));
  CYC(b_+56, b_+59);
  TAIL(interactionSetScript);

subid1_state2:
  CYC(b_+78, b_+80); C = 0x28;
  CALL_C(b_+80, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+83);
  CALL_C(b_+83, s_interactionRunScript, SYM(interactionRunScript), b_+86);
  CYC(b_+86, b_+89);
  TAIL(interactionAnimate);
subid1_state3:
  CALL_C(b_+89, s_interactionAnimate, SYM(interactionAnimate), b_+92);
  CYC(b_+92, b_+94); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+94, b_+95); A = mem_rd(gb, DE);
  CYC(b_+95, b_+96); push_effect(gb, b_+96);
  do { uint16_t jt_ = (danceHallMinigame_jump_table(gb));
    if (jt_ == b_+102) goto subid1_state3_substate0;
    if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), s_interactionRunScript)) { s_interactionRunScript(gb); return; }
    if (jt_ == b_+119) goto subid1_state3_substate2;
    HANDOFF(HL);
  } while (0);
subid1_state3_substate0:
  CYC(b_+102, b_+104); A = 0x01;
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  CYC(b_+105, b_+108); mem_wr(gb, wTmpcfc0 + 0x1a, A);
  CYC(b_+108, b_+110); A = 0x50;
  CYC(b_+110, b_+113); mem_wr(gb, wTmpcfc0 + 0x13, A);
  CYC(b_+113, b_+116); SET_HL(SYM(danceLeaderScript_promptForTutorial));
  CYC(b_+116, b_+119);
  TAIL(interactionSetScript);
subid1_state3_substate2:
  CYC(b_+119, b_+122); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+123); return; }
  CYC(b_+123, b_+124);
  CYC(b_+124, b_+125); alu_xor(gb, A);
  CYC(b_+125, b_+126); H = D;
  CYC(b_+126, b_+127); L = E;
  CYC(b_+127, b_+128); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+128, b_+129); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+129, b_+131); A = 0x01;
  CALL_C(b_+131, s_setLinkIDOverride, SYM(setLinkIDOverride), b_+134);
  CYC(b_+134, b_+137);
  TAIL(fastFadeinFromWhite);

subid1_state4:
  CYC(b_+174, b_+176); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+178); push_effect(gb, b_+178);
  do { uint16_t jt_ = (danceHallMinigame_jump_table(gb));
    if (jt_ == b_+188) goto subid1_state4_substate0;
    if (jt_ == b_+302) goto subid1_state4_substate1;
    if (jt_ == b_+337) goto subid1_state4_substate2;
    if (jt_ == b_+377) goto subid1_state4_substate3;
    if (jt_ == b_+475) goto subid1_state4_substate4;
    HANDOFF(HL);
  } while (0);
subid1_state4_substate0:
  CYC(b_+188, b_+190); A = 0x01;
  CYC(b_+190, b_+191); mem_wr(gb, DE, A);
  CYC(b_+191, b_+194); A = mem_rd(gb, wNumTimesPlayedSubrosianDance);
  CYC(b_+194, b_+196); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(b_+196, b_+198); goto L_5cae; }
  CYC(b_+196, b_+198);
  CYC(b_+198, b_+200); A = 0x08;
L_5cae:
  CYC(b_+200, b_+203); mem_wr(gb, wTmpcfc0 + 0x17, A);
  CYC(b_+203, b_+206); mem_wr(gb, wTmpcfc0 + 0x1c, A);
  CALL_L(b_+206, danceHallMinigame_subid1_state4_func_5cdd, b_+209);
  CYC(b_+209, b_+212); A = mem_rd(gb, wTmpcfc0 + 0x17);
  CYC(b_+212, b_+215); SET_HL(b_+293 /* @subid1@state4@table_5d0b */);
  CYC(b_+215, b_+216); danceHallMinigame_add_a_to_hl(gb, b_+216);
  CALL_C(b_+216, s_getRandomNumber, SYM(getRandomNumber), b_+219);
  CYC(b_+219, b_+221); alu_and(gb, 0x03);
  CYC(b_+221, b_+222); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+222, b_+225); mem_wr(gb, wTmpcfc0 + 0x15, A);
  CYC(b_+225, b_+226); alu_xor(gb, A);
  CYC(b_+226, b_+229); mem_wr(gb, wTmpcfc0 + 0x14, A);
  CYC(b_+229, b_+232); mem_wr(gb, wTmpcfc0 + 0x1b, A);
  CYC(b_+232, b_+234); A = 0xcc;
  CALL_C(b_+234, s_playSound, SYM(playSound_b00), b_+237);
  CYC(b_+237, b_+239); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+239, b_+241); A = 0x3c;
  CYC(b_+241, b_+242); mem_wr(gb, DE, A);
  CYC(b_+242, b_+244); A = 0x22;
  CYC(b_+244, b_+247);
  TAIL(playSound_b00);

subid1_state4_substate1:
  CALL_C(b_+302, s_interactionDecCounter2, SYM(interactionDecCounter2), b_+305);
  if (!(F & FZ)) { RET_TAKEN(b_+305); return; }
  CYC(b_+305, b_+306);
  CYC(b_+306, b_+308); mem_wr(gb, HL, 0x01);
  CYC(b_+308, b_+310); A = 0x02;
  CYC(b_+310, b_+311); mem_wr(gb, DE, A);
  CYC(b_+311, b_+314); SET_HL(wTmpcfc0 + 0x08);
  CALL_L(b_+314, danceHallMinigame_func_5ec4, b_+317);
  CYC(b_+317, b_+318); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_L(b_+318, danceHallMinigame_func_5ec4, b_+321);
  CYC(b_+321, b_+322); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_L(b_+322, danceHallMinigame_func_5ec4, b_+325);
  CYC(b_+325, b_+326); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+326, b_+327); alu_xor(gb, A);
  CYC(b_+327, b_+328); mem_wr(gb, HL, A);
  CYC(b_+328, b_+330); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+330, b_+333); A = mem_rd(gb, wTmpcfc0 + 0x16);
  CYC(b_+333, b_+334); mem_wr(gb, DE, A);
  CALL_L(b_+334, danceHallMinigame_func_5eb9, b_+337);
subid1_state4_substate2:
  CALL_L(b_+337, danceHallMinigame_func_5f1d, b_+340);
  if (!(F & FZ)) { RET_TAKEN(b_+340); return; }
  CYC(b_+340, b_+341);
  CYC(b_+341, b_+344); A = mem_rd(gb, wTmpcfc0 + 0x0b);
  CYC(b_+344, b_+346); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+346, b_+348); goto L_5d45; }
  CYC(b_+346, b_+348);
  CYC(b_+348, b_+351);
  goto func_5ee1;
L_5d45:
  CALL_C(b_+351, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+354);
  CYC(b_+354, b_+357); A = mem_rd(gb, wTmpcfc0 + 0x16);
  CYC(b_+357, b_+359); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+359, b_+360); mem_wr(gb, HL, A);
  CYC(b_+360, b_+361); alu_xor(gb, A);
  CYC(b_+361, b_+364); mem_wr(gb, wTmpcfc0 + 0x0b, A);
  CYC(b_+364, b_+367); mem_wr(gb, wTmpcfc0 + 0x19, A);
  CYC(b_+367, b_+369); A = 0xff;
  CYC(b_+369, b_+372); mem_wr(gb, wTmpcfc0 + 0x18, A);
  CYC(b_+372, b_+374); A = 0x02;
  CALL_C(b_+374, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+377);
subid1_state4_substate3:
  CALL_L(b_+377, danceHallMinigame_func_5e97, b_+380);
  if (!(F & FZ)) { CYCT(b_+380, b_+382); goto subid1_state4_func_5d91; }
  CYC(b_+380, b_+382);
  CALL_L(b_+382, danceHallMinigame_func_5f1d, b_+385);
  if (!(F & FZ)) { RET_TAKEN(b_+385); return; }
  CYC(b_+385, b_+386);
  CYC(b_+386, b_+389); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+389, b_+390); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+390); return; }
  CYC(b_+390, b_+391);
  CYC(b_+391, b_+394); A = mem_rd(gb, wTmpcfc0 + 0x0b);
  CYC(b_+394, b_+396); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+396, b_+398); goto L_5d77; }
  CYC(b_+396, b_+398);
  CYC(b_+398, b_+401);
  goto func_5eea;
L_5d77:
  CYC(b_+401, b_+404); A = mem_rd(gb, wTmpcfc0 + 0x19);
  CYC(b_+404, b_+406); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+406, b_+408); goto subid1_state4_func_5d91; }
  CYC(b_+406, b_+408);
  CYC(b_+408, b_+411); SET_HL(wTmpcfc0 + 0x15);
  CYC(b_+411, b_+412); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+412, b_+414); goto subid1_state4_func_5dab; }
  CYC(b_+412, b_+414);
  CALL_L(b_+414, danceHallMinigame_subid1_func_5e77, b_+417);
  CYC(b_+417, b_+419); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+419, b_+421); A = 0x01;
  CYC(b_+421, b_+422); mem_wr(gb, DE, A);
  CYC(b_+422, b_+423); alu_xor(gb, A);
  CYC(b_+423, b_+426); mem_wr(gb, wTmpcfc0 + 0x0b, A);
  RET(b_+426); return;
subid1_state4_func_5d91:
  CYC(b_+427, b_+430); SET_BC(0x0104);
  CALL_C(b_+430, s_showText, SYM(showText), b_+433);
  CYC(b_+433, b_+435); A = 0x04;
  CYC(b_+435, b_+437); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+437, b_+438); mem_wr(gb, DE, A);
  CYC(b_+438, b_+440); A = 0xff;
  CYC(b_+440, b_+443); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+443, b_+445); A = 0xcc;
  CALL_C(b_+445, s_playSound, SYM(playSound_b00), b_+448);
  CYC(b_+448, b_+450); A = 0xfb;
  CYC(b_+450, b_+453);
  TAIL(playSound_b00);
subid1_state4_func_5dab:
  CALL_C(b_+453, s_interactionIncState, SYM(interactionIncState), b_+456);
  CYC(b_+456, b_+457); L = alu_inc8(gb, L);
  CYC(b_+457, b_+459); mem_wr(gb, HL, 0x00);
  CYC(b_+459, b_+461); A = 0x01;
  CYC(b_+461, b_+464); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+464, b_+466); A = 0xfb;
  CALL_C(b_+466, s_playSound, SYM(playSound_b00), b_+469);
  CYC(b_+469, b_+472); SET_BC(0x010a);
  CYC(b_+472, b_+475);
  TAIL(showText);
subid1_state4_substate4:
  CALL_C(b_+475, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+478);
  CYC(b_+478, b_+481); SET_HL(b_+484 /* @subid1@state4@warpDestVariables */);
  CYC(b_+481, b_+484);
  TAIL(setWarpDestVariables);

subid1_state5:
  CYC(b_+489, b_+491); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+491, b_+492); A = mem_rd(gb, DE);
  CYC(b_+492, b_+493); push_effect(gb, b_+493);
  do { uint16_t jt_ = (danceHallMinigame_jump_table(gb));
    if (jt_ == b_+503) goto subid1_state5_substate0;
    if (jt_ == b_+514) goto subid1_state5_substate1;
    if (jt_ == b_+554) goto subid1_state5_substate2;
    if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), s_interactionRunScript)) { s_interactionRunScript(gb); return; }
    if (jt_ == b_+475) goto subid1_state4_substate4;
    HANDOFF(HL);
  } while (0);
subid1_state5_substate0:
  CALL_C(b_+503, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+506);
  CYC(b_+506, b_+508); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+508, b_+510); A = 0x01;
  CYC(b_+510, b_+511); mem_wr(gb, DE, A);
  CYC(b_+511, b_+514);
  TAIL(fastFadeoutToWhite);
subid1_state5_substate1:
  CYC(b_+514, b_+517); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+517, b_+518); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+518); return; }
  CYC(b_+518, b_+519);
  CYC(b_+519, b_+520); alu_xor(gb, A);
  CALL_C(b_+520, s_setLinkIDOverride, SYM(setLinkIDOverride), b_+523);
  CYC(b_+523, b_+525); L = 0x0b;
  CYC(b_+525, b_+527); mem_wr(gb, HL, 0x30);
  CYC(b_+527, b_+529); L = 0x0d;
  CYC(b_+529, b_+531); mem_wr(gb, HL, 0x48);
  CYC(b_+531, b_+533); L = 0x08;
  CYC(b_+533, b_+535); mem_wr(gb, HL, 0x02);
  CALL_C(b_+535, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+538);
  CYC(b_+538, b_+540); A = 0x81;
  CYC(b_+540, b_+543); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+543, b_+546); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+546, b_+548); A = 0x1e;
  CALL_C(b_+548, s_addToGashaMaturity, SYM(addToGashaMaturity), b_+551);
  CYC(b_+551, b_+554);
  TAIL(fastFadeinFromWhite);
subid1_state5_substate2:
  CYC(b_+554, b_+557); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+557, b_+558); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+558); return; }
  CYC(b_+558, b_+559);
  CYC(b_+559, b_+561); A = 0x81;
  CYC(b_+561, b_+564); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+564, b_+567); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+567, b_+570); SET_HL(wNumTimesPlayedSubrosianDance);
  CALL_C(b_+570, s_incHlRefWithCap, SYM(incHlRefWithCap), b_+573);
  CYC(b_+573, b_+574); A = mem_rd(gb, HL);
  CYC(b_+574, b_+575); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+575, b_+577); goto subid1_state5_func_5e25; }
  CYC(b_+575, b_+577);
  CYC(b_+577, b_+579); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+579, b_+581); goto subid1_state5_func_5e40; }
  CYC(b_+579, b_+581);
  CYC(b_+581, b_+583); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+583, b_+585); goto subid1_state5_func_5e56; }
  CYC(b_+583, b_+585);
  CALL_C(b_+585, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+588);
  if (!(F & FZ)) { CYCT(b_+588, b_+590); goto subid1_state5_func_5e56; }
  CYC(b_+588, b_+590);
  CYC(b_+590, b_+593); A = mem_rd(gb, wRickyState);
  CYC(b_+593, b_+595); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+595, b_+597); goto subid1_state5_func_5e56; }
  CYC(b_+595, b_+597);
  CYC(b_+597, b_+600); SET_HL(SYM(danceLeaderScript_giveFlute));
  CYC(b_+600, b_+602);
  goto subid1_state5_func_5e68;
subid1_state5_func_5e40:
  CYC(b_+602, b_+605); SET_HL(SYM(seasonsFunc_15_5e20));
  CYC(b_+605, b_+607); E = 0x15;
  CALL_C(b_+607, s_interBankCall, SYM(interBankCall), b_+610);
  CYC(b_+610, b_+612); alu_bit(gb, 7, B);
  if (!(F & FZ)) { CYCT(b_+612, b_+614); goto L_5e60; }
  CYC(b_+612, b_+614);
  CYC(b_+614, b_+616); C = 0x00;
  CALL_C(b_+616, s_giveRingToLink, SYM(giveRingToLink), b_+619);
  CYC(b_+619, b_+622); SET_HL(SYM(danceLeaderScript_itemGiven));
  CYC(b_+622, b_+624);
  goto subid1_state5_func_5e68;
subid1_state5_func_5e56:
  CALL_C(b_+624, s_getRandomNumber, SYM(getRandomNumber), b_+627);
  CYC(b_+627, b_+629); alu_cp(gb, 0x60);
  CYC(b_+629, b_+632); SET_HL(SYM(danceLeaderScript_giveOreChunks));
  if (!(F & FC)) { CYCT(b_+632, b_+634); goto subid1_state5_func_5e68; }
  CYC(b_+632, b_+634);
L_5e60:
  CYC(b_+634, b_+637); SET_HL(SYM(danceLeaderScript_gashaSeed));
  CYC(b_+637, b_+639);
  goto subid1_state5_func_5e68;
subid1_state5_func_5e25:
  CYC(b_+639, b_+642); SET_HL(SYM(danceLeaderScript_boomerang));
subid1_state5_func_5e68:
  CALL_C(b_+642, s_interactionSetScript, SYM(interactionSetScript), b_+645);
  CYC(b_+645, b_+647); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+647, b_+649); A = 0x03;
  CYC(b_+649, b_+650); mem_wr(gb, DE, A);
  RET(b_+650); return;
subid1_state1:
  CALL_C(b_+651, s_interactionRunScript, SYM(interactionRunScript), b_+654);
  CYC(b_+654, b_+657);
  TAIL(npcFaceLinkAndAnimate);

func_5ee1:
  CALL_L(b_+763, danceHallMinigame_func_5efd, b_+766);
  CYC(b_+766, b_+767); A = E;
  CALL_C(b_+767, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+770);
  CYC(b_+770, b_+772);
  goto L_5ef2;
func_5eea:
  CALL_L(b_+772, danceHallMinigame_func_5efd, b_+775);
  CYC(b_+775, b_+777); A = mem_rd(gb, hFF8B);
  CALL_L(b_+777, danceHallMinigame_func_5f10, b_+780);
L_5ef2:
  CYC(b_+780, b_+783); SET_HL(wTmpcfc0 + 0x0b);
  CYC(b_+783, b_+784); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+784, b_+786); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+786, b_+789); A = mem_rd(gb, wTmpcfc0 + 0x16);
  CYC(b_+789, b_+790); mem_wr(gb, DE, A);
  RET(b_+790); return;

subid2:
  CYC(b_+855, b_+857); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+857, b_+858); A = mem_rd(gb, DE);
  CYC(b_+858, b_+859); push_effect(gb, b_+859);
  do { uint16_t jt_ = (danceHallMinigame_jump_table(gb));
    if (jt_ == b_+873) goto subid2_state0;
    if (jt_ == b_+917) goto subid2_state1;
    if (jt_ == b_+939) goto subid2_state2;
    if (jt_ == b_+1075) goto subid2_state3;
    if (jt_ == b_+1085) goto subid2_state4;
    if (jt_ == b_+1095) goto subid2_state5;
    if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), s_interactionAnimate)) { s_interactionAnimate(gb); return; }
    HANDOFF(HL);
  } while (0);
subid2_state0:
  CALL_L(b_+873, danceHallMinigame_subid1_func_5c21, b_+876);
  CYC(b_+876, b_+878); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+878, b_+879); A = mem_rd(gb, DE);
  CYC(b_+879, b_+882); SET_HL(b_+908 /* @subid2@table_5f72 */);
  CYC(b_+882, b_+883); danceHallMinigame_add_a_to_hl(gb, b_+883);
  CYC(b_+883, b_+885); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+885, b_+886); A = mem_rd(gb, HL);
  CYC(b_+886, b_+887); mem_wr(gb, DE, A);
  CYC(b_+887, b_+889); A = 0x01;
  CYC(b_+889, b_+890); E = alu_inc8(gb, E);
  CYC(b_+890, b_+891); mem_wr(gb, DE, A);
  CYC(b_+891, b_+892); H = D;
  CYC(b_+892, b_+894); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+894, b_+896); mem_wr(gb, HL, 0x01);
  CYC(b_+896, b_+898); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+898, b_+899); A = mem_rd(gb, HL);
  CALL_C(b_+899, s_setShortPosition, SYM(setShortPosition), b_+902);
  CYC(b_+902, b_+905); SET_HL(SYM(danceLeaderScript_showLoadedText));
  CYC(b_+905, b_+908);
  TAIL(interactionSetScript);

subid2_state1:
  CYC(b_+917, b_+920); A = mem_rd(gb, wTmpcfc0 + 0x1a);
  CYC(b_+920, b_+921); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+921, b_+923); goto L_5f87; }
  CYC(b_+921, b_+923);
  CALL_C(b_+923, s_interactionRunScript, SYM(interactionRunScript), b_+926);
  CYC(b_+926, b_+929);
  TAIL(npcFaceLinkAndAnimate);
L_5f87:
  CYC(b_+929, b_+931); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+931, b_+933); A = 0x02;
  CYC(b_+933, b_+934); mem_wr(gb, DE, A);
  CYC(b_+934, b_+936); A = 0x02;
  CYC(b_+936, b_+939);
  TAIL(interactionSetAnimation);
subid2_state2:
  CALL_L(b_+939, danceHallMinigame_func_60a4, b_+942);
  if (F & FC) { CYCT(b_+942, b_+944); goto subid2_func_5fb8; }
  CYC(b_+942, b_+944);
  CALL_C(b_+944, s_interactionAnimate, SYM(interactionAnimate), b_+947);
  CYC(b_+947, b_+950); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+950, b_+951); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+951, b_+953); goto subid2_func_5fb8; }
  CYC(b_+951, b_+953);
  CYC(b_+953, b_+954); H = D;
  CYC(b_+954, b_+956); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+956, b_+959); A = mem_rd(gb, wTmpcfc0 + 0x1a);
  CYC(b_+959, b_+960); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+960); return; }
  CYC(b_+960, b_+961);
  CYC(b_+961, b_+962); mem_wr(gb, HL, A);
  CYC(b_+962, b_+965); A = mem_rd(gb, wTmpcfc0 + 0x14);
  CYC(b_+965, b_+967); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+967, b_+968); mem_wr(gb, HL, A);
  CYC(b_+968, b_+970); alu_cp(gb, 0x04);
  if (F & FZ) CALL_L_CC(b_+970, danceHallMinigame_subid2_func_5fc3, b_+973);
  else CYC(b_+970, b_+973);
  CYC(b_+973, b_+974); alu_xor(gb, A);
  CYC(b_+974, b_+976); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+976, b_+977); mem_wr(gb, DE, A);
  RET(b_+977); return;
subid2_func_5fb8:
  CYC(b_+978, b_+980); A = 0x02;
  CALL_C(b_+980, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+983);
  CYC(b_+983, b_+985); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+985, b_+987); A = 0x06;
  CYC(b_+987, b_+988); mem_wr(gb, DE, A);
  RET(b_+988); return;

subid2_state3:
  CYC(b_+1075, b_+1077); A = 0x02;
  CYC(b_+1077, b_+1078); mem_wr(gb, DE, A);
  CYC(b_+1078, b_+1080); A = 0x02;
  CALL_C(b_+1080, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+1083);
  CYC(b_+1083, b_+1085);
  goto func_6037;
subid2_state4:
  CALL_L(b_+1085, danceHallMinigame_func_603f, b_+1088);
  if (F & FC) { RET_TAKEN(b_+1088); return; }
  CYC(b_+1088, b_+1089);
  CYC(b_+1089, b_+1091); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+1091, b_+1093); mem_wr(gb, HL, 0x02);
  CYC(b_+1093, b_+1095);
  goto func_6037;
subid2_state5:
  CYC(b_+1095, b_+1097); A = 0x02;
  CYC(b_+1097, b_+1098); mem_wr(gb, DE, A);
  CYC(b_+1098, b_+1100); A = 0x04;
  CALL_C(b_+1100, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+1103);
  CYC(b_+1103, b_+1105);
  goto func_6037;
func_6037:
  CYC(b_+1105, b_+1108); SET_HL(wTmpcfc0 + 0x11);
  CYC(b_+1108, b_+1109); A = mem_rd(gb, HL);
  CYC(b_+1109, b_+1110); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+1110); return; }
  CYC(b_+1110, b_+1111);
  CYC(b_+1111, b_+1112); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+1112); return;

subid3:
  CYC(b_+1224, b_+1226); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+1226, b_+1227); A = mem_rd(gb, DE);
  CYC(b_+1227, b_+1228); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+1228, b_+1230); goto L_60bf; }
  CYC(b_+1228, b_+1230);
  CYC(b_+1230, b_+1232); A = 0x01;
  CYC(b_+1232, b_+1233); mem_wr(gb, DE, A);
  CYC(b_+1233, b_+1235); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+1235, b_+1237); A = 0x81;
  CYC(b_+1237, b_+1238); mem_wr(gb, DE, A);
  CALL_C(b_+1238, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+1241);
L_60bf:
  CYC(b_+1241, b_+1244); A = mem_rd(gb, wTmpcfc0 + 0x1f);
  CYC(b_+1244, b_+1245); B = A;
  CYC(b_+1245, b_+1246); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+1246, b_+1249); TAIL(objectSetInvisible); }
  CYC(b_+1246, b_+1249);
  CALL_C(b_+1249, s_objectSetVisible80, SYM(objectSetVisible80), b_+1252);
  CYC(b_+1252, b_+1253); A = B;
  CYC(b_+1253, b_+1255); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+1255, b_+1258); TAIL(interactionDelete); }
  CYC(b_+1255, b_+1258);
  CYC(b_+1258, b_+1259); alu_add(gb, A);
  CYC(b_+1259, b_+1260); alu_add(gb, B);
  CYC(b_+1260, b_+1263); SET_HL(b_+1276 /* @table_60e2 */);
  CYC(b_+1263, b_+1264); danceHallMinigame_add_a_to_hl(gb, b_+1264);
  CYC(b_+1264, b_+1265); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1265, b_+1267); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+1267, b_+1268); mem_wr(gb, DE, A);
  CYC(b_+1268, b_+1270); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+1270, b_+1271); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1271, b_+1272); mem_wr(gb, DE, A);
  CYC(b_+1272, b_+1273); A = mem_rd(gb, HL);
  CYC(b_+1273, b_+1276);
  TAIL(interactionSetAnimation);
}
