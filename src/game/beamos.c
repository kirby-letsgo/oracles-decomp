#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x53f1, ecom_decCounter1_b0d_hook, 0x439a, 0x53f4);
  if (!(F & FZ)) { RET_TAKEN(0x53f4); return; } // ret nz
  CYC(0x53f4, 0x53f5);
  CYC(0x53f5, 0x53f7); mem_wr(gb, HL, 0x05);
  CYC(0x53f7, 0x53f9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x53f9, 0x53fa); A = mem_rd(gb, HL);
  CYC(0x53fa, 0x53fb); A = alu_inc8(gb, A);
  CYC(0x53fb, 0x53fd); alu_and(gb, 0x1f);
  CYC(0x53fd, 0x53fe); mem_wr(gb, HL, A);
  CYC(0x53fe, 0x5401); SET_HL(0x5406); // @angleToAnimation (data)
  CYC(0x5401, 0x5402); enemyCode16_addAToHl_from_rst(gb, 0x5402);
  CYC(0x5402, 0x5403); A = mem_rd(gb, HL);
  CYC(0x5403, 0x5406); enemySetAnimation_hook(gb); return; // jp
}

// ==================================================================================================
// ENEMY_BEAMOS
// ==================================================================================================
void enemyCode16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x539b, 0x539d); goto normalStatus; } // jr z
  CYC(0x539b, 0x539d);
  CYC(0x539d, 0x539f); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x539f); return; } // ret c
  CYC(0x539f, 0x53a0);

normalStatus:
  CYC(0x53a0, 0x53a2); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x53a2, 0x53a3); A = mem_rd(gb, DE);
  {
    CYC(0x53a3, 0x53a4); push_effect(gb, 0x53a4);
    uint16_t target = enemyCode16_jump_table(gb);
    if (target == 0x53b8) goto state_uninitialized;
    if (target == 0x53c2) goto state_stub;
    if (target == 0x53c3) goto state8;
    if (target == 0x53cc) goto state9;
    HANDOFF(target);
  }

state_uninitialized:
  CALL_C(0x53b8, ecom_setSpeedAndState8AndVisible_b0d_hook, 0x435e, 0x53bb);
  CYC(0x53bb, 0x53bd); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x53bd, 0x53bf); mem_wr(gb, HL, 0x05);
  CYC(0x53bf, 0x53c2); objectMakeTileSolid_hook(gb); return; // jp

state_stub:
  RET(0x53c2); return; // ret

state8:
  CALL_C(0x53c3, enemyCode16_updateAngle_hook, 0x53f1, 0x53c6);
  CALL_C(0x53c6, ecom_decCounter2_b0d_hook, 0x43a3, 0x53c9);
  if (!(F & FZ)) { RET_TAKEN(0x53c9); return; } // ret nz
  CYC(0x53c9, 0x53ca);
  CYC(0x53ca, 0x53cc); goto checkFireBeam; // jr

state9:
  CALL_C(0x53cc, ecom_decCounter1_b0d_hook, 0x439a, 0x53cf);
  if (!(F & FZ)) { CYCT(0x53cf, 0x53d1); goto playBeamSound; } // jr nz
  CYC(0x53cf, 0x53d1);
  CYC(0x53d1, 0x53d3); mem_wr(gb, HL, 0x05); // [counter1] = 5
  CYC(0x53d3, 0x53d4); L = alu_inc8(gb, L);
  CYC(0x53d4, 0x53d6); mem_wr(gb, HL, 40); // [counter2] = 40
  CYC(0x53d6, 0x53d7); L = E;
  CYC(0x53d7, 0x53d8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state] = 8
  RET(0x53d8); return; // ret

playBeamSound:
  CYC(0x53d9, 0x53da); A = mem_rd(gb, HL);
  CYC(0x53da, 0x53dc); alu_cp(gb, 0x0b);
  CYC(0x53dc, 0x53de); A = 0xa4; // SND_BEAM
  if (F & FZ) { CYCT(0x53de, 0x53e1); playSound_b00_hook(gb); return; } // jp z
  CYC(0x53de, 0x53e1);
  if (!(F & FC)) { RET_TAKEN(0x53e1); return; } // ret nc
  CYC(0x53e1, 0x53e2);
  CYC(0x53e2, 0x53e4); B = 0x29; // PART_BEAM
  CALL_C(0x53e4, ecom_spawnProjectile_b0d_hook, 0x437c, 0x53e7);
  if (!(F & FZ)) { RET_TAKEN(0x53e7); return; } // ret nz
  CYC(0x53e7, 0x53e8);
  CYC(0x53e8, 0x53ea); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x53ea, 0x53eb); A = mem_rd(gb, DE);
  CYC(0x53eb, 0x53ed); alu_and(gb, 0x01);
  CYC(0x53ed, 0x53ef); L = PART_BASE + OBJ_SUBID;
  CYC(0x53ef, 0x53f0); mem_wr(gb, HL, A);
  RET(0x53f0); return; // ret

checkFireBeam:
  CALL_C(0x5426, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5429);
  CYC(0x5429, 0x542a); H = D;
  CYC(0x542a, 0x542c); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x542c, 0x542d); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x542d, 0x542e); A = alu_inc8(gb, A);
  CYC(0x542e, 0x5430); alu_cp(gb, 0x02);
  if (!(F & FC)) { RET_TAKEN(0x5430); return; } // ret nc
  CYC(0x5430, 0x5431);
  CYC(0x5431, 0x5433); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5433, 0x5435); mem_wr(gb, HL, 20);
  CYC(0x5435, 0x5437); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x5437, 0x5439); mem_wr(gb, HL, 0x14);
  CYC(0x5439, 0x543b); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x543b, 0x543c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x543c); return; // ret
}
