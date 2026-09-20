#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/enemies/mergedTwinrova.s (ENEMY_MERGED_TWINROVA), bank $10.

static void mergedTwinrova_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void mergedTwinrova_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

static uint16_t mergedTwinrova_jump_table(GB *gb) {
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

void mergedTwinrova_state_uninitialized_hook(GB *gb);
void mergedTwinrova_state_stub_hook(GB *gb);
void mergedTwinrova_state8_hook(GB *gb);
void mergedTwinrova_state9_hook(GB *gb);
void mergedTwinrova_stateA_hook(GB *gb);
void mergedTwinrova_stateB_hook(GB *gb);
void mergedTwinrova_lavaRoom_hook(GB *gb);
void mergedTwinrova_lavaRoom_stateC_hook(GB *gb);
void mergedTwinrova_chooseTargetPosition_hook(GB *gb);
void mergedTwinrova_lavaRoom_stateD_hook(GB *gb);
void mergedTwinrova_lavaRoom_stateE_hook(GB *gb);
void mergedTwinrova_iceRoom_hook(GB *gb);
void mergedTwinrova_iceRoom_stateC_hook(GB *gb);
void mergedTwinrova_iceRoom_stateD_hook(GB *gb);
void mergedTwinrova_iceRoom_stateE_hook(GB *gb);
void mergedTwinrova_iceRoom_stateF_hook(GB *gb);
void mergedTwinrova_iceRoom_state10_hook(GB *gb);
void mergedTwinrova_checkTimeToSwapRoomFromTimer_hook(GB *gb);
void mergedTwinrova_checkPositionsCloseEnough_hook(GB *gb);
void mergedTwinrova_checkTimeToSwapRoomFromDamage_hook(GB *gb);
void mergedTwinrova_deathCutscene_hook(GB *gb);
void mergedTwinrova_decVar3bIfNonzero_hook(GB *gb);

// 10:45f1 (real call from enemyCode01, tail-jumps into whichever state/subid root is
// selected, so this stays a plain non-hooked helper invoked via explicit push_effect).
void enemyCode01__normalStatus_hook(GB *gb) {
  BASE(enemyCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+93, mergedTwinrova_checkTimeToSwapRoomFromDamage_hook, SYM(mergedTwinrova_checkTimeToSwapRoomFromDamage), b_+96);
  CALL_C(b_+96, mergedTwinrova_checkTimeToSwapRoomFromTimer_hook, SYM(mergedTwinrova_checkTimeToSwapRoomFromTimer), b_+99);
  CALL_C(b_+99, ecom_getSubidAndCpStateTo08_b10_hook, SYM(ecom_getSubidAndCpStateTo08_b10), b_+102);
  CYC(b_+102, b_+104); alu_cp(gb, 0x0c);  // cp $0c
  if (!(F & FC)) { CYCT(b_+104, b_+106); goto stateCOrHigher; } CYC(b_+104, b_+106);  // jr nc,@stateCOrHigher
  CYC(b_+106, b_+107); push_effect(gb, b_+107);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == SYM(mergedTwinrova_state_uninitialized)) { mergedTwinrova_state_uninitialized_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_state_stub)) { mergedTwinrova_state_stub_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_state8)) { mergedTwinrova_state8_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_state9)) { mergedTwinrova_state9_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_stateA)) { mergedTwinrova_stateA_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_stateB)) { mergedTwinrova_stateB_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
stateCOrHigher:
  CYC(b_+131, b_+132); A = B;  // ld a,b
  CYC(b_+132, b_+133); push_effect(gb, b_+133);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == SYM(mergedTwinrova_lavaRoom)) { mergedTwinrova_lavaRoom_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_iceRoom)) { mergedTwinrova_iceRoom_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void enemyCode01_hook(GB *gb) {
  BASE(enemyCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if ((F & FZ)) { CYCT(b_+0, b_+2); goto normalStatus; } CYC(b_+0, b_+2);  // jr z,@normalStatus
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);  // sub ENEMYSTATUS_NO_HEALTH
  if ((F & FC)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret c
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto collisionOccurred; } CYC(b_+5, b_+7);  // jr nz,@collisionOccurred
  CYC(b_+7, b_+9); E = 0xb4;  // ld e,Enemy.var34
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+10, b_+11); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+11, b_+14); mergedTwinrova_deathCutscene_hook(gb); return; } CYC(b_+11, b_+14);  // jp z,mergedTwinrova_deathCutscene
  CYC(b_+14, b_+15); H = D;  // ld h,d
  CYC(b_+15, b_+17); L = 0xba;  // ld l,Enemy.var3a
  CYC(b_+17, b_+19); mem_wr(gb, HL, 120);  // ld (hl),120
  CYC(b_+19, b_+21); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+21, b_+23); mem_wr(gb, HL, 20);  // ld (hl),20
  CYC(b_+23, b_+25); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+25, b_+27); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+27, b_+29); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+29, b_+30); alu_xor(gb, A);  // xor a
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+32, b_+34); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+34, b_+36); mem_wr(gb, HL, GV(0xff, 0x87));  // ld (hl),$ff
  CYC(b_+36, b_+38); A = 0x09;  // ld a,$09
  CYC(b_+38, b_+41); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
