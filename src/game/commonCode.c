#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

void linkAdjustGivenAngleInSidescrollingArea_hook(GB *gb);
void companionTryToBreakTileFromMoving_hook(GB *gb);
void companionCalculateAdjacentWallsBitset_hook(GB *gb);
void checkCollisionForCompanion_hook(GB *gb);
void specialObjectGetRelativeTileFromHl_hook(GB *gb);
void specialObjectCheckFacingWall_hook(GB *gb);

static void common_code_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void common_code_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t common_code_jump_table(GB *gb) {
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

void specialObjectSetOamVariables_hook(GB *gb) {
  CYC(0x41f7, 0x41f9); E = 0x32;
  CYC(0x41f9, 0x41fb); A = 0xff;
  CYC(0x41fb, 0x41fc); mem_wr(gb, DE, A);
  CYC(0x41fc, 0x41fe); E = 0x01;
  CYC(0x41fe, 0x41ff); A = mem_rd(gb, DE);
  CYC(0x41ff, 0x4202); SET_HL(0x420d);
  CYC(0x4202, 0x4203); common_code_add_double_index(gb, 0x4203);
  CYC(0x4203, 0x4205); E = 0x1d;
  CYC(0x4205, 0x4206); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4206, 0x4207); mem_wr(gb, DE, A);
  CYC(0x4207, 0x4208); E = alu_dec8(gb, E);
  CYC(0x4208, 0x4209); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4209, 0x420a); mem_wr(gb, DE, A);
  CYC(0x420a, 0x420b); E = alu_dec8(gb, E);
  CYC(0x420b, 0x420c); mem_wr(gb, DE, A);
  CYC(0x420c, 0x420d); ret_effect(gb);
}

void dealSpikeDamageToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4235, 0x4238); A = W8(wLinkRidingObject);
  CYC(0x4238, 0x4239); B = A;
  CYC(0x4239, 0x423a); H = D;
  CYC(0x423a, 0x423c); L = 0x2b;
  CYC(0x423c, 0x423d); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x423d, 0x423e); ret_effect(gb); return;
  }
  CYC(0x423d, 0x423e);
  CYC(0x423e, 0x4240); mem_wr(gb, HL, 0x28);
  CYC(0x4240, 0x4242); A = 0x1d;
  CALL_C(0x4242, cpActiveRing_hook, 0x23b0, 0x4245);
  CYC(0x4245, 0x4247); A = 0xfc;
  if (F & FZ) {
    CYC(0x4247, 0x4249);
    CYC(0x4249, 0x424b); A = alu_sra(gb, A);
  } else {
    CYCT(0x4247, 0x4249);
  }
  CYC(0x424b, 0x424d); L = 0x25;
  CYC(0x424d, 0x424e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x424e, 0x424f); mem_wr(gb, HL, A);
  CYC(0x424f, 0x4251); L = 0x2a;
  CYC(0x4251, 0x4253); mem_wr(gb, HL, 0x80);
  CYC(0x4253, 0x4255); L = 0x2d;
  CYC(0x4255, 0x4257); A = 0x0a;
  CYC(0x4257, 0x4258); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4258, 0x4259); mem_wr(gb, HL, A);
  CYC(0x4259, 0x425b); E = 0x09;
  CYC(0x425b, 0x425c); A = mem_rd(gb, DE);
  CYC(0x425c, 0x425e); alu_xor(gb, 0x10);
  CYC(0x425e, 0x4260); L = 0x2c;
  CYC(0x4260, 0x4261); mem_wr(gb, HL, A);
  CYC(0x4261, 0x4263); A = 0x5f;
  CALL_C(0x4263, playSound_b00_hook, 0x0c98, 0x4266);
  CYC(0x4266, 0x4268);
  linkApplyDamage_b5_hook(gb);
}

void updateLinkDamageTaken_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4268, 0x426b); SET_HL(0x4668);
  CYC(0x426b, 0x426d); E = 0x06;
  CALL_C(0x426d, interBankCall_hook, 0x008a, 0x4270);
  linkApplyDamage_b5_hook(gb);
}

