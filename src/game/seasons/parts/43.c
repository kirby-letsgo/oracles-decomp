#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/43.s.
// PART_43

static uint16_t part43_jump_table(GB *gb) {
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

static void part43_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// partCode43@func_72e9
static void part43_func_72e9(GB *gb) {
  BASE(partCode43);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+229, s_getFreePartSlot, SYM(getFreePartSlot), b_+232);
  CYC(b_+232, b_+234); mem_wr(gb, HL, 0x43);
  CYC(b_+234, b_+235); L = alu_inc8(gb, L);
  CYC(b_+235, b_+237); mem_wr(gb, HL, 0x04);
  CYC(b_+237, b_+238); L = alu_inc8(gb, L);
  CYC(b_+238, b_+239); mem_wr(gb, HL, B);
  CYC(b_+239, b_+241); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+241, b_+242); mem_wr(gb, HL, C);
  CYC(b_+242, b_+245);
  TAIL(objectCopyPosition);
}

// partCode43@subid1@func_729a
static void part43_subid1_func_729a(GB *gb) {
  BASE(partCode43);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+150, s_func_733d_hook, SYM(func_733d), b_+153);
  CALL_C(b_+153, s_objectSetVisible81, SYM(objectSetVisible81), b_+156);
  CYC(b_+156, b_+158); A = 0x72;
  CYC(b_+158, b_+161);
  TAIL(playSound_b00);
}

// partCode43@func_7236
static void part43_func_7236(GB *gb) {
  BASE(partCode43);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+50, s_getFreePartSlot, SYM(getFreePartSlot), b_+53);
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x43);
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x03);
  CYC(b_+59, b_+61); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+61, b_+62); mem_wr(gb, HL, C);
  CYC(b_+62, b_+65);
  TAIL(objectCopyPosition);
}

// PART_43
void s_partCode43_hook(GB *gb) {
  BASE(partCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = PART_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (part43_jump_table(gb));
    if (jt_ == b_+16) goto subid0;
    if (jt_ == b_+107) goto subid1;
    if (jt_ == b_+170) goto subid2;
    if (jt_ == b_+245) goto subid3;
    if (jt_ == b_+291) goto subid4;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+20); goto func_724b; }
  CYC(b_+18, b_+20);
  CALL_C(b_+20, s_partAnimate, SYM(partAnimate), b_+23);
  CALL_C(b_+23, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+26);
  if (!(F & FZ)) { CYCT(b_+26, b_+29); TAIL(objectApplyComponentSpeed); }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+31); B = 0x06;
L_7223:
  CYC(b_+31, b_+32); A = B;
  CYC(b_+32, b_+33); A = alu_dec8(gb, A);
  CYC(b_+33, b_+36); SET_HL(b_+65 /* @table_7245 */);
  CYC(b_+36, b_+37); part43_add_a_to_hl(gb, b_+37);
  CYC(b_+37, b_+38); C = mem_rd(gb, HL);
  CALL_L(b_+38, part43_func_7236, b_+41);
  CYC(b_+41, b_+42); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto L_7223; }
  CYC(b_+42, b_+44);
  CALL_C(b_+44, s_objectCreatePuff, SYM(objectCreatePuff), b_+47);
  CYC(b_+47, b_+50);
  TAIL(partDelete);

func_724b:
  CYC(b_+71, b_+72); H = D;
  CYC(b_+72, b_+73); L = E;
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+74, b_+76); L = PART_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x06);
  CYC(b_+78, b_+79); L = alu_dec8(gb, L);
  CYC(b_+79, b_+81); A = 0x0a;
  CYC(b_+81, b_+82); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
  CYC(b_+83, b_+85); L = PART_BASE + OBJ_YH;
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+88); alu_add(gb, 0x06);
  CYC(b_+88, b_+89); mem_wr(gb, HL, A);
  CYC(b_+89, b_+91); L = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+91, b_+93); A = 0x05;
  CYC(b_+93, b_+94); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+94, b_+95); mem_wr(gb, HL, A);
  CYC(b_+95, b_+97); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x0c);
  CYC(b_+99, b_+101); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x10);
  CYC(b_+103, b_+105); B = 0x50;
  CYC(b_+105, b_+107);
  goto subid1_func_729a;
