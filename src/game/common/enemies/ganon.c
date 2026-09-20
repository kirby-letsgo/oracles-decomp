#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/enemies/ganon.s (ENEMY_GANON), bank $10.

static void ganon_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void ganon_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t ganon_jump_table(GB *gb) {
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

void ganon_state_uninitialized_hook(GB *gb);
void ganon_state1_hook(GB *gb);
void ganon_state2_hook(GB *gb);
void ganon_state3_hook(GB *gb);
void ganon_state4_hook(GB *gb);
void ganon_state5_hook(GB *gb);
void ganon_state6_hook(GB *gb);
void ganon_state7_hook(GB *gb);
void ganon_state8_hook(GB *gb);
void ganon_state8_substate0_hook(GB *gb);
void ganon_state8_substate1_hook(GB *gb);
void ganon_state8_substate2_hook(GB *gb);
void ganon_state8_substate3_hook(GB *gb);
void ganon_state8_substate4_hook(GB *gb);
void ganon_state8_substate5_hook(GB *gb);
void ganon_state8_substate6_hook(GB *gb);
void ganon_state8_spawnProjectile_hook(GB *gb);
void ganon_state8_substate7_hook(GB *gb);
void ganon_state9_hook(GB *gb);
void ganon_state9_substate0_hook(GB *gb);
void ganon_state9_substate1_hook(GB *gb);
void ganon_state9_substate3_hook(GB *gb);
void ganon_state9_substate4_hook(GB *gb);
void ganon_state9_substate4__spawnProjectile_hook(GB *gb);
void ganon_state9_substate5_hook(GB *gb);
void ganon_state9_substate6_hook(GB *gb);
void ganon_state9_substate7_hook(GB *gb);
void ganon_stateA_hook(GB *gb);
void ganon_stateA_substate1_hook(GB *gb);
void ganon_stateA_substate2_hook(GB *gb);
void ganon_stateA_substate3_hook(GB *gb);
void ganon_stateA_substate4_hook(GB *gb);
void ganon_stateA_substate5_hook(GB *gb);
void ganon_stateA_substate6_hook(GB *gb);
void ganon_stateA_substate7_hook(GB *gb);
void ganon_stateB_hook(GB *gb);
void ganon_stateB_substate1_hook(GB *gb);
void ganon_stateB_substate2_hook(GB *gb);
void ganon_stateB_substate3_hook(GB *gb);
void ganon_stateB_substate4_hook(GB *gb);
void ganon_stateB_substate5_hook(GB *gb);
void ganon_stateB_substate6_hook(GB *gb);
void ganon_stateB_substate7_hook(GB *gb);
void ganon_stateB_substate8_hook(GB *gb);
void ganon_stateB_substate9_hook(GB *gb);
void ganon_stateB_substateA_hook(GB *gb);
void ganon_stateC_hook(GB *gb);
void ganon_stateC_substate1_hook(GB *gb);
void ganon_stateC_substate3_hook(GB *gb);
void ganon_stateC_substate4_hook(GB *gb);
void ganon_stateC_substate5_hook(GB *gb);
void ganon_stateC_substate6_hook(GB *gb);
void ganon_stateC_substate7_hook(GB *gb);
void ganon_stateC_substate8_hook(GB *gb);
void ganon_stateC_substate9_hook(GB *gb);
void ganon_stateC_substateA_hook(GB *gb);
void ganon_stateD_hook(GB *gb);
void ganon_stateD_substate3_hook(GB *gb);
void ganon_stateE_hook(GB *gb);
void ganon_stateE_substate0_hook(GB *gb);
void ganon_stateE_substate1_hook(GB *gb);
void ganon_stateE_substate2_hook(GB *gb);
void ganon_stateE_substate3_hook(GB *gb);
void ganon_loadGfxHeader_hook(GB *gb);
void ganon_updateTeleportAnimationGoingOut_hook(GB *gb);
void ganon_updateFlickeringXPosition_hook(GB *gb);
void ganon_updateTeleportVarsAndPlaySound_hook(GB *gb);
void ganon_updateTeleportAnimationComingIn_hook(GB *gb);
void ganon_finishAttack_hook(GB *gb);
void label_10_135_hook(GB *gb);
void ganon_decideNextMove_hook(GB *gb);
void ganon_decideTeleportLocationAndCounter_hook(GB *gb);
void ganon_spawnPart_hook(GB *gb);
void ganon_makeRoomBoundarySolid_hook(GB *gb);
void ganon_updateSeizurePalette_hook(GB *gb);
void ganon_setTileReplacementMode_hook(GB *gb);

// 10:50a3 (real call from enemyCode04, tail-jumps into whichever state root is selected,
// so this stays a plain non-hooked helper invoked via explicit push_effect).
void enemyCode04__normalStatus_hook(GB *gb) {
  BASE(enemyCode04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+70, b_+72); E = 0x84;  // ld e,Enemy.state
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+73, b_+74); push_effect(gb, b_+74);
  do { uint16_t jt_ = (ganon_jump_table(gb));
    if (jt_ == SYM(ganon_state_uninitialized)) { ganon_state_uninitialized_hook(gb); return; }
    else if (jt_ == SYM(ganon_state1)) { ganon_state1_hook(gb); return; }
    else if (jt_ == SYM(ganon_state2)) { ganon_state2_hook(gb); return; }
    else if (jt_ == SYM(ganon_state3)) { ganon_state3_hook(gb); return; }
    else if (jt_ == SYM(ganon_state4)) { ganon_state4_hook(gb); return; }
    else if (jt_ == SYM(ganon_state5)) { ganon_state5_hook(gb); return; }
    else if (jt_ == SYM(ganon_state6)) { ganon_state6_hook(gb); return; }
    else if (jt_ == SYM(ganon_state7)) { ganon_state7_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8)) { ganon_state8_hook(gb); return; }
    else if (jt_ == SYM(ganon_state9)) { ganon_state9_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateA)) { ganon_stateA_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB)) { ganon_stateB_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC)) { ganon_stateC_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateD)) { ganon_stateD_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateE)) { ganon_stateE_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void enemyCode04_hook(GB *gb) {
  BASE(enemyCode04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if ((F & FZ)) { CYCT(b_+0, b_+2); goto normalStatus; } CYC(b_+0, b_+2);  // jr z,@normalStatus
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);  // sub ENEMYSTATUS_NO_HEALTH
  if ((F & FC)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret c
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } CYC(b_+5, b_+7);  // jr nz,@normalStatus
  CALL_C(b_+7, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+10);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } CYC(b_+10, b_+11);  // ret nc
  CYC(b_+11, b_+12); H = D;  // ld h,d
  CYC(b_+12, b_+14); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x40);  // ld (hl),$40
  CYC(b_+16, b_+18); L = 0x84;  // ld l,Enemy.state
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x0e);  // ld (hl),$0e
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);  // inc l
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x00);  // ld (hl),$00 [substate]
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);  // inc l
  CYC(b_+24, b_+26); mem_wr(gb, HL, 120);  // ld (hl),120
  CYC(b_+26, b_+28); A = 0x01;  // ld a,$01
  CYC(b_+28, b_+31); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(b_+31, b_+33); A = 0x67;  // ld a,SND_BOSS_DEAD
  CALL_C(b_+33, playSound_b00_hook, SYM(playSound_b00), b_+36);
  CYC(b_+36, b_+38); A = 0xb4;  // ld a,GFXH_GANON_D
  CALL_C(b_+38, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+41);
  CYC(b_+41, b_+43); A = 0x0e;  // ld a,$0e
  CALL_C(b_+43, enemySetAnimation_hook, SYM(enemySetAnimation), b_+46);
  CALL_C(b_+46, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+49);
  CYC(b_+49, b_+51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+51, b_+53); L = 0xf1;  // ld l,<ROOM_ZELDA_IN_FINAL_DUNGEON
  CYC(b_+53, b_+55); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+55, b_+57); L = 0xf5;  // ld l,<ROOM_TWINROVA_FIGHT
  CYC(b_+57, b_+59); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+59, b_+61); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CALL_C(b_+61, playSound_b00_hook, SYM(playSound_b00), b_+64);
  CYC(b_+64, b_+67); SET_BC(0x2f0e);  // ld bc,TX_2f0e
  CYC(b_+67, b_+70); if (hook_enabled_at(SYM(showText))) { showText_hook(gb); return; } HANDOFF(SYM(showText));  // jp showText
normalStatus:
  CYC(b_+70, b_+72); push_effect(gb, b_+72); enemyCode04__normalStatus_hook(gb);  // call @normalStatus
}

