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
void companionUpdateDirectionAndSetAnimation_hook(GB *gb);
void companionSetAnimation_hook(GB *gb);
void setLinkMountingSpeed_hook(GB *gb);
void companionGotoHazardHandlingState_hook(GB *gb);
void companionDismount_hook(GB *gb);
void saveLinkLocalRespawnAndCompanionPosition_hook(GB *gb);
void companionRetIfInactiveWithoutStateCheck_hook(GB *gb);

static void companion_ret_if_inactive_return_from_caller(GB *gb) {
  CYC(0x48a1, 0x48a2); SET_AF(pop_effect(gb));
  CYC(0x48a2, 0x48a3); ret_effect(gb);
}

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

void companionCreateWeaponItem_hook(GB *gb) {
  CYC(0x455b, 0x455e); SET_HL(w1WeaponItem_enabled);
  CYC(0x455e, 0x455f); A = mem_rd(gb, HL);
  CYC(0x455f, 0x4560); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4560, 0x4561); ret_effect(gb); return;
  }
  CYC(0x4560, 0x4561);
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

void companionUpdateDirectionAndAnimate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x456c, 0x456e); E = 0x08;
  CYC(0x456e, 0x456f); A = mem_rd(gb, DE);
  CYC(0x456f, 0x4572); mem_wr(gb, w1Link_direction, A);
  CYC(0x4572, 0x4574); E = 0x04;
  CYC(0x4574, 0x4575); A = mem_rd(gb, DE);
  CYC(0x4575, 0x4577); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(0x4577, 0x457a); specialObjectAnimate_hook(gb); return;
  }
  CYC(0x4577, 0x457a);
  CALL_C(0x457a, updateLinkDirectionFromAngle_hook, 0x2b64, 0x457d);
  CYC(0x457d, 0x4580); SET_HL(w1Companion_direction);
  CYC(0x4580, 0x4581); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4581, 0x4584); specialObjectAnimate_hook(gb); return;
  }
  CYC(0x4581, 0x4584);
  companionUpdateDirectionAndSetAnimation_hook(gb);
}

void companionUpdateDirectionAndSetAnimation_hook(GB *gb) {
  CYC(0x4584, 0x4586); E = 0x09;
  CYC(0x4586, 0x4587); A = mem_rd(gb, DE);
  CYC(0x4587, 0x4588); alu_add(gb, A);
  CYC(0x4588, 0x458a); A = alu_swap(gb, A);
  CYC(0x458a, 0x458c); alu_and(gb, 0x03);
  CYC(0x458c, 0x458d); E = alu_dec8(gb, E);
  CYC(0x458d, 0x458e); mem_wr(gb, DE, A);
  companionSetAnimation_hook(gb);
}

void companionSetAnimation_hook(GB *gb) {
  CYC(0x458e, 0x458f); H = D;
  CYC(0x458f, 0x4590); A = C;
  CYC(0x4590, 0x4592); L = 0x08;
  CYC(0x4592, 0x4593); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4593, 0x4595); L = 0x38;
  CYC(0x4595, 0x4596); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4596, 0x4599);
  specialObjectSetAnimation_hook(gb);
}

void companionTryToMount_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4599, 0x459c); A = W8(wActiveTileType);
  CYC(0x459c, 0x459e); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x459e, 0x45a0);
    goto cant_mount;
  }
  CYC(0x459e, 0x45a0);
  CYC(0x45a0, 0x45a3); A = W8(wDisallowMountingCompanion);
  CYC(0x45a3, 0x45a4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x45a4, 0x45a6);
    goto cant_mount;
  }
  CYC(0x45a4, 0x45a6);
  CALL_C(0x45a6, checkLinkVulnerableAndIDZero_hook, 0x1d20, 0x45a9);
  if (F & FC) {
    CYCT(0x45a9, 0x45ab);
    goto try_mounting;
  }
  CYC(0x45a9, 0x45ab);

cant_mount:
  CYC(0x45ab, 0x45ac); alu_or(gb, D);
  CYC(0x45ac, 0x45ad); ret_effect(gb);
  return;

try_mounting:
  CYC(0x45ad, 0x45b0); A = W8(w1Link_state);
  CYC(0x45b0, 0x45b2); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x45b2, 0x45b3); ret_effect(gb); return;
  }
  CYC(0x45b2, 0x45b3);
  CYC(0x45b3, 0x45b6); A = W8(wLinkSwimmingState);
  CYC(0x45b6, 0x45b7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x45b7, 0x45b8); ret_effect(gb); return;
  }
  CYC(0x45b7, 0x45b8);
  CYC(0x45b8, 0x45bb); A = W8(wLinkGrabState);
  CYC(0x45bb, 0x45bc); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x45bc, 0x45bd); ret_effect(gb); return;
  }
  CYC(0x45bc, 0x45bd);
  CYC(0x45bd, 0x45c0); A = W8(wLinkInAir);
  CYC(0x45c0, 0x45c1); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x45c1, 0x45c2); ret_effect(gb); return;
  }
  CYC(0x45c1, 0x45c2);
  CYC(0x45c2, 0x45c3); A = alu_inc8(gb, A);
  CYC(0x45c3, 0x45c6); W8(wDisableWarpTiles) = A;
  CYC(0x45c6, 0x45c9); W8(wWarpsDisabled) = A;
  CYC(0x45c9, 0x45cb); E = 0x04;
  CYC(0x45cb, 0x45cd); A = 0x03;
  CYC(0x45cd, 0x45ce); mem_wr(gb, DE, A);
  CYC(0x45ce, 0x45d0); A = 0xff;
  setLinkMountingSpeed_hook(gb);
}

