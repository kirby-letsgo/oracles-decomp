#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

void interactionCode4a_hook(GB *gb);
void introSpriteIncStateAndLoadGraphics_hook(GB *gb);
void introSpriteFunc_461a_hook(GB *gb);
void introSpritesState1_hook(GB *gb);
void introSpriteTriforceSubid_hook(GB *gb);
void introSpriteRunSubid07_hook(GB *gb);
void introSpriteRunTriforceGlowSubid_hook(GB *gb);
void introSpriteRunSubid04_hook(GB *gb);
void introSpriteRunSubid08_hook(GB *gb);
void introSpriteSetChildRelatedObject1ToSelf_hook(GB *gb);

static uint16_t introSprites1_jumpTable(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void introSprites1_addDoubleIndex(GB *gb, uint16_t returnAddress) {
  push_effect(gb, returnAddress);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void introSprite_addRandomVariance_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4656, 0x4657); B = A;
  CALL_C(0x4657, getRandomNumber_hook, 0x043e, 0x465a);
  CYC(0x465a, 0x465c); alu_and(gb, 3);
  CYC(0x465c, 0x465e); alu_sub(gb, 2);
  CYC(0x465e, 0x465f); alu_add(gb, B);
  CYC(0x465f, 0x4660); ret_effect(gb);
}

void interactionCode4a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4591, 0x4593); E = 0x44;
  CYC(0x4593, 0x4594); A = mem_rd(gb, DE);
  CYC(0x4594, 0x4595); push_effect(gb, 0x4595);
  switch (introSprites1_jumpTable(gb)) {
    case 0x4599: goto state0;
    case 0x467e: introSpritesState1_hook(gb); return;
    default: HANDOFF(HL);
  }

state0:
  CALL_C(0x4599, introSpriteIncStateAndLoadGraphics_hook, 0x4613, 0x459c);
  CYC(0x459c, 0x459e); E = 0x42;
  CYC(0x459e, 0x459f); A = mem_rd(gb, DE);
  CYC(0x459f, 0x45a0); push_effect(gb, 0x45a0);
  switch (introSprites1_jumpTable(gb)) {
    case 0x45b6: goto initTriforce;
    case 0x45cc: goto initMovingSprite;
    case 0x45df: goto initTree;
    case 0x460c: goto initGlow;
    case 0x4613: introSpriteIncStateAndLoadGraphics_hook(gb); return;
    case 0x1e69: objectSetVisible82_hook(gb); return;
    default: HANDOFF(HL);
  }

initTriforce:
  CALL_C(0x45b6, getFreeInteractionSlot_hook, 0x3aef, 0x45b9);
  if (!(F & FZ)) { CYCT(0x45b9, 0x45bb); goto makeVisible82; }
  CYC(0x45b9, 0x45bb);
  CYC(0x45bb, 0x45bd); mem_wr(gb, HL, 0x4a);
  CYC(0x45bd, 0x45be); L = alu_inc8(gb, L);
  CYC(0x45be, 0x45c0); mem_wr(gb, HL, 4);
  CYC(0x45c0, 0x45c1); L = alu_inc8(gb, L);
  CYC(0x45c1, 0x45c3); E = 0x42;
  CYC(0x45c3, 0x45c4); A = mem_rd(gb, DE);
  CYC(0x45c4, 0x45c5); A = alu_inc8(gb, A);
  CYC(0x45c5, 0x45c6); mem_wr(gb, HL, A);
  CALL_C(0x45c6, introSpriteSetChildRelatedObject1ToSelf_hook, 0x4794, 0x45c9);
makeVisible82:
  CYC(0x45c9, 0x45cc); objectSetVisible82_hook(gb);
  return;

initMovingSprite:
  CYC(0x45cc, 0x45ce); E = 0x43;
  CYC(0x45ce, 0x45cf); A = mem_rd(gb, DE);
  CYC(0x45cf, 0x45d0); alu_add(gb, A);
  CYC(0x45d0, 0x45d1); alu_add(gb, A);
  CYC(0x45d1, 0x45d2); H = D;
  CYC(0x45d2, 0x45d4); L = 0x60;
  CYC(0x45d4, 0x45d5); alu_add(gb, mem_rd(gb, HL));
  CYC(0x45d5, 0x45d6); mem_wr(gb, HL, A);
  CALL_C(0x45d6, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x45d9);
  CALL_C(0x45d9, introSpriteFunc_461a_hook, 0x461a, 0x45dc);
  CYC(0x45dc, 0x45df); objectSetVisible80_hook(gb);
  return;