void ganon_state_uninitialized_hook(GB *gb) {
  BASE(ganon_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+2); L = E;  // ld l,e
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+3, b_+5); L = 0x9d;  // ld l,Enemy.oamTileIndexBase
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+7, b_+9); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x48);  // ld (hl),$48
  CYC(b_+11, b_+13); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(b_+15, b_+17); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(b_+18, b_+21); SET_HL(w1Link_yh);  // ld hl,w1Link.yh
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x88);  // ld (hl),$88
  CYC(b_+23, b_+25); L = 0x0d;  // ld l,<w1Link.xh
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(b_+27, b_+29); L = 0x08;  // ld l,<w1Link.direction
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x00);  // ld (hl),DIR_UP
  CYC(b_+31, b_+33); L = 0x00;  // ld l,<w1Link.enabled
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x03);  // ld (hl),$03
  CYC(b_+35, b_+38); SET_HL(wLoadedObjectGfx);  // ld hl,wLoadedObjectGfx
  CYC(b_+38, b_+40); A = 0x01;  // ld a,$01
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x16);  // ld (hl),OBJ_GFXH_16
  CYC(b_+42, b_+43); L = alu_inc8(gb, L);  // inc l
  CYC(b_+43, b_+44); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x18);  // ld (hl),OBJ_GFXH_18
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);  // inc l
  CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x19);  // ld (hl),OBJ_GFXH_19
  CYC(b_+50, b_+51); L = alu_inc8(gb, L);  // inc l
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x1a);  // ld (hl),OBJ_GFXH_1a
  CYC(b_+54, b_+55); L = alu_inc8(gb, L);  // inc l
  CYC(b_+55, b_+56); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x1b);  // ld (hl),OBJ_GFXH_1b
  CYC(b_+58, b_+59); L = alu_inc8(gb, L);  // inc l
  CYC(b_+59, b_+60); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+60, b_+61); alu_xor(gb, A);  // xor a
  CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+62, b_+63); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+63, b_+64); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+64, b_+65); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+65, b_+66); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+66, b_+67); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+67, b_+70); SET_BC(0x0012);  // ld bc,$0012
  CALL_C(b_+70, enemyBoss_spawnShadow_b10_hook, SYM(enemyBoss_spawnShadow_b10), b_+73);
  CYC(b_+73, b_+75); E = 0x98;  // ld e,Enemy.relatedObj2
  CYC(b_+75, b_+77); A = 0xc0;  // ld a,Part.start
  CYC(b_+77, b_+78); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+78, b_+79); E = alu_inc8(gb, E);  // inc e
  CYC(b_+79, b_+80); A = H;  // ld a,h
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(b_+81, disableLcd_hook, SYM(disableLcd), b_+84);
  CYC(b_+84, b_+86); A = 0xf5;  // ld a,<ROOM_TWINROVA_FIGHT
  CYC(b_+86, b_+89); mem_wr(gb, wActiveRoom, A);  // ld (wActiveRoom),a
  CYC(b_+89, b_+91); A = 0x03;  // ld a,$03
  CYC(b_+91, b_+94); mem_wr(gb, wTwinrovaTileReplacementMode, A);  // ld (wTwinrovaTileReplacementMode),a
  CALL_C(b_+94, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+97);
  CALL_C(b_+97, loadTilesetData_hook, SYM(loadTilesetData), b_+100);
  CALL_C(b_+100, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+103);
  CALL_C(b_+103, func_131f_hook, SYM(func_131f), b_+106);
  CALL_C(b_+106, resetCamera_hook, SYM(resetCamera), b_+109);
  CALL_C(b_+109, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+112);
  CYC(b_+112, b_+114); A = 0x8b;  // ld a,PALH_8b
  CALL_C(b_+114, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+117);
  CYC(b_+117, b_+119); A = 0xb1;  // ld a,PALH_b1
  CYC(b_+119, b_+122); mem_wr(gb, wExtraBgPaletteHeader, A);  // ld (wExtraBgPaletteHeader),a
  CYC(b_+122, b_+124); A = 0xb0;  // ld a,GFXH_GANON_REVIVAL
  CALL_C(b_+124, loadGfxHeader_hook, SYM(loadGfxHeader), b_+127);
  CYC(b_+127, b_+129); A = 0x02;  // ld a,$02
  CALL_C(b_+129, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+132);
  CYC(b_+132, b_+134); A = mem_rd(gb, hActiveObject);  // ldh a,(<hActiveObject)
  CYC(b_+134, b_+135); D = A;  // ld d,a
  CALL_C(b_+135, objectSetVisible83_hook, SYM(objectSetVisible83), b_+138);
  CYC(b_+138, b_+141); if (hook_enabled_at(SYM(fadeinFromWhite))) { fadeinFromWhite_hook(gb); return; } HANDOFF(SYM(fadeinFromWhite));  // jp fadeinFromWhite
}

void ganon_state1_hook(GB *gb) {
  BASE(ganon_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60);  // ld (hl),ENEMY_GANON_REVIVAL_CUTSCENE
  CYC(b_+6, b_+8); L = 0x96;  // ld l,Enemy.relatedObj1
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);  // inc l
  CYC(b_+11, b_+12); mem_wr(gb, HL, D);  // ld (hl),d
  CALL_C(b_+12, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+15);
  CYC(b_+15, b_+17); L = 0x87;  // ld l,Enemy.counter2
  CYC(b_+17, b_+19); mem_wr(gb, HL, 60);  // ld (hl),60
  RET(b_+19); return;  // ret
}

void ganon_state2_hook(GB *gb) {
  BASE(ganon_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x86;  // ld e,Enemy.counter1
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret z
  CALL_C(b_+5, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); if (hook_enabled_at(SYM(ecom_flickerVisibility_b10))) { ecom_flickerVisibility_b10_hook(gb); return; } HANDOFF(SYM(ecom_flickerVisibility_b10)); } CYC(b_+8, b_+11);  // jp nz,ecom_flickerVisibility
  CYC(b_+11, b_+12); L = alu_dec8(gb, L);  // dec l
  CYC(b_+12, b_+14); mem_wr(gb, HL, 193);  // ld (hl),193
  CYC(b_+14, b_+16); L = 0x84;  // ld l,Enemy.state
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+17, b_+19); A = 0x0d;  // ld a,$0d
  CALL_C(b_+19, enemySetAnimation_hook, SYM(enemySetAnimation), b_+22);
  CYC(b_+22, b_+25); if (hook_enabled_at(SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
}

void ganon_state3_hook(GB *gb) {
  BASE(ganon_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if ((F & FZ)) { CYCT(b_+3, b_+5); goto nextState; } CYC(b_+3, b_+5);  // jr z,@nextState
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+6, b_+8); alu_and(gb, 0x3f);  // and $3f
  CYC(b_+8, b_+10); A = 0xb8;  // ld a,SND_RUMBLE2
  if ((F & FZ)) { CALL_C_CC(b_+10, playSound_b00_hook, SYM(playSound_b00), b_+13); } else CYC(b_+10, b_+13);  // call z,playSound
  CYC(b_+13, b_+16); if (hook_enabled_at(SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate));  // jp enemyAnimate
nextState:
  CYC(b_+16, b_+17); L = E;  // ld l,e
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+18, b_+20); L = 0x8f;  // ld l,$8f
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+22, b_+24); A = 0x02;  // ld a,$02
  CALL_C(b_+24, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(b_+29, b_+31); A = 0x01;  // ld a,$01
  CYC(b_+31, b_+34); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_state4_hook(GB *gb) {
  BASE(ganon_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { CYCT(b_+4, b_+7); if (hook_enabled_at(SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+4, b_+7);  // jp nz,enemyAnimate
  CALL_C(b_+7, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+10);
  CYC(b_+10, b_+12); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+12, b_+14); mem_wr(gb, HL, 15);  // ld (hl),15
  CYC(b_+14, b_+16); A = 0xb1;  // ld a,GFXH_GANON_A
  CALL_C(b_+16, loadGfxHeader_hook, SYM(loadGfxHeader), b_+19);
  CYC(b_+19, b_+21); A = 0x32;  // ld a,UNCMP_GFXH_32
  CALL_C(b_+21, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+24);
  CYC(b_+24, b_+27); SET_HL((wLoadedObjectGfx + 2));  // ld hl,wLoadedObjectGfx+2
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x17);  // ld (hl),OBJ_GFXH_17
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);  // inc l
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(b_+32, b_+34); A = mem_rd(gb, hActiveObject);  // ldh a,(<hActiveObject)
  CYC(b_+34, b_+35); D = A;  // ld d,a
  CYC(b_+35, b_+37); A = 0x02;  // ld a,$02
  CYC(b_+37, b_+40); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_state5_hook(GB *gb) {
  BASE(ganon_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); A = 120;  // ld a,120
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);  // ld (hl),a [counter1]
  CYC(b_+7, b_+10); mem_wr(gb, wScreenShakeCounterY, A);  // ld (wScreenShakeCounterY),a
  CYC(b_+10, b_+11); L = E;  // ld l,e
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+12, b_+14); A = 0x67;  // ld a,SND_BOSS_DEAD
  CALL_C(b_+14, playSound_b00_hook, SYM(playSound_b00), b_+17);
  CYC(b_+17, b_+19); A = 0x03;  // ld a,$03
  CALL_C(b_+19, enemySetAnimation_hook, SYM(enemySetAnimation), b_+22);
  CALL_C(b_+22, showStatusBar_hook, SYM(showStatusBar), b_+25);
  CYC(b_+25, b_+27); A = mem_rd(gb, hActiveObject);  // ldh a,(<hActiveObject)
  CYC(b_+27, b_+28); D = A;  // ld d,a
  CYC(b_+28, b_+31); if (hook_enabled_at(SYM(clearPaletteFadeVariablesAndRefreshPalettes))) { clearPaletteFadeVariablesAndRefreshPalettes_hook(gb); return; } HANDOFF(SYM(clearPaletteFadeVariablesAndRefreshPalettes));  // jp clearPaletteFadeVariablesAndRefreshPalettes
}