void setLinkMountingSpeed_hook(GB *gb) {
  CYC(0x45d0, 0x45d3); W8(wLinkAngle) = A;
  CYC(0x45d3, 0x45d5); A = 0x81;
  CYC(0x45d5, 0x45d8); W8(wLinkInAir) = A;
  CYC(0x45d8, 0x45db); W8(wDisableScreenTransitions) = A;
  CYC(0x45db, 0x45de); SET_HL(w1Link_angle);
  CYC(0x45de, 0x45df); mem_wr(gb, HL, A);
  CYC(0x45df, 0x45e1); L = 0x10;
  CYC(0x45e1, 0x45e3); mem_wr(gb, HL, 0x14);
  CYC(0x45e3, 0x45e5); L = 0x14;
  CYC(0x45e5, 0x45e7); mem_wr(gb, HL, 0x40);
  CYC(0x45e7, 0x45e8); L = alu_inc8(gb, L);
  CYC(0x45e8, 0x45ea); mem_wr(gb, HL, 0xfe);
  CYC(0x45ea, 0x45eb); alu_xor(gb, A);
  CYC(0x45eb, 0x45ec); ret_effect(gb);
}

void companionCheckHazards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x45ec, objectCheckIsOnHazard_hook, 0x220d, 0x45ef);
  CYC(0x45ef, 0x45f0); H = D;
  if (!(F & FC)) {
    CYCT(0x45f0, 0x45f1); ret_effect(gb); return;
  }
  CYC(0x45f0, 0x45f1);
  companionGotoHazardHandlingState_hook(gb);
}

void companionGotoHazardHandlingState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x45f1, 0x45f2); push_effect(gb, AF);
  CYC(0x45f2, 0x45f4); L = 0x04;
  CYC(0x45f4, 0x45f6); A = 0x04;
  CYC(0x45f6, 0x45f7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x45f7, 0x45f8); alu_xor(gb, A);
  CYC(0x45f8, 0x45f9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x45f9, 0x45fa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x45fa, 0x45fc); L = 0x01;
  CYC(0x45fc, 0x45fd); A = mem_rd(gb, HL);
  CYC(0x45fd, 0x45ff); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(0x45ff, 0x4601);
  } else {
    CYC(0x45ff, 0x4601);
    CYC(0x4601, 0x4604); W8(wDisableScreenTransitions) = A;
    CYC(0x4604, 0x4606); A = 0x87;
    CALL_C(0x4606, playSound_b00_hook, 0x0c98, 0x4609);
  }
  CYC(0x4609, 0x460a); SET_AF(pop_effect(gb));
  CYC(0x460a, 0x460b); alu_scf(gb);
  CYC(0x460b, 0x460c); ret_effect(gb);
}

void companionDismountAndSavePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x460c, companionDismount_hook, 0x4630, 0x460f);
  CYC(0x460f, 0x4611); E = 0x01;
  CYC(0x4611, 0x4612); A = mem_rd(gb, DE);
  CYC(0x4612, 0x4615); SET_HL(wAnimalCompanion);
  CYC(0x4615, 0x4616); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4616, 0x4618);
    goto normal_dismount;
  }
  CYC(0x4616, 0x4618);
  CYC(0x4618, 0x461a); alu_cp(gb, 0x0b);
  if (F & FZ) {
    CYCT(0x461a, 0x461c);
    CYC(0x4622, 0x4624);
    goto normal_dismount;
  }
  CYC(0x461a, 0x461c);
  CYC(0x461c, 0x461e); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(0x461e, 0x4620);
    CYC(0x4624, 0x4626);
    goto normal_dismount;
  }
  CYC(0x461e, 0x4620);
  CYC(0x4620, 0x4622);

normal_dismount:
  CYC(0x462e, 0x4630);
  saveLinkLocalRespawnAndCompanionPosition_hook(gb);
}

void companionDismount_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4630, 0x4631); alu_xor(gb, A);
  CALL_C(0x4631, setLinkID_hook, 0x2ad9, 0x4634);
  CYC(0x4634, 0x4637); SET_HL(w1Link_oamFlagsBackup);
  CYC(0x4637, 0x4638); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4638, 0x4639); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4639, 0x463a); H = D;
  CYC(0x463a, 0x463b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x463b, 0x463c); mem_wr(gb, HL, A);
  CYC(0x463c, 0x463d); alu_xor(gb, A);
  CYC(0x463d, 0x463f); L = 0x25;
  CYC(0x463f, 0x4640); mem_wr(gb, HL, A);
  CYC(0x4640, 0x4642); L = 0x2b;
  CYC(0x4642, 0x4643); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4643, 0x4644); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4644, 0x4645); mem_wr(gb, HL, A);
  CYC(0x4645, 0x4647); L = 0x3c;
  CYC(0x4647, 0x4648); mem_wr(gb, HL, A);
  CYC(0x4648, 0x464b); W8(wLinkForceState) = A;
  CYC(0x464b, 0x464e); W8(wcc50) = A;
  CYC(0x464e, 0x4650); L = 0x00;
  CYC(0x4650, 0x4652); mem_wr(gb, HL, 0x01);
  CYC(0x4652, 0x4654); L = 0x08;
  CYC(0x4654, 0x4655); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4655, 0x4657); A = alu_swap(gb, A);
  CYC(0x4657, 0x4659); A = alu_srl(gb, A);
  CYC(0x4659, 0x465a); mem_wr(gb, HL, A);
  CALL_C(0x465a, setLinkMountingSpeed_hook, 0x45d0, 0x465d);
  CYC(0x465d, 0x4660); SET_HL(w1Link_angle);
  CYC(0x4660, 0x4662); mem_wr(gb, HL, 0xff);
  CALL_C(0x4662, objectCopyPosition_hook, 0x2242, 0x4665);
  CYC(0x4665, 0x4666); L = alu_dec8(gb, L);
  CYC(0x4666, 0x4668); mem_wr(gb, HL, 0xf8);
  CYC(0x4668, 0x4669); A = H;
  CYC(0x4669, 0x466c); W8(wLinkObjectIndex) = A;
  CYC(0x466c, 0x466d); alu_xor(gb, A);
  CYC(0x466d, 0x4670); W8(wDisableWarpTiles) = A;
  CYC(0x4670, 0x4673); W8(wWarpsDisabled) = A;
  CYC(0x4673, 0x4676); W8(wForceCompanionDismount) = A;
  CYC(0x4676, 0x4679); W8(wDisableScreenTransitions) = A;
  CYC(0x4679, 0x467c);
  setCameraFocusedObjectToLink_hook(gb);
}

