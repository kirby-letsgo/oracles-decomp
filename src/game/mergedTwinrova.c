#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x45f1, mergedTwinrova_checkTimeToSwapRoomFromDamage_hook, 0x49a8, 0x45f4);
  CALL_C(0x45f4, mergedTwinrova_checkTimeToSwapRoomFromTimer_hook, 0x497e, 0x45f7);
  CALL_C(0x45f7, ecom_getSubidAndCpStateTo08_b10_hook, 0x4426, 0x45fa);
  CYC(0x45fa, 0x45fc); alu_cp(gb, 0x0c);  // cp $0c
  if (!(F & FC)) { CYCT(0x45fc, 0x45fe); goto stateCOrHigher; } CYC(0x45fc, 0x45fe);  // jr nc,@stateCOrHigher
  CYC(0x45fe, 0x45ff); push_effect(gb, 0x45ff);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x461d: mergedTwinrova_state_uninitialized_hook(gb); return;
    case 0x464a: mergedTwinrova_state_stub_hook(gb); return;
    case 0x464b: mergedTwinrova_state8_hook(gb); return;
    case 0x467a: mergedTwinrova_state9_hook(gb); return;
    case 0x46a2: mergedTwinrova_stateA_hook(gb); return;
    case 0x473b: mergedTwinrova_stateB_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
stateCOrHigher:
  CYC(0x4617, 0x4618); A = B;  // ld a,b
  CYC(0x4618, 0x4619); push_effect(gb, 0x4619);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x4743: mergedTwinrova_lavaRoom_hook(gb); return;
    case 0x4871: mergedTwinrova_iceRoom_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void enemyCode01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if ((F & FZ)) { CYCT(0x4594, 0x4596); goto normalStatus; } CYC(0x4594, 0x4596);  // jr z,@normalStatus
  CYC(0x4596, 0x4598); alu_sub(gb, 0x03);  // sub ENEMYSTATUS_NO_HEALTH
  if ((F & FC)) { RET_TAKEN(0x4598); return; } CYC(0x4598, 0x4599);  // ret c
  if (!(F & FZ)) { CYCT(0x4599, 0x459b); goto collisionOccurred; } CYC(0x4599, 0x459b);  // jr nz,@collisionOccurred
  CYC(0x459b, 0x459d); E = 0xb4;  // ld e,Enemy.var34
  CYC(0x459d, 0x459e); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x459e, 0x459f); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x459f, 0x45a2); mergedTwinrova_deathCutscene_hook(gb); return; } CYC(0x459f, 0x45a2);  // jp z,mergedTwinrova_deathCutscene
  CYC(0x45a2, 0x45a3); H = D;  // ld h,d
  CYC(0x45a3, 0x45a5); L = 0xba;  // ld l,Enemy.var3a
  CYC(0x45a5, 0x45a7); mem_wr(gb, HL, 120);  // ld (hl),120
  CYC(0x45a7, 0x45a9); L = 0xa9;  // ld l,Enemy.health
  CYC(0x45a9, 0x45ab); mem_wr(gb, HL, 20);  // ld (hl),20
  CYC(0x45ab, 0x45ad); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x45ad, 0x45af); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x45af, 0x45b1); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x45b1, 0x45b2); alu_xor(gb, A);  // xor a
  CYC(0x45b2, 0x45b3); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x45b3, 0x45b4); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x45b4, 0x45b6); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x45b6, 0x45b8); mem_wr(gb, HL, 0xff);  // ld (hl),$ff
  CYC(0x45b8, 0x45ba); A = 0x09;  // ld a,$09
  CYC(0x45ba, 0x45bd); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
collisionOccurred:
  CYC(0x45bd, 0x45bf); E = 0xba;  // ld e,Enemy.var3a
  CYC(0x45bf, 0x45c0); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x45c0, 0x45c1); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x45c1, 0x45c3); goto normalStatus; } CYC(0x45c1, 0x45c3);  // jr z,@normalStatus
  CYC(0x45c3, 0x45c5); E = 0xaa;  // ld e,Enemy.var2a
  CYC(0x45c5, 0x45c6); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x45c6, 0x45c8); A = (uint8_t)(A & ~(1 << 7));  // res 7,a
  CYC(0x45c8, 0x45ca); alu_sub(gb, 0x1a);  // sub ITEMCOLLISION_MYSTERY_SEED
  CYC(0x45ca, 0x45cc); alu_cp(gb, 0x05);  // cp ITEMCOLLISION_GALE_SEED - ITEMCOLLISION_MYSTERY_SEED + 1
  if (!(F & FC)) { CYCT(0x45cc, 0x45ce); goto normalStatus; } CYC(0x45cc, 0x45ce);  // jr nc,@normalStatus
  CYC(0x45ce, 0x45d0); A = 0x63;  // ld a,SND_BOSS_DAMAGE
  CALL_C(0x45d0, playSound_b00_hook, 0x0c98, 0x45d3);
  CYC(0x45d3, 0x45d4); H = D;  // ld h,d
  CYC(0x45d4, 0x45d6); L = 0xab;  // ld l,Enemy.invincibilityCounter
  CYC(0x45d6, 0x45d8); mem_wr(gb, HL, 45);  // ld (hl),45
  CYC(0x45d8, 0x45da); L = 0xb4;  // ld l,Enemy.var34
  CYC(0x45da, 0x45db); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if (!(F & FZ)) { CYCT(0x45db, 0x45dd); goto normalStatus; } CYC(0x45db, 0x45dd);  // jr nz,@normalStatus
  CYC(0x45dd, 0x45df); L = 0xa9;  // ld l,Enemy.health
  CYC(0x45df, 0x45e1); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x45e1, 0x45e3); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x45e3, 0x45e5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x45e5, 0x45e7); L = 0x85;  // ld l,Enemy.substate
  CYC(0x45e7, 0x45e9); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x45e9, 0x45ea); L = alu_inc8(gb, L);  // inc l
  CYC(0x45ea, 0x45ec); mem_wr(gb, HL, 216);  // ld (hl),216
  CYC(0x45ec, 0x45ee); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CYC(0x45ee, 0x45f1); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
normalStatus:
  CYC(0x45f1, 0x45f4); push_effect(gb, 0x45f4); enemyCode01__normalStatus_hook(gb);  // call @normalStatus
}

