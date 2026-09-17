#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x7338, 0x733a); E = 0xc4; // Part.state
  CYC(0x733a, 0x733b); A = mem_rd(gb, DE);
  {
    CYC(0x733b, 0x733c); push_effect(gb, 0x733c);
    uint16_t target = shadowHagShadow_jump_table(gb);
    if (target == 0x7364) goto state1;
    if (target == 0x7382) goto state2;
    if (target == 0x3ea1) { partDelete_hook(gb); return; }
    goto state0;
  }

state0:
  CYC(0x7344, 0x7345); H = D;
  CYC(0x7345, 0x7346); L = E;
  CYC(0x7346, 0x7347); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7347, 0x7349); L = 0xc6; // Part.counter1
  CYC(0x7349, 0x734b); mem_wr(gb, HL, 0x08);
  CYC(0x734b, 0x734d); L = 0xd0; // Part.speed
  CYC(0x734d, 0x734f); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x734f, 0x7351); E = 0xc9; // Part.angle
  CYC(0x7351, 0x7352); A = mem_rd(gb, DE);
  CYC(0x7352, 0x7355); SET_HL(0x7360); // table_7360 (angles)
  CYC(0x7355, 0x7356); shadowHagShadow_addAToHl_from_rst(gb, 0x7356);
  CYC(0x7356, 0x7357); A = mem_rd(gb, HL);
  CYC(0x7357, 0x7358); mem_wr(gb, DE, A);
  CALL_C(0x7358, objectSetVisible82_hook, 0x1e69, 0x735b);
  CYC(0x735b, 0x735d); A = 0x01;
  CYC(0x735d, 0x7360); partSetAnimation_hook(gb); return; // jp

state1:
  CYC(0x7364, 0x7366); A = 0x06; // Object.counter1
  CALL_C(0x7366, objectGetRelatedObject1Var_hook, 0x2160, 0x7369);
  CYC(0x7369, 0x736a); A = mem_rd(gb, HL);
  CYC(0x736a, 0x736b); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x736b, 0x736d); goto L_7372; } // jr nz
  CYC(0x736b, 0x736d);
  CYC(0x736d, 0x736f); E = 0xc4; // Part.state
  CYC(0x736f, 0x7371); A = 0x02;
  CYC(0x7371, 0x7372); mem_wr(gb, DE, A);

L_7372:
  CALL_C(0x7372, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7375);
  if (!(F & FZ)) { CYCT(0x7375, 0x7377); goto L_737f; } // jr nz
  CYC(0x7375, 0x7377);
  CYC(0x7377, 0x7379); mem_wr(gb, HL, 0x08);
  CALL_C(0x7379, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x737c);
  CALL_C(0x737c, objectNudgeAngleTowards_hook, 0x1fd4, 0x737f);

L_737f:
  CYC(0x737f, 0x7382); objectApplySpeed_hook(gb); return; // jp

state2:
  CYC(0x7382, 0x7384); A = 0x0b; // Object.yh
  CALL_C(0x7384, objectGetRelatedObject1Var_hook, 0x2160, 0x7387);
  CYC(0x7387, 0x7388); B = mem_rd(gb, HL);
  CYC(0x7388, 0x738a); L = 0x8d; // Enemy.xh
  CYC(0x738a, 0x738b); C = mem_rd(gb, HL);
  CYC(0x738b, 0x738d); E = 0xcb; // Part.yh
  CYC(0x738d, 0x738e); A = mem_rd(gb, DE);
  CYC(0x738e, 0x7390); mem_wr(gb, 0xff8f, A);
  CYC(0x7390, 0x7392); E = 0xcd; // Part.xh
  CYC(0x7392, 0x7393); A = mem_rd(gb, DE);
  CYC(0x7393, 0x7395); mem_wr(gb, 0xff8e, A);
  CYC(0x7395, 0x7396); alu_sub(gb, C);
  CYC(0x7396, 0x7398); alu_add(gb, 0x04);
  CYC(0x7398, 0x739a); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x739a, 0x739c); goto updateAngleAndApplySpeed; } // jr nc
  CYC(0x739a, 0x739c);
  CYC(0x739c, 0x739e); A = mem_rd(gb, 0xff8f);
  CYC(0x739e, 0x739f); alu_sub(gb, B);
  CYC(0x739f, 0x73a1); alu_add(gb, 0x04);
  CYC(0x73a1, 0x73a3); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x73a3, 0x73a5); goto updateAngleAndApplySpeed; } // jr nc
  CYC(0x73a3, 0x73a5);
  CYC(0x73a5, 0x73a7); L = 0x87; // Enemy.counter2
  CYC(0x73a7, 0x73a8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x73a8, 0x73aa); L = 0x9a; // Enemy.visible
  CYC(0x73aa, 0x73ac); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x73ac, 0x73ae); E = 0xc4; // Part.state
  CYC(0x73ae, 0x73b0); A = 0x03;
  CYC(0x73b0, 0x73b1); mem_wr(gb, DE, A);

updateAngleAndApplySpeed:
  CALL_C(0x73b1, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x73b4);
  CYC(0x73b4, 0x73b6); E = 0xc9; // Part.angle
  CYC(0x73b6, 0x73b7); mem_wr(gb, DE, A);
  CYC(0x73b7, 0x73ba); objectApplySpeed_hook(gb); return; // jp
}