collisionOccurred:
  CYC(b_+41, b_+43); E = 0xba;  // ld e,Enemy.var3a
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+44, b_+45); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+45, b_+47); goto normalStatus; } CYC(b_+45, b_+47);  // jr z,@normalStatus
  CYC(b_+47, b_+49); E = 0xaa;  // ld e,Enemy.var2a
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+50, b_+52); A = (uint8_t)(A & ~(1 << 7));  // res 7,a
  CYC(b_+52, b_+54); alu_sub(gb, 0x1a);  // sub ITEMCOLLISION_MYSTERY_SEED
  CYC(b_+54, b_+56); alu_cp(gb, 0x05);  // cp ITEMCOLLISION_GALE_SEED - ITEMCOLLISION_MYSTERY_SEED + 1
  if (!(F & FC)) { CYCT(b_+56, b_+58); goto normalStatus; } CYC(b_+56, b_+58);  // jr nc,@normalStatus
  CYC(b_+58, b_+60); A = 0x63;  // ld a,SND_BOSS_DAMAGE
  CALL_C(b_+60, playSound_b00_hook, SYM(playSound_b00), b_+63);
  CYC(b_+63, b_+64); H = D;  // ld h,d
  CYC(b_+64, b_+66); L = 0xab;  // ld l,Enemy.invincibilityCounter
  CYC(b_+66, b_+68); mem_wr(gb, HL, 45);  // ld (hl),45
  CYC(b_+68, b_+70); L = 0xb4;  // ld l,Enemy.var34
  CYC(b_+70, b_+71); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if (!(F & FZ)) { CYCT(b_+71, b_+73); goto normalStatus; } CYC(b_+71, b_+73);  // jr nz,@normalStatus
  CYC(b_+73, b_+75); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+77, b_+79); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+79, b_+81); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+81, b_+83); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+85, b_+86); L = alu_inc8(gb, L);  // inc l
  CYC(b_+86, b_+88); mem_wr(gb, HL, 216);  // ld (hl),216
  CYC(b_+88, b_+90); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CYC(b_+90, b_+93); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
normalStatus:
  CYC(b_+93, b_+96); push_effect(gb, b_+96); enemyCode01__normalStatus_hook(gb);  // call @normalStatus
}

void mergedTwinrova_state_uninitialized_hook(GB *gb) {
  BASE(mergedTwinrova_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;  // ld a,ENEMY_MERGED_TWINROVA
  CYC(b_+2, b_+5); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);  // ld (wEnemyIDToLoadExtraGfx),a
  CYC(b_+5, b_+7); A = mem_rd(gb, hActiveObject);  // ldh a,(<hActiveObject)
  CYC(b_+7, b_+8); D = A;  // ld d,a
  CYC(b_+8, b_+11); SET_BC(0x0012);  // ld bc,$0012
  CALL_C(b_+11, enemyBoss_spawnShadow_b10_hook, SYM(enemyBoss_spawnShadow_b10), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } CYC(b_+14, b_+15);  // ret nz
  CYC(b_+15, b_+17); A = 0x3c;  // ld a,SPEED_180
  CALL_C(b_+17, ecom_setSpeedAndState8_b10_hook, SYM(ecom_setSpeedAndState8_b10), b_+20);
  CYC(b_+20, b_+22); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+24, b_+26); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+26, b_+28); mem_wr(gb, HL, 210);  // ld (hl),210
  CYC(b_+28, b_+30); L = 0xb4;  // ld l,Enemy.var34
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x05);  // ld (hl),$05
  CYC(b_+32, b_+34); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0xff);  // ld (hl),$ff
  CALL_C(b_+36, objectSetVisible83_hook, SYM(objectSetVisible83), b_+39);
  CYC(b_+39, b_+42); SET_BC(0x2f0b);  // ld bc,TX_2f0b
  CYC(b_+42, b_+45); if (hook_enabled_at(gb, SYM(showText))) { showText_hook(gb); return; } HANDOFF(SYM(showText));  // jp showText
}

void mergedTwinrova_state_stub_hook(GB *gb) {
  BASE(mergedTwinrova_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;  // ret
}

void mergedTwinrova_state8_hook(GB *gb) {
  BASE(mergedTwinrova_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+5); L = E;  // ld l,e
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+6, b_+8); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(b_+10, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x01);  // and $01
  CYC(b_+15, b_+17); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+18, b_+19); B = A;  // ld b,a
  CYC(b_+19, b_+21); alu_xor(gb, 0x01);  // xor $01
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);  // inc a
  CYC(b_+22, b_+24); E = 0x9b;  // ld e,Enemy.oamFlagsBackup
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);  // inc e
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+27, b_+28); A = B;  // ld a,b
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);  // inc a
  CALL_C(b_+29, enemySetAnimation_hook, SYM(enemySetAnimation), b_+32);
  CYC(b_+32, b_+33); alu_xor(gb, A);  // xor a
  CYC(b_+33, b_+36); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CYC(b_+36, b_+39); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(b_+39, b_+41); A = 0x33;  // ld a,MUS_TWINROVA
  CYC(b_+41, b_+44); mem_wr(gb, wActiveMusic, A);  // ld (wActiveMusic),a
  CYC(b_+44, b_+47); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void mergedTwinrova_state9_hook(GB *gb) {
  BASE(mergedTwinrova_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x4878);  // ld bc,$4878
  CYC(b_+3, b_+4); H = D;  // ld h,d
  CYC(b_+4, b_+6); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(b_+7, b_+9); mem_wr(gb, hFF8F, A);  // ldh (<hFF8F),a
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);  // inc l
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+11, b_+13); mem_wr(gb, hFF8E, A);  // ldh (<hFF8E),a
  CALL_C(b_+13, mergedTwinrova_checkPositionsCloseEnough_hook, SYM(mergedTwinrova_checkPositionsCloseEnough), b_+16);
  if (!(F & FC)) { CYCT(b_+16, b_+19); if (hook_enabled_at(gb, SYM(ecom_moveTowardPosition_b10))) { ecom_moveTowardPosition_b10_hook(gb); return; } HANDOFF(SYM(ecom_moveTowardPosition_b10)); } CYC(b_+16, b_+19);  // jp nc,ecom_moveTowardPosition
  CYC(b_+19, b_+20); L = E;  // ld l,e
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+21, b_+22); L = alu_inc8(gb, L);  // inc l
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);  // inc l
  CYC(b_+25, b_+27); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+27, b_+29); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x96);  // ld (hl),$80|ENEMY_BEAMOS
  CYC(b_+31, b_+33); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(b_+35, b_+37); L = 0xbb;  // ld l,Enemy.var3b
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  RET(b_+39); return;  // ret
}