void updateLinkInvincibilityCounter_hook(GB *gb) {
  CYC(0x4279, 0x427c); SET_HL(w1Link_invincibilityCounter);
  CYC(0x427c, 0x427d); A = mem_rd(gb, HL);
  CYC(0x427d, 0x427e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x427e, 0x427f); ret_effect(gb); return;
  }
  CYC(0x427e, 0x427f);
  CYC(0x427f, 0x4281); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x4281, 0x4283);
    goto increment;
  }
  CYC(0x4281, 0x4283);
  CYC(0x4283, 0x4284); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(0x4284, 0x4286);
    goto normal_flags;
  }
  CYC(0x4284, 0x4286);
  CYC(0x4286, 0x4289); A = W8(wFrameCounter);
  CYC(0x4289, 0x428b); alu_bit(gb, 2, A);
  if (!(F & FZ)) {
    CYCT(0x428b, 0x428d);
    goto normal_flags;
  }
  CYC(0x428b, 0x428d);
  CYC(0x428d, 0x428f); L = 0x1c;
  CYC(0x428f, 0x4291); mem_wr(gb, HL, 0x0d);
  CYC(0x4291, 0x4292); ret_effect(gb);
  return;

increment:
  CYC(0x4292, 0x4293); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

normal_flags:
  CYC(0x4293, 0x4295); L = 0x1b;
  CYC(0x4295, 0x4296); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4296, 0x4297); mem_wr(gb, HL, A);
  CYC(0x4297, 0x4298); ret_effect(gb);
}

void sidescrollUpdateActiveTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4298, objectGetTileAtPosition_hook, 0x1444, 0x429b);
  CYC(0x429b, 0x429e); W8(wActiveTileIndex) = A;
  CYC(0x429e, 0x42a1); SET_HL(0x7c6e);
  CALL_C(0x42a1, lookupCollisionTable_hook, 0x1e1f, 0x42a4);
  CYC(0x42a4, 0x42a7); W8(wActiveTileType) = A;
  CYC(0x42a7, 0x42aa); SET_BC(0x0800);
  CALL_C(0x42aa, objectGetRelativeTile_hook, 0x1435, 0x42ad);
  CYC(0x42ad, 0x42b0); SET_HL(0x7c6e);
  CALL_C(0x42b0, lookupCollisionTable_hook, 0x1e1f, 0x42b3);
  CYC(0x42b3, 0x42b6); W8(wLastActiveTileType) = A;
  CYC(0x42b6, 0x42b7); ret_effect(gb);
}

static void link_apply_tile_types_adjust_conveyor(GB *gb) {
  CYC(0x43e9, 0x43eb); A = 0x01;
  CYC(0x43eb, 0x43ee); W8(wcc92) = A;
  CYC(0x43ee, 0x43f1); A = W8(wActiveTileType);
  CYC(0x43f1, 0x43f2); alu_sub(gb, C);
  CYC(0x43f2, 0x43f5); SET_HL(0x43fa);
  CYC(0x43f5, 0x43f6); common_code_add_a_to_hl(gb, 0x43f6);
  CYC(0x43f6, 0x43f7); C = mem_rd(gb, HL);
  CYC(0x43f7, 0x43fa);
  specialObjectUpdatePositionGivenVelocity(gb);
}

void linkApplyTileTypes_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42b7, 0x42b8); alu_xor(gb, A);
  CYC(0x42b8, 0x42bb); W8(wIsTileSlippery) = A;
  CYC(0x42bb, 0x42be); A = W8(wLinkInAir);
  CYC(0x42be, 0x42bf); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x42bf, 0x42c2);
    goto normal;
  }
  CYC(0x42bf, 0x42c2);
  CYC(0x42c2, 0x42c5); W8(wLinkRaisedFloorOffset) = A;
  CYC(0x42c5, 0x42c8); push_effect(gb, 0x42c8);
  CYC(0x4406, 0x4409); SET_BC(0x0500);
  CALL_C(0x4409, objectGetRelativeTile_hook, 0x1435, 0x440c);
  CYC(0x440c, 0x440d); C = A;
  CYC(0x440d, 0x440e); B = L;
  CYC(0x440e, 0x4411); SET_HL(wActiveTilePos);
  CYC(0x4411, 0x4412); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4412, 0x4413); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x4413, 0x4415);
    goto update_active_tile;
  }
  CYC(0x4413, 0x4415);
  CYC(0x4415, 0x4416); A = mem_rd(gb, HL);
  CYC(0x4416, 0x4417); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x4417, 0x4419);
    goto increment_standing_counter;
  }
  CYC(0x4417, 0x4419);