void ganon_state6_hook(GB *gb) {
  BASE(ganon_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); if (hook_enabled_at(SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+3, b_+6);  // jp nz,enemyAnimate
  CYC(b_+6, b_+8); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+13); SET_HL((wLoadedObjectGfx + 6));  // ld hl,wLoadedObjectGfx+6
  CYC(b_+13, b_+14); alu_xor(gb, A);  // xor a
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+18, b_+21); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(b_+21, b_+24); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CYC(b_+24, b_+27); SET_BC(0x2f0d);  // ld bc,TX_2f0d
  CALL_C(b_+27, showText_hook, SYM(showText), b_+30);
  CYC(b_+30, b_+32); A = 0x02;  // ld a,$02
  CYC(b_+32, b_+35); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_state7_hook(GB *gb) {
  BASE(ganon_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x34;  // ld a,MUS_GANON
  CYC(b_+2, b_+5); mem_wr(gb, wActiveMusic, A);  // ld (wActiveMusic),a
  CALL_C(b_+5, playSound_b00_hook, SYM(playSound_b00), b_+8);
  CYC(b_+8, b_+11); ganon_decideNextMove_hook(gb); return;  // jp ganon_decideNextMove
}

void ganon_state8_hook(GB *gb) {
  BASE(ganon_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ganon_jump_table(gb));
    if (jt_ == SYM(ganon_state8_substate0)) { ganon_state8_substate0_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate1)) { ganon_state8_substate1_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate2)) { ganon_state8_substate2_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate3)) { ganon_state8_substate3_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate4)) { ganon_state8_substate4_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate5)) { ganon_state8_substate5_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate6)) { ganon_state8_substate6_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate7)) { ganon_state8_substate7_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

// Also used by state D substate0.
void ganon_state8_substate0_hook(GB *gb) {
  BASE(ganon_state8_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ganon_updateTeleportVarsAndPlaySound_hook, SYM(ganon_updateTeleportVarsAndPlaySound), b_+3);
  CYC(b_+3, b_+5); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  RET(b_+6); return;  // ret
}

// Also used by state D substate1.
void ganon_state8_substate1_hook(GB *gb) {
  BASE(ganon_state8_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CALL_C(b_+8, ganon_decideTeleportLocationAndCounter_hook, SYM(ganon_decideTeleportLocationAndCounter), b_+11);
  CYC(b_+11, b_+14); if (hook_enabled_at(SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
}

// Also used by state 9, C, D substate2.
void ganon_state8_substate2_hook(GB *gb) {
  BASE(ganon_state8_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+5); L = E;  // ld l,e
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+6, b_+9); ganon_updateTeleportVarsAndPlaySound_hook(gb); return;  // jp ganon_updateTeleportVarsAndPlaySound
}

void ganon_state8_substate3_hook(GB *gb) {
  BASE(ganon_state8_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+12); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+13, b_+15); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+16, b_+18); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+20, b_+23); if (hook_enabled_at(SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
}

void ganon_state8_substate4_hook(GB *gb) {
  BASE(ganon_state8_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0xb3;  // ld a,GFXH_GANON_C
  CYC(b_+10, b_+13); ganon_loadGfxHeader_hook(gb); return;  // jp ganon_loadGfxHeader
}

void ganon_state8_substate5_hook(GB *gb) {
  BASE(ganon_state8_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 45);  // ld (hl),45
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0x05;  // ld a,$05
  CALL_C(b_+10, enemySetAnimation_hook, SYM(enemySetAnimation), b_+13);
  CALL_C(b_+13, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+16);
  CYC(b_+16, b_+19); SET_BC(0x003c);  // ldbc $00,SPEED_180
  CYC(b_+19, b_+21); ganon_state8_spawnProjectile_hook(gb); return;  // jr ganon_state8_spawnProjectile
}

void ganon_state8_substate6_hook(GB *gb) {
  BASE(ganon_state8_substate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto checkAttackDone; } CYC(b_+3, b_+5);  // jr nz,+
  CYC(b_+5, b_+7); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+7, b_+8); L = E;  // ld l,e
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+9, b_+11); A = 0x02;  // ld a,$02
  CYC(b_+11, b_+14); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
checkAttackDone:
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+15, b_+17); alu_cp(gb, 0x19);  // cp $19
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; } CYC(b_+17, b_+18);  // ret nz
  CYC(b_+18, b_+21); SET_BC(0x0264);  // ldbc $02,SPEED_280
  CALL_C(b_+21, ganon_state8_spawnProjectile_hook, SYM(ganon_state8_spawnProjectile), b_+24);
  CYC(b_+24, b_+27); SET_BC(0xfe64);  // ldbc $fe,SPEED_280
  ganon_state8_spawnProjectile_hook(gb); return;  // fallthrough
}

void ganon_state8_spawnProjectile_hook(GB *gb) {
  BASE(ganon_state8_spawnProjectile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x52;  // ld e,PART_52
  CALL_C(b_+2, ganon_spawnPart_hook, SYM(ganon_spawnPart), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } CYC(b_+5, b_+6);  // ret nz
  CYC(b_+6, b_+8); L = 0xc9;  // ld l,Part.angle
  CYC(b_+8, b_+10); E = 0x89;  // ld e,Enemy.angle
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+11, b_+12); alu_add(gb, B);  // add b
  CYC(b_+12, b_+14); alu_and(gb, 0x1f);  // and $1f
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+15, b_+17); L = 0xd0;  // ld l,Part.speed
  CYC(b_+17, b_+18); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(b_+18, b_+21); if (hook_enabled_at(SYM(objectCopyPosition))) { objectCopyPosition_hook(gb); return; } HANDOFF(SYM(objectCopyPosition));  // jp objectCopyPosition
}

