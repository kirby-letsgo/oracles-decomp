#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t vireProjectile_jump_table(GB *gb) {
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

static void vireProjectile_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode3a_hook(GB *gb);
void vireProjectile_func_6d22_hook(GB *gb);
void func_6e2f_hook(GB *gb);
void func_6e4a_hook(GB *gb);
void func_6e50_hook(GB *gb);
void func_6e5d_hook(GB *gb);

void vireProjectile_func_6d22_hook(GB *gb) {
  BASE(partCode3a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+88, getFreePartSlot_hook, SYM(getFreePartSlot), b_+91);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x3a); // PART_VIRE_PROJECTILE
  CYC(b_+93, b_+94); L = alu_inc8(gb, L);
  CYC(b_+94, b_+95); mem_wr(gb, HL, E);
  CYC(b_+95, b_+96); L = alu_inc8(gb, L);
  CYC(b_+96, b_+97); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+97, b_+99); L = 0xc9; // Part.angle
  CYC(b_+99, b_+100); mem_wr(gb, HL, B);
  CYC(b_+100, b_+102); L = 0xd6; // Part.relatedObj1
  CYC(b_+102, b_+103); E = L;
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+105); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+105, b_+106); E = alu_inc8(gb, E);
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A);
  CYC(b_+108, b_+111); objectCopyPosition_hook(gb); return; // jp
}

void partCode3a_hook(GB *gb) {
  BASE(partCode3a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_6cd9; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+7, b_+9); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+9, b_+12); partDelete_hook(gb); return; } // jp c
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+15); func_6e4a_hook(gb); return; // jp

L_6cd9:
  CYC(b_+15, b_+17); E = 0xc2; // Part.subid
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); E = 0xc4; // Part.state
  {
    CYC(b_+20, b_+21); push_effect(gb, b_+21);
    uint16_t target = vireProjectile_jump_table(gb);
    if (target == b_+29) goto subid0;
    if (target == b_+60) goto subid1;
    if (target == b_+111) goto subid2;
    goto subid3;
  }

subid0:
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+31, b_+33); goto subid0_state0; } // jr z
  CYC(b_+31, b_+33);
func_6ceb:
  CALL_C(b_+33, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+36);
  if (F & FZ) { CYCT(b_+36, b_+39); partDelete_hook(gb); return; } // jp z
  CYC(b_+36, b_+39);
  CALL_C(b_+39, objectApplySpeed_hook, SYM(objectApplySpeed), b_+42);
  CYC(b_+42, b_+45); partAnimate_hook(gb); return; // jp

subid0_state0:
  CALL_C(b_+45, func_6e50_hook, SYM(func_6e50), b_+48);
  CALL_C(b_+48, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+51);
  CYC(b_+51, b_+53); E = 0xc9; // Part.angle
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CALL_C(b_+54, func_6e5d_hook, SYM(func_6e5d), b_+57);
  CYC(b_+57, b_+60); objectSetVisible80_hook(gb); return; // jp

subid1:
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+62, b_+64); goto func_6ceb; } // jr nz
  CYC(b_+62, b_+64);
  CALL_C(b_+64, func_6e50_hook, SYM(func_6e50), b_+67);
  CALL_C(b_+67, func_6e2f_hook, SYM(func_6e2f), b_+70);
  CYC(b_+70, b_+72); E = 0xc3; // Part.var03
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+74); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+74); return; } // ret nz
  CYC(b_+74, b_+75);
  CALL_C(b_+75, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+78);
  CYC(b_+78, b_+80); E = 0xc9; // Part.angle
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  CYC(b_+81, b_+83); alu_sub(gb, 0x02);
  CYC(b_+83, b_+85); alu_and(gb, 0x1f);
  CYC(b_+85, b_+86); B = A;
  CYC(b_+86, b_+88); E = 0x01;
  vireProjectile_func_6d22_hook(gb); return; // falls through to func_6d22

subid2:
  CYC(b_+111, b_+112); A = mem_rd(gb, DE);
  {
    CYC(b_+112, b_+113); push_effect(gb, b_+113);
    uint16_t target = vireProjectile_jump_table(gb);
    if (target == b_+121) goto subid2_state0;
    if (target == b_+186) goto subid2_state1;
    if (target == b_+252) goto subid2_state2;
    goto func_6ceb;
  }

subid2_state0:
  CYC(b_+121, b_+122); H = D;
  CYC(b_+122, b_+124); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+124, b_+126); A = 0x03;
  CYC(b_+126, b_+127); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+127, b_+128); mem_wr(gb, HL, A);
  CYC(b_+128, b_+130); L = 0xc3; // Part.var03
  CYC(b_+130, b_+131); A = mem_rd(gb, HL);
  CYC(b_+131, b_+132); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+132, b_+134); goto fimc_6d5e; } // jr z
  CYC(b_+132, b_+134);
  CYC(b_+134, b_+135); L = E;
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x03);
  CALL_C(b_+137, func_6e5d_hook, SYM(func_6e5d), b_+140);
  CYC(b_+140, b_+142); A = 0x01;
  CALL_C(b_+142, partSetAnimation_hook, SYM(partSetAnimation), b_+145);
  CYC(b_+145, b_+148); objectSetVisible82_hook(gb); return; // jp