update_active_tile:
  CYC(0x4419, 0x441b); L = 0x99;
  CYC(0x441b, 0x441c); A = mem_rd(gb, HL);
  CYC(0x441c, 0x441d); mem_wr(gb, HL, B);
  CYC(0x441d, 0x441e); B = A;
  CYC(0x441e, 0x441f); L = alu_inc8(gb, L);
  CYC(0x441f, 0x4420); mem_wr(gb, HL, C);
  CYC(0x4420, 0x4421); L = alu_inc8(gb, L);
  CYC(0x4421, 0x4423); mem_wr(gb, HL, 0x00);

increment_standing_counter:
  CYC(0x4423, 0x4425); L = 0x9b;
  CYC(0x4425, 0x4426); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4426, 0x4427); L = alu_inc8(gb, L);
  CYC(0x4427, 0x4428); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4428, 0x4429); mem_wr(gb, HL, A);
  CYC(0x4429, 0x442a); A = C;
  CYC(0x442a, 0x442d); SET_HL(0x7c6e);
  CYC(0x442d, 0x4430);
  lookupCollisionTable_hook(gb);
  CYC(0x42c8, 0x42cb); W8(wActiveTileType) = A;
  CYC(0x42cb, 0x42cc); push_effect(gb, 0x42cc);
  switch (common_code_jump_table(gb)) {
    case 0x42fe: goto raisable_floor;
    case 0x4303: goto normal;
    case 0x430e: goto puddle;
    case 0x4322: goto stump;
    case 0x432d: goto vines;
    case 0x4335: goto not_swimming;
    case 0x433a: goto cracked_floor;
    case 0x4355: goto hole;
    case 0x4386: goto ice;
    case 0x4394: goto cracked_ice;
    case 0x4395: goto swimming;
    case 0x43b8: goto lava;
    case 0x43d7: goto conveyor;
    case 0x43fe: goto current;
    case 0x4235: dealSpikeDamageToLink_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

raisable_floor:
  CYC(0x42fe, 0x4300); A = 0xfd;
  CYC(0x4300, 0x4303); W8(wLinkRaisedFloorOffset) = A;

normal:
  CYC(0x4303, 0x4304); alu_xor(gb, A);
  CYC(0x4304, 0x4307); W8(wActiveTileType) = A;
  CYC(0x4307, 0x430a); W8(wStandingOnTileCounter) = A;
  CYC(0x430a, 0x430d); W8(wLinkSwimmingState) = A;
  CYC(0x430d, 0x430e); ret_effect(gb);
  return;

puddle:
  CYC(0x430e, 0x430f); H = D;
  CYC(0x430f, 0x4311); L = 0x21;
  CYC(0x4311, 0x4313); alu_bit(gb, 5, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4313, 0x4315);
    goto normal;
  }
  CYC(0x4313, 0x4315);
  CYC(0x4315, 0x4317); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 5)));
  CYC(0x4317, 0x431a); A = W8(wLinkImmobilized);
  CYC(0x431a, 0x431b); alu_or(gb, A);
  CYC(0x431b, 0x431d); A = 0x87;
  if (F & FZ) {
    CALL_C_CC(0x431d, playSound_b00_hook, 0x0c98, 0x4320);
  } else {
    CYC(0x431d, 0x4320);
  }
  CYC(0x4320, 0x4322);
  goto normal;

stump:
  CYC(0x4322, 0x4323); H = D;
  CYC(0x4323, 0x4325); L = 0x33;
  CYC(0x4325, 0x4327); mem_wr(gb, HL, 0xff);
  CYC(0x4327, 0x4329); L = 0x24;
  CYC(0x4329, 0x432b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x432b, 0x432d);
  goto not_swimming;

vines:
  CALL_C(0x432d, dropLinkHeldItem_hook, 0x2c43, 0x4330);
  CYC(0x4330, 0x4332); A = 0xff;
  CYC(0x4332, 0x4335); W8(wLinkClimbingVine) = A;

