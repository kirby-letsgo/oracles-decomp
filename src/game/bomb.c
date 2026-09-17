#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t bomb_jump_table(GB *gb) {
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

// PART_BOMB
void partCode47_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x75e5, 0x75e7); E = 0xc4; // Part.state
  CYC(0x75e7, 0x75e8); A = mem_rd(gb, DE);
  {
    CYC(0x75e8, 0x75e9); push_effect(gb, 0x75e9);
    uint16_t target = bomb_jump_table(gb);
    if (target == 0x7602) goto state1;
    if (target == 0x760a) goto state2;
    if (target == 0x7631) goto state3;
    goto state0;
  }

state0:
  CYC(0x75f1, 0x75f2); H = D;
  CYC(0x75f2, 0x75f3); L = E;
  CYC(0x75f3, 0x75f4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x75f4, 0x75f6); L = 0xd0; // Part.speed
  CYC(0x75f6, 0x75f8); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x75f8, 0x75fa); L = 0xd4; // Part.speedZ
  CYC(0x75fa, 0x75fc); A = 0x80;
  CYC(0x75fc, 0x75fd); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x75fd, 0x75ff); mem_wr(gb, HL, 0xfd);
  CALL_C(0x75ff, objectSetVisiblec1_hook, 0x1e3c, 0x7602);

state1:
  CYC(0x7602, 0x7604); A = 0x00; // Object.id
  CALL_C(0x7604, objectGetRelatedObject1Var_hook, 0x2160, 0x7607);
  CYC(0x7607, 0x760a); objectTakePosition_hook(gb); return; // jp

state2:
  CALL_C(0x760a, objectApplySpeed_hook, 0x201d, 0x760d);
  CYC(0x760d, 0x760f); C = 0x20;
  CALL_C(0x760f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7612);
  if (!(F & FZ)) { CYCT(0x7612, 0x7615); partAnimate_hook(gb); return; } // jp nz
  CYC(0x7612, 0x7615);
  CYC(0x7615, 0x7617); L = 0xc4; // Part.state
  CYC(0x7617, 0x7618); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7618, 0x761a); L = 0xe4; // Part.collisionType
  CYC(0x761a, 0x761c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x761c, 0x761e); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x761e, 0x7620); A = 0x0a;
  CYC(0x7620, 0x7621); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7621, 0x7622); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7622, 0x7624); mem_wr(gb, HL, 0x0c); // oamTileIndexBase
  CYC(0x7624, 0x7626); A = 0x01;
  CALL_C(0x7626, partSetAnimation_hook, 0x2988, 0x7629);
  CYC(0x7629, 0x762b); A = 0x6f; // SND_EXPLOSION
  CALL_C(0x762b, playSound_b00_hook, 0x0c98, 0x762e);
  CYC(0x762e, 0x7631); objectSetVisible83_hook(gb); return; // jp

state3:
  CALL_C(0x7631, partAnimate_hook, 0x2978, 0x7634);
  CYC(0x7634, 0x7636); E = 0xe1; // Part.animParameter
  CYC(0x7636, 0x7637); A = mem_rd(gb, DE);
  CYC(0x7637, 0x7638); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x7638, 0x763b); partDelete_hook(gb); return; } // jp z
  CYC(0x7638, 0x763b);
  CYC(0x763b, 0x763c); A = alu_dec8(gb, A);
  CYC(0x763c, 0x763e); E = 0xe6; // Part.collisionRadiusY
  CYC(0x763e, 0x763f); mem_wr(gb, DE, A);
  CYC(0x763f, 0x7640); E = alu_inc8(gb, E);
  CYC(0x7640, 0x7641); mem_wr(gb, DE, A);
  RET(0x7641); return; // ret
}
