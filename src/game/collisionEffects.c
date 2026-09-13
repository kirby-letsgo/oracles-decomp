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

void collisionEffect3c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x43f4, 0x43f6); A = H8(hActiveObjectType);
  CYC(0x43f6, 0x43f7); A = alu_inc8(gb, A);
  CYC(0x43f7, 0x43f8); E = A;
  CYC(0x43f8, 0x43f9); A = mem_rd(gb, DE);
  CYC(0x43f9, 0x43fa); C = A;
  CYC(0x43fa, 0x43fd); SET_HL(0x441d);
  for (;;) {
    CYC(0x43fd, 0x43fe); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x43fe, 0x43ff); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x43ff, 0x4401);
      collisionEffect02_hook(gb);
      return;
    }
    CYC(0x43ff, 0x4401);
    CYC(0x4401, 0x4402); alu_cp(gb, C);
    CYC(0x4402, 0x4403); A = mem_rd(gb, HL); SET_HL(HL + 1);
    if (!(F & FZ)) {
      CYCT(0x4403, 0x4405);
      continue;
    }
    CYC(0x4403, 0x4405);
    break;
  }
  CYC(0x4405, 0x4406); C = A;
  CYC(0x4406, 0x4408); alu_and(gb, 0x7f);
  CALL_C(0x4408, cpActiveRing_hook, 0x23b0, 0x440b);
  if (!(F & FZ)) {
    CYCT(0x440b, 0x440d);
    collisionEffect02_hook(gb);
    return;
  }
  CYC(0x440b, 0x440d);
  CYC(0x440d, 0x440f); alu_bit(gb, 7, C);
  CYC(0x440f, 0x4411); A = 0x40;
  if (F & FZ) {
    CYCT(0x4411, 0x4414);
    applyDamageToEnemyOrPart(gb);
    return;
  }
  CYC(0x4411, 0x4414);
  CALL_C(0x4414, collisionEffect02_hook, 0x442a, 0x4417);
  CYC(0x4417, 0x4418); H = B;
  CYC(0x4418, 0x441a); L = 0x25;
  CYC(0x441a, 0x441c); mem_wr(gb, HL, alu_sra(gb, mem_rd(gb, HL)));
  CYC(0x441c, 0x441d); ret_effect(gb);
}

void collisionEffect0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x4448, func_07_47b7, 0x47b7, 0x444b);
  if (F & FZ) { CYCT(0x444b, 0x444c); ret_effect(gb); return; }
  CYC(0x444b, 0x444c);
  CYC(0x444c, 0x444e); E = 0x0c;
  CYC(0x444e, 0x4450); label_07_027_hook(gb);
}

void collisionEffect21_hook(GB *gb) {
  CYC(0x4450, 0x4452); E = 0x30;
  label_07_027_hook(gb);
}

void collisionEffect2e_hook(GB *gb) {
  CYC(0x4551, 0x4552); H = D;
  CYC(0x4552, 0x4554); A = H8(hActiveObjectType);
  CYC(0x4554, 0x4556); alu_add(gb, 0x29);
  CYC(0x4556, 0x4557); L = A;
  CYC(0x4557, 0x4558); A = mem_rd(gb, HL);
  CYC(0x4558, 0x4559); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4559, 0x455b);
    collisionEffect1c_hook(gb);
    return;
  }
  CYC(0x4559, 0x455b);
  CYC(0x455b, 0x455c); A = L;
  CYC(0x455c, 0x455e); alu_add(gb, 0x05);
  CYC(0x455e, 0x455f); L = A;
  CYC(0x455f, 0x4560); alu_xor(gb, A);
  CYC(0x4560, 0x4561); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4561, 0x4562); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4562, 0x4564); A = H8(hFF8A);
  CYC(0x4564, 0x4566); alu_xor(gb, 0x10);
  CYC(0x4566, 0x4567); mem_wr(gb, HL, A);
  CYC(0x4567, 0x4569); L &= 0xf7;
  CYC(0x4569, 0x456b); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x456b, 0x456c); A = L;
  CYC(0x456c, 0x456e); alu_add(gb, 0xe0);
  CYC(0x456e, 0x456f); L = A;
  CYC(0x456f, 0x4571); mem_wr(gb, HL, 0x03);
  CYC(0x4571, 0x4572); L = alu_inc8(gb, L);
  CYC(0x4572, 0x4574); mem_wr(gb, HL, 0x00);
  CYC(0x4574, 0x4575); H = B;
  CYC(0x4575, 0x4577); L = 0x2a;
  CYC(0x4577, 0x4579); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(0x4579, 0x457b); L = 0x24;
  CYC(0x457b, 0x457d); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x457d, 0x457f); L = 0x18;
  CYC(0x457f, 0x4581); A = H8(hActiveObjectType);
  CYC(0x4581, 0x4582); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4582, 0x4583); mem_wr(gb, HL, D);
  CYC(0x4583, 0x4584); ret_effect(gb);
}

