#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

#define shieldPositionOffsets_bank07 0x4231
#define enemyActiveCollisions_bank07 0x69a2
#define partActiveCollisions_bank07 0x6ba2
#define objectCollisionTable_bank07 0x6d0a

void label_07_027_hook(GB *gb);
void label_07_028_hook(GB *gb);
void applyDamageToBothObjects_hook(GB *gb);
void collisionEffect0c_hook(GB *gb);
void collisionEffect0d_hook(GB *gb);
void collisionEffect0e_hook(GB *gb);
void collisionEffect0f_hook(GB *gb);
void collisionEffect10_hook(GB *gb);
void collisionEffect11_hook(GB *gb);
void collisionEffect3d_hook(GB *gb);
void label_07_033_hook(GB *gb);
void killEnemyOrPart_hook(GB *gb);
void createFlamePart_hook(GB *gb);
void createClinkInteraction_hook(GB *gb);
void applyDamageToEnemyOrPart_hook(GB *gb);
void func_07_47b7_hook(GB *gb);
void applyDamageToLink_paramE_hook(GB *gb);
void applyDamageToLink_hook(GB *gb);
void partCheckCollisions_hook(GB *gb);
void enemyCheckCollisions_hook(GB *gb);

static uint16_t collision_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void collision_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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

static void collision_check_flag(GB *gb) {
  CYC(0x432b, 0x432c); B = A;
  CYC(0x432c, 0x432e); alu_and(gb, 0xf8);
  CYC(0x432e, 0x432f); alu_rlca(gb);
  CYC(0x432f, 0x4331); A = alu_swap(gb, A);
  CYC(0x4331, 0x4332); C = A;
  CYC(0x4332, 0x4333); A = B;
  CYC(0x4333, 0x4335); alu_and(gb, 0x07);
  CYC(0x4335, 0x4337); B = 0x00;
  CYC(0x4337, 0x4338); alu_add_hl(gb, BC);
  CYC(0x4338, 0x4339); C = mem_rd(gb, HL);
  CYC(0x4339, 0x433c); SET_HL(0x00f8);
  CYC(0x433c, 0x433d); alu_add(gb, L);
  CYC(0x433d, 0x433e); L = A;
  CYC(0x433e, 0x433f); A = mem_rd(gb, HL);
  CYC(0x433f, 0x4340); alu_and(gb, C);
  CYC(0x4340, 0x4341); ret_effect(gb);
}

static void collision_check_object(GB *gb, uint16_t sp0_) {
  CYC(0x4250, 0x4251); alu_add(gb, A);
  CYC(0x4251, 0x4252); C = A;
  CYC(0x4252, 0x4254); B = 0x00;
  CYC(0x4254, 0x4255); alu_add_hl(gb, BC);
  CYC(0x4255, 0x4256); alu_add_hl(gb, BC);
  CYC(0x4256, 0x4257); A = L;
  CYC(0x4257, 0x4259); H8(hFF92) = A;
  CYC(0x4259, 0x425a); A = H;
  CYC(0x425a, 0x425c); H8(hFF93) = A;
  CYC(0x425c, 0x425d); H = D;
  CYC(0x425d, 0x425e); L = E;
  CYC(0x425e, 0x425f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x425f, 0x4261); H8(hFF8F) = A;
  CYC(0x4261, 0x4262); L = alu_inc8(gb, L);
  CYC(0x4262, 0x4263); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4263, 0x4265); H8(hFF8E) = A;
  CYC(0x4265, 0x4266); L = alu_inc8(gb, L);
  CYC(0x4266, 0x4267); A = mem_rd(gb, HL);
  CYC(0x4267, 0x4269); H8(hFF91) = A;
  CYC(0x4269, 0x426a); A = L;
  CYC(0x426a, 0x426c); alu_add(gb, 0x1c);
  CYC(0x426c, 0x426d); L = A;
  CYC(0x426d, 0x426e); A = mem_rd(gb, HL);
  CYC(0x426e, 0x426f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x426f, 0x4271);
    goto done_checking_items;
  }
  CYC(0x426f, 0x4271);
  CYC(0x4271, 0x4273); H = 0xd6;

check_item:
  CYC(0x4273, 0x4275); L = 0x24;
  CYC(0x4275, 0x4276); A = mem_rd(gb, HL);
  CYC(0x4276, 0x4278); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x4278, 0x427a);
    goto next_item;
  }
  CYC(0x4278, 0x427a);
  CYC(0x427a, 0x427c); alu_and(gb, 0x7f);
  CYC(0x427c, 0x427e); H8(hFF90) = A;
  CYC(0x427e, 0x427f); B = A;
  CYC(0x427f, 0x4280); E = H;
  CYC(0x4280, 0x4282); A = H8(hFF92);
  CYC(0x4282, 0x4283); L = A;
  CYC(0x4283, 0x4285); A = H8(hFF93);
  CYC(0x4285, 0x4286); H = A;
  CYC(0x4286, 0x4287); A = B;
  CYC(0x4287, 0x428a); push_effect(gb, 0x428a); collision_check_flag(gb);
  CYC(0x428a, 0x428b); H = E;
  if (F & FZ) {
    CYCT(0x428b, 0x428d);
    goto next_item;
  }
  CYC(0x428b, 0x428d);
  CYC(0x428d, 0x4290); SET_BC(0x0e07);
  CYC(0x4290, 0x4292); A = H8(hFF90);
  CYC(0x4292, 0x4294); alu_cp(gb, 0x18);
  if (!(F & FZ)) {
    CYCT(0x4294, 0x4296);
  } else {
    CYC(0x4294, 0x4296);
    CYC(0x4296, 0x4298); L = 0x26;
    CYC(0x4298, 0x4299); A = mem_rd(gb, HL);
    CYC(0x4299, 0x429a); C = A;
    CYC(0x429a, 0x429b); alu_add(gb, A);
    CYC(0x429b, 0x429c); B = A;
  }
  CYC(0x429c, 0x429e); L = 0x0f;
  CYC(0x429e, 0x42a0); A = H8(hFF91);
  CYC(0x42a0, 0x42a1); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x42a1, 0x42a2); alu_add(gb, C);
  CYC(0x42a2, 0x42a3); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(0x42a3, 0x42a5);
    goto next_item;
  }
  CYC(0x42a3, 0x42a5);
  CYC(0x42a5, 0x42a7); L = 0x0b;
  CYC(0x42a7, 0x42a8); B = mem_rd(gb, HL);
  CYC(0x42a8, 0x42aa); L = 0x0d;
  CYC(0x42aa, 0x42ab); C = mem_rd(gb, HL);
  CYC(0x42ab, 0x42ad); L = 0x26;
  CYC(0x42ad, 0x42af); A = H8(hActiveObjectType);
  CYC(0x42af, 0x42b1); alu_add(gb, 0x26);
  CYC(0x42b1, 0x42b2); E = A;
  CALL_C(0x42b2, checkObjectsCollidedFromVariables_hook, 0x1c04, 0x42b5);
  if (F & FC) {
    CYCT(0x42b5, 0x42b8);
    goto handle_collision;
  }
  CYC(0x42b5, 0x42b8);

next_item:
  CYC(0x42b8, 0x42b9); H = alu_inc8(gb, H);
  CYC(0x42b9, 0x42ba); A = H;
  CYC(0x42ba, 0x42bc); alu_cp(gb, 0xde);
  if (F & FC) {
    CYCT(0x42bc, 0x42be);
    goto check_item;
  }
  CYC(0x42bc, 0x42be);