void ganon_state8_substate7_hook(GB *gb) {
  BASE(ganon_state8_substate7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+7); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_state9_hook(GB *gb) {
  BASE(ganon_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ganon_jump_table(gb));
    if (jt_ == SYM(ganon_state9_substate0)) { ganon_state9_substate0_hook(gb); return; }
    else if (jt_ == SYM(ganon_state9_substate1)) { ganon_state9_substate1_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate2)) { ganon_state8_substate2_hook(gb); return; }
    else if (jt_ == SYM(ganon_state9_substate3)) { ganon_state9_substate3_hook(gb); return; }
    else if (jt_ == SYM(ganon_state9_substate4)) { ganon_state9_substate4_hook(gb); return; }
    else if (jt_ == SYM(ganon_state9_substate5)) { ganon_state9_substate5_hook(gb); return; }
    else if (jt_ == SYM(ganon_state9_substate6)) { ganon_state9_substate6_hook(gb); return; }
    else if (jt_ == SYM(ganon_state9_substate7)) { ganon_state9_substate7_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

// Also used by state A, B, C substate0.
void ganon_state9_substate0_hook(GB *gb) {
  BASE(ganon_state9_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ganon_updateTeleportVarsAndPlaySound_hook, SYM(ganon_updateTeleportVarsAndPlaySound), b_+3);
  CYC(b_+3, b_+5); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  RET(b_+6); return;  // ret
}

void ganon_state9_substate1_hook(GB *gb) {
  BASE(ganon_state9_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+12); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x58);  // ld (hl),$58
  CYC(b_+14, b_+16); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(b_+18, b_+21); if (hook_enabled_at(SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
}

void ganon_state9_substate3_hook(GB *gb) {
  BASE(ganon_state9_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+12); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+14, b_+17); if (hook_enabled_at(SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
}

void ganon_state9_substate4_hook(GB *gb) {
  BASE(ganon_state9_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x28);  // ld (hl),$28
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0xb6;  // ld a,GFXH_GANON_F
  CALL_C(b_+10, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+13);
  CYC(b_+13, b_+15); A = 0x09;  // ld a,$09
  CALL_C(b_+15, enemySetAnimation_hook, SYM(enemySetAnimation), b_+18);
  CYC(b_+18, b_+20); B = 0x1c;  // ld b,$1c
  CYC(b_+20, b_+23); push_effect(gb, b_+23); ganon_state9_substate4__spawnProjectile_hook(gb);  // call @spawnProjectile
  CYC(b_+23, b_+25); B = 0x14;  // ld b,$14
  CYC(b_+25, b_+28); push_effect(gb, b_+28); ganon_state9_substate4__spawnProjectile_hook(gb);  // call @spawnProjectile
  CYC(b_+28, b_+30); B = 0x0c;  // ld b,$0c
  CYC(b_+30, b_+33); push_effect(gb, b_+33); ganon_state9_substate4__spawnProjectile_hook(gb);  // call @spawnProjectile
  CYC(b_+33, b_+35); B = 0x04;  // ld b,$04
  ganon_state9_substate4__spawnProjectile_hook(gb); return;  // fallthrough
}

// 10:531a (real call from three sites above, needs genuine continuation, and also reached
// by fallthrough; extracted as a plain non-hooked helper).
void ganon_state9_substate4__spawnProjectile_hook(GB *gb) {
  BASE(ganon_state9_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+35, b_+37); E = 0x52;  // ld e,PART_52
  CALL_C(b_+37, ganon_spawnPart_hook, SYM(ganon_spawnPart), b_+40);
  CYC(b_+40, b_+42); L = 0xc2;  // ld l,Part.subid
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [subid]=1
  CYC(b_+43, b_+45); L = 0xc9;  // ld l,Part.angle
  CYC(b_+45, b_+46); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+46, b_+48); L = 0xd7;  // ld l,Part.relatedObj1+1
  CYC(b_+48, b_+49); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(b_+49, b_+50); L = alu_dec8(gb, L);  // dec l
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(b_+52, b_+55); if (hook_enabled_at(SYM(objectCopyPosition))) { objectCopyPosition_hook(gb); return; } HANDOFF(SYM(objectCopyPosition));  // jp objectCopyPosition
}

void ganon_state9_substate5_hook(GB *gb) {
  BASE(ganon_state9_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x28);  // ld (hl),$28
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0xb2;  // ld a,GFXH_GANON_B
  CALL_C(b_+10, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+13);
  CYC(b_+13, b_+15); A = 0x07;  // ld a,$07
  CYC(b_+15, b_+18); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_state9_substate6_hook(GB *gb) {
  BASE(ganon_state9_substate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 80);  // ld (hl),80
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0x02;  // ld a,$02
  CYC(b_+10, b_+13); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_state9_substate7_hook(GB *gb) {
  BASE(ganon_state9_substate7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+7); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_stateA_hook(GB *gb) {
  BASE(ganon_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ganon_jump_table(gb));
    if (jt_ == SYM(ganon_state9_substate0)) { ganon_state9_substate0_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateA_substate1)) { ganon_stateA_substate1_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateA_substate2)) { ganon_stateA_substate2_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateA_substate3)) { ganon_stateA_substate3_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateA_substate4)) { ganon_stateA_substate4_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateA_substate5)) { ganon_stateA_substate5_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateA_substate6)) { ganon_stateA_substate6_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateA_substate7)) { ganon_stateA_substate7_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

// Teleporting out.
void ganon_stateA_substate1_hook(GB *gb) {
  BASE(ganon_stateA_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+13); if (hook_enabled_at(SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
}

// Delay before reappearing.
void ganon_stateA_substate2_hook(GB *gb) {
  BASE(ganon_stateA_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+5); L = E;  // ld l,e
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+6, b_+8); A = mem_rd(gb, hEnemyTargetX);  // ldh a,(<hEnemyTargetX)
  CYC(b_+8, b_+10); alu_cp(gb, 0x78);  // cp (LARGE_ROOM_WIDTH<<4)/2
  CYC(b_+10, b_+13); SET_BC(0x0328);  // ldbc $03,$28
  if ((F & FC)) { CYCT(b_+13, b_+15); goto haveBC; } CYC(b_+13, b_+15);  // jr c,+
  CYC(b_+15, b_+18); SET_BC(0x00d8);  // ldbc $00,-$28
haveBC:
  CYC(b_+18, b_+20); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+20, b_+21); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+21, b_+22); alu_add(gb, C);  // add c
  CYC(b_+22, b_+24); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL - 1);  // ldd (hl),a
  CYC(b_+25, b_+27); A = mem_rd(gb, hEnemyTargetY);  // ldh a,(<hEnemyTargetY)
  CYC(b_+27, b_+29); alu_cp(gb, 0x30);  // cp $30
  if ((F & FC)) { CYCT(b_+29, b_+31); goto setYh; } CYC(b_+29, b_+31);  // jr c,+
  CYC(b_+31, b_+33); alu_sub(gb, 0x18);  // sub $18
setYh:
  CYC(b_+33, b_+34); L = alu_dec8(gb, L);  // dec l
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);  // ld (hl),a [yh]
  CYC(b_+35, b_+37); A = 0xb2;  // ld a,GFXH_GANON_B
  CALL_C(b_+37, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+40);
  CYC(b_+40, b_+42); E = 0xb2;  // ld e,Enemy.var32
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+43, b_+45); alu_add(gb, 0x07);  // add $07
  CALL_C(b_+45, enemySetAnimation_hook, SYM(enemySetAnimation), b_+48);
  CYC(b_+48, b_+51); ganon_updateTeleportVarsAndPlaySound_hook(gb); return;  // jp ganon_updateTeleportVarsAndPlaySound
}

void ganon_stateA_substate3_hook(GB *gb) {
  BASE(ganon_stateA_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+12); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+14, b_+16); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x78);  // ld (hl),SPEED_300
  CALL_C(b_+18, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+21);
  CYC(b_+21, b_+23); E = 0x50;  // ld e,PART_GANON_TRIDENT
  CALL_C(b_+23, ganon_spawnPart_hook, SYM(ganon_spawnPart), b_+26);
  CYC(b_+26, b_+29); if (hook_enabled_at(SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
}

void ganon_stateA_substate4_hook(GB *gb) {
  BASE(ganon_stateA_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x04);  // ld (hl),$04
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0xb7;  // ld a,GFXH_GANON_G
  CALL_C(b_+10, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+13);
  CYC(b_+13, b_+15); E = 0xb2;  // ld e,Enemy.var32
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+16, b_+18); alu_add(gb, 0x08);  // add $08
  CALL_C(b_+18, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(ganon_stateA_substate5));
  ganon_stateA_substate5_hook(gb); return;  // fallthrough
}

void ganon_stateA_substate5_hook(GB *gb) {
  BASE(ganon_stateA_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto checkBounds; } CYC(b_+3, b_+5);  // jr nz,+++
  CYC(b_+5, b_+7); mem_wr(gb, HL, 16);  // ld (hl),16
  CYC(b_+7, b_+8); L = E;  // ld l,e
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+9, b_+11); A = 0xb6;  // ld a,GFXH_GANON_F
  CALL_C(b_+11, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+14);
  CYC(b_+14, b_+16); E = 0xb2;  // ld e,Enemy.var32
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+17, b_+19); alu_add(gb, 0x09);  // add $09
  CYC(b_+19, b_+22); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
checkBounds:
  CYC((SYM(ganon_stateA_substate6) + 11), (SYM(ganon_stateA_substate6) + 13)); E = 0x8b;  // ld e,Enemy.yh
  CYC((SYM(ganon_stateA_substate6) + 13), (SYM(ganon_stateA_substate6) + 14)); A = mem_rd(gb, DE);  // ld a,(de)
  CYC((SYM(ganon_stateA_substate6) + 14), (SYM(ganon_stateA_substate6) + 16)); alu_sub(gb, 0x18);  // sub $18
  CYC((SYM(ganon_stateA_substate6) + 16), (SYM(ganon_stateA_substate6) + 18)); alu_cp(gb, 0x80);  // cp $80
  if (!(F & FC)) { RET_TAKEN((SYM(ganon_stateA_substate6) + 18)); return; } CYC((SYM(ganon_stateA_substate6) + 18), (SYM(ganon_stateA_substate6) + 19));  // ret nc
  CYC((SYM(ganon_stateA_substate6) + 19), (SYM(ganon_stateA_substate6) + 21)); E = 0x8d;  // ld e,Enemy.xh
  CYC((SYM(ganon_stateA_substate6) + 21), (SYM(ganon_stateA_substate6) + 22)); A = mem_rd(gb, DE);  // ld a,(de)
  CYC((SYM(ganon_stateA_substate6) + 22), (SYM(ganon_stateA_substate6) + 24)); alu_sub(gb, 0x18);  // sub $18
  CYC((SYM(ganon_stateA_substate6) + 24), (SYM(ganon_stateA_substate6) + 26)); alu_cp(gb, 0xc0);  // cp $c0
  if (!(F & FC)) { RET_TAKEN((SYM(ganon_stateA_substate6) + 26)); return; } CYC((SYM(ganon_stateA_substate6) + 26), (SYM(ganon_stateA_substate6) + 27));  // ret nc
  CYC((SYM(ganon_stateA_substate6) + 27), (SYM(ganon_stateA_substate6) + 30)); if (hook_enabled_at(SYM(objectApplySpeed))) { objectApplySpeed_hook(gb); return; } HANDOFF(SYM(objectApplySpeed));  // jp objectApplySpeed
}

