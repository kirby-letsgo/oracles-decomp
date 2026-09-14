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
void soldierSubid04Substate0_hook(GB *gb);
void soldierUpdateAnimationAndRunScript_hook(GB *gb);
void soldierSubid04Substate1_hook(GB *gb);
void soldierSubid04Substate2_hook(GB *gb);
void soldierSubid04Substate3_hook(GB *gb);
void soldierSubid04Substate4_hook(GB *gb);
void soldierSubid05_hook(GB *gb);
void soldierSubid06_hook(GB *gb);
void soldierSubid07_hook(GB *gb);
void soldierSubid08_hook(GB *gb);
void soldierSubid0a_hook(GB *gb);
void soldierSubid0b_hook(GB *gb);
void soldierSubid0d_hook(GB *gb);
void soldierInitGraphics_hook(GB *gb);
void soldierInitGraphicsAndLoadScript_hook(GB *gb);
void soldierCheckBeatD6_hook(GB *gb);
void label_09_090_hook(GB *gb);
void label_09_092_hook(GB *gb);
void label_09_093_hook(GB *gb);
void label_09_094_hook(GB *gb);
void label_09_095_hook(GB *gb);

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

static void soldier_addDoubleIndex(GB *gb, uint16_t returnAddress) {
  push_effect(gb, returnAddress);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
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
    case 0x507d: soldierSubid05_hook(gb); return;
    case 0x50b9: soldierSubid06_hook(gb); return;
    case 0x50f0: soldierSubid07_hook(gb); return;
    case 0x5107: soldierSubid08_hook(gb); return;
    case 0x5121: soldierSubid0a_hook(gb); return;
    case 0x5147: soldierSubid0b_hook(gb); return;
    case 0x4fa2: soldierSubid0c_hook(gb); return;
    case 0x516c: soldierSubid0d_hook(gb); return;
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
  if (!(F & FZ)) { CYCT(0x4f96, 0x4f98); label_09_090_hook(gb); return; }
  CYC(0x4f96, 0x4f98);
  CYC(0x4f98, 0x4f99); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f99, 0x4f9c); interactionDelete_hook(gb); return; }
  CYC(0x4f99, 0x4f9c);
  CYC(0x4f9c, 0x4f9e); soldierSubid0c_hook(gb);
}

void soldierSubid0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fa2, checkInteractionState_hook, 0x23fe, 0x4fa5);
  if (!(F & FZ)) { CYCT(0x4fa5, 0x4fa7); label_09_092_hook(gb); return; }
  CYC(0x4fa5, 0x4fa7);
  CALL_C(0x4fa7, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x4faa);
  label_09_092_hook(gb);
}

void soldierSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fb3, checkInteractionState_hook, 0x23fe, 0x4fb6);
  if (!(F & FZ)) { CYCT(0x4fb6, 0x4fb8); label_09_093_hook(gb); return; }
  CYC(0x4fb6, 0x4fb8);
  CALL_C(0x4fb8, soldierCheckBeatD6_hook, 0x51f8, 0x4fbb);
  if (!(F & FC)) { CYCT(0x4fbb, 0x4fbe); interactionDelete_hook(gb); return; }
  CYC(0x4fbb, 0x4fbe);
  CALL_C(0x4fbe, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x4fc1);
  CALL_C(0x4fc1, objectSetVisible82_hook, 0x1e69, 0x4fc4);
  label_09_093_hook(gb);
}

void soldierSubid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fe0, checkInteractionState_hook, 0x23fe, 0x4fe3);
  if (!(F & FZ)) { CYCT(0x4fe3, 0x4fe5); label_09_095_hook(gb); return; }
  CYC(0x4fe3, 0x4fe5);
  CALL_C(0x4fe5, soldierCheckBeatD6_hook, 0x51f8, 0x4fe8);
  if (!(F & FC)) { CYCT(0x4fe8, 0x4feb); interactionDelete_hook(gb); return; }
  CYC(0x4fe8, 0x4feb);
  CALL_C(0x4feb, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x4fee);
  CYC(0x4fee, 0x4ff1); objectSetVisible82_hook(gb);
}