done_checking_items:
  CALL_C(0x42be, checkLinkVulnerable_hook, 0x1d28, 0x42c1);
  if (!(F & FC)) { CYCT(0x42c1, 0x42c2); ret_effect(gb); return; }
  CYC(0x42c1, 0x42c2);
  CYC(0x42c2, 0x42c4); L = 0x0f;
  CYC(0x42c4, 0x42c6); A = H8(hFF91);
  CYC(0x42c6, 0x42c7); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x42c7, 0x42c9); alu_add(gb, 0x07);
  CYC(0x42c9, 0x42cb); alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(0x42cb, 0x42cc); ret_effect(gb); return; }
  CYC(0x42cb, 0x42cc);
  CYC(0x42cc, 0x42cf); A = W8(wUsingShield);
  CYC(0x42cf, 0x42d0); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x42d0, 0x42d2);
    goto check_hit_link;
  }
  CYC(0x42d0, 0x42d2);
  CYC(0x42d2, 0x42d4); H8(hFF90) = A;
  CYC(0x42d4, 0x42d6); A = H8(hFF92);
  CYC(0x42d6, 0x42d7); L = A;
  CYC(0x42d7, 0x42d9); A = H8(hFF93);
  CYC(0x42d9, 0x42da); H = A;
  CYC(0x42da, 0x42dc); A = H8(hFF90);
  CYC(0x42dc, 0x42df); push_effect(gb, 0x42df); collision_check_flag(gb);
  if (F & FZ) {
    CYCT(0x42df, 0x42e1);
    goto check_hit_link;
  }
  CYC(0x42df, 0x42e1);
  CYC(0x42e1, 0x42e4); SET_HL(wShieldY);
  CYC(0x42e4, 0x42e5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x42e5, 0x42e6); B = A;
  CYC(0x42e6, 0x42e7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x42e7, 0x42e8); C = A;
  CYC(0x42e8, 0x42ea); A = H8(hActiveObjectType);
  CYC(0x42ea, 0x42ec); alu_add(gb, 0x26);
  CYC(0x42ec, 0x42ed); E = A;
  CALL_C(0x42ed, checkObjectsCollidedFromVariables_hook, 0x1c04, 0x42f0);
  CYC(0x42f0, 0x42f3); SET_HL(w1Link);
  if (F & FC) {
    CYCT(0x42f3, 0x42f6);
    goto handle_collision;
  }
  CYC(0x42f3, 0x42f6);

check_hit_link:
  CYC(0x42f6, 0x42f8); A = H8(hActiveObjectType);
  CYC(0x42f8, 0x42fa); alu_add(gb, 0x2e);
  CYC(0x42fa, 0x42fb); E = A;
  CYC(0x42fb, 0x42fc); A = mem_rd(gb, DE);
  CYC(0x42fc, 0x42fd); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x42fd, 0x42fe); ret_effect(gb); return; }
  CYC(0x42fd, 0x42fe);
  CYC(0x42fe, 0x4301); A = W8(wLinkObjectIndex);
  CYC(0x4301, 0x4302); H = A;
  CYC(0x4302, 0x4303); E = A;
  CYC(0x4303, 0x4305); L = 0x24;
  CYC(0x4305, 0x4306); A = mem_rd(gb, HL);
  CYC(0x4306, 0x4308); alu_and(gb, 0x7f);
  CYC(0x4308, 0x430a); H8(hFF90) = A;
  CYC(0x430a, 0x430c); A = H8(hFF92);
  CYC(0x430c, 0x430d); L = A;
  CYC(0x430d, 0x430f); A = H8(hFF93);
  CYC(0x430f, 0x4310); H = A;
  CYC(0x4310, 0x4312); A = H8(hFF90);
  CYC(0x4312, 0x4315); push_effect(gb, 0x4315); collision_check_flag(gb);
  if (F & FZ) { CYCT(0x4315, 0x4316); ret_effect(gb); return; }
  CYC(0x4315, 0x4316);
  CYC(0x4316, 0x4317); H = E;
  CYC(0x4317, 0x4319); L = 0x0b;
  CYC(0x4319, 0x431a); B = mem_rd(gb, HL);
  CYC(0x431a, 0x431c); L = 0x0d;
  CYC(0x431c, 0x431d); C = mem_rd(gb, HL);
  CYC(0x431d, 0x431f); L = 0x26;
  CYC(0x431f, 0x4321); A = H8(hActiveObjectType);
  CYC(0x4321, 0x4323); alu_add(gb, 0x26);
  CYC(0x4323, 0x4324); E = A;
  CALL_C(0x4324, checkObjectsCollidedFromVariables_hook, 0x1c04, 0x4327);
  if (F & FC) {
    CYCT(0x4327, 0x432a);
    goto handle_collision;
  }
  CYC(0x4327, 0x432a);
  CYC(0x432a, 0x432b); ret_effect(gb);
  return;

handle_collision:
  CYC(0x4341, 0x4342); A = L;
  CYC(0x4342, 0x4344); alu_and(gb, 0xc0);
  CYC(0x4344, 0x4345); L = A;
  CYC(0x4345, 0x4346); push_effect(gb, HL);
  CYC(0x4346, 0x4348); A = 0xd6;
  CYC(0x4348, 0x4349); alu_cp(gb, H);
  if (!(F & FZ)) {
    CYCT(0x4349, 0x434b);
    CYC(0x4354, 0x4356); A = H8(hFF8D);
    CYC(0x4356, 0x4357); B = A;
    CYC(0x4357, 0x4359); A = H8(hFF8C);
  } else {
    CYC(0x4349, 0x434b);
    CYC(0x434b, 0x434e); A = W8(w1Link_yh);
    CYC(0x434e, 0x434f); B = A;
    CYC(0x434f, 0x4352); A = W8(w1Link_xh);
    CYC(0x4352, 0x4354);
  }
  CYC(0x4359, 0x435a); C = A;
  CALL_C(0x435a, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x435d);
  CYC(0x435d, 0x435f); H8(hFF8A) = A;
  CYC(0x435f, 0x4361); A = H8(hActiveObjectType);
  CYC(0x4361, 0x4363); alu_add(gb, 0x25);
  CYC(0x4363, 0x4364); E = A;
  CYC(0x4364, 0x4365); A = mem_rd(gb, DE);
  CYC(0x4365, 0x4366); alu_add(gb, A);
  CALL_C(0x4366, multiplyABy16_hook, 0x01ac, 0x4369);
  CYC(0x4369, 0x436c); SET_HL(objectCollisionTable_bank07);
  CYC(0x436c, 0x436d); alu_add_hl(gb, BC);
  CYC(0x436d, 0x436e); SET_BC(pop_effect(gb));
  CYC(0x436e, 0x4370); A = H8(hFF90);
  CYC(0x4370, 0x4371); collision_add_a_to_hl_from_rst(gb, 0x4371);
  CYC(0x4371, 0x4372); A = mem_rd(gb, HL);
  CYC(0x4372, 0x4373); push_effect(gb, 0x4373);
  hook_continue(gb, collision_jump_table(gb), sp0_);
}

void checkEnemyAndPartCollisions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41d1, 0x41d4); A = W8(w1Link_direction);
  CYC(0x41d4, 0x41d5); alu_add(gb, A);
  CYC(0x41d5, 0x41d6); alu_add(gb, A);
  CYC(0x41d6, 0x41d9); SET_HL(shieldPositionOffsets_bank07);
  CYC(0x41d9, 0x41da); collision_add_a_to_hl_from_rst(gb, 0x41da);
  CYC(0x41da, 0x41dd); SET_DE(wShieldY);
  CYC(0x41dd, 0x41e0); A = W8(w1Link_yh);
  CYC(0x41e0, 0x41e1); alu_add(gb, mem_rd(gb, HL));
  CYC(0x41e1, 0x41e2); mem_wr(gb, DE, A);
  CYC(0x41e2, 0x41e3); SET_HL(HL + 1);
  CYC(0x41e3, 0x41e4); E = alu_inc8(gb, E);
  CYC(0x41e4, 0x41e7); A = W8(w1Link_xh);
  CYC(0x41e7, 0x41e8); alu_add(gb, mem_rd(gb, HL));
  CYC(0x41e8, 0x41e9); mem_wr(gb, DE, A);
  CYC(0x41e9, 0x41ea); SET_HL(HL + 1);
  CYC(0x41ea, 0x41eb); E = alu_inc8(gb, E);
  CYC(0x41eb, 0x41ec); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41ec, 0x41ed); mem_wr(gb, DE, A);
  CYC(0x41ed, 0x41ee); E = alu_inc8(gb, E);
  CYC(0x41ee, 0x41ef); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41ef, 0x41f0); mem_wr(gb, DE, A);
  CYC(0x41f0, 0x41f2); A = 0x80;
  CYC(0x41f2, 0x41f4); H8(hActiveObjectType) = A;
  CYC(0x41f4, 0x41f6); D = 0xd0;
  CYC(0x41f6, 0x41f7); A = D;