void mergedTwinrova_stateA_hook(GB *gb) {
  BASE(mergedTwinrova_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == b_+13) { goto substate0; }
    else if (jt_ == b_+51) { goto substate1; }
    else if (jt_ == b_+89) { goto substate2; }
    else if (jt_ == b_+114) { goto substate3; }
    else if (jt_ == b_+120) { goto substate4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CALL_C(b_+13, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+16);
  if ((F & FZ)) { CYCT(b_+16, b_+18); goto substate0_swapSubid; } CYC(b_+16, b_+18);  // jr z,++
  CYC(b_+18, b_+20); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+21, b_+23); alu_xor(gb, 0x03);  // xor $03
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);  // ld (hl),a
  RET(b_+25); return;  // ret
substate0_swapSubid:
  CYC(b_+26, b_+27); L = E;  // ld l,e
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+28, b_+29); L = alu_inc8(gb, L);  // inc l
  CYC(b_+29, b_+31); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+31, b_+33); L = 0x82;  // ld l,Enemy.subid
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+34, b_+35); A = alu_inc8(gb, A);  // inc a
  CYC(b_+35, b_+37); alu_and(gb, 0x01);  // and $01
  CYC(b_+37, b_+38); B = A;  // ld b,a
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+39, b_+41); alu_xor(gb, 0x01);  // xor $01
  CYC(b_+41, b_+42); A = alu_inc8(gb, A);  // inc a
  CYC(b_+42, b_+44); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+44, b_+45); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+46, b_+47); A = B;  // ld a,b
  CYC(b_+47, b_+48); A = alu_inc8(gb, A);  // inc a
  CYC(b_+48, b_+51); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
substate1:
  CALL_C(b_+51, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+54);
  if ((F & FZ)) { CYCT(b_+54, b_+56); goto substate1_swapSubid; } CYC(b_+54, b_+56);  // jr z,++
  CYC(b_+56, b_+58); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+59, b_+61); alu_xor(gb, 0x03);  // xor $03
  CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);  // ld (hl),a
  RET(b_+63); return;  // ret
substate1_swapSubid:
  CYC(b_+64, b_+65); L = E;  // ld l,e
  CYC(b_+65, b_+66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+66, b_+68); L = 0x82;  // ld l,Enemy.subid
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+69, b_+71); alu_xor(gb, 0x01);  // xor $01
  CYC(b_+71, b_+72); A = alu_inc8(gb, A);  // inc a
  CYC(b_+72, b_+74); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+74, b_+75); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+75, b_+76); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+76, b_+78); A = 0x01;  // ld a,$01
  CYC(b_+78, b_+81); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CALL_C(b_+81, fastFadeoutToWhite_hook, SYM(fastFadeoutToWhite), b_+84);
  CYC(b_+84, b_+86); A = 0xb9;  // ld a,SND_ENDLESS
  CYC(b_+86, b_+89); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
substate2:
  CYC(b_+89, b_+91); A = 0x03;  // ld a,$03
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(b_+92, disableLcd_hook, SYM(disableLcd), b_+95);
  CYC(b_+95, b_+97); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+98, b_+99); A = alu_inc8(gb, A);  // inc a
  CYC(b_+99, b_+102); mem_wr(gb, wTwinrovaTileReplacementMode, A);  // ld (wTwinrovaTileReplacementMode),a
  CALL_C(b_+102, func_131f_hook, SYM(func_131f), b_+105);
  CYC(b_+105, b_+107); A = 0x02;  // ld a,$02
  CALL_C(b_+107, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+110);
  CYC(b_+110, b_+112); A = mem_rd(gb, hActiveObject);  // ldh a,(<hActiveObject)
  CYC(b_+112, b_+113); D = A;  // ld d,a
  RET(b_+113); return;  // ret
substate3:
  CYC(b_+114, b_+116); A = 0x04;  // ld a,$04
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+117, b_+120); if (hook_enabled_at(gb, SYM(fadeinFromWhiteWithDelay))) { fadeinFromWhiteWithDelay_hook(gb); return; } HANDOFF(SYM(fadeinFromWhiteWithDelay));  // jp fadeinFromWhiteWithDelay
substate4:
  CYC(b_+120, b_+123); A = mem_rd(gb, wPaletteThread_mode);  // ld a,(wPaletteThread_mode)
  CYC(b_+123, b_+124); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+124); return; } CYC(b_+124, b_+125);  // ret nz
  CYC(b_+125, b_+126); H = D;  // ld h,d
  CYC(b_+126, b_+128); L = 0x84;  // ld l,Enemy.state
  CYC(b_+128, b_+129); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+129, b_+131); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x81);  // ld (hl),$80|ENEMY_MERGED_TWINROVA
  CYC(b_+133, b_+135); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+135, b_+137); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+137, b_+139); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+139, b_+141); mem_wr(gb, HL, 210);  // ld (hl),210
  CYC(b_+141, b_+143); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+143, b_+144); alu_xor(gb, A);  // xor a
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+145, b_+148); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(b_+148, b_+150); A = 0xf1;  // ld a,SNDCTRL_STOPSFX
  CYC(b_+150, b_+153); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void mergedTwinrova_stateB_hook(GB *gb) {
  BASE(mergedTwinrova_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+5); L = E;  // ld l,e
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0c);  // ld (hl),$0c
  RET(b_+7); return;  // ret
}