void mergedTwinrova_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x461d, 0x461f); A = 0x01;  // ld a,ENEMY_MERGED_TWINROVA
  CYC(0x461f, 0x4622); mem_wr(gb, 0xcc1d, A);  // ld (wEnemyIDToLoadExtraGfx),a
  CYC(0x4622, 0x4624); A = mem_rd(gb, 0xffaf);  // ldh a,(<hActiveObject)
  CYC(0x4624, 0x4625); D = A;  // ld d,a
  CYC(0x4625, 0x4628); SET_BC(0x0012);  // ld bc,$0012
  CALL_C(0x4628, enemyBoss_spawnShadow_b10_hook, 0x4534, 0x462b);
  if (!(F & FZ)) { RET_TAKEN(0x462b); return; } CYC(0x462b, 0x462c);  // ret nz
  CYC(0x462c, 0x462e); A = 0x3c;  // ld a,SPEED_180
  CALL_C(0x462e, ecom_setSpeedAndState8_b10_hook, 0x4364, 0x4631);
  CYC(0x4631, 0x4633); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x4633, 0x4635); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x4635, 0x4637); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x4637, 0x4639); mem_wr(gb, HL, 210);  // ld (hl),210
  CYC(0x4639, 0x463b); L = 0xb4;  // ld l,Enemy.var34
  CYC(0x463b, 0x463d); mem_wr(gb, HL, 0x05);  // ld (hl),$05
  CYC(0x463d, 0x463f); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x463f, 0x4641); mem_wr(gb, HL, 0xff);  // ld (hl),$ff
  CALL_C(0x4641, objectSetVisible83_hook, 0x1e72, 0x4644);
  CYC(0x4644, 0x4647); SET_BC(0x2f0b);  // ld bc,TX_2f0b
  CYC(0x4647, 0x464a); if (hook_enabled_at(0x1872)) { showText_hook(gb); return; } HANDOFF(0x1872);  // jp showText
}

void mergedTwinrova_state_stub_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(0x464a); return;  // ret
}

void mergedTwinrova_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x464b, ecom_decCounter1_b10_hook, 0x439a, 0x464e);
  if (!(F & FZ)) { RET_TAKEN(0x464e); return; } CYC(0x464e, 0x464f);  // ret nz
  CYC(0x464f, 0x4650); L = E;  // ld l,e
  CYC(0x4650, 0x4651); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4651, 0x4653); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x4653, 0x4655); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(0x4655, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4658);
  CYC(0x4658, 0x465a); alu_and(gb, 0x01);  // and $01
  CYC(0x465a, 0x465c); E = 0x82;  // ld e,Enemy.subid
  CYC(0x465c, 0x465d); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x465d, 0x465e); B = A;  // ld b,a
  CYC(0x465e, 0x4660); alu_xor(gb, 0x01);  // xor $01
  CYC(0x4660, 0x4661); A = alu_inc8(gb, A);  // inc a
  CYC(0x4661, 0x4663); E = 0x9b;  // ld e,Enemy.oamFlagsBackup
  CYC(0x4663, 0x4664); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4664, 0x4665); E = alu_inc8(gb, E);  // inc e
  CYC(0x4665, 0x4666); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4666, 0x4667); A = B;  // ld a,b
  CYC(0x4667, 0x4668); A = alu_inc8(gb, A);  // inc a
  CALL_C(0x4668, enemySetAnimation_hook, 0x282b, 0x466b);
  CYC(0x466b, 0x466c); alu_xor(gb, A);  // xor a
  CYC(0x466c, 0x466f); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CYC(0x466f, 0x4672); mem_wr(gb, 0xcbca, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(0x4672, 0x4674); A = 0x33;  // ld a,MUS_TWINROVA
  CYC(0x4674, 0x4677); mem_wr(gb, 0xcc35, A);  // ld (wActiveMusic),a
  CYC(0x4677, 0x467a); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void mergedTwinrova_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x467a, 0x467d); SET_BC(0x4878);  // ld bc,$4878
  CYC(0x467d, 0x467e); H = D;  // ld h,d
  CYC(0x467e, 0x4680); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x4680, 0x4681); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(0x4681, 0x4683); mem_wr(gb, 0xff8f, A);  // ldh (<hFF8F),a
  CYC(0x4683, 0x4684); L = alu_inc8(gb, L);  // inc l
  CYC(0x4684, 0x4685); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4685, 0x4687); mem_wr(gb, 0xff8e, A);  // ldh (<hFF8E),a
  CALL_C(0x4687, mergedTwinrova_checkPositionsCloseEnough_hook, 0x499a, 0x468a);
  if (!(F & FC)) { CYCT(0x468a, 0x468d); if (hook_enabled_at(0x4430)) { ecom_moveTowardPosition_b10_hook(gb); return; } HANDOFF(0x4430); } CYC(0x468a, 0x468d);  // jp nc,ecom_moveTowardPosition
  CYC(0x468d, 0x468e); L = E;  // ld l,e
  CYC(0x468e, 0x468f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x468f, 0x4690); L = alu_inc8(gb, L);  // inc l
  CYC(0x4690, 0x4692); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x4692, 0x4693); L = alu_inc8(gb, L);  // inc l
  CYC(0x4693, 0x4695); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x4695, 0x4697); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x4697, 0x4699); mem_wr(gb, HL, 0x96);  // ld (hl),$80|ENEMY_BEAMOS
  CYC(0x4699, 0x469b); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x469b, 0x469d); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(0x469d, 0x469f); L = 0xbb;  // ld l,Enemy.var3b
  CYC(0x469f, 0x46a1); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  RET(0x46a1); return;  // ret
}

void mergedTwinrova_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46a2, 0x46a3); E = alu_inc8(gb, E);  // inc e
  CYC(0x46a3, 0x46a4); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x46a4, 0x46a5); push_effect(gb, 0x46a5);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x46af: goto substate0;
    case 0x46d5: goto substate1;
    case 0x46fb: goto substate2;
    case 0x4714: goto substate3;
    case 0x471a: goto substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x46af, ecom_decCounter1_b10_hook, 0x439a, 0x46b2);
  if ((F & FZ)) { CYCT(0x46b2, 0x46b4); goto substate0_swapSubid; } CYC(0x46b2, 0x46b4);  // jr z,++
  CYC(0x46b4, 0x46b6); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x46b6, 0x46b7); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x46b7, 0x46b9); alu_xor(gb, 0x03);  // xor $03
  CYC(0x46b9, 0x46ba); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x46ba, 0x46bb); mem_wr(gb, HL, A);  // ld (hl),a
  RET(0x46bb); return;  // ret
