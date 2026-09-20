#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(doorController_updateLinkRespawn);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+3);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wLinkLocalRespawnY);
  CYC(b_+7, b_+9); alu_and(gb, 0xf0);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+13); A = mem_rd(gb, wLinkLocalRespawnX);
  CYC(b_+13, b_+15); alu_and(gb, 0xf0);
  CYC(b_+15, b_+17); alu_swap_a(gb);
  CYC(b_+17, b_+18); alu_or(gb, B);
  CYC(b_+18, b_+19); alu_cp(gb, C);
  if (!(F & FZ)) {
    RET_TAKEN(b_+19); return;
  }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_rrca(gb);
  CYC(b_+24, b_+26); alu_and(gb, 0x03);
  CYC(b_+26, b_+29); SET_HL(b_+51);
  CYC(b_+29, b_+30); doorController_addAToHl_from_rst(gb, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+32); alu_add(gb, C);
  CYC(b_+32, b_+33); C = A;
  CYC(b_+33, b_+35); alu_and(gb, 0xf0);
  CYC(b_+35, b_+37); alu_or(gb, 0x08);
  CYC(b_+37, b_+40); mem_wr(gb, wLinkLocalRespawnY, A);
  CYC(b_+40, b_+41); A = C;
  CYC(b_+41, b_+43); alu_swap_a(gb);
  CYC(b_+43, b_+45); alu_and(gb, 0xf0);
  CYC(b_+45, b_+47); alu_or(gb, 0x08);
  CYC(b_+47, b_+50); mem_wr(gb, wLinkLocalRespawnX, A);
  RET(b_+50);
}

static void doorController_checkTileIsShutterDoor(GB *gb, uint16_t sp0_) {
  BASE(doorController_decideActionBasedOnTriggers);
  CYC(b_+32, b_+34); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+37); alu_sub(gb, 0x10);
  CYC(b_+37, b_+39); A = alu_srl(gb, A);
  CYC(b_+39, b_+42); SET_HL(b_+52);
  CYC(b_+42, b_+43); doorController_addAToHl_from_rst(gb, b_+43);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); C = A;
  CYC(b_+47, b_+49); B = 0xcf;
  CYC(b_+49, b_+50); A = mem_rd(gb, BC);
  CYC(b_+50, b_+51); alu_cp(gb, mem_rd(gb, HL));
  RET(b_+51);
}

static void doorController_checkTileCollision(GB *gb, uint16_t sp0_) {
  BASE(doorController_decideActionBasedOnTriggers);
  CYC(b_+56, b_+58); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); C = A;
  CYC(b_+60, b_+62); B = 0xce;
  CYC(b_+62, b_+63); A = mem_rd(gb, BC);
  CYC(b_+63, b_+64); alu_or(gb, A);
  RET(b_+64);
}

void doorController_decideActionBasedOnTriggers_hook(GB *gb) {
  BASE(doorController_decideActionBasedOnTriggers);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+7, b_+8); alu_and(gb, B);
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    goto triggerInactive;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); push_effect(gb, b_+13); doorController_checkTileIsShutterDoor(gb, sp0_);
  CYC(b_+13, b_+15); A = 0x01;
  if (F & FZ) {
    CYCT(b_+15, b_+17);
    goto end;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+20);
  goto end;

triggerInactive:
  CYC(b_+20, b_+23); push_effect(gb, b_+23); doorController_checkTileCollision(gb, sp0_);
  CYC(b_+23, b_+25); A = 0x02;
  if (F & FZ) {
    CYCT(b_+25, b_+27);
    goto end;
  }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); alu_xor(gb, A);

end:
  CYC(b_+28, b_+31); mem_wr(gb, wTmpcfc0_normal_doorControllerState, A);
  RET(b_+31);
}

void doorController_checkMinecartCollidedWithDoor_hook(GB *gb) {
  BASE(doorController_checkMinecartCollidedWithDoor);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0_normal_doorControllerState, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+7, b_+8); alu_rrca(gb);
  if (!(F & FC)) {
    RET_TAKEN(b_+8); return;
  }
  CALL_C(b_+9, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+12);
  if (!(F & FC)) {
    RET_TAKEN(b_+12); return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+18); mem_wr(gb, wTmpcfc0_normal_doorControllerState, A);
  RET(b_+18);
}

void doorController_checkTileIsMinecartTrack_hook(GB *gb) {
  BASE(doorController_checkTileIsMinecartTrack);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); B = 0xcf;
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+9); alu_cp(gb, 0x5d);
  CYC(b_+9, b_+11); B = 0x01;
  if (F & FZ) {
    CYCT(b_+11, b_+13);
    goto done;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_cp(gb, 0x5e);
  if (F & FZ) {
    CYCT(b_+15, b_+17);
    goto done;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); B = alu_dec8(gb, B);

done:
  CYC(b_+18, b_+19); A = B;
  CYC(b_+19, b_+22); mem_wr(gb, wTmpcfc0_normal_doorControllerState, A);
  RET(b_+22);
}

void doorController_checkEnoughTorchesLit_hook(GB *gb) {
  BASE(doorController_checkEnoughTorchesLit);
  CYC(b_+0, b_+3); A = mem_rd(gb, wNumTorchesLit);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_cp(gb, B);
  CYC(b_+8, b_+10); A = 0x01;
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    goto done;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); A = alu_dec8(gb, A);

done:
  CYC(b_+13, b_+16); mem_wr(gb, wEnemyPlacement, A);
  RET(b_+16);
}
