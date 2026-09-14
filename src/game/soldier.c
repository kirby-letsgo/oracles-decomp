#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

void interactionCode40_hook(GB *gb);
void soldierSubid00_hook(GB *gb);
void soldierSubid0c_hook(GB *gb);
void soldierSubid02_hook(GB *gb);
void soldierSubid03_hook(GB *gb);
void soldierSubid04_hook(GB *gb);

static uint16_t soldier_jump_table(GB *gb) {
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

void interactionCode40_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f66, 0x4f68); E = 0x42;
  CYC(0x4f68, 0x4f69); A = mem_rd(gb, DE);
  CYC(0x4f69, 0x4f6a); push_effect(gb, 0x4f6a);
  switch (soldier_jump_table(gb)) {
    case 0x4f86: soldierSubid00_hook(gb); return;
    case 0x4fb3: soldierSubid02_hook(gb); return;
    case 0x4fe0: soldierSubid03_hook(gb); return;
    case 0x4ff7: soldierSubid04_hook(gb); return;
    case 0x507d: soldierSubid05(gb); return;
    case 0x50b9: soldierSubid06(gb); return;
    case 0x50f0: soldierSubid07(gb); return;
    case 0x5107: soldierSubid08(gb); return;
    case 0x5121: soldierSubid0a(gb); return;
    case 0x5147: soldierSubid0b(gb); return;
    case 0x4fa2: soldierSubid0c_hook(gb); return;
    case 0x516c: soldierSubid0d(gb); return;
    default: HANDOFF(HL);
  }
}

void soldierSubid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f86, 0x4f88); A = 0x14;
  CALL_C(0x4f88, checkGlobalFlag_hook, 0x31f3, 0x4f8b);
  if (!(F & FZ)) { CYCT(0x4f8b, 0x4f8e); interactionDelete_hook(gb); return; }
  CYC(0x4f8b, 0x4f8e);
  CYC(0x4f8e, 0x4f90); A = 0x0b;
  CALL_C(0x4f90, checkGlobalFlag_hook, 0x31f3, 0x4f93);
  CYC(0x4f93, 0x4f95); E = 0x43;
  CYC(0x4f95, 0x4f96); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(0x4f96, 0x4f98); label_09_090(gb); return; }
  CYC(0x4f96, 0x4f98);
  CYC(0x4f98, 0x4f99); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f99, 0x4f9c); interactionDelete_hook(gb); return; }
  CYC(0x4f99, 0x4f9c);
  CYC(0x4f9c, 0x4f9e); soldierSubid0c_hook(gb);
}

void soldierSubid0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fa2, checkInteractionState_hook, 0x23fe, 0x4fa5);
  if (!(F & FZ)) { CYCT(0x4fa5, 0x4fa7); label_09_092(gb); return; }
  CYC(0x4fa5, 0x4fa7);
  CALL_C(0x4fa7, soldierInitGraphicsAndLoadScript, 0x51d2, 0x4faa);
  label_09_092(gb);
}

void soldierSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fb3, checkInteractionState_hook, 0x23fe, 0x4fb6);
  if (!(F & FZ)) { CYCT(0x4fb6, 0x4fb8); label_09_093(gb); return; }
  CYC(0x4fb6, 0x4fb8);
  CALL_C(0x4fb8, soldierCheckBeatD6, 0x51f8, 0x4fbb);
  if (!(F & FC)) { CYCT(0x4fbb, 0x4fbe); interactionDelete_hook(gb); return; }
  CYC(0x4fbb, 0x4fbe);
  CALL_C(0x4fbe, soldierInitGraphicsAndLoadScript, 0x51d2, 0x4fc1);
  CALL_C(0x4fc1, objectSetVisible82_hook, 0x1e69, 0x4fc4);
  label_09_093(gb);
}

void soldierSubid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fe0, checkInteractionState_hook, 0x23fe, 0x4fe3);
  if (!(F & FZ)) { CYCT(0x4fe3, 0x4fe5); label_09_095(gb); return; }
  CYC(0x4fe3, 0x4fe5);
  CALL_C(0x4fe5, soldierCheckBeatD6, 0x51f8, 0x4fe8);
  if (!(F & FC)) { CYCT(0x4fe8, 0x4feb); interactionDelete_hook(gb); return; }
  CYC(0x4fe8, 0x4feb);
  CALL_C(0x4feb, soldierInitGraphicsAndLoadScript, 0x51d2, 0x4fee);
  CYC(0x4fee, 0x4ff1); objectSetVisible82_hook(gb);
}

void soldierSubid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ff7, checkInteractionState_hook, 0x23fe, 0x4ffa);
  if (!(F & FZ)) { CYCT(0x4ffa, 0x4ffc); goto state1; }
  CYC(0x4ffa, 0x4ffc);
  CALL_C(0x4ffc, soldierCheckBeatD6, 0x51f8, 0x4fff);
  if (!(F & FC)) { CYCT(0x4fff, 0x5002); interactionDelete_hook(gb); return; }
  CYC(0x4fff, 0x5002);
  CYC(0x5002, 0x5004); A = 0x0b;
  CALL_C(0x5004, checkGlobalFlag_hook, 0x31f3, 0x5007);
  if (!(F & FZ)) { CYCT(0x5007, 0x500a); interactionDelete_hook(gb); return; }
  CYC(0x5007, 0x500a);
  CALL_C(0x500a, soldierInitGraphicsAndLoadScript, 0x51d2, 0x500d);
  CYC(0x500d, 0x500f); E = 0x5c;
  CYC(0x500f, 0x5011); A = 3;
  CYC(0x5011, 0x5012); mem_wr(gb, DE, A);
  CYC(0x5012, 0x5015); objectSetVisiblec2_hook(gb);
  return;

state1:
  CYC(0x5015, 0x5017); E = 0x45;
  CYC(0x5017, 0x5018); A = mem_rd(gb, DE);
  CYC(0x5018, 0x5019); push_effect(gb, 0x5019);
  switch (soldier_jump_table(gb)) {
    case 0x5023: soldierSubid04Substate0(gb); return;
    case 0x503b: soldierSubid04Substate1(gb); return;
    case 0x5050: soldierSubid04Substate2(gb); return;
    case 0x5062: soldierSubid04Substate3(gb); return;
    case 0x5071: soldierSubid04Substate4(gb); return;
    default: HANDOFF(HL);
  }
}
