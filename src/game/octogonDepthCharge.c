#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void octogonDepthCharge_subid1_hook(GB *gb);
void octogonDepthCharge_subid0_hook(GB *gb);

static uint16_t octogonDepthCharge_jump_table(GB *gb) {
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

static void octogonDepthCharge_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_OCTOGON_DEPTH_CHARGE
void partCode48_hook(GB *gb) {
  if (F & FZ) { CYCT(0x7642, 0x7644); goto normalStatus; } // jr z
  CYC(0x7642, 0x7644);
  CYC(0x7644, 0x7646); E = 0xc2; // Part.subid
  CYC(0x7646, 0x7647); A = mem_rd(gb, DE);
  CYC(0x7647, 0x7648); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7648, 0x764b); partDelete_hook(gb); return; } // jp nz
  CYC(0x7648, 0x764b);

normalStatus:
  CYC(0x764b, 0x764d); E = 0xc2; // Part.subid
  CYC(0x764d, 0x764e); A = mem_rd(gb, DE);
  CYC(0x764e, 0x764f); alu_or(gb, A);
  CYC(0x764f, 0x7651); E = 0xc4; // Part.state
  if (F & FZ) { CYCT(0x7651, 0x7653); octogonDepthCharge_subid0_hook(gb); return; } // jr z
  CYC(0x7651, 0x7653);
  octogonDepthCharge_subid1_hook(gb);
}

// Small (split) projectile
void octogonDepthCharge_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7653, 0x7654); A = mem_rd(gb, DE);
  CYC(0x7654, 0x7655); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7655, 0x7657); goto state0; } // jr z
  CYC(0x7655, 0x7657);

state1:
  CALL_C(0x7657, objectApplySpeed_hook, 0x201d, 0x765a);
  CALL_C(0x765a, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x765d);
  if (!(F & FZ)) { CYCT(0x765d, 0x7660); partAnimate_hook(gb); return; } // jp nz
  CYC(0x765d, 0x7660);
  CYC(0x7660, 0x7663); partDelete_hook(gb); return; // jp

state0:
  CYC(0x7663, 0x7664); H = D;
  CYC(0x7664, 0x7665); L = E;
  CYC(0x7665, 0x7666); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7666, 0x7668); L = 0xe6; // Part.collisionRadiusY
  CYC(0x7668, 0x766a); A = 0x02;
  CYC(0x766a, 0x766b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x766b, 0x766c); mem_wr(gb, HL, A);
  CYC(0x766c, 0x766e); L = 0xd0; // Part.speed
  CYC(0x766e, 0x7670); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x7670, 0x7672); A = 0x01;
  CALL_C(0x7672, partSetAnimation_hook, 0x2988, 0x7675);
  CYC(0x7675, 0x7678); objectSetVisible82_hook(gb); return; // jp
}

// Large projectile, before being split into 4 smaller ones (subid 1)
void octogonDepthCharge_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7678, 0x7679); A = mem_rd(gb, DE);
  {
    CYC(0x7679, 0x767a); push_effect(gb, 0x767a);
    uint16_t target = octogonDepthCharge_jump_table(gb);
    if (target == 0x76cd) goto state1;
    if (target == 0x76fa) goto state2;
    if (target == 0x7707) goto state3;
    goto state0;
  }

state0:
  CYC(0x7682, 0x7684); A = 0x1a; // Object.visible
  CALL_C(0x7684, objectGetRelatedObject1Var_hook, 0x2160, 0x7687);
  CYC(0x7687, 0x7688); A = mem_rd(gb, HL);
  CYC(0x7688, 0x7689); H = D;
  CYC(0x7689, 0x768b); L = 0xc4; // Part.state
  CYC(0x768b, 0x768c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x768c, 0x768d); alu_rlca(gb);
  if (F & FC) { CYCT(0x768d, 0x768f); goto aboveWater; } // jr c
  CYC(0x768d, 0x768f);
  CYC(0x768f, 0x7690); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7690, 0x7692); L = 0xc6; // Part.counter1
  CYC(0x7692, 0x7693); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7693, 0x7695); L = 0xcf; // Part.zh
  CYC(0x7695, 0x7697); mem_wr(gb, HL, 0xb8);
  CYC(0x7697, 0x7699); L = 0xf0; // Part.var30
  CYC(0x7699, 0x769b); mem_wr(gb, HL, 0x10);
  CALL_C(0x769b, getRandomNumber_noPreserveVars_hook, 0x0453, 0x769e);
  CYC(0x769e, 0x76a0); alu_and(gb, 0x06);
  CYC(0x76a0, 0x76a3); SET_HL(0x76b3); // table_76b3 (positionCandidates)
  CYC(0x76a3, 0x76a4); octogonDepthCharge_addAToHl_from_rst(gb, 0x76a4);
  CYC(0x76a4, 0x76a6); E = 0xcb; // Part.yh
  CYC(0x76a6, 0x76a7); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x76a7, 0x76a8); mem_wr(gb, DE, A);
  CYC(0x76a8, 0x76aa); E = 0xcd; // Part.xh
  CYC(0x76aa, 0x76ab); A = mem_rd(gb, HL);
  CYC(0x76ab, 0x76ac); mem_wr(gb, DE, A);
  CYC(0x76ac, 0x76ae); A = 0x87; // SND_SPLASH
  CALL_C(0x76ae, playSound_b00_hook, 0x0c98, 0x76b1);
  CYC(0x76b1, 0x76b3); goto setVisible81; // jr