void saveLinkLocalRespawnAndCompanionPosition_hook(GB *gb) {
  CYC(0x467c, 0x467f); SET_HL(wRememberedCompanionId);
  CYC(0x467f, 0x4682); A = W8(w1Companion_id);
  CYC(0x4682, 0x4683); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4683, 0x4686); A = W8(wActiveGroup);
  CYC(0x4686, 0x4687); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4687, 0x468a); A = W8(wActiveRoom);
  CYC(0x468a, 0x468b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x468b, 0x468e); A = W8(w1Companion_direction);
  CYC(0x468e, 0x4691); W8(wLinkLocalRespawnDir) = A;
  CYC(0x4691, 0x4694); A = W8(w1Companion_yh);
  CYC(0x4694, 0x4695); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4695, 0x4698); W8(wLinkLocalRespawnY) = A;
  CYC(0x4698, 0x469b); A = W8(w1Companion_xh);
  CYC(0x469b, 0x469c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x469c, 0x469f); W8(wLinkLocalRespawnX) = A;
  CYC(0x469f, 0x46a0); ret_effect(gb);
}

void companionDragToCenterOfHole_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46a0, 0x46a2); E = 0x3d;
  CYC(0x46a2, 0x46a3); A = mem_rd(gb, DE);
  CYC(0x46a3, 0x46a4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x46a4, 0x46a6);
  } else {
    CYC(0x46a4, 0x46a6);
    CYC(0x46a6, 0x46a7); alu_xor(gb, A);
    CYC(0x46a7, 0x46a8); ret_effect(gb);
    return;
  }
  CYC(0x46a8, 0x46ab); SET_BC(0x0500);
  CALL_C(0x46ab, objectGetRelativeTile_hook, 0x1435, 0x46ae);
  CYC(0x46ae, 0x46af); C = L;
  CALL_C(0x46af, convertShortToLongPosition_paramC_hook, 0x20cc, 0x46b2);
  CYC(0x46b2, 0x46b4); E = 0x0d;
  CYC(0x46b4, 0x46b5); A = mem_rd(gb, DE);
  CYC(0x46b5, 0x46b6); alu_cp(gb, C);
  CYC(0x46b6, 0x46b8); C = 0x00;
  if (F & FZ) {
    CYCT(0x46b8, 0x46ba);
  } else {
    CYC(0x46b8, 0x46ba);
    CYC(0x46ba, 0x46bd); SET_HL(0x0040);
    if (F & FC) {
      CYCT(0x46bd, 0x46bf);
    } else {
      CYC(0x46bd, 0x46bf);
      CYC(0x46bf, 0x46c2); SET_HL(0xffc0);
    }
    CYC(0x46c2, 0x46c3); E = alu_dec8(gb, E);
    CYC(0x46c3, 0x46c4); A = mem_rd(gb, DE);
    CYC(0x46c4, 0x46c5); alu_add(gb, L);
    CYC(0x46c5, 0x46c6); mem_wr(gb, DE, A);
    CYC(0x46c6, 0x46c7); E = alu_inc8(gb, E);
    CYC(0x46c7, 0x46c8); A = mem_rd(gb, DE);
    CYC(0x46c8, 0x46c9); alu_adc(gb, H);
    CYC(0x46c9, 0x46ca); mem_wr(gb, DE, A);
    CYC(0x46ca, 0x46cb); C = alu_dec8(gb, C);
  }
  CYC(0x46cb, 0x46cd); E = 0x0b;
  CYC(0x46cd, 0x46ce); A = mem_rd(gb, DE);
  CYC(0x46ce, 0x46cf); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x46cf, 0x46d1);
  } else {
    CYC(0x46cf, 0x46d1);
    CYC(0x46d1, 0x46d4); SET_HL(0x0040);
    if (F & FC) {
      CYCT(0x46d4, 0x46d6);
    } else {
      CYC(0x46d4, 0x46d6);
      CYC(0x46d6, 0x46d9); SET_HL(0xffc0);
    }
    CYC(0x46d9, 0x46da); E = alu_dec8(gb, E);
    CYC(0x46da, 0x46db); A = mem_rd(gb, DE);
    CYC(0x46db, 0x46dc); alu_add(gb, L);
    CYC(0x46dc, 0x46dd); mem_wr(gb, DE, A);
    CYC(0x46dd, 0x46de); E = alu_inc8(gb, E);
    CYC(0x46de, 0x46df); A = mem_rd(gb, DE);
    CYC(0x46df, 0x46e0); alu_adc(gb, H);
    CYC(0x46e0, 0x46e1); mem_wr(gb, DE, A);
    CYC(0x46e1, 0x46e2); C = alu_dec8(gb, C);
  }
  CYC(0x46e2, 0x46e3); H = D;
  CYC(0x46e3, 0x46e4); A = C;
  CYC(0x46e4, 0x46e5); alu_or(gb, A);
  CYC(0x46e5, 0x46e6); ret_effect(gb);
}

void companionRespawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46e6, 0x46e7); alu_xor(gb, A);
  CYC(0x46e7, 0x46ea); W8(wDisableScreenTransitions) = A;
  CYC(0x46ea, 0x46ed); W8(wLinkForceState) = A;
  CYC(0x46ed, 0x46f0); W8(wcc50) = A;
  CALL_C(0x46f0, specialObjectSetCoordinatesToRespawnYX_hook, 0x2b8a, 0x46f3);
  CALL_C(0x46f3, objectCheckSimpleCollision_hook, 0x1487, 0x46f6);
  if (!(F & FZ)) {
    CYCT(0x46f6, 0x46f8);
    goto invalid_position;
  }
  CYC(0x46f6, 0x46f8);
  CALL_C(0x46f8, objectGetPosition_hook, 0x208a, 0x46fb);
  CALL_C(0x46fb, checkCollisionForCompanion_hook, 0x44be, 0x46fe);
  if (F & FC) {
    CYCT(0x46fe, 0x4700);
    goto invalid_position;
  }
  CYC(0x46fe, 0x4700);
  CALL_C(0x4700, objectCheckIsOnHazard_hook, 0x220d, 0x4703);
  if (!(F & FC)) {
    CYCT(0x4703, 0x4705);
    goto apply_damage_and_set_state;
  }
  CYC(0x4703, 0x4705);

invalid_position:
  CYC(0x4705, 0x4706); H = D;
  CYC(0x4706, 0x4708); L = 0x0b;
  CYC(0x4708, 0x470b); A = W8(wLastAnimalMountPointY);
  CYC(0x470b, 0x470e); W8(wLinkLocalRespawnY) = A;
  CYC(0x470e, 0x470f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x470f, 0x4710); L = alu_inc8(gb, L);
  CYC(0x4710, 0x4713); A = W8(wLastAnimalMountPointX);
  CYC(0x4713, 0x4716); W8(wLinkLocalRespawnX) = A;
  CYC(0x4716, 0x4717); mem_wr(gb, HL, A); SET_HL(HL + 1);

apply_damage_and_set_state:
  CYC(0x4717, 0x471a); A = W8(wLinkObjectIndex);
  CYC(0x471a, 0x471b); alu_rrca(gb);
  CYC(0x471b, 0x471d); A = 0x01;
  if (!(F & FC)) {
    CYCT(0x471d, 0x471f);
    goto set_state;
  }
  CYC(0x471d, 0x471f);
  CYC(0x471f, 0x4721); A = 0xfe;
  CYC(0x4721, 0x4724); W8(w1Link_damageToApply) = A;
  CYC(0x4724, 0x4726); A = 0x40;
  CYC(0x4726, 0x4729); W8(w1Link_invincibilityCounter) = A;
  CYC(0x4729, 0x472b); A = 0x05;

set_state:
  CYC(0x472b, 0x472c); H = D;
  CYC(0x472c, 0x472e); L = 0x04;
  CYC(0x472e, 0x472f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x472f, 0x4730); alu_xor(gb, A);
  CYC(0x4730, 0x4731); mem_wr(gb, HL, A);
  CYC(0x4731, 0x4733); L = 0x3d;
  CYC(0x4733, 0x4734); mem_wr(gb, HL, A);
  CYC(0x4734, 0x4737); W8(wDisableScreenTransitions) = A;
  CYC(0x4737, 0x4739); L = 0x24;
  CYC(0x4739, 0x473b); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x473b, 0x473c); ret_effect(gb);
}

void companionCheckHopDownCliff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x473c, 0x473f); A = W8(wLinkAngle);
  CYC(0x473f, 0x4740); C = A;
  CYC(0x4740, 0x4742); alu_and(gb, 0xe7);
  if (!(F & FZ)) {
    CYCT(0x4742, 0x4743); ret_effect(gb); return;
  }
  CYC(0x4742, 0x4743);
  CYC(0x4743, 0x4745); E = 0x09;
  CYC(0x4745, 0x4746); A = mem_rd(gb, DE);
  CYC(0x4746, 0x4747); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(0x4747, 0x4748); ret_effect(gb); return;
  }
  CYC(0x4747, 0x4748);
  CALL_C(0x4748, specialObjectCheckMovingTowardWall_hook, 0x451b, 0x474b);
  CYC(0x474b, 0x474d); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x474d, 0x474f);
    goto check_tile;
  }
  CYC(0x474d, 0x474f);
  CYC(0x474f, 0x4751); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(0x4751, 0x4753);
    goto check_tile;
  }
  CYC(0x4751, 0x4753);
  CYC(0x4753, 0x4755); alu_cp(gb, 0x30);
  if (!(F & FZ)) {
    CYCT(0x4755, 0x4756); ret_effect(gb); return;
  }
  CYC(0x4755, 0x4756);