substate0_swapSubid:
  CYC(0x46bc, 0x46bd); L = E;  // ld l,e
  CYC(0x46bd, 0x46be); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x46be, 0x46bf); L = alu_inc8(gb, L);  // inc l
  CYC(0x46bf, 0x46c1); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x46c1, 0x46c3); L = 0x82;  // ld l,Enemy.subid
  CYC(0x46c3, 0x46c4); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x46c4, 0x46c5); A = alu_inc8(gb, A);  // inc a
  CYC(0x46c5, 0x46c7); alu_and(gb, 0x01);  // and $01
  CYC(0x46c7, 0x46c8); B = A;  // ld b,a
  CYC(0x46c8, 0x46c9); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x46c9, 0x46cb); alu_xor(gb, 0x01);  // xor $01
  CYC(0x46cb, 0x46cc); A = alu_inc8(gb, A);  // inc a
  CYC(0x46cc, 0x46ce); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x46ce, 0x46cf); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x46cf, 0x46d0); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x46d0, 0x46d1); A = B;  // ld a,b
  CYC(0x46d1, 0x46d2); A = alu_inc8(gb, A);  // inc a
  CYC(0x46d2, 0x46d5); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
substate1:
  CALL_C(0x46d5, ecom_decCounter1_b10_hook, 0x439a, 0x46d8);
  if ((F & FZ)) { CYCT(0x46d8, 0x46da); goto substate1_swapSubid; } CYC(0x46d8, 0x46da);  // jr z,++
  CYC(0x46da, 0x46dc); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x46dc, 0x46dd); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x46dd, 0x46df); alu_xor(gb, 0x03);  // xor $03
  CYC(0x46df, 0x46e0); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x46e0, 0x46e1); mem_wr(gb, HL, A);  // ld (hl),a
  RET(0x46e1); return;  // ret
substate1_swapSubid:
  CYC(0x46e2, 0x46e3); L = E;  // ld l,e
  CYC(0x46e3, 0x46e4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x46e4, 0x46e6); L = 0x82;  // ld l,Enemy.subid
  CYC(0x46e6, 0x46e7); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x46e7, 0x46e9); alu_xor(gb, 0x01);  // xor $01
  CYC(0x46e9, 0x46ea); A = alu_inc8(gb, A);  // inc a
  CYC(0x46ea, 0x46ec); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x46ec, 0x46ed); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x46ed, 0x46ee); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x46ee, 0x46f0); A = 0x01;  // ld a,$01
  CYC(0x46f0, 0x46f3); mem_wr(gb, 0xcbca, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CALL_C(0x46f3, fastFadeoutToWhite_hook, 0x3263, 0x46f6);
  CYC(0x46f6, 0x46f8); A = 0xb9;  // ld a,SND_ENDLESS
  CYC(0x46f8, 0x46fb); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
substate2:
  CYC(0x46fb, 0x46fd); A = 0x03;  // ld a,$03
  CYC(0x46fd, 0x46fe); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(0x46fe, disableLcd_hook, 0x02c1, 0x4701);
  CYC(0x4701, 0x4703); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4703, 0x4704); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4704, 0x4705); A = alu_inc8(gb, A);  // inc a
  CYC(0x4705, 0x4708); mem_wr(gb, 0xcca9, A);  // ld (wTwinrovaTileReplacementMode),a
  CALL_C(0x4708, func_131f_hook, 0x131f, 0x470b);
  CYC(0x470b, 0x470d); A = 0x02;  // ld a,$02
  CALL_C(0x470d, loadGfxRegisterStateIndex_hook, 0x02ea, 0x4710);
  CYC(0x4710, 0x4712); A = mem_rd(gb, 0xffaf);  // ldh a,(<hActiveObject)
  CYC(0x4712, 0x4713); D = A;  // ld d,a
  RET(0x4713); return;  // ret
substate3:
  CYC(0x4714, 0x4716); A = 0x04;  // ld a,$04
  CYC(0x4716, 0x4717); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4717, 0x471a); if (hook_enabled_at(0x3284)) { fadeinFromWhiteWithDelay_hook(gb); return; } HANDOFF(0x3284);  // jp fadeinFromWhiteWithDelay
substate4:
  CYC(0x471a, 0x471d); A = mem_rd(gb, 0xc4ab);  // ld a,(wPaletteThread_mode)
  CYC(0x471d, 0x471e); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x471e); return; } CYC(0x471e, 0x471f);  // ret nz
  CYC(0x471f, 0x4720); H = D;  // ld h,d
  CYC(0x4720, 0x4722); L = 0x84;  // ld l,Enemy.state
  CYC(0x4722, 0x4723); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4723, 0x4725); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x4725, 0x4727); mem_wr(gb, HL, 0x81);  // ld (hl),$80|ENEMY_MERGED_TWINROVA
  CYC(0x4727, 0x4729); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x4729, 0x472b); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x472b, 0x472d); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x472d, 0x472f); mem_wr(gb, HL, 210);  // ld (hl),210
  CYC(0x472f, 0x4731); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x4731, 0x4732); alu_xor(gb, A);  // xor a
  CYC(0x4732, 0x4733); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4733, 0x4736); mem_wr(gb, 0xcbca, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(0x4736, 0x4738); A = 0xf1;  // ld a,SNDCTRL_STOPSFX
  CYC(0x4738, 0x473b); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void mergedTwinrova_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x473b, ecom_decCounter1_b10_hook, 0x439a, 0x473e);
  if (!(F & FZ)) { RET_TAKEN(0x473e); return; } CYC(0x473e, 0x473f);  // ret nz
  CYC(0x473f, 0x4740); L = E;  // ld l,e
  CYC(0x4740, 0x4742); mem_wr(gb, HL, 0x0c);  // ld (hl),$0c
  RET(0x4742); return;  // ret
}

void mergedTwinrova_lavaRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4743, 0x4744); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4744, 0x4746); alu_sub(gb, 0x0c);  // sub $0c
  CYC(0x4746, 0x4747); push_effect(gb, 0x4747);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x474d: mergedTwinrova_lavaRoom_stateC_hook(gb); return;
    case 0x4785: mergedTwinrova_lavaRoom_stateD_hook(gb); return;
    case 0x47c6: mergedTwinrova_lavaRoom_stateE_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void mergedTwinrova_lavaRoom_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x474d, mergedTwinrova_decVar3bIfNonzero_hook, 0x4a4f, 0x4750);
  if (!(F & FZ)) { RET_TAKEN(0x4750); return; } CYC(0x4750, 0x4751);  // ret nz
  CYC(0x4751, 0x4753); L = 0x90;  // ld l,Enemy.speed
  CYC(0x4753, 0x4755); mem_wr(gb, HL, 0x32);  // ld (hl),SPEED_140
  mergedTwinrova_chooseTargetPosition_hook(gb); return;  // fallthrough
}

void mergedTwinrova_chooseTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4755, 0x4756); L = E;  // ld l,e
  CYC(0x4756, 0x4757); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
