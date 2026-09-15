#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t veranProjectile_jump_table(GB *gb) {
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

void veranProjectile_subid0_hook(GB *gb);
void veranProjectile_subid1_hook(GB *gb);

// PART_VERAN_PROJECTILE
void partCode37_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(0x69ed, 0x69f0); partDelete_hook(gb); return; } // jp nz
  CYC(0x69ed, 0x69f0);
  CYC(0x69f0, 0x69f2); E = 0xc2; // Part.subid
  CYC(0x69f2, 0x69f3); A = mem_rd(gb, DE);
  CYC(0x69f3, 0x69f4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x69f4, 0x69f7); veranProjectile_subid1_hook(gb); return; } // jp nz
  CYC(0x69f4, 0x69f7);
  veranProjectile_subid0_hook(gb);
}

// The "core" projectile spawner
void veranProjectile_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x69f7, 0x69f9); A = 0x24; // Object.collisionType
  CALL_C(0x69f9, objectGetRelatedObject1Var_hook, 0x2160, 0x69fc);
  CYC(0x69fc, 0x69fe); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x69fe, 0x6a00); goto delete; } // jr z
  CYC(0x69fe, 0x6a00);
  CYC(0x6a00, 0x6a02); E = 0xc4; // Part.state
  CYC(0x6a02, 0x6a03); A = mem_rd(gb, DE);
  {
    CYC(0x6a03, 0x6a04); push_effect(gb, 0x6a04);
    uint16_t target = veranProjectile_jump_table(gb);
    if (target == 0x6a14) goto state1;
    if (target == 0x6a25) goto state2;
    goto state0;
  }

state0:
  CYC(0x6a0a, 0x6a0b); H = D;
  CYC(0x6a0b, 0x6a0c); L = E;
  CYC(0x6a0c, 0x6a0d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6a0d, 0x6a0f); L = 0xcf; // Part.zh
  CYC(0x6a0f, 0x6a11); mem_wr(gb, HL, 0xfc);
  CYC(0x6a11, 0x6a14); objectSetVisible81_hook(gb); return; // jp

state1:
  CYC(0x6a14, 0x6a15); H = D;
  CYC(0x6a15, 0x6a17); L = 0xcf; // Part.zh
  CYC(0x6a17, 0x6a18); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6a18, 0x6a19); A = mem_rd(gb, HL);
  CYC(0x6a19, 0x6a1b); alu_cp(gb, 0xf0);
  if (!(F & FZ)) { CYCT(0x6a1b, 0x6a1d); goto animate; } // jr nz
  CYC(0x6a1b, 0x6a1d);
  CYC(0x6a1d, 0x6a1e); L = E;
  CYC(0x6a1e, 0x6a1f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6a1f, 0x6a21); L = 0xc6; // Part.counter1
  CYC(0x6a21, 0x6a23); mem_wr(gb, HL, 0x81);
  CYC(0x6a23, 0x6a25); goto animate; // jr

state2:
  CALL_C(0x6a25, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6a28);
  if (F & FZ) { CYCT(0x6a28, 0x6a2a); goto delete; } // jr z
  CYC(0x6a28, 0x6a2a);
  CYC(0x6a2a, 0x6a2b); A = mem_rd(gb, HL);
  CYC(0x6a2b, 0x6a2d); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x6a2d, 0x6a2f); goto animate; } // jr nz
  CYC(0x6a2d, 0x6a2f);
  CYC(0x6a2f, 0x6a30); A = mem_rd(gb, HL);
  CYC(0x6a30, 0x6a31); alu_rrca(gb);
  CYC(0x6a31, 0x6a32); alu_rrca(gb);
  CYC(0x6a32, 0x6a34); alu_and(gb, 0x1f);
  CYC(0x6a34, 0x6a35); B = A;
  CALL_C(0x6a35, getFreePartSlot_hook, 0x3e8e, 0x6a38);
  if (!(F & FZ)) { CYCT(0x6a38, 0x6a3a); goto animate; } // jr nz
  CYC(0x6a38, 0x6a3a);
  CYC(0x6a3a, 0x6a3c); mem_wr(gb, HL, 0x37); // PART_VERAN_PROJECTILE
  CYC(0x6a3c, 0x6a3d); L = alu_inc8(gb, L);
  CYC(0x6a3d, 0x6a3e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // subid = 1
  CYC(0x6a3e, 0x6a40); L = 0xc9; // Part.angle
  CYC(0x6a40, 0x6a41); mem_wr(gb, HL, B);
  CALL_C(0x6a41, objectCopyPosition_hook, 0x2242, 0x6a44);

animate:
  CYC(0x6a44, 0x6a47); partAnimate_hook(gb); return; // jp

delete:
  CYC(0x6a47, 0x6a4a); SET_BC(0x0580);
  CALL_C(0x6a4a, objectCreateInteraction_hook, 0x24c5, 0x6a4d);
  CYC(0x6a4d, 0x6a50); partDelete_hook(gb); return; // jp
}

// An individual projectile
void veranProjectile_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a50, 0x6a52); E = 0xc4; // Part.state
  CYC(0x6a52, 0x6a53); A = mem_rd(gb, DE);
  {
    CYC(0x6a53, 0x6a54); push_effect(gb, 0x6a54);
    uint16_t target = veranProjectile_jump_table(gb);
    if (target == 0x6a74) goto state1;
    if (target == 0x6a7c) goto state2;
    goto state0;
  }

state0:
  CYC(0x6a5a, 0x6a5b); H = D;
  CYC(0x6a5b, 0x6a5c); L = E;
  CYC(0x6a5c, 0x6a5d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6a5d, 0x6a5f); L = 0xd0; // Part.speed
  CYC(0x6a5f, 0x6a61); mem_wr(gb, HL, 0x64); // SPEED_280
  CYC(0x6a61, 0x6a63); L = 0xe6; // Part.collisionRadiusY
  CYC(0x6a63, 0x6a65); A = 0x04;
  CYC(0x6a65, 0x6a66); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6a66, 0x6a67); mem_wr(gb, HL, A);
  CALL_C(0x6a67, objectSetVisible81_hook, 0x1e60, 0x6a6a);
  CYC(0x6a6a, 0x6a6c); A = 0xbe; // SND_VERAN_PROJECTILE
  CALL_C(0x6a6c, playSound_b00_hook, 0x0c98, 0x6a6f);
  CYC(0x6a6f, 0x6a71); A = 0x01;
  CYC(0x6a71, 0x6a74); partSetAnimation_hook(gb); return; // jp

state1:
  CYC(0x6a74, 0x6a75); H = D;
  CYC(0x6a75, 0x6a77); L = 0xcf; // Part.zh
  CYC(0x6a77, 0x6a78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x6a78, 0x6a7a); goto state2; } // jr nz
  CYC(0x6a78, 0x6a7a);
  CYC(0x6a7a, 0x6a7b); L = E;
  CYC(0x6a7b, 0x6a7c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state2:
  CALL_C(0x6a7c, objectApplySpeed_hook, 0x201d, 0x6a7f);
  CALL_C(0x6a7f, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x6a82);
  if (!(F & FZ)) { RET_TAKEN(0x6a82); return; } // ret nz
  CYC(0x6a82, 0x6a83);
  CYC(0x6a83, 0x6a86); partDelete_hook(gb); return; // jp
}
