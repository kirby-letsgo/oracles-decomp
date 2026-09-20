#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t gashaTree_jump_table(GB *gb) {
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

static void gashaTree_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void partCode17_hook(GB *gb);
void gashaTree_func_4fb2_hook(GB *gb);
void func_5010_hook(GB *gb);

void partCode17_hook(GB *gb) {
  BASE(partCode17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xc2; // Part.subid
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_add(gb, A);
  CYC(b_+6, b_+9); SET_HL(SYM(table_501e)); // table_501e
  CYC(b_+9, b_+10); gashaTree_addDoubleIndexToHl_from_rst(gb, b_+10);
  CYC(b_+10, b_+12); E = 0xea; // Part.var2a
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_and(gb, 0x1f);
  CALL_C(b_+15, checkFlag_hook, SYM(checkFlag), b_+18);
  if (F & FZ) { CYCT(b_+18, b_+20); goto normalStatus; } // jr z
  CYC(b_+18, b_+20);
  CALL_C(b_+20, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+23);
  if (!(F & FC)) { CYCT(b_+23, b_+25); goto normalStatus; } // jr nc
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); L = 0xc4; // Part.state
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x02);
  CYC(b_+30, b_+32); L = 0xe4; // Part.collisionType
  CYC(b_+32, b_+34); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+34, b_+36); L = 0xc2; // Part.subid
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+38, b_+40); goto normalStatus; } // jr z
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); A = 0x2a;
  CALL_C(b_+42, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+45);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0xff);

normalStatus:
  CYC(b_+47, b_+49); E = 0xc4; // Part.state
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  {
    CYC(b_+50, b_+51); push_effect(gb, b_+51);
    uint16_t target = gashaTree_jump_table(gb);
    if (target == b_+57) goto state0;
    if (target == b_+82) goto state1;
    goto state2;
  }

state0:
  CYC(b_+57, b_+59); A = 0x01;
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CYC(b_+60, b_+62); A = 0x26;
  CALL_C(b_+62, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+65);
  CYC(b_+65, b_+67); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+67, b_+68); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+70); E = alu_inc8(gb, E);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A); // collisionRadiusX
  CALL_C(b_+72, objectTakePosition_hook, SYM(objectTakePosition), b_+75);
  CYC(b_+75, b_+77); E = 0xf0; // Part.var30
  CYC(b_+77, b_+79); L = 0x41;
  CYC(b_+79, b_+80); A = mem_rd(gb, HL);
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  RET(b_+81); return; // ret

state1:
  CYC(b_+82, b_+85); gashaTree_func_4fb2_hook(gb);
  if (F & FZ) { RET_TAKEN(b_+85); return; } // ret z
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+89); partDelete_hook(gb); return; // jp

state2:
  CYC(b_+99, b_+102); gashaTree_func_4fb2_hook(gb);
  if (!(F & FZ)) { CYCT(b_+102, b_+105); partDelete_hook(gb); return; } // jp nz
  CYC(b_+102, b_+105);
  CYC(b_+105, b_+107); E = 0xc5; // Part.substate
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  {
    CYC(b_+108, b_+109); push_effect(gb, b_+109);
    uint16_t target = gashaTree_jump_table(gb);
    if (target == b_+115) goto substate0;
    if (target == b_+147) goto substate1;
    goto substate2;
  }

substate0:
  CYC(b_+115, b_+116); H = D;
  CYC(b_+116, b_+117); L = E;
  CYC(b_+117, b_+118); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+118, b_+120); L = 0xd0; // Part.speed
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+122, b_+124); A = 0x1a;
  CALL_C(b_+124, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+127);
  CYC(b_+127, b_+129); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(b_+129, b_+131); E = 0xc2; // Part.subid
  CYC(b_+131, b_+132); A = mem_rd(gb, DE);
  CYC(b_+132, b_+133); alu_or(gb, A);
  CYC(b_+133, b_+135); A = 0x10;
  if (!(F & FZ)) {
    CALL_C_CC(b_+135, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+138);
  } else {
    CYC(b_+135, b_+138);
  }
  CYC(b_+138, b_+140); E = 0xc9; // Part.angle
  CYC(b_+140, b_+141); mem_wr(gb, DE, A);
  CYC(b_+141, b_+144); SET_BC(0xfec0); // -0x140
  CYC(b_+144, b_+147); objectSetSpeedZ_hook(gb); return; // jp

substate1:
  CYC(b_+147, b_+149); C = 0x18;
  CALL_C(b_+149, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+152);
  if (F & FZ) { CYCT(b_+152, b_+154); goto L_4ffe; } // jr z
  CYC(b_+152, b_+154);
  CALL_C(b_+154, objectApplySpeed_hook, SYM(objectApplySpeed), b_+157);
  CYC(b_+157, b_+159); A = 0x00;
  CALL_C(b_+159, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+162);
  CYC(b_+162, b_+165); objectCopyPosition_hook(gb); return; // jp

L_4ffe:
  CYC(b_+165, b_+167); E = 0xc5; // Part.substate
  CYC(b_+167, b_+169); A = 0x02;
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);

substate2:
  CYC(b_+170, b_+172); C = 0x18;
  CALL_C(b_+172, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+175);
  if (!(F & FC)) { CYCT(b_+175, b_+177); func_5010_hook(gb); return; } // jr nc
  CYC(b_+175, b_+177);
  CALL_C(b_+177, func_5010_hook, SYM(func_5010), b_+180);
  CYC(b_+180, b_+183); partDelete_hook(gb); return; // jp
}

void gashaTree_func_4fb2_hook(GB *gb) {
  BASE(partCode17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+89, b_+91); A = 0x01;
  CALL_C(b_+91, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+94);
  CYC(b_+94, b_+96); E = 0xf0; // Part.var30
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+98, b_+99); return; // ret
}

void func_5010_hook(GB *gb) {
  BASE(func_5010);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+3);
  if (!(F & FC)) {
    CALL_C_CC(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), b_+6);
  } else {
    CYC(b_+3, b_+6);
  }
  CYC(b_+6, b_+8); A = 0x00;
  CALL_C(b_+8, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+11);
  CYC(b_+11, b_+14); objectCopyPosition_hook(gb); return; // jp
}