void soldierSubid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ff7, checkInteractionState_hook, 0x23fe, 0x4ffa);
  if (!(F & FZ)) { CYCT(0x4ffa, 0x4ffc); goto state1; }
  CYC(0x4ffa, 0x4ffc);
  CALL_C(0x4ffc, soldierCheckBeatD6_hook, 0x51f8, 0x4fff);
  if (!(F & FC)) { CYCT(0x4fff, 0x5002); interactionDelete_hook(gb); return; }
  CYC(0x4fff, 0x5002);
  CYC(0x5002, 0x5004); A = 0x0b;
  CALL_C(0x5004, checkGlobalFlag_hook, 0x31f3, 0x5007);
  if (!(F & FZ)) { CYCT(0x5007, 0x500a); interactionDelete_hook(gb); return; }
  CYC(0x5007, 0x500a);
  CALL_C(0x500a, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x500d);
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
    case 0x5023: soldierSubid04Substate0_hook(gb); return;
    case 0x503b: soldierSubid04Substate1_hook(gb); return;
    case 0x5050: soldierSubid04Substate2_hook(gb); return;
    case 0x5062: soldierSubid04Substate3_hook(gb); return;
    case 0x5071: soldierSubid04Substate4_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void soldierSubid04Substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5023, 0x5026); A = mem_rd(gb, 0xcfd1);
  CYC(0x5026, 0x5028); alu_cp(gb, 6);
  if (!(F & FZ)) { CYCT(0x5028, 0x502a); soldierUpdateAnimationAndRunScript_hook(gb); return; } CYC(0x5028, 0x502a);
  CALL_C(0x502a, interactionIncSubstate_hook, 0x23e5, 0x502d);
  CYC(0x502d, 0x502f); L = 0x46;
  CYC(0x502f, 0x5031); mem_wr(gb, HL, 0x1e);
  CYC(0x5031, 0x5032); alu_xor(gb, A);
  CYC(0x5032, 0x5035); interactionSetAnimation_hook(gb);
}

void soldierUpdateAnimationAndRunScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5035, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x5038);
  CYC(0x5038, 0x503b); interactionRunScript_hook(gb);
}

void soldierSubid04Substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x503b, interactionAnimate_hook, 0x261b, 0x503e);
  CALL_C(0x503e, interactionDecCounter1_hook, 0x23cc, 0x5041);
  if (!(F & FZ)) { CYCT(0x5041, 0x5042); ret_effect(gb); return; } CYC(0x5041, 0x5042);
  CALL_C(0x5042, interactionIncSubstate_hook, 0x23e5, 0x5045);
  CYC(0x5045, 0x5048); SET_BC(0xfe40);
  CALL_C(0x5048, objectSetSpeedZ_hook, 0x239d, 0x504b);
  CYC(0x504b, 0x504d); A = 0x53;
  CYC(0x504d, 0x5050); playSound_b00_hook(gb);
}

void soldierSubid04Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5050, 0x5052); C = 0x20;
  CALL_C(0x5052, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5055);
  if (!(F & FZ)) { CYCT(0x5055, 0x5056); ret_effect(gb); return; } CYC(0x5055, 0x5056);
  CALL_C(0x5056, interactionIncSubstate_hook, 0x23e5, 0x5059);
  CYC(0x5059, 0x505b); L = 0x46;
  CYC(0x505b, 0x505d); mem_wr(gb, HL, 8);
  CYC(0x505d, 0x505f); A = 2;
  CYC(0x505f, 0x5062); interactionSetAnimation_hook(gb);
}

void soldierSubid04Substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5062, interactionDecCounter1_hook, 0x23cc, 0x5065);
  if (!(F & FZ)) { CYCT(0x5065, 0x5066); ret_effect(gb); return; } CYC(0x5065, 0x5066);
  CYC(0x5066, 0x5068); L = 0x49;
  CYC(0x5068, 0x506a); mem_wr(gb, HL, 0x10);
  CYC(0x506a, 0x506c); L = 0x50;
  CYC(0x506c, 0x506e); mem_wr(gb, HL, 0x50);
  CYC(0x506e, 0x5071); interactionIncSubstate_hook(gb);
}