chooseTargetPositionIndex:
  CALL_C(0x4757, getRandomNumber_hook, 0x043e, 0x475a);
  CYC(0x475a, 0x475c); alu_and(gb, 0x0e);  // and $0e
  CYC(0x475c, 0x475e); L = 0xb6;  // ld l,Enemy.var36
  CYC(0x475e, 0x475f); alu_cp(gb, mem_rd(gb, HL));  // cp (hl)
  if ((F & FZ)) { CYCT(0x475f, 0x4761); goto chooseTargetPositionIndex; } CYC(0x475f, 0x4761);  // jr z,@chooseTargetPositionIndex
  CYC(0x4761, 0x4762); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4762, 0x4765); SET_BC(0x4775);  // ld bc,@targetPositions
  CALL_C(0x4765, addAToBc_hook, 0x006d, 0x4768);
  CYC(0x4768, 0x476a); E = 0xb7;  // ld e,Enemy.var37
  CYC(0x476a, 0x476b); A = mem_rd(gb, BC);  // ld a,(bc)
  CYC(0x476b, 0x476c); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x476c, 0x476d); E = alu_inc8(gb, E);  // inc e
  CYC(0x476d, 0x476e); SET_BC(BC + 1);  // inc bc
  CYC(0x476e, 0x476f); A = mem_rd(gb, BC);  // ld a,(bc)
  CYC(0x476f, 0x4770); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4770, 0x4772); A = 0xc9;  // ld a,SND_CIRCLING
  CYC(0x4772, 0x4775); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void mergedTwinrova_lavaRoom_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4785, 0x4786); H = D;  // ld h,d
  CYC(0x4786, 0x4788); L = 0xb3;  // ld l,Enemy.var33
  CYC(0x4788, 0x4789); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4789, 0x478a); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x478a, 0x478c); goto readTarget; } CYC(0x478a, 0x478c);  // jr z,+
  CYC(0x478c, 0x478d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
readTarget:
  CYC(0x478d, 0x478f); L = 0xb7;  // ld l,Enemy.var37
  CALL_C(0x478f, ecom_readPositionVars_b10_hook, 0x4439, 0x4792);
  CALL_C(0x4792, mergedTwinrova_checkPositionsCloseEnough_hook, 0x499a, 0x4795);
  if (!(F & FC)) { CYCT(0x4795, 0x4798); if (hook_enabled_at(0x4430)) { ecom_moveTowardPosition_b10_hook(gb); return; } HANDOFF(0x4430); } CYC(0x4795, 0x4798);  // jp nc,ecom_moveTowardPosition
  CYC(0x4798, 0x479a); L = 0xb3;  // ld l,Enemy.var33
  CYC(0x479a, 0x479b); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x479b, 0x479c); alu_or(gb, A);  // or a
  CYC(0x479c, 0x479e); L = 0x84;  // ld l,Enemy.state
  if ((F & FZ)) { CYCT(0x479e, 0x47a0); goto attack; } CYC(0x479e, 0x47a0);  // jr z,@attack
  CYC(0x47a0, 0x47a1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(0x47a1, 0x47a3); L = 0xbb;  // ld l,Enemy.var3b
  CYC(0x47a3, 0x47a5); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(0x47a5); return;  // ret
attack:
  CYC(0x47a6, 0x47a7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x47a7, 0x47a8); L = alu_inc8(gb, L);  // inc l
  CYC(0x47a8, 0x47aa); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x47aa, 0x47ac); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x47ac, 0x47ae); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(0x47ae, 0x47b0); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x47b0, 0x47b1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x47b1, 0x47b2); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x47b2, 0x47b4); alu_and(gb, 0x07);  // and $07
  CYC(0x47b4, 0x47b5); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x47b5, 0x47b8); SET_HL(0x47be);  // ld hl,@var03Vals
  CYC(0x47b8, 0x47b9);
  mergedTwinrova_addAToHl_from_rst(gb, 0x47b9);  // rst_addAToHl
  CYC(0x47b9, 0x47bb); E = 0x83;  // ld e,Enemy.var03
  CYC(0x47bb, 0x47bc); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x47bc, 0x47bd); mem_wr(gb, DE, A);  // ld (de),a
  RET(0x47bd); return;  // ret
}

void mergedTwinrova_lavaRoom_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x47c6, 0x47c8); E = 0x83;  // ld e,Enemy.var03
  CYC(0x47c8, 0x47c9); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x47c9, 0x47ca); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(0x47ca, 0x47cd); goto keeseAttack; } CYC(0x47ca, 0x47cd);  // jp nz,@keeseAttack
  CYC(0x47cd, 0x47cf); E = 0x85;  // ld e,Enemy.substate
  CYC(0x47cf, 0x47d0); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x47d0, 0x47d1); push_effect(gb, 0x47d1);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x47d7: goto flameAttack_substate0;
    case 0x47f5: goto flameAttack_substate1;
    case 0x4804: goto flameAttack_substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }
flameAttack_substate0:
  CYC(0x47d7, 0x47d8); H = D;  // ld h,d
  CYC(0x47d8, 0x47d9); L = E;  // ld l,e
  CYC(0x47d9, 0x47da); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x47da, 0x47db); L = alu_inc8(gb, L);  // inc l
  CYC(0x47db, 0x47dd); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x47dd, 0x47df); A = 0x03;  // ld a,$03
  CALL_C(0x47df, enemySetAnimation_hook, 0x282b, 0x47e2);
  CALL_C(0x47e2, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x47e5);
  CYC(0x47e5, 0x47e6); B = A;  // ld b,a
  CALL_C(0x47e6, getFreePartSlot_hook, 0x3e8e, 0x47e9);
  if (!(F & FZ)) { RET_TAKEN(0x47e9); return; } CYC(0x47e9, 0x47ea);  // ret nz
  CYC(0x47ea, 0x47ec); mem_wr(gb, HL, 0x4c);  // ld (hl),PART_TWINROVA_FLAME
  CYC(0x47ec, 0x47ee); L = 0xc9;  // ld l,Part.angle
  CYC(0x47ee, 0x47ef); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x47ef, 0x47f2); SET_BC(0x1000);  // ld bc,$1000
  CYC(0x47f2, 0x47f5); if (hook_enabled_at(0x225a)) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(0x225a);  // jp objectCopyPositionWithOffset
flameAttack_substate1:
  CALL_C(0x47f5, ecom_decCounter1_b10_hook, 0x439a, 0x47f8);
  if (!(F & FZ)) { CYCT(0x47f8, 0x47fb); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x47f8, 0x47fb);  // jp nz,enemyAnimate
  CYC(0x47fb, 0x47fd); mem_wr(gb, HL, 16);  // ld (hl),16
  CYC(0x47fd, 0x47fe); L = E;  // ld l,e
  CYC(0x47fe, 0x47ff); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x47ff, 0x4801); A = 0x04;  // ld a,$04
  CYC(0x4801, 0x4804); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