fimc_6d5e:
  CALL_C(b_+148, func_6e50_hook, SYM(func_6e50), b_+151);
  CYC(b_+151, b_+153); L = 0xf0; // Part.speedZ
  CYC(b_+153, b_+155); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+155, b_+156); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+156, b_+158); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+158, b_+159); mem_wr(gb, HL, A);
  CYC(b_+159, b_+161); A = 0x29; // Object.health
  CALL_C(b_+161, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+164);
  CYC(b_+164, b_+165); A = mem_rd(gb, HL);
  CYC(b_+165, b_+167); B = 0x19;
  CYC(b_+167, b_+169); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+169, b_+171); goto L_6d7d; } // jr nc
  CYC(b_+169, b_+171);
  CYC(b_+171, b_+173); B = 0x2d;
  CYC(b_+173, b_+175); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+175, b_+177); goto L_6d7d; } // jr nc
  CYC(b_+175, b_+177);
  CYC(b_+177, b_+179); B = 0x41;
L_6d7d:
  CYC(b_+179, b_+181); E = 0xd0; // Part.speed
  CYC(b_+181, b_+182); A = B;
  CYC(b_+182, b_+183); mem_wr(gb, DE, A);
  CYC(b_+183, b_+186); objectSetVisible80_hook(gb); return; // jp

subid2_state1:
  CYC(b_+186, b_+187); H = D;
  CYC(b_+187, b_+189); L = 0xf0; // Part.speedZ
  CYC(b_+189, b_+190); B = mem_rd(gb, HL);
  CYC(b_+190, b_+191); L = alu_inc8(gb, L);
  CYC(b_+191, b_+192); C = mem_rd(gb, HL);
  CYC(b_+192, b_+194); L = 0xcb; // Part.yh
  CYC(b_+194, b_+195); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+195, b_+197); mem_wr(gb, hFF8F, A);
  CYC(b_+197, b_+198); L = alu_inc8(gb, L);
  CYC(b_+198, b_+199); A = mem_rd(gb, HL);
  CYC(b_+199, b_+201); mem_wr(gb, hFF8E, A);
  CYC(b_+201, b_+202); alu_sub(gb, C);
  CYC(b_+202, b_+204); alu_add(gb, 0x02);
  CYC(b_+204, b_+206); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+206, b_+208); goto func_6dba; } // jr nc
  CYC(b_+206, b_+208);
  CYC(b_+208, b_+210); A = mem_rd(gb, hFF8F);
  CYC(b_+210, b_+211); alu_sub(gb, B);
  CYC(b_+211, b_+213); alu_add(gb, 0x02);
  CYC(b_+213, b_+215); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+215, b_+217); goto func_6dba; } // jr nc
  CYC(b_+215, b_+217);
  CYC(b_+217, b_+220); SET_BC(0x0502); // INTERAC_PUFF, $02
  CALL_C(b_+220, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+223);
  if (!(F & FZ)) { RET_TAKEN(b_+223); return; } // ret nz
  CYC(b_+223, b_+224);
  CYC(b_+224, b_+226); E = 0xd8;
  CYC(b_+226, b_+228); A = 0x40;
  CYC(b_+228, b_+229); mem_wr(gb, DE, A);
  CYC(b_+229, b_+230); E = alu_inc8(gb, E);
  CYC(b_+230, b_+231); A = H;
  CYC(b_+231, b_+232); mem_wr(gb, DE, A);
  CYC(b_+232, b_+234); E = 0xc4; // Part.state
  CYC(b_+234, b_+236); A = 0x02;
  CYC(b_+236, b_+237); mem_wr(gb, DE, A);
  CYC(b_+237, b_+240); objectSetInvisible_hook(gb); return; // jp

func_6dba:
  CALL_C(b_+240, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+243);
  CYC(b_+243, b_+245); E = 0xc9; // Part.angle
  CYC(b_+245, b_+246); mem_wr(gb, DE, A);
  CALL_C(b_+246, objectApplySpeed_hook, SYM(objectApplySpeed), b_+249);
  CYC(b_+249, b_+252); partAnimate_hook(gb); return; // jp

subid2_state2:
  CYC(b_+252, b_+254); A = 0x21;
  CALL_C(b_+254, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+257);
  CYC(b_+257, b_+259); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+259); return; } // ret z
  CYC(b_+259, b_+260);
  CYC(b_+260, b_+262); B = 0x05;
  CALL_C(b_+262, checkBPartSlotsAvailable_hook, SYM(checkBPartSlotsAvailable), b_+265);
  if (!(F & FZ)) { RET_TAKEN(b_+265); return; } // ret nz
  CYC(b_+265, b_+266);
  CYC(b_+266, b_+268); C = 0x05;