void ganon_stateA_substate6_hook(GB *gb) {
  BASE(ganon_stateA_substate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto checkBounds; } CYC(b_+3, b_+5);  // jr nz,+++
  CYC(b_+5, b_+6); L = E;  // ld l,e
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);  // inc l
  CYC(b_+8, b_+10); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(b_+10); return;  // ret
checkBounds:
  CYC(b_+11, b_+13); E = 0x8b;  // ld e,Enemy.yh
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+14, b_+16); alu_sub(gb, 0x18);  // sub $18
  CYC(b_+16, b_+18); alu_cp(gb, 0x80);  // cp $80
  if (!(F & FC)) { RET_TAKEN(b_+18); return; } CYC(b_+18, b_+19);  // ret nc
  CYC(b_+19, b_+21); E = 0x8d;  // ld e,Enemy.xh
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+22, b_+24); alu_sub(gb, 0x18);  // sub $18
  CYC(b_+24, b_+26); alu_cp(gb, 0xc0);  // cp $c0
  if (!(F & FC)) { RET_TAKEN(b_+26); return; } CYC(b_+26, b_+27);  // ret nc
  CYC(b_+27, b_+30); if (hook_enabled_at(SYM(objectApplySpeed))) { objectApplySpeed_hook(gb); return; } HANDOFF(SYM(objectApplySpeed));  // jp objectApplySpeed
}

void ganon_stateA_substate7_hook(GB *gb) {
  BASE(ganon_stateA_substate7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); A = 0x02;  // ld a,$02
  CALL_C(b_+6, enemySetAnimation_hook, SYM(enemySetAnimation), b_+9);
  CYC(b_+9, b_+12); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_stateB_hook(GB *gb) {
  BASE(ganon_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ganon_jump_table(gb));
    if (jt_ == SYM(ganon_state9_substate0)) { ganon_state9_substate0_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate1)) { ganon_stateB_substate1_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate2)) { ganon_stateB_substate2_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate3)) { ganon_stateB_substate3_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate4)) { ganon_stateB_substate4_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate5)) { ganon_stateB_substate5_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate6)) { ganon_stateB_substate6_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate7)) { ganon_stateB_substate7_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate8)) { ganon_stateB_substate8_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substate9)) { ganon_stateB_substate9_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateB_substateA)) { ganon_stateB_substateA_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ganon_stateB_substate1_hook(GB *gb) {
  BASE(ganon_stateB_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(b_+6, b_+8); mem_wr(gb, HL, 180);  // ld (hl),180
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+13); if (hook_enabled_at(SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
}

void ganon_stateB_substate2_hook(GB *gb) {
  BASE(ganon_stateB_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+5); L = E;  // ld l,e
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+6, b_+8); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x28);  // ld (hl),$28
  CYC(b_+10, b_+12); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(b_+14, b_+16); A = 0xb2;  // ld a,GFXH_GANON_B
  CALL_C(b_+16, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+19);
  CYC(b_+19, b_+21); A = 0x04;  // ld a,$04
  CALL_C(b_+21, enemySetAnimation_hook, SYM(enemySetAnimation), b_+24);
  CYC(b_+24, b_+27); ganon_updateTeleportVarsAndPlaySound_hook(gb); return;  // jp ganon_updateTeleportVarsAndPlaySound
}

void ganon_stateB_substate3_hook(GB *gb) {
  BASE(ganon_stateB_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x40);  // ld (hl),$40
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+12); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(b_+14, objectSetVisible83_hook, SYM(objectSetVisible83), b_+17);
  CYC(b_+17, b_+19); E = 0x51;  // ld e,PART_51
  CALL_C(b_+19, ganon_spawnPart_hook, SYM(ganon_spawnPart), b_+22);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; } CYC(b_+22, b_+23);  // ret nz
  CYC(b_+23, b_+26); SET_BC(0xf810);  // ld bc,$f810
  CYC(b_+26, b_+29); if (hook_enabled_at(SYM(objectCopyPositionWithOffset))) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(SYM(objectCopyPositionWithOffset));  // jp objectCopyPositionWithOffset
}

void ganon_stateB_substate4_hook(GB *gb) {
  BASE(ganon_stateB_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+5); L = E;  // ld l,e
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+6, b_+8); L = 0x94;  // ld l,Enemy.speedZ
  CYC(b_+8, b_+10); A = 0x40;  // ld a,<(-$1c0)
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xfe);  // ld (hl),>(-$1c0)
  CYC(b_+13, b_+15); A = 0xb3;  // ld a,GFXH_GANON_C
  CALL_C(b_+15, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+18);
  CYC(b_+18, b_+20); A = 0x05;  // ld a,$05
  CYC(b_+20, b_+23); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_stateB_substate5_hook(GB *gb) {
  BASE(ganon_stateB_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;  // ld c,$20
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto shakeAndAdvance; } CYC(b_+5, b_+7);  // jr z,++
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL - 1);  // ldd a,(hl)
  CYC(b_+8, b_+9); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } CYC(b_+9, b_+10);  // ret nz
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+11, b_+13); alu_cp(gb, 0xc0);  // cp $c0
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } CYC(b_+13, b_+14);  // ret nz
  CYC(b_+14, b_+16); A = 0xb5;  // ld a,GFXH_GANON_E
  CALL_C(b_+16, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+19);
  CYC(b_+19, b_+21); A = 0x06;  // ld a,$06
  CYC(b_+21, b_+24); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
shakeAndAdvance:
  CYC(b_+24, b_+26); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+26, b_+28); A = 120;  // ld a,120
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+29, b_+32); mem_wr(gb, wScreenShakeCounterY, A);  // ld (wScreenShakeCounterY),a
  CYC(b_+32, b_+34); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+35, b_+37); A = 0x6f;  // ld a,SND_EXPLOSION
  CYC(b_+37, b_+40); if (hook_enabled_at(SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void ganon_stateB_substate6_hook(GB *gb) {
  BASE(ganon_stateB_substate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if ((F & FZ)) { CYCT(b_+3, b_+5); goto nextState; } CYC(b_+3, b_+5);  // jr z,++
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+6, b_+8); alu_cp(gb, 105);  // cp 105
  if ((F & FC)) { RET_TAKEN(b_+8); return; } CYC(b_+8, b_+9);  // ret c
  CYC(b_+9, b_+12); A = mem_rd(gb, w1Link_zh);  // ld a,(w1Link.zh)
  CYC(b_+12, b_+13); alu_rlca(gb);  // rlca
  if ((F & FC)) { RET_TAKEN(b_+13); return; } CYC(b_+13, b_+14);  // ret c
  CYC(b_+14, b_+17); SET_HL(wLinkForceState);  // ld hl,wLinkForceState
  CYC(b_+17, b_+19); A = 0x14;  // ld a,LINK_STATE_COLLAPSED
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  RET(b_+22); return;  // ret
nextState:
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x04);  // ld (hl),$04
  CYC(b_+25, b_+26); L = E;  // ld l,e
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+27, b_+29); A = 0xb2;  // ld a,GFXH_GANON_B
  CALL_C(b_+29, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+32);
  CYC(b_+32, b_+34); A = 0x04;  // ld a,$04
  CYC(b_+34, b_+37); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_stateB_substate7_hook(GB *gb) {
  BASE(ganon_stateB_substate7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x18);  // ld (hl),$18
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); E = 0x51;  // ld e,PART_51
  CALL_C(b_+10, ganon_spawnPart_hook, SYM(ganon_spawnPart), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } CYC(b_+13, b_+14);  // ret nz
  CYC(b_+14, b_+16); L = 0xc2;  // ld l,Part.subid
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+17, b_+20); SET_BC(0xf810);  // ld bc,$f810
  CYC(b_+20, b_+23); if (hook_enabled_at(SYM(objectCopyPositionWithOffset))) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(SYM(objectCopyPositionWithOffset));  // jp objectCopyPositionWithOffset
}