void mergedTwinrova_lavaRoom_hook(GB *gb) {
  BASE(mergedTwinrova_lavaRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+1, b_+3); alu_sub(gb, 0x0c);  // sub $0c
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == SYM(mergedTwinrova_lavaRoom_stateC)) { mergedTwinrova_lavaRoom_stateC_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_lavaRoom_stateD)) { mergedTwinrova_lavaRoom_stateD_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_lavaRoom_stateE)) { mergedTwinrova_lavaRoom_stateE_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void mergedTwinrova_lavaRoom_stateC_hook(GB *gb) {
  BASE(mergedTwinrova_lavaRoom_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, mergedTwinrova_decVar3bIfNonzero_hook, SYM(mergedTwinrova_decVar3bIfNonzero), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x32);  // ld (hl),SPEED_140
  mergedTwinrova_chooseTargetPosition_hook(gb); return;  // fallthrough
}

void mergedTwinrova_chooseTargetPosition_hook(GB *gb) {
  BASE(mergedTwinrova_chooseTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); L = E;  // ld l,e
  CYC(b_+1, b_+2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
chooseTargetPositionIndex:
  CALL_C(b_+2, getRandomNumber_hook, SYM(getRandomNumber), b_+5);
  CYC(b_+5, b_+7); alu_and(gb, 0x0e);  // and $0e
  CYC(b_+7, b_+9); L = 0xb6;  // ld l,Enemy.var36
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));  // cp (hl)
  if ((F & FZ)) { CYCT(b_+10, b_+12); goto chooseTargetPositionIndex; } CYC(b_+10, b_+12);  // jr z,@chooseTargetPositionIndex
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+13, b_+16); SET_BC(b_+32);  // ld bc,@targetPositions
  CALL_C(b_+16, addAToBc_hook, 0x006d, b_+19);
  CYC(b_+19, b_+21); E = 0xb7;  // ld e,Enemy.var37
  CYC(b_+21, b_+22); A = mem_rd(gb, BC);  // ld a,(bc)
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);  // inc e
  CYC(b_+24, b_+25); SET_BC(BC + 1);  // inc bc
  CYC(b_+25, b_+26); A = mem_rd(gb, BC);  // ld a,(bc)
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+27, b_+29); A = 0xc9;  // ld a,SND_CIRCLING
  CYC(b_+29, b_+32); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void mergedTwinrova_lavaRoom_stateD_hook(GB *gb) {
  BASE(mergedTwinrova_lavaRoom_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xb3;  // ld l,Enemy.var33
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+4, b_+5); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto readTarget; } CYC(b_+5, b_+7);  // jr z,+
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
readTarget:
  CYC(b_+8, b_+10); L = 0xb7;  // ld l,Enemy.var37
  CALL_C(b_+10, ecom_readPositionVars_b10_hook, SYM(ecom_readPositionVars_b10), b_+13);
  CALL_C(b_+13, mergedTwinrova_checkPositionsCloseEnough_hook, SYM(mergedTwinrova_checkPositionsCloseEnough), b_+16);
  if (!(F & FC)) { CYCT(b_+16, b_+19); if (hook_enabled_at(gb, SYM(ecom_moveTowardPosition_b10))) { ecom_moveTowardPosition_b10_hook(gb); return; } HANDOFF(SYM(ecom_moveTowardPosition_b10)); } CYC(b_+16, b_+19);  // jp nc,ecom_moveTowardPosition
  CYC(b_+19, b_+21); L = 0xb3;  // ld l,Enemy.var33
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+22, b_+23); alu_or(gb, A);  // or a
  CYC(b_+23, b_+25); L = 0x84;  // ld l,Enemy.state
  if ((F & FZ)) { CYCT(b_+25, b_+27); goto attack; } CYC(b_+25, b_+27);  // jr z,@attack
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(b_+28, b_+30); L = 0xbb;  // ld l,Enemy.var3b
  CYC(b_+30, b_+32); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(b_+32); return;  // ret
attack:
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);  // inc l
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+37, b_+39); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(b_+41, b_+43); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+45, b_+47); alu_and(gb, 0x07);  // and $07
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+48, b_+51); SET_HL(b_+57);  // ld hl,@var03Vals
  CYC(b_+51, b_+52);
  mergedTwinrova_addAToHl_from_rst(gb, b_+52);  // rst_addAToHl
  CYC(b_+52, b_+54); E = 0x83;  // ld e,Enemy.var03
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);  // ld (de),a
  RET(b_+56); return;  // ret
}

void mergedTwinrova_lavaRoom_stateE_hook(GB *gb) {
  BASE(mergedTwinrova_lavaRoom_stateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x83;  // ld e,Enemy.var03
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(b_+4, b_+7); goto keeseAttack; } CYC(b_+4, b_+7);  // jp nz,@keeseAttack
  CYC(b_+7, b_+9); E = 0x85;  // ld e,Enemy.substate
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == b_+17) { goto flameAttack_substate0; }
    else if (jt_ == b_+47) { goto flameAttack_substate1; }
    else if (jt_ == b_+62) { goto flameAttack_substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
flameAttack_substate0:
  CYC(b_+17, b_+18); H = D;  // ld h,d
  CYC(b_+18, b_+19); L = E;  // ld l,e
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);  // inc l
  CYC(b_+21, b_+23); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+23, b_+25); A = 0x03;  // ld a,$03
  CALL_C(b_+25, enemySetAnimation_hook, SYM(enemySetAnimation), b_+28);
  CALL_C(b_+28, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+31);
  CYC(b_+31, b_+32); B = A;  // ld b,a
  CALL_C(b_+32, getFreePartSlot_hook, SYM(getFreePartSlot), b_+35);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; } CYC(b_+35, b_+36);  // ret nz
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x4c);  // ld (hl),PART_TWINROVA_FLAME
  CYC(b_+38, b_+40); L = 0xc9;  // ld l,Part.angle
  CYC(b_+40, b_+41); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+41, b_+44); SET_BC(0x1000);  // ld bc,$1000
  CYC(b_+44, b_+47); if (hook_enabled_at(gb, SYM(objectCopyPositionWithOffset))) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(SYM(objectCopyPositionWithOffset));  // jp objectCopyPositionWithOffset