check_tile:
  CYC(0x4756, 0x4758); E = 0x08;
  CYC(0x4758, 0x4759); A = mem_rd(gb, DE);
  CYC(0x4759, 0x475c); SET_HL(0x4796);
  CYC(0x475c, 0x475d); common_code_add_double_index(gb, 0x475d);
  CYC(0x475d, 0x475e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x475e, 0x475f); B = A;
  CYC(0x475f, 0x4760); C = mem_rd(gb, HL);
  CALL_C(0x4760, objectGetRelativeTile_hook, 0x1435, 0x4763);
  CYC(0x4763, 0x4765); alu_cp(gb, 0xd4);
  if (F & FZ) {
    CYCT(0x4765, 0x4767);
    CYC(0x4771, 0x4773); A = 0x10;
    goto cliff_tile;
  }
  CYC(0x4765, 0x4767);
  CYC(0x4767, 0x476a); SET_HL(0x7d09);
  CALL_C(0x476a, lookupCollisionTable_hook, 0x1e1f, 0x476d);
  if (F & FC) {
    CYCT(0x476d, 0x476f);
    goto cliff_tile;
  }
  CYC(0x476d, 0x476f);
  CYC(0x476f, 0x4770); alu_or(gb, D);
  CYC(0x4770, 0x4771); ret_effect(gb);
  return;

cliff_tile:
  CYC(0x4773, 0x4774); H = D;
  CYC(0x4774, 0x4776); L = 0x09;
  CYC(0x4776, 0x4777); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4777, 0x4778); ret_effect(gb); return;
  }
  CYC(0x4777, 0x4778);
  CYC(0x4778, 0x477a); A = 0x80;
  CYC(0x477a, 0x477d); W8(wLinkInAir) = A;
  CYC(0x477d, 0x4780); SET_BC(0xfd40);
  CALL_C(0x4780, objectSetSpeedZ_hook, 0x239d, 0x4783);
  CYC(0x4783, 0x4785); L = 0x10;
  CYC(0x4785, 0x4787); mem_wr(gb, HL, 0x50);
  CYC(0x4787, 0x4789); L = 0x06;
  CYC(0x4789, 0x478b); A = 0x14;
  CYC(0x478b, 0x478c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x478c, 0x478d); alu_xor(gb, A);
  CYC(0x478d, 0x478e); mem_wr(gb, HL, A);
  CYC(0x478e, 0x4790); L = 0x04;
  CYC(0x4790, 0x4792); A = 0x07;
  CYC(0x4792, 0x4793); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4793, 0x4794); alu_xor(gb, A);
  CYC(0x4794, 0x4795); mem_wr(gb, HL, A);
  CYC(0x4795, 0x4796); ret_effect(gb);
}

void companionFinalizeMounting_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x479e, 0x479f); H = D;
  CYC(0x479f, 0x47a1); L = 0x00;
  CYC(0x47a1, 0x47a3); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(0x47a3, 0x47a5); L = 0x04;
  CYC(0x47a5, 0x47a7); mem_wr(gb, HL, 0x05);
  CYC(0x47a7, 0x47a9); L = 0x09;
  CYC(0x47a9, 0x47ab); A = 0xff;
  CYC(0x47ab, 0x47ac); mem_wr(gb, HL, A);
  CYC(0x47ac, 0x47ae); L = 0x3c;
  CYC(0x47ae, 0x47af); mem_wr(gb, HL, A);
  CYC(0x47af, 0x47b1); L = 0x1a;
  CYC(0x47b1, 0x47b2); A = mem_rd(gb, HL);
  CYC(0x47b2, 0x47b4); alu_and(gb, 0xc0);
  CYC(0x47b4, 0x47b6); alu_or(gb, 0x01);
  CYC(0x47b6, 0x47b7); mem_wr(gb, HL, A);
  CYC(0x47b7, 0x47b8); alu_xor(gb, A);
  CYC(0x47b8, 0x47ba); L = 0x3d;
  CYC(0x47ba, 0x47bb); mem_wr(gb, HL, A);
  CYC(0x47bb, 0x47be); W8(wLinkInAir) = A;
  CYC(0x47be, 0x47c1); W8(wDisableScreenTransitions) = A;
  CYC(0x47c1, 0x47c4); SET_BC(wLastAnimalMountPointY);
  CYC(0x47c4, 0x47c6); L = 0x0b;
  CYC(0x47c6, 0x47c7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x47c7, 0x47c8); mem_wr(gb, BC, A);
  CYC(0x47c8, 0x47c9); C = alu_inc8(gb, C);
  CYC(0x47c9, 0x47ca); L = alu_inc8(gb, L);
  CYC(0x47ca, 0x47cb); A = mem_rd(gb, HL);
  CYC(0x47cb, 0x47cc); mem_wr(gb, BC, A);
  CYC(0x47cc, 0x47cd); A = D;
  CYC(0x47cd, 0x47d0); W8(wLinkObjectIndex) = A;
  CALL_C(0x47d0, setCameraFocusedObjectToLink_hook, 0x12f0, 0x47d3);
  CYC(0x47d3, 0x47d5); A = 0x09;
  CYC(0x47d5, 0x47d8);
  setLinkID_hook(gb);
}