flameAttack_substate2:
  CALL_C(0x4804, ecom_decCounter1_b10_hook, 0x439a, 0x4807);
  if (!(F & FZ)) { RET_TAKEN(0x4807); return; } CYC(0x4807, 0x4808);  // ret nz
doneAttack:
  CYC(0x4808, 0x480a); L = 0x84;  // ld l,Enemy.state
  CYC(0x480a, 0x480c); mem_wr(gb, HL, 0x0c);  // ld (hl),$0c
  CYC(0x480c, 0x480e); L = 0xbb;  // ld l,Enemy.var3b
  CYC(0x480e, 0x4810); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x4810, 0x4812); L = 0xb3;  // ld l,Enemy.var33
  CYC(0x4812, 0x4814); mem_wr(gb, HL, 150);  // ld (hl),150
  CYC(0x4814, 0x4816); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x4816, 0x4818); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x4818, 0x481a); A = 0x01;  // ld a,$01
  CYC(0x481a, 0x481d); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
keeseAttack:
  CYC(0x481d, 0x481f); E = 0x85;  // ld e,Enemy.substate
  CYC(0x481f, 0x4820); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4820, 0x4821); push_effect(gb, 0x4821);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x4827: goto keeseAttack_substate0;
    case 0x4835: goto keeseAttack_substate1;
    case 0x486b: goto keeseAttack_substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }
keeseAttack_substate0:
  CYC(0x4827, 0x4828); H = D;  // ld h,d
  CYC(0x4828, 0x4829); L = E;  // ld l,e
  CYC(0x4829, 0x482a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x482a, 0x482b); L = alu_inc8(gb, L);  // inc l
  CYC(0x482b, 0x482d); mem_wr(gb, HL, 0x0a);  // ld (hl),$0a
  CYC(0x482d, 0x482e); L = alu_inc8(gb, L);  // inc l
  CYC(0x482e, 0x4830); mem_wr(gb, HL, 0x05);  // ld (hl),$05
  CYC(0x4830, 0x4832); A = 0x07;  // ld a,$07
  CYC(0x4832, 0x4835); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
keeseAttack_substate1:
  CALL_C(0x4835, ecom_decCounter1_b10_hook, 0x439a, 0x4838);
  if (!(F & FZ)) { CYCT(0x4838, 0x483b); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x4838, 0x483b);  // jp nz,enemyAnimate
  CYC(0x483b, 0x483d); mem_wr(gb, HL, 20);  // ld (hl),20
  CYC(0x483d, 0x483e); L = alu_inc8(gb, L);  // inc l
  CYC(0x483e, 0x483f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if ((F & FZ)) { CYCT(0x483f, 0x4841); goto doneSpawningKeese; } CYC(0x483f, 0x4841);  // jr z,@doneSpawningKeese
  CYC(0x4841, 0x4842); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4842, 0x4843); A = alu_dec8(gb, A);  // dec a
  CYC(0x4843, 0x4846); SET_HL(0x4863);  // ld hl,@keesePositions
  CYC(0x4846, 0x4847);
  mergedTwinrova_addDoubleIndexToHl_from_rst(gb, 0x4847);  // rst_addDoubleIndexToHl
  CYC(0x4847, 0x4848); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(0x4848, 0x4849); B = A;  // ld b,a
  CYC(0x4849, 0x484a); C = mem_rd(gb, HL);  // ld c,(hl)
  CALL_C(0x484a, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x484d);
  if (!(F & FZ)) { RET_TAKEN(0x484d); return; } CYC(0x484d, 0x484e);  // ret nz
  CYC(0x484e, 0x4850); mem_wr(gb, HL, 0x5e);  // ld (hl),ENEMY_TWINROVA_BAT
  CYC(0x4850, 0x4852); L = 0x97;  // ld l,Enemy.relatedObj1+1
  CYC(0x4852, 0x4853); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(0x4853, 0x4854); L = alu_dec8(gb, L);  // dec l
  CYC(0x4854, 0x4856); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(0x4856, 0x4859); if (hook_enabled_at(0x225a)) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(0x225a);  // jp objectCopyPositionWithOffset
doneSpawningKeese:
  CYC(0x4859, 0x485a); L = alu_dec8(gb, L);  // dec l
  CYC(0x485a, 0x485c); mem_wr(gb, HL, 180);  // ld (hl),180
  CYC(0x485c, 0x485d); L = E;  // ld l,e
  CYC(0x485d, 0x485e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x485e, 0x4860); A = 0x03;  // ld a,$03
  CYC(0x4860, 0x4863); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
keeseAttack_substate2:
  CALL_C(0x486b, ecom_decCounter1_b10_hook, 0x439a, 0x486e);
  if (!(F & FZ)) { RET_TAKEN(0x486e); return; } CYC(0x486e, 0x486f);  // ret nz
  CYCT(0x486f, 0x4871); goto doneAttack;  // jr @doneAttack
}

void mergedTwinrova_iceRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4871, 0x4872); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4872, 0x4874); alu_sub(gb, 0x0c);  // sub $0c
  CYC(0x4874, 0x4875); push_effect(gb, 0x4875);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x487f: mergedTwinrova_iceRoom_stateC_hook(gb); return;
    case 0x48a6: mergedTwinrova_iceRoom_stateD_hook(gb); return;
    case 0x491a: mergedTwinrova_iceRoom_stateE_hook(gb); return;
    case 0x4925: mergedTwinrova_iceRoom_stateF_hook(gb); return;
    case 0x494f: mergedTwinrova_iceRoom_state10_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void mergedTwinrova_iceRoom_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x487f, 0x4880); H = D;  // ld h,d
  CYC(0x4880, 0x4881); L = E;  // ld l,e
  CYC(0x4881, 0x4882); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4882, 0x4883); L = alu_inc8(gb, L);  // inc l
  CYC(0x4883, 0x4885); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x4885, 0x4886); L = alu_inc8(gb, L);  // inc l
  CYC(0x4886, 0x4888); mem_wr(gb, HL, 10);  // ld (hl),10
  CYC(0x4888, 0x488a); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x488a, 0x488c); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CALL_C(0x488c, getRandomNumber_noPreserveVars_hook, 0x0453, 0x488f);
  CYC(0x488f, 0x4891); alu_and(gb, 0x01);  // and $01
  CYC(0x4891, 0x4893); E = 0xb5;  // ld e,Enemy.var35
  CYC(0x4893, 0x4894); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4894, 0x4896); E = 0xb4;  // ld e,Enemy.var34
  CYC(0x4896, 0x4897); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4897, 0x4899); alu_cp(gb, 0x02);  // cp $02
  CYC(0x4899, 0x489b); A = 0x03;  // ld a,$03
  if (!(F & FC)) { CYCT(0x489b, 0x489d); goto counter2Set; } CYC(0x489b, 0x489d);  // jr nc,+
  CYC(0x489d, 0x489e); A = alu_inc8(gb, A);  // inc a
