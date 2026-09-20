#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t shadowHagShadow_jump_table(GB *gb) {
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

static void shadowHagShadow_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_SHADOW_HAG_SHADOW
void partCode41_hook(GB *gb) {
  BASE(partCode41);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = shadowHagShadow_jump_table(gb);
    if (target == b_+44) goto state1;
    if (target == b_+74) goto state2;
    if (target == SYM(partDelete)) { partDelete_hook(gb); return; }
    goto state0;
  }

state0:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = 0xc6; // Part.counter1
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x08);
  CYC(b_+19, b_+21); L = 0xd0; // Part.speed
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+23, b_+25); E = 0xc9; // Part.angle
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+29); SET_HL(b_+40); // table_7360 (angles)
  CYC(b_+29, b_+30); shadowHagShadow_addAToHl_from_rst(gb, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CALL_C(b_+32, objectSetVisible82_hook, SYM(objectSetVisible82), b_+35);
  CYC(b_+35, b_+37); A = 0x01;
  CYC(b_+37, b_+40); partSetAnimation_hook(gb); return; // jp

state1:
  CYC(b_+44, b_+46); A = 0x06; // Object.counter1
  CALL_C(b_+46, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+49);
  CYC(b_+49, b_+50); A = mem_rd(gb, HL);
  CYC(b_+50, b_+51); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto L_7372; } // jr nz
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); E = 0xc4; // Part.state
  CYC(b_+55, b_+57); A = 0x02;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);

L_7372:
  CALL_C(b_+58, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+61);
  if (!(F & FZ)) { CYCT(b_+61, b_+63); goto L_737f; } // jr nz
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x08);
  CALL_C(b_+65, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+68);
  CALL_C(b_+68, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+71);

L_737f:
  CYC(b_+71, b_+74); objectApplySpeed_hook(gb); return; // jp

state2:
  CYC(b_+74, b_+76); A = 0x0b; // Object.yh
  CALL_C(b_+76, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+79);
  CYC(b_+79, b_+80); B = mem_rd(gb, HL);
  CYC(b_+80, b_+82); L = 0x8d; // Enemy.xh
  CYC(b_+82, b_+83); C = mem_rd(gb, HL);
  CYC(b_+83, b_+85); E = 0xcb; // Part.yh
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+88); mem_wr(gb, hFF8F, A);
  CYC(b_+88, b_+90); E = 0xcd; // Part.xh
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+93); mem_wr(gb, hFF8E, A);
  CYC(b_+93, b_+94); alu_sub(gb, C);
  CYC(b_+94, b_+96); alu_add(gb, 0x04);
  CYC(b_+96, b_+98); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+98, b_+100); goto updateAngleAndApplySpeed; } // jr nc
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); A = mem_rd(gb, hFF8F);
  CYC(b_+102, b_+103); alu_sub(gb, B);
  CYC(b_+103, b_+105); alu_add(gb, 0x04);
  CYC(b_+105, b_+107); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+107, b_+109); goto updateAngleAndApplySpeed; } // jr nc
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+111); L = 0x87; // Enemy.counter2
  CYC(b_+111, b_+112); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+112, b_+114); L = 0x9a; // Enemy.visible
  CYC(b_+114, b_+116); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+116, b_+118); E = 0xc4; // Part.state
  CYC(b_+118, b_+120); A = 0x03;
  CYC(b_+120, b_+121); mem_wr(gb, DE, A);

updateAngleAndApplySpeed:
  CALL_C(b_+121, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+124);
  CYC(b_+124, b_+126); E = 0xc9; // Part.angle
  CYC(b_+126, b_+127); mem_wr(gb, DE, A);
  CYC(b_+127, b_+130); objectApplySpeed_hook(gb); return; // jp
}