initTree:
  CYC(0x45df, 0x45e1); E = 0x43;
  CYC(0x45e1, 0x45e2); A = mem_rd(gb, DE);
  CYC(0x45e2, 0x45e5); SET_HL(0x4606);
  CYC(0x45e5, 0x45e6); introSprites1_addDoubleIndex(gb, 0x45e6);
  CYC(0x45e6, 0x45e7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45e7, 0x45e9); E = 0x4b;
  CYC(0x45e9, 0x45ea); mem_wr(gb, DE, A);
  CYC(0x45ea, 0x45eb); E = alu_inc8(gb, E);
  CYC(0x45eb, 0x45ec); E = alu_inc8(gb, E);
  CYC(0x45ec, 0x45ed); A = mem_rd(gb, HL);
  CYC(0x45ed, 0x45ee); mem_wr(gb, DE, A);
  CYC(0x45ee, 0x45f0); B = 3;
spawnTreeBranches:
  CALL_C(0x45f0, getFreeInteractionSlot_hook, 0x3aef, 0x45f3);
  if (!(F & FZ)) { CYCT(0x45f3, 0x45f5); goto treeVisible82; }
  CYC(0x45f3, 0x45f5);
  CYC(0x45f5, 0x45f7); mem_wr(gb, HL, 0x4a);
  CYC(0x45f7, 0x45f8); L = alu_inc8(gb, L);
  CYC(0x45f8, 0x45fa); mem_wr(gb, HL, 0x0a);
  CYC(0x45fa, 0x45fb); L = alu_inc8(gb, L);
  CYC(0x45fb, 0x45fc); mem_wr(gb, HL, B);
  CYC(0x45fc, 0x45fd); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x45fd, introSpriteSetChildRelatedObject1ToSelf_hook, 0x4794, 0x4600);
  CYC(0x4600, 0x4601); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x4601, 0x4603); goto spawnTreeBranches; }
  CYC(0x4601, 0x4603);
treeVisible82:
  CYC(0x4603, 0x4606); objectSetVisible82_hook(gb);
  return;

initGlow:
  CALL_C(0x460c, objectSetVisible83_hook, 0x1e72, 0x460f);
  CYC(0x460f, 0x4611); alu_xor(gb, 0x80);
  CYC(0x4611, 0x4612); mem_wr(gb, DE, A);
  CYC(0x4612, 0x4613); ret_effect(gb);
}

void introSpriteIncStateAndLoadGraphics_hook(GB *gb) {
  CYC(0x4613, 0x4614); H = D;
  CYC(0x4614, 0x4616); L = 0x44;
  CYC(0x4616, 0x4617); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4617, 0x461a); interactionInitGraphics_hook(gb);
}