subid1:
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+109); push_effect(gb, b_+109);
  do { uint16_t jt_ = (part43_jump_table(gb));
    if (jt_ == b_+115) goto subid1_state0;
    if (jt_ == b_+136) goto subid1_state1;
    if (jt_ == b_+161) goto subid1_state2;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+115, b_+116); H = D;
  CYC(b_+116, b_+117); L = E;
  CYC(b_+117, b_+118); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+118, b_+120); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x04);
  CYC(b_+122, b_+124); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+124, b_+126); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+126, b_+128); L = PART_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x06);
  CYC(b_+130, b_+131); L = alu_dec8(gb, L);
  CYC(b_+131, b_+133); A = 0x0a;
  CYC(b_+133, b_+134); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+134, b_+135); mem_wr(gb, HL, A);
  RET(b_+135); return;
subid1_state1:
  CALL_C(b_+136, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+139);
  if (!(F & FZ)) { RET_TAKEN(b_+139); return; }
  CYC(b_+139, b_+140);
  CYC(b_+140, b_+142); mem_wr(gb, HL, 0xb4);
  CYC(b_+142, b_+143); L = E;
  CYC(b_+143, b_+144); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+144, b_+146); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+146, b_+148); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+148, b_+150); B = 0x3c;
subid1_func_729a:
  CALL_C(b_+150, s_func_733d_hook, SYM(func_733d), b_+153);
  CALL_C(b_+153, s_objectSetVisible81, SYM(objectSetVisible81), b_+156);
  CYC(b_+156, b_+158); A = 0x72;
  CYC(b_+158, b_+161);
  TAIL(playSound_b00);
subid1_state2:
  CALL_C(b_+161, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+164);
  if (F & FZ) { CYCT(b_+164, b_+167); TAIL(partDelete); }
  CYC(b_+164, b_+167);
  CYC(b_+167, b_+170);
  TAIL(partAnimate);
subid2:
  CYC(b_+170, b_+171); A = mem_rd(gb, DE);
  CYC(b_+171, b_+172); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+172, b_+174); goto func_72be; }
  CYC(b_+172, b_+174);
seasonsFunc_10_72b2:
  CALL_C(b_+174, s_partCommon_checkOutOfBounds, SYM(partCommon_checkOutOfBounds), b_+177);
  if (F & FZ) { CYCT(b_+177, b_+180); TAIL(partDelete); }
  CYC(b_+177, b_+180);
  CALL_C(b_+180, s_objectApplyComponentSpeed, SYM(objectApplyComponentSpeed), b_+183);
  CYC(b_+183, b_+186);
  TAIL(partAnimate);