not_swimming:
  CYC(0x4335, 0x4336); alu_xor(gb, A);
  CYC(0x4336, 0x4339); W8(wLinkSwimmingState) = A;
  CYC(0x4339, 0x433a); ret_effect(gb);
  return;

cracked_floor:
  CYC(0x433a, 0x433c); A = 0x22;
  CALL_C(0x433c, cpActiveRing_hook, 0x23b0, 0x433f);
  if (F & FZ) {
    CYCT(0x433f, 0x4341);
    goto normal;
  }
  CYC(0x433f, 0x4341);
  CYC(0x4341, 0x4344); A = W8(wStandingOnTileCounter);
  CYC(0x4344, 0x4346); alu_cp(gb, 0x20);
  if (F & FC) {
    CYCT(0x4346, 0x4348);
    goto not_swimming;
  }
  CYC(0x4346, 0x4348);
  CYC(0x4348, 0x434b); A = W8(wActiveTilePos);
  CYC(0x434b, 0x434c); C = A;
  CYC(0x434c, 0x434e); A = 0xf3;
  CALL_C(0x434e, breakCrackedFloor_hook, 0x2291, 0x4351);
  CYC(0x4351, 0x4352); alu_xor(gb, A);
  CYC(0x4352, 0x4355); W8(wStandingOnTileCounter) = A;

hole:
  CYC(0x4355, 0x4358); A = W8(wTilesetFlags);
  CYC(0x4358, 0x435a); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x435a, 0x435c);
    goto normal;
  }
  CYC(0x435a, 0x435c);
  CYC(0x435c, 0x435d); alu_xor(gb, A);
  CYC(0x435d, 0x4360); W8(wLinkSwimmingState) = A;
  CYC(0x4360, 0x4363); A = W8(wLinkRidingObject);
  CYC(0x4363, 0x4364); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4364, 0x4366);
    goto normal;
  }
  CYC(0x4364, 0x4366);
  CYC(0x4366, 0x4369); A = W8(wMagnetGloveState);
  CYC(0x4369, 0x436b); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x436b, 0x436d);
    goto normal;
  }
  CYC(0x436b, 0x436d);
  CYC(0x436d, 0x4370); SET_HL(wLastActiveTileType);
  CYC(0x4370, 0x4371); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x4371, 0x4372); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4372, 0x4374);
    goto pull_into_hole;
  }
  CYC(0x4372, 0x4374);
  CYC(0x4374, 0x4376); L = 0x99;
  CYC(0x4376, 0x4377); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4377, 0x4378); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x4378, 0x437a);
    goto pull_into_hole;
  }
  CYC(0x4378, 0x437a);
  CYC(0x437a, 0x437b); L = alu_inc8(gb, L);
  CYC(0x437b, 0x437d); A = 0x0e;
  CYC(0x437d, 0x437e); mem_wr(gb, HL, A);

pull_into_hole:
  CYC(0x437e, 0x4380); A = 0x80;
  CYC(0x4380, 0x4383); W8(wcc92) = A;
  CYC(0x4383, 0x4386);
  linkPullIntoHole(gb);
  return;

ice:
  CYC(0x4386, 0x4388); A = 0x21;
  CALL_C(0x4388, cpActiveRing_hook, 0x23b0, 0x438b);
  if (F & FZ) {
    CYCT(0x438b, 0x438d);
    goto not_swimming;
  }
  CYC(0x438b, 0x438d);
  CYC(0x438d, 0x4390); SET_HL(wIsTileSlippery);
  CYC(0x4390, 0x4392); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(0x4392, 0x4394);
  goto not_swimming;

cracked_ice:
  CYC(0x4394, 0x4395); ret_effect(gb);
  return;