void collisionEffect29_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x45b4, 0x45b5); H = B;
  CYC(0x45b5, 0x45b7); L = 0x24;
  CYC(0x45b7, 0x45b9); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(0x45b9, func_07_47b7, 0x47b7, 0x45bc);
  if (F & FZ) { CYCT(0x45bc, 0x45bd); ret_effect(gb); return; }
  CYC(0x45bc, 0x45bd);
  CYC(0x45bd, 0x45be); H = D;
  CYC(0x45be, 0x45c0); L = 0xaa;
  CYC(0x45c0, 0x45c2); mem_wr(gb, HL, 0x9e);
  CYC(0x45c2, 0x45c4); L = 0xae;
  CYC(0x45c4, 0x45c6); mem_wr(gb, HL, 0x00);
  CYC(0x45c6, 0x45c8); L = 0xa4;
  CYC(0x45c8, 0x45ca); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x45ca, 0x45cc); L = 0x84;
  CYC(0x45cc, 0x45ce); mem_wr(gb, HL, 0x05);
  CYC(0x45ce, 0x45d0); L = 0x9a;
  CYC(0x45d0, 0x45d1); A = mem_rd(gb, HL);
  CYC(0x45d1, 0x45d3); alu_and(gb, 0xc0);
  CYC(0x45d3, 0x45d5); alu_or(gb, 0x02);
  CYC(0x45d5, 0x45d6); mem_wr(gb, HL, A);
  CYC(0x45d6, 0x45d8); L = 0x87;
  CYC(0x45d8, 0x45da); mem_wr(gb, HL, 0x1e);
  CYC(0x45da, 0x45dc); L = 0x90;
  CYC(0x45dc, 0x45de); mem_wr(gb, HL, 0x05);
  CYC(0x45de, 0x45e0); L = 0x94;
  CYC(0x45e0, 0x45e2); mem_wr(gb, HL, 0x00);
  CYC(0x45e2, 0x45e3); L = alu_inc8(gb, L);
  CYC(0x45e3, 0x45e5); mem_wr(gb, HL, 0xfa);
  CYC(0x45e5, 0x45e7); L = 0x8b;
  CYC(0x45e7, 0x45e9); C = 0x0b;
  CYC(0x45e9, 0x45ea); A = mem_rd(gb, BC);
  CYC(0x45ea, 0x45eb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x45eb, 0x45ec); L = alu_inc8(gb, L);
  CYC(0x45ec, 0x45ee); C = 0x0d;
  CYC(0x45ee, 0x45ef); A = mem_rd(gb, BC);
  CYC(0x45ef, 0x45f0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x45f0, 0x45f1); L = alu_inc8(gb, L);
  CYC(0x45f1, 0x45f2); A = mem_rd(gb, HL);
  CYC(0x45f2, 0x45f3); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x45f3, 0x45f5);
  } else {
    CYC(0x45f3, 0x45f5);
    CYC(0x45f5, 0x45f7); mem_wr(gb, HL, 0xff);
  }
  CALL_C(0x45f7, getRandomNumber_hook, 0x043e, 0x45fa);
  CYC(0x45fa, 0x45fc); alu_and(gb, 0x18);
  CYC(0x45fc, 0x45fe); E = 0x89;
  CYC(0x45fe, 0x45ff); mem_wr(gb, DE, A);
  CYC(0x45ff, 0x4601); A = 0x1c;
  CYC(0x4601, 0x4604); applyDamageToLink(gb);
}