func_72be:
  CYC(b_+186, b_+188); B = 0x02;
  CALL_C(b_+188, s_checkBPartSlotsAvailable, SYM(checkBPartSlotsAvailable), b_+191);
  if (!(F & FZ)) { RET_TAKEN(b_+191); return; }
  CYC(b_+191, b_+192);
  CYC(b_+192, b_+193); H = D;
  CYC(b_+193, b_+195); L = PART_BASE + OBJ_STATE;
  CYC(b_+195, b_+196); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+196, b_+198); L = PART_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+198, b_+200); mem_wr(gb, HL, 0x06);
  CYC(b_+200, b_+201); L = alu_dec8(gb, L);
  CYC(b_+201, b_+203); A = 0x0a;
  CYC(b_+203, b_+204); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+204, b_+205); mem_wr(gb, HL, A);
  CYC(b_+205, b_+207); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+207, b_+209); mem_wr(gb, HL, 0x10);
  CYC(b_+209, b_+211); L = PART_BASE + OBJ_YH;
  CYC(b_+211, b_+212); A = mem_rd(gb, HL);
  CYC(b_+212, b_+214); alu_add(gb, 0x06);
  CYC(b_+214, b_+215); mem_wr(gb, HL, A);
  CYC(b_+215, b_+217); B = 0x3c;
  CALL_L(b_+217, part43_subid1_func_729a, b_+220);
  CYC(b_+220, b_+223); SET_BC(0x0213);
  CALL_L(b_+223, part43_func_72e9, b_+226);
  CYC(b_+226, b_+229); SET_BC(0x030d);
  CALL_C(b_+229, s_getFreePartSlot, SYM(getFreePartSlot), b_+232);
  CYC(b_+232, b_+234); mem_wr(gb, HL, 0x43);
  CYC(b_+234, b_+235); L = alu_inc8(gb, L);
  CYC(b_+235, b_+237); mem_wr(gb, HL, 0x04);
  CYC(b_+237, b_+238); L = alu_inc8(gb, L);
  CYC(b_+238, b_+239); mem_wr(gb, HL, B);
  CYC(b_+239, b_+241); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+241, b_+242); mem_wr(gb, HL, C);
  CYC(b_+242, b_+245);
  TAIL(objectCopyPosition);
subid3:
  CYC(b_+245, b_+246); A = mem_rd(gb, DE);
  CYC(b_+246, b_+247); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+247, b_+249); goto L_730b; }
  CYC(b_+247, b_+249);
  CALL_C(b_+249, s_objectApplyComponentSpeed, SYM(objectApplyComponentSpeed), b_+252);
  CYC(b_+252, b_+254); C = 0x12;
  CALL_C(b_+254, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+257);
  if (!(F & FZ)) { CYCT(b_+257, b_+260); TAIL(partAnimate); }
  CYC(b_+257, b_+260);
  CYC(b_+260, b_+263);
  TAIL(partDelete);
L_730b:
  CYC(b_+263, b_+266); SET_BC(0xff20);
  CALL_C(b_+266, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+269);
  CYC(b_+269, b_+270); L = E;
  CYC(b_+270, b_+271); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+271, b_+273); L = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+273, b_+275); mem_wr(gb, HL, 0x05);
  CYC(b_+275, b_+276); L = alu_inc8(gb, L);
  CYC(b_+276, b_+278); mem_wr(gb, HL, 0x02);
  CYC(b_+278, b_+280); B = 0x3c;
  CALL_C(b_+280, s_func_733d_hook, SYM(func_733d), b_+283);
  CALL_C(b_+283, s_objectSetVisible82, SYM(objectSetVisible82), b_+286);
  CYC(b_+286, b_+288); A = 0x01;
  CYC(b_+288, b_+291);
  TAIL(partSetAnimation);
subid4:
  CYC(b_+291, b_+292); A = mem_rd(gb, DE);
  CYC(b_+292, b_+293); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+293, b_+296); goto seasonsFunc_10_72b2; }
  CYC(b_+293, b_+296);
  CYC(b_+296, b_+297); H = D;
  CYC(b_+297, b_+298); L = E;
  CYC(b_+298, b_+299); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+299, b_+301); B = 0x3c;
  CALL_C(b_+301, s_func_733d_hook, SYM(func_733d), b_+304);
  CALL_C(b_+304, s_objectSetVisible82, SYM(objectSetVisible82), b_+307);
  CYC(b_+307, b_+309); E = PART_BASE + OBJ_VAR03;
  CYC(b_+309, b_+310); A = mem_rd(gb, DE);
  CYC(b_+310, b_+313);
  TAIL(partSetAnimation);
}

void s_func_733d_hook(GB *gb) {
  BASE(func_733d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CALL_C(b_+4, s_getPositionOffsetForVelocity, SYM(getPositionOffsetForVelocity), b_+7);
  CYC(b_+7, b_+9); E = PART_BASE + OBJ_SPEED;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); E = alu_inc8(gb, E);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); E = alu_inc8(gb, E);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); E = alu_inc8(gb, E);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