void soldierSubid04Substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5071, objectApplySpeed_hook, 0x201d, 0x5074);
  CALL_C(0x5074, objectCheckWithinScreenBoundary_hook, 0x2184, 0x5077);
  if (!(F & FC)) { CYCT(0x5077, 0x507a); interactionDelete_hook(gb); return; } CYC(0x5077, 0x507a);
  CYC(0x507a, 0x507d); interactionAnimateBasedOnSpeed_hook(gb);
}

void soldierSubid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x507d, checkInteractionState_hook, 0x23fe, 0x5080);
  if (!(F & FZ)) { CYCT(0x5080, 0x5082); goto state1; } CYC(0x5080, 0x5082);
  CALL_C(0x5082, soldierCheckBeatD6_hook, 0x51f8, 0x5085);
  if (!(F & FC)) { CYCT(0x5085, 0x5088); interactionDelete_hook(gb); return; } CYC(0x5085, 0x5088);
  CYC(0x5088, 0x508a); A = 0x0b;
  CALL_C(0x508a, checkGlobalFlag_hook, 0x31f3, 0x508d);
  if (!(F & FZ)) { CYCT(0x508d, 0x5090); interactionDelete_hook(gb); return; } CYC(0x508d, 0x5090);
  CALL_C(0x5090, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x5093);
  CYC(0x5093, 0x5094); alu_xor(gb, A);
  CALL_C(0x5094, interactionSetAnimation_hook, 0x262e, 0x5097);
  CYC(0x5097, 0x509a); SET_HL(0xd00d);
  CYC(0x509a, 0x509c); mem_wr(gb, HL, 0x50);
  CYC(0x509c, 0x509f); objectSetVisible82_hook(gb); return;
state1:
  CALL_C(0x509f, objectCheckWithinScreenBoundary_hook, 0x2184, 0x50a2);
  if (!(F & FC)) { CYCT(0x50a2, 0x50a5); interactionDelete_hook(gb); return; } CYC(0x50a2, 0x50a5);
  CALL_C(0x50a5, objectGetTileAtPosition_hook, 0x1444, 0x50a8);
  CYC(0x50a8, 0x50aa); alu_cp(gb, 0xd0);
  CYC(0x50aa, 0x50ac); A = 0x28;
  if (!(F & FZ)) { CYCT(0x50ac, 0x50ae); goto L_50b0; } CYC(0x50ac, 0x50ae);
  CYC(0x50ae, 0x50b0); A = 0x19;
L_50b0:
  CYC(0x50b0, 0x50b2); E = 0x50;
  CYC(0x50b2, 0x50b3); mem_wr(gb, DE, A);
  CALL_C(0x50b3, interactionRunScript_hook, 0x2552, 0x50b6);
  CYC(0x50b6, 0x50b9); interactionAnimate2Times_hook(gb);
}

void soldierSubid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x50b9, checkInteractionState_hook, 0x23fe, 0x50bc);
  if (!(F & FZ)) { CYCT(0x50bc, 0x50be); goto state1; } CYC(0x50bc, 0x50be);
  CALL_C(0x50be, soldierCheckBeatD6_hook, 0x51f8, 0x50c1);
  if (!(F & FC)) { CYCT(0x50c1, 0x50c4); interactionDelete_hook(gb); return; } CYC(0x50c1, 0x50c4);
  CYC(0x50c4, 0x50c6); A = 0x0b;
  CALL_C(0x50c6, checkGlobalFlag_hook, 0x31f3, 0x50c9);
  if (!(F & FZ)) { CYCT(0x50c9, 0x50cc); interactionDelete_hook(gb); return; } CYC(0x50c9, 0x50cc);
  CALL_C(0x50cc, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x50cf);
  CYC(0x50cf, 0x50d0); alu_xor(gb, A);
  CALL_C(0x50d0, interactionSetAnimation_hook, 0x262e, 0x50d3);
  CYC(0x50d3, 0x50d6); objectSetVisible82_hook(gb); return;