void collisionEffect36_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4657, 0x4658); H = D;
  CYC(0x4658, 0x465a); A = H8(hActiveObjectType);
  CYC(0x465a, 0x465c); alu_add(gb, 0x2a);
  CYC(0x465c, 0x465d); L = A;
  CYC(0x465d, 0x465f); mem_wr(gb, HL, 0xa0);
  CYC(0x465f, 0x4661); alu_add(gb, 0xfa);
  CYC(0x4661, 0x4662); L = A;
  CYC(0x4662, 0x4664); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x4664, 0x4666); A = 0x1e;
  CALL_C(0x4666, cpActiveRing_hook, 0x23b0, 0x4669);
  CYC(0x4669, 0x466b); A = 0xf8;
  if (!(F & FZ)) {
    CYCT(0x466b, 0x466d);
  } else {
    CYC(0x466b, 0x466d);
    CYC(0x466d, 0x466e); alu_xor(gb, A);
  }
  CYC(0x466e, 0x4671); SET_HL(w1Link_damageToApply);
  CYC(0x4671, 0x4672); mem_wr(gb, HL, A);
  CYC(0x4672, 0x4674); L = 0x2c;
  CYC(0x4674, 0x4676); A = H8(hFF8A);
  CYC(0x4676, 0x4677); mem_wr(gb, HL, A);
  CYC(0x4677, 0x4679); L = 0x2d;
  CYC(0x4679, 0x467b); mem_wr(gb, HL, 0x08);
  CYC(0x467b, 0x467d); L = 0x2b;
  CYC(0x467d, 0x467f); mem_wr(gb, HL, 0x0c);
  CYC(0x467f, 0x4682); A = mem_rd(gb, wIsLinkBeingShocked);
  CYC(0x4682, 0x4683); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4683, 0x4685);
  } else {
    CYC(0x4683, 0x4685);
    CYC(0x4685, 0x4686); A = alu_inc8(gb, A);
    CYC(0x4686, 0x4689); mem_wr(gb, wIsLinkBeingShocked, A);
  }
  CYC(0x4689, 0x468a); H = B;
  CYC(0x468a, 0x468c); L = 0x24;
  CYC(0x468c, 0x468e); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x468e, 0x4690); A = 0x1c;
  CYC(0x4690, 0x4693); applyDamageToLink(gb);
}

void collisionEffect37_hook(GB *gb) {
  CYC(0x4693, 0x4695); A = H8(hActiveObjectType);
  CYC(0x4695, 0x4697); alu_add(gb, 0x2b);
  CYC(0x4697, 0x4698); E = A;
  CYC(0x4698, 0x4699); A = mem_rd(gb, DE);
  CYC(0x4699, 0x469a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x469a, 0x469b); ret_effect(gb); return; }
  CYC(0x469a, 0x469b);
  CYC(0x469b, 0x469e); A = mem_rd(gb, wWarpsDisabled);
  CYC(0x469e, 0x469f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x469f, 0x46a0); ret_effect(gb); return; }
  CYC(0x469f, 0x46a0);
  CYC(0x46a0, 0x46a3); A = mem_rd(gb, w1Link_state);
  CYC(0x46a3, 0x46a5); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x46a5, 0x46a6); ret_effect(gb); return; }
  CYC(0x46a5, 0x46a6);
  CYC(0x46a6, 0x46a7); A = E;
  CYC(0x46a7, 0x46a9); alu_add(gb, 0xf9);
  CYC(0x46a9, 0x46aa); E = A;
  CYC(0x46aa, 0x46ab); alu_xor(gb, A);
  CYC(0x46ab, 0x46ac); mem_wr(gb, DE, A);
  CYC(0x46ac, 0x46ae); A = 0x0c;
  CYC(0x46ae, 0x46b1); mem_wr(gb, wLinkForceState, A);
  CYC(0x46b1, 0x46b3); A = 0x1c;
  CYC(0x46b3, 0x46b6); applyDamageToEnemyOrPart(gb);
}

void collisionEffect38_hook(GB *gb) {
  CYC(0x46b6, 0x46b7); H = D;
  CYC(0x46b7, 0x46b9); A = H8(hActiveObjectType);
  CYC(0x46b9, 0x46bb); alu_add(gb, 0x24);
  CYC(0x46bb, 0x46bc); L = A;
  CYC(0x46bc, 0x46be); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x46be, 0x46c0); alu_add(gb, 0xe2);
  CYC(0x46c0, 0x46c1); L = A;
  CYC(0x46c1, 0x46c3); mem_wr(gb, HL, 0x60);
  CYC(0x46c3, 0x46c5); alu_add(gb, 0x09);
  CYC(0x46c5, 0x46c6); L = A;
  CYC(0x46c6, 0x46c8); mem_wr(gb, HL, 0x00);
  CYC(0x46c8, 0x46ca); A = 0x1c;
  CYC(0x46ca, 0x46cd); applyDamageToEnemyOrPart(gb);
}

void collisionEffect39_hook(GB *gb) {
  CYC(0x46cd, 0x46ce); ret_effect(gb);
}

void collisionEffect3b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x46ce, 0x46d0); A = 0x02;
  CALL_C(0x46d0, setLinkIDOverride_hook, 0x2acf, 0x46d3);
  CYC(0x46d3, 0x46d5); A = 0x1c;
  CYC(0x46d5, 0x46d8); applyDamageToEnemyOrPart(gb);
}

void collisionEffect3e_hook(GB *gb) {
  CYC(0x46d8, 0x46d9); ret_effect(gb);
}

void collisionEffect3f_hook(GB *gb) {
  CYC(0x46d9, 0x46da); ret_effect(gb);
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