next_enemy:
  CYC(0x41f7, 0x41f9); H8(hActiveObject) = A;
  CYC(0x41f9, 0x41fa); H = D;
  CYC(0x41fa, 0x41fc); L = 0xa4;
  CYC(0x41fc, 0x41fe); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYC(0x41fe, 0x4200);
    CYC(0x4200, 0x4201); A = mem_rd(gb, HL);
    CYC(0x4201, 0x4203); L = 0xaa;
    CYC(0x4203, 0x4205); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) CALL_C_CC(0x4205, enemyCheckCollisions_hook, 0x424b, 0x4208);
    else CYC(0x4205, 0x4208);
  } else {
    CYCT(0x41fe, 0x4200);
  }
  CYC(0x4208, 0x4209); D = alu_inc8(gb, D);
  CYC(0x4209, 0x420a); A = D;
  CYC(0x420a, 0x420c); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(0x420c, 0x420e);
    goto next_enemy;
  }
  CYC(0x420c, 0x420e);
  CYC(0x420e, 0x4210); A = 0xc0;
  CYC(0x4210, 0x4212); H8(hActiveObjectType) = A;
  CYC(0x4212, 0x4214); D = 0xd0;
  CYC(0x4214, 0x4215); A = D;

next_part:
  CYC(0x4215, 0x4217); H8(hActiveObject) = A;
  CYC(0x4217, 0x4218); H = D;
  CYC(0x4218, 0x421a); L = 0xe4;
  CYC(0x421a, 0x421c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYC(0x421c, 0x421e);
    CYC(0x421e, 0x4220); L = 0xea;
    CYC(0x4220, 0x4222); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) {
      CYC(0x4222, 0x4224);
      CYC(0x4224, 0x4225); L = alu_inc8(gb, L);
      CYC(0x4225, 0x4226); A = mem_rd(gb, HL);
      CYC(0x4226, 0x4227); alu_or(gb, A);
      if (F & FZ) CALL_C_CC(0x4227, partCheckCollisions_hook, 0x4241, 0x422a);
      else CYC(0x4227, 0x422a);
    } else {
      CYCT(0x4222, 0x4224);
    }
  } else {
    CYCT(0x421c, 0x421e);
  }
  CYC(0x422a, 0x422b); D = alu_inc8(gb, D);
  CYC(0x422b, 0x422c); A = D;
  CYC(0x422c, 0x422e); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(0x422e, 0x4230);
    goto next_part;
  }
  CYC(0x422e, 0x4230);
  CYC(0x4230, 0x4231); ret_effect(gb);
}

void partCheckCollisions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4241, 0x4243); E = 0xe4;
  CYC(0x4243, 0x4244); A = mem_rd(gb, DE);
  CYC(0x4244, 0x4247); SET_HL(partActiveCollisions_bank07);
  CYC(0x4247, 0x4249); E = 0xcb;
  CYC(0x4249, 0x424b);
  collision_check_object(gb, sp0_);
}

void enemyCheckCollisions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x424b, 0x424e); SET_HL(enemyActiveCollisions_bank07);
  CYC(0x424e, 0x4250); E = 0x8b;
  collision_check_object(gb, sp0_);
}

static void collisionEffectDamageLinkTail_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x4434, applyDamageToLink_paramE_hook, 0x47df, 0x4437);
  CYC(0x4437, 0x4439); A = 0x1c;
  CYC(0x4439, 0x443c); applyDamageToEnemyOrPart_hook(gb);
}

void collisionEffect00_hook(GB *gb) {
  CYC(0x43f3, 0x43f4); ret_effect(gb);
}

void collisionEffect01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4426, 0x4428); E = 0x00;
  CYC(0x4428, 0x442a); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x442a, 0x442c); E = 0x04;
  CYC(0x442c, 0x442e); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x442e, 0x4430); E = 0x08;
  CYC(0x4430, 0x4432); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4432, 0x4434); E = 0x0c;
  collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect08_hook(GB *gb) {
  CYC(0x443c, 0x443e); E = 0x00;
  CYC(0x443e, 0x4440); label_07_027_hook(gb);
}

void collisionEffect09_hook(GB *gb) {
  CYC(0x4440, 0x4442); E = 0x04;
  CYC(0x4442, 0x4444); label_07_027_hook(gb);
}

void collisionEffect0a_hook(GB *gb) {
  CYC(0x4444, 0x4446); E = 0x08;
  CYC(0x4446, 0x4448); label_07_027_hook(gb);
}

void label_07_027_hook(GB *gb) {
  CYC(0x4452, 0x4454); A = H8(hActiveObjectType);
  CYC(0x4454, 0x4456); alu_add(gb, 0x3e);
  CYC(0x4456, 0x4457); L = A;
  CYC(0x4457, 0x4458); H = D;
  CYC(0x4458, 0x445a); C = 0x2a;
  CYC(0x445a, 0x445b); A = mem_rd(gb, BC);
  CYC(0x445b, 0x445c); alu_or(gb, mem_rd(gb, HL));
  CYC(0x445c, 0x445d); mem_wr(gb, BC, A);
  CYC(0x445d, 0x445e); A = E;
  CYC(0x445e, 0x4461); applyDamageToEnemyOrPart_hook(gb);
}

void collisionEffect12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4461, createClinkInteraction_hook, 0x46e7, 0x4464);
  collisionEffect0c_hook(gb);
}

void collisionEffect0c_hook(GB *gb) {
  CYC(0x4464, 0x4466); E = 0x10;
  CYC(0x4466, 0x4468); label_07_028_hook(gb);
}

void collisionEffect13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4468, createClinkInteraction_hook, 0x46e7, 0x446b);
  collisionEffect0d_hook(gb);
}

void collisionEffect0d_hook(GB *gb) {
  CYC(0x446b, 0x446d); E = 0x14;
  CYC(0x446d, 0x446f); label_07_028_hook(gb);
}

void collisionEffect14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x446f, createClinkInteraction_hook, 0x46e7, 0x4472);
  collisionEffect0e_hook(gb);
}

void collisionEffect0e_hook(GB *gb) {
  CYC(0x4472, 0x4474); E = 0x18;
  label_07_028_hook(gb);
}

void label_07_028_hook(GB *gb) {
  CYC(0x4474, 0x4476); A = H8(hActiveObjectType);
  CYC(0x4476, 0x4478); alu_add(gb, 0x3e);
  CYC(0x4478, 0x4479); L = A;
  CYC(0x4479, 0x447a); H = D;
  CYC(0x447a, 0x447c); C = 0x2a;
  CYC(0x447c, 0x447d); A = mem_rd(gb, BC);
  CYC(0x447d, 0x447e); alu_or(gb, mem_rd(gb, HL));
  CYC(0x447e, 0x447f); mem_wr(gb, BC, A);
  CYC(0x447f, 0x4480); A = E;
  CYC(0x4480, 0x4483); applyDamageToEnemyOrPart_hook(gb);
}

void collisionEffect05_hook(GB *gb) {
  CYC(0x4483, 0x4486); SET_HL(0x101c);
  CYC(0x4486, 0x4488); applyDamageToBothObjects_hook(gb);
}

void collisionEffect06_hook(GB *gb) {
  CYC(0x4488, 0x448b); SET_HL(0x141c);
  CYC(0x448b, 0x448d); applyDamageToBothObjects_hook(gb);
}

void collisionEffect07_hook(GB *gb) {
  CYC(0x448d, 0x4490); SET_HL(0x181c);
  CYC(0x4490, 0x4492); applyDamageToBothObjects_hook(gb);
}

void collisionEffect18_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4492, createClinkInteraction_hook, 0x46e7, 0x4495);
  collisionEffect0f_hook(gb);
}

