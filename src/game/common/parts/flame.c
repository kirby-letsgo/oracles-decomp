#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode12_hook(GB *gb);
void flame_state0_hook(GB *gb);

void partCode12_hook(GB *gb) {
  BASE(partCode12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+7); // call z
    push_effect(gb, b_+7); flame_state0_hook(gb);
  } else {
    CYC(b_+4, b_+7);
  }

  CYC(b_+7, b_+9); A = 0x01; // Object.id
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+14); E = 0xf0; // Part.var30
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto delete; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); C = 0x10;
  CALL_C(b_+20, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+23);
  CYC(b_+23, b_+25); A = 0x0f; // Object.zh
  CALL_C(b_+25, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+28);
  CYC(b_+28, b_+30); E = 0xcf; // Part.zh
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CALL_C(b_+32, objectTakePosition_hook, SYM(objectTakePosition), b_+35);
  CYC(b_+35, b_+36); C = H;
  CALL_C(b_+36, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+39);
  if (!(F & FZ)) { CYCT(b_+39, b_+42); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+43); H = C;
  CYC(b_+43, b_+45); L = 0xa9; // Enemy.health
  CYC(b_+45, b_+47); E = 0xf1; // Part.var31
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); mem_wr(gb, HL, A);
  CYC(b_+49, b_+50); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto L_4c6b; } // jr nz
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); L = 0xa4; // Enemy.collisionType
  CYC(b_+54, b_+56); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));

L_4c6b:
  CYC(b_+56, b_+58); L = 0xab; // Enemy.invincibilityCounter
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x00);
  CYC(b_+60, b_+62); L = 0xae; // Enemy.stunCounter
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x01);

delete:
  CYC(b_+64, b_+67); TAIL(partDelete); // jp
}

void flame_state0_hook(GB *gb) {
  BASE(partCode12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+67, b_+68); H = D;
  CYC(b_+68, b_+69); L = E;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+70, b_+72); L = 0xc6; // Part.counter1
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x3b);
  CYC(b_+74, b_+76); A = 0x01; // Object.id
  CALL_C(b_+76, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+79);
  CYC(b_+79, b_+81); E = 0xf0; // Part.var30
  CYC(b_+81, b_+82); A = mem_rd(gb, HL);
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CYC(b_+83, b_+85); E = 0xf1; // Part.var31
  CYC(b_+85, b_+87); L = 0xa9; // Enemy.health
  CYC(b_+87, b_+88); A = mem_rd(gb, HL);
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x01);
  CALL_C(b_+91, objectTakePosition_hook, SYM(objectTakePosition), b_+94);
  CYC(b_+94, b_+97); TAIL(objectSetVisible80); // jp
}