aboveWater:
  CYC(0x76bb, 0x76bd); L = 0xf0; // Part.var30
  CYC(0x76bd, 0x76bf); mem_wr(gb, HL, 0x20);
  CYC(0x76bf, 0x76c1); L = 0xcb; // Part.yh
  CYC(0x76c1, 0x76c2); A = mem_rd(gb, HL);
  CYC(0x76c2, 0x76c4); alu_sub(gb, 0x10);
  CYC(0x76c4, 0x76c5); mem_wr(gb, HL, A);
  CYC(0x76c5, 0x76c7); A = 0x85; // SND_SCENT_SEED
  CALL_C(0x76c7, playSound_b00_hook, 0x0c98, 0x76ca);

setVisible81:
  CYC(0x76ca, 0x76cd); objectSetVisible81_hook(gb); return; // jp

state1:
  CYC(0x76cd, 0x76ce); H = D;
  CYC(0x76ce, 0x76d0); L = 0xcf; // Part.zh
  CYC(0x76d0, 0x76d1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x76d1, 0x76d2); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x76d2, 0x76d3); A = mem_rd(gb, HL);
  CYC(0x76d3, 0x76d5); alu_cp(gb, 0xd0);
  if (!(F & FC)) { CYCT(0x76d5, 0x76d7); goto animate; } // jr nc
  CYC(0x76d5, 0x76d7);
  CYC(0x76d7, 0x76d9); alu_cp(gb, 0xb8);
  if (!(F & FC)) { CYCT(0x76d9, 0x76db); goto flickerVisibility; } // jr nc
  CYC(0x76d9, 0x76db);
  CYC(0x76db, 0x76dc); L = E;
  CYC(0x76dc, 0x76dd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x76dd, 0x76df); L = 0xc6; // Part.counter1
  CYC(0x76df, 0x76e1); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x76e1, 0x76e3); L = 0xe4; // Part.collisionType
  CYC(0x76e3, 0x76e5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x76e5, 0x76e7); L = 0xcb; // Part.yh
  CYC(0x76e7, 0x76e9); A = mem_rd(gb, 0xffb0); // hEnemyTargetY
  CYC(0x76e9, 0x76ea); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x76ea, 0x76eb); L = alu_inc8(gb, L);
  CYC(0x76eb, 0x76ed); A = mem_rd(gb, 0xffb1); // hEnemyTargetX
  CYC(0x76ed, 0x76ee); mem_wr(gb, HL, A);
  CYC(0x76ee, 0x76f1); objectSetInvisible_hook(gb); return; // jp

flickerVisibility:
  CYC(0x76f1, 0x76f3); L = 0xda; // Part.visible
  CYC(0x76f3, 0x76f4); A = mem_rd(gb, HL);
  CYC(0x76f4, 0x76f6); alu_xor(gb, 0x80);
  CYC(0x76f6, 0x76f7); mem_wr(gb, HL, A);

animate:
  CYC(0x76f7, 0x76fa); partAnimate_hook(gb); return; // jp

state2:
  CALL_C(0x76fa, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x76fd);
  if (!(F & FZ)) { RET_TAKEN(0x76fd); return; } // ret nz
  CYC(0x76fd, 0x76fe);
  CYC(0x76fe, 0x76ff); L = E;
  CYC(0x76ff, 0x7700); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7700, 0x7702); L = 0xe4; // Part.collisionType
  CYC(0x7702, 0x7704); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x7704, 0x7707); objectSetVisiblec1_hook(gb); return; // jp

state3:
  CYC(0x7707, 0x7709); E = 0xf0; // Part.var30
  CYC(0x7709, 0x770a); A = mem_rd(gb, DE);
  CALL_C(0x770a, objectUpdateSpeedZ_hook, 0x1f45, 0x770d);
  if (!(F & FZ)) { CYCT(0x770d, 0x770f); goto animate; } // jr nz
  CYC(0x770d, 0x770f);
  CALL_C(0x770f, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7712);
  CYC(0x7712, 0x7714); alu_and(gb, 0x04);
  CYC(0x7714, 0x7715); B = A;
  CYC(0x7715, 0x7717); C = 0x04;

spawnNext:
  CALL_C(0x7717, getFreePartSlot_hook, 0x3e8e, 0x771a);
  if (!(F & FZ)) { CYCT(0x771a, 0x771c); goto L_772a; } // jr nz
  CYC(0x771a, 0x771c);
  CYC(0x771c, 0x771e); mem_wr(gb, HL, 0x48); // PART_OCTOGON_DEPTH_CHARGE
  CYC(0x771e, 0x771f); L = alu_inc8(gb, L);
  CYC(0x771f, 0x7720); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // subid = 1
  CYC(0x7720, 0x7722); L = 0xc9; // Part.angle
  CYC(0x7722, 0x7723); mem_wr(gb, HL, B);
  CALL_C(0x7723, objectCopyPosition_hook, 0x2242, 0x7726);
  CYC(0x7726, 0x7727); A = B;
  CYC(0x7727, 0x7729); alu_add(gb, 0x08);
  CYC(0x7729, 0x772a); B = A;

L_772a:
  CYC(0x772a, 0x772b); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x772b, 0x772d); goto spawnNext; } // jr nz
  CYC(0x772b, 0x772d);
  CYC(0x772d, 0x772f); A = 0x68; // SND_UNKNOWN3
  CALL_C(0x772f, playSound_b00_hook, 0x0c98, 0x7732);
  CYC(0x7732, 0x7735); partDelete_hook(gb); return; // jp
}
