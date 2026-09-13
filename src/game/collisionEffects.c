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
void collisionEffect3d_hook(GB *gb);
void label_07_033_hook(GB *gb);
void killEnemyOrPart_hook(GB *gb);

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

void collisionEffect20_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x44d9, 0x44da); H = B;
  CYC(0x44da, 0x44dc); L = 0x01;
  CYC(0x44dc, 0x44dd); A = mem_rd(gb, HL);
  CYC(0x44dd, 0x44df); alu_cp(gb, 0x28);
  if (!(F & FC)) {
    CYCT(0x44df, 0x44e1);
  } else {
    CYC(0x44df, 0x44e1);
    CYC(0x44e1, 0x44e3); L = 0x24;
    CYC(0x44e3, 0x44e5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  }
  CALL_C(0x44e5, func_07_47b7, 0x47b7, 0x44e8);
  if (F & FZ) { CYCT(0x44e8, 0x44e9); ret_effect(gb); return; }
  CYC(0x44e8, 0x44e9);
  CYC(0x44e9, 0x44ec); SET_HL(0x2444);
  CYC(0x44ec, 0x44ee); applyDamageToBothObjects_hook(gb);
}

void collisionEffect22_hook(GB *gb) {
  CYC(0x44ee, 0x44f1); SET_HL(0x1c24);
  applyDamageToBothObjects_hook(gb);
}

void collisionEffect26_hook(GB *gb) {
  CYC(0x44fb, 0x44fe); SET_HL(0x1c34);
  CYC(0x44fe, 0x4500); applyDamageToBothObjects_hook(gb);
}

void collisionEffect27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4500, 0x4501); H = B;
  CYC(0x4501, 0x4503); L = 0x24;
  CYC(0x4503, 0x4505); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(0x4505, func_07_47b7, 0x47b7, 0x4508);
  if (F & FZ) { CYCT(0x4508, 0x4509); ret_effect(gb); return; }
  CYC(0x4508, 0x4509);
  CALL_C(0x4509, createFlamePart, 0x46da, 0x450c);
  CYC(0x450c, 0x450f); SET_HL(0x1c2c);
  CYC(0x450f, 0x4511); applyDamageToBothObjects_hook(gb);
}

void collisionEffect28_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4511, 0x4512); H = B;
  CYC(0x4512, 0x4514); L = 0x24;
  CYC(0x4514, 0x4516); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(0x4516, func_07_47b7, 0x47b7, 0x4519);
  if (F & FZ) { CYCT(0x4519, 0x451a); ret_effect(gb); return; }
  CYC(0x4519, 0x451a);
  CYC(0x451a, 0x451d); SET_HL(0x1c38);
  CYC(0x451d, 0x451f); applyDamageToBothObjects_hook(gb);
}

void collisionEffect3a_hook(GB *gb) {
  CYC(0x451f, 0x4521); E = 0xad;
  CYC(0x4521, 0x4522); A = mem_rd(gb, DE);
  CYC(0x4522, 0x4523); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4523, 0x4524); ret_effect(gb); return; }
  CYC(0x4523, 0x4524);
  collisionEffect3d_hook(gb);
}

void collisionEffect3d_hook(GB *gb) {
  CYC(0x4524, 0x4527); A = mem_rd(gb, w1Link_id);
  CYC(0x4527, 0x4528); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4528, 0x4529); ret_effect(gb); return; }
  CYC(0x4528, 0x4529);
  CYC(0x4529, 0x452c); A = mem_rd(gb, wWarpsDisabled);
  CYC(0x452c, 0x452d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x452d, 0x452e); ret_effect(gb); return; }
  CYC(0x452d, 0x452e);
  CYC(0x452e, 0x4530); A = 0x0d;
  CYC(0x4530, 0x4533); mem_wr(gb, wLinkForceState, A);
  CYC(0x4533, 0x4536); SET_HL(0x2c1c);
  CYC(0x4536, 0x4538); applyDamageToBothObjects_hook(gb);
}

void collisionEffect2b_hook(GB *gb) {
  CYC(0x4538, 0x453b); SET_HL(0x1c3c);
  CYC(0x453b, 0x453d); applyDamageToBothObjects_hook(gb);
}

void collisionEffect2c_hook(GB *gb) {
  CYC(0x453d, 0x4540); SET_HL(0x1430);
  CYC(0x4540, 0x4542); applyDamageToBothObjects_hook(gb);
}

void collisionEffect2f_hook(GB *gb) {
  CYC(0x4542, 0x4545); SET_HL(0x3004);
  CYC(0x4545, 0x4547); applyDamageToBothObjects_hook(gb);
}