flameAttack_substate1:
  CALL_C(b_+47, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+50);
  if (!(F & FZ)) { CYCT(b_+50, b_+53); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+50, b_+53);  // jp nz,enemyAnimate
  CYC(b_+53, b_+55); mem_wr(gb, HL, 16);  // ld (hl),16
  CYC(b_+55, b_+56); L = E;  // ld l,e
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+57, b_+59); A = 0x04;  // ld a,$04
  CYC(b_+59, b_+62); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
flameAttack_substate2:
  CALL_C(b_+62, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+65);
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; } CYC(b_+65, b_+66);  // ret nz
doneAttack:
  CYC(b_+66, b_+68); L = 0x84;  // ld l,Enemy.state
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x0c);  // ld (hl),$0c
  CYC(b_+70, b_+72); L = 0xbb;  // ld l,Enemy.var3b
  CYC(b_+72, b_+74); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+74, b_+76); L = 0xb3;  // ld l,Enemy.var33
  CYC(b_+76, b_+78); mem_wr(gb, HL, 150);  // ld (hl),150
  CYC(b_+78, b_+80); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+82, b_+84); A = 0x01;  // ld a,$01
  CYC(b_+84, b_+87); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
keeseAttack:
  CYC(b_+87, b_+89); E = 0x85;  // ld e,Enemy.substate
  CYC(b_+89, b_+90); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+90, b_+91); push_effect(gb, b_+91);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == b_+97) { goto keeseAttack_substate0; }
    else if (jt_ == b_+111) { goto keeseAttack_substate1; }
    else if (jt_ == b_+165) { goto keeseAttack_substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
keeseAttack_substate0:
  CYC(b_+97, b_+98); H = D;  // ld h,d
  CYC(b_+98, b_+99); L = E;  // ld l,e
  CYC(b_+99, b_+100); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+100, b_+101); L = alu_inc8(gb, L);  // inc l
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x0a);  // ld (hl),$0a
  CYC(b_+103, b_+104); L = alu_inc8(gb, L);  // inc l
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x05);  // ld (hl),$05
  CYC(b_+106, b_+108); A = 0x07;  // ld a,$07
  CYC(b_+108, b_+111); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
keeseAttack_substate1:
  CALL_C(b_+111, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+114);
  if (!(F & FZ)) { CYCT(b_+114, b_+117); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+114, b_+117);  // jp nz,enemyAnimate
  CYC(b_+117, b_+119); mem_wr(gb, HL, 20);  // ld (hl),20
  CYC(b_+119, b_+120); L = alu_inc8(gb, L);  // inc l
  CYC(b_+120, b_+121); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if ((F & FZ)) { CYCT(b_+121, b_+123); goto doneSpawningKeese; } CYC(b_+121, b_+123);  // jr z,@doneSpawningKeese
  CYC(b_+123, b_+124); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+124, b_+125); A = alu_dec8(gb, A);  // dec a
  CYC(b_+125, b_+128); SET_HL(b_+157);  // ld hl,@keesePositions
  CYC(b_+128, b_+129);
  mergedTwinrova_addDoubleIndexToHl_from_rst(gb, b_+129);  // rst_addDoubleIndexToHl
  CYC(b_+129, b_+130); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(b_+130, b_+131); B = A;  // ld b,a
  CYC(b_+131, b_+132); C = mem_rd(gb, HL);  // ld c,(hl)
  CALL_C(b_+132, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+135);
  if (!(F & FZ)) { RET_TAKEN(b_+135); return; } CYC(b_+135, b_+136);  // ret nz
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x5e);  // ld (hl),ENEMY_TWINROVA_BAT
  CYC(b_+138, b_+140); L = 0x97;  // ld l,Enemy.relatedObj1+1
  CYC(b_+140, b_+141); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(b_+141, b_+142); L = alu_dec8(gb, L);  // dec l
  CYC(b_+142, b_+144); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(b_+144, b_+147); if (hook_enabled_at(gb, SYM(objectCopyPositionWithOffset))) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(SYM(objectCopyPositionWithOffset));  // jp objectCopyPositionWithOffset
doneSpawningKeese:
  CYC(b_+147, b_+148); L = alu_dec8(gb, L);  // dec l
  CYC(b_+148, b_+150); mem_wr(gb, HL, 180);  // ld (hl),180
  CYC(b_+150, b_+151); L = E;  // ld l,e
  CYC(b_+151, b_+152); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+152, b_+154); A = 0x03;  // ld a,$03
  CYC(b_+154, b_+157); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
keeseAttack_substate2:
  CALL_C(b_+165, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+168);
  if (!(F & FZ)) { RET_TAKEN(b_+168); return; } CYC(b_+168, b_+169);  // ret nz
  CYCT(b_+169, b_+171); goto doneAttack;  // jr @doneAttack
}

