#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void doorController_updateLinkRespawn_hook(GB *gb);
void doorController_decideActionBasedOnTriggers_hook(GB *gb);
void doorController_checkMinecartCollidedWithDoor_hook(GB *gb);
void doorController_checkTileIsMinecartTrack_hook(GB *gb);
void doorController_checkEnoughTorchesLit_hook(GB *gb);

static void doorController_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void doorController_updateLinkRespawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4069, objectGetShortPosition_hook, 0x2096, 0x406c);
  CYC(0x406c, 0x406d); C = A;
  CYC(0x406d, 0x4070); A = mem_rd(gb, wLinkLocalRespawnY);
  CYC(0x4070, 0x4072); alu_and(gb, 0xf0);
  CYC(0x4072, 0x4073); B = A;
  CYC(0x4073, 0x4076); A = mem_rd(gb, wLinkLocalRespawnX);
  CYC(0x4076, 0x4078); alu_and(gb, 0xf0);
  CYC(0x4078, 0x407a); alu_swap_a(gb);
  CYC(0x407a, 0x407b); alu_or(gb, B);
  CYC(0x407b, 0x407c); alu_cp(gb, C);
  if (!(F & FZ)) {
    RET_TAKEN(0x407c); return;
  }
  CYC(0x407c, 0x407d);
  CYC(0x407d, 0x407f); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x407f, 0x4080); A = mem_rd(gb, DE);
  CYC(0x4080, 0x4081); alu_rrca(gb);
  CYC(0x4081, 0x4083); alu_and(gb, 0x03);
  CYC(0x4083, 0x4086); SET_HL(0x409c);
  CYC(0x4086, 0x4087); doorController_addAToHl_from_rst(gb, 0x4087);
  CYC(0x4087, 0x4088); A = mem_rd(gb, HL);
  CYC(0x4088, 0x4089); alu_add(gb, C);
  CYC(0x4089, 0x408a); C = A;
  CYC(0x408a, 0x408c); alu_and(gb, 0xf0);
  CYC(0x408c, 0x408e); alu_or(gb, 0x08);
  CYC(0x408e, 0x4091); mem_wr(gb, wLinkLocalRespawnY, A);
  CYC(0x4091, 0x4092); A = C;
  CYC(0x4092, 0x4094); alu_swap_a(gb);
  CYC(0x4094, 0x4096); alu_and(gb, 0xf0);
  CYC(0x4096, 0x4098); alu_or(gb, 0x08);
  CYC(0x4098, 0x409b); mem_wr(gb, wLinkLocalRespawnX, A);
  RET(0x409b);
}

static void doorController_checkTileIsShutterDoor(GB *gb, uint16_t sp0_) {
  CYC(0x40c0, 0x40c2); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x40c2, 0x40c3); A = mem_rd(gb, DE);
  CYC(0x40c3, 0x40c5); alu_sub(gb, 0x10);
  CYC(0x40c5, 0x40c7); A = alu_srl(gb, A);
  CYC(0x40c7, 0x40ca); SET_HL(0x40d4);
  CYC(0x40ca, 0x40cb); doorController_addAToHl_from_rst(gb, 0x40cb);
  CYC(0x40cb, 0x40cd); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x40cd, 0x40ce); A = mem_rd(gb, DE);
  CYC(0x40ce, 0x40cf); C = A;
  CYC(0x40cf, 0x40d1); B = 0xcf;
  CYC(0x40d1, 0x40d2); A = mem_rd(gb, BC);
  CYC(0x40d2, 0x40d3); alu_cp(gb, mem_rd(gb, HL));
  RET(0x40d3);
}

static void doorController_checkTileCollision(GB *gb, uint16_t sp0_) {
  CYC(0x40d8, 0x40da); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x40da, 0x40db); A = mem_rd(gb, DE);
  CYC(0x40db, 0x40dc); C = A;
  CYC(0x40dc, 0x40de); B = 0xce;
  CYC(0x40de, 0x40df); A = mem_rd(gb, BC);
  CYC(0x40df, 0x40e0); alu_or(gb, A);
  RET(0x40e0);
}

