#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void waterTektite_state_uninitialized_hook(GB *gb);
void waterTektike_decideNewAngle_hook(GB *gb);
void waterTektike_state_stub_hook(GB *gb);
void waterTektike_state8_hook(GB *gb);
void waterTektike_animate_hook(GB *gb);
void waterTektike_state9_hook(GB *gb);
void waterTektite_getAdjacentWallsBitset_hook(GB *gb);
void waterTektite_getAdjacentWallsBitsetGivenAngle_hook(GB *gb);
void waterTektike_setSpeedFromCounter1_hook(GB *gb);

static uint16_t waterTektite_jump_table(GB *gb) {
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

static void waterTektite_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_WATER_TEKTITE
// ==================================================================================================
void enemyCode3a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5476, 0x5478); goto normalStatus; } // jr z
  CYC(0x5476, 0x5478);
  CYC(0x5478, 0x547a); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x547a); return; } // ret c
  CYC(0x547a, 0x547b);
  if (F & FZ) { CYCT(0x547b, 0x547e); enemyDie_hook(gb); return; } // jp z
  CYC(0x547b, 0x547e);
  CYC(0x547e, 0x547f); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x547f); return; } // ret z
  CYC(0x547f, 0x5480);

  // ENEMYSTATUS_KNOCKBACK
  // Need special knockback code for special "solidity" properties (water is
  // traversible, everything else is solid)
  CYC(0x5480, 0x5482); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5482, 0x5483); A = mem_rd(gb, DE);
  CYC(0x5483, 0x5484); push_effect(gb, AF); // push af
  CYC(0x5484, 0x5486); A = 0x50; // SPEED_200
  CYC(0x5486, 0x5487); mem_wr(gb, DE, A);
  CYC(0x5487, 0x5489); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(0x5489, waterTektite_getAdjacentWallsBitsetGivenAngle_hook, 0x550a, 0x548c);
  CYC(0x548c, 0x548e); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(0x548e, ecom_applyVelocityGivenAdjacentWalls_b0e_hook, 0x415b, 0x5491);

  CYC(0x5491, 0x5492); SET_AF(pop_effect(gb)); // pop af
  CYC(0x5492, 0x5494); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5494, 0x5495); mem_wr(gb, DE, A);
  RET(0x5495); return; // ret

normalStatus:
  CYC(0x5496, 0x5498); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5498, 0x5499); A = mem_rd(gb, DE);
  {
    CYC(0x5499, 0x549a); push_effect(gb, 0x549a);
    uint16_t target = waterTektite_jump_table(gb);
    if (target == 0x54ae) { waterTektite_state_uninitialized_hook(gb); return; }
    if (target == 0x54e2) { waterTektike_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x54e3) { waterTektike_state8_hook(gb); return; }
    if (target == 0x5501) { waterTektike_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:54ae, bare global; jump-table target from enemyCode3a. Falls into
// waterTektike_decideNewAngle.
void waterTektite_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x54ae, objectSetVisible82_hook, 0x1e69, 0x54b1);
  waterTektike_decideNewAngle_hook(gb); return; // fallthrough
}

// 0e:54b1, bare global; falls into from waterTektite_state_uninitialized, also reached by
// genuine jr from waterTektike_state9.
void waterTektike_decideNewAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54b1, 0x54b2); H = D;
  CYC(0x54b2, 0x54b4); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x54b4, 0x54b6); mem_wr(gb, HL, 0x08);
  CYC(0x54b6, 0x54b8); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x54b8, 0x54ba); mem_wr(gb, HL, 0x40);
  CYC(0x54ba, 0x54bd); A = mem_rd(gb, wScentSeedActive);
  CYC(0x54bd, 0x54be); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x54be, 0x54c0); goto scentSeedActive; } // jr nz
  CYC(0x54be, 0x54c0);

  // Random diagonal angle
  CALL_C(0x54c0, getRandomNumber_noPreserveVars_hook, 0x0453, 0x54c3);
  CYC(0x54c3, 0x54c5); alu_and(gb, 0x18);
  CYC(0x54c5, 0x54c7); alu_add(gb, 0x04);
  CYC(0x54c7, 0x54c9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x54c9, 0x54ca); mem_wr(gb, DE, A);
  CYC(0x54ca, 0x54cc); waterTektike_animate_hook(gb); return; // jr

