#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode08), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode08), (from), (to), true)

static uint16_t enemyCode08_jump_table(GB *gb) {
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
// ENEMY_RIVER_ZORA
// ==================================================================================================
void enemyCode08_hook(GB *gb) {
  BASE(enemyCode08);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);

normalStatus:
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  {
    CYC(b_+11, b_+12); push_effect(gb, b_+12);
    uint16_t target = enemyCode08_jump_table(gb);
    if (target == b_+36) goto state_uninitialized;
    if (target == b_+40) goto state_stub;
    if (target == b_+41) goto state_08;
    if (target == b_+48) goto state_09;
    if (target == b_+90) goto state_0a;
    if (target == b_+106) goto state_0b;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+36, b_+38); A = 0x09;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  RET(b_+39); return; // ret

state_stub:
  RET(b_+40); return; // ret

state_08:
  CALL_C(b_+41, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+44);
  if (!(F & FZ)) { RET_TAKEN(b_+44); return; } // ret nz
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+46); L = E;
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+47); return; // ret

state_09:
  CALL_C(b_+48, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+51);
  CYC(b_+51, b_+53); alu_cp(gb, 0x98); // (SCREEN_WIDTH<<4)-8
  if (!(F & FC)) { RET_TAKEN(b_+53); return; } // ret nc
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+55); C = A;
  CYC(b_+55, b_+57); A = H8(hCameraX);
  CYC(b_+57, b_+58); alu_add(gb, C); C = A;
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+61); A = H8(hCameraY);
  CYC(b_+61, b_+62); B = A;
  CYC(b_+62, b_+64); A = H8(hRng2);
  CYC(b_+64, b_+66); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+66, b_+67); alu_add(gb, B); B = A;
  CYC(b_+67, b_+68);
  CALL_C(b_+68, checkTileAtPositionIsWater_hook, SYM(checkTileAtPositionIsWater), b_+71);
  if (!(F & FC)) { RET_TAKEN(b_+71); return; } // ret nc
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+73); C = L;
  CALL_C(b_+73, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+76);
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x30); // 48
  CYC(b_+80, b_+82); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+82, b_+83); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+83, b_+84); alu_xor(gb, A);
  CALL_C(b_+84, enemySetAnimation_hook, SYM(enemySetAnimation), b_+87);
  CYC(b_+87, b_+90); objectSetVisible83_hook(gb); return; // jp

state_0a:
  CALL_C(b_+90, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+93);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto animate; } // jr nz
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+96); L = E;
  CYC(b_+96, b_+97); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+97, b_+99); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+99, b_+101); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+101, b_+103); A = 0x01;
  CYC(b_+103, b_+106); enemySetAnimation_hook(gb); return; // jp

state_0b:
  CYC(b_+106, b_+107); H = D;
  CYC(b_+107, b_+109); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+109, b_+110); A = mem_rd(gb, HL);
  CYC(b_+110, b_+111); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+111, b_+113); goto disappear; } // jr z
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+114); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+114, b_+116); goto animate; } // jr z
  CYC(b_+114, b_+116);
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x00);
  CYC(b_+118, b_+120); B = 0x19; // PART_ZORA_FIRE
  CALL_C(b_+120, ecom_spawnProjectile_b0d_hook, SYM(ecom_spawnProjectile_b0d), b_+123);
  if (!(F & FZ)) { CYCT(b_+123, b_+125); goto animate; } // jr nz
  CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); L = PART_BASE + OBJ_SUBID; // Part.subid
  CYC(b_+127, b_+128); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

animate:
  CYC(b_+128, b_+131); enemyAnimate_hook(gb); return; // jp

disappear:
  CYC(b_+131, b_+133); A = 0x08;
  CYC(b_+133, b_+134); mem_wr(gb, DE, A);
  CYC(b_+134, b_+136); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+136, b_+138); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(b_+138, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+141);
  CYC(b_+141, b_+143); alu_and(gb, 0x1f);
  CYC(b_+143, b_+145); alu_add(gb, 0x18);
  CYC(b_+145, b_+147); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+147, b_+148); mem_wr(gb, DE, A);
  CYC(b_+148, b_+150); B = 0x03; // INTERAC_SPLASH
  CALL_C(b_+150, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+153);
  CYC(b_+153, SYM(enemyCode09)); objectSetInvisible_hook(gb); return; // jp
}
