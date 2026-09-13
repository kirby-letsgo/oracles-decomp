#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void label_07_027_hook(GB *gb);
void applyDamageToBothObjects_hook(GB *gb);

void collisionEffect00_hook(GB *gb) {
  CYC(0x43f3, 0x43f4); ret_effect(gb);
}

void collisionEffect08_hook(GB *gb) {
  CYC(0x443c, 0x443e); E = 0x00;
  CYC(0x443e, 0x4440); label_07_027_hook(gb);
}

void collisionEffect09_hook(GB *gb) {
  CYC(0x4440, 0x4442); E = 0x04;
  CYC(0x4442, 0x4444); label_07_027_hook(gb);
}

void collisionEffect0a_hook(GB *gb) {
  CYC(0x4444, 0x4446); E = 0x08;
  CYC(0x4446, 0x4448); label_07_027_hook(gb);
}

void label_07_027_hook(GB *gb) {
  CYC(0x4452, 0x4454); A = H8(hActiveObjectType);
  CYC(0x4454, 0x4456); alu_add(gb, 0x3e);
  CYC(0x4456, 0x4457); L = A;
  CYC(0x4457, 0x4458); H = D;
  CYC(0x4458, 0x445a); C = 0x2a;
  CYC(0x445a, 0x445b); A = mem_rd(gb, BC);
  CYC(0x445b, 0x445c); alu_or(gb, mem_rd(gb, HL));
  CYC(0x445c, 0x445d); mem_wr(gb, BC, A);
  CYC(0x445d, 0x445e); A = E;
  CYC(0x445e, 0x4461); applyDamageToEnemyOrPart(gb);
}

void collisionEffect05_hook(GB *gb) {
  CYC(0x4483, 0x4486); SET_HL(0x101c);
  CYC(0x4486, 0x4488); applyDamageToBothObjects_hook(gb);
}

void collisionEffect06_hook(GB *gb) {
  CYC(0x4488, 0x448b); SET_HL(0x141c);
  CYC(0x448b, 0x448d); applyDamageToBothObjects_hook(gb);
}

void collisionEffect07_hook(GB *gb) {
  CYC(0x448d, 0x4490); SET_HL(0x181c);
  CYC(0x4490, 0x4492); applyDamageToBothObjects_hook(gb);
}

void applyDamageToBothObjects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x44f1, 0x44f2); A = H;
  CYC(0x44f2, 0x44f3); push_effect(gb, HL);
  CALL_C(0x44f3, applyDamageToLink, 0x47e0, 0x44f6);
  CYC(0x44f6, 0x44f7); SET_HL(pop_effect(gb));
  CYC(0x44f7, 0x44f8); A = L;
  CYC(0x44f8, 0x44fb); applyDamageToEnemyOrPart(gb);
}