swimming:
  CYC(0x4395, 0x4398); A = W8(wLinkRidingObject);
  CYC(0x4398, 0x4399); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4399, 0x439c);
    goto normal;
  }
  CYC(0x4399, 0x439c);
  CYC(0x439c, 0x439f); A = W8(wLinkSwimmingState);
  CYC(0x439f, 0x43a0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x43a0, 0x43a1); ret_effect(gb); return;
  }
  CYC(0x43a0, 0x43a1);
  CYC(0x43a1, 0x43a4); A = mem_rd(gb, w1Link_var2f);
  CYC(0x43a4, 0x43a6); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x43a6, 0x43a7); ret_effect(gb); return;
  }
  CYC(0x43a6, 0x43a7);
  CYC(0x43a7, 0x43a8); alu_xor(gb, A);
  CYC(0x43a8, 0x43aa); E = 0x35;
  CYC(0x43aa, 0x43ab); mem_wr(gb, DE, A);
  CYC(0x43ab, 0x43ad); E = 0x2d;
  CYC(0x43ad, 0x43ae); mem_wr(gb, DE, A);
  CYC(0x43ae, 0x43af); A = alu_inc8(gb, A);
  CYC(0x43af, 0x43b2); W8(wLinkSwimmingState) = A;
  CYC(0x43b2, 0x43b4); A = 0x80;
  CYC(0x43b4, 0x43b7); W8(wcc92) = A;
  CYC(0x43b7, 0x43b8); ret_effect(gb);
  return;

lava:
  CYC(0x43b8, 0x43bb); A = W8(wLinkRidingObject);
  CYC(0x43bb, 0x43bc); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x43bc, 0x43bf);
    goto normal;
  }
  CYC(0x43bc, 0x43bf);
  CYC(0x43bf, 0x43c1); A = 0x80;
  CYC(0x43c1, 0x43c4); W8(wcc92) = A;
  CYC(0x43c4, 0x43c6); E = 0x2d;
  CYC(0x43c6, 0x43c7); alu_xor(gb, A);
  CYC(0x43c7, 0x43c8); mem_wr(gb, DE, A);
  CYC(0x43c8, 0x43cb); A = W8(wLinkSwimmingState);
  CYC(0x43cb, 0x43cc); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x43cc, 0x43cd); ret_effect(gb); return;
  }
  CYC(0x43cc, 0x43cd);
  CYC(0x43cd, 0x43ce); alu_xor(gb, A);
  CYC(0x43ce, 0x43d0); E = 0x35;
  CYC(0x43d0, 0x43d1); mem_wr(gb, DE, A);
  CYC(0x43d1, 0x43d3); A = 0x41;
  CYC(0x43d3, 0x43d6); W8(wLinkSwimmingState) = A;
  CYC(0x43d6, 0x43d7); ret_effect(gb);
  return;

conveyor:
  CYC(0x43d7, 0x43da); A = W8(wLinkRidingObject);
  CYC(0x43da, 0x43db); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x43db, 0x43de);
    goto normal;
  }
  CYC(0x43db, 0x43de);
  CYC(0x43de, 0x43e0); A = 0x23;
  CALL_C(0x43e0, cpActiveRing_hook, 0x23b0, 0x43e3);
  if (F & FZ) {
    CYCT(0x43e3, 0x43e6);
    goto normal;
  }
  CYC(0x43e3, 0x43e6);
  CYC(0x43e6, 0x43e9); SET_BC(0x1409);
  link_apply_tile_types_adjust_conveyor(gb);
  return;

current:
  CYC(0x43fe, 0x4401); SET_BC(0x1e12);
  CYC(0x4401, 0x4404); push_effect(gb, 0x4404);
  link_apply_tile_types_adjust_conveyor(gb);
  CYC(0x4404, 0x4406);
  goto swimming;
}

void linkAdjustAngleInSidescrollingArea_hook(GB *gb) {
  CYC(0x4430, 0x4432); L = 0x09;
  linkAdjustGivenAngleInSidescrollingArea_hook(gb);
}

void linkAdjustGivenAngleInSidescrollingArea_hook(GB *gb) {
  CYC(0x4432, 0x4433); H = D;
  CYC(0x4433, 0x4434); E = L;
  CYC(0x4434, 0x4437); A = W8(wTilesetFlags);
  CYC(0x4437, 0x4439); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x4439, 0x443a); ret_effect(gb); return;
  }
  CYC(0x4439, 0x443a);
  CYC(0x443a, 0x443c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x443c, 0x443d); ret_effect(gb); return;
  }
  CYC(0x443c, 0x443d);
  CYC(0x443d, 0x443e); A = mem_rd(gb, HL);
  CYC(0x443e, 0x4441); SET_HL(0x4445);
  CYC(0x4441, 0x4442); common_code_add_a_to_hl(gb, 0x4442);
  CYC(0x4442, 0x4443); A = mem_rd(gb, HL);
  CYC(0x4443, 0x4444); mem_wr(gb, DE, A);
  CYC(0x4444, 0x4445); ret_effect(gb);
}