state1:
  CALL_C(0x50d6, checkInteractionSubstate_hook, 0x2403, 0x50d9);
  if (!(F & FZ)) { CYCT(0x50d9, 0x50db); goto update; } CYC(0x50d9, 0x50db);
  CYC(0x50db, 0x50de); A = mem_rd(gb, 0xd00b);
  CYC(0x50de, 0x50e0); alu_cp(gb, 0x68);
  if (!(F & FZ)) { CYCT(0x50e0, 0x50e2); goto update; } CYC(0x50e0, 0x50e2);
  CYC(0x50e2, 0x50e3); alu_xor(gb, A);
  CYC(0x50e3, 0x50e6); mem_wr(gb, 0xcbc3, A);
  CYC(0x50e6, 0x50e7); A = alu_inc8(gb, A);
  CYC(0x50e7, 0x50ea); mem_wr(gb, 0xcc8a, A);
  CALL_C(0x50ea, interactionIncSubstate_hook, 0x23e5, 0x50ed);
update:
  CYC(0x50ed, 0x50f0); soldierUpdateAnimationAndRunScript_hook(gb);
}

void soldierSubid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x50f0, checkInteractionState_hook, 0x23fe, 0x50f3);
  if (!(F & FZ)) { CYCT(0x50f3, 0x50f5); goto state1; } CYC(0x50f3, 0x50f5);
  CALL_C(0x50f5, soldierCheckBeatD6_hook, 0x51f8, 0x50f8);
  if (!(F & FC)) { CYCT(0x50f8, 0x50fb); interactionDelete_hook(gb); return; } CYC(0x50f8, 0x50fb);
  CALL_C(0x50fb, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x50fe);
  CYC(0x50fe, 0x5101); objectSetVisible82_hook(gb); return;
state1:
  CALL_C(0x5101, interactionRunScript_hook, 0x2552, 0x5104);
  CYC(0x5104, 0x5107); interactionAnimateAsNpc_hook(gb);
}

void soldierSubid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5107, checkInteractionState_hook, 0x23fe, 0x510a);
  if (!(F & FZ)) { CYCT(0x510a, 0x510c); goto state1; } CYC(0x510a, 0x510c);
  CALL_C(0x510c, soldierInitGraphics_hook, 0x51c9, 0x510f);
  CYC(0x510f, 0x5111); L = 0x5c;
  CYC(0x5111, 0x5113); mem_wr(gb, HL, 3);
  CYC(0x5113, 0x5116); objectSetVisible82_hook(gb); return;
state1:
  CYC(0x5116, 0x5119); SET_HL(0x5613);
  CYC(0x5119, 0x511b); E = 0x15;
  CALL_C(0x511b, interBankCall_hook, 0x008a, 0x511e);
  CYC(0x511e, 0x5121); interactionAnimate_hook(gb);
}

void soldierSubid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5121, checkInteractionState_hook, 0x23fe, 0x5124);
  if (!(F & FZ)) { CYCT(0x5124, 0x5126); goto state1; } CYC(0x5124, 0x5126);
  CALL_C(0x5126, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x5129);
  CYC(0x5129, 0x512b); L = 0x5c;
  CYC(0x512b, 0x512d); mem_wr(gb, HL, 2);
  CYC(0x512d, 0x5130); SET_BC(0x68f0);
  CYC(0x5130, 0x5133); interactionSetPosition_hook(gb); return;
state1:
  CALL_C(0x5133, soldierUpdateAnimationAndRunScript_hook, 0x5035, 0x5136);
  if (!(F & FC)) { CYCT(0x5136, 0x5137); ret_effect(gb); return; } CYC(0x5136, 0x5137);
  CYC(0x5137, 0x513a); SET_HL(0xcc05);
  CYC(0x513a, 0x513c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 2));
  CYC(0x513c, 0x513f); SET_HL(0x5142);
  CYC(0x513f, 0x5142); setWarpDestVariables_hook(gb);
}