void collisionEffect30_hook(GB *gb) {
  CYC(0x4547, 0x454a); SET_HL(0x1c44);
  CYC(0x454a, 0x454c); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1c_hook(GB *gb) {
  CYC(0x454c, 0x454f); SET_HL(0x1c1c);
  CYC(0x454f, 0x4551); applyDamageToBothObjects_hook(gb);
}

void collisionEffect23_hook(GB *gb) {
  CYC(0x4584, 0x4586); A = H8(hActiveObjectType);
  CYC(0x4586, 0x4588); alu_add(gb, 0x29);
  CYC(0x4588, 0x4589); L = A;
  CYC(0x4589, 0x458a); H = D;
  CYC(0x458a, 0x458c); mem_wr(gb, HL, 0x00);
  CYC(0x458c, 0x458d); ret_effect(gb);
}

void collisionEffect24_hook(GB *gb) {
  CYC(0x458d, 0x458f); A = H8(hActiveObjectType);
  CYC(0x458f, 0x4591); alu_add(gb, 0x2a);
  CYC(0x4591, 0x4592); E = A;
  CYC(0x4592, 0x4594); A = H8(hFF90);
  CYC(0x4594, 0x4596); alu_or(gb, 0x80);
  CYC(0x4596, 0x4597); mem_wr(gb, DE, A);
  CYC(0x4597, 0x4598); A = E;
  CYC(0x4598, 0x459a); alu_add(gb, 0xec);
  CYC(0x459a, 0x459b); L = A;
  CYC(0x459b, 0x459c); H = D;
  CYC(0x459c, 0x459d); mem_wr(gb, HL, C);
  CYC(0x459d, 0x459e); L = alu_inc8(gb, L);
  CYC(0x459e, 0x459f); mem_wr(gb, HL, B);
  CYC(0x459f, 0x45a1); C = 0x2a;
  CYC(0x45a1, 0x45a3); A = 0x01;
  CYC(0x45a3, 0x45a4); mem_wr(gb, BC, A);
  CYC(0x45a4, 0x45a5); ret_effect(gb);
}

void collisionEffect25_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x45a5, killEnemyOrPart_hook, 0x4649, 0x45a8);
  CYC(0x45a8, 0x45a9); A = L;
  CYC(0x45a9, 0x45ab); alu_add(gb, 0x1b);
  CYC(0x45ab, 0x45ac); L = A;
  CYC(0x45ac, 0x45ae); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x45ae, 0x45b0); C = 0x2a;
  CYC(0x45b0, 0x45b2); A = 0x02;
  CYC(0x45b2, 0x45b3); mem_wr(gb, BC, A);
  CYC(0x45b3, 0x45b4); ret_effect(gb);
}

void collisionEffect2a_hook(GB *gb) {
  CYC(0x4604, 0x4605); H = B;
  CYC(0x4605, 0x4607); L = 0x2d;
  CYC(0x4607, 0x4608); A = D;
  CYC(0x4608, 0x4609); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4609, 0x460a); ret_effect(gb); return; }
  CYC(0x4609, 0x460a);
  CYC(0x460a, 0x460b); mem_wr(gb, HL, A); L--;
  CYC(0x460b, 0x460d); E = 0xe1;
  CYC(0x460d, 0x460e); A = mem_rd(gb, DE);
  CYC(0x460e, 0x460f); mem_wr(gb, HL, A); L--;
  CYC(0x460f, 0x4610); L = alu_dec8(gb, L);
  CYC(0x4610, 0x4612); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(0x4612, 0x4614); E = 0xea;
  CYC(0x4614, 0x4616); A = H8(hFF90);
  CYC(0x4616, 0x4618); alu_or(gb, 0x80);
  CYC(0x4618, 0x4619); mem_wr(gb, DE, A);
  CYC(0x4619, 0x461a); ret_effect(gb);
}

void collisionEffect2d_hook(GB *gb) {
  CYC(0x461a, 0x461b); H = B;
  CYC(0x461b, 0x461d); L = 0x2f;
  CYC(0x461d, 0x461f); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(0x461f, 0x4620); ret_effect(gb);
}

void collisionEffect31_hook(GB *gb) {
  CYC(0x4620, 0x4622); A = 0x34;
  CYC(0x4622, 0x4625); applyDamageToEnemyOrPart(gb);
}

void collisionEffect32_hook(GB *gb) {
  CYC(0x4625, 0x4628); SET_HL(0x3448);
  CYC(0x4628, 0x462a); label_07_033_hook(gb);
}

void collisionEffect33_hook(GB *gb) {
  CYC(0x462a, 0x462d); SET_HL(0x384c);
  label_07_033_hook(gb);
}

void label_07_033_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x462d, applyDamageToBothObjects_hook, 0x44f1, 0x4630);
  CYC(0x4630, 0x4633); createClinkInteraction(gb);
}

void collisionEffect34_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x4633, createFlamePart, 0x46da, 0x4636);
  CYC(0x4636, 0x4637); H = B;
  CYC(0x4637, 0x4639); L = 0x24;
  CYC(0x4639, 0x463b); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x463b, 0x463e); SET_HL(0x1c2c);
  CALL_C(0x463e, applyDamageToBothObjects_hook, 0x44f1, 0x4641);
  CYC(0x4641, 0x4643); killEnemyOrPart_hook(gb);
}

void collisionEffect35_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4643, 0x4646); SET_HL(0x1c1c);
  CALL_C(0x4646, applyDamageToBothObjects_hook, 0x44f1, 0x4649);
  killEnemyOrPart_hook(gb);
}

void killEnemyOrPart_hook(GB *gb) {
  CYC(0x4649, 0x464a); H = D;
  CYC(0x464a, 0x464c); A = H8(hActiveObjectType);
  CYC(0x464c, 0x464e); alu_add(gb, 0x29);
  CYC(0x464e, 0x464f); L = A;
  CYC(0x464f, 0x4651); mem_wr(gb, HL, 0x00);
  CYC(0x4651, 0x4653); alu_add(gb, 0xfb);
  CYC(0x4653, 0x4654); L = A;
  CYC(0x4654, 0x4656); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x4656, 0x4657); ret_effect(gb);
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