void companionPreventLinkFromPassing_noExtraChecks_hook(GB *gb) {
  CYC(0x4465, 0x4468); SET_HL(w1Link);
  CYC(0x4468, 0x446b);
  preventObjectHFromPassingObjectD_hook(gb);
}

void companionUpdateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x446b, companionCalculateAdjacentWallsBitset_hook, 0x4486, 0x446e);
  CALL_C(0x446e, specialObjectUpdatePosition, 0x5d97, 0x4471);
  CYC(0x4471, 0x4472); H = D;
  CYC(0x4472, 0x4474); L = 0x0f;
  CYC(0x4474, 0x4475); A = mem_rd(gb, HL);
  CYC(0x4475, 0x4476); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4476, 0x4477); ret_effect(gb); return;
  }
  CYC(0x4476, 0x4477);
  companionTryToBreakTileFromMoving_hook(gb);
}

void companionTryToBreakTileFromMoving_hook(GB *gb) {
  CYC(0x4477, 0x4478); H = D;
  CYC(0x4478, 0x447a); L = 0x0b;
  CYC(0x447a, 0x447b); A = mem_rd(gb, HL);
  CYC(0x447b, 0x447d); alu_add(gb, 0x05);
  CYC(0x447d, 0x447e); B = A;
  CYC(0x447e, 0x4480); L = 0x0d;
  CYC(0x4480, 0x4481); C = mem_rd(gb, HL);
  CYC(0x4481, 0x4483); A = 0x13;
  CYC(0x4483, 0x4486);
  tryToBreakTile_hook(gb);
}

void companionCalculateAdjacentWallsBitset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4486, 0x4488); E = 0x33;
  CYC(0x4488, 0x4489); alu_xor(gb, A);
  CYC(0x4489, 0x448a); mem_wr(gb, DE, A);
  CYC(0x448a, 0x448b); H = D;
  CYC(0x448b, 0x448d); L = 0x0b;
  CYC(0x448d, 0x448e); B = mem_rd(gb, HL);
  CYC(0x448e, 0x4490); L = 0x0d;
  CYC(0x4490, 0x4491); C = mem_rd(gb, HL);
  CYC(0x4491, 0x4493); A = 0x01;
  CYC(0x4493, 0x4495); H8(hFF8B) = A;
  CYC(0x4495, 0x4498); SET_HL(0x44ae);

  do {
    CYC(0x4498, 0x4499); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4499, 0x449a); alu_add(gb, B);
    CYC(0x449a, 0x449b); B = A;
    CYC(0x449b, 0x449c); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x449c, 0x449d); alu_add(gb, C);
    CYC(0x449d, 0x449e); C = A;
    CYC(0x449e, 0x449f); push_effect(gb, HL);
    CALL_C(0x449f, checkCollisionForCompanion_hook, 0x44be, 0x44a2);
    CYC(0x44a2, 0x44a3); SET_HL(pop_effect(gb));
    CYC(0x44a3, 0x44a5); A = H8(hFF8B);
    CYC(0x44a5, 0x44a6); alu_rla(gb);
    CYC(0x44a6, 0x44a8); H8(hFF8B) = A;
    if (!(F & FC)) CYCT(0x44a8, 0x44aa);
    else CYC(0x44a8, 0x44aa);
  } while (!(F & FC));

  CYC(0x44aa, 0x44ac); E = 0x33;
  CYC(0x44ac, 0x44ad); mem_wr(gb, DE, A);
  CYC(0x44ad, 0x44ae); ret_effect(gb);
}

