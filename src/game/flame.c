#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode12_hook(GB *gb);
void flame_state0_hook(GB *gb);

void partCode12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c33, 0x4c35); E = 0xc4; // Part.state
  CYC(0x4c35, 0x4c36); A = mem_rd(gb, DE);
  CYC(0x4c36, 0x4c37); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4c37, 0x4c3a); // call z
    push_effect(gb, 0x4c3a); flame_state0_hook(gb);
  } else {
    CYC(0x4c37, 0x4c3a);
  }

  CYC(0x4c3a, 0x4c3c); A = 0x01; // Object.id
  CALL_C(0x4c3c, objectGetRelatedObject1Var_hook, 0x2160, 0x4c3f);
  CYC(0x4c3f, 0x4c41); E = 0xf0; // Part.var30
  CYC(0x4c41, 0x4c42); A = mem_rd(gb, DE);
  CYC(0x4c42, 0x4c43); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4c43, 0x4c45); goto delete; } // jr nz
  CYC(0x4c43, 0x4c45);
  CYC(0x4c45, 0x4c47); C = 0x10;
  CALL_C(0x4c47, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4c4a);
  CYC(0x4c4a, 0x4c4c); A = 0x0f; // Object.zh
  CALL_C(0x4c4c, objectGetRelatedObject1Var_hook, 0x2160, 0x4c4f);
  CYC(0x4c4f, 0x4c51); E = 0xcf; // Part.zh
  CYC(0x4c51, 0x4c52); A = mem_rd(gb, DE);
  CYC(0x4c52, 0x4c53); mem_wr(gb, HL, A);
  CALL_C(0x4c53, objectTakePosition_hook, 0x2274, 0x4c56);
  CYC(0x4c56, 0x4c57); C = H;
  CALL_C(0x4c57, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x4c5a);
  if (!(F & FZ)) { CYCT(0x4c5a, 0x4c5d); partAnimate_hook(gb); return; } // jp nz
  CYC(0x4c5a, 0x4c5d);
  CYC(0x4c5d, 0x4c5e); H = C;
  CYC(0x4c5e, 0x4c60); L = 0xa9; // Enemy.health
  CYC(0x4c60, 0x4c62); E = 0xf1; // Part.var31
  CYC(0x4c62, 0x4c63); A = mem_rd(gb, DE);
  CYC(0x4c63, 0x4c64); mem_wr(gb, HL, A);
  CYC(0x4c64, 0x4c65); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4c65, 0x4c67); goto L_4c6b; } // jr nz
  CYC(0x4c65, 0x4c67);
  CYC(0x4c67, 0x4c69); L = 0xa4; // Enemy.collisionType
  CYC(0x4c69, 0x4c6b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));

L_4c6b:
  CYC(0x4c6b, 0x4c6d); L = 0xab; // Enemy.invincibilityCounter
  CYC(0x4c6d, 0x4c6f); mem_wr(gb, HL, 0x00);
  CYC(0x4c6f, 0x4c71); L = 0xae; // Enemy.stunCounter
  CYC(0x4c71, 0x4c73); mem_wr(gb, HL, 0x01);

delete:
  CYC(0x4c73, 0x4c76); partDelete_hook(gb); return; // jp
}

void flame_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c76, 0x4c77); H = D;
  CYC(0x4c77, 0x4c78); L = E;
  CYC(0x4c78, 0x4c79); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x4c79, 0x4c7b); L = 0xc6; // Part.counter1
  CYC(0x4c7b, 0x4c7d); mem_wr(gb, HL, 0x3b);
  CYC(0x4c7d, 0x4c7f); A = 0x01; // Object.id
  CALL_C(0x4c7f, objectGetRelatedObject1Var_hook, 0x2160, 0x4c82);
  CYC(0x4c82, 0x4c84); E = 0xf0; // Part.var30
  CYC(0x4c84, 0x4c85); A = mem_rd(gb, HL);
  CYC(0x4c85, 0x4c86); mem_wr(gb, DE, A);
  CYC(0x4c86, 0x4c88); E = 0xf1; // Part.var31
  CYC(0x4c88, 0x4c8a); L = 0xa9; // Enemy.health
  CYC(0x4c8a, 0x4c8b); A = mem_rd(gb, HL);
  CYC(0x4c8b, 0x4c8c); mem_wr(gb, DE, A);
  CYC(0x4c8c, 0x4c8e); mem_wr(gb, HL, 0x01);
  CALL_C(0x4c8e, objectTakePosition_hook, 0x2274, 0x4c91);
  CYC(0x4c91, 0x4c94); objectSetVisible80_hook(gb); return; // jp
}