void companionFunc_47d8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x47d8, 0x47d9); H = D;
  CYC(0x47d9, 0x47db); L = 0x3c;
  CYC(0x47db, 0x47dc); A = mem_rd(gb, HL);
  CYC(0x47dc, 0x47dd); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x47dd, 0x47de); ret_effect(gb); return;
  }
  CYC(0x47dd, 0x47de);
  CYC(0x47de, 0x47e1); A = W8(wLinkDeathTrigger);
  CYC(0x47e1, 0x47e2); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x47e2, 0x47e3); ret_effect(gb); return;
  }
  CYC(0x47e2, 0x47e3);
  CYC(0x47e3, 0x47e4); alu_xor(gb, A);
  CYC(0x47e4, 0x47e5); mem_wr(gb, HL, A);
  CYC(0x47e5, 0x47e7); E = 0x0e;
  CYC(0x47e7, 0x47e8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x47e8, 0x47e9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x47e9, 0x47eb); L = 0x04;
  CYC(0x47eb, 0x47ed); mem_wr(gb, HL, 0x09);
  CYC(0x47ed, 0x47ef); E = 0x1b;
  CYC(0x47ef, 0x47f0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x47f0, 0x47f1); mem_wr(gb, HL, A);
  CYC(0x47f1, 0x47f3); E = 0x1a;
  CYC(0x47f3, 0x47f4); alu_xor(gb, A);
  CYC(0x47f4, 0x47f5); mem_wr(gb, DE, A);
  CYC(0x47f5, 0x47f7); H = 0xd0;
  CALL_C(0x47f7, objectCopyPosition_hook, 0x2242, 0x47fa);
  CYC(0x47fa, 0x47fb); A = H;
  CYC(0x47fb, 0x47fe); W8(wLinkObjectIndex) = A;
  CALL_C(0x47fe, setCameraFocusedObjectToLink_hook, 0x12f0, 0x4801);
  CYC(0x4801, 0x4802); alu_xor(gb, A);
  CALL_C(0x4802, setLinkID_hook, 0x2ad9, 0x4805);
  CYC(0x4805, 0x4806); alu_or(gb, D);
  CYC(0x4806, 0x4807); ret_effect(gb);
}

static void companion_set_state_and_clear_substate(GB *gb) {
  CYC(0x481b, 0x481d); E = 0x04;
  CYC(0x481d, 0x481e); mem_wr(gb, DE, A);
  CYC(0x481e, 0x481f); E = alu_inc8(gb, E);
  CYC(0x481f, 0x4820); alu_xor(gb, A);
  CYC(0x4820, 0x4821); mem_wr(gb, DE, A);
  CYC(0x4821, 0x4822); ret_effect(gb);
}

void companionGotoDismountState_hook(GB *gb) {
  CYC(0x4807, 0x4809); E = 0x38;
  CYC(0x4809, 0x480a); A = mem_rd(gb, DE);
  CYC(0x480a, 0x480b); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x480b, 0x480d);
    CYC(0x4812, 0x4814); A = 0x06;
    CYC(0x4814, 0x4816);
    companion_set_state_and_clear_substate(gb);
    return;
  }
  CYC(0x480b, 0x480d);
  CYC(0x480d, 0x480e); alu_xor(gb, A);
  CYC(0x480e, 0x4811); W8(wForceCompanionDismount) = A;
  CYC(0x4811, 0x4812); ret_effect(gb);
}

void companionSetAnimationAndGotoState5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4816, companionSetAnimation_hook, 0x458e, 0x4819);
  CYC(0x4819, 0x481b); A = 0x05;
  companion_set_state_and_clear_substate(gb);
}

void companionCheckCanSpawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4822, 0x4824); E = 0x04;
  CYC(0x4824, 0x4825); A = mem_rd(gb, DE);
  CYC(0x4825, 0x4826); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4826, 0x4828);
    goto can_spawn;
  }
  CYC(0x4826, 0x4828);
  CYC(0x4828, 0x4829); E = alu_inc8(gb, E);
  CYC(0x4829, 0x482a); A = mem_rd(gb, DE);
  CYC(0x482a, 0x482b); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x482b, 0x482d);
  } else {
    CYC(0x482b, 0x482d);
    CYC(0x482d, 0x482e); A = alu_inc8(gb, A);
    CYC(0x482e, 0x482f); mem_wr(gb, DE, A);
    CYC(0x482f, 0x4830); SET_AF(pop_effect(gb));
    CYC(0x4830, 0x4831); ret_effect(gb);
    return;
  }
  CYC(0x4831, 0x4832); alu_xor(gb, A);
  CYC(0x4832, 0x4833); mem_wr(gb, DE, A);
  CALL_C(0x4833, objectGetShortPosition_hook, 0x2096, 0x4836);
  CYC(0x4836, 0x4837); B = A;
  CYC(0x4837, 0x4839); A = 0x02;
  CYC(0x4839, 0x483b); hram_wr(gb, R_SVBK, A);
  CYC(0x483b, 0x483c); A = B;
  CYC(0x483c, 0x483f); SET_HL(w2SolidObjectPositions);
  CALL_C(0x483f, checkFlag_hook, 0x0205, 0x4842);
  CYC(0x4842, 0x4844); A = 0x00;
  CYC(0x4844, 0x4846); hram_wr(gb, R_SVBK, A);
  if (F & FZ) {
    CYCT(0x4846, 0x4848);
  } else {
    CYC(0x4846, 0x4848);
    CYC(0x4848, 0x4849); SET_AF(pop_effect(gb));
    CYC(0x4849, 0x484c);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x484c, 0x484e); E = 0x0b;
  CYC(0x484e, 0x484f); A = mem_rd(gb, DE);
  CYC(0x484f, 0x4851); alu_add(gb, 0x05);
  CYC(0x4851, 0x4852); B = A;
  CYC(0x4852, 0x4854); E = 0x0d;
  CYC(0x4854, 0x4855); A = mem_rd(gb, DE);
  CYC(0x4855, 0x4856); C = A;
  CALL_C(0x4856, getTileCollisionsAtPosition_hook, 0x14b7, 0x4859);
  CYC(0x4859, 0x485b); alu_cp(gb, 0x10);
  if (F & FZ) {
    CYCT(0x485b, 0x485d);
    goto try_last_mount_point;
  }
  CYC(0x485b, 0x485d);
  CYC(0x485d, 0x485f); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x485f, 0x4861);
    goto can_spawn;
  }
  CYC(0x485f, 0x4861);