void soldierSubid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5147, checkInteractionState_hook, 0x23fe, 0x514a);
  if (!(F & FZ)) { CYCT(0x514a, 0x514d); interactionAnimate_hook(gb); return; } CYC(0x514a, 0x514d);
  CYC(0x514d, 0x514f); A = 0x0b;
  CALL_C(0x514f, checkGlobalFlag_hook, 0x31f3, 0x5152);
  if (!(F & FZ)) { CYCT(0x5152, 0x5155); interactionDelete_hook(gb); return; } CYC(0x5152, 0x5155);
  CYC(0x5155, 0x5157); A = 0x24;
  CALL_C(0x5157, checkTreasureObtained_hook, 0x1748, 0x515a);
  if (!(F & FC)) { CYCT(0x515a, 0x515d); interactionDelete_hook(gb); return; } CYC(0x515a, 0x515d);
  CALL_C(0x515d, soldierInitGraphics_hook, 0x51c9, 0x5160);
  CYC(0x5160, 0x5162); L = 0x5c;
  CYC(0x5162, 0x5164); mem_wr(gb, HL, 2);
  CYC(0x5164, 0x5166); A = 1;
  CALL_C(0x5166, interactionSetAnimation_hook, 0x262e, 0x5169);
  CYC(0x5169, 0x516c); objectSetVisible82_hook(gb);
}

void soldierSubid0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x516c, checkInteractionState_hook, 0x23fe, 0x516f);
  if (!(F & FZ)) { CYCT(0x516f, 0x5171); goto state1; } CYC(0x516f, 0x5171);
  CYC(0x5171, 0x5173); A = 0x14;
  CALL_C(0x5173, checkGlobalFlag_hook, 0x31f3, 0x5176);
  if (F & FZ) { CYCT(0x5176, 0x5179); interactionDelete_hook(gb); return; } CYC(0x5176, 0x5179);
  CALL_C(0x5179, soldierInitGraphicsAndLoadScript_hook, 0x51d2, 0x517c);
  CYC(0x517c, 0x517e); E = 0x43;
  CYC(0x517e, 0x517f); A = mem_rd(gb, DE);
  CYC(0x517f, 0x5181); L = 0x5c;
  CYC(0x5181, 0x5183); mem_wr(gb, HL, 1);
  CYC(0x5183, 0x5185); alu_cp(gb, 7);
  if (F & FC) { CYCT(0x5185, 0x5187); goto L_5188; } CYC(0x5185, 0x5187);
  CYC(0x5187, 0x5188); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_5188:
  CYC(0x5188, 0x518b); SET_BC(0x5197);
  CALL_C(0x518b, addAToBc_hook, 0x006d, 0x518e);
  CYC(0x518e, 0x518f); A = mem_rd(gb, BC);
  CYC(0x518f, 0x5191); L = 0x7b;
  CYC(0x5191, 0x5192); mem_wr(gb, HL, A);
  CALL_C(0x5192, interactionRunScript_hook, 0x2552, 0x5195);
  CYC(0x5195, 0x5197); goto state1;
state1:
  CALL_C(0x51a7, interactionRunScript_hook, 0x2552, 0x51aa);
  if (F & FC) { CYCT(0x51aa, 0x51ad); interactionDelete_hook(gb); return; } CYC(0x51aa, 0x51ad);
  CYC(0x51ad, 0x51af); E = 0x7b;
  CYC(0x51af, 0x51b0); A = mem_rd(gb, DE);
  CYC(0x51b0, 0x51b1); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x51b1, 0x51b3); goto L_51bc; } CYC(0x51b1, 0x51b3);
  CALL_C(0x51b3, interactionRunScript_hook, 0x2552, 0x51b6);
  if (F & FC) { CYCT(0x51b6, 0x51b9); interactionDelete_hook(gb); return; } CYC(0x51b6, 0x51b9);
  CYC(0x51b9, 0x51bc); npcFaceLinkAndAnimate_hook(gb); return;
L_51bc:
  CYC(0x51bc, 0x51be); E = 0x7f;
  CYC(0x51be, 0x51bf); A = mem_rd(gb, DE);
  CYC(0x51bf, 0x51c0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x51c0, 0x51c3); npcFaceLinkAndAnimate_hook(gb); return; } CYC(0x51c0, 0x51c3);
  CALL_C(0x51c3, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x51c6);
  CYC(0x51c6, 0x51c9); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}

void soldierInitGraphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x51c9, interactionInitGraphics_hook, 0x15fb, 0x51cc);
  CALL_C(0x51cc, objectMarkSolidPosition_hook, 0x24f0, 0x51cf);
  CYC(0x51cf, 0x51d2); interactionIncState_hook(gb);
}

void soldierInitGraphicsAndLoadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x51d2, interactionInitGraphics_hook, 0x15fb, 0x51d5);
  CALL_C(0x51d5, objectMarkSolidPosition_hook, 0x24f0, 0x51d8);
  CYC(0x51d8, 0x51da); E = 0x42;
  CYC(0x51da, 0x51db); A = mem_rd(gb, DE);
  CYC(0x51db, 0x51de); SET_HL(0x5207);
  CYC(0x51de, 0x51df); soldier_addDoubleIndex(gb, 0x51df);
  CYC(0x51df, 0x51e0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x51e0, 0x51e1); H = mem_rd(gb, HL);
  CYC(0x51e1, 0x51e2); L = A;
  CALL_C(0x51e2, interactionSetScript_hook, 0x2544, 0x51e5);
  CYC(0x51e5, 0x51e8); interactionIncState_hook(gb);
}

void soldierCheckBeatD6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x51f8, 0x51fa); A = 0x40;
  CALL_C(0x51fa, checkTreasureObtained_hook, 0x1748, 0x51fd);
  if (!(F & FC)) { CYCT(0x51fd, 0x51ff); goto L_5205; } CYC(0x51fd, 0x51ff);
  CALL_C(0x51ff, getHighestSetBit_hook, 0x01ea, 0x5202);
  CYC(0x5202, 0x5204); alu_cp(gb, 5);
  CYC(0x5204, 0x5205); ret_effect(gb);
  return;
L_5205:
  CYC(0x5205, 0x5206); alu_scf(gb);
  CYC(0x5206, 0x5207); ret_effect(gb);
}

void label_09_090_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f9e, 0x4f9f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4f9f, 0x4fa2); interactionDelete_hook(gb); return; }
  CYC(0x4f9f, 0x4fa2);
  soldierSubid0c_hook(gb);
}

void label_09_092_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4faa, interactionRunScript_hook, 0x2552, 0x4fad);
  if (F & FC) { CYCT(0x4fad, 0x4fb0); interactionDelete_hook(gb); return; }
  CYC(0x4fad, 0x4fb0);
  CYC(0x4fb0, 0x4fb3); npcFaceLinkAndAnimate_hook(gb);
}

void label_09_093_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4fc4, objectCheckWithinScreenBoundary_hook, 0x2184, 0x4fc7);
  if (!(F & FC)) { CYCT(0x4fc7, 0x4fca); interactionDelete_hook(gb); return; }
  CYC(0x4fc7, 0x4fca);
  CALL_C(0x4fca, soldierUpdateAnimationAndRunScript_hook, 0x5035, 0x4fcd);
  CALL_C(0x4fcd, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x4fd0);
  CYC(0x4fd0, 0x4fd2); A = 0x10;
  CALL_C(0x4fd2, checkGlobalFlag_hook, 0x31f3, 0x4fd5);
  if (F & FZ) { CYCT(0x4fd5, 0x4fd7); label_09_094_hook(gb); return; }
  CYC(0x4fd5, 0x4fd7);
  CYC(0x4fd7, 0x4fd9); A = 0x0b;
  CALL_C(0x4fd9, checkGlobalFlag_hook, 0x31f3, 0x4fdc);
  if (F & FZ) { CYCT(0x4fdc, 0x4fdd); ret_effect(gb); return; }
  CYC(0x4fdc, 0x4fdd);
  label_09_094_hook(gb);
}

void label_09_094_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4fdd, 0x4fe0); objectPreventLinkFromPassing_hook(gb);
}

void label_09_095_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4ff1, interactionRunScript_hook, 0x2552, 0x4ff4);
  CYC(0x4ff4, 0x4ff7); interactionAnimate_hook(gb);
}