void introSpriteFunc_461a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x461a, objectGetRelatedObject1Var_hook, 0x2160, 0x461d);
  CALL_C(0x461d, objectTakePosition_hook, 0x2274, 0x4620);
  CYC(0x4620, 0x4621); push_effect(gb, BC);
  CYC(0x4621, 0x4623); E = 0x42;
  CYC(0x4623, 0x4624); A = mem_rd(gb, DE);
  CYC(0x4624, 0x4627); SET_HL(0x4660);
  CYC(0x4627, 0x4629); alu_cp(gb, 3);
  if (F & FZ) {
    CYCT(0x4629, 0x462b);
  } else {
    CYC(0x4629, 0x462b);
    CYC(0x462b, 0x462d); alu_cp(gb, 0x0a);
    if (F & FZ) {
      CYCT(0x462d, 0x462f);
    } else {
      CYC(0x462d, 0x462f);
      CYC(0x462f, 0x4632); SET_HL(0x4666);
      CYC(0x4632, 0x4634); E = 0x47;
      CYC(0x4634, 0x4635); A = mem_rd(gb, DE);
      CYC(0x4635, 0x4636); A = alu_inc8(gb, A);
      CYC(0x4636, 0x4637); mem_wr(gb, DE, A);
      CYC(0x4637, 0x4639); alu_and(gb, 3);
      CYC(0x4639, 0x463a); C = A;
      CYC(0x463a, 0x463b); alu_add(gb, A);
      CYC(0x463b, 0x463c); alu_add(gb, C);
      CYC(0x463c, 0x463d); introSprites1_addDoubleIndex(gb, 0x463d);
    }
  }
  CYC(0x463d, 0x463f); E = 0x43;
  CYC(0x463f, 0x4640); A = mem_rd(gb, DE);
  CYC(0x4640, 0x4641); introSprites1_addDoubleIndex(gb, 0x4641);
  CYC(0x4641, 0x4642); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4642, 0x4645); push_effect(gb, 0x4645); introSprite_addRandomVariance_hook(gb);
  CYC(0x4645, 0x4646); B = A;
  CYC(0x4646, 0x4648); E = 0x4b;
  CYC(0x4648, 0x4649); A = mem_rd(gb, DE);
  CYC(0x4649, 0x464a); alu_add(gb, B);
  CYC(0x464a, 0x464b); mem_wr(gb, DE, A);
  CYC(0x464b, 0x464c); A = mem_rd(gb, HL);
  CYC(0x464c, 0x464f); push_effect(gb, 0x464f); introSprite_addRandomVariance_hook(gb);
  CYC(0x464f, 0x4650); H = D;
  CYC(0x4650, 0x4652); L = 0x4d;
  CYC(0x4652, 0x4653); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4653, 0x4654); mem_wr(gb, HL, A);
  CYC(0x4654, 0x4655); SET_BC(pop_effect(gb));
  CYC(0x4655, 0x4656); ret_effect(gb);
}

void introSpritesState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x467e, 0x4680); E = 0x42;
  CYC(0x4680, 0x4681); A = mem_rd(gb, DE);
  CYC(0x4681, 0x4683); alu_cp(gb, 5);
  if (!(F & FC)) {
    CYCT(0x4683, 0x4685);
  } else {
    CYC(0x4683, 0x4685);
    CYC(0x4685, 0x4688); A = W8(wIntro_triforceState);
    CYC(0x4688, 0x468a); alu_cp(gb, 4);
    if (F & FZ) { CYCT(0x468a, 0x468d); interactionDelete_hook(gb); return; }
    CYC(0x468a, 0x468d);
  }
  CYC(0x468d, 0x468e); A = mem_rd(gb, DE);
  CYC(0x468e, 0x468f); push_effect(gb, 0x468f);
  switch (introSprites1_jumpTable(gb)) {
    case 0x46a5: introSpriteTriforceSubid_hook(gb); return;
    case 0x475f: introSpriteRunTriforceGlowSubid_hook(gb); return;
    case 0x4769: introSpriteRunSubid04_hook(gb); return;
    case 0x474d: introSpriteRunSubid07_hook(gb); return;
    case 0x4785: introSpriteRunSubid08_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void introSpriteTriforceSubid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46a5, 0x46a7); E = 0x45;
  CYC(0x46a7, 0x46a8); A = mem_rd(gb, DE);
  CYC(0x46a8, 0x46a9); push_effect(gb, 0x46a9);
  switch (introSprites1_jumpTable(gb)) {
    case 0x46b5: goto substate0;
    case 0x46d1: goto substate1;
    case 0x4703: goto substate2;
    case 0x4719: goto substate3;
    case 0x4739: goto substate4;
    case 0x261b: interactionAnimate_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x46b5, 0x46b8); A = W8(wIntro_triforceState);
  CYC(0x46b8, 0x46ba); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(0x46ba, 0x46bd); interactionAnimate_hook(gb); return; }
  CYC(0x46ba, 0x46bd);
  CYC(0x46bd, 0x46bf); B = 0;
  CYC(0x46bf, 0x46c1); E = 0x42;
  CYC(0x46c1, 0x46c2); A = mem_rd(gb, DE);
  CYC(0x46c2, 0x46c4); alu_cp(gb, 1);
  if (F & FZ) { CYCT(0x46c4, 0x46c6); } else { CYC(0x46c4, 0x46c6); CYC(0x46c6, 0x46c8); B = 0x0a; }
  CALL_C(0x46c8, func_2d48_hook, 0x2d48, 0x46cb);
  CALL_C(0x46cb, interactionIncSubstate_hook, 0x23e5, 0x46ce);
  CYC(0x46ce, 0x46d0); L = 0x46;
  CYC(0x46d0, 0x46d1); mem_wr(gb, HL, B);