void mergedTwinrova_iceRoom_hook(GB *gb) {
  BASE(mergedTwinrova_iceRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+1, b_+3); alu_sub(gb, 0x0c);  // sub $0c
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == SYM(mergedTwinrova_iceRoom_stateC)) { mergedTwinrova_iceRoom_stateC_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_iceRoom_stateD)) { mergedTwinrova_iceRoom_stateD_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_iceRoom_stateE)) { mergedTwinrova_iceRoom_stateE_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_iceRoom_stateF)) { mergedTwinrova_iceRoom_stateF_hook(gb); return; }
    else if (jt_ == SYM(mergedTwinrova_iceRoom_state10)) { mergedTwinrova_iceRoom_state10_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void mergedTwinrova_iceRoom_stateC_hook(GB *gb) {
  BASE(mergedTwinrova_iceRoom_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+2); L = E;  // ld l,e
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);  // inc l
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);  // inc l
  CYC(b_+7, b_+9); mem_wr(gb, HL, 10);  // ld (hl),10
  CYC(b_+9, b_+11); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CALL_C(b_+13, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x01);  // and $01
  CYC(b_+18, b_+20); E = 0xb5;  // ld e,Enemy.var35
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+21, b_+23); E = 0xb4;  // ld e,Enemy.var34
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+24, b_+26); alu_cp(gb, 0x02);  // cp $02
  CYC(b_+26, b_+28); A = 0x03;  // ld a,$03
  if (!(F & FC)) { CYCT(b_+28, b_+30); goto counter2Set; } CYC(b_+28, b_+30);  // jr nc,+
  CYC(b_+30, b_+31); A = alu_inc8(gb, A);  // inc a
counter2Set:
  CYC(b_+31, b_+33); E = 0x87;  // ld e,Enemy.counter2
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+34, b_+36); A = 0x05;  // ld a,$05
  CYC(b_+36, b_+39); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void mergedTwinrova_iceRoom_stateD_hook(GB *gb) {
  BASE(mergedTwinrova_iceRoom_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == b_+7) { goto substate0; }
    else if (jt_ == b_+102) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CALL_C(b_+7, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+10, b_+13);  // jp nz,enemyAnimate
  CYC(b_+13, b_+15); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);  // inc l
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if ((F & FZ)) { CYCT(b_+17, b_+19); goto doneSpawningProjectiles; } CYC(b_+17, b_+19);  // jr z,@doneSpawningProjectiles
  CYC(b_+19, b_+20); B = mem_rd(gb, HL);  // ld b,(hl)
  CYC(b_+20, b_+21); B = alu_dec8(gb, B);  // dec b
  CYC(b_+21, b_+23); L = 0xb4;  // ld l,Enemy.var34
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+24, b_+26); alu_cp(gb, 0x02);  // cp $02
  CYC(b_+26, b_+29); SET_HL(b_+94);  // ld hl,@positionData1
  if (!(F & FC)) { CYCT(b_+29, b_+31); goto haveTable; } CYC(b_+29, b_+31);  // jr nc,+
  CYC(b_+31, b_+34); SET_HL(b_+82);  // ld hl,@positionData0
haveTable:
  CYC(b_+34, b_+35); A = B;  // ld a,b
  CYC(b_+35, b_+36); alu_add(gb, A);  // add a
  CYC(b_+36, b_+37);
  mergedTwinrova_addDoubleIndexToHl_from_rst(gb, b_+37);  // rst_addDoubleIndexToHl
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(b_+38, b_+39); B = A;  // ld b,a
  CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(b_+40, b_+41); C = A;  // ld c,a
  CYC(b_+41, b_+43); E = 0xb5;  // ld e,Enemy.var35
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+44, b_+45); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+45, b_+47); goto readAngle; } CYC(b_+45, b_+47);  // jr z,+
  CYC(b_+47, b_+48); SET_HL(HL + 1);  // inc hl
readAngle:
  CYC(b_+48, b_+49); E = mem_rd(gb, HL);  // ld e,(hl)
  CALL_C(b_+49, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+52);
  if (!(F & FZ)) { RET_TAKEN(b_+52); return; } CYC(b_+52, b_+53);  // ret nz
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x5d);  // ld (hl),ENEMY_TWINROVA_ICE
  CYC(b_+55, b_+57); L = 0x89;  // ld l,Enemy.angle
  CYC(b_+57, b_+58); mem_wr(gb, HL, E);  // ld (hl),e
  CYC(b_+58, b_+60); L = 0x96;  // ld l,Enemy.relatedObj1
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(b_+62, b_+63); L = alu_inc8(gb, L);  // inc l
  CYC(b_+63, b_+64); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(b_+64, b_+67); if (hook_enabled_at(gb, SYM(objectCopyPositionWithOffset))) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(SYM(objectCopyPositionWithOffset));  // jp objectCopyPositionWithOffset
doneSpawningProjectiles:
  CYC(b_+67, b_+68); L = E;  // ld l,e
  CYC(b_+68, b_+69); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+69, b_+71); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+71, b_+73); mem_wr(gb, HL, 120);  // ld (hl),120
  CYC(b_+73, b_+75); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+77, b_+79); A = 0x06;  // ld a,$06
  CYC(b_+79, b_+82); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
substate1:
  CALL_C(b_+102, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+105);
  if (!(F & FZ)) { RET_TAKEN(b_+105); return; } CYC(b_+105, b_+106);  // ret nz
  CYC(b_+106, b_+108); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(b_+108, b_+110); L = 0x84;  // ld l,Enemy.state
  CYC(b_+110, b_+111); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+111, b_+113); A = 0x02;  // ld a,$02
  CYC(b_+113, b_+116); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void mergedTwinrova_iceRoom_stateE_hook(GB *gb) {
  BASE(mergedTwinrova_iceRoom_stateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, mergedTwinrova_decVar3bIfNonzero_hook, SYM(mergedTwinrova_decVar3bIfNonzero), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x3c);  // ld (hl),SPEED_180
  CYC(b_+8, b_+11); mergedTwinrova_chooseTargetPosition_hook(gb); return;  // jp mergedTwinrova_chooseTargetPosition
}