void collisionEffect0f_hook(GB *gb) {
  CYC(0x4495, 0x4498); SET_HL(0x1010);
  CYC(0x4498, 0x449a); applyDamageToBothObjects_hook(gb);
}

void collisionEffect19_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x449a, createClinkInteraction_hook, 0x46e7, 0x449d);
  collisionEffect10_hook(gb);
}

void collisionEffect10_hook(GB *gb) {
  CYC(0x449d, 0x44a0); SET_HL(0x1414);
  CYC(0x44a0, 0x44a2); applyDamageToBothObjects_hook(gb);
}

void collisionEffect15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44a2, createClinkInteraction_hook, 0x46e7, 0x44a5);
  CYC(0x44a5, 0x44a8); SET_HL(0x1034);
  CYC(0x44a8, 0x44aa); applyDamageToBothObjects_hook(gb);
}

void collisionEffect16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44aa, createClinkInteraction_hook, 0x46e7, 0x44ad);
  CYC(0x44ad, 0x44b0); SET_HL(0x1434);
  CYC(0x44b0, 0x44b2); applyDamageToBothObjects_hook(gb);
}

void collisionEffect17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44b2, createClinkInteraction_hook, 0x46e7, 0x44b5);
  CYC(0x44b5, 0x44b8); SET_HL(0x1834);
  CYC(0x44b8, 0x44ba); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44ba, createClinkInteraction_hook, 0x46e7, 0x44bd);
  collisionEffect11_hook(gb);
}

void collisionEffect11_hook(GB *gb) {
  CYC(0x44bd, 0x44c0); SET_HL(0x1818);
  CYC(0x44c0, 0x44c2); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44c2, createClinkInteraction_hook, 0x46e7, 0x44c5);
  CYC(0x44c5, 0x44c8); SET_HL(0x1c28);
  CYC(0x44c8, 0x44ca); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1d_hook(GB *gb) {
  CYC(0x44ca, 0x44cd); SET_HL(0x0c04);
  CYC(0x44cd, 0x44cf); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1e_hook(GB *gb) {
  CYC(0x44cf, 0x44d2); SET_HL(0x2834);
  CYC(0x44d2, 0x44d4); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1f_hook(GB *gb) {
  CYC(0x44d4, 0x44d7); SET_HL(0x2034);
  CYC(0x44d7, 0x44d9); applyDamageToBothObjects_hook(gb);
}

void collisionEffect20_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x44d9, 0x44da); H = B;
  CYC(0x44da, 0x44dc); L = 0x01;
  CYC(0x44dc, 0x44dd); A = mem_rd(gb, HL);
  CYC(0x44dd, 0x44df); alu_cp(gb, 0x28);
  if (!(F & FC)) {
    CYCT(0x44df, 0x44e1);
  } else {
    CYC(0x44df, 0x44e1);
    CYC(0x44e1, 0x44e3); L = 0x24;
    CYC(0x44e3, 0x44e5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  }
  CALL_C(0x44e5, func_07_47b7_hook, 0x47b7, 0x44e8);
  if (F & FZ) { CYCT(0x44e8, 0x44e9); ret_effect(gb); return; }
  CYC(0x44e8, 0x44e9);
  CYC(0x44e9, 0x44ec); SET_HL(0x2444);
  CYC(0x44ec, 0x44ee); applyDamageToBothObjects_hook(gb);
}

void collisionEffect22_hook(GB *gb) {
  CYC(0x44ee, 0x44f1); SET_HL(0x1c24);
  applyDamageToBothObjects_hook(gb);
}

void collisionEffect26_hook(GB *gb) {
  CYC(0x44fb, 0x44fe); SET_HL(0x1c34);
  CYC(0x44fe, 0x4500); applyDamageToBothObjects_hook(gb);
}

void collisionEffect27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4500, 0x4501); H = B;
  CYC(0x4501, 0x4503); L = 0x24;
  CYC(0x4503, 0x4505); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(0x4505, func_07_47b7_hook, 0x47b7, 0x4508);
  if (F & FZ) { CYCT(0x4508, 0x4509); ret_effect(gb); return; }
  CYC(0x4508, 0x4509);
  CALL_C(0x4509, createFlamePart_hook, 0x46da, 0x450c);
  CYC(0x450c, 0x450f); SET_HL(0x1c2c);
  CYC(0x450f, 0x4511); applyDamageToBothObjects_hook(gb);
}

void collisionEffect28_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4511, 0x4512); H = B;
  CYC(0x4512, 0x4514); L = 0x24;
  CYC(0x4514, 0x4516); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(0x4516, func_07_47b7_hook, 0x47b7, 0x4519);
  if (F & FZ) { CYCT(0x4519, 0x451a); ret_effect(gb); return; }
  CYC(0x4519, 0x451a);
  CYC(0x451a, 0x451d); SET_HL(0x1c38);
  CYC(0x451d, 0x451f); applyDamageToBothObjects_hook(gb);
}

void collisionEffect3a_hook(GB *gb) {
  CYC(0x451f, 0x4521); E = 0xad;
  CYC(0x4521, 0x4522); A = mem_rd(gb, DE);
  CYC(0x4522, 0x4523); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4523, 0x4524); ret_effect(gb); return; }
  CYC(0x4523, 0x4524);
  collisionEffect3d_hook(gb);
}

void collisionEffect3d_hook(GB *gb) {
  CYC(0x4524, 0x4527); A = mem_rd(gb, w1Link_id);
  CYC(0x4527, 0x4528); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4528, 0x4529); ret_effect(gb); return; }
  CYC(0x4528, 0x4529);
  CYC(0x4529, 0x452c); A = mem_rd(gb, wWarpsDisabled);
  CYC(0x452c, 0x452d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x452d, 0x452e); ret_effect(gb); return; }
  CYC(0x452d, 0x452e);
  CYC(0x452e, 0x4530); A = 0x0d;
  CYC(0x4530, 0x4533); mem_wr(gb, wLinkForceState, A);
  CYC(0x4533, 0x4536); SET_HL(0x2c1c);
  CYC(0x4536, 0x4538); applyDamageToBothObjects_hook(gb);
}

void collisionEffect2b_hook(GB *gb) {
  CYC(0x4538, 0x453b); SET_HL(0x1c3c);
  CYC(0x453b, 0x453d); applyDamageToBothObjects_hook(gb);
}

void collisionEffect2c_hook(GB *gb) {
  CYC(0x453d, 0x4540); SET_HL(0x1430);
  CYC(0x4540, 0x4542); applyDamageToBothObjects_hook(gb);
}

void collisionEffect2f_hook(GB *gb) {
  CYC(0x4542, 0x4545); SET_HL(0x3004);
  CYC(0x4545, 0x4547); applyDamageToBothObjects_hook(gb);
}

void collisionEffect30_hook(GB *gb) {
  CYC(0x4547, 0x454a); SET_HL(0x1c44);
  CYC(0x454a, 0x454c); applyDamageToBothObjects_hook(gb);
}

void collisionEffect1c_hook(GB *gb) {
  CYC(0x454c, 0x454f); SET_HL(0x1c1c);
  CYC(0x454f, 0x4551); applyDamageToBothObjects_hook(gb);
}

void collisionEffect23_hook(GB *gb) {
  CYC(0x4584, 0x4586); A = H8(hActiveObjectType);
  CYC(0x4586, 0x4588); alu_add(gb, 0x29);
  CYC(0x4588, 0x4589); L = A;
  CYC(0x4589, 0x458a); H = D;
  CYC(0x458a, 0x458c); mem_wr(gb, HL, 0x00);
  CYC(0x458c, 0x458d); ret_effect(gb);
}

