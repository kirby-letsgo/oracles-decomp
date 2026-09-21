#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// PART_PLASMARINE_PROJECTILE
void partCode43_hook(GB *gb) {
  BASE(partCode43);
  uint16_t sp0_ = gb->sp;
  if (!(F & FZ)) { CYCT(b_+0, b_+2); goto delete; } // jr nz
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); A = 0x01; // Object.id
  CALL_C(b_+4, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0x7e); // ENEMY_PLASMARINE
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto delete; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); E = 0xc4; // Part.state
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+16, b_+18); goto state0; } // jr z
  CYC(b_+16, b_+18);

state1:
  CYC(b_+18, b_+20); L = 0xb2; // Enemy.var32
  CYC(b_+20, b_+22); E = 0xc2; // Part.subid
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+24, b_+26); goto noCollision; } // jr z
  CYC(b_+24, b_+26);
  CALL_C(b_+26, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+29);
  if (F & FC) { CYCT(b_+29, b_+31); goto collidedWithPlasmarine; } // jr c
  CYC(b_+29, b_+31);

noCollision:
  CYC(b_+31, b_+34); A = mem_rd(gb, wFrameCounter);
  CYC(b_+34, b_+35); alu_rrca(gb);
  if (F & FC) { CYCT(b_+35, b_+37); goto updateMovement; } // jr c
  CYC(b_+35, b_+37);
  CALL_C(b_+37, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+40);
  if (F & FZ) { CYCT(b_+40, b_+43); partDelete_hook(gb); return; } // jp z
  CYC(b_+40, b_+43);
  CYC(b_+43, b_+44); A = mem_rd(gb, HL);
  CYC(b_+44, b_+46); alu_cp(gb, 0x1e); // 30
  if (!(F & FC)) { CYCT(b_+46, b_+48); goto L_7479; } // jr nc
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); E = 0xda; // Part.visible
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+53); alu_xor(gb, 0x80);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);

L_7479:
  CYC(b_+54, b_+55); L = alu_inc8(gb, L);
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto updateMovement; } // jr nz
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x10);
  CALL_C(b_+60, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+63);
  CALL_C(b_+63, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+66);

updateMovement:
  CALL_C(b_+66, objectApplySpeed_hook, SYM(objectApplySpeed), b_+69);
  CALL_C(b_+69, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+72);
  if (!(F & FZ)) { CYCT(b_+72, b_+75); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+72, b_+75);
  CYC(b_+75, b_+77); goto delete; // jr

collidedWithPlasmarine:
  CYC(b_+77, b_+79); L = 0xab; // Enemy.invincibilityCounter
  CYC(b_+79, b_+80); A = mem_rd(gb, HL);
  CYC(b_+80, b_+81); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+81, b_+83); goto noCollision; } // jr nz
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x18); // 24
  CYC(b_+85, b_+87); L = 0xa9; // Enemy.health
  CYC(b_+87, b_+88); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+88, b_+90); goto L_74a1; } // jr nz
  CYC(b_+88, b_+90);
  CYC(b_+90, b_+92); L = 0xa4; // Enemy.collisionType
  CYC(b_+92, b_+94); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

L_74a1:
  CYC(b_+94, b_+96); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(b_+96, playSound_b00_hook, SYM(playSound_b00), b_+99);

delete:
  CYC(b_+99, b_+102); TAIL(partDelete); // jp

state0:
  CYC(b_+102, b_+104); L = 0xa9; // Enemy.health
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+107); alu_cp(gb, 0x03);
  CYC(b_+107, b_+109); A = 0x14; // SPEED_80
  if (!(F & FC)) { CYCT(b_+109, b_+111); goto L_74b4; } // jr nc
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+113); A = 0x23; // SPEED_e0

L_74b4:
  CYC(b_+113, b_+114); H = D;
  CYC(b_+114, b_+115); L = E;
  CYC(b_+115, b_+116); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+116, b_+118); L = 0xd0; // Part.speed
  CYC(b_+118, b_+119); mem_wr(gb, HL, A);
  CYC(b_+119, b_+121); L = 0xc6; // Part.counter1
  CYC(b_+121, b_+123); mem_wr(gb, HL, 0x96); // 150
  CYC(b_+123, b_+124); L = alu_inc8(gb, L);
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0x08);
  CYC(b_+126, b_+128); L = 0xc2; // Part.subid
  CYC(b_+128, b_+129); A = mem_rd(gb, HL);
  CYC(b_+129, b_+130); A = alu_inc8(gb, A);
  CYC(b_+130, b_+132); L = 0xdc; // Part.oamFlags
  CYC(b_+132, b_+133); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+133, b_+134); mem_wr(gb, HL, A);
  CYC(b_+134, b_+135); A = alu_dec8(gb, A);
  CALL_C(b_+135, partSetAnimation_hook, SYM(partSetAnimation), b_+138);
  CALL_C(b_+138, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+141);
  CYC(b_+141, b_+143); E = 0xc9; // Part.angle
  CYC(b_+143, b_+144); mem_wr(gb, DE, A);
  CYC(b_+144, b_+147); TAIL(objectSetVisible82); // jp
}