scentSeedActive:
  CYC(0x54cc, 0x54ce); A = hram_rd(gb, 0xb2); // hFFB2
  CYC(0x54ce, 0x54d0); hram_wr(gb, 0x8f, A); // hFF8F
  CYC(0x54d0, 0x54d2); A = hram_rd(gb, 0xb3); // hFFB3
  CYC(0x54d2, 0x54d4); hram_wr(gb, 0x8e, A); // hFF8E
  CYC(0x54d4, 0x54d6); L = ENEMY_BASE + OBJ_YH;
  CYC(0x54d6, 0x54d7); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x54d7, 0x54d8); B = A;
  CYC(0x54d8, 0x54d9); L = alu_inc8(gb, L);
  CYC(0x54d9, 0x54da); C = mem_rd(gb, HL);
  CALL_C(0x54da, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x54dd);
  CYC(0x54dd, 0x54df); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x54df, 0x54e0); mem_wr(gb, DE, A);
  CYC(0x54e0, 0x54e2); waterTektike_animate_hook(gb); return; // jr
}

// 0e:54e2, bare global; jump-table target from enemyCode3a.
void waterTektike_state_stub_hook(GB *gb) {
  RET(0x54e2); return; // ret
}

// 0e:54e3, bare global; jump-table target from enemyCode3a. Moving in some direction for
// [counter1] frames, at varying speeds.
void waterTektike_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x54e3, ecom_decCounter1_b0e_hook, 0x439a, 0x54e6);
  if (!(F & FZ)) { CYCT(0x54e6, 0x54e8); goto keepMoving; } // jr nz
  CYC(0x54e6, 0x54e8);
  CYC(0x54e8, 0x54e9); L = E;
  CYC(0x54e9, 0x54ea); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x54ea, 0x54ec); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x54ec, 0x54ee); mem_wr(gb, HL, 0x08);
  CYC(0x54ee, 0x54f0); waterTektike_animate_hook(gb); return; // jr

keepMoving:
  CALL_C(0x54f0, waterTektike_setSpeedFromCounter1_hook, 0x5542, 0x54f3);
  CALL_C(0x54f3, waterTektite_getAdjacentWallsBitset_hook, 0x5508, 0x54f6);
  CYC(0x54f6, 0x54f8); E = ENEMY_BASE + OBJ_ANGLE;
  CALL_C(0x54f8, ecom_applyVelocityGivenAdjacentWalls_b0e_hook, 0x415b, 0x54fb);
  CALL_C(0x54fb, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x54fe);
  waterTektike_animate_hook(gb); return; // fallthrough
}

// 0e:54fe, bare global; falls into from waterTektike_state8, also reached by genuine jr
// from waterTektite_state_uninitialized (via waterTektike_decideNewAngle) and
// waterTektike_state9.
void waterTektike_animate_hook(GB *gb) {
  CYC(0x54fe, 0x5501); enemyAnimate_hook(gb); return; // jp
}

// 0e:5501, bare global; jump-table target from enemyCode3a. Not moving for [counter1]
// frames; then choosing new angle.
void waterTektike_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5501, ecom_decCounter1_b0e_hook, 0x439a, 0x5504);
  if (!(F & FZ)) { CYCT(0x5504, 0x5506); waterTektike_animate_hook(gb); return; } // jr nz
  CYC(0x5504, 0x5506);
  CYC(0x5506, 0x5508); waterTektike_decideNewAngle_hook(gb); return; // jr
}