void collisionEffect24_hook(GB *gb) {
  CYC(0x458d, 0x458f); A = H8(hActiveObjectType);
  CYC(0x458f, 0x4591); alu_add(gb, 0x2a);
  CYC(0x4591, 0x4592); E = A;
  CYC(0x4592, 0x4594); A = H8(hFF90);
  CYC(0x4594, 0x4596); alu_or(gb, 0x80);
  CYC(0x4596, 0x4597); mem_wr(gb, DE, A);
  CYC(0x4597, 0x4598); A = E;
  CYC(0x4598, 0x459a); alu_add(gb, 0xec);
  CYC(0x459a, 0x459b); L = A;
  CYC(0x459b, 0x459c); H = D;
  CYC(0x459c, 0x459d); mem_wr(gb, HL, C);
  CYC(0x459d, 0x459e); L = alu_inc8(gb, L);
  CYC(0x459e, 0x459f); mem_wr(gb, HL, B);
  CYC(0x459f, 0x45a1); C = 0x2a;
  CYC(0x45a1, 0x45a3); A = 0x01;
  CYC(0x45a3, 0x45a4); mem_wr(gb, BC, A);
  CYC(0x45a4, 0x45a5); ret_effect(gb);
}

void collisionEffect25_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x45a5, killEnemyOrPart_hook, 0x4649, 0x45a8);
  CYC(0x45a8, 0x45a9); A = L;
  CYC(0x45a9, 0x45ab); alu_add(gb, 0x1b);
  CYC(0x45ab, 0x45ac); L = A;
  CYC(0x45ac, 0x45ae); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x45ae, 0x45b0); C = 0x2a;
  CYC(0x45b0, 0x45b2); A = 0x02;
  CYC(0x45b2, 0x45b3); mem_wr(gb, BC, A);
  CYC(0x45b3, 0x45b4); ret_effect(gb);
}

void collisionEffect2a_hook(GB *gb) {
  CYC(0x4604, 0x4605); H = B;
  CYC(0x4605, 0x4607); L = 0x2d;
  CYC(0x4607, 0x4608); A = D;
  CYC(0x4608, 0x4609); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4609, 0x460a); ret_effect(gb); return; }
  CYC(0x4609, 0x460a);
  CYC(0x460a, 0x460b); mem_wr(gb, HL, A); L--;
  CYC(0x460b, 0x460d); E = 0xe1;
  CYC(0x460d, 0x460e); A = mem_rd(gb, DE);
  CYC(0x460e, 0x460f); mem_wr(gb, HL, A); L--;
  CYC(0x460f, 0x4610); L = alu_dec8(gb, L);
  CYC(0x4610, 0x4612); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(0x4612, 0x4614); E = 0xea;
  CYC(0x4614, 0x4616); A = H8(hFF90);
  CYC(0x4616, 0x4618); alu_or(gb, 0x80);
  CYC(0x4618, 0x4619); mem_wr(gb, DE, A);
  CYC(0x4619, 0x461a); ret_effect(gb);
}

void collisionEffect2d_hook(GB *gb) {
  CYC(0x461a, 0x461b); H = B;
  CYC(0x461b, 0x461d); L = 0x2f;
  CYC(0x461d, 0x461f); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(0x461f, 0x4620); ret_effect(gb);
}

void collisionEffect31_hook(GB *gb) {
  CYC(0x4620, 0x4622); A = 0x34;
  CYC(0x4622, 0x4625); applyDamageToEnemyOrPart_hook(gb);
}

void collisionEffect32_hook(GB *gb) {
  CYC(0x4625, 0x4628); SET_HL(0x3448);
  CYC(0x4628, 0x462a); label_07_033_hook(gb);
}

void collisionEffect33_hook(GB *gb) {
  CYC(0x462a, 0x462d); SET_HL(0x384c);
  label_07_033_hook(gb);
}

void label_07_033_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x462d, applyDamageToBothObjects_hook, 0x44f1, 0x4630);
  CYC(0x4630, 0x4633); createClinkInteraction_hook(gb);
}

void collisionEffect34_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x4633, createFlamePart_hook, 0x46da, 0x4636);
  CYC(0x4636, 0x4637); H = B;
  CYC(0x4637, 0x4639); L = 0x24;
  CYC(0x4639, 0x463b); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x463b, 0x463e); SET_HL(0x1c2c);
  CALL_C(0x463e, applyDamageToBothObjects_hook, 0x44f1, 0x4641);
  CYC(0x4641, 0x4643); killEnemyOrPart_hook(gb);
}

void collisionEffect35_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4643, 0x4646); SET_HL(0x1c1c);
  CALL_C(0x4646, applyDamageToBothObjects_hook, 0x44f1, 0x4649);
  killEnemyOrPart_hook(gb);
}

void killEnemyOrPart_hook(GB *gb) {
  CYC(0x4649, 0x464a); H = D;
  CYC(0x464a, 0x464c); A = H8(hActiveObjectType);
  CYC(0x464c, 0x464e); alu_add(gb, 0x29);
  CYC(0x464e, 0x464f); L = A;
  CYC(0x464f, 0x4651); mem_wr(gb, HL, 0x00);
  CYC(0x4651, 0x4653); alu_add(gb, 0xfb);
  CYC(0x4653, 0x4654); L = A;
  CYC(0x4654, 0x4656); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x4656, 0x4657); ret_effect(gb);
}

void collisionEffect3c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x43f4, 0x43f6); A = H8(hActiveObjectType);
  CYC(0x43f6, 0x43f7); A = alu_inc8(gb, A);
  CYC(0x43f7, 0x43f8); E = A;
  CYC(0x43f8, 0x43f9); A = mem_rd(gb, DE);
  CYC(0x43f9, 0x43fa); C = A;
  CYC(0x43fa, 0x43fd); SET_HL(0x441d);
  for (;;) {
    CYC(0x43fd, 0x43fe); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x43fe, 0x43ff); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x43ff, 0x4401);
      collisionEffect02_hook(gb);
      return;
    }
    CYC(0x43ff, 0x4401);
    CYC(0x4401, 0x4402); alu_cp(gb, C);
    CYC(0x4402, 0x4403); A = mem_rd(gb, HL); SET_HL(HL + 1);
    if (!(F & FZ)) {
      CYCT(0x4403, 0x4405);
      continue;
    }
    CYC(0x4403, 0x4405);
    break;
  }
  CYC(0x4405, 0x4406); C = A;
  CYC(0x4406, 0x4408); alu_and(gb, 0x7f);
  CALL_C(0x4408, cpActiveRing_hook, 0x23b0, 0x440b);
  if (!(F & FZ)) {
    CYCT(0x440b, 0x440d);
    collisionEffect02_hook(gb);
    return;
  }
  CYC(0x440b, 0x440d);
  CYC(0x440d, 0x440f); alu_bit(gb, 7, C);
  CYC(0x440f, 0x4411); A = 0x40;
  if (F & FZ) {
    CYCT(0x4411, 0x4414);
    applyDamageToEnemyOrPart_hook(gb);
    return;
  }
  CYC(0x4411, 0x4414);
  CALL_C(0x4414, collisionEffect02_hook, 0x442a, 0x4417);
  CYC(0x4417, 0x4418); H = B;
  CYC(0x4418, 0x441a); L = 0x25;
  CYC(0x441a, 0x441c); mem_wr(gb, HL, alu_sra(gb, mem_rd(gb, HL)));
  CYC(0x441c, 0x441d); ret_effect(gb);
}

void collisionEffect0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x4448, func_07_47b7_hook, 0x47b7, 0x444b);
  if (F & FZ) { CYCT(0x444b, 0x444c); ret_effect(gb); return; }
  CYC(0x444b, 0x444c);
  CYC(0x444c, 0x444e); E = 0x0c;
  CYC(0x444e, 0x4450); label_07_027_hook(gb);
}

void collisionEffect21_hook(GB *gb) {
  CYC(0x4450, 0x4452); E = 0x30;
  label_07_027_hook(gb);
}

