#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void faroreCheckSecretValidity_hook(GB *gb);
void faroreShowTextForSecretHint_hook(GB *gb);
void faroreSpawnSecretChest_hook(GB *gb);
void faroreGenerateGameTransferSecret_hook(GB *gb);

static uint16_t farore_jumpTable(GB *gb) {
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

static void farore_setVar3f(GB *gb, uint16_t sp0_) {
  CYC(0x4007, 0x4009); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x4009, 0x400a); mem_wr(gb, DE, A);
  RET(0x400a);
}

void faroreCheckSecretValidity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4000, 0x4003); A = mem_rd(gb, 0xcc88);
  CYC(0x4003, 0x4004); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4004, 0x4006);
  } else {
    CYC(0x4004, 0x4006);
    CYC(0x4006, 0x4007); alu_xor(gb, A);
    farore_setVar3f(gb, sp0_);
    return;
  }

  CYC(0x400b, 0x400e); A = mem_rd(gb, 0xcc89);
  CYC(0x400e, 0x4010); alu_swap_a(gb);
  CYC(0x4010, 0x4012); alu_and(gb, 0x03);
  CYC(0x4012, 0x4013); push_effect(gb, 0x4013);
  switch (farore_jumpTable(gb)) {
    case 0x401f: goto jump0;
    case 0x401b: goto jump1Or2;
    case 0x4023: goto jump3;
    default: HANDOFF(HL);
  }

jump1Or2:
  CYC(0x401b, 0x401d); A = 0x04;
  CYC(0x401d, 0x401f);
  farore_setVar3f(gb, sp0_);
  return;

jump0:
  CYC(0x401f, 0x4021); A = 0x03;
  CYC(0x4021, 0x4023);
  farore_setVar3f(gb, sp0_);
  return;

jump3:
  CYC(0x4023, 0x4026); A = mem_rd(gb, 0xcc89);
  CYC(0x4026, 0x4028); alu_and(gb, 0x0f);
  CYC(0x4028, 0x402a); alu_add(gb, 0x5a);
  CYC(0x402a, 0x402b); B = A;
  CALL_C(0x402b, checkGlobalFlag_hook, 0x31f3, 0x402e);
  CYC(0x402e, 0x4030); A = 0x02;
  if (!(F & FZ)) {
    CYCT(0x4030, 0x4032);
    farore_setVar3f(gb, sp0_);
    return;
  }
  CYC(0x4030, 0x4032);
  CYC(0x4032, 0x4033); A = B;
  CYC(0x4033, 0x4035); alu_sub(gb, 0x0a);
  CALL_C(0x4035, checkGlobalFlag_hook, 0x31f3, 0x4038);
  CYC(0x4038, 0x403a); A = 0x01;
  if (!(F & FZ)) {
    CYCT(0x403a, 0x403c);
    farore_setVar3f(gb, sp0_);
    return;
  }
  CYC(0x403a, 0x403c);
  CYC(0x403c, 0x403e); A = 0x05;
  CYC(0x403e, 0x4040);
  farore_setVar3f(gb, sp0_);
}

void faroreShowTextForSecretHint_hook(GB *gb) {
  CYC(0x4040, 0x4043); A = mem_rd(gb, 0xcc89);
  CYC(0x4043, 0x4045); alu_and(gb, 0x0f);
  CYC(0x4045, 0x4047); alu_add(gb, 0x0f);
  CYC(0x4047, 0x4048); C = A;
  CYC(0x4048, 0x404a); B = 0x55;
  CYC(0x404a, 0x404d); showText_hook(gb);
}

void faroreSpawnSecretChest_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x404d, getFreeInteractionSlot_hook, 0x3aef, 0x4050);
  if (!(F & FZ)) {
    RET_TAKEN(0x4050); return;
  }
  CYC(0x4050, 0x4051);
  CYC(0x4051, 0x4053); mem_wr(gb, HL, 0xd9);
  CYC(0x4053, 0x4054); L = alu_inc8(gb, L);
  CYC(0x4054, 0x4057); A = mem_rd(gb, 0xcc89);
  CYC(0x4057, 0x4059); alu_and(gb, 0x0f);
  CYC(0x4059, 0x405a); mem_wr(gb, HL, A);
  CYC(0x405a, 0x405c); L = 0x4b;
  CYC(0x405c, 0x405e); C = 0x75;
  CYC(0x405e, 0x4061); setShortPosition_paramC_hook(gb);
}

void faroreGenerateGameTransferSecret_hook(GB *gb) {
  CYC(0x4061, 0x4064); SET_HL(0x481b);
  CYC(0x4064, 0x4066); E = 0x03;
  CYC(0x4066, 0x4069); interBankCall_hook(gb);
}
