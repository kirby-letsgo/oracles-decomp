#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t enemyCode16_jump_table(GB *gb) {
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

static void enemyCode16_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// 0d:53f1, called once from enemyCode16's @state8; increments angle every 5 frames.
void enemyCode16_updateAngle_hook(GB *gb) {
  BASE(enemyCode16);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+86, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+89);
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; } // ret nz
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x05);
  CYC(b_+92, b_+94); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+94, b_+95); A = mem_rd(gb, HL);
  CYC(b_+95, b_+96); A = alu_inc8(gb, A);
  CYC(b_+96, b_+98); alu_and(gb, 0x1f);
  CYC(b_+98, b_+99); mem_wr(gb, HL, A);
  CYC(b_+99, b_+102); SET_HL(b_+107); // @angleToAnimation (data)
  CYC(b_+102, b_+103); enemyCode16_addAToHl_from_rst(gb, b_+103);
  CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+107); enemySetAnimation_hook(gb); return; // jp
}

// ==================================================================================================
// ENEMY_BEAMOS
// ==================================================================================================
void enemyCode16_hook(GB *gb) {
  BASE(enemyCode16);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);

normalStatus:
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  {
    CYC(b_+8, b_+9); push_effect(gb, b_+9);
    uint16_t target = enemyCode16_jump_table(gb);
    if (target == b_+29) goto state_uninitialized;
    if (target == b_+39) goto state_stub;
    if (target == b_+40) goto state8;
    if (target == b_+49) goto state9;
    HANDOFF(target);
  }

state_uninitialized:
  CALL_C(b_+29, ecom_setSpeedAndState8AndVisible_b0d_hook, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+32);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x05);
  CYC(b_+36, b_+39); objectMakeTileSolid_hook(gb); return; // jp

state_stub:
  RET(b_+39); return; // ret

state8:
  CALL_C(b_+40, enemyCode16_updateAngle_hook, b_+86, b_+43);
  CALL_C(b_+43, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+46);
  if (!(F & FZ)) { RET_TAKEN(b_+46); return; } // ret nz
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); goto checkFireBeam; // jr

state9:
  CALL_C(b_+49, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+52);
  if (!(F & FZ)) { CYCT(b_+52, b_+54); goto playBeamSound; } // jr nz
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x05); // [counter1] = 5
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 40); // [counter2] = 40
  CYC(b_+59, b_+60); L = E;
  CYC(b_+60, b_+61); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state] = 8
  RET(b_+61); return; // ret

playBeamSound:
  CYC(b_+62, b_+63); A = mem_rd(gb, HL);
  CYC(b_+63, b_+65); alu_cp(gb, 0x0b);
  CYC(b_+65, b_+67); A = 0xa4; // SND_BEAM
  if (F & FZ) { CYCT(b_+67, b_+70); playSound_b00_hook(gb); return; } // jp z
  CYC(b_+67, b_+70);
  if (!(F & FC)) { RET_TAKEN(b_+70); return; } // ret nc
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+73); B = 0x29; // PART_BEAM
  CALL_C(b_+73, ecom_spawnProjectile_b0d_hook, SYM(ecom_spawnProjectile_b0d), b_+76);
  if (!(F & FZ)) { RET_TAKEN(b_+76); return; } // ret nz
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+79, b_+80); A = mem_rd(gb, DE);
  CYC(b_+80, b_+82); alu_and(gb, 0x01);
  CYC(b_+82, b_+84); L = PART_BASE + OBJ_SUBID;
  CYC(b_+84, b_+85); mem_wr(gb, HL, A);
  RET(b_+85); return; // ret

checkFireBeam:
  CALL_C(b_+139, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+142);
  CYC(b_+142, b_+143); H = D;
  CYC(b_+143, b_+145); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+145, b_+146); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+146, b_+147); A = alu_inc8(gb, A);
  CYC(b_+147, b_+149); alu_cp(gb, 0x02);
  if (!(F & FC)) { RET_TAKEN(b_+149); return; } // ret nc
  CYC(b_+149, b_+150);
  CYC(b_+150, b_+152); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+152, b_+154); mem_wr(gb, HL, 20);
  CYC(b_+154, b_+156); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+156, b_+158); mem_wr(gb, HL, 0x14);
  CYC(b_+158, b_+160); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+160, b_+161); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+161); return; // ret
}