void collisionEffect2e_hook(GB *gb) {
  CYC(0x4551, 0x4552); H = D;
  CYC(0x4552, 0x4554); A = H8(hActiveObjectType);
  CYC(0x4554, 0x4556); alu_add(gb, 0x29);
  CYC(0x4556, 0x4557); L = A;
  CYC(0x4557, 0x4558); A = mem_rd(gb, HL);
  CYC(0x4558, 0x4559); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4559, 0x455b);
    collisionEffect1c_hook(gb);
    return;
  }
  CYC(0x4559, 0x455b);
  CYC(0x455b, 0x455c); A = L;
  CYC(0x455c, 0x455e); alu_add(gb, 0x05);
  CYC(0x455e, 0x455f); L = A;
  CYC(0x455f, 0x4560); alu_xor(gb, A);
  CYC(0x4560, 0x4561); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4561, 0x4562); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4562, 0x4564); A = H8(hFF8A);
  CYC(0x4564, 0x4566); alu_xor(gb, 0x10);
  CYC(0x4566, 0x4567); mem_wr(gb, HL, A);
  CYC(0x4567, 0x4569); L &= 0xf7;
  CYC(0x4569, 0x456b); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x456b, 0x456c); A = L;
  CYC(0x456c, 0x456e); alu_add(gb, 0xe0);
  CYC(0x456e, 0x456f); L = A;
  CYC(0x456f, 0x4571); mem_wr(gb, HL, 0x03);
  CYC(0x4571, 0x4572); L = alu_inc8(gb, L);
  CYC(0x4572, 0x4574); mem_wr(gb, HL, 0x00);
  CYC(0x4574, 0x4575); H = B;
  CYC(0x4575, 0x4577); L = 0x2a;
  CYC(0x4577, 0x4579); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(0x4579, 0x457b); L = 0x24;
  CYC(0x457b, 0x457d); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x457d, 0x457f); L = 0x18;
  CYC(0x457f, 0x4581); A = H8(hActiveObjectType);
  CYC(0x4581, 0x4582); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4582, 0x4583); mem_wr(gb, HL, D);
  CYC(0x4583, 0x4584); ret_effect(gb);
}

void collisionEffect29_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x45b4, 0x45b5); H = B;
  CYC(0x45b5, 0x45b7); L = 0x24;
  CYC(0x45b7, 0x45b9); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(0x45b9, func_07_47b7_hook, 0x47b7, 0x45bc);
  if (F & FZ) { CYCT(0x45bc, 0x45bd); ret_effect(gb); return; }
  CYC(0x45bc, 0x45bd);
  CYC(0x45bd, 0x45be); H = D;
  CYC(0x45be, 0x45c0); L = 0xaa;
  CYC(0x45c0, 0x45c2); mem_wr(gb, HL, 0x9e);
  CYC(0x45c2, 0x45c4); L = 0xae;
  CYC(0x45c4, 0x45c6); mem_wr(gb, HL, 0x00);
  CYC(0x45c6, 0x45c8); L = 0xa4;
  CYC(0x45c8, 0x45ca); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x45ca, 0x45cc); L = 0x84;
  CYC(0x45cc, 0x45ce); mem_wr(gb, HL, 0x05);
  CYC(0x45ce, 0x45d0); L = 0x9a;
  CYC(0x45d0, 0x45d1); A = mem_rd(gb, HL);
  CYC(0x45d1, 0x45d3); alu_and(gb, 0xc0);
  CYC(0x45d3, 0x45d5); alu_or(gb, 0x02);
  CYC(0x45d5, 0x45d6); mem_wr(gb, HL, A);
  CYC(0x45d6, 0x45d8); L = 0x87;
  CYC(0x45d8, 0x45da); mem_wr(gb, HL, 0x1e);
  CYC(0x45da, 0x45dc); L = 0x90;
  CYC(0x45dc, 0x45de); mem_wr(gb, HL, 0x05);
  CYC(0x45de, 0x45e0); L = 0x94;
  CYC(0x45e0, 0x45e2); mem_wr(gb, HL, 0x00);
  CYC(0x45e2, 0x45e3); L = alu_inc8(gb, L);
  CYC(0x45e3, 0x45e5); mem_wr(gb, HL, 0xfa);
  CYC(0x45e5, 0x45e7); L = 0x8b;
  CYC(0x45e7, 0x45e9); C = 0x0b;
  CYC(0x45e9, 0x45ea); A = mem_rd(gb, BC);
  CYC(0x45ea, 0x45eb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x45eb, 0x45ec); L = alu_inc8(gb, L);
  CYC(0x45ec, 0x45ee); C = 0x0d;
  CYC(0x45ee, 0x45ef); A = mem_rd(gb, BC);
  CYC(0x45ef, 0x45f0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x45f0, 0x45f1); L = alu_inc8(gb, L);
  CYC(0x45f1, 0x45f2); A = mem_rd(gb, HL);
  CYC(0x45f2, 0x45f3); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x45f3, 0x45f5);
  } else {
    CYC(0x45f3, 0x45f5);
    CYC(0x45f5, 0x45f7); mem_wr(gb, HL, 0xff);
  }
  CALL_C(0x45f7, getRandomNumber_hook, 0x043e, 0x45fa);
  CYC(0x45fa, 0x45fc); alu_and(gb, 0x18);
  CYC(0x45fc, 0x45fe); E = 0x89;
  CYC(0x45fe, 0x45ff); mem_wr(gb, DE, A);
  CYC(0x45ff, 0x4601); A = 0x1c;
  CYC(0x4601, 0x4604); applyDamageToLink_hook(gb);
}

void collisionEffect36_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4657, 0x4658); H = D;
  CYC(0x4658, 0x465a); A = H8(hActiveObjectType);
  CYC(0x465a, 0x465c); alu_add(gb, 0x2a);
  CYC(0x465c, 0x465d); L = A;
  CYC(0x465d, 0x465f); mem_wr(gb, HL, 0xa0);
  CYC(0x465f, 0x4661); alu_add(gb, 0xfa);
  CYC(0x4661, 0x4662); L = A;
  CYC(0x4662, 0x4664); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x4664, 0x4666); A = 0x1e;
  CALL_C(0x4666, cpActiveRing_hook, 0x23b0, 0x4669);
  CYC(0x4669, 0x466b); A = 0xf8;
  if (!(F & FZ)) {
    CYCT(0x466b, 0x466d);
  } else {
    CYC(0x466b, 0x466d);
    CYC(0x466d, 0x466e); alu_xor(gb, A);
  }
  CYC(0x466e, 0x4671); SET_HL(w1Link_damageToApply);
  CYC(0x4671, 0x4672); mem_wr(gb, HL, A);
  CYC(0x4672, 0x4674); L = 0x2c;
  CYC(0x4674, 0x4676); A = H8(hFF8A);
  CYC(0x4676, 0x4677); mem_wr(gb, HL, A);
  CYC(0x4677, 0x4679); L = 0x2d;
  CYC(0x4679, 0x467b); mem_wr(gb, HL, 0x08);
  CYC(0x467b, 0x467d); L = 0x2b;
  CYC(0x467d, 0x467f); mem_wr(gb, HL, 0x0c);
  CYC(0x467f, 0x4682); A = mem_rd(gb, wIsLinkBeingShocked);
  CYC(0x4682, 0x4683); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4683, 0x4685);
  } else {
    CYC(0x4683, 0x4685);
    CYC(0x4685, 0x4686); A = alu_inc8(gb, A);
    CYC(0x4686, 0x4689); mem_wr(gb, wIsLinkBeingShocked, A);
  }
  CYC(0x4689, 0x468a); H = B;
  CYC(0x468a, 0x468c); L = 0x24;
  CYC(0x468c, 0x468e); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x468e, 0x4690); A = 0x1c;
  CYC(0x4690, 0x4693); applyDamageToLink_hook(gb);
}