try_last_mount_point:
  CYC(0x4861, 0x4864); SET_HL(wLastAnimalMountPointY);
  CYC(0x4864, 0x4865); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4865, 0x4867); E = 0x0b;
  CYC(0x4867, 0x4868); mem_wr(gb, DE, A);
  CYC(0x4868, 0x4869); A = mem_rd(gb, HL);
  CYC(0x4869, 0x486b); E = 0x0d;
  CYC(0x486b, 0x486c); mem_wr(gb, DE, A);
  CALL_C(0x486c, objectGetTileCollisions_hook, 0x14ad, 0x486f);
  if (F & FZ) {
    CYCT(0x486f, 0x4871);
    goto can_spawn;
  }
  CYC(0x486f, 0x4871);
  CYC(0x4871, 0x4872); SET_AF(pop_effect(gb));
  CYC(0x4872, 0x4875);
  itemDelete_hook(gb);
  return;

can_spawn:
  CALL_C(0x4875, specialObjectSetOamVariables_hook, 0x41f7, 0x4878);
  CYC(0x4878, 0x487b); SET_HL(w1Companion_var03);
  CYC(0x487b, 0x487c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x487c, 0x487d); A = alu_inc8(gb, A);
  CYC(0x487d, 0x487e); mem_wr(gb, HL, A);
  CYC(0x487e, 0x4880); L = 0x24;
  CYC(0x4880, 0x4882); mem_wr(gb, HL, 0x80);
  CYC(0x4882, 0x4883); ret_effect(gb);
}

void companionRetIfInactive_hook(GB *gb) {
  CYC(0x4883, 0x4885); E = 0x04;
  CYC(0x4885, 0x4886); A = mem_rd(gb, DE);
  CYC(0x4886, 0x4887); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4887, 0x4888); ret_effect(gb); return;
  }
  CYC(0x4887, 0x4888);
  CYC(0x4888, 0x488b); A = W8(wTextIsActive);
  CYC(0x488b, 0x488c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x488c, 0x488e);
    companion_ret_if_inactive_return_from_caller(gb);
    return;
  }
  CYC(0x488c, 0x488e);
  companionRetIfInactiveWithoutStateCheck_hook(gb);
}

void companionRetIfInactiveWithoutStateCheck_hook(GB *gb) {
  CYC(0x488e, 0x4891); A = W8(wScrollMode);
  CYC(0x4891, 0x4893); alu_and(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x4893, 0x4895);
    companion_ret_if_inactive_return_from_caller(gb);
    return;
  }
  CYC(0x4893, 0x4895);
  CYC(0x4895, 0x4898); A = W8(wPaletteThread_mode);
  CYC(0x4898, 0x4899); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4899, 0x489b);
    companion_ret_if_inactive_return_from_caller(gb);
    return;
  }
  CYC(0x4899, 0x489b);
  CYC(0x489b, 0x489e); A = W8(wDisabledObjects);
  CYC(0x489e, 0x48a0); alu_and(gb, 0xa0);
  if (F & FZ) {
    CYCT(0x48a0, 0x48a1); ret_effect(gb); return;
  }
  CYC(0x48a0, 0x48a1);
  companion_ret_if_inactive_return_from_caller(gb);
}

void companionSetAnimationToVar3f_hook(GB *gb) {
  CYC(0x48a3, 0x48a4); H = D;
  CYC(0x48a4, 0x48a6); L = 0x3f;
  CYC(0x48a6, 0x48a7); A = mem_rd(gb, HL);
  CYC(0x48a7, 0x48a9); L = 0x30;
  CYC(0x48a9, 0x48aa); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x48aa, 0x48ad);
    specialObjectSetAnimation_hook(gb);
    return;
  }
  CYC(0x48aa, 0x48ad);
  CYC(0x48ad, 0x48ae); ret_effect(gb);
}

void companionFlashFromChargingAnimation_hook(GB *gb) {
  CYC(0x48ae, 0x48b1); SET_HL(w1Link_oamFlagsBackup);
  CYC(0x48b1, 0x48b4); A = W8(wFrameCounter);
  CYC(0x48b4, 0x48b6); alu_bit(gb, 2, A);
  if (!(F & FZ)) {
    CYCT(0x48b6, 0x48b8);
    CYC(0x48be, 0x48bf); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x48bf, 0x48c0); mem_wr(gb, HL, A);
    CYC(0x48c0, 0x48c1); ret_effect(gb);
    return;
  }
  CYC(0x48b6, 0x48b8);
  CYC(0x48b8, 0x48b9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x48b9, 0x48bb); alu_and(gb, 0xf8);
  CYC(0x48bb, 0x48bc); alu_or(gb, C);
  CYC(0x48bc, 0x48bd); mem_wr(gb, HL, A);
  CYC(0x48bd, 0x48be); ret_effect(gb);
}

