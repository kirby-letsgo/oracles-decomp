#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

// PART_PLASMARINE_PROJECTILE
void partCode43_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (!(F & FZ)) { CYCT(0x7443, 0x7445); goto delete; } // jr nz
  CYC(0x7443, 0x7445);
  CYC(0x7445, 0x7447); A = 0x01; // Object.id
  CALL_C(0x7447, objectGetRelatedObject1Var_hook, 0x2160, 0x744a);
  CYC(0x744a, 0x744b); A = mem_rd(gb, HL);
  CYC(0x744b, 0x744d); alu_cp(gb, 0x7e); // ENEMY_PLASMARINE
  if (!(F & FZ)) { CYCT(0x744d, 0x744f); goto delete; } // jr nz
  CYC(0x744d, 0x744f);
  CYC(0x744f, 0x7451); E = 0xc4; // Part.state
  CYC(0x7451, 0x7452); A = mem_rd(gb, DE);
  CYC(0x7452, 0x7453); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7453, 0x7455); goto state0; } // jr z
  CYC(0x7453, 0x7455);

state1:
  CYC(0x7455, 0x7457); L = 0xb2; // Enemy.var32
  CYC(0x7457, 0x7459); E = 0xc2; // Part.subid
  CYC(0x7459, 0x745a); A = mem_rd(gb, DE);
  CYC(0x745a, 0x745b); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x745b, 0x745d); goto noCollision; } // jr z
  CYC(0x745b, 0x745d);
  CALL_C(0x745d, checkObjectsCollided_hook, 0x1d5a, 0x7460);
  if (F & FC) { CYCT(0x7460, 0x7462); goto collidedWithPlasmarine; } // jr c
  CYC(0x7460, 0x7462);

noCollision:
  CYC(0x7462, 0x7465); A = mem_rd(gb, wFrameCounter);
  CYC(0x7465, 0x7466); alu_rrca(gb);
  if (F & FC) { CYCT(0x7466, 0x7468); goto updateMovement; } // jr c
  CYC(0x7466, 0x7468);
  CALL_C(0x7468, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x746b);
  if (F & FZ) { CYCT(0x746b, 0x746e); partDelete_hook(gb); return; } // jp z
  CYC(0x746b, 0x746e);
  CYC(0x746e, 0x746f); A = mem_rd(gb, HL);
  CYC(0x746f, 0x7471); alu_cp(gb, 0x1e); // 30
  if (!(F & FC)) { CYCT(0x7471, 0x7473); goto L_7479; } // jr nc
  CYC(0x7471, 0x7473);
  CYC(0x7473, 0x7475); E = 0xda; // Part.visible
  CYC(0x7475, 0x7476); A = mem_rd(gb, DE);
  CYC(0x7476, 0x7478); alu_xor(gb, 0x80);
  CYC(0x7478, 0x7479); mem_wr(gb, DE, A);

L_7479:
  CYC(0x7479, 0x747a); L = alu_inc8(gb, L);
  CYC(0x747a, 0x747b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x747b, 0x747d); goto updateMovement; } // jr nz
  CYC(0x747b, 0x747d);
  CYC(0x747d, 0x747f); mem_wr(gb, HL, 0x10);
  CALL_C(0x747f, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x7482);
  CALL_C(0x7482, objectNudgeAngleTowards_hook, 0x1fd4, 0x7485);

updateMovement:
  CALL_C(0x7485, objectApplySpeed_hook, 0x201d, 0x7488);
  CALL_C(0x7488, partCommon_checkOutOfBounds_hook, 0x407e, 0x748b);
  if (!(F & FZ)) { CYCT(0x748b, 0x748e); partAnimate_hook(gb); return; } // jp nz
  CYC(0x748b, 0x748e);
  CYC(0x748e, 0x7490); goto delete; // jr

collidedWithPlasmarine:
  CYC(0x7490, 0x7492); L = 0xab; // Enemy.invincibilityCounter
  CYC(0x7492, 0x7493); A = mem_rd(gb, HL);
  CYC(0x7493, 0x7494); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7494, 0x7496); goto noCollision; } // jr nz
  CYC(0x7494, 0x7496);
  CYC(0x7496, 0x7498); mem_wr(gb, HL, 0x18); // 24
  CYC(0x7498, 0x749a); L = 0xa9; // Enemy.health
  CYC(0x749a, 0x749b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x749b, 0x749d); goto L_74a1; } // jr nz
  CYC(0x749b, 0x749d);
  CYC(0x749d, 0x749f); L = 0xa4; // Enemy.collisionType
  CYC(0x749f, 0x74a1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

L_74a1:
  CYC(0x74a1, 0x74a3); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(0x74a3, playSound_b00_hook, 0x0c98, 0x74a6);

delete:
  CYC(0x74a6, 0x74a9); partDelete_hook(gb); return; // jp

state0:
  CYC(0x74a9, 0x74ab); L = 0xa9; // Enemy.health
  CYC(0x74ab, 0x74ac); A = mem_rd(gb, HL);
  CYC(0x74ac, 0x74ae); alu_cp(gb, 0x03);
  CYC(0x74ae, 0x74b0); A = 0x14; // SPEED_80
  if (!(F & FC)) { CYCT(0x74b0, 0x74b2); goto L_74b4; } // jr nc
  CYC(0x74b0, 0x74b2);
  CYC(0x74b2, 0x74b4); A = 0x23; // SPEED_e0

L_74b4:
  CYC(0x74b4, 0x74b5); H = D;
  CYC(0x74b5, 0x74b6); L = E;
  CYC(0x74b6, 0x74b7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x74b7, 0x74b9); L = 0xd0; // Part.speed
  CYC(0x74b9, 0x74ba); mem_wr(gb, HL, A);
  CYC(0x74ba, 0x74bc); L = 0xc6; // Part.counter1
  CYC(0x74bc, 0x74be); mem_wr(gb, HL, 0x96); // 150
  CYC(0x74be, 0x74bf); L = alu_inc8(gb, L);
  CYC(0x74bf, 0x74c1); mem_wr(gb, HL, 0x08);
  CYC(0x74c1, 0x74c3); L = 0xc2; // Part.subid
  CYC(0x74c3, 0x74c4); A = mem_rd(gb, HL);
  CYC(0x74c4, 0x74c5); A = alu_inc8(gb, A);
  CYC(0x74c5, 0x74c7); L = 0xdc; // Part.oamFlags
  CYC(0x74c7, 0x74c8); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x74c8, 0x74c9); mem_wr(gb, HL, A);
  CYC(0x74c9, 0x74ca); A = alu_dec8(gb, A);
  CALL_C(0x74ca, partSetAnimation_hook, 0x2988, 0x74cd);
  CALL_C(0x74cd, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x74d0);
  CYC(0x74d0, 0x74d2); E = 0xc9; // Part.angle
  CYC(0x74d2, 0x74d3); mem_wr(gb, DE, A);
  CYC(0x74d3, 0x74d6); objectSetVisible82_hook(gb); return; // jp
}