void collisionEffect37_hook(GB *gb) {
  CYC(0x4693, 0x4695); A = H8(hActiveObjectType);
  CYC(0x4695, 0x4697); alu_add(gb, 0x2b);
  CYC(0x4697, 0x4698); E = A;
  CYC(0x4698, 0x4699); A = mem_rd(gb, DE);
  CYC(0x4699, 0x469a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x469a, 0x469b); ret_effect(gb); return; }
  CYC(0x469a, 0x469b);
  CYC(0x469b, 0x469e); A = mem_rd(gb, wWarpsDisabled);
  CYC(0x469e, 0x469f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x469f, 0x46a0); ret_effect(gb); return; }
  CYC(0x469f, 0x46a0);
  CYC(0x46a0, 0x46a3); A = mem_rd(gb, w1Link_state);
  CYC(0x46a3, 0x46a5); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x46a5, 0x46a6); ret_effect(gb); return; }
  CYC(0x46a5, 0x46a6);
  CYC(0x46a6, 0x46a7); A = E;
  CYC(0x46a7, 0x46a9); alu_add(gb, 0xf9);
  CYC(0x46a9, 0x46aa); E = A;
  CYC(0x46aa, 0x46ab); alu_xor(gb, A);
  CYC(0x46ab, 0x46ac); mem_wr(gb, DE, A);
  CYC(0x46ac, 0x46ae); A = 0x0c;
  CYC(0x46ae, 0x46b1); mem_wr(gb, wLinkForceState, A);
  CYC(0x46b1, 0x46b3); A = 0x1c;
  CYC(0x46b3, 0x46b6); applyDamageToEnemyOrPart_hook(gb);
}

void collisionEffect38_hook(GB *gb) {
  CYC(0x46b6, 0x46b7); H = D;
  CYC(0x46b7, 0x46b9); A = H8(hActiveObjectType);
  CYC(0x46b9, 0x46bb); alu_add(gb, 0x24);
  CYC(0x46bb, 0x46bc); L = A;
  CYC(0x46bc, 0x46be); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x46be, 0x46c0); alu_add(gb, 0xe2);
  CYC(0x46c0, 0x46c1); L = A;
  CYC(0x46c1, 0x46c3); mem_wr(gb, HL, 0x60);
  CYC(0x46c3, 0x46c5); alu_add(gb, 0x09);
  CYC(0x46c5, 0x46c6); L = A;
  CYC(0x46c6, 0x46c8); mem_wr(gb, HL, 0x00);
  CYC(0x46c8, 0x46ca); A = 0x1c;
  CYC(0x46ca, 0x46cd); applyDamageToEnemyOrPart_hook(gb);
}

void collisionEffect39_hook(GB *gb) {
  CYC(0x46cd, 0x46ce); ret_effect(gb);
}

void collisionEffect3b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x46ce, 0x46d0); A = 0x02;
  CALL_C(0x46d0, setLinkIDOverride_hook, 0x2acf, 0x46d3);
  CYC(0x46d3, 0x46d5); A = 0x1c;
  CYC(0x46d5, 0x46d8); applyDamageToEnemyOrPart_hook(gb);
}

void collisionEffect3e_hook(GB *gb) {
  CYC(0x46d8, 0x46d9); ret_effect(gb);
}

void collisionEffect3f_hook(GB *gb) {
  CYC(0x46d9, 0x46da); ret_effect(gb);
}

void applyDamageToBothObjects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x44f1, 0x44f2); A = H;
  CYC(0x44f2, 0x44f3); push_effect(gb, HL);
  CALL_C(0x44f3, applyDamageToLink_hook, 0x47e0, 0x44f6);
  CYC(0x44f6, 0x44f7); SET_HL(pop_effect(gb));
  CYC(0x44f7, 0x44f8); A = L;
  CYC(0x44f8, 0x44fb); applyDamageToEnemyOrPart_hook(gb);
}

void createFlamePart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x46da, getFreePartSlot_hook, 0x3e8e, 0x46dd);
  if (!(F & FZ)) {
    CYCT(0x46dd, 0x46de); ret_effect(gb); return;
  }
  CYC(0x46dd, 0x46de);
  CYC(0x46de, 0x46e0); mem_wr(gb, HL, 0x12);
  CYC(0x46e0, 0x46e2); L = 0xd6;
  CYC(0x46e2, 0x46e4); A = H8(hActiveObjectType);
  CYC(0x46e4, 0x46e5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x46e5, 0x46e6); mem_wr(gb, HL, D);
  CYC(0x46e6, 0x46e7); ret_effect(gb);
}

void createClinkInteraction_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x46e7, getFreeInteractionSlot_hook, 0x3aef, 0x46ea);
  if (!(F & FZ)) {
    CYCT(0x46ea, 0x46ec);
  } else {
    CYC(0x46ea, 0x46ec);
    CYC(0x46ec, 0x46ee); mem_wr(gb, HL, 0x07);
    CYC(0x46ee, 0x46f0); A = H8(hFF8F);
    CYC(0x46f0, 0x46f1); L = A;
    CYC(0x46f1, 0x46f3); A = H8(hFF8D);
    CYC(0x46f3, 0x46f4); alu_sub(gb, L);
    CYC(0x46f4, 0x46f6); A = alu_sra(gb, A);
    CYC(0x46f6, 0x46f7); alu_add(gb, L);
    CYC(0x46f7, 0x46f9); L = 0x4b;
    CYC(0x46f9, 0x46fa); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x46fa, 0x46fc); A = H8(hFF8E);
    CYC(0x46fc, 0x46fd); L = A;
    CYC(0x46fd, 0x46ff); A = H8(hFF8C);
    CYC(0x46ff, 0x4700); alu_sub(gb, L);
    CYC(0x4700, 0x4702); A = alu_sra(gb, A);
    CYC(0x4702, 0x4703); alu_add(gb, L);
    CYC(0x4703, 0x4705); L = 0x4d;
    CYC(0x4705, 0x4706); mem_wr(gb, HL, A);
  }
  CYC(0x4706, 0x4707); ret_effect(gb);
}

void applyDamageToEnemyOrPart_hook(GB *gb) {
  CYC(0x4707, 0x470a); SET_HL(0x475f);
  CYC(0x470a, 0x470b); collision_add_a_to_hl_from_rst(gb, 0x470b);
  CYC(0x470b, 0x470d); A = H8(hActiveObjectType);
  CYC(0x470d, 0x470f); alu_add(gb, 0x29);
  CYC(0x470f, 0x4710); E = A;
  CYC(0x4710, 0x4712); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4712, 0x4714);
  } else {
    CYC(0x4712, 0x4714);
    CYC(0x4714, 0x4716); C = 0x28;
    CYC(0x4716, 0x4717); A = mem_rd(gb, BC);
    CYC(0x4717, 0x4718); C = A;
    CYC(0x4718, 0x4719); A = mem_rd(gb, DE);
    CYC(0x4719, 0x471a); alu_add(gb, C);
    if (F & FC) {
      CYCT(0x471a, 0x471c);
    } else {
      CYC(0x471a, 0x471c);
      CYC(0x471c, 0x471d); alu_xor(gb, A);
    }
    CYC(0x471d, 0x471e); mem_wr(gb, DE, A);
    if (!(F & FZ)) {
      CYCT(0x471e, 0x4720);
    } else {
      CYC(0x471e, 0x4720);
      CYC(0x4720, 0x4721); C = E;
      CYC(0x4721, 0x4722); A = E;
      CYC(0x4722, 0x4724); alu_add(gb, 0xfb);
      CYC(0x4724, 0x4725); E = A;
      CYC(0x4725, 0x4726); A = mem_rd(gb, DE);
      CYC(0x4726, 0x4728); A &= 0x7f;
      CYC(0x4728, 0x4729); mem_wr(gb, DE, A);
      CYC(0x4729, 0x472a); E = C;
    }
  }
  CYC(0x472a, 0x472b); E = alu_inc8(gb, E);
  CYC(0x472b, 0x472c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x472c, 0x472d); C = A;
  CYC(0x472d, 0x472f); alu_bit(gb, 6, C);
  if (F & FZ) {
    CYCT(0x472f, 0x4731);
  } else {
    CYC(0x472f, 0x4731);
    CYC(0x4731, 0x4733); A = H8(hFF90);
    CYC(0x4733, 0x4735); alu_or(gb, 0x80);
    CYC(0x4735, 0x4736); mem_wr(gb, DE, A);
  }
  CYC(0x4736, 0x4737); E = alu_inc8(gb, E);
  CYC(0x4737, 0x4738); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4738, 0x473a); alu_bit(gb, 5, C);
  if (F & FZ) {
    CYCT(0x473a, 0x473c);
  } else {
    CYC(0x473a, 0x473c);
    CYC(0x473c, 0x473d); mem_wr(gb, DE, A);
  }
  CYC(0x473d, 0x473e); E = alu_inc8(gb, E);
  CYC(0x473e, 0x473f); E = alu_inc8(gb, E);
  CYC(0x473f, 0x4741); alu_bit(gb, 4, C);
  CYC(0x4741, 0x4742); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (F & FZ) {
    CYCT(0x4742, 0x4744);
  } else {
    CYC(0x4742, 0x4744);
    CYC(0x4744, 0x4745); mem_wr(gb, DE, A);
    CYC(0x4745, 0x4747); A = H8(hFF8A);
    CYC(0x4747, 0x4749); alu_xor(gb, 0x10);
    CYC(0x4749, 0x474a); E = alu_dec8(gb, E);
    CYC(0x474a, 0x474b); mem_wr(gb, DE, A);
    CYC(0x474b, 0x474c); E = alu_inc8(gb, E);
  }
  CYC(0x474c, 0x474d); E = alu_inc8(gb, E);
  CYC(0x474d, 0x474e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x474e, 0x4750); alu_bit(gb, 3, C);
  if (F & FZ) {
    CYCT(0x4750, 0x4752);
  } else {
    CYC(0x4750, 0x4752);
    CYC(0x4752, 0x4753); mem_wr(gb, DE, A);
  }
  CYC(0x4753, 0x4754); A = C;
  CYC(0x4754, 0x4756); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(0x4756, 0x4757); ret_effect(gb); return;
  }
  CYC(0x4756, 0x4757);
  CYC(0x4757, 0x475a); SET_HL(0x47af);
  CYC(0x475a, 0x475b); collision_add_a_to_hl_from_rst(gb, 0x475b);
  CYC(0x475b, 0x475c); A = mem_rd(gb, HL);
  CYC(0x475c, 0x475f); playSound_b00_hook(gb);
}

