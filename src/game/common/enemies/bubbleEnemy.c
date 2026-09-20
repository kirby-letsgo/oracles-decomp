#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode15), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode15), (from), (to), true)

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
  BASE(enemyCode15);
  uint16_t sp0_ = gb->sp;
  CYC(b_+78, b_+81); SET_BC((SYM(_label_00_063) + 4));
  CALL_C(b_+81, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+84);
  CYC(b_+84, b_+85); alu_or(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+85); return; } // ret nz
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+88, b_+89); A = C;
  CYC(b_+89, b_+90); mem_wr(gb, DE, A);
  RET(b_+90); return; // ret
}

// 0d:5390, called once from enemyCode15's @state8.
static void enemyCode15_checkCenteredOnTile(GB *gb) {
  BASE(enemyCode15);
  CYC(b_+91, b_+92); H = D;
  CYC(b_+92, b_+94); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+94, b_+95); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+95, b_+96); B = A;
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+98); C = mem_rd(gb, HL);
  CYC(b_+98, b_+99); alu_or(gb, C);
  CYC(b_+99, b_+101); alu_and(gb, 0x07);
  RET(b_+101); return; // ret
}

// ==================================================================================================
// ENEMY_BUBBLE
// ==================================================================================================
void enemyCode15_hook(GB *gb) {
  BASE(enemyCode15);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto normalStatus; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0x39; // WHISP_RING
  CALL_C(b_+14, cpActiveRing_hook, SYM(cpActiveRing), b_+17);
  if (F & FZ) { CYCT(b_+17, b_+19); goto normalStatus; } // jr z
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 0xb4;
  CYC(b_+21, b_+24); mem_wr(gb, wSwordDisabledCounter, A);

normalStatus:
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  {
    CYC(b_+27, b_+28); push_effect(gb, b_+28);
    uint16_t target = enemyCode15_jump_table(gb);
    if (target == b_+46) goto state_uninitialized;
    if (target == b_+62) goto state_stub;
    if (target == b_+63) goto state8;
    HANDOFF(target);
  }

state_uninitialized:
  CALL_C(b_+46, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+49);
  CYC(b_+49, b_+51); alu_and(gb, 0x18);
  CYC(b_+51, b_+53); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+56); A = 0x1e; // SPEED_c0
  CALL_C(b_+56, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+59);
  CYC(b_+59, b_+62); objectSetVisible82_hook(gb); return; // jp

state_stub:
  RET(b_+62); return; // ret

state8:
  CALL_C(b_+63, enemyCode15_checkCenteredOnTile, b_+91, b_+66);
  if (F & FZ) { CALL_C(b_+66, enemyCode15_chooseNewDirection_hook, b_+78, b_+69); } else { CYC(b_+66, b_+69); } // call z
  CALL_C(b_+69, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+72);
  if (F & FZ) { CALL_C(b_+72, enemyCode15_chooseNewDirection_hook, b_+78, b_+75); } else { CYC(b_+72, b_+75); } // call z
  CYC(b_+75, b_+78); enemyAnimate_hook(gb); return; // jp
}