void checkCollisionForCompanion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44be, getTileAtPosition_hook, 0x1447, 0x44c1);
  CYC(0x44c1, 0x44c2); A = mem_rd(gb, HL);
  CYC(0x44c2, 0x44c4); alu_cp(gb, 0xd6);
  if (F & FZ) {
    CYCT(0x44c4, 0x44c6);
    goto set_collision;
  }
  CYC(0x44c4, 0x44c6);
  CYC(0x44c6, 0x44c8); alu_cp(gb, 0xd5);
  if (F & FZ) {
    CYCT(0x44c8, 0x44ca);
    goto set_collision;
  }
  CYC(0x44c8, 0x44ca);
  CYC(0x44ca, 0x44cc); alu_cp(gb, 0xd4);
  CYC(0x44cc, 0x44ce); A = 0x03;
  if (F & FZ) {
    CYCT(0x44ce, 0x44d1);
    checkGivenCollision_allowHoles_hook(gb);
    return;
  }
  CYC(0x44ce, 0x44d1);
  CYC(0x44d1, 0x44d3); E = 0x01;
  CYC(0x44d3, 0x44d4); A = mem_rd(gb, DE);
  CYC(0x44d4, 0x44d6); alu_cp(gb, 0x0b);
  if (!(F & FZ)) {
    CYCT(0x44d6, 0x44d8);
    goto not_ricky;
  }
  CYC(0x44d6, 0x44d8);
  CYC(0x44d8, 0x44da); E = 0x0f;
  CYC(0x44da, 0x44db); A = mem_rd(gb, DE);
  CYC(0x44db, 0x44dd); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x44dd, 0x44df);
    goto check_collision;
  }
  CYC(0x44dd, 0x44df);
  CYC(0x44df, 0x44e0); A = mem_rd(gb, HL);
  CYC(0x44e0, 0x44e2);
  goto check_collision;

not_ricky:
  CYC(0x44e2, 0x44e4); alu_cp(gb, 0x0c);
  if (!(F & FZ)) {
    CYCT(0x44e4, 0x44e6);
    goto check_collision;
  }
  CYC(0x44e4, 0x44e6);
  CYC(0x44e6, 0x44e7); A = mem_rd(gb, HL);
  CYC(0x44e7, 0x44e9); alu_cp(gb, 0xfe);
  if (!(F & FC)) {
    CYCT(0x44e9, 0x44ea); ret_effect(gb); return;
  }
  CYC(0x44e9, 0x44ea);
  CYC(0x44ea, 0x44ec);
  goto check_collision;

set_collision:
  CYC(0x44ec, 0x44ed); alu_scf(gb);
  CYC(0x44ed, 0x44ee); ret_effect(gb);
  return;

check_collision:
  CYC(0x44ee, 0x44f1);
  checkCollisionPosition_disallowSmallBridges_hook(gb);
}

void specialObjectGetRelativeTileWithDirectionTable_hook(GB *gb) {
  CYC(0x44f1, 0x44f3); E = 0x08;
  CYC(0x44f3, 0x44f4); A = mem_rd(gb, DE);
  CYC(0x44f4, 0x44f5); common_code_add_double_index(gb, 0x44f5);
  specialObjectGetRelativeTileFromHl_hook(gb);
}

void specialObjectGetRelativeTileFromHl_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x44f5, 0x44f6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x44f6, 0x44f7); B = A;
  CYC(0x44f7, 0x44f8); C = mem_rd(gb, HL);
  CALL_C(0x44f8, objectGetRelativeTile_hook, 0x1435, 0x44fb);
  CYC(0x44fb, 0x44fc); B = A;
  CYC(0x44fc, 0x44fe); H = wRoomCollisions >> 8;
  CYC(0x44fe, 0x44ff); A = mem_rd(gb, HL);
  CYC(0x44ff, 0x4500); ret_effect(gb);
}

void specialObjectCheckMovingAwayFromWall_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4500, 0x4501); H = D;
  CYC(0x4501, 0x4503); L = 0x09;
  CYC(0x4503, 0x4504); A = mem_rd(gb, HL);
  CYC(0x4504, 0x4506); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x4506, 0x4507); ret_effect(gb); return;
  }
  CYC(0x4506, 0x4507);
  CYC(0x4507, 0x4509); alu_add(gb, 0x10);
  CYC(0x4509, 0x450b); alu_and(gb, 0x1f);
  CYC(0x450b, 0x450c); mem_wr(gb, HL, A);
  CALL_C(0x450c, specialObjectCheckFacingWall_hook, 0x4522, 0x450f);
  CYC(0x450f, 0x4510); C = A;
  CYC(0x4510, 0x4512); L = 0x09;
  CYC(0x4512, 0x4513); A = mem_rd(gb, HL);
  CYC(0x4513, 0x4515); alu_add(gb, 0x10);
  CYC(0x4515, 0x4517); alu_and(gb, 0x1f);
  CYC(0x4517, 0x4518); mem_wr(gb, HL, A);
  CYC(0x4518, 0x4519); A = C;
  CYC(0x4519, 0x451a); alu_or(gb, A);
  CYC(0x451a, 0x451b); ret_effect(gb);
}

