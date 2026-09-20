#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void itemCheckWithinRangeOfLink_hook(GB *gb);

static uint16_t boomerang_jump_table(GB *gb) {
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

void itemCode06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x569d, 0x569f); E = 0x04;
  CYC(0x569f, 0x56a0); A = mem_rd(gb, DE);
  CYC(0x56a0, 0x56a1); push_effect(gb, 0x56a1);
  switch (boomerang_jump_table(gb)) {
    case 0x56ab: goto state0;
    case 0x56dc: goto state1;
    case 0x5730: goto state2;
    case 0x5741: goto state3;
    case 0x575d: goto state4;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x56ab, itemLoadAttributesAndGraphics_hook, 0x4993, 0x56ae);
  CYC(0x56ae, 0x56b0); A = 0x18;
  CALL_C(0x56b0, loadWeaponGfx_b00_hook, 0x166d, 0x56b3);
  CALL_C(0x56b3, itemIncState_hook, 0x23ea, 0x56b6);
  CYC(0x56b6, 0x56b8); L = 0x10;
  CYC(0x56b8, 0x56ba); mem_wr(gb, HL, 0x41);
  CYC(0x56ba, 0x56bc); L = 0x06;
  CYC(0x56bc, 0x56be); mem_wr(gb, HL, 0x28);
  CYC(0x56be, 0x56c0); C = 0xff;
  CYC(0x56c0, 0x56c2); A = 0x0d;
  CALL_C(0x56c2, cpActiveRing_hook, 0x23b0, 0x56c5);
  if (F & FZ) {
    CYCT(0x56c5, 0x56c7);
    goto increase_damage;
  }
  CYC(0x56c5, 0x56c7);
  CYC(0x56c7, 0x56c9); A = 0x29;
  CALL_C(0x56c9, cpActiveRing_hook, 0x23b0, 0x56cc);
  if (!(F & FZ)) {
    CYCT(0x56cc, 0x56ce);
    goto make_visible;
  }
  CYC(0x56cc, 0x56ce);
  CYC(0x56ce, 0x56d0); C = 0xfe;

increase_damage:
  CYC(0x56d0, 0x56d2); L = 0x28;
  CYC(0x56d2, 0x56d3); A = mem_rd(gb, HL);
  CYC(0x56d3, 0x56d4); alu_add(gb, C);
  CYC(0x56d4, 0x56d5); mem_wr(gb, HL, A);

make_visible:
  CALL_C(0x56d5, objectSetVisible82_hook, 0x1e69, 0x56d8);
  CYC(0x56d8, 0x56d9); alu_xor(gb, A);
  CYC(0x56d9, 0x56dc);
  itemSetAnimation_hook(gb);
  return;

state1:
  CYC(0x56dc, 0x56de); E = 0x2a;
  CYC(0x56de, 0x56df); A = mem_rd(gb, DE);
  CYC(0x56df, 0x56e0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x56e0, 0x56e2);
    goto return_to_link;
  }
  CYC(0x56e0, 0x56e2);
  CALL_C(0x56e2, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x56e5);
  if (!(F & FC)) {
    CYCT(0x56e5, 0x56e7);
    goto no_collision;
  }
  CYC(0x56e5, 0x56e7);
  CALL_C(0x56e7, itemCheckCanPassSolidTile_hook, 0x4b95, 0x56ea);
  if (!(F & FZ)) {
    CYCT(0x56ea, 0x56ec);
    goto hit_wall;
  }
  CYC(0x56ea, 0x56ec);

no_collision:
  CALL_C(0x56ec, objectCheckWithinRoomBoundary_hook, 0x219f, 0x56ef);
  if (!(F & FC)) {
    CYCT(0x56ef, 0x56f1);
    goto return_to_link;
  }
  CYC(0x56ef, 0x56f1);
  CYC(0x56f1, 0x56f3); E = 0x34;
  CYC(0x56f3, 0x56f4); A = mem_rd(gb, DE);
  CALL_C(0x56f4, objectNudgeAngleTowards_hook, 0x1fd4, 0x56f7);
  CALL_C(0x56f7, itemDecCounter1_hook, 0x23d6, 0x56fa);
  if (!(F & FZ)) {
    CYCT(0x56fa, 0x56fc);
    goto update_speed_and_animation;
  }
  CYC(0x56fa, 0x56fc);

return_to_link:
  CALL_C(0x56fc, objectGetAngleTowardLink_hook, 0x1e9c, 0x56ff);
  CYC(0x56ff, 0x5700); C = A;
  CYC(0x5700, 0x5701); H = D;
  CYC(0x5701, 0x5703); L = 0x0b;
  CYC(0x5703, 0x5705); A = 0xf0;
  CYC(0x5705, 0x5706); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x5706, 0x5708);
    goto set_angle;
  }
  CYC(0x5706, 0x5708);
  CYC(0x5708, 0x570a); L = 0x0d;
  CYC(0x570a, 0x570b); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x570b, 0x570d);
    goto set_angle;
  }
  CYC(0x570b, 0x570d);
  CYC(0x570d, 0x570f); L = 0x09;
  CYC(0x570f, 0x5710); A = C;
  CYC(0x5710, 0x5711); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5711, 0x5713); alu_add(gb, 0x08);
  CYC(0x5713, 0x5715); alu_cp(gb, 0x11);
  if (F & FC) {
    CYCT(0x5715, 0x5717);
    goto next_state;
  }
  CYC(0x5715, 0x5717);

