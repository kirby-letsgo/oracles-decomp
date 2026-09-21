#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode02_hook(GB *gb);
void enemyDestroyed_initialize_hook(GB *gb);
static void enemyDestroyed_decCounter2_hook(GB *gb);

void partCode02_hook(GB *gb) {
  BASE(partCode02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+7); // call z
    push_effect(gb, b_+7);
    enemyDestroyed_initialize_hook(gb);
  } else {
    CYC(b_+4, b_+7);
  }
  CALL_C(b_+7, partAnimate_hook, SYM(partAnimate), b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wFrameCounter); // wFrameCounter
  CYC(b_+13, b_+14); alu_rrca(gb);
  if (F & FC) { CYCT(b_+14, b_+16); goto L_4457; } // jr c
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); E = 0xdc; // Part.oamFlags
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_xor(gb, 0x01);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);

L_4457:
  CYC(b_+22, b_+24); E = 0xe1; // Part.animParameter
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+26); return; } // ret z
  CYC(b_+26, b_+27);

  CYC(b_+27, b_+30); push_effect(gb, b_+30); enemyDestroyed_decCounter2_hook(gb);
  CYC(b_+30, b_+31); A = mem_rd(gb, DE); // [counter2]
  CYC(b_+31, b_+32); alu_rlca(gb);
  if (F & FC) { CYCT(b_+32, b_+35); partDelete_hook(gb); return; } // jp c
  CYC(b_+32, b_+35);

  CYC(b_+35, b_+36); alu_xor(gb, A);
  CALL_C(b_+36, decideItemDrop_hook, SYM(decideItemDrop), b_+39);
  if (F & FZ) { CYCT(b_+39, b_+42); partDelete_hook(gb); return; } // jp z
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); B = 0x01; // PART_ITEM_DROP
  CYC(b_+44, b_+47); TAIL(objectReplaceWithID); // jp
}

void enemyDestroyed_initialize_hook(GB *gb) {
  BASE(partCode02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+47, b_+48); A = alu_inc8(gb, A);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A); // [state] = 1
  CYC(b_+49, b_+51); E = 0xed; // Part.knockbackCounter
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); alu_rlca(gb);
  CYC(b_+53, b_+55); A = 0x01;
  if (F & FC) {
    CALL_C_CC(b_+55, partSetAnimation_hook, SYM(partSetAnimation), b_+58);
  } else {
    CYC(b_+55, b_+58);
  }
  CYC(b_+58, b_+61); TAIL(objectSetVisible82); // jp
}

static void enemyDestroyed_decCounter2_hook(GB *gb) {
  BASE(partCode02);
  CYC(b_+61, b_+63); E = 0xc7; // Part.counter2
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+65); return; } // ret nc
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+69); TAIL(decNumEnemies); // jp
}