void ganon_stateB_substate8_hook(GB *gb) {
  BASE(ganon_stateB_substate8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CALL_C(b_+8, objectCreatePuff_hook, SYM(objectCreatePuff), b_+11);
  CYC(b_+11, b_+13); A = 0xb3;  // ld a,GFXH_GANON_C
  CALL_C(b_+13, ganon_loadGfxHeader_hook, SYM(ganon_loadGfxHeader), b_+16);
  CYC(b_+16, b_+18); A = 0x05;  // ld a,$05
  CYC(b_+18, b_+21); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_stateB_substate9_hook(GB *gb) {
  BASE(ganon_stateB_substate9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0x02;  // ld a,$02
  CYC(b_+10, b_+13); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_stateB_substateA_hook(GB *gb) {
  BASE(ganon_stateB_substateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+7); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_stateC_hook(GB *gb) {
  BASE(ganon_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ganon_jump_table(gb));
    if (jt_ == SYM(ganon_state9_substate0)) { ganon_state9_substate0_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substate1)) { ganon_stateC_substate1_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate2)) { ganon_state8_substate2_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substate3)) { ganon_stateC_substate3_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substate4)) { ganon_stateC_substate4_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substate5)) { ganon_stateC_substate5_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substate6)) { ganon_stateC_substate6_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substate7)) { ganon_stateC_substate7_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substate8)) { ganon_stateC_substate8_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substate9)) { ganon_stateC_substate9_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateC_substateA)) { ganon_stateC_substateA_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ganon_stateC_substate1_hook(GB *gb) {
  BASE(ganon_stateC_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(b_+6, b_+8); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+12); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x58);  // ld (hl),$58
  CYC(b_+14, b_+16); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(b_+18, b_+21); if (hook_enabled_at(SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
}

void ganon_stateC_substate3_hook(GB *gb) {
  BASE(ganon_stateC_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(b_+6, b_+8); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(b_+8, b_+9); L = E;  // ld l,e
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+10, b_+12); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(b_+14, objectSetVisible83_hook, SYM(objectSetVisible83), b_+17);
  CYC(b_+17, b_+19); A = 0xb4;  // ld a,SND_FADEOUT
  CYC(b_+19, b_+22); if (hook_enabled_at(SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void ganon_stateC_substate4_hook(GB *gb) {
  BASE(ganon_stateC_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if ((F & FZ)) { CYCT(b_+3, b_+5); goto nextSubstate; } CYC(b_+3, b_+5);  // jr z,@nextSubstate
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+6, b_+8); alu_cp(gb, 60);  // cp 60
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } CYC(b_+8, b_+9);  // ret nc
  CYC(b_+9, b_+11); alu_and(gb, 0x03);  // and $03
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } CYC(b_+11, b_+12);  // ret nz
  CYC(b_+12, b_+14); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+15, b_+17); alu_xor(gb, 0x05);  // xor $05
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);  // ld (hl),a
  RET(b_+19); return;  // ret
nextSubstate:
  CYC(b_+20, b_+22); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+23, b_+24); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(b_+24); return; } CYC(b_+24, b_+25);  // ret z
  CYC(b_+25, b_+26); L = E;  // ld l,e
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+27, b_+29); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+29, b_+31); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+31, b_+33); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+33, b_+35); A = 0x01;  // ld a,$01
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+37, b_+39); A = 0x02;  // ld a,$02
  CYC(b_+39, b_+42); if (hook_enabled_at(SYM(fadeoutToBlackWithDelay))) { fadeoutToBlackWithDelay_hook(gb); return; } HANDOFF(SYM(fadeoutToBlackWithDelay));  // jp fadeoutToBlackWithDelay
}

void ganon_stateC_substate5_hook(GB *gb) {
  BASE(ganon_stateC_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);  // ld a,(wPaletteThread_mode)
  CYC(b_+3, b_+4); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret nz
  CYC(b_+5, b_+7); A = 0x06;  // ld a,$06
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);  // ld (de),a [substate]
  CYC(b_+8, b_+10); A = 0x04;  // ld a,$04
  CALL_C(b_+10, ganon_setTileReplacementMode_hook, SYM(ganon_setTileReplacementMode), b_+13);
  CYC(b_+13, b_+16); ganon_makeRoomBoundarySolid_hook(gb); return;  // jp ganon_makeRoomBoundarySolid
}

void ganon_stateC_substate6_hook(GB *gb) {
  BASE(ganon_stateC_substate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+2); L = E;  // ld l,e
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);  // inc l
  CYC(b_+4, b_+6); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+6, b_+8); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+10, b_+12); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+12, b_+14); mem_wr(gb, HL, 20);  // ld (hl),SPEED_80
  CALL_C(b_+14, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x07);  // and $07
  CYC(b_+19, b_+22); SET_HL(b_+30);  // ld hl,@counter2Vals
  CYC(b_+22, b_+23);
  ganon_addAToHl_from_rst(gb, b_+23);  // rst_addAToHl
  CYC(b_+23, b_+25); E = 0x87;  // ld e,Enemy.counter2
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+27, b_+30); if (hook_enabled_at(SYM(fadeinFromBlack))) { fadeinFromBlack_hook(gb); return; } HANDOFF(SYM(fadeinFromBlack));  // jp fadeinFromBlack
}

void ganon_stateC_substate7_hook(GB *gb) {
  BASE(ganon_stateC_substate7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;  // ld a,$02
  CYC(b_+2, b_+5); mem_wr(gb, wMenuUnionEnd, A);  // ld (wUseSimulatedInput),a
  CYC(b_+5, b_+8); A = mem_rd(gb, wFrameCounter);  // ld a,(wFrameCounter)
  CYC(b_+8, b_+10); alu_and(gb, 0x03);  // and $03
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto checkCounter1; } CYC(b_+10, b_+12);  // jr nz,+
  CALL_C(b_+12, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto checkCounter1; } CYC(b_+15, b_+17);  // jr nz,+
  CYC(b_+17, b_+19); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [substate]=9
  CYC(b_+21, b_+23); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+23, b_+25); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+25, b_+28); if (hook_enabled_at(SYM(fastFadeoutToWhite))) { fastFadeoutToWhite_hook(gb); return; } HANDOFF(SYM(fastFadeoutToWhite));  // jp fastFadeoutToWhite
checkCounter1:
  CALL_C(b_+28, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto loadGfx; } CYC(b_+31, b_+33);  // jr nz,+
  CYC(b_+33, b_+34); L = E;  // ld l,e
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [substate]=8
  CYC(b_+35, b_+37); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+37, b_+39); mem_wr(gb, HL, 80);  // ld (hl),80
  CYC(b_+39, b_+41); A = 0xb3;  // ld a,GFXH_GANON_C
  CYC(b_+41, b_+44); ganon_loadGfxHeader_hook(gb); return;  // jp ganon_loadGfxHeader
loadGfx:
  CALL_C(b_+44, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+47);
  CALL_C(b_+47, ecom_applyVelocityForSideviewEnemyNoHoles_b10_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b10), b_+50);
  CALL_C(b_+50, enemyAnimate_hook, SYM(enemyAnimate), b_+53);
  CYC(b_+53, b_+56); ganon_updateSeizurePalette_hook(gb); return;  // jp ganon_updateSeizurePalette
}