void doorController_decideActionBasedOnTriggers_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40a0, 0x40a2); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x40a2, 0x40a3); A = mem_rd(gb, DE);
  CYC(0x40a3, 0x40a4); B = A;
  CYC(0x40a4, 0x40a7); A = mem_rd(gb, wActiveTriggers);
  CYC(0x40a7, 0x40a8); alu_and(gb, B);
  if (F & FZ) {
    CYCT(0x40a8, 0x40aa);
    goto triggerInactive;
  }
  CYC(0x40a8, 0x40aa);
  CYC(0x40aa, 0x40ad); push_effect(gb, 0x40ad); doorController_checkTileIsShutterDoor(gb, sp0_);
  CYC(0x40ad, 0x40af); A = 0x01;
  if (F & FZ) {
    CYCT(0x40af, 0x40b1);
    goto end;
  }
  CYC(0x40af, 0x40b1);
  CYC(0x40b1, 0x40b2); alu_xor(gb, A);
  CYC(0x40b2, 0x40b4);
  goto end;

triggerInactive:
  CYC(0x40b4, 0x40b7); push_effect(gb, 0x40b7); doorController_checkTileCollision(gb, sp0_);
  CYC(0x40b7, 0x40b9); A = 0x02;
  if (F & FZ) {
    CYCT(0x40b9, 0x40bb);
    goto end;
  }
  CYC(0x40b9, 0x40bb);
  CYC(0x40bb, 0x40bc); alu_xor(gb, A);

end:
  CYC(0x40bc, 0x40bf); mem_wr(gb, wTmpcfc0_normal_doorControllerState, A);
  RET(0x40bf);
}

void doorController_checkMinecartCollidedWithDoor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40e1, 0x40e2); alu_xor(gb, A);
  CYC(0x40e2, 0x40e5); mem_wr(gb, wTmpcfc0_normal_doorControllerState, A);
  CYC(0x40e5, 0x40e8); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x40e8, 0x40e9); alu_rrca(gb);
  if (!(F & FC)) {
    RET_TAKEN(0x40e9); return;
  }
  CALL_C(0x40ea, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x40ed);
  if (!(F & FC)) {
    RET_TAKEN(0x40ed); return;
  }
  CYC(0x40ed, 0x40ee);
  CYC(0x40ee, 0x40f0); A = 0x01;
  CYC(0x40f0, 0x40f3); mem_wr(gb, wTmpcfc0_normal_doorControllerState, A);
  RET(0x40f3);
}

void doorController_checkTileIsMinecartTrack_hook(GB *gb) {
  CYC(0x40f4, 0x40f6); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x40f6, 0x40f7); A = mem_rd(gb, DE);
  CYC(0x40f7, 0x40f8); C = A;
  CYC(0x40f8, 0x40fa); B = 0xcf;
  CYC(0x40fa, 0x40fb); A = mem_rd(gb, BC);
  CYC(0x40fb, 0x40fd); alu_cp(gb, 0x5d);
  CYC(0x40fd, 0x40ff); B = 0x01;
  if (F & FZ) {
    CYCT(0x40ff, 0x4101);
    goto done;
  }
  CYC(0x40ff, 0x4101);
  CYC(0x4101, 0x4103); alu_cp(gb, 0x5e);
  if (F & FZ) {
    CYCT(0x4103, 0x4105);
    goto done;
  }
  CYC(0x4103, 0x4105);
  CYC(0x4105, 0x4106); B = alu_dec8(gb, B);

done:
  CYC(0x4106, 0x4107); A = B;
  CYC(0x4107, 0x410a); mem_wr(gb, wTmpcfc0_normal_doorControllerState, A);
  RET(0x410a);
}

void doorController_checkEnoughTorchesLit_hook(GB *gb) {
  CYC(0x410b, 0x410e); A = mem_rd(gb, wNumTorchesLit);
  CYC(0x410e, 0x410f); B = A;
  CYC(0x410f, 0x4111); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x4111, 0x4112); A = mem_rd(gb, DE);
  CYC(0x4112, 0x4113); alu_cp(gb, B);
  CYC(0x4113, 0x4115); A = 0x01;
  if (F & FZ) {
    CYCT(0x4115, 0x4117);
    goto done;
  }
  CYC(0x4115, 0x4117);
  CYC(0x4117, 0x4118); A = alu_dec8(gb, A);

done:
  CYC(0x4118, 0x411b); mem_wr(gb, wEnemyPlacement, A);
  RET(0x411b);
}