set_angle:
  CYC(0x5717, 0x5719); L = 0x09;
  CYC(0x5719, 0x571a); mem_wr(gb, HL, C);
  CYC(0x571a, 0x571c);
  goto next_state;

hit_wall:
  CALL_C(0x571c, objectCreateClinkInteraction_hook, 0x4b81, 0x571f);
  CYC(0x571f, 0x5720); H = D;
  CYC(0x5720, 0x5722); L = 0x09;
  CYC(0x5722, 0x5723); A = mem_rd(gb, HL);
  CYC(0x5723, 0x5725); alu_xor(gb, 0x10);
  CYC(0x5725, 0x5726); mem_wr(gb, HL, A);

next_state:
  CYC(0x5726, 0x5728); L = 0x04;
  CYC(0x5728, 0x5729); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5729, 0x572b); L = 0x16;
  CYC(0x572b, 0x572c); alu_xor(gb, A);
  CYC(0x572c, 0x572d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x572d, 0x572e); mem_wr(gb, HL, A);
  CYC(0x572e, 0x5730);
  goto update_speed_and_animation;

state2:
  CALL_C(0x5730, objectGetAngleTowardLink_hook, 0x1e9c, 0x5733);
  CALL_C(0x5733, objectNudgeAngleTowards_hook, 0x1fd4, 0x5736);
  CYC(0x5736, 0x5739); SET_BC(0x140a);
  CALL_C(0x5739, itemCheckWithinRangeOfLink_hook, 0x577e, 0x573c);
  if (F & FC) CALL_C_CC(0x573c, itemIncState_hook, 0x23ea, 0x573f);
  else CYC(0x573c, 0x573f);
  CYC(0x573f, 0x5741);
  goto update_speed_and_animation;

state3:
  CALL_C(0x5741, objectGetAngleTowardLink_hook, 0x1e9c, 0x5744);
  CYC(0x5744, 0x5746); E = 0x09;
  CYC(0x5746, 0x5747); mem_wr(gb, DE, A);
  CYC(0x5747, 0x574a); SET_BC(0x0402);
  CALL_C(0x574a, itemCheckWithinRangeOfLink_hook, 0x577e, 0x574d);
  if (!(F & FC)) {
    CYCT(0x574d, 0x574f);
    goto update_speed_and_animation;
  }
  CYC(0x574d, 0x574f);
  CALL_C(0x574f, itemIncState_hook, 0x23ea, 0x5752);
  CYC(0x5752, 0x5754); L = 0x06;
  CYC(0x5754, 0x5756); mem_wr(gb, HL, 0x04);
  CYC(0x5756, 0x5758); L = 0x24;
  CYC(0x5758, 0x575a); mem_wr(gb, HL, 0x00);
  CYC(0x575a, 0x575d);
  objectSetInvisible_hook(gb);
  return;

state4:
  CALL_C(0x575d, itemDecCounter1_hook, 0x23d6, 0x5760);
  if (F & FZ) {
    CYCT(0x5760, 0x5763);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x5760, 0x5763);
  CYC(0x5763, 0x5766); A = W8(wLinkObjectIndex);
  CYC(0x5766, 0x5767); H = A;
  CYC(0x5767, 0x5769); L = 0x0b;
  CYC(0x5769, 0x576c);
  objectTakePosition_hook(gb);
  return;

update_speed_and_animation:
  CALL_C(0x576c, objectApplySpeed_hook, 0x201d, 0x576f);
  CYC(0x576f, 0x5770); H = D;
  CYC(0x5770, 0x5772); L = 0x21;
  CYC(0x5772, 0x5773); A = mem_rd(gb, HL);
  CYC(0x5773, 0x5774); alu_or(gb, A);
  CYC(0x5774, 0x5776); mem_wr(gb, HL, 0x00);
  CYC(0x5776, 0x5778); A = 0x78;
  if (!(F & FZ)) CALL_C_CC(0x5778, playSound_b00_hook, 0x0c98, 0x577b);
  else CYC(0x5778, 0x577b);
  CYC(0x577b, 0x577e);
  itemAnimate_hook(gb);
}

void itemCheckWithinRangeOfLink_hook(GB *gb) {
  CYC(0x577e, 0x5781); SET_HL(w1Link_yh);
  CYC(0x5781, 0x5783); E = 0x0b;
  CYC(0x5783, 0x5784); A = mem_rd(gb, DE);
  CYC(0x5784, 0x5785); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5785, 0x5786); alu_add(gb, C);
  CYC(0x5786, 0x5787); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(0x5787, 0x5788); ret_effect(gb); return;
  }
  CYC(0x5787, 0x5788);
  CYC(0x5788, 0x578a); L = 0x0d;
  CYC(0x578a, 0x578c); E = 0x0d;
  CYC(0x578c, 0x578d); A = mem_rd(gb, DE);
  CYC(0x578d, 0x578e); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x578e, 0x578f); alu_add(gb, C);
  CYC(0x578f, 0x5790); alu_cp(gb, B);
  CYC(0x5790, 0x5791); ret_effect(gb);
}