void companionCheckEnableTerrainEffects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x490a, 0x490b); H = D;
  CYC(0x490b, 0x490d); L = 0x00;
  CYC(0x490d, 0x490e); A = mem_rd(gb, HL);
  CYC(0x490e, 0x490f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x490f, 0x4910); ret_effect(gb); return;
  }
  CYC(0x490f, 0x4910);
  CYC(0x4910, 0x4912); L = 0x3c;
  CYC(0x4912, 0x4913); A = mem_rd(gb, HL);
  CYC(0x4913, 0x4916); W8(wWarpsDisabled) = A;
  CYC(0x4916, 0x4918); L = 0x0f;
  CYC(0x4918, 0x4919); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4919, 0x491b); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x491b, 0x491d);
    goto enable_terrain_effects;
  }
  CYC(0x491b, 0x491d);
  CYC(0x491d, 0x4920); SET_BC(0x0500);
  CALL_C(0x4920, objectGetRelativeTile_hook, 0x1435, 0x4923);
  CYC(0x4923, 0x4924); H = D;
  CYC(0x4924, 0x4926); alu_cp(gb, 0xf9);
  if (!(F & FZ)) {
    CYCT(0x4926, 0x4928);
    CYC(0x492d, 0x492f); L = 0x0f;
    CYC(0x492f, 0x4931); mem_wr(gb, HL, 0x00);
    goto enable_terrain_effects;
  }
  CYC(0x4926, 0x4928);
  CYC(0x4928, 0x492a); L = 0x1a;
  CYC(0x492a, 0x492c); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(0x492c, 0x492d); ret_effect(gb);
  return;

enable_terrain_effects:
  CYC(0x4931, 0x4933); L = 0x1a;
  CYC(0x4933, 0x4935); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(0x4935, 0x4936); ret_effect(gb);
}

void companionSetPriorityRelativeToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4936, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x4939);
  CYC(0x4939, 0x493a); B = alu_dec8(gb, B);
  CYC(0x493a, 0x493c); alu_and(gb, 0xc0);
  CYC(0x493c, 0x493d); alu_or(gb, B);
  CYC(0x493d, 0x493e); mem_wr(gb, DE, A);
  CYC(0x493e, 0x493f); ret_effect(gb);
}

void companionDecCounter1ToJumpDownCliff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x493f, 0x4941); E = 0x06;
  CYC(0x4941, 0x4942); A = mem_rd(gb, DE);
  CYC(0x4942, 0x4943); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4943, 0x4945);
    goto animate;
  }
  CYC(0x4943, 0x4945);
  CYC(0x4945, 0x4946); A = alu_dec8(gb, A);
  CYC(0x4946, 0x4947); mem_wr(gb, DE, A);
  CYC(0x4947, 0x4949); A = 0x53;
  CYC(0x4949, 0x494a); alu_scf(gb);
  if (!(F & FZ)) {
    CYCT(0x494a, 0x494b); ret_effect(gb); return;
  }
  CYC(0x494a, 0x494b);
  CALL_C(0x494b, playSound_b00_hook, 0x0c98, 0x494e);
  CYC(0x494e, 0x494f); alu_xor(gb, A);
  CYC(0x494f, 0x4950); alu_scf(gb);
  CYC(0x4950, 0x4951); ret_effect(gb);
  return;

animate:
  CALL_C(0x4951, specialObjectAnimate_hook, 0x2aef, 0x4954);
  CALL_C(0x4954, objectApplySpeed_hook, 0x201d, 0x4957);
  CYC(0x4957, 0x4959); C = 0x40;
  CALL_C(0x4959, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x495c);
  CYC(0x495c, 0x495d); alu_or(gb, D);
  CYC(0x495d, 0x495e); ret_effect(gb);
}

void companionDecCounter1IfNonzero_hook(GB *gb) {
  CYC(0x495e, 0x495f); H = D;
  CYC(0x495f, 0x4961); L = 0x06;
  CYC(0x4961, 0x4962); A = mem_rd(gb, HL);
  CYC(0x4962, 0x4963); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4963, 0x4964); ret_effect(gb); return;
  }
  CYC(0x4963, 0x4964);
  CYC(0x4964, 0x4965); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4965, 0x4966); ret_effect(gb);
}

void companionAnimateDrowningOrFallingThenRespawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4966, specialObjectAnimate_hook, 0x2aef, 0x4969);
  CYC(0x4969, 0x496b); E = 0x21;
  CYC(0x496b, 0x496c); A = mem_rd(gb, DE);
  CYC(0x496c, 0x496d); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x496d, 0x496e); ret_effect(gb); return;
  }
  CYC(0x496d, 0x496e);
  CALL_C(0x496e, companionRespawn_hook, 0x46e6, 0x4971);
  CYC(0x4971, 0x4972); alu_scf(gb);
  CYC(0x4972, 0x4973); ret_effect(gb);
}

void companionInitializeOnEnteringScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4973, companionCheckCanSpawn_hook, 0x4822, 0x4976);
  CYC(0x4976, 0x4978); L = 0x04;
  CYC(0x4978, 0x497a); mem_wr(gb, HL, 0x0c);
  CYC(0x497a, 0x497c); L = 0x03;
  CYC(0x497c, 0x497d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x497d, 0x497f); L = 0x07;
  CYC(0x497f, 0x4982);
  objectSetVisiblec1_hook(gb);
}

void companionRetIfNotFinishedWalkingIn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4982, specialObjectGetRelativeTileWithDirectionTable_hook, 0x44f1, 0x4985);
  CYC(0x4985, 0x4986); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4986, 0x4987); ret_effect(gb); return;
  }
  CYC(0x4986, 0x4987);
  CYC(0x4987, 0x4989); E = 0x07;
  CYC(0x4989, 0x498a); A = mem_rd(gb, DE);
  CYC(0x498a, 0x498b); A = alu_dec8(gb, A);
  CYC(0x498b, 0x498c); mem_wr(gb, DE, A);
  if (F & FZ) {
    CYCT(0x498c, 0x498d); ret_effect(gb); return;
  }
  CYC(0x498c, 0x498d);
  CYC(0x498d, 0x498e); SET_AF(pop_effect(gb));
  CYC(0x498e, 0x498f); ret_effect(gb);
}