substate1:
  CALL_C(0x46d1, interactionDecCounter1_hook, 0x23cc, 0x46d4);
  if (!(F & FZ)) { CYCT(0x46d4, 0x46d7); interactionAnimate_hook(gb); return; }
  CYC(0x46d4, 0x46d7);
  CYC(0x46d7, 0x46d9); L = 0x42;
  CYC(0x46d9, 0x46da); A = mem_rd(gb, HL);
  CYC(0x46da, 0x46dc); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(0x46dc, 0x46de); goto centerPiece; }
  CYC(0x46dc, 0x46de);
  CYC(0x46de, 0x46e0); L = 0x49;
  CYC(0x46e0, 0x46e2); mem_wr(gb, HL, 0);
  CYC(0x46e2, 0x46e4); L = 0x50;
  CYC(0x46e4, 0x46e6); mem_wr(gb, HL, 5);
  CYC(0x46e6, 0x46e8); B = 1;
  CYC(0x46e8, 0x46ea); goto afterAngle;

centerPiece:
  CYC(0x46ea, 0x46eb); alu_or(gb, A);
  CYC(0x46eb, 0x46ed); A = 0x18;
  if (F & FZ) { CYCT(0x46ed, 0x46ef); } else { CYC(0x46ed, 0x46ef); CYC(0x46ef, 0x46f1); A = 8; }
  CYC(0x46f1, 0x46f3); L = 0x49;
  CYC(0x46f3, 0x46f4); mem_wr(gb, HL, A);
  CYC(0x46f4, 0x46f6); L = 0x50;
  CYC(0x46f6, 0x46f8); mem_wr(gb, HL, 5);
  CYC(0x46f8, 0x46fa); B = 0x0b;

afterAngle:
  CALL_C(0x46fa, func_2d48_hook, 0x2d48, 0x46fd);
  CALL_C(0x46fd, interactionIncSubstate_hook, 0x23e5, 0x4700);
  CYC(0x4700, 0x4702); L = 0x46;
  CYC(0x4702, 0x4703); mem_wr(gb, HL, B);

substate2:
  CALL_C(0x4703, interactionDecCounter1_hook, 0x23cc, 0x4706);
  if (!(F & FZ)) { CYCT(0x4706, 0x4708); goto applySpeed; }
  CYC(0x4706, 0x4708);
  CYC(0x4708, 0x470a); B = 2;
  CALL_C(0x470a, func_2d48_hook, 0x2d48, 0x470d);
  CALL_C(0x470d, interactionIncSubstate_hook, 0x23e5, 0x4710);
  CYC(0x4710, 0x4712); L = 0x46;
  CYC(0x4712, 0x4713); mem_wr(gb, HL, B);
applySpeed:
  CALL_C(0x4713, objectApplySpeed_hook, 0x201d, 0x4716);
  CYC(0x4716, 0x4719); interactionAnimate_hook(gb);
  return;

substate3:
  CALL_C(0x4719, interactionDecCounter1_hook, 0x23cc, 0x471c);
  if (!(F & FZ)) { CYCT(0x471c, 0x471f); interactionAnimate_hook(gb); return; }
  CYC(0x471c, 0x471f);
  CYC(0x471f, 0x4721); B = 3;
  CALL_C(0x4721, func_2d48_hook, 0x2d48, 0x4724);
  CALL_C(0x4724, interactionIncSubstate_hook, 0x23e5, 0x4727);
  CYC(0x4727, 0x4729); L = 0x46;
  CYC(0x4729, 0x472a); mem_wr(gb, HL, B);
  CYC(0x472a, 0x472c); E = 0x42;
  CYC(0x472c, 0x472d); A = mem_rd(gb, DE);
  CYC(0x472d, 0x472f); alu_cp(gb, 1);
  if (F & FZ) { CYCT(0x472f, 0x4731); CYC(0x4734, 0x4736); A = 0x5c; CYC(0x4736, 0x4739); playSound_b00_hook(gb); return; }
  CYC(0x472f, 0x4731);
  CYC(0x4731, 0x4734); interactionIncSubstate_hook(gb);
  return;

