#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode37), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode37), (from), (to), true)

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
  BASE(partCode37);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); partDelete_hook(gb); return; } // jp nz
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); E = 0xc2; // Part.subid
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, SYM(veranProjectile_subid0)); veranProjectile_subid1_hook(gb); return; } // jp nz
  CYC(b_+7, SYM(veranProjectile_subid0));
  veranProjectile_subid0_hook(gb);
}

// The "core" projectile spawner
void veranProjectile_subid0_hook(GB *gb) {
  BASE(veranProjectile_subid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x24; // Object.collisionType
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+7, b_+9); goto delete; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); E = 0xc4; // Part.state
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  {
    CYC(b_+12, b_+13); push_effect(gb, b_+13);
    uint16_t target = veranProjectile_jump_table(gb);
    if (target == b_+29) goto state1;
    if (target == b_+46) goto state2;
    goto state0;
  }

state0:
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+21); L = E;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+24); L = 0xcf; // Part.zh
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0xfc);
  CYC(b_+26, b_+29); objectSetVisible81_hook(gb); return; // jp

state1:
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = 0xcf; // Part.zh
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_cp(gb, 0xf0);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto animate; } // jr nz
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); L = E;
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+40, b_+42); L = 0xc6; // Part.counter1
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x81);
  CYC(b_+44, b_+46); goto animate; // jr

state2:
  CALL_C(b_+46, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+49);
  if (F & FZ) { CYCT(b_+49, b_+51); goto delete; } // jr z
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+54); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto animate; } // jr nz
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+58); alu_rrca(gb);
  CYC(b_+58, b_+59); alu_rrca(gb);
  CYC(b_+59, b_+61); alu_and(gb, 0x1f);
  CYC(b_+61, b_+62); B = A;
  CALL_C(b_+62, getFreePartSlot_hook, SYM(getFreePartSlot), b_+65);
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto animate; } // jr nz
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x37); // PART_VERAN_PROJECTILE
  CYC(b_+69, b_+70); L = alu_inc8(gb, L);
  CYC(b_+70, b_+71); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // subid = 1
  CYC(b_+71, b_+73); L = 0xc9; // Part.angle
  CYC(b_+73, b_+74); mem_wr(gb, HL, B);
  CALL_C(b_+74, objectCopyPosition_hook, SYM(objectCopyPosition), b_+77);

animate:
  CYC(b_+77, b_+80); partAnimate_hook(gb); return; // jp

delete:
  CYC(b_+80, b_+83); SET_BC((SYM(loadPaletteHeader) + 117));
  CALL_C(b_+83, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+86);
  CYC(b_+86, SYM(veranProjectile_subid1)); partDelete_hook(gb); return; // jp
}

// An individual projectile
void veranProjectile_subid1_hook(GB *gb) {
  BASE(veranProjectile_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = veranProjectile_jump_table(gb);
    if (target == b_+36) goto state1;
    if (target == b_+44) goto state2;
    goto state0;
  }

state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = 0xd0; // Part.speed
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x64); // SPEED_280
  CYC(b_+17, b_+19); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+19, b_+21); A = 0x04;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CALL_C(b_+23, objectSetVisible81_hook, SYM(objectSetVisible81), b_+26);
  CYC(b_+26, b_+28); A = 0xbe; // SND_VERAN_PROJECTILE
  CALL_C(b_+28, playSound_b00_hook, SYM(playSound_b00), b_+31);
  CYC(b_+31, b_+33); A = 0x01;
  CYC(b_+33, b_+36); partSetAnimation_hook(gb); return; // jp

state1:
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); L = 0xcf; // Part.zh
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto state2; } // jr nz
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); L = E;
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state2:
  CALL_C(b_+44, objectApplySpeed_hook, SYM(objectApplySpeed), b_+47);
  CALL_C(b_+47, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+50);
  if (!(F & FZ)) { RET_TAKEN(b_+50); return; } // ret nz
  CYC(b_+50, b_+51);
  CYC(b_+51, SYM(partCode38)); partDelete_hook(gb); return; // jp
}