L_6dd6:
  CYC(b_+268, b_+269); A = C;
  CYC(b_+269, b_+270); A = alu_dec8(gb, A);
  CYC(b_+270, b_+273); SET_HL(b_+302); // @table_6df8
  CYC(b_+273, b_+274); vireProjectile_addAToHl_from_rst(gb, b_+274);
  CYC(b_+274, b_+275); B = mem_rd(gb, HL);
  CYC(b_+275, b_+277); E = 0x02;
  CYC(b_+277, b_+280); push_effect(gb, b_+280); vireProjectile_func_6d22_hook(gb);
  CYC(b_+280, b_+281); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+281, b_+283); goto L_6dd6; } // jr nz
  CYC(b_+281, b_+283);
  CYC(b_+283, b_+284); H = D;
  CYC(b_+284, b_+286); L = 0xc4; // Part.state
  CYC(b_+286, b_+287); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+287, b_+289); L = 0xc9; // Part.angle
  CYC(b_+289, b_+291); mem_wr(gb, HL, 0x1d);
  CALL_C(b_+291, func_6e5d_hook, SYM(func_6e5d), b_+294);
  CYC(b_+294, b_+296); A = 0x01;
  CALL_C(b_+296, partSetAnimation_hook, SYM(partSetAnimation), b_+299);
  CYC(b_+299, b_+302); objectSetVisible82_hook(gb); return; // jp

subid3:
  CYC(b_+307, b_+308); A = mem_rd(gb, DE);
  CYC(b_+308, b_+309); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+309, b_+311); goto subid3_state0; } // jr z
  CYC(b_+309, b_+311);
  CALL_C(b_+311, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+314);
  if (F & FZ) { CYCT(b_+314, b_+317); func_6e4a_hook(gb); return; } // jp z
  CYC(b_+314, b_+317);
  CYC(b_+317, b_+318); L = alu_inc8(gb, L);
  CYC(b_+318, b_+319); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+319, b_+321); goto L_6e13; } // jr nz
  CYC(b_+319, b_+321);
  CYC(b_+321, b_+323); mem_wr(gb, HL, 0x07);
  CALL_C(b_+323, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+326);
  CALL_C(b_+326, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+329);
L_6e13:
  CALL_C(b_+329, objectApplySpeed_hook, SYM(objectApplySpeed), b_+332);
  CYC(b_+332, b_+335); partAnimate_hook(gb); return; // jp

subid3_state0:
  CALL_C(b_+335, func_6e50_hook, SYM(func_6e50), b_+338);
  CYC(b_+338, b_+340); L = 0xc6; // Part.counter1
  CYC(b_+340, b_+342); mem_wr(gb, HL, 0xf0);
  CYC(b_+342, b_+343); L = alu_inc8(gb, L);
  CYC(b_+343, b_+345); mem_wr(gb, HL, 0x07);
  CYC(b_+345, b_+347); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+347, b_+349); A = 0x02;
  CYC(b_+349, b_+350); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+350, b_+351); mem_wr(gb, HL, A);
  CALL_C(b_+351, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+354);
  CYC(b_+354, b_+356); E = 0xc9; // Part.angle
  CYC(b_+356, b_+357); mem_wr(gb, DE, A);
  func_6e2f_hook(gb); // falls through
}

void func_6e2f_hook(GB *gb) {
  BASE(func_6e2f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29; // Object.health
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); B = 0x1e;
  CYC(b_+8, b_+10); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto L_6e43; } // jr nc
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); B = 0x2d;
  CYC(b_+14, b_+16); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto L_6e43; } // jr nc
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); B = 0x3c;
L_6e43:
  CYC(b_+20, b_+22); E = 0xd0; // Part.speed
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+27); objectSetVisible80_hook(gb); return; // jp
}

void func_6e4a_hook(GB *gb) {
  BASE(func_6e4a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCreatePuff_hook, SYM(objectCreatePuff), b_+3);
  CYC(b_+3, b_+6); partDelete_hook(gb); return; // jp
}

void func_6e50_hook(GB *gb) {
  BASE(func_6e50);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E; // Part.state
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xcf; // Part.zh
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+10); L = 0xcb; // Part.yh
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  RET(b_+12); return; // ret
}

void func_6e5d_hook(GB *gb) {
  BASE(func_6e5d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29; // Object.health
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); B = 0x3c;
  CYC(b_+8, b_+10); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto L_6e71; } // jr nc
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); B = 0x5a;
  CYC(b_+14, b_+16); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto L_6e71; } // jr nc
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); B = 0x78;
L_6e71:
  CYC(b_+20, b_+22); E = 0xd0; // Part.speed
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  RET(b_+24); return; // ret
}