void ganon_stateC_substate8_hook(GB *gb) {
  BASE(ganon_stateC_substate8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;  // ld a,$02
  CYC(b_+2, b_+5); mem_wr(gb, wMenuUnionEnd, A);  // ld (wUseSimulatedInput),a
  CALL_C(b_+5, ganon_updateSeizurePalette_hook, SYM(ganon_updateSeizurePalette), b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wFrameCounter);  // ld a,(wFrameCounter)
  CYC(b_+11, b_+13); alu_and(gb, 0x03);  // and $03
  if ((F & FZ)) { CALL_C_CC(b_+13, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+16); } else CYC(b_+13, b_+16);  // call z,ecom_decCounter2
  CALL_C(b_+16, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+19);
  if ((F & FZ)) { CYCT(b_+19, b_+21); goto nextSubstate; } CYC(b_+19, b_+21);  // jr z,@nextSubstate
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+22, b_+24); alu_cp(gb, 60);  // cp 60
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } CYC(b_+24, b_+25);  // ret nz
  CYC(b_+25, b_+27); A = 0x05;  // ld a,$05
  CALL_C(b_+27, enemySetAnimation_hook, SYM(enemySetAnimation), b_+30);
  CYC(b_+30, b_+32); E = 0x52;  // ld e,PART_52
  CALL_C(b_+32, ganon_spawnPart_hook, SYM(ganon_spawnPart), b_+35);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; } CYC(b_+35, b_+36);  // ret nz
  CYC(b_+36, b_+38); L = 0xc2;  // ld l,Part.subid
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(b_+40, b_+43); if (hook_enabled_at(SYM(objectCopyPosition))) { objectCopyPosition_hook(gb); return; } HANDOFF(SYM(objectCopyPosition));  // jp objectCopyPosition
nextSubstate:
  CYC(b_+43, b_+45); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+45, b_+46); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);  // inc l
  CYC(b_+47, b_+49); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+49, b_+51); A = 0x02;  // ld a,$02
  CYC(b_+51, b_+54); if (hook_enabled_at(SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void ganon_stateC_substate9_hook(GB *gb) {
  BASE(ganon_stateC_substate9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);  // ld a,(wPaletteThread_mode)
  CYC(b_+3, b_+4); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret nz
  CYC(b_+5, b_+7); A = 0x0a;  // ld a,$0a
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);  // ld (de),a [substate]
  CYC(b_+8, b_+10); A = 0x03;  // ld a,$03
  CALL_C(b_+10, ganon_setTileReplacementMode_hook, SYM(ganon_setTileReplacementMode), b_+13);
  CYC(b_+13, b_+15); A = 0xb1;  // ld a,PALH_b1
  CYC(b_+15, b_+18); mem_wr(gb, wExtraBgPaletteHeader, A);  // ld (wExtraBgPaletteHeader),a
  CYC(b_+18, b_+21); if (hook_enabled_at(SYM(loadPaletteHeader))) { loadPaletteHeader_hook(gb); return; } HANDOFF(SYM(loadPaletteHeader));  // jp loadPaletteHeader
}

void ganon_stateC_substateA_hook(GB *gb) {
  BASE(ganon_stateC_substateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(b_+5, clearPaletteFadeVariablesAndRefreshPalettes_hook, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+8);
  CYC(b_+8, b_+11); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_stateD_hook(GB *gb) {
  BASE(ganon_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ganon_jump_table(gb));
    if (jt_ == SYM(ganon_state8_substate0)) { ganon_state8_substate0_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate1)) { ganon_state8_substate1_hook(gb); return; }
    else if (jt_ == SYM(ganon_state8_substate2)) { ganon_state8_substate2_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateD_substate3)) { ganon_stateD_substate3_hook(gb); return; }
    else if (jt_ == SYM(ganon_finishAttack)) { ganon_finishAttack_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ganon_stateD_substate3_hook(GB *gb) {
  BASE(ganon_stateD_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(b_+3, b_+6);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+11, b_+13); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+14, b_+17); if (hook_enabled_at(SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
}

void ganon_stateE_hook(GB *gb) {
  BASE(ganon_stateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ganon_jump_table(gb));
    if (jt_ == SYM(ganon_stateE_substate0)) { ganon_stateE_substate0_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateE_substate1)) { ganon_stateE_substate1_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateE_substate2)) { ganon_stateE_substate2_hook(gb); return; }
    else if (jt_ == SYM(ganon_stateE_substate3)) { ganon_stateE_substate3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ganon_stateE_substate0_hook(GB *gb) {
  BASE(ganon_stateE_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); if (hook_enabled_at(SYM(ecom_flickerVisibility_b10))) { ecom_flickerVisibility_b10_hook(gb); return; } HANDOFF(SYM(ecom_flickerVisibility_b10)); } CYC(b_+3, b_+6);  // jp nz,ecom_flickerVisibility
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+7, b_+9); E = 0x04;  // ld e,PART_BOSS_DEATH_EXPLOSION
  CALL_C(b_+9, ganon_spawnPart_hook, SYM(ganon_spawnPart), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } CYC(b_+12, b_+13);  // ret nz
  CYC(b_+13, b_+15); L = 0xc2;  // ld l,Part.subid
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [subid]=1
  CALL_C(b_+16, objectCopyPosition_hook, SYM(objectCopyPosition), b_+19);
  CYC(b_+19, b_+21); E = 0x98;  // ld e,Enemy.relatedObj2
  CYC(b_+21, b_+23); A = 0xc0;  // ld a,Part.start
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+24, b_+25); E = alu_inc8(gb, E);  // inc e
  CYC(b_+25, b_+26); A = H;  // ld a,h
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+27, b_+30); SET_HL(wNumEnemies);  // ld hl,wNumEnemies
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+31, b_+32); H = D;  // ld h,d
  CYC(b_+32, b_+34); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+35, b_+37); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CALL_C(b_+39, objectSetInvisible_hook, SYM(objectSetInvisible), b_+42);
  CYC(b_+42, b_+44); A = 0xbc;  // ld a,SND_BIG_EXPLOSION_2
  CYC(b_+44, b_+47); if (hook_enabled_at(SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void ganon_stateE_substate1_hook(GB *gb) {
  BASE(ganon_stateE_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x21;  // ld a,Object.animParameter
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+7); return; } CYC(b_+7, b_+8);  // ret z
  CYC(b_+8, b_+9); H = D;  // ld h,d
  CYC(b_+9, b_+11); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);  // inc l
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+15, b_+18); if (hook_enabled_at(SYM(fastFadeoutToWhite))) { fastFadeoutToWhite_hook(gb); return; } HANDOFF(SYM(fastFadeoutToWhite));  // jp fastFadeoutToWhite
}

void ganon_stateE_substate2_hook(GB *gb) {
  BASE(ganon_stateE_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+9); alu_xor(gb, A);  // xor a
  CYC(b_+9, b_+12); mem_wr(gb, wExtraBgPaletteHeader, A);  // ld (wExtraBgPaletteHeader),a
  CALL_C(b_+12, ganon_setTileReplacementMode_hook, SYM(ganon_setTileReplacementMode), b_+15);
  CYC(b_+15, b_+17); A = 0x02;  // ld a,$02
  CYC(b_+17, b_+20); if (hook_enabled_at(SYM(fadeinFromWhiteWithDelay))) { fadeinFromWhiteWithDelay_hook(gb); return; } HANDOFF(SYM(fadeinFromWhiteWithDelay));  // jp fadeinFromWhiteWithDelay
}

void ganon_stateE_substate3_hook(GB *gb) {
  BASE(ganon_stateE_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);  // ld a,(wPaletteThread_mode)
  CYC(b_+3, b_+4); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret nz
  CALL_C(b_+5, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } CYC(b_+8, b_+9);  // ret nz
  CYC(b_+9, b_+10); alu_xor(gb, A);  // xor a
  CYC(b_+10, b_+13); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CALL_C(b_+13, decNumEnemies_hook, SYM(decNumEnemies), b_+16);
  CYC(b_+16, b_+19); if (hook_enabled_at(SYM(enemyDelete))) { enemyDelete_hook(gb); return; } HANDOFF(SYM(enemyDelete));  // jp enemyDelete
}

void ganon_loadGfxHeader_hook(GB *gb) {
  BASE(ganon_loadGfxHeader);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, AF);  // push af
  CALL_C(b_+1, loadGfxHeader_hook, SYM(loadGfxHeader), b_+4);
  CYC(b_+4, b_+6); A = 0x33;  // ld a,UNCMP_GFXH_33
  CALL_C(b_+6, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+9);
  CYC(b_+9, b_+10); SET_AF(pop_effect(gb));  // pop af
  CYC(b_+10, b_+12); alu_sub(gb, 0xb2);  // sub GFXH_GANON_B
  CYC(b_+12, b_+14); alu_add(gb, 0x1e);  // add OBJ_GFXH_1e
  CYC(b_+14, b_+17); SET_HL((wLoadedObjectGfx + 4));  // ld hl,wLoadedObjectGfx+4
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(b_+20, b_+22); A = mem_rd(gb, hActiveObject);  // ldh a,(<hActiveObject)
  CYC(b_+22, b_+23); D = A;  // ld d,a
  RET(b_+23); return;  // ret
}

// X-position alternates left & right each frame while teleporting.
void ganon_updateTeleportAnimationGoingOut_hook(GB *gb) {
  BASE(ganon_updateTeleportAnimationGoingOut);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+1, b_+3); alu_and(gb, 0x3e);  // and $3e
  CYC(b_+3, b_+4); alu_rrca(gb);  // rrca
  CYC(b_+4, b_+5); B = A;  // ld b,a
  CYC(b_+5, b_+7); A = 0x20;  // ld a,$20
  CYC(b_+7, b_+8); alu_sub(gb, B);  // sub b
  ganon_updateFlickeringXPosition_hook(gb); return;  // fallthrough
}

void ganon_updateFlickeringXPosition_hook(GB *gb) {
  BASE(ganon_updateFlickeringXPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_bit(gb, 1, mem_rd(gb, HL));  // bit 1,(hl)
  if ((F & FZ)) { CYCT(b_+2, b_+4); goto haveDelta; } CYC(b_+2, b_+4);  // jr z,+
  CYC(b_+4, b_+5); alu_cpl(gb);  // cpl
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);  // inc a
haveDelta:
  CYC(b_+6, b_+8); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+8, b_+9); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(b_+9, b_+11); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+12, b_+15); if (hook_enabled_at(SYM(ecom_flickerVisibility_b10))) { ecom_flickerVisibility_b10_hook(gb); return; } HANDOFF(SYM(ecom_flickerVisibility_b10));  // jp ecom_flickerVisibility
}

void ganon_updateTeleportVarsAndPlaySound_hook(GB *gb) {
  BASE(ganon_updateTeleportVarsAndPlaySound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x8d;  // ld a,SND_TELEPORT
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CYC(b_+5, b_+6); H = D;  // ld h,d
  CYC(b_+6, b_+8); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+10, b_+12); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+12, b_+14); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+14, b_+16); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+17, b_+19); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);  // ld (hl),a
  RET(b_+20); return;  // ret
}