counter2Set:
  CYC(0x489e, 0x48a0); E = 0x87;  // ld e,Enemy.counter2
  CYC(0x48a0, 0x48a1); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x48a1, 0x48a3); A = 0x05;  // ld a,$05
  CYC(0x48a3, 0x48a6); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void mergedTwinrova_iceRoom_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x48a6, 0x48a7); E = alu_inc8(gb, E);  // inc e
  CYC(0x48a7, 0x48a8); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x48a8, 0x48a9); push_effect(gb, 0x48a9);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x48ad: goto substate0;
    case 0x490c: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x48ad, ecom_decCounter1_b10_hook, 0x439a, 0x48b0);
  if (!(F & FZ)) { CYCT(0x48b0, 0x48b3); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x48b0, 0x48b3);  // jp nz,enemyAnimate
  CYC(0x48b3, 0x48b5); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x48b5, 0x48b6); L = alu_inc8(gb, L);  // inc l
  CYC(0x48b6, 0x48b7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if ((F & FZ)) { CYCT(0x48b7, 0x48b9); goto doneSpawningProjectiles; } CYC(0x48b7, 0x48b9);  // jr z,@doneSpawningProjectiles
  CYC(0x48b9, 0x48ba); B = mem_rd(gb, HL);  // ld b,(hl)
  CYC(0x48ba, 0x48bb); B = alu_dec8(gb, B);  // dec b
  CYC(0x48bb, 0x48bd); L = 0xb4;  // ld l,Enemy.var34
  CYC(0x48bd, 0x48be); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x48be, 0x48c0); alu_cp(gb, 0x02);  // cp $02
  CYC(0x48c0, 0x48c3); SET_HL(0x4904);  // ld hl,@positionData1
  if (!(F & FC)) { CYCT(0x48c3, 0x48c5); goto haveTable; } CYC(0x48c3, 0x48c5);  // jr nc,+
  CYC(0x48c5, 0x48c8); SET_HL(0x48f8);  // ld hl,@positionData0
haveTable:
  CYC(0x48c8, 0x48c9); A = B;  // ld a,b
  CYC(0x48c9, 0x48ca); alu_add(gb, A);  // add a
  CYC(0x48ca, 0x48cb);
  mergedTwinrova_addDoubleIndexToHl_from_rst(gb, 0x48cb);  // rst_addDoubleIndexToHl
  CYC(0x48cb, 0x48cc); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(0x48cc, 0x48cd); B = A;  // ld b,a
  CYC(0x48cd, 0x48ce); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(0x48ce, 0x48cf); C = A;  // ld c,a
  CYC(0x48cf, 0x48d1); E = 0xb5;  // ld e,Enemy.var35
  CYC(0x48d1, 0x48d2); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x48d2, 0x48d3); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x48d3, 0x48d5); goto readAngle; } CYC(0x48d3, 0x48d5);  // jr z,+
  CYC(0x48d5, 0x48d6); SET_HL(HL + 1);  // inc hl
readAngle:
  CYC(0x48d6, 0x48d7); E = mem_rd(gb, HL);  // ld e,(hl)
  CALL_C(0x48d7, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x48da);
  if (!(F & FZ)) { RET_TAKEN(0x48da); return; } CYC(0x48da, 0x48db);  // ret nz
  CYC(0x48db, 0x48dd); mem_wr(gb, HL, 0x5d);  // ld (hl),ENEMY_TWINROVA_ICE
  CYC(0x48dd, 0x48df); L = 0x89;  // ld l,Enemy.angle
  CYC(0x48df, 0x48e0); mem_wr(gb, HL, E);  // ld (hl),e
  CYC(0x48e0, 0x48e2); L = 0x96;  // ld l,Enemy.relatedObj1
  CYC(0x48e2, 0x48e4); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(0x48e4, 0x48e5); L = alu_inc8(gb, L);  // inc l
  CYC(0x48e5, 0x48e6); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(0x48e6, 0x48e9); if (hook_enabled_at(0x225a)) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(0x225a);  // jp objectCopyPositionWithOffset
doneSpawningProjectiles:
  CYC(0x48e9, 0x48ea); L = E;  // ld l,e
  CYC(0x48ea, 0x48eb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x48eb, 0x48ed); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x48ed, 0x48ef); mem_wr(gb, HL, 120);  // ld (hl),120
  CYC(0x48ef, 0x48f1); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x48f1, 0x48f3); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x48f3, 0x48f5); A = 0x06;  // ld a,$06
  CYC(0x48f5, 0x48f8); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
substate1:
  CALL_C(0x490c, ecom_decCounter1_b10_hook, 0x439a, 0x490f);
  if (!(F & FZ)) { RET_TAKEN(0x490f); return; } CYC(0x490f, 0x4910);  // ret nz
  CYC(0x4910, 0x4912); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(0x4912, 0x4914); L = 0x84;  // ld l,Enemy.state
  CYC(0x4914, 0x4915); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4915, 0x4917); A = 0x02;  // ld a,$02
  CYC(0x4917, 0x491a); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void mergedTwinrova_iceRoom_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x491a, mergedTwinrova_decVar3bIfNonzero_hook, 0x4a4f, 0x491d);
  if (!(F & FZ)) { RET_TAKEN(0x491d); return; } CYC(0x491d, 0x491e);  // ret nz
  CYC(0x491e, 0x4920); L = 0x90;  // ld l,Enemy.speed
  CYC(0x4920, 0x4922); mem_wr(gb, HL, 0x3c);  // ld (hl),SPEED_180
  CYC(0x4922, 0x4925); mergedTwinrova_chooseTargetPosition_hook(gb); return;  // jp mergedTwinrova_chooseTargetPosition
}