void func_07_47b7_hook(GB *gb) {
  CYC(0x47b7, 0x47b9); C = 0x01;
  CYC(0x47b9, 0x47ba); A = mem_rd(gb, BC);
  CYC(0x47ba, 0x47bc); alu_cp(gb, 0x24);
  if (!(F & FZ)) {
    CYCT(0x47bc, 0x47bd); ret_effect(gb); return;
  }
  CYC(0x47bc, 0x47bd);
  CYC(0x47bd, 0x47bf); A = H8(hActiveObjectType);
  CYC(0x47bf, 0x47c1); alu_add(gb, 0x3f);
  CYC(0x47c1, 0x47c2); E = A;
  CYC(0x47c2, 0x47c3); A = mem_rd(gb, DE);
  CYC(0x47c3, 0x47c4); alu_cpl(gb);
  CYC(0x47c4, 0x47c6); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(0x47c6, 0x47c7); ret_effect(gb); return;
  }
  CYC(0x47c6, 0x47c7);
  CYC(0x47c7, 0x47c8); H = B;
  CYC(0x47c8, 0x47ca); L = 0x2a;
  CYC(0x47ca, 0x47cc); mem_wr(gb, HL, 0x40);
  CYC(0x47cc, 0x47ce); L = 0x24;
  CYC(0x47ce, 0x47d0); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x47d0, 0x47d2); A = H8(hActiveObjectType);
  CYC(0x47d2, 0x47d4); alu_add(gb, 0x2a);
  CYC(0x47d4, 0x47d5); E = A;
  CYC(0x47d5, 0x47d7); A = 0x9a;
  CYC(0x47d7, 0x47d8); mem_wr(gb, DE, A);
  CYC(0x47d8, 0x47d9); A = E;
  CYC(0x47d9, 0x47db); alu_add(gb, 0x04);
  CYC(0x47db, 0x47dc); E = A;
  CYC(0x47dc, 0x47dd); alu_xor(gb, A);
  CYC(0x47dd, 0x47de); mem_wr(gb, DE, A);
  CYC(0x47de, 0x47df); ret_effect(gb);
}

void applyDamageToLink_paramE_hook(GB *gb) {
  CYC(0x47df, 0x47e0); A = E;
  applyDamageToLink_hook(gb);
}

void applyDamageToLink_hook(GB *gb) {
  CYC(0x47e0, 0x47e1); push_effect(gb, AF);
  CYC(0x47e1, 0x47e3); A = H8(hActiveObjectType);
  CYC(0x47e3, 0x47e5); alu_add(gb, 0x3e);
  CYC(0x47e5, 0x47e6); E = A;
  CYC(0x47e6, 0x47e7); A = mem_rd(gb, DE);
  CYC(0x47e7, 0x47ea); W8(wTmpcec0) = A;
  CYC(0x47ea, 0x47eb); SET_AF(pop_effect(gb));
  CYC(0x47eb, 0x47ee); SET_HL(0x482e);
  CYC(0x47ee, 0x47ef); collision_add_a_to_hl_from_rst(gb, 0x47ef);
  CYC(0x47ef, 0x47f1); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x47f1, 0x47f3);
  } else {
    CYC(0x47f1, 0x47f3);
    CYC(0x47f3, 0x47f5); A = H8(hActiveObjectType);
    CYC(0x47f5, 0x47f7); alu_add(gb, 0x28);
    CYC(0x47f7, 0x47f8); E = A;
    CYC(0x47f8, 0x47f9); A = mem_rd(gb, DE);
    CYC(0x47f9, 0x47fb); C = 0x25;
    CYC(0x47fb, 0x47fc); mem_wr(gb, BC, A);
  }
  CYC(0x47fc, 0x47fd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x47fd, 0x47fe); E = A;
  CYC(0x47fe, 0x4800); C = 0x2a;
  CYC(0x4800, 0x4801); A = mem_rd(gb, BC);
  CYC(0x4801, 0x4802); C = A;
  CYC(0x4802, 0x4805); A = W8(wTmpcec0);
  CYC(0x4805, 0x4806); alu_or(gb, C);
  CYC(0x4806, 0x4808); C = 0x2a;
  CYC(0x4808, 0x4809); mem_wr(gb, BC, A);
  CYC(0x4809, 0x480a); C = alu_inc8(gb, C);
  CYC(0x480a, 0x480b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x480b, 0x480d); alu_bit(gb, 5, E);
  if (F & FZ) {
    CYCT(0x480d, 0x480f);
  } else {
    CYC(0x480d, 0x480f);
    CYC(0x480f, 0x4810); mem_wr(gb, BC, A);
  }
  CYC(0x4810, 0x4811); C = alu_inc8(gb, C);
  CYC(0x4811, 0x4813); A = H8(hFF8A);
  CYC(0x4813, 0x4814); mem_wr(gb, BC, A);
  CYC(0x4814, 0x4815); C = alu_inc8(gb, C);
  CYC(0x4815, 0x4816); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4816, 0x4818); alu_bit(gb, 4, E);
  if (F & FZ) {
    CYCT(0x4818, 0x481a);
  } else {
    CYC(0x4818, 0x481a);
    CYC(0x481a, 0x481b); mem_wr(gb, BC, A);
  }
  CYC(0x481b, 0x481c); C = alu_inc8(gb, C);
  CYC(0x481c, 0x481d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x481d, 0x481f); alu_bit(gb, 4, E);
  if (F & FZ) {
    CYCT(0x481f, 0x4821);
  } else {
    CYC(0x481f, 0x4821);
    CYC(0x4821, 0x4822); mem_wr(gb, BC, A);
  }
  CYC(0x4822, 0x4823); A = E;
  CYC(0x4823, 0x4825); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(0x4825, 0x4826); ret_effect(gb); return;
  }
  CYC(0x4825, 0x4826);
  CYC(0x4826, 0x4829); SET_HL(0x486a);
  CYC(0x4829, 0x482a); collision_add_a_to_hl_from_rst(gb, 0x482a);
  CYC(0x482a, 0x482b); A = mem_rd(gb, HL);
  CYC(0x482b, 0x482e); playSound_b00_hook(gb);
}
