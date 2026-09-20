#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void label_266_hook(GB *gb);
void label_267_hook(GB *gb);
void ganonRevivalCutscene_controller_hook(GB *gb);
void label_270_hook(GB *gb);
void ganonRevivalCutscene_spawnShadow_hook(GB *gb);

// ==================================================================================================
// ENEMY_GANON_REVIVAL_CUTSCENE
//
// Variables:
//   var30: Copied to counter2?
//   var31: Nonzero if initialization has occurred? (spawner only)
// ==================================================================================================
void enemyCode60_hook(GB *gb) {
  BASE(enemyCode60);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+6); E = ENEMY_BASE + 0x31; // Enemy.var31
  if (F & FZ) { CYCT(b_+6, b_+8); ganonRevivalCutscene_controller_hook(gb); return; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); label_266_hook(gb); return; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(b_+19, objectSetVisible83_hook, SYM(objectSetVisible83), b_+22);
  CYC(b_+22, b_+24); A = 0xd3; // SND_WIND
  CALL_C(b_+24, playSound_b00_hook, SYM(playSound_b00), SYM(label_266));
  label_266_hook(gb); return; // fallthrough
}

// 0e:6a6c, bare global; falls into from enemyCode60, also reached by genuine jr from same.
void label_266_hook(GB *gb) {
  BASE(label_266);
  CYC(b_+0, b_+3); SET_BC((SYM(enemyCode3a) + 2));
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); hram_wr(gb, (uint8_t)hFF8F, A);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); hram_wr(gb, (uint8_t)hFF8E, A);
  CYC(b_+13, b_+14); alu_sub(gb, C);
  CYC(b_+14, b_+16); alu_add(gb, 0x08);
  CYC(b_+16, b_+18); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(b_+18, b_+20); goto checkOtherAxis; } // jr nc
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); A = hram_rd(gb, (uint8_t)hFF8F);
  CYC(b_+22, b_+23); alu_sub(gb, B);
  CYC(b_+23, b_+25); alu_add(gb, 0x08);
  CYC(b_+25, b_+27); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(b_+27, b_+30); enemyDelete_hook(gb); return; } // jp c
  CYC(b_+27, b_+30);

checkOtherAxis:
  label_267_hook(gb); return; // fallthrough
}

// 0e:6a8a, bare global; falls into from label_266.
void label_267_hook(GB *gb) {
  BASE(label_267);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto skipNudge; } // jr nz
  CYC(b_+5, b_+7);
  CALL_C(b_+7, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+10);
  CALL_C(b_+10, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+13);

skipNudge:
  CALL_C(b_+13, objectApplySpeed_hook, SYM(objectApplySpeed), b_+16);
  CYC(b_+16, b_+19); ecom_flickerVisibility_b0e_hook(gb); return; // jp
}

// 0e:6a9d, bare global; called from enemyCode60.
void ganonRevivalCutscene_controller_hook(GB *gb) {
  BASE(ganonRevivalCutscene_controller);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE); // [var31]
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); label_270_hook(gb); return; } // jr nz
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+11); L = E;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [var31] = 1
  CYC(b_+12, b_+14); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x28);
  CALL_C(b_+16, hideStatusBar_hook, SYM(hideStatusBar), b_+19);
  CYC(b_+19, b_+21); D = hram_rd(gb, (uint8_t)hActiveObject);
  CYC(b_+21, b_+22); A = 0x0e;
  CALL_C(b_+22, fadeoutToBlackWithDelay_hook, SYM(fadeoutToBlackWithDelay), b_+27);
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CYC(b_+28, b_+31); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+31, b_+34); mem_wr(gb, wFadeSprPaletteSources, A);
  label_270_hook(gb); return; // fallthrough
}

// 0e:6abf, bare global; falls into from ganonRevivalCutscene_controller.
void label_270_hook(GB *gb) {
  BASE(label_270);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = alu_dec8(gb, L);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+6, b_+8); alu_cp(gb, 0x10);
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (F & FC) { CYCT(b_+9, b_+11); goto delete; } // jr nc
  CYC(b_+9, b_+11);
  CALL_C(b_+11, ganonRevivalCutscene_spawnShadow_hook, SYM(ganonRevivalCutscene_spawnShadow), b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_sub(gb, 0x04);
  CYC(b_+25, b_+27); alu_cp(gb, 0x10);
  if (F & FC) { RET_TAKEN(b_+27); return; } // ret c
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return; // ret

delete:
  CYC(b_+30, b_+32); A = 0x06; // Object.counter1
  CALL_C(b_+32, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+35);
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+36, b_+39); enemyDelete_hook(gb); return; // jp
}

// 0e:6ae6, bare global; called from label_270.
void ganonRevivalCutscene_spawnShadow_hook(GB *gb) {
  BASE(ganonRevivalCutscene_spawnShadow);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60); // ENEMY_GANON_REVIVAL_CUTSCENE
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [child.subid] = 1
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_and(gb, 0x07);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); alu_add(gb, A); // add a
  CYC(b_+15, b_+16); alu_add(gb, B); // add a,b
  CYC(b_+16, b_+19); SET_BC(b_+36); // @shadowVariables
  CALL_C(b_+19, addAToBc_hook, 0x006d, b_+22);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+24, b_+25); A = mem_rd(gb, BC);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+26, b_+27); L = alu_inc8(gb, L);
  CYC(b_+27, b_+28); SET_BC(BC + 1); // inc bc
  CYC(b_+28, b_+29); A = mem_rd(gb, BC);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); // [xh]
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+32, b_+33); SET_BC(BC + 1); // inc bc
  CYC(b_+33, b_+34); A = mem_rd(gb, BC);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);
  RET(b_+35); return; // ret
}
