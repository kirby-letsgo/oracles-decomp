#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6a51, 0x6a53); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a53, 0x6a54); A = mem_rd(gb, DE);
  CYC(0x6a54, 0x6a55); alu_or(gb, A);
  CYC(0x6a55, 0x6a57); E = ENEMY_BASE + 0x31; // Enemy.var31
  if (F & FZ) { CYCT(0x6a57, 0x6a59); ganonRevivalCutscene_controller_hook(gb); return; } // jr z
  CYC(0x6a57, 0x6a59);
  CYC(0x6a59, 0x6a5a); A = mem_rd(gb, DE);
  CYC(0x6a5a, 0x6a5b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6a5b, 0x6a5d); label_266_hook(gb); return; } // jr nz
  CYC(0x6a5b, 0x6a5d);
  CYC(0x6a5d, 0x6a5e); H = D;
  CYC(0x6a5e, 0x6a5f); L = E;
  CYC(0x6a5f, 0x6a60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6a60, 0x6a62); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6a62, 0x6a64); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(0x6a64, objectSetVisible83_hook, 0x1e72, 0x6a67);
  CYC(0x6a67, 0x6a69); A = 0xd3; // SND_WIND
  CALL_C(0x6a69, playSound_b00_hook, 0x0c98, 0x6a6c);
  label_266_hook(gb); return; // fallthrough
}

// 0e:6a6c, bare global; falls into from enemyCode60, also reached by genuine jr from same.
void label_266_hook(GB *gb) {
  CYC(0x6a6c, 0x6a6f); SET_BC(0x5478);
  CYC(0x6a6f, 0x6a71); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6a71, 0x6a72); A = mem_rd(gb, DE);
  CYC(0x6a72, 0x6a74); hram_wr(gb, (uint8_t)hFF8F, A);
  CYC(0x6a74, 0x6a76); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6a76, 0x6a77); A = mem_rd(gb, DE);
  CYC(0x6a77, 0x6a79); hram_wr(gb, (uint8_t)hFF8E, A);
  CYC(0x6a79, 0x6a7a); alu_sub(gb, C);
  CYC(0x6a7a, 0x6a7c); alu_add(gb, 0x08);
  CYC(0x6a7c, 0x6a7e); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(0x6a7e, 0x6a80); goto checkOtherAxis; } // jr nc
  CYC(0x6a7e, 0x6a80);
  CYC(0x6a80, 0x6a82); A = hram_rd(gb, (uint8_t)hFF8F);
  CYC(0x6a82, 0x6a83); alu_sub(gb, B);
  CYC(0x6a83, 0x6a85); alu_add(gb, 0x08);
  CYC(0x6a85, 0x6a87); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(0x6a87, 0x6a8a); enemyDelete_hook(gb); return; } // jp c
  CYC(0x6a87, 0x6a8a);

checkOtherAxis:
  label_267_hook(gb); return; // fallthrough
}

// 0e:6a8a, bare global; falls into from label_266.
void label_267_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a8a, 0x6a8d); A = mem_rd(gb, wFrameCounter);
  CYC(0x6a8d, 0x6a8f); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x6a8f, 0x6a91); goto skipNudge; } // jr nz
  CYC(0x6a8f, 0x6a91);
  CALL_C(0x6a91, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x6a94);
  CALL_C(0x6a94, objectNudgeAngleTowards_hook, 0x1fd4, 0x6a97);

skipNudge:
  CALL_C(0x6a97, objectApplySpeed_hook, 0x201d, 0x6a9a);
  CYC(0x6a9a, 0x6a9d); ecom_flickerVisibility_b0e_hook(gb); return; // jp
}

// 0e:6a9d, bare global; called from enemyCode60.
void ganonRevivalCutscene_controller_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a9d, 0x6a9e); A = mem_rd(gb, DE); // [var31]
  CYC(0x6a9e, 0x6a9f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6a9f, 0x6aa1); label_270_hook(gb); return; } // jr nz
  CYC(0x6a9f, 0x6aa1);
  CYC(0x6aa1, 0x6aa4); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6aa4, 0x6aa5); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6aa5); return; } // ret nz
  CYC(0x6aa5, 0x6aa6);
  CYC(0x6aa6, 0x6aa7); H = D;
  CYC(0x6aa7, 0x6aa8); L = E;
  CYC(0x6aa8, 0x6aa9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [var31] = 1
  CYC(0x6aa9, 0x6aab); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6aab, 0x6aad); mem_wr(gb, HL, 0x28);
  CALL_C(0x6aad, hideStatusBar_hook, 0x1aa0, 0x6ab0);
  CYC(0x6ab0, 0x6ab2); D = hram_rd(gb, (uint8_t)hActiveObject);
  CYC(0x6ab2, 0x6ab3); A = 0x0e;
  CALL_C(0x6ab3, fadeoutToBlackWithDelay_hook, 0x32ab, 0x6ab8);
  CYC(0x6ab8, 0x6ab9); alu_xor(gb, A);
  CYC(0x6ab9, 0x6abc); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x6abc, 0x6abf); mem_wr(gb, wFadeSprPaletteSources, A);
  label_270_hook(gb); return; // fallthrough
}