substate4:
  CALL_C(0x4739, interactionAnimate_hook, 0x261b, 0x473c);
  CALL_C(0x473c, interactionDecCounter1_hook, 0x23cc, 0x473f);
  if (!(F & FZ)) { CYCT(0x473f, 0x4740); ret_effect(gb); return; }
  CYC(0x473f, 0x4740);
  CALL_C(0x4740, interactionIncSubstate_hook, 0x23e5, 0x4743);
  CYC(0x4743, 0x4745); A = 2;
  CYC(0x4745, 0x4748); W8(wIntro_triforceState) = A;
  CYC(0x4748, 0x474a); A = 0x7c;
  CYC(0x474a, 0x474d); playSound_b00_hook(gb);
}

void introSpriteRunSubid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x474d, objectSetVisible_hook, 0x1e84, 0x4750);
  CYC(0x4750, 0x4752); E = 0x43;
  CYC(0x4752, 0x4753); A = mem_rd(gb, DE);
  CYC(0x4753, 0x4755); alu_and(gb, 1);
  CYC(0x4755, 0x4756); B = A;
  CYC(0x4756, 0x4759); A = W8(wIntro_frameCounter);
  CYC(0x4759, 0x475b); alu_and(gb, 1);
  CYC(0x475b, 0x475c); alu_xor(gb, B);
  if (F & FZ) CALL_C_CC(0x475c, objectSetInvisible_hook, 0x1e7b, 0x475f); else CYC(0x475c, 0x475f);
  introSpriteRunTriforceGlowSubid_hook(gb);
}

void introSpriteRunTriforceGlowSubid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x475f, 0x4761); E = 0x61;
  CYC(0x4761, 0x4762); A = mem_rd(gb, DE);
  CYC(0x4762, 0x4763); A = alu_inc8(gb, A);
  if (F & FZ) CALL_C_CC(0x4763, introSpriteFunc_461a_hook, 0x461a, 0x4766); else CYC(0x4763, 0x4766);
  CYC(0x4766, 0x4769); interactionAnimate_hook(gb);
}

void introSpriteRunSubid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4769, interactionAnimate_hook, 0x261b, 0x476c);
  CYC(0x476c, 0x476e); A = 0;
  CALL_C(0x476e, objectGetRelatedObject1Var_hook, 0x2160, 0x4771);
  CALL_C(0x4771, objectTakePosition_hook, 0x2274, 0x4774);
  CYC(0x4774, 0x4776); E = 0x43;
  CYC(0x4776, 0x4777); A = mem_rd(gb, DE);
  CYC(0x4777, 0x4778); H = D;
  CYC(0x4778, 0x477a); L = 0x60;
  CYC(0x477a, 0x477b); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x477b, 0x477d); L = 0x5a;
  if (!(F & FZ)) { CYCT(0x477d, 0x477f); CYC(0x4782, 0x4784); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f); CYC(0x4784, 0x4785); ret_effect(gb); return; }
  CYC(0x477d, 0x477f);
  CYC(0x477f, 0x4781); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x4781, 0x4782); ret_effect(gb);
}

void introSpriteRunSubid08_hook(GB *gb) {
  CYC(0x4785, 0x4788); A = W8(wGfxRegs1_SCY);
  CYC(0x4788, 0x4789); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4789, 0x478c); interactionDelete_hook(gb); return; }
  CYC(0x4789, 0x478c);
  CYC(0x478c, 0x478d); B = A;
  CYC(0x478d, 0x478f); E = 0x4a;
  CYC(0x478f, 0x4790); A = mem_rd(gb, DE);
  CYC(0x4790, 0x4791); alu_sub(gb, B);
  CYC(0x4791, 0x4792); E = alu_inc8(gb, E);
  CYC(0x4792, 0x4793); mem_wr(gb, DE, A);
  CYC(0x4793, 0x4794); ret_effect(gb);
}

void introSpriteSetChildRelatedObject1ToSelf_hook(GB *gb) {
  CYC(0x4794, 0x4796); L = 0x56;
  CYC(0x4796, 0x4798); mem_wr(gb, HL, 0x40);
  CYC(0x4798, 0x4799); L = alu_inc8(gb, L);
  CYC(0x4799, 0x479a); mem_wr(gb, HL, D);
  CYC(0x479a, 0x479b); ret_effect(gb);
}