void mergedTwinrova_iceRoom_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4925, 0x4926); H = D;  // ld h,d
  CYC(0x4926, 0x4928); L = 0xb7;  // ld l,Enemy.var37
  CALL_C(0x4928, ecom_readPositionVars_b10_hook, 0x4439, 0x492b);
  CALL_C(0x492b, mergedTwinrova_checkPositionsCloseEnough_hook, 0x499a, 0x492e);
  if (!(F & FC)) { CYCT(0x492e, 0x4931); if (hook_enabled_at(0x4430)) { ecom_moveTowardPosition_b10_hook(gb); return; } HANDOFF(0x4430); } CYC(0x492e, 0x4931);  // jp nc,ecom_moveTowardPosition
  CYC(0x4931, 0x4933); L = 0x84;  // ld l,Enemy.state
  CYC(0x4933, 0x4934); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4934, 0x4935); L = alu_inc8(gb, L);  // inc l
  CYC(0x4935, 0x4937); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x4937, 0x4938); L = alu_inc8(gb, L);  // inc l
  CYC(0x4938, 0x493a); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x493a, 0x493c); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x493c, 0x493e); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(0x493e, 0x4940); A = 0x08;  // ld a,$08
  CALL_C(0x4940, enemySetAnimation_hook, 0x282b, 0x4943);
  CALL_C(0x4943, getFreePartSlot_hook, 0x3e8e, 0x4946);
  if (!(F & FZ)) { RET_TAKEN(0x4946); return; } CYC(0x4946, 0x4947);  // ret nz
  CYC(0x4947, 0x4949); mem_wr(gb, HL, 0x4e);  // ld (hl),PART_TWINROVA_SNOWBALL
  CYC(0x4949, 0x494c); SET_BC(0xe800);  // ld bc,$e800
  CYC(0x494c, 0x494f); if (hook_enabled_at(0x225a)) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(0x225a);  // jp objectCopyPositionWithOffset
}

void mergedTwinrova_iceRoom_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x494f, 0x4950); E = alu_inc8(gb, E);  // inc e
  CYC(0x4950, 0x4951); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4951, 0x4952); push_effect(gb, 0x4952);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x4956: goto substate0;
    case 0x4965: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x4956, ecom_decCounter1_b10_hook, 0x439a, 0x4959);
  if (!(F & FZ)) { CYCT(0x4959, 0x495c); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x4959, 0x495c);  // jp nz,enemyAnimate
  CYC(0x495c, 0x495e); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x495e, 0x495f); L = E;  // ld l,e
  CYC(0x495f, 0x4960); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4960, 0x4962); A = 0x06;  // ld a,$06
  CYC(0x4962, 0x4965); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
substate1:
  CALL_C(0x4965, ecom_decCounter1_b10_hook, 0x439a, 0x4968);
  if (!(F & FZ)) { RET_TAKEN(0x4968); return; } CYC(0x4968, 0x4969);  // ret nz
  CYC(0x4969, 0x496b); L = 0x84;  // ld l,Enemy.state
  CYC(0x496b, 0x496d); mem_wr(gb, HL, 0x0e);  // ld (hl),$0e
  CYC(0x496d, 0x496f); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x496f, 0x4971); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(0x4971, 0x4973); L = 0xbb;  // ld l,Enemy.var3b
  CYC(0x4973, 0x4975); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x4975, 0x4977); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x4977, 0x4979); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x4979, 0x497b); A = 0x02;  // ld a,$02
  CYC(0x497b, 0x497e); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void mergedTwinrova_checkTimeToSwapRoomFromTimer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x497e, 0x4981); A = mem_rd(gb, 0xcc00);  // ld a,(wFrameCounter)
  CYC(0x4981, 0x4983); alu_and(gb, 0x03);  // and $03
  if (!(F & FZ)) { RET_TAKEN(0x4983); return; } CYC(0x4983, 0x4984);  // ret nz
  CYC(0x4984, 0x4985); H = D;  // ld h,d
  CYC(0x4985, 0x4987); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x4987, 0x4988); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if (!(F & FZ)) { RET_TAKEN(0x4988); return; } CYC(0x4988, 0x4989);  // ret nz
  CYC(0x4989, 0x498a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x498a, 0x498c); E = 0xb9;  // ld e,Enemy.var39
  CYC(0x498c, 0x498d); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x498d, 0x498e); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x498e); return; } CYC(0x498e, 0x498f);  // ret nz
  CYC(0x498f, 0x4991); mem_wr(gb, HL, 210);  // ld (hl),210
  CYC(0x4991, 0x4993); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x4993, 0x4995); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x4995, 0x4997); L = 0x84;  // ld l,Enemy.state
  CYC(0x4997, 0x4999); mem_wr(gb, HL, 0x09);  // ld (hl),$09
  RET(0x4999); return;  // ret
}

void mergedTwinrova_checkPositionsCloseEnough_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x499a, 0x499b); alu_sub(gb, C);  // sub c
  CYC(0x499b, 0x499d); alu_add(gb, 0x02);  // add $02
  CYC(0x499d, 0x499f); alu_cp(gb, 0x05);  // cp $05
  if (!(F & FC)) { RET_TAKEN(0x499f); return; } CYC(0x499f, 0x49a0);  // ret nc
  CYC(0x49a0, 0x49a2); A = mem_rd(gb, 0xff8f);  // ldh a,(<hFF8F)
  CYC(0x49a2, 0x49a3); alu_sub(gb, B);  // sub b
  CYC(0x49a3, 0x49a5); alu_add(gb, 0x02);  // add $02
  CYC(0x49a5, 0x49a7); alu_cp(gb, 0x05);  // cp $05
  RET(0x49a7); return;  // ret
}

void mergedTwinrova_checkTimeToSwapRoomFromDamage_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x49a8, 0x49a9); H = D;  // ld h,d
  CYC(0x49a9, 0x49ab); L = 0xba;  // ld l,Enemy.var3a
  CYC(0x49ab, 0x49ac); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x49ac, 0x49ad); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(0x49ad); return; } CYC(0x49ad, 0x49ae);  // ret z
  CYC(0x49ae, 0x49af); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if ((F & FZ)) { CYCT(0x49af, 0x49b1); goto setState9; } CYC(0x49af, 0x49b1);  // jr z,+
  SET_HL(POP(0x49b1));  // pop hl
  RET(0x49b2); return;  // ret