void mergedTwinrova_iceRoom_stateF_hook(GB *gb) {
  BASE(mergedTwinrova_iceRoom_stateF);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xb7;  // ld l,Enemy.var37
  CALL_C(b_+3, ecom_readPositionVars_b10_hook, SYM(ecom_readPositionVars_b10), b_+6);
  CALL_C(b_+6, mergedTwinrova_checkPositionsCloseEnough_hook, SYM(mergedTwinrova_checkPositionsCloseEnough), b_+9);
  if (!(F & FC)) { CYCT(b_+9, b_+12); if (hook_enabled_at(gb, SYM(ecom_moveTowardPosition_b10))) { ecom_moveTowardPosition_b10_hook(gb); return; } HANDOFF(SYM(ecom_moveTowardPosition_b10)); } CYC(b_+9, b_+12);  // jp nc,ecom_moveTowardPosition
  CYC(b_+12, b_+14); L = 0x84;  // ld l,Enemy.state
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);  // inc l
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);  // inc l
  CYC(b_+19, b_+21); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+21, b_+23); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(b_+25, b_+27); A = 0x08;  // ld a,$08
  CALL_C(b_+27, enemySetAnimation_hook, SYM(enemySetAnimation), b_+30);
  CALL_C(b_+30, getFreePartSlot_hook, SYM(getFreePartSlot), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } CYC(b_+33, b_+34);  // ret nz
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x4e);  // ld (hl),PART_TWINROVA_SNOWBALL
  CYC(b_+36, b_+39); SET_BC(0xe800);  // ld bc,$e800
  CYC(b_+39, b_+42); if (hook_enabled_at(gb, SYM(objectCopyPositionWithOffset))) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(SYM(objectCopyPositionWithOffset));  // jp objectCopyPositionWithOffset
}

void mergedTwinrova_iceRoom_state10_hook(GB *gb) {
  BASE(mergedTwinrova_iceRoom_state10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == b_+7) { goto substate0; }
    else if (jt_ == b_+22) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CALL_C(b_+7, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+10, b_+13);  // jp nz,enemyAnimate
  CYC(b_+13, b_+15); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+15, b_+16); L = E;  // ld l,e
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+17, b_+19); A = 0x06;  // ld a,$06
  CYC(b_+19, b_+22); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
substate1:
  CALL_C(b_+22, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+25);
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; } CYC(b_+25, b_+26);  // ret nz
  CYC(b_+26, b_+28); L = 0x84;  // ld l,Enemy.state
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x0e);  // ld (hl),$0e
  CYC(b_+30, b_+32); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+32, b_+34); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(b_+34, b_+36); L = 0xbb;  // ld l,Enemy.var3b
  CYC(b_+36, b_+38); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+38, b_+40); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+42, b_+44); A = 0x02;  // ld a,$02
  CYC(b_+44, b_+47); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void mergedTwinrova_checkTimeToSwapRoomFromTimer_hook(GB *gb) {
  BASE(mergedTwinrova_checkTimeToSwapRoomFromTimer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);  // ld a,(wFrameCounter)
  CYC(b_+3, b_+5); alu_and(gb, 0x03);  // and $03
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } CYC(b_+5, b_+6);  // ret nz
  CYC(b_+6, b_+7); H = D;  // ld h,d
  CYC(b_+7, b_+9); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } CYC(b_+10, b_+11);  // ret nz
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+12, b_+14); E = 0xb9;  // ld e,Enemy.var39
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+15, b_+16); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; } CYC(b_+16, b_+17);  // ret nz
  CYC(b_+17, b_+19); mem_wr(gb, HL, 210);  // ld (hl),210
  CYC(b_+19, b_+21); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+21, b_+23); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+23, b_+25); L = 0x84;  // ld l,Enemy.state
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x09);  // ld (hl),$09
  RET(b_+27); return;  // ret
}

void mergedTwinrova_checkPositionsCloseEnough_hook(GB *gb) {
  BASE(mergedTwinrova_checkPositionsCloseEnough);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_sub(gb, C);  // sub c
  CYC(b_+1, b_+3); alu_add(gb, 0x02);  // add $02
  CYC(b_+3, b_+5); alu_cp(gb, 0x05);  // cp $05
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } CYC(b_+5, b_+6);  // ret nc
  CYC(b_+6, b_+8); A = mem_rd(gb, hFF8F);  // ldh a,(<hFF8F)
  CYC(b_+8, b_+9); alu_sub(gb, B);  // sub b
  CYC(b_+9, b_+11); alu_add(gb, 0x02);  // add $02
  CYC(b_+11, b_+13); alu_cp(gb, 0x05);  // cp $05
  RET(b_+13); return;  // ret
}

void mergedTwinrova_checkTimeToSwapRoomFromDamage_hook(GB *gb) {
  BASE(mergedTwinrova_checkTimeToSwapRoomFromDamage);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xba;  // ld l,Enemy.var3a
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+4, b_+5); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(b_+5); return; } CYC(b_+5, b_+6);  // ret z
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if ((F & FZ)) { CYCT(b_+7, b_+9); goto setState9; } CYC(b_+7, b_+9);  // jr z,+
  SET_HL(POP(b_+9));  // pop hl
  RET(b_+10); return;  // ret