// 0e:6abf, bare global; falls into from ganonRevivalCutscene_controller.
void label_270_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6abf, ecom_decCounter2_b0e_hook, 0x43a3, 0x6ac2);
  if (!(F & FZ)) { RET_TAKEN(0x6ac2); return; } // ret nz
  CYC(0x6ac2, 0x6ac3);
  CYC(0x6ac3, 0x6ac4); L = alu_dec8(gb, L);
  CYC(0x6ac4, 0x6ac5); A = mem_rd(gb, HL); // [counter1]
  CYC(0x6ac5, 0x6ac7); alu_cp(gb, 0x10);
  CYC(0x6ac7, 0x6ac8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (F & FC) { CYCT(0x6ac8, 0x6aca); goto delete; } // jr nc
  CYC(0x6ac8, 0x6aca);
  CALL_C(0x6aca, ganonRevivalCutscene_spawnShadow_hook, 0x6ae6, 0x6acd);
  CYC(0x6acd, 0x6acf); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6acf, 0x6ad0); A = mem_rd(gb, DE);
  CYC(0x6ad0, 0x6ad2); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6ad2, 0x6ad3); mem_wr(gb, DE, A);
  CYC(0x6ad3, 0x6ad5); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6ad5, 0x6ad6); A = mem_rd(gb, DE);
  CYC(0x6ad6, 0x6ad8); alu_sub(gb, 0x04);
  CYC(0x6ad8, 0x6ada); alu_cp(gb, 0x10);
  if (F & FC) { RET_TAKEN(0x6ada); return; } // ret c
  CYC(0x6ada, 0x6adb);
  CYC(0x6adb, 0x6adc); mem_wr(gb, DE, A);
  RET(0x6adc); return; // ret

delete:
  CYC(0x6add, 0x6adf); A = 0x06; // Object.counter1
  CALL_C(0x6adf, objectGetRelatedObject1Var_hook, 0x2160, 0x6ae2);
  CYC(0x6ae2, 0x6ae3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ae3, 0x6ae6); enemyDelete_hook(gb); return; // jp
}

// 0e:6ae6, bare global; called from label_270.
void ganonRevivalCutscene_spawnShadow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6ae6, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x6ae9);
  if (!(F & FZ)) { RET_TAKEN(0x6ae9); return; } // ret nz
  CYC(0x6ae9, 0x6aea);
  CYC(0x6aea, 0x6aec); mem_wr(gb, HL, 0x60); // ENEMY_GANON_REVIVAL_CUTSCENE
  CYC(0x6aec, 0x6aed); L = alu_inc8(gb, L);
  CYC(0x6aed, 0x6aee); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [child.subid] = 1
  CYC(0x6aee, 0x6af0); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6af0, 0x6af1); A = mem_rd(gb, DE);
  CYC(0x6af1, 0x6af3); alu_and(gb, 0x07);
  CYC(0x6af3, 0x6af4); B = A;
  CYC(0x6af4, 0x6af5); alu_add(gb, A); // add a
  CYC(0x6af5, 0x6af6); alu_add(gb, B); // add a,b
  CYC(0x6af6, 0x6af9); SET_BC(0x6b0a); // @shadowVariables
  CALL_C(0x6af9, addAToBc_hook, 0x006d, 0x6afc);
  CYC(0x6afc, 0x6afe); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6afe, 0x6aff); A = mem_rd(gb, BC);
  CYC(0x6aff, 0x6b00); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6b00, 0x6b01); L = alu_inc8(gb, L);
  CYC(0x6b01, 0x6b02); SET_BC(BC + 1); // inc bc
  CYC(0x6b02, 0x6b03); A = mem_rd(gb, BC);
  CYC(0x6b03, 0x6b04); mem_wr(gb, HL, A); // [xh]
  CYC(0x6b04, 0x6b06); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6b06, 0x6b07); SET_BC(BC + 1); // inc bc
  CYC(0x6b07, 0x6b08); A = mem_rd(gb, BC);
  CYC(0x6b08, 0x6b09); mem_wr(gb, HL, A);
  RET(0x6b09); return; // ret
}
