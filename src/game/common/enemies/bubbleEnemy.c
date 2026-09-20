#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode15_jump_table(GB *gb) {
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

// 0d:5383, called (conditionally, twice) from enemyCode15's @state8.
void enemyCode15_chooseNewDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5383, 0x5386); SET_BC(0x0718);
  CALL_C(0x5386, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x5389);
  CYC(0x5389, 0x538a); alu_or(gb, B);
  if (!(F & FZ)) { RET_TAKEN(0x538a); return; } // ret nz
  CYC(0x538a, 0x538b);
  CYC(0x538b, 0x538d); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x538d, 0x538e); A = C;
  CYC(0x538e, 0x538f); mem_wr(gb, DE, A);
  RET(0x538f); return; // ret
}

// 0d:5390, called once from enemyCode15's @state8.
static void enemyCode15_checkCenteredOnTile(GB *gb) {
  CYC(0x5390, 0x5391); H = D;
  CYC(0x5391, 0x5393); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5393, 0x5394); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5394, 0x5395); B = A;
  CYC(0x5395, 0x5396); L = alu_inc8(gb, L);
  CYC(0x5396, 0x5397); C = mem_rd(gb, HL);
  CYC(0x5397, 0x5398); alu_or(gb, C);
  CYC(0x5398, 0x539a); alu_and(gb, 0x07);
  RET(0x539a); return; // ret
}

// ==================================================================================================
// ENEMY_BUBBLE
// ==================================================================================================
void enemyCode15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5335, 0x5337); goto normalStatus; } // jr z
  CYC(0x5335, 0x5337);
  CYC(0x5337, 0x5339); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5339); return; } // ret c
  CYC(0x5339, 0x533a);
  CYC(0x533a, 0x533c); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x533c, 0x533d); A = mem_rd(gb, DE);
  CYC(0x533d, 0x533f); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { CYCT(0x533f, 0x5341); goto normalStatus; } // jr nz
  CYC(0x533f, 0x5341);
  CYC(0x5341, 0x5343); A = 0x39; // WHISP_RING
  CALL_C(0x5343, cpActiveRing_hook, 0x23b0, 0x5346);
  if (F & FZ) { CYCT(0x5346, 0x5348); goto normalStatus; } // jr z
  CYC(0x5346, 0x5348);
  CYC(0x5348, 0x534a); A = 0xb4;
  CYC(0x534a, 0x534d); mem_wr(gb, wSwordDisabledCounter, A);

normalStatus:
  CYC(0x534d, 0x534f); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x534f, 0x5350); A = mem_rd(gb, DE);
  {
    CYC(0x5350, 0x5351); push_effect(gb, 0x5351);
    uint16_t target = enemyCode15_jump_table(gb);
    if (target == 0x5363) goto state_uninitialized;
    if (target == 0x5373) goto state_stub;
    if (target == 0x5374) goto state8;
    HANDOFF(target);
  }

state_uninitialized:
  CALL_C(0x5363, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5366);
  CYC(0x5366, 0x5368); alu_and(gb, 0x18);
  CYC(0x5368, 0x536a); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x536a, 0x536b); mem_wr(gb, DE, A);
  CYC(0x536b, 0x536d); A = 0x1e; // SPEED_c0
  CALL_C(0x536d, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x5370);
  CYC(0x5370, 0x5373); objectSetVisible82_hook(gb); return; // jp

state_stub:
  RET(0x5373); return; // ret

state8:
  CALL_C(0x5374, enemyCode15_checkCenteredOnTile, 0x5390, 0x5377);
  if (F & FZ) { CALL_C(0x5377, enemyCode15_chooseNewDirection_hook, 0x5383, 0x537a); } else { CYC(0x5377, 0x537a); } // call z
  CALL_C(0x537a, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x537d);
  if (F & FZ) { CALL_C(0x537d, enemyCode15_chooseNewDirection_hook, 0x5383, 0x5380); } else { CYC(0x537d, 0x5380); } // call z
  CYC(0x5380, 0x5383); enemyAnimate_hook(gb); return; // jp
}