// 0e:5508, bare global; called from waterTektike_state8. Gets the "adjacent walls bitset"
// for the tektike; since this swims, water is traversable, everything else is not. This is
// identical to "fish_getAdjacentWallsBitsetForKnockback".
void waterTektite_getAdjacentWallsBitset_hook(GB *gb) {
  CYC(0x5508, 0x550a); E = ENEMY_BASE + OBJ_ANGLE;
  waterTektite_getAdjacentWallsBitsetGivenAngle_hook(gb); return; // fallthrough
}

// 0e:550a, bare global; falls into from waterTektite_getAdjacentWallsBitset, also reached
// by genuine call from enemyCode3a.
// @param de Angle variable
void waterTektite_getAdjacentWallsBitsetGivenAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x550a, 0x550b); A = mem_rd(gb, DE);
  CALL_C(0x550b, ecom_getAdjacentWallTableOffset_b0e_hook, 0x4253, 0x550e);
  CYC(0x550e, 0x550f); H = D;
  CYC(0x550f, 0x5511); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5511, 0x5512); B = mem_rd(gb, HL);
  CYC(0x5512, 0x5514); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5514, 0x5515); C = mem_rd(gb, HL);
  CYC(0x5515, 0x5518); SET_HL(0x425e); // ecom_sideviewAdjacentWallOffsetTable (bank 0e)
  waterTektite_addAToHl_from_rst(gb, 0x5519);

  CYC(0x5519, 0x551b); A = 0x10;
  CYC(0x551b, 0x551d); hram_wr(gb, 0x8b, A); // hFF8B
  CYC(0x551d, 0x551f); D = 0xcf; // >wRoomLayout

nextOffset:
  CYC(0x551f, 0x5520); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5520, 0x5521); alu_add(gb, B);
  CYC(0x5521, 0x5522); B = A;
  CYC(0x5522, 0x5524); alu_and(gb, 0xf0);
  CYC(0x5524, 0x5525); E = A;
  CYC(0x5525, 0x5526); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5526, 0x5527); alu_add(gb, C);
  CYC(0x5527, 0x5528); C = A;
  CYC(0x5528, 0x552a); alu_and(gb, 0xf0);
  CYC(0x552a, 0x552c); A = alu_swap(gb, A);
  CYC(0x552c, 0x552d); alu_or(gb, E);
  CYC(0x552d, 0x552e); E = A;
  CYC(0x552e, 0x552f); A = mem_rd(gb, DE);
  CYC(0x552f, 0x5531); alu_sub(gb, 0xf9); // TILEINDEX_PUDDLE
  CYC(0x5531, 0x5533); alu_cp(gb, 0x05); // TILEINDEX_FD-TILEINDEX_PUDDLE+1
  CYC(0x5533, 0x5535); A = hram_rd(gb, 0x8b); // hFF8B
  CYC(0x5535, 0x5536); alu_rla(gb);
  CYC(0x5536, 0x5538); hram_wr(gb, 0x8b, A); // hFF8B
  if (!(F & FC)) { CYCT(0x5538, 0x553a); goto nextOffset; } // jr nc
  CYC(0x5538, 0x553a);

  CYC(0x553a, 0x553c); alu_xor(gb, 0x0f);
  CYC(0x553c, 0x553e); hram_wr(gb, 0x8b, A); // hFF8B
  CYC(0x553e, 0x5540); A = hram_rd(gb, 0xaf); // hActiveObject
  CYC(0x5540, 0x5541); D = A;
  RET(0x5541); return; // ret
}

// 0e:5542, bare global; called from waterTektike_state8.
// @param hl Pointer to counter1
void waterTektike_setSpeedFromCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5542, 0x5543); A = mem_rd(gb, HL);
  CYC(0x5543, 0x5545); A = alu_srl(gb, A);
  CYC(0x5545, 0x5547); A = alu_srl(gb, A);
  CYC(0x5547, 0x554a); SET_HL(0x5550); // @speedVals
  waterTektite_addAToHl_from_rst(gb, 0x554b);
  CYC(0x554b, 0x554d); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x554d, 0x554e); A = mem_rd(gb, HL);
  CYC(0x554e, 0x554f); mem_wr(gb, DE, A);
  RET(0x554f); return; // ret
}
