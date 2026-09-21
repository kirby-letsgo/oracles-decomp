#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t donkeyKongFlame_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void donkeyKongFlame_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void func_6261_hook(GB *gb) {
  BASE(func_6261);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc9; // Part.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_xor(gb, 0x10);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); E = 0xc8;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_xor(gb, 0x01);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+15); TAIL(partSetAnimation); // jp
}

void func_6256_hook(GB *gb) {
  BASE(func_6256);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partCommon_getTileCollisionInFront_hook, SYM(partCommon_getTileCollisionInFront), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); func_6261_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectApplySpeed_hook, SYM(objectApplySpeed), b_+8);
  CYC(b_+8, b_+11); TAIL(partAnimate); // jp
}

void func_6248_hook(GB *gb) {
  BASE(func_6248);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x91);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); func_6256_hook(gb); return; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); SET_HL(pop_effect(gb));
  CALL_C(b_+8, objectCreatePuff_hook, SYM(objectCreatePuff), b_+11);
  CYC(b_+11, b_+14); TAIL(partDelete); // jp
}

void func_6270_hook(GB *gb) {
  BASE(func_6270);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+5);
  if (F & FC) { RET_TAKEN(b_+5); return; } // ret c
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+9, b_+11); goto L_6280; } // jr c
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x02);
  CYC(b_+13, b_+14); L = alu_dec8(gb, L);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x00);

L_6280:
  CALL_C(b_+16, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+19);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x10);
  CYC(b_+22, b_+25); SET_BC(0x1000);
  CALL_C(b_+25, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+28);
  CYC(b_+28, b_+30); alu_sub(gb, 0x19);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; } // ret nz
  CYC(b_+31, b_+32);
  CALL_C(b_+32, getRandomNumber_hook, SYM(getRandomNumber), b_+35);
  CYC(b_+35, b_+37); alu_and(gb, 0x07);
  CYC(b_+37, b_+40); SET_HL(SYM(table_629f)); // table_629f
  CYC(b_+40, b_+41); donkeyKongFlame_addAToHl_from_rst(gb, b_+41);
  CYC(b_+41, b_+43); E = 0xc5; // Part.substate
  CYC(b_+43, b_+44); A = mem_rd(gb, HL);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+46); alu_rrca(gb);
  RET(b_+46); return; // ret
}

void partCode2c_hook(GB *gb) {
  BASE(partCode2c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); partDelete_hook(gb); return; } // jp nz
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions); // wTmpcfc0 (context-dependent)
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_6169; } // jr z
  CYC(b_+7, b_+9);
  CALL_C(b_+9, objectCreatePuff_hook, SYM(objectCreatePuff), b_+12);
  CYC(b_+12, b_+15); TAIL(partDelete); // jp

L_6169:
  CYC(b_+15, b_+17); E = 0xc4; // Part.state
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  {
    CYC(b_+18, b_+19); push_effect(gb, b_+19);
    uint16_t target = donkeyKongFlame_jump_table(gb);
    if (target == b_+29) goto state0;
    if (target == b_+82) goto state1;
    if (target == b_+104) goto state2;
    if (target == b_+120) goto state3;
    goto state4;
  }

state0:
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+31); L = E;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+32, b_+34); L = 0xd0; // Part.speed
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x1e);
  CYC(b_+36, b_+38); E = 0xc2; // Part.subid
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+41); A = alu_swap(gb, A);
  CYC(b_+41, b_+43); alu_add(gb, 0x08);
  CYC(b_+43, b_+45); L = 0xc9; // Part.angle
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+48); alu_bit(gb, 4, A);
  if (F & FZ) { CYCT(b_+48, b_+50); goto L_61a7; } // jr z
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); L = 0xcb; // Part.yh
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0xfe);
  CALL_C(b_+54, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+57);
  CYC(b_+57, b_+59); alu_and(gb, 0x07);
  CYC(b_+59, b_+62); SET_HL(SYM(table_629f)); // table_629f
  CYC(b_+62, b_+63); donkeyKongFlame_addAToHl_from_rst(gb, b_+63);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+67); SET_HL(b_+80); // table_61aa
  CYC(b_+67, b_+68); donkeyKongFlame_addAToHl_from_rst(gb, b_+68);
  CYC(b_+68, b_+70); E = 0xcd; // Part.xh
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+74); A = 0x01;
  CALL_C(b_+74, partSetAnimation_hook, SYM(partSetAnimation), b_+77);

L_61a7:
  CYC(b_+77, b_+82); TAIL(objectSetVisible82); // jp

state1:
  CYC(b_+82, b_+84); A = 0x20;
  CALL_C(b_+84, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+87);
  if (!(F & FC)) { CYCT(b_+87, b_+89); goto animate; } // jr nc
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = 0xc4; // Part.state
  CYC(b_+92, b_+93); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+93, b_+95); L = 0xf1;
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x04);
  CYC(b_+97, b_+99); L = 0xd4; // Part.speedZ
  CYC(b_+99, b_+100); alu_xor(gb, A);
  CYC(b_+100, b_+101); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+101, b_+102); mem_wr(gb, HL, A);
  CYC(b_+102, b_+104); goto animate; // jr