setState9:
  CYC(0x49b3, 0x49b5); L = 0x84;  // ld l,Enemy.state
  CYC(0x49b5, 0x49b7); mem_wr(gb, HL, 0x09);  // ld (hl),$09
  CYC(0x49b7, 0x49b9); E = 0x82;  // ld e,Enemy.subid
  CYC(0x49b9, 0x49ba); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x49ba, 0x49bb); B = A;  // ld b,a
  CYC(0x49bb, 0x49bd); alu_xor(gb, 0x01);  // xor $01
  CYC(0x49bd, 0x49be); A = alu_inc8(gb, A);  // inc a
  CYC(0x49be, 0x49c0); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x49c0, 0x49c1); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x49c1, 0x49c2); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x49c2, 0x49c3); A = B;  // ld a,b
  CYC(0x49c3, 0x49c4); A = alu_inc8(gb, A);  // inc a
  CYC(0x49c4, 0x49c7); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void mergedTwinrova_deathCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x49c7, 0x49c9); E = 0x85;  // ld e,Enemy.substate
  CYC(0x49c9, 0x49ca); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x49ca, 0x49cb); push_effect(gb, 0x49cb);
  switch (mergedTwinrova_jump_table(gb)) {
    case 0x49d3: goto substate0;
    case 0x49ff: goto substate1;
    case 0x4a15: goto substate2;
    case 0x4a22: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x49d3, 0x49d6); A = mem_rd(gb, 0xcc8a);  // ld a,(wDisabledObjects)
  CYC(0x49d6, 0x49d7); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(0x49d7, 0x49d9); goto flickerAndDecCounter; } CYC(0x49d7, 0x49d9);  // jr nz,++
  CYC(0x49d9, 0x49dc); A = mem_rd(gb, 0xcdd5);  // ld a,(wLinkDeathTrigger)
  CYC(0x49dc, 0x49dd); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x49dd); return; } CYC(0x49dd, 0x49de);  // ret nz
  CYC(0x49de, 0x49df); A = alu_inc8(gb, A);  // inc a
  CYC(0x49df, 0x49e2); mem_wr(gb, 0xcbca, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(0x49e2, 0x49e5); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CALL_C(0x49e5, clearAllParentItems_hook, 0x2c10, 0x49e8);
flickerAndDecCounter:
  CALL_C(0x49e8, ecom_flickerVisibility_b10_hook, 0x441f, 0x49eb);
  CALL_C(0x49eb, ecom_decCounter1_b10_hook, 0x439a, 0x49ee);
  if ((F & FZ)) { CYCT(0x49ee, 0x49f0); goto doneExplosions; } CYC(0x49ee, 0x49f0);  // jr z,@doneExplosions
  CYC(0x49f0, 0x49f1); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x49f1, 0x49f3); alu_cp(gb, 97);  // cp 97
  if (!(F & FC)) { RET_TAKEN(0x49f3); return; } CYC(0x49f3, 0x49f4);  // ret nc
  CYC(0x49f4, 0x49f6); alu_and(gb, 0x0f);  // and $0f
  if ((F & FZ)) { CYCT(0x49f6, 0x49f9); goto createExplosion; } CYC(0x49f6, 0x49f9);  // jp z,@createExplosion
  RET(0x49f9); return;  // ret
doneExplosions:
  CYC(0x49fa, 0x49fc); mem_wr(gb, HL, 25);  // ld (hl),25
  CYC(0x49fc, 0x49fe); L = 0x85;  // ld l,Enemy.substate
  CYC(0x49fe, 0x49ff); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
substate1:
  CALL_C(0x49ff, ecom_decCounter1_b10_hook, 0x439a, 0x4a02);
  if (!(F & FZ)) { CYCT(0x4a02, 0x4a05); if (hook_enabled_at(0x441f)) { ecom_flickerVisibility_b10_hook(gb); return; } HANDOFF(0x441f); } CYC(0x4a02, 0x4a05);  // jp nz,ecom_flickerVisibility
  CYC(0x4a05, 0x4a06); L = E;  // ld l,e
  CYC(0x4a06, 0x4a07); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4a07, 0x4a09); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x4a09, 0x4a0a); alu_xor(gb, A);  // xor a
  CYC(0x4a0a, 0x4a0b); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x4a0b, 0x4a0c); mem_wr(gb, HL, A);  // ld (hl),a
  CALL_C(0x4a0c, enemySetAnimation_hook, 0x282b, 0x4a0f);
  CYC(0x4a0f, 0x4a12); SET_BC(0x2f0c);  // ld bc,TX_2f0c
  CYC(0x4a12, 0x4a15); if (hook_enabled_at(0x1872)) { showText_hook(gb); return; } HANDOFF(0x1872);  // jp showText
substate2:
  CYC(0x4a15, 0x4a17); A = 0x03;  // ld a,$03
  CYC(0x4a17, 0x4a18); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4a18, 0x4a1a); A = 0x19;  // ld a,CUTSCENE_TWINROVA_SACRIFICE
  CYC(0x4a1a, 0x4a1d); mem_wr(gb, 0xcc04, A);  // ld (wCutsceneTrigger),a
  CYC(0x4a1d, 0x4a1f); A = 0x1d;  // ld a,MUS_ROOM_OF_RITES
  CYC(0x4a1f, 0x4a22); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
substate3:
  RET(0x4a22); return;  // ret
createExplosion:
  CYC(0x4a23, 0x4a24); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4a24, 0x4a26); A = alu_swap(gb, A);  // swap a
  CYC(0x4a26, 0x4a27); A = alu_dec8(gb, A);  // dec a
  CYC(0x4a27, 0x4a2a); SET_HL(0x4a43);  // ld hl,@explosionPositions
  CYC(0x4a2a, 0x4a2b);
  mergedTwinrova_addDoubleIndexToHl_from_rst(gb, 0x4a2b);  // rst_addDoubleIndexToHl
  CYC(0x4a2b, 0x4a2d); E = 0x8b;  // ld e,Enemy.yh
  CYC(0x4a2d, 0x4a2e); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4a2e, 0x4a2f); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(0x4a2f, 0x4a30); B = A;  // ld b,a
  CYC(0x4a30, 0x4a32); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x4a32, 0x4a33); SET_HL(HL + 1);  // inc hl
  CYC(0x4a33, 0x4a34); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4a34, 0x4a35); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(0x4a35, 0x4a36); C = A;  // ld c,a
  CALL_C(0x4a36, getFreeInteractionSlot_hook, 0x3aef, 0x4a39);
  if (!(F & FZ)) { RET_TAKEN(0x4a39); return; } CYC(0x4a39, 0x4a3a);  // ret nz
  CYC(0x4a3a, 0x4a3c); mem_wr(gb, HL, 0x56);  // ld (hl),INTERAC_EXPLOSION
  CYC(0x4a3c, 0x4a3e); L = 0x4b;  // ld l,Interaction.yh
  CYC(0x4a3e, 0x4a3f); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x4a3f, 0x4a41); L = 0x4d;  // ld l,Interaction.xh
  CYC(0x4a41, 0x4a42); mem_wr(gb, HL, C);  // ld (hl),c
  RET(0x4a42); return;  // ret
}

void mergedTwinrova_decVar3bIfNonzero_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4a4f, 0x4a50); H = D;  // ld h,d
  CYC(0x4a50, 0x4a52); L = 0xbb;  // ld l,Enemy.var3b
  CYC(0x4a52, 0x4a53); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4a53, 0x4a54); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(0x4a54); return; } CYC(0x4a54, 0x4a55);  // ret z
  CYC(0x4a55, 0x4a56); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  RET(0x4a56); return;  // ret
}
