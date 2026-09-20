#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x5a86, 0x5a88); goto normalStatus; } // jr z
  CYC(0x5a86, 0x5a88);
  CYC(0x5a88, 0x5a8a); E = 0xea; // Part.var2a
  CYC(0x5a8a, 0x5a8b); A = mem_rd(gb, DE);
  CYC(0x5a8b, 0x5a8d); alu_cp(gb, 0x83);
  if (F & FZ) { CYCT(0x5a8d, 0x5a8f); goto destroy; } // jr z
  CYC(0x5a8d, 0x5a8f);
  CYC(0x5a8f, 0x5a91); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x5a91, 0x5a93); alu_sub(gb, 0x05);
  CYC(0x5a93, 0x5a95); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(0x5a95, 0x5a98); goto destroy; } // jp c
  CYC(0x5a95, 0x5a98);

normalStatus:
  CYC(0x5a98, 0x5a9a); E = 0xc4; // Part.state
  CYC(0x5a9a, 0x5a9b); A = mem_rd(gb, DE);
  {
    CYC(0x5a9b, 0x5a9c); push_effect(gb, 0x5a9c);
    uint16_t target = twinrovaSnowball_jump_table(gb);
    if (target == 0x5aa2) goto state0;
    if (target == 0x5ab5) goto state1;
    goto state2;
  }

state0:
  CYC(0x5aa2, 0x5aa3); H = D;
  CYC(0x5aa3, 0x5aa4); L = E;
  CYC(0x5aa4, 0x5aa5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5aa5, 0x5aa7); L = 0xc6; // Part.counter1
  CYC(0x5aa7, 0x5aa9); mem_wr(gb, HL, 0x1e);
  CYC(0x5aa9, 0x5aab); L = 0xd0; // Part.speed
  CYC(0x5aab, 0x5aad); mem_wr(gb, HL, 0x5a);
  CYC(0x5aad, 0x5aaf); A = 0x8d; // SND_TELEPORT
  CALL_C(0x5aaf, playSound_b00_hook, 0x0c98, 0x5ab2);
  CYC(0x5ab2, 0x5ab5); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x5ab5, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5ab8);
  if (F & FZ) { CYCT(0x5ab8, 0x5aba); goto beginMoving; } // jr z
  CYC(0x5ab8, 0x5aba);
  CYC(0x5aba, 0x5abc); L = 0xe1; // Part.animParameter
  CYC(0x5abc, 0x5abe); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5abe, 0x5ac0); goto animate; } // jr z
  CYC(0x5abe, 0x5ac0);
  CYC(0x5ac0, 0x5ac2); mem_wr(gb, HL, 0x00);
  CYC(0x5ac2, 0x5ac4); L = 0xe4; // Part.collisionType
  CYC(0x5ac4, 0x5ac6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));

animate:
  CYC(0x5ac6, 0x5ac9); partAnimate_hook(gb); return; // jp

beginMoving:
  CYC(0x5ac9, 0x5aca); L = E;
  CYC(0x5aca, 0x5acb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x5acb, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5ace);
  CYC(0x5ace, 0x5ad0); E = 0xc9; // Part.angle
  CYC(0x5ad0, 0x5ad1); mem_wr(gb, DE, A);

state2:
  CALL_C(0x5ad1, objectApplySpeed_hook, 0x201d, 0x5ad4);
  CALL_C(0x5ad4, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x5ad7);
  if (!(F & FC)) { RET_TAKEN(0x5ad7); return; } // ret nc
  CYC(0x5ad7, 0x5ad8);

destroy:
  CYC(0x5ad8, 0x5ada); B = 0x09; // INTERAC_SNOWDEBRIS
  CALL_C(0x5ada, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x5add);
  CYC(0x5add, 0x5ae0); partDelete_hook(gb); return; // jp
}