state2:
  CYC(b_+104, b_+105); H = D;
  CYC(b_+105, b_+107); L = 0xf1;
  CYC(b_+107, b_+108); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+108, b_+110); goto animate; } // jr nz
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); mem_wr(gb, HL, 0x04);
  CYC(b_+112, b_+113); L = E;
  CYC(b_+113, b_+114); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+114, b_+115); L = alu_inc8(gb, L);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x00);

animate:
  CYC(b_+117, b_+120); TAIL(partAnimate); // jp

state3:
  CYC(b_+120, b_+122); E = 0xc5; // Part.substate
  CYC(b_+122, b_+123); A = mem_rd(gb, DE);
  {
    CYC(b_+123, b_+124); push_effect(gb, b_+124);
    uint16_t target = donkeyKongFlame_jump_table(gb);
    if (target == b_+128) goto substate0;
    goto substate1;
  }

substate0:
  CALL_C(b_+128, func_6248_hook, SYM(func_6248), b_+131);
  CALL_C(b_+131, func_6270_hook, SYM(func_6270), b_+134);
  if (F & FC) { RET_TAKEN(b_+134); return; } // ret c
  CYC(b_+134, b_+135);
  CYC(b_+135, b_+136); H = D;
  CYC(b_+136, b_+138); L = 0xc4; // Part.state
  CYC(b_+138, b_+139); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+139); return; // ret

substate1:
  CYC(b_+140, b_+143); SET_BC(0x1000);
  CALL_C(b_+143, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+146);
  CYC(b_+146, b_+148); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(b_+148, b_+151); func_6248_hook(gb); return; } // jp z
  CYC(b_+148, b_+151);
  CYC(b_+151, b_+152); H = D;
  CYC(b_+152, b_+154); L = 0xc5; // Part.substate
  CYC(b_+154, b_+155); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+155, b_+157); L = 0xc6; // Part.counter1
  CYC(b_+157, b_+158); alu_xor(gb, A);
  CYC(b_+158, b_+159); mem_wr(gb, HL, A);
  CYC(b_+159, b_+161); L = 0xd4; // Part.speedZ
  CYC(b_+161, b_+162); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+162, b_+163); mem_wr(gb, HL, A);
  CYC(b_+163, b_+165); goto substate0; // jr

state4:
  CYC(b_+165, b_+167); E = 0xcb; // Part.yh
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+170); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(b_+170, b_+173); partDelete_hook(gb); return; } // jp nc
  CYC(b_+170, b_+173);
  CALL_C(b_+173, func_6248_hook, SYM(func_6248), b_+176);
  CALL_C(b_+176, func_6270_hook, SYM(func_6270), b_+179);
  if (!(F & FC)) { RET_TAKEN(b_+179); return; } // ret nc
  CYC(b_+179, b_+180);
  CYC(b_+180, b_+181); H = D;
  CYC(b_+181, b_+183); L = 0xc4; // Part.state
  CYC(b_+183, b_+185); mem_wr(gb, HL, 0x02);
  CYC(b_+185, b_+186); alu_xor(gb, A);
  CYC(b_+186, b_+188); L = 0xd4; // Part.speedZ
  CYC(b_+188, b_+189); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+189, b_+190); mem_wr(gb, HL, A);
  CYC(b_+190, b_+192); L = 0xc6; // Part.counter1
  CYC(b_+192, b_+193); mem_wr(gb, HL, A);
  CYC(b_+193, b_+195); E = 0xc2; // Part.subid
  CYC(b_+195, b_+196); A = mem_rd(gb, DE);
  CYC(b_+196, b_+198); A = alu_swap(gb, A);
  CYC(b_+198, b_+199); alu_rrca(gb);
  CYC(b_+199, b_+200); L = alu_inc8(gb, L);
  CYC(b_+200, b_+201); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+201, b_+202); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+202, b_+205); SET_BC(SYM(table_6238)); // table_6238
  CALL_C(b_+205, addAToBc_hook, 0x006d, b_+208);
  CYC(b_+208, b_+210); L = 0xc9; // Part.angle
  CYC(b_+210, b_+211); A = mem_rd(gb, BC);
  CYC(b_+211, b_+212); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+212, b_+214); alu_and(gb, 0x10);
  CYC(b_+214, b_+216); A = alu_swap(gb, A);
  CYC(b_+216, b_+217); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+217); return; } // ret z
  CYC(b_+217, b_+218);
  CYC(b_+218, b_+219); mem_wr(gb, HL, A);
  CYC(b_+219, b_+222); TAIL(partSetAnimation); // jp
}
