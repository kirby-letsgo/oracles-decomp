#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t smogProjectile_jump_table(GB *gb) {
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

// PART_SMOG_PROJECTILE
void partCode4a_hook(GB *gb) {
  BASE(partCode4a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = smogProjectile_jump_table(gb);
    if (target == b_+60) goto state1;
    if (target == b_+111) goto state2;
    goto state0;
  }

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A); // Part.state
  CALL_C(b_+13, objectSetVisible81_hook, SYM(objectSetVisible81), b_+16);
  CALL_C(b_+16, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+19);
  CYC(b_+19, b_+21); E = 0xc9; // Part.angle
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); C = A;
  CYC(b_+23, b_+25); A = 0x1e; // SPEED_c0
  CYC(b_+25, b_+27); E = 0xd0; // Part.speed
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); E = 0xc2; // Part.subid
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+32, b_+34); goto setAnimation; } // jr z
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); A = 0x28; // SPEED_100
  CYC(b_+36, b_+38); E = 0xd0; // Part.speed
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+41); A = 0x05;
  CYC(b_+41, b_+43); E = 0xdc; // Part.oamFlags
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+46); E = 0xe5; // Part.enemyCollisionMode
  CYC(b_+46, b_+48); A = 0x04; // ENEMYCOLLISION_PODOBOO
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); A = C;
  CALL_C(b_+50, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+53);
  CYC(b_+53, b_+55); alu_and(gb, 0x01);
  CYC(b_+55, b_+57); alu_add(gb, 0x02);

setAnimation:
  CALL_C(b_+57, partSetAnimation_hook, SYM(partSetAnimation), b_+60);

state1:
  CALL_C(b_+60, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+63);
  CYC(b_+63, b_+65); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto delete; } // jr nz
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+70); A = mem_rd(gb, wNumEnemies);
  CYC(b_+70, b_+71); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+71, b_+73); goto delete; } // jr z
  CYC(b_+71, b_+73);
  CALL_C(b_+73, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+76);
  if (!(F & FC)) { CYCT(b_+76, b_+78); goto delete; } // jr nc
  CYC(b_+76, b_+78);
  CALL_C(b_+78, objectApplySpeed_hook, SYM(objectApplySpeed), b_+81);
  CYC(b_+81, b_+83); E = 0xc2; // Part.subid
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+85); return; } // ret nz
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); E = 0xea; // Part.var2a
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+90, b_+92); goto beginDestroyAnimation; } // jr nz
  CYC(b_+90, b_+92);
  CALL_C(b_+92, partCommon_getTileCollisionInFront_hook, SYM(partCommon_getTileCollisionInFront), b_+95);
  if (F & FZ) { CYCT(b_+95, b_+97); goto state2; } // jr z
  CYC(b_+95, b_+97);

beginDestroyAnimation:
  CYC(b_+97, b_+98); H = D;
  CYC(b_+98, b_+100); L = 0xe4; // Part.collisionType
  CYC(b_+100, b_+102); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+102, b_+104); A = 0x02;
  CYC(b_+104, b_+106); L = 0xc4; // Part.state
  CYC(b_+106, b_+107); mem_wr(gb, HL, A);
  CYC(b_+107, b_+108); A = alu_dec8(gb, A);
  CALL_C(b_+108, partSetAnimation_hook, SYM(partSetAnimation), b_+111);

state2:
  CALL_C(b_+111, partAnimate_hook, SYM(partAnimate), b_+114);
  CYC(b_+114, b_+116); E = 0xe1; // Part.animParameter
  CYC(b_+116, b_+117); A = mem_rd(gb, DE);
  CYC(b_+117, b_+118); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+118); return; } // ret z
  CYC(b_+118, b_+119);

delete:
  CYC(b_+119, b_+122); partDelete_hook(gb); return; // jp
}