void ganon_updateTeleportAnimationComingIn_hook(GB *gb) {
  BASE(ganon_updateTeleportAnimationComingIn);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+1, b_+3); alu_and(gb, 0x3e);  // and $3e
  CYC(b_+3, b_+4); alu_rrca(gb);  // rrca
  CYC(b_+4, b_+6); ganon_updateFlickeringXPosition_hook(gb); return;  // jr ganon_updateFlickeringXPosition
}

void ganon_finishAttack_hook(GB *gb) {
  BASE(ganon_finishAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xb5;  // ld l,Enemy.var35
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+4, b_+5); H = mem_rd(gb, HL);  // ld h,(hl)
  CYC(b_+5, b_+6); L = A;  // ld l,a
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+7, b_+8); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+8, b_+10); ganon_decideNextMove_hook(gb); return; } CYC(b_+8, b_+10);  // jr z,ganon_decideNextMove
  label_10_135_hook(gb); return;  // fallthrough
}

void label_10_135_hook(GB *gb) {
  BASE(label_10_135);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x84;  // ld e,Enemy.state
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+3, b_+4); E = alu_inc8(gb, E);  // inc e
  CYC(b_+4, b_+5); alu_xor(gb, A);  // xor a
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);  // ld (de),a [substate]
  CYC(b_+6, b_+8); E = 0xb5;  // ld e,Enemy.var35
  CYC(b_+8, b_+9); A = L;  // ld a,l
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+10, b_+11); E = alu_inc8(gb, E);  // inc e
  CYC(b_+11, b_+12); A = H;  // ld a,h
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);  // ld (de),a
  RET(b_+13); return;  // ret
}

void ganon_decideNextMove_hook(GB *gb) {
  BASE(ganon_decideNextMove);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xa9;  // ld e,Enemy.health
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+5); alu_cp(gb, 0x41);  // cp $41
  CYC(b_+5, b_+7); C = 0x00;  // ld c,$00
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto haveC; } CYC(b_+7, b_+9);  // jr nc,+
  CYC(b_+9, b_+11); C = 0x04;  // ld c,$04
haveC:
  CALL_C(b_+11, getRandomNumber_hook, SYM(getRandomNumber), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x03);  // and $03
  CYC(b_+16, b_+17); alu_add(gb, C);  // add c
  CYC(b_+17, b_+20); SET_HL(b_+27);  // ld hl,@stateTable
  CYC(b_+20, b_+21);
  ganon_addDoubleIndexToHl_from_rst(gb, b_+21);  // rst_addDoubleIndexToHl
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+22, b_+23); H = mem_rd(gb, HL);  // ld h,(hl)
  CYC(b_+23, b_+24); L = A;  // ld l,a
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+25, b_+28); label_10_135_hook(gb); return;  // jr label_10_135
}

void ganon_decideTeleportLocationAndCounter_hook(GB *gb) {
  BASE(ganon_decideTeleportLocationAndCounter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0e0f);  // ld bc,$0e0f
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b10_hook, SYM(ecom_randomBitwiseAndBCE_b10), b_+6);
  CYC(b_+6, b_+7); A = B;  // ld a,b
  CYC(b_+7, b_+10); SET_HL(b_+29);  // ld hl,@teleportTargetTable
  CYC(b_+10, b_+11);
  ganon_addAToHl_from_rst(gb, b_+11);  // rst_addAToHl
  CYC(b_+11, b_+13); E = 0x8b;  // ld e,Enemy.yh
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+15, b_+17); E = 0x8d;  // ld e,Enemy.xh
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+19, b_+20); A = C;  // ld a,c
  CYC(b_+20, b_+23); SET_HL(b_+45);  // ld hl,@counter1Vals
  CYC(b_+23, b_+24);
  ganon_addAToHl_from_rst(gb, b_+24);  // rst_addAToHl
  CYC(b_+24, b_+26); E = 0x86;  // ld e,Enemy.counter1
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);  // ld (de),a
  RET(b_+28); return;  // ret
}

void ganon_spawnPart_hook(GB *gb) {
  BASE(ganon_spawnPart);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+5); mem_wr(gb, HL, E);  // ld (hl),e
  CYC(b_+5, b_+7); L = 0xd6;  // ld l,Part.relatedObj1
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);  // inc l
  CYC(b_+10, b_+11); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(b_+11, b_+12); alu_xor(gb, A);  // xor a
  RET(b_+12); return;  // ret
}

void ganon_makeRoomBoundarySolid_hook(GB *gb) {
  BASE(ganon_makeRoomBoundarySolid);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((wRoomCollisions + 16));  // ld hl,wRoomCollisions+$10
  CYC(b_+3, b_+5); B = 0x09;  // ld b,LARGE_ROOM_HEIGHT-2
topRow:
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0f);  // ld (hl),$0f
  CYC(b_+7, b_+8); A = L;  // ld a,l
  CYC(b_+8, b_+10); alu_add(gb, 0x10);  // add $10
  CYC(b_+10, b_+11); L = A;  // ld l,a
  CYC(b_+11, b_+12); B = alu_dec8(gb, B);  // dec b
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto topRow; } CYC(b_+12, b_+14);  // jr nz,-
  CYC(b_+14, b_+16); L = 0x1e;  // ld l,$1e
  CYC(b_+16, b_+18); B = 0x09;  // ld b,LARGE_ROOM_HEIGHT-2
bottomRow:
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x0f);  // ld (hl),$0f
  CYC(b_+20, b_+21); A = L;  // ld a,l
  CYC(b_+21, b_+23); alu_add(gb, 0x10);  // add $10
  CYC(b_+23, b_+24); L = A;  // ld l,a
  CYC(b_+24, b_+25); B = alu_dec8(gb, B);  // dec b
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto bottomRow; } CYC(b_+25, b_+27);  // jr nz,-
  CYC(b_+27, b_+29); L = 0x00;  // ld l,$00
  CYC(b_+29, b_+31); A = 0x0f;  // ld a,$0f
  CYC(b_+31, b_+32); B = A;  // ld b,a
leftCol:
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+33, b_+34); B = alu_dec8(gb, B);  // dec b
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto leftCol; } CYC(b_+34, b_+36);  // jr nz,-
  CYC(b_+36, b_+38); L = 0xa0;  // ld l,(LARGE_ROOM_HEIGHT-1)<<4
  CYC(b_+38, b_+39); B = A;  // ld b,a
rightCol:
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+40, b_+41); B = alu_dec8(gb, B);  // dec b
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto rightCol; } CYC(b_+41, b_+43);  // jr nz,-
  RET(b_+43); return;  // ret
}

void ganon_updateSeizurePalette_hook(GB *gb) {
  BASE(ganon_updateSeizurePalette);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenVariables);  // ld a,(wScrollMode)
  CYC(b_+3, b_+5); alu_and(gb, 0x01);  // and $01
  if ((F & FZ)) { RET_TAKEN(b_+5); return; } CYC(b_+5, b_+6);  // ret z
  CYC(b_+6, b_+9); A = mem_rd(gb, wPaletteThread_mode);  // ld a,(wPaletteThread_mode)
  CYC(b_+9, b_+10); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } CYC(b_+10, b_+11);  // ret nz
  CYC(b_+11, b_+14); A = mem_rd(gb, wFrameCounter);  // ld a,(wFrameCounter)
  CYC(b_+14, b_+15); alu_rrca(gb);  // rrca
  if ((F & FC)) { RET_TAKEN(b_+15); return; } CYC(b_+15, b_+16);  // ret c
  CYC(b_+16, b_+17); H = D;  // ld h,d
  CYC(b_+17, b_+19); L = 0xb7;  // ld l,$b7
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+21, b_+23); alu_and(gb, 0x07);  // and $07
  CYC(b_+23, b_+25); alu_add(gb, 0xb1);  // add PALH_b1
  CYC(b_+25, b_+28); mem_wr(gb, wExtraBgPaletteHeader, A);  // ld (wExtraBgPaletteHeader),a
  CYC(b_+28, b_+31); if (hook_enabled_at(SYM(loadPaletteHeader))) { loadPaletteHeader_hook(gb); return; } HANDOFF(SYM(loadPaletteHeader));  // jp loadPaletteHeader
}

void ganon_setTileReplacementMode_hook(GB *gb) {
  BASE(ganon_setTileReplacementMode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); mem_wr(gb, wTwinrovaTileReplacementMode, A);  // ld (wTwinrovaTileReplacementMode),a
  CALL_C(b_+3, func_131f_hook, SYM(func_131f), b_+6);
  CYC(b_+6, b_+8); A = mem_rd(gb, hActiveObject);  // ldh a,(<hActiveObject)
  CYC(b_+8, b_+9); D = A;  // ld d,a
  RET(b_+9); return;  // ret
}