void specialObjectCheckMovingTowardWall_hook(GB *gb) {
  CYC(0x451b, 0x451c); H = D;
  CYC(0x451c, 0x451e); L = 0x09;
  CYC(0x451e, 0x451f); A = mem_rd(gb, HL);
  CYC(0x451f, 0x4521); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x4521, 0x4522); ret_effect(gb); return;
  }
  CYC(0x4521, 0x4522);
  specialObjectCheckFacingWall_hook(gb);
}

void specialObjectCheckFacingWall_hook(GB *gb) {
  CYC(0x4522, 0x4525); SET_BC(0x0000);
  CYC(0x4525, 0x4527); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(0x4527, 0x4529);
    goto check_vertical;
  }
  CYC(0x4527, 0x4529);
  CYC(0x4529, 0x452b); alu_cp(gb, 0x18);
  if (F & FZ) {
    CYCT(0x452b, 0x452d);
    goto check_vertical;
  }
  CYC(0x452b, 0x452d);
  CYC(0x452d, 0x452f); L = 0x33;
  CYC(0x452f, 0x4530); B = mem_rd(gb, HL);
  CYC(0x4530, 0x4531); alu_add(gb, A);
  CYC(0x4531, 0x4533); A = alu_swap(gb, A);
  CYC(0x4533, 0x4535); alu_and(gb, 0x03);
  CYC(0x4535, 0x4537); A = 0x30;
  if (!(F & FZ)) {
    CYCT(0x4537, 0x4539);
  } else {
    CYC(0x4537, 0x4539);
    CYC(0x4539, 0x453b); A = 0xc0;
  }
  CYC(0x453b, 0x453c); alu_and(gb, B);
  CYC(0x453c, 0x453d); B = A;

check_vertical:
  CYC(0x453d, 0x453f); L = 0x09;
  CYC(0x453f, 0x4540); A = mem_rd(gb, HL);
  CYC(0x4540, 0x4542); alu_and(gb, 0x0f);
  if (F & FZ) {
    CYCT(0x4542, 0x4544);
    goto done;
  }
  CYC(0x4542, 0x4544);
  CYC(0x4544, 0x4545); A = mem_rd(gb, HL);
  CYC(0x4545, 0x4547); L = 0x33;
  CYC(0x4547, 0x4548); C = mem_rd(gb, HL);
  CYC(0x4548, 0x454a); alu_bit(gb, 4, A);
  CYC(0x454a, 0x454c); A = 0x03;
  if (F & FZ) {
    CYCT(0x454c, 0x454e);
  } else {
    CYC(0x454c, 0x454e);
    CYC(0x454e, 0x4550); A = 0x0c;
  }
  CYC(0x4550, 0x4551); alu_and(gb, C);
  CYC(0x4551, 0x4552); C = A;

done:
  CYC(0x4552, 0x4553); A = B;
  CYC(0x4553, 0x4554); alu_or(gb, C);
  CYC(0x4554, 0x4555); ret_effect(gb);
}

void companionCreateItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4555, getFreeItemSlot_hook, 0x2cf9, 0x4558);
  if (!(F & FZ)) {
    CYCT(0x4558, 0x4559); ret_effect(gb); return;
  }
  CYC(0x4558, 0x4559);
  CYC(0x4559, 0x455b);
  CYC(0x4561, 0x4562); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4562, 0x4563); L = alu_inc8(gb, L);
  CYC(0x4563, 0x4564); mem_wr(gb, HL, B);
  CYC(0x4564, 0x4565); L = alu_inc8(gb, L);
  CYC(0x4565, 0x4566); mem_wr(gb, HL, C);
  CYC(0x4566, 0x4568); L = 0x28;
  CYC(0x4568, 0x456a); mem_wr(gb, HL, 0xf9);
  CYC(0x456a, 0x456b); alu_xor(gb, A);
  CYC(0x456b, 0x456c); ret_effect(gb);
}
