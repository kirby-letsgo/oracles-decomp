#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode02_hook(GB *gb);
void enemyDestroyed_initialize_hook(GB *gb);
static void enemyDestroyed_decCounter2_hook(GB *gb);

void partCode02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4441, 0x4443); E = 0xc4; // Part.state
  CYC(0x4443, 0x4444); A = mem_rd(gb, DE);
  CYC(0x4444, 0x4445); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4445, 0x4448); // call z
    push_effect(gb, 0x4448);
    enemyDestroyed_initialize_hook(gb);
  } else {
    CYC(0x4445, 0x4448);
  }
  CALL_C(0x4448, partAnimate_hook, 0x2978, 0x444b);
  CYC(0x444b, 0x444e); A = mem_rd(gb, 0xcc00); // wFrameCounter
  CYC(0x444e, 0x444f); alu_rrca(gb);
  if (F & FC) { CYCT(0x444f, 0x4451); goto L_4457; } // jr c
  CYC(0x444f, 0x4451);
  CYC(0x4451, 0x4453); E = 0xdc; // Part.oamFlags
  CYC(0x4453, 0x4454); A = mem_rd(gb, DE);
  CYC(0x4454, 0x4456); alu_xor(gb, 0x01);
  CYC(0x4456, 0x4457); mem_wr(gb, DE, A);

L_4457:
  CYC(0x4457, 0x4459); E = 0xe1; // Part.animParameter
  CYC(0x4459, 0x445a); A = mem_rd(gb, DE);
  CYC(0x445a, 0x445b); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x445b); return; } // ret z
  CYC(0x445b, 0x445c);

  CYC(0x445c, 0x445f); push_effect(gb, 0x445f); enemyDestroyed_decCounter2_hook(gb);
  CYC(0x445f, 0x4460); A = mem_rd(gb, DE); // [counter2]
  CYC(0x4460, 0x4461); alu_rlca(gb);
  if (F & FC) { CYCT(0x4461, 0x4464); partDelete_hook(gb); return; } // jp c
  CYC(0x4461, 0x4464);

  CYC(0x4464, 0x4465); alu_xor(gb, A);
  CALL_C(0x4465, decideItemDrop_hook, 0x16eb, 0x4468);
  if (F & FZ) { CYCT(0x4468, 0x446b); partDelete_hook(gb); return; } // jp z
  CYC(0x4468, 0x446b);
  CYC(0x446b, 0x446d); B = 0x01; // PART_ITEM_DROP
  CYC(0x446d, 0x4470); objectReplaceWithID_hook(gb); return; // jp
}

void enemyDestroyed_initialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4470, 0x4471); A = alu_inc8(gb, A);
  CYC(0x4471, 0x4472); mem_wr(gb, DE, A); // [state] = 1
  CYC(0x4472, 0x4474); E = 0xed; // Part.knockbackCounter
  CYC(0x4474, 0x4475); A = mem_rd(gb, DE);
  CYC(0x4475, 0x4476); alu_rlca(gb);
  CYC(0x4476, 0x4478); A = 0x01;
  if (F & FC) {
    CALL_C_CC(0x4478, partSetAnimation_hook, 0x2988, 0x447b);
  } else {
    CYC(0x4478, 0x447b);
  }
  CYC(0x447b, 0x447e); objectSetVisible82_hook(gb); return; // jp
}

static void enemyDestroyed_decCounter2_hook(GB *gb) {
  CYC(0x447e, 0x4480); E = 0xc7; // Part.counter2
  CYC(0x4480, 0x4481); A = mem_rd(gb, DE);
  CYC(0x4481, 0x4482); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x4482); return; } // ret nc
  CYC(0x4482, 0x4483);
  CYC(0x4483, 0x4486); decNumEnemies_hook(gb); return; // jp
}
