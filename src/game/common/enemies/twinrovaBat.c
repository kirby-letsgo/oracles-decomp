#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void enemyCode5e_checkInBounds_hook(GB *gb);
void enemyCode5e_updateOamFlags_hook(GB *gb);

static uint16_t twinrovaBat_jump_table(GB *gb) {
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

// ==================================================================================================
// ENEMY_TWINROVA_BAT
// ==================================================================================================
void enemyCode5e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x69d9, 0x69db); goto afterHealthCheck; } // jr z
  CYC(0x69d9, 0x69db);
  CYC(0x69db, 0x69dd); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x69dd); return; } // ret c
  CYC(0x69dd, 0x69de);
  if (F & FZ) { CYCT(0x69de, 0x69e1); enemyDie_uncounted_hook(gb); return; } // jp z
  CYC(0x69de, 0x69e1);

afterHealthCheck:
  CYC(0x69e1, 0x69e3); A = OBJ_ID; // Object.id
  CALL_C(0x69e3, objectGetRelatedObject1Var_hook, 0x2160, 0x69e6);
  CYC(0x69e6, 0x69e7); A = mem_rd(gb, HL);
  CYC(0x69e7, 0x69e9); alu_cp(gb, 0x01); // ENEMY_MERGED_TWINROVA
  if (!(F & FZ)) { CYCT(0x69e9, 0x69ec); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x69e9, 0x69ec);
  CYC(0x69ec, 0x69ee); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x69ee, 0x69ef); A = mem_rd(gb, DE);
  CYC(0x69ef, 0x69f0); A = alu_inc8(gb, A);
  CYC(0x69f0, 0x69f2); alu_and(gb, 0x1f);
  CYC(0x69f2, 0x69f4); A = 0x78; // SND_BOOMERANG
  if (F & FZ) CALL_C_CC(0x69f4, playSound_b00_hook, 0x0c98, 0x69f7); else CYC(0x69f4, 0x69f7); // call z
  CYC(0x69f7, 0x69f9); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x69f9, 0x69fa); A = mem_rd(gb, DE);
  {
    CYC(0x69fa, 0x69fb); push_effect(gb, 0x69fb);
    uint16_t target = twinrovaBat_jump_table(gb);
    if (target == 0x6a01) goto state0;
    if (target == 0x6a1a) goto state1;
    if (target == 0x6a27) goto state2;
    HANDOFF(target);
  }

state0:
  CYC(0x6a01, 0x6a02); H = D;
  CYC(0x6a02, 0x6a03); L = E;
  CYC(0x6a03, 0x6a04); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6a04, 0x6a06); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6a06, 0x6a08); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x6a08, 0x6a0a); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6a0a, 0x6a0c); mem_wr(gb, HL, 0x50);
  CALL_C(0x6a0c, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6a0f);
  CYC(0x6a0f, 0x6a11); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6a11, 0x6a12); mem_wr(gb, DE, A);
  CYC(0x6a12, 0x6a14); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CALL_C(0x6a14, playSound_b00_hook, 0x0c98, 0x6a17);
  CYC(0x6a17, 0x6a1a); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x6a1a, enemyCode5e_updateOamFlags_hook, 0x6a42, 0x6a1d);
  CALL_C(0x6a1d, ecom_decCounter2_b0e_hook, 0x43a3, 0x6a20);
  if (!(F & FZ)) { CYCT(0x6a20, 0x6a22); goto animate; } // jr nz
  CYC(0x6a20, 0x6a22);
  CYC(0x6a22, 0x6a23); L = E;
  CYC(0x6a23, 0x6a24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(0x6a24, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x6a27);

state2:
  CALL_C(0x6a27, enemyCode5e_checkInBounds_hook, 0x6a36, 0x6a2a);
  if (!(F & FC)) { CYCT(0x6a2a, 0x6a2d); enemyDelete_hook(gb); return; } // jp nc
  CYC(0x6a2a, 0x6a2d);
  CALL_C(0x6a2d, enemyCode5e_updateOamFlags_hook, 0x6a42, 0x6a30);
  CALL_C(0x6a30, objectApplySpeed_hook, 0x201d, 0x6a33);

animate:
  CYC(0x6a33, 0x6a36); enemyAnimate_hook(gb); return; // jp
}

// 0e:6a36, bare local (no exported symbol); called via genuine call/ret from
// enemyCode5e@state2.
// @return cflag c if in bounds
void enemyCode5e_checkInBounds_hook(GB *gb) {
  CYC(0x6a36, 0x6a38); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6a38, 0x6a39); A = mem_rd(gb, DE);
  CYC(0x6a39, 0x6a3b); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FC)) { RET_TAKEN(0x6a3b); return; } // ret nc
  CYC(0x6a3b, 0x6a3c);
  CYC(0x6a3c, 0x6a3e); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6a3e, 0x6a3f); A = mem_rd(gb, DE);
  CYC(0x6a3f, 0x6a41); alu_cp(gb, 0xf0); // LARGE_ROOM_WIDTH<<4
  RET(0x6a41); return; // ret
}

// 0e:6a42, bare local (no exported symbol); called via genuine call/ret from
// enemyCode5e@state1 and @state2.
void enemyCode5e_updateOamFlags_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6a42, ecom_decCounter1_b0e_hook, 0x439a, 0x6a45);
  CYC(0x6a45, 0x6a46); A = mem_rd(gb, HL);
  CYC(0x6a46, 0x6a48); alu_and(gb, 0x04);
  CYC(0x6a48, 0x6a49); alu_rrca(gb);
  CYC(0x6a49, 0x6a4a); alu_rrca(gb);
  CYC(0x6a4a, 0x6a4c); alu_add(gb, 0x02);
  CYC(0x6a4c, 0x6a4e); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x6a4e, 0x6a4f); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6a4f, 0x6a50); mem_wr(gb, HL, A);
  RET(0x6a50); return; // ret
}
