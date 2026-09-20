#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

static uint16_t breakTileDebris_jumpTable(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void breakTileDebris_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode0c@doSpecializedInitialization: init for interactions 0 (underwater bush
// breaking) and $0a (shovel debris). Reached only by a static `call` from interactionCode00
// itself; not independently hookable.
static void breakTileDebris_doSpecializedInitialization(GB *gb, uint16_t sp0_) {
  CYC(0x407e, 0x4080); E = INTERACTION_BASE + OBJ_ID;
  CYC(0x4080, 0x4081); A = mem_rd(gb, DE);
  CYC(0x4081, 0x4082); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4082, 0x4084);
  } else {
    CYC(0x4082, 0x4084);
    CYC(0x4084, 0x4086); alu_cp(gb, 0x0a);
    if (F & FZ) {
      CYC(0x4086, 0x4087);
    } else {
      CYCT(0x4086, 0x4087); ret_effect(gb); return;
    }
    // @interac0A
    CYC(0x4087, 0x408a); SET_BC(0xfdc0);
    CALL_C(0x408a, objectSetSpeedZ_hook, 0x239d, 0x408d);
    CYC(0x408d, 0x408f); E = INTERACTION_BASE + OBJ_DIRECTION;
    CYC(0x408f, 0x4090); A = mem_rd(gb, DE);
    CYC(0x4090, 0x4093); interactionSetAnimation_hook(gb);
    return;
  }
  // @interac00
  CYC(0x4093, 0x4096); A = mem_rd(gb, wTilesetFlags);
  CYC(0x4096, 0x4098); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x4098, 0x409a); goto notUnderwater;
  }
  CYC(0x4098, 0x409a);
  CYC(0x409a, 0x409c); A = 0x0e;
  CYC(0x409c, 0x409e); goto stored;
notUnderwater:
  CYC(0x409e, 0x40a1); A = mem_rd(gb, wGrassAnimationModifier);
  CYC(0x40a1, 0x40a3); alu_and(gb, 0x03);
  CYC(0x40a3, 0x40a5); alu_or(gb, 0x08);
stored:
  CYC(0x40a5, 0x40a7); E = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x40a7, 0x40a8); mem_wr(gb, DE, A);
  CYC(0x40a8, 0x40a9); E = alu_inc8(gb, E);
  CYC(0x40a9, 0x40aa); mem_wr(gb, DE, A);
  CYC(0x40aa, 0x40ab); ret_effect(gb);
}

void interactionCode00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4000, 0x4002); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4002, 0x4003); A = mem_rd(gb, DE);
  CYC(0x4003, 0x4004); push_effect(gb, 0x4004);
  switch (breakTileDebris_jumpTable(gb)) {
    case 0x4008: goto state0;
    case 0x4051: goto state1;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x4008, 0x400a); A = 0x01;
  CYC(0x400a, 0x400b); mem_wr(gb, DE, A);
  CALL_C(0x400b, interactionInitGraphics_hook, 0x15fb, 0x400e);
  CYC(0x400e, 0x400f); H = D;
  CYC(0x400f, 0x4011); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x4011, 0x4013); mem_wr(gb, HL, 0x14);
  CYC(0x4013, 0x4015); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4015, 0x4017); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) {
    CALL_C_CC(0x4017, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x401a);
  } else {
    CYC(0x4017, 0x401a);
  }
  CYC(0x401a, 0x401d); push_effect(gb, 0x401d); breakTileDebris_doSpecializedInitialization(gb, sp0_);
  CYC(0x401d, 0x401f); E = INTERACTION_BASE + OBJ_ID;
  CYC(0x401f, 0x4020); A = mem_rd(gb, DE);
  CYC(0x4020, 0x4023); SET_HL(0x4037);
  CYC(0x4023, 0x4024); breakTileDebris_addDoubleIndex(gb, 0x4024);
  CYC(0x4024, 0x4026); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4026, 0x4027); A = mem_rd(gb, DE);
  CYC(0x4027, 0x4028); alu_rlca(gb);
  CYC(0x4028, 0x4029); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4029, 0x402a); E = mem_rd(gb, HL);
  if (!(F & FC)) {
    CALL_C_CC(0x402a, playSound_b00_hook, 0x0c98, 0x402d);
  } else {
    CYC(0x402a, 0x402d);
  }
  CYC(0x402d, 0x402e); A = E;
  CYC(0x402e, 0x402f); push_effect(gb, 0x402f);
  switch (breakTileDebris_jumpTable(gb)) {
    case 0x1e57: objectSetVisible80_hook(gb); return;
    case 0x1e60: objectSetVisible81_hook(gb); return;
    case 0x1e69: objectSetVisible82_hook(gb); return;
    case 0x1e72: objectSetVisible83_hook(gb); return;
    default: HANDOFF(HL);
  }

state1:
  CYC(0x4051, 0x4052); H = D;
  CYC(0x4052, 0x4054); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4054, 0x4056); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYC(0x4056, 0x4059);
  } else {
    CYCT(0x4056, 0x4059); interactionDelete_hook(gb); return;
  }
  CYC(0x4059, 0x405b); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x405b, 0x405d); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x405d, 0x405f); goto checkId;
  }
  CYC(0x405d, 0x405f);
  CYC(0x405f, 0x4062); A = mem_rd(gb, wFrameCounter);
  CYC(0x4062, 0x4063); alu_xor(gb, D);
  CYC(0x4063, 0x4064); alu_rrca(gb);
  CYC(0x4064, 0x4066); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x4066, 0x4068); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 7));
  if (!(F & FC)) {
    CYCT(0x4068, 0x406a); goto checkId;
  }
  CYC(0x4068, 0x406a);
  CYC(0x406a, 0x406c); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 7));

checkId:
  CYC(0x406c, 0x406e); E = INTERACTION_BASE + OBJ_ID;
  CYC(0x406e, 0x406f); A = mem_rd(gb, DE);
  CYC(0x406f, 0x4071); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYC(0x4071, 0x4073);
    CYC(0x4073, 0x4075); C = 0x60;
    CALL_C(0x4075, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4078);
    CALL_C(0x4078, objectApplySpeed_hook, 0x201d, 0x407b);
  } else {
    CYCT(0x4071, 0x4073);
  }
  CYC(0x407b, 0x407e); interactionAnimate_hook(gb);
}
