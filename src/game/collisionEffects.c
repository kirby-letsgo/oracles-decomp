#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void label_07_027_hook(GB *gb);
void applyDamageToBothObjects_hook(GB *gb);
void collisionEffect0c_hook(GB *gb);
void collisionEffect0d_hook(GB *gb);
void collisionEffect0e_hook(GB *gb);
void collisionEffect0f_hook(GB *gb);
void collisionEffect10_hook(GB *gb);
void collisionEffect11_hook(GB *gb);

static void collisionEffectDamageLinkTail_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x4434, applyDamageToLink_paramE, 0x47df, 0x4437);
  CYC(0x4437, 0x4439); A = 0x1c;
  CYC(0x4439, 0x443c); applyDamageToEnemyOrPart(gb);
}

void collisionEffect00_hook(GB *gb) {
  CYC(0x43f3, 0x43f4); ret_effect(gb);
}

void collisionEffect01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4426, 0x4428); E = 0x00;
  CYC(0x4428, 0x442a); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x442a, 0x442c); E = 0x04;
  CYC(0x442c, 0x442e); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x442e, 0x4430); E = 0x08;
  CYC(0x4430, 0x4432); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4432, 0x4434); E = 0x0c;
  collisionEffectDamageLinkTail_hook(gb, sp0_);
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

void collisionEffect12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4461, createClinkInteraction, 0x46e7, 0x4464);
  collisionEffect0c_hook(gb);
}

void collisionEffect0c_hook(GB *gb) {
  CYC(0x4464, 0x4466); E = 0x10;
  CYC(0x4466, 0x4468); label_07_028(gb);
}

void collisionEffect13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4468, createClinkInteraction, 0x46e7, 0x446b);
  collisionEffect0d_hook(gb);
}

void collisionEffect0d_hook(GB *gb) {
  CYC(0x446b, 0x446d); E = 0x14;
  CYC(0x446d, 0x446f); label_07_028(gb);
}

void collisionEffect14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x446f, createClinkInteraction, 0x46e7, 0x4472);
  collisionEffect0e_hook(gb);
}

void collisionEffect0e_hook(GB *gb) {
  CYC(0x4472, 0x4474); E = 0x18;
  label_07_028(gb);
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

void collisionEffect18_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4492, createClinkInteraction, 0x46e7, 0x4495);
  collisionEffect0f_hook(gb);
}

void collisionEffect0f_hook(GB *gb) {
  CYC(0x4495, 0x4498); SET_HL(0x1010);
  CYC(0x4498, 0x449a); applyDamageToBothObjects_hook(gb);
}

void collisionEffect19_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x449a, createClinkInteraction, 0x46e7, 0x449d);
  collisionEffect10_hook(gb);
}

void collisionEffect10_hook(GB *gb) {
  CYC(0x449d, 0x44a0); SET_HL(0x1414);
  CYC(0x44a0, 0x44a2); applyDamageToBothObjects_hook(gb);
}

void collisionEffect15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44a2, createClinkInteraction, 0x46e7, 0x44a5);
  CYC(0x44a5, 0x44a8); SET_HL(0x1034);
  CYC(0x44a8, 0x44aa); applyDamageToBothObjects_hook(gb);
}

void collisionEffect16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44aa, createClinkInteraction, 0x46e7, 0x44ad);
  CYC(0x44ad, 0x44b0); SET_HL(0x1434);
  CYC(0x44b0, 0x44b2); applyDamageToBothObjects_hook(gb);
}

void collisionEffect17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44b2, createClinkInteraction, 0x46e7, 0x44b5);
  CYC(0x44b5, 0x44b8); SET_HL(0x1834);
  CYC(0x44b8, 0x44ba); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44ba, createClinkInteraction, 0x46e7, 0x44bd);
  collisionEffect11_hook(gb);
}

void collisionEffect11_hook(GB *gb) {
  CYC(0x44bd, 0x44c0); SET_HL(0x1818);
  CYC(0x44c0, 0x44c2); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44c2, createClinkInteraction, 0x46e7, 0x44c5);
  CYC(0x44c5, 0x44c8); SET_HL(0x1c28);
  CYC(0x44c8, 0x44ca); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1d_hook(GB *gb) {
  CYC(0x44ca, 0x44cd); SET_HL(0x0c04);
  CYC(0x44cd, 0x44cf); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1e_hook(GB *gb) {
  CYC(0x44cf, 0x44d2); SET_HL(0x2834);
  CYC(0x44d2, 0x44d4); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1f_hook(GB *gb) {
  CYC(0x44d4, 0x44d7); SET_HL(0x2034);
  CYC(0x44d7, 0x44d9); applyDamageToBothObjects_hook(gb);
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
