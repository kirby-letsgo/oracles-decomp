#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode4e), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode4e), (from), (to), true)

static uint16_t twinrovaSnowball_jump_table(GB *gb) {
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

void partCode4e_hook(GB *gb) {
  BASE(partCode4e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x83);
  if (F & FZ) { CYCT(b_+7, b_+9); goto destroy; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+11, b_+13); alu_sub(gb, 0x05);
  CYC(b_+13, b_+15); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+15, b_+18); goto destroy; } // jp c
  CYC(b_+15, b_+18);

normalStatus:
  CYC(b_+18, b_+20); E = 0xc4; // Part.state
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  {
    CYC(b_+21, b_+22); push_effect(gb, b_+22);
    uint16_t target = twinrovaSnowball_jump_table(gb);
    if (target == b_+28) goto state0;
    if (target == b_+47) goto state1;
    goto state2;
  }

state0:
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+30); L = E;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+31, b_+33); L = 0xc6; // Part.counter1
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x1e);
  CYC(b_+35, b_+37); L = 0xd0; // Part.speed
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x5a);
  CYC(b_+39, b_+41); A = 0x8d; // SND_TELEPORT
  CALL_C(b_+41, playSound_b00_hook, SYM(playSound_b00), b_+44);
  CYC(b_+44, b_+47); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(b_+47, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+50);
  if (F & FZ) { CYCT(b_+50, b_+52); goto beginMoving; } // jr z
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); L = 0xe1; // Part.animParameter
  CYC(b_+54, b_+56); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+56, b_+58); goto animate; } // jr z
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x00);
  CYC(b_+60, b_+62); L = 0xe4; // Part.collisionType
  CYC(b_+62, b_+64); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));

animate:
  CYC(b_+64, b_+67); partAnimate_hook(gb); return; // jp

beginMoving:
  CYC(b_+67, b_+68); L = E;
  CYC(b_+68, b_+69); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+69, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+72);
  CYC(b_+72, b_+74); E = 0xc9; // Part.angle
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);

state2:
  CALL_C(b_+75, objectApplySpeed_hook, SYM(objectApplySpeed), b_+78);
  CALL_C(b_+78, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+81);
  if (!(F & FC)) { RET_TAKEN(b_+81); return; } // ret nc
  CYC(b_+81, b_+82);

destroy:
  CYC(b_+82, b_+84); B = 0x09; // INTERAC_SNOWDEBRIS
  CALL_C(b_+84, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+87);
  CYC(b_+87, SYM(partCode50)); partDelete_hook(gb); return; // jp
}