setState9:
  CYC(b_+11, b_+13); L = 0x84;  // ld l,Enemy.state
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x09);  // ld (hl),$09
  CYC(b_+15, b_+17); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+18, b_+19); B = A;  // ld b,a
  CYC(b_+19, b_+21); alu_xor(gb, 0x01);  // xor $01
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);  // inc a
  CYC(b_+22, b_+24); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+26, b_+27); A = B;  // ld a,b
  CYC(b_+27, b_+28); A = alu_inc8(gb, A);  // inc a
  CYC(b_+28, b_+31); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void mergedTwinrova_deathCutscene_hook(GB *gb) {
  BASE(mergedTwinrova_deathCutscene);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x85;  // ld e,Enemy.substate
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (mergedTwinrova_jump_table(gb));
    if (jt_ == b_+12) { goto substate0; }
    else if (jt_ == b_+56) { goto substate1; }
    else if (jt_ == b_+78) { goto substate2; }
    else if (jt_ == b_+91) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+12, b_+15); A = mem_rd(gb, wDisabledObjects);  // ld a,(wDisabledObjects)
  CYC(b_+15, b_+16); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto flickerAndDecCounter; } CYC(b_+16, b_+18);  // jr nz,++
  CYC(b_+18, b_+21); A = mem_rd(gb, wLinkDeathTrigger);  // ld a,(wLinkDeathTrigger)
  CYC(b_+21, b_+22); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; } CYC(b_+22, b_+23);  // ret nz
  CYC(b_+23, b_+24); A = alu_inc8(gb, A);  // inc a
  CYC(b_+24, b_+27); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(b_+27, b_+30); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CALL_C(b_+30, clearAllParentItems_hook, SYM(clearAllParentItems), b_+33);
flickerAndDecCounter:
  CALL_C(b_+33, ecom_flickerVisibility_b10_hook, SYM(ecom_flickerVisibility_b10), b_+36);
  CALL_C(b_+36, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+39);
  if ((F & FZ)) { CYCT(b_+39, b_+41); goto doneExplosions; } CYC(b_+39, b_+41);  // jr z,@doneExplosions
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+42, b_+44); alu_cp(gb, 97);  // cp 97
  if (!(F & FC)) { RET_TAKEN(b_+44); return; } CYC(b_+44, b_+45);  // ret nc
  CYC(b_+45, b_+47); alu_and(gb, 0x0f);  // and $0f
  if ((F & FZ)) { CYCT(b_+47, b_+50); goto createExplosion; } CYC(b_+47, b_+50);  // jp z,@createExplosion
  RET(b_+50); return;  // ret
doneExplosions:
  CYC(b_+51, b_+53); mem_wr(gb, HL, 25);  // ld (hl),25
  CYC(b_+53, b_+55); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
substate1:
  CALL_C(b_+56, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+59);
  if (!(F & FZ)) { CYCT(b_+59, b_+62); if (hook_enabled_at(gb, SYM(ecom_flickerVisibility_b10))) { ecom_flickerVisibility_b10_hook(gb); return; } HANDOFF(SYM(ecom_flickerVisibility_b10)); } CYC(b_+59, b_+62);  // jp nz,ecom_flickerVisibility
  CYC(b_+62, b_+63); L = E;  // ld l,e
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+64, b_+66); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+66, b_+67); alu_xor(gb, A);  // xor a
  CYC(b_+67, b_+68); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+68, b_+69); mem_wr(gb, HL, A);  // ld (hl),a
  CALL_C(b_+69, enemySetAnimation_hook, SYM(enemySetAnimation), b_+72);
  CYC(b_+72, b_+75); SET_BC(0x2f0c);  // ld bc,TX_2f0c
  CYC(b_+75, b_+78); if (hook_enabled_at(gb, SYM(showText))) { showText_hook(gb); return; } HANDOFF(SYM(showText));  // jp showText
substate2:
  CYC(b_+78, b_+80); A = 0x03;  // ld a,$03
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+81, b_+83); A = 0x19;  // ld a,CUTSCENE_TWINROVA_SACRIFICE
  CYC(b_+83, b_+86); mem_wr(gb, wCutsceneTrigger, A);  // ld (wCutsceneTrigger),a
  CYC(b_+86, b_+88); A = 0x1d;  // ld a,MUS_ROOM_OF_RITES
  CYC(b_+88, b_+91); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
substate3:
  RET(b_+91); return;  // ret
createExplosion:
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+93, b_+95); A = alu_swap(gb, A);  // swap a
  CYC(b_+95, b_+96); A = alu_dec8(gb, A);  // dec a
  CYC(b_+96, b_+99); SET_HL(b_+124);  // ld hl,@explosionPositions
  CYC(b_+99, b_+100);
  mergedTwinrova_addDoubleIndexToHl_from_rst(gb, b_+100);  // rst_addDoubleIndexToHl
  CYC(b_+100, b_+102); E = 0x8b;  // ld e,Enemy.yh
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+103, b_+104); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(b_+104, b_+105); B = A;  // ld b,a
  CYC(b_+105, b_+107); E = 0x8d;  // ld e,Enemy.xh
  CYC(b_+107, b_+108); SET_HL(HL + 1);  // inc hl
  CYC(b_+108, b_+109); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+109, b_+110); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(b_+110, b_+111); C = A;  // ld c,a
  CALL_C(b_+111, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+114);
  if (!(F & FZ)) { RET_TAKEN(b_+114); return; } CYC(b_+114, b_+115);  // ret nz
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x56);  // ld (hl),INTERAC_EXPLOSION
  CYC(b_+117, b_+119); L = 0x4b;  // ld l,Interaction.yh
  CYC(b_+119, b_+120); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+120, b_+122); L = 0x4d;  // ld l,Interaction.xh
  CYC(b_+122, b_+123); mem_wr(gb, HL, C);  // ld (hl),c
  RET(b_+123); return;  // ret
}

void mergedTwinrova_decVar3bIfNonzero_hook(GB *gb) {
  BASE(mergedTwinrova_decVar3bIfNonzero);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xbb;  // ld l,Enemy.var3b
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+4, b_+5); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(b_+5); return; } CYC(b_+5, b_+6);  // ret z
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  RET(b_+7); return;  // ret
}
