#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define shieldPositionOffsets_bank07 SYM(checkEnemyAndPartCollisions__shieldPositionOffsets)
#define enemyActiveCollisions_bank07 SYM(enemyActiveCollisions)
#define partActiveCollisions_bank07 SYM(partActiveCollisions)
#define objectCollisionTable_bank07 SYM(objectCollisionTable)

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
  BASE(enemyCheckCollisions);
  CYC(b_+224, b_+225); B = A;
  CYC(b_+225, b_+227); alu_and(gb, 0xf8);
  CYC(b_+227, b_+228); alu_rlca(gb);
  CYC(b_+228, b_+230); A = alu_swap(gb, A);
  CYC(b_+230, b_+231); C = A;
  CYC(b_+231, b_+232); A = B;
  CYC(b_+232, b_+234); alu_and(gb, 0x07);
  CYC(b_+234, b_+236); B = 0x00;
  CYC(b_+236, b_+237); alu_add_hl(gb, BC);
  CYC(b_+237, b_+238); C = mem_rd(gb, HL);
  CYC(b_+238, b_+241); SET_HL(0x00f8);
  CYC(b_+241, b_+242); alu_add(gb, L);
  CYC(b_+242, b_+243); L = A;
  CYC(b_+243, b_+244); A = mem_rd(gb, HL);
  CYC(b_+244, b_+245); alu_and(gb, C);
  CYC(b_+245, b_+246); ret_effect(gb);
}

static void collision_check_object(GB *gb, uint16_t sp0_) {
  BASE(enemyCheckCollisions);
  CYC(b_+5, b_+6); alu_add(gb, A);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+9); B = 0x00;
  CYC(b_+9, b_+10); alu_add_hl(gb, BC);
  CYC(b_+10, b_+11); alu_add_hl(gb, BC);
  CYC(b_+11, b_+12); A = L;
  CYC(b_+12, b_+14); H8(hFF92) = A;
  CYC(b_+14, b_+15); A = H;
  CYC(b_+15, b_+17); H8(hFF93) = A;
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+19); L = E;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+22); H8(hFF8F) = A;
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+26); H8(hFF8E) = A;
  CYC(b_+26, b_+27); L = alu_inc8(gb, L);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); H8(hFF91) = A;
  CYC(b_+30, b_+31); A = L;
  CYC(b_+31, b_+33); alu_add(gb, 0x1c);
  CYC(b_+33, b_+34); L = A;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+36, b_+38);
    goto done_checking_items;
  }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); H = 0xd6;

check_item:
  CYC(b_+40, b_+42); L = 0x24;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+45, b_+47);
    goto next_item;
  }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); alu_and(gb, 0x7f);
  CYC(b_+49, b_+51); H8(hFF90) = A;
  CYC(b_+51, b_+52); B = A;
  CYC(b_+52, b_+53); E = H;
  CYC(b_+53, b_+55); A = H8(hFF92);
  CYC(b_+55, b_+56); L = A;
  CYC(b_+56, b_+58); A = H8(hFF93);
  CYC(b_+58, b_+59); H = A;
  CYC(b_+59, b_+60); A = B;
  CYC(b_+60, b_+63); push_effect(gb, b_+63); collision_check_flag(gb);
  CYC(b_+63, b_+64); H = E;
  if (F & FZ) {
    CYCT(b_+64, b_+66);
    goto next_item;
  }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+69); SET_BC(0x0e07);
  CYC(b_+69, b_+71); A = H8(hFF90);
  CYC(b_+71, b_+73); alu_cp(gb, GV(0x18, 0x17));
  if (!(F & FZ)) {
    CYCT(b_+73, b_+75);
  } else {
    CYC(b_+73, b_+75);
    CYC(b_+75, b_+77); L = 0x26;
    CYC(b_+77, b_+78); A = mem_rd(gb, HL);
    CYC(b_+78, b_+79); C = A;
    CYC(b_+79, b_+80); alu_add(gb, A);
    CYC(b_+80, b_+81); B = A;
  }
  CYC(b_+81, b_+83); L = 0x0f;
  CYC(b_+83, b_+85); A = H8(hFF91);
  CYC(b_+85, b_+86); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+86, b_+87); alu_add(gb, C);
  CYC(b_+87, b_+88); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(b_+88, b_+90);
    goto next_item;
  }
  CYC(b_+88, b_+90);
  CYC(b_+90, b_+92); L = 0x0b;
  CYC(b_+92, b_+93); B = mem_rd(gb, HL);
  CYC(b_+93, b_+95); L = 0x0d;
  CYC(b_+95, b_+96); C = mem_rd(gb, HL);
  CYC(b_+96, b_+98); L = 0x26;
  CYC(b_+98, b_+100); A = H8(hActiveObjectType);
  CYC(b_+100, b_+102); alu_add(gb, 0x26);
  CYC(b_+102, b_+103); E = A;
  CALL_C(b_+103, checkObjectsCollidedFromVariables_hook, SYM(checkObjectsCollidedFromVariables), b_+106);
  if (F & FC) {
    CYCT(b_+106, b_+109);
    goto handle_collision;
  }
  CYC(b_+106, b_+109);

next_item:
  CYC(b_+109, b_+110); H = alu_inc8(gb, H);
  CYC(b_+110, b_+111); A = H;
  CYC(b_+111, b_+113); alu_cp(gb, 0xde);
  if (F & FC) {
    CYCT(b_+113, b_+115);
    goto check_item;
  }
  CYC(b_+113, b_+115);

done_checking_items:
  CALL_C(b_+115, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+118);
  if (!(F & FC)) { CYCT(b_+118, b_+119); ret_effect(gb); return; }
  CYC(b_+118, b_+119);
  CYC(b_+119, b_+121); L = 0x0f;
  CYC(b_+121, b_+123); A = H8(hFF91);
  CYC(b_+123, b_+124); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+124, b_+126); alu_add(gb, 0x07);
  CYC(b_+126, b_+128); alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(b_+128, b_+129); ret_effect(gb); return; }
  CYC(b_+128, b_+129);
  CYC(b_+129, b_+132); A = W8(wUsingShield);
  CYC(b_+132, b_+133); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+133, b_+135);
    goto check_hit_link;
  }
  CYC(b_+133, b_+135);
  CYC(b_+135, b_+137); H8(hFF90) = A;
  CYC(b_+137, b_+139); A = H8(hFF92);
  CYC(b_+139, b_+140); L = A;
  CYC(b_+140, b_+142); A = H8(hFF93);
  CYC(b_+142, b_+143); H = A;
  CYC(b_+143, b_+145); A = H8(hFF90);
  CYC(b_+145, b_+148); push_effect(gb, b_+148); collision_check_flag(gb);
  if (F & FZ) {
    CYCT(b_+148, b_+150);
    goto check_hit_link;
  }
  CYC(b_+148, b_+150);
  CYC(b_+150, b_+153); SET_HL(wShieldY);
  CYC(b_+153, b_+154); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+154, b_+155); B = A;
  CYC(b_+155, b_+156); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+156, b_+157); C = A;
  CYC(b_+157, b_+159); A = H8(hActiveObjectType);
  CYC(b_+159, b_+161); alu_add(gb, 0x26);
  CYC(b_+161, b_+162); E = A;
  CALL_C(b_+162, checkObjectsCollidedFromVariables_hook, SYM(checkObjectsCollidedFromVariables), b_+165);
  CYC(b_+165, b_+168); SET_HL(w1Link);
  if (F & FC) {
    CYCT(b_+168, b_+171);
    goto handle_collision;
  }
  CYC(b_+168, b_+171);

check_hit_link:
  CYC(b_+171, b_+173); A = H8(hActiveObjectType);
  CYC(b_+173, b_+175); alu_add(gb, 0x2e);
  CYC(b_+175, b_+176); E = A;
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+178); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+178, b_+179); ret_effect(gb); return; }
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+182); A = W8(wLinkObjectIndex);
  CYC(b_+182, b_+183); H = A;
  CYC(b_+183, b_+184); E = A;
  CYC(b_+184, b_+186); L = 0x24;
  CYC(b_+186, b_+187); A = mem_rd(gb, HL);
  CYC(b_+187, b_+189); alu_and(gb, 0x7f);
  CYC(b_+189, b_+191); H8(hFF90) = A;
  CYC(b_+191, b_+193); A = H8(hFF92);
  CYC(b_+193, b_+194); L = A;
  CYC(b_+194, b_+196); A = H8(hFF93);
  CYC(b_+196, b_+197); H = A;
  CYC(b_+197, b_+199); A = H8(hFF90);
  CYC(b_+199, b_+202); push_effect(gb, b_+202); collision_check_flag(gb);
  if (F & FZ) { CYCT(b_+202, b_+203); ret_effect(gb); return; }
  CYC(b_+202, b_+203);
  CYC(b_+203, b_+204); H = E;
  CYC(b_+204, b_+206); L = 0x0b;
  CYC(b_+206, b_+207); B = mem_rd(gb, HL);
  CYC(b_+207, b_+209); L = 0x0d;
  CYC(b_+209, b_+210); C = mem_rd(gb, HL);
  CYC(b_+210, b_+212); L = 0x26;
  CYC(b_+212, b_+214); A = H8(hActiveObjectType);
  CYC(b_+214, b_+216); alu_add(gb, 0x26);
  CYC(b_+216, b_+217); E = A;
  CALL_C(b_+217, checkObjectsCollidedFromVariables_hook, SYM(checkObjectsCollidedFromVariables), b_+220);
  if (F & FC) {
    CYCT(b_+220, b_+223);
    goto handle_collision;
  }
  CYC(b_+220, b_+223);
  CYC(b_+223, b_+224); ret_effect(gb);
  return;

handle_collision:
  CYC(b_+246, b_+247); A = L;
  CYC(b_+247, b_+249); alu_and(gb, 0xc0);
  CYC(b_+249, b_+250); L = A;
  CYC(b_+250, b_+251); push_effect(gb, HL);
  CYC(b_+251, b_+253); A = 0xd6;
  CYC(b_+253, b_+254); alu_cp(gb, H);
  if (!(F & FZ)) {
    CYCT(b_+254, b_+256);
    CYC(b_+265, b_+267); A = H8(hFF8D);
    CYC(b_+267, b_+268); B = A;
    CYC(b_+268, b_+270); A = H8(hFF8C);
  } else {
    CYC(b_+254, b_+256);
    CYC(b_+256, b_+259); A = W8(w1Link_yh);
    CYC(b_+259, b_+260); B = A;
    CYC(b_+260, b_+263); A = W8(w1Link_xh);
    CYC(b_+263, b_+265);
  }
  CYC(b_+270, b_+271); C = A;
  CALL_C(b_+271, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+274);
  CYC(b_+274, b_+276); H8(hFF8A) = A;
  CYC(b_+276, b_+278); A = H8(hActiveObjectType);
  CYC(b_+278, b_+280); alu_add(gb, 0x25);
  CYC(b_+280, b_+281); E = A;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+283); alu_add(gb, A);
  CALL_C(b_+283, multiplyABy16_hook, SYM(multiplyABy16), b_+286);
  CYC(b_+286, b_+289); SET_HL(objectCollisionTable_bank07);
  CYC(b_+289, b_+290); alu_add_hl(gb, BC);
  CYC(b_+290, b_+291); SET_BC(pop_effect(gb));
  CYC(b_+291, b_+293); A = H8(hFF90);
  CYC(b_+293, b_+294); collision_add_a_to_hl_from_rst(gb, b_+294);
  CYC(b_+294, b_+295); A = mem_rd(gb, HL);
  CYC(b_+295, b_+296); push_effect(gb, b_+296);
  hook_continue(gb, collision_jump_table(gb), sp0_);
}

void checkEnemyAndPartCollisions_hook(GB *gb) {
  BASE(checkEnemyAndPartCollisions);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(w1Link_direction);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+8); SET_HL(shieldPositionOffsets_bank07);
  CYC(b_+8, b_+9); collision_add_a_to_hl_from_rst(gb, b_+9);
  CYC(b_+9, b_+12); SET_DE(wShieldY);
  CYC(b_+12, b_+15); A = W8(w1Link_yh);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); SET_HL(HL + 1);
  CYC(b_+18, b_+19); E = alu_inc8(gb, E);
  CYC(b_+19, b_+22); A = W8(w1Link_xh);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); SET_HL(HL + 1);
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); E = alu_inc8(gb, E);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); A = 0x80;
  CYC(b_+33, b_+35); H8(hActiveObjectType) = A;
  CYC(b_+35, b_+37); D = 0xd0;
  CYC(b_+37, b_+38); A = D;

next_enemy:
  CYC(b_+38, b_+40); H8(hActiveObject) = A;
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); L = 0xa4;
  CYC(b_+43, b_+45); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYC(b_+45, b_+47);
    CYC(b_+47, b_+48); A = mem_rd(gb, HL);
    CYC(b_+48, b_+50); L = 0xaa;
    CYC(b_+50, b_+52); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) CALL_C_CC(b_+52, enemyCheckCollisions_hook, SYM(enemyCheckCollisions), b_+55);
    else CYC(b_+52, b_+55);
  } else {
    CYCT(b_+45, b_+47);
  }
  CYC(b_+55, b_+56); D = alu_inc8(gb, D);
  CYC(b_+56, b_+57); A = D;
  CYC(b_+57, b_+59); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(b_+59, b_+61);
    goto next_enemy;
  }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); A = 0xc0;
  CYC(b_+63, b_+65); H8(hActiveObjectType) = A;
  CYC(b_+65, b_+67); D = 0xd0;
  CYC(b_+67, b_+68); A = D;

next_part:
  CYC(b_+68, b_+70); H8(hActiveObject) = A;
  CYC(b_+70, b_+71); H = D;
  CYC(b_+71, b_+73); L = 0xe4;
  CYC(b_+73, b_+75); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYC(b_+75, b_+77);
    CYC(b_+77, b_+79); L = 0xea;
    CYC(b_+79, b_+81); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) {
      CYC(b_+81, b_+83);
      CYC(b_+83, b_+84); L = alu_inc8(gb, L);
      CYC(b_+84, b_+85); A = mem_rd(gb, HL);
      CYC(b_+85, b_+86); alu_or(gb, A);
      if (F & FZ) CALL_C_CC(b_+86, partCheckCollisions_hook, SYM(partCheckCollisions), b_+89);
      else CYC(b_+86, b_+89);
    } else {
      CYCT(b_+81, b_+83);
    }
  } else {
    CYCT(b_+75, b_+77);
  }
  CYC(b_+89, b_+90); D = alu_inc8(gb, D);
  CYC(b_+90, b_+91); A = D;
  CYC(b_+91, b_+93); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(b_+93, b_+95);
    goto next_part;
  }
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+96); ret_effect(gb);
}

void partCheckCollisions_hook(GB *gb) {
  BASE(partCheckCollisions);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xe4;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(partActiveCollisions_bank07);
  CYC(b_+6, b_+8); E = 0xcb;
  CYC(b_+8, b_+10);
  collision_check_object(gb, sp0_);
}

void enemyCheckCollisions_hook(GB *gb) {
  BASE(enemyCheckCollisions);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(enemyActiveCollisions_bank07);
  CYC(b_+3, b_+5); E = 0x8b;
  collision_check_object(gb, sp0_);
}

static void collisionEffectDamageLinkTail_hook(GB *gb, uint16_t sp0_) {
  BASE(collisionEffect04);
  CALL_C(b_+2, applyDamageToLink_paramE_hook, SYM(applyDamageToLink_paramE), b_+5);
  CYC(b_+5, b_+7); A = 0x1c;
  CYC(b_+7, b_+10); applyDamageToEnemyOrPart_hook(gb);
}

void collisionEffect00_hook(GB *gb) {
  BASE(collisionEffect00);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void collisionEffect01_hook(GB *gb) {
  BASE(collisionEffect01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x00;
  CYC(b_+2, b_+4); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect02_hook(GB *gb) {
  BASE(collisionEffect02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+4); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect03_hook(GB *gb) {
  BASE(collisionEffect03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x08;
  CYC(b_+2, b_+4); collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect04_hook(GB *gb) {
  BASE(collisionEffect04);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x0c;
  collisionEffectDamageLinkTail_hook(gb, sp0_);
}

void collisionEffect08_hook(GB *gb) {
  BASE(collisionEffect08);
  CYC(b_+0, b_+2); E = 0x00;
  CYC(b_+2, b_+4); TAIL(label_07_027);
}

void collisionEffect09_hook(GB *gb) {
  BASE(collisionEffect09);
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+4); TAIL(label_07_027);
}

void collisionEffect0a_hook(GB *gb) {
  BASE(collisionEffect0a);
  CYC(b_+0, b_+2); E = 0x08;
  CYC(b_+2, b_+4); TAIL(label_07_027);
}

void label_07_027_hook(GB *gb) {
  BASE(label_07_027);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  CYC(b_+2, b_+4); alu_add(gb, 0x3e);
  CYC(b_+4, b_+5); L = A;
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); C = 0x2a;
  CYC(b_+8, b_+9); A = mem_rd(gb, BC);
  CYC(b_+9, b_+10); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+11); mem_wr(gb, BC, A);
  CYC(b_+11, b_+12); A = E;
  CYC(b_+12, b_+15); TAIL(applyDamageToEnemyOrPart);
}

void collisionEffect12_hook(GB *gb) {
  BASE(collisionEffect12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), SYM(collisionEffect0c));
  TAIL(collisionEffect0c);
}

void collisionEffect0c_hook(GB *gb) {
  BASE(collisionEffect0c);
  CYC(b_+0, b_+2); E = 0x10;
  CYC(b_+2, b_+4); TAIL(label_07_028);
}

void collisionEffect13_hook(GB *gb) {
  BASE(collisionEffect13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), SYM(collisionEffect0d));
  TAIL(collisionEffect0d);
}

void collisionEffect0d_hook(GB *gb) {
  BASE(collisionEffect0d);
  CYC(b_+0, b_+2); E = 0x14;
  CYC(b_+2, b_+4); TAIL(label_07_028);
}

void collisionEffect14_hook(GB *gb) {
  BASE(collisionEffect14);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), SYM(collisionEffect0e));
  TAIL(collisionEffect0e);
}

void collisionEffect0e_hook(GB *gb) {
  BASE(collisionEffect0e);
  CYC(b_+0, b_+2); E = 0x18;
  TAIL(label_07_028);
}

void label_07_028_hook(GB *gb) {
  BASE(label_07_028);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  CYC(b_+2, b_+4); alu_add(gb, 0x3e);
  CYC(b_+4, b_+5); L = A;
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); C = 0x2a;
  CYC(b_+8, b_+9); A = mem_rd(gb, BC);
  CYC(b_+9, b_+10); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+11); mem_wr(gb, BC, A);
  CYC(b_+11, b_+12); A = E;
  CYC(b_+12, b_+15); TAIL(applyDamageToEnemyOrPart);
}

void collisionEffect05_hook(GB *gb) {
  BASE(collisionEffect05);
  CYC(b_+0, b_+3); SET_HL(0x101c);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect06_hook(GB *gb) {
  BASE(collisionEffect06);
  CYC(b_+0, b_+3); SET_HL(0x141c);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect07_hook(GB *gb) {
  BASE(collisionEffect07);
  CYC(b_+0, b_+3); SET_HL(0x181c);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect18_hook(GB *gb) {
  BASE(collisionEffect18);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), SYM(collisionEffect0f));
  TAIL(collisionEffect0f);
}

void collisionEffect0f_hook(GB *gb) {
  BASE(collisionEffect0f);
  CYC(b_+0, b_+3); SET_HL(0x1010);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect19_hook(GB *gb) {
  BASE(collisionEffect19);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), SYM(collisionEffect10));
  TAIL(collisionEffect10);
}

void collisionEffect10_hook(GB *gb) {
  BASE(collisionEffect10);
  CYC(b_+0, b_+3); SET_HL(0x1414);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect15_hook(GB *gb) {
  BASE(collisionEffect15);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), b_+3);
  CYC(b_+3, b_+6); SET_HL(0x1034);
  CYC(b_+6, b_+8); TAIL(applyDamageToBothObjects);
}

void collisionEffect16_hook(GB *gb) {
  BASE(collisionEffect16);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), b_+3);
  CYC(b_+3, b_+6); SET_HL(0x1434);
  CYC(b_+6, b_+8); TAIL(applyDamageToBothObjects);
}

void collisionEffect17_hook(GB *gb) {
  BASE(collisionEffect17);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), b_+3);
  CYC(b_+3, b_+6); SET_HL(0x1834);
  CYC(b_+6, b_+8); TAIL(applyDamageToBothObjects);
}

void collisionEffect1a_hook(GB *gb) {
  BASE(collisionEffect1a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), SYM(collisionEffect11));
  TAIL(collisionEffect11);
}

void collisionEffect11_hook(GB *gb) {
  BASE(collisionEffect11);
  CYC(b_+0, b_+3); SET_HL(0x1818);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect1b_hook(GB *gb) {
  BASE(collisionEffect1b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, createClinkInteraction_hook, SYM(createClinkInteraction), b_+3);
  CYC(b_+3, b_+6); SET_HL(0x1c28);
  CYC(b_+6, b_+8); TAIL(applyDamageToBothObjects);
}

void collisionEffect1d_hook(GB *gb) {
  BASE(collisionEffect1d);
  CYC(b_+0, b_+3); SET_HL(0x0c04);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect1e_hook(GB *gb) {
  BASE(collisionEffect1e);
  CYC(b_+0, b_+3); SET_HL(0x2834);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect1f_hook(GB *gb) {
  BASE(collisionEffect1f);
  CYC(b_+0, b_+3); SET_HL(0x2034);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect20_hook(GB *gb) {
  BASE(collisionEffect20);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = 0x01;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0x28);
  if (!(F & FC)) {
    CYCT(b_+6, b_+8);
  } else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+10); L = 0x24;
    CYC(b_+10, b_+12); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  }
  CALL_C(b_+12, func_07_47b7_hook, SYM(func_07_47b7), b_+15);
  if (F & FZ) { CYCT(b_+15, b_+16); ret_effect(gb); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+19); SET_HL(0x2444);
  CYC(b_+19, b_+21); TAIL(applyDamageToBothObjects);
}

void collisionEffect22_hook(GB *gb) {
  BASE(collisionEffect22);
  CYC(b_+0, b_+3); SET_HL(0x1c24);
  TAIL(applyDamageToBothObjects);
}

void collisionEffect26_hook(GB *gb) {
  BASE(collisionEffect26);
  CYC(b_+0, b_+3); SET_HL(0x1c34);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect27_hook(GB *gb) {
  BASE(collisionEffect27);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = 0x24;
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+5, func_07_47b7_hook, SYM(func_07_47b7), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, createFlamePart_hook, SYM(createFlamePart), b_+12);
  CYC(b_+12, b_+15); SET_HL(0x1c2c);
  CYC(b_+15, b_+17); TAIL(applyDamageToBothObjects);
}

void collisionEffect28_hook(GB *gb) {
  BASE(collisionEffect28);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = 0x24;
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+5, func_07_47b7_hook, SYM(func_07_47b7), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_HL(0x1c38);
  CYC(b_+12, b_+14); TAIL(applyDamageToBothObjects);
}

void collisionEffect3a_hook(GB *gb) {
  BASE(collisionEffect3a);
  CYC(b_+0, b_+2); E = 0xad;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  TAIL(collisionEffect3d);
}

void collisionEffect3d_hook(GB *gb) {
  BASE(collisionEffect3d);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_id);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wWarpsDisabled);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x0d;
  CYC(b_+12, b_+15); mem_wr(gb, wLinkForceState, A);
  CYC(b_+15, b_+18); SET_HL(0x2c1c);
  CYC(b_+18, b_+20); TAIL(applyDamageToBothObjects);
}

void collisionEffect2b_hook(GB *gb) {
  BASE(collisionEffect2b);
  CYC(b_+0, b_+3); SET_HL(0x1c3c);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect2c_hook(GB *gb) {
  BASE(collisionEffect2c);
  CYC(b_+0, b_+3); SET_HL(0x1430);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect2f_hook(GB *gb) {
  BASE(collisionEffect2f);
  CYC(b_+0, b_+3); SET_HL(0x3004);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect30_hook(GB *gb) {
  BASE(collisionEffect30);
  CYC(b_+0, b_+3); SET_HL(0x1c44);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect1c_hook(GB *gb) {
  BASE(collisionEffect1c);
  CYC(b_+0, b_+3); SET_HL(0x1c1c);
  CYC(b_+3, b_+5); TAIL(applyDamageToBothObjects);
}

void collisionEffect23_hook(GB *gb) {
  BASE(collisionEffect23);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  CYC(b_+2, b_+4); alu_add(gb, 0x29);
  CYC(b_+4, b_+5); L = A;
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void collisionEffect24_hook(GB *gb) {
  BASE(collisionEffect24);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  CYC(b_+2, b_+4); alu_add(gb, 0x2a);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+7); A = H8(hFF90);
  CYC(b_+7, b_+9); alu_or(gb, 0x80);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+11); A = E;
  CYC(b_+11, b_+13); alu_add(gb, 0xec);
  CYC(b_+13, b_+14); L = A;
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+16); mem_wr(gb, HL, C);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+18); mem_wr(gb, HL, B);
  CYC(b_+18, b_+20); C = 0x2a;
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+23); mem_wr(gb, BC, A);
  CYC(b_+23, b_+24); ret_effect(gb);
}

void collisionEffect25_hook(GB *gb) {
  BASE(collisionEffect25);
  uint16_t sp0_ = gb->sp;

  CALL_C(b_+0, killEnemyOrPart_hook, SYM(killEnemyOrPart), b_+3);
  CYC(b_+3, b_+4); A = L;
  CYC(b_+4, b_+6); alu_add(gb, 0x1b);
  CYC(b_+6, b_+7); L = A;
  CYC(b_+7, b_+9); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+9, b_+11); C = 0x2a;
  CYC(b_+11, b_+13); A = 0x02;
  CYC(b_+13, b_+14); mem_wr(gb, BC, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void collisionEffect2a_hook(GB *gb) {
  BASE(collisionEffect2a);
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = 0x2d;
  CYC(b_+3, b_+4); A = D;
  CYC(b_+4, b_+5); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); L--;
  CYC(b_+7, b_+9); E = 0xe1;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); L--;
  CYC(b_+11, b_+12); L = alu_dec8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+14, b_+16); E = 0xea;
  CYC(b_+16, b_+18); A = H8(hFF90);
  CYC(b_+18, b_+20); alu_or(gb, 0x80);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void collisionEffect2d_hook(GB *gb) {
  BASE(collisionEffect2d);
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = 0x2f;
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void collisionEffect31_hook(GB *gb) {
  BASE(collisionEffect31);
  CYC(b_+0, b_+2); A = 0x34;
  CYC(b_+2, b_+5); TAIL(applyDamageToEnemyOrPart);
}

void collisionEffect32_hook(GB *gb) {
  BASE(collisionEffect32);
  CYC(b_+0, b_+3); SET_HL(0x3448);
  CYC(b_+3, b_+5); TAIL(label_07_033);
}

void collisionEffect33_hook(GB *gb) {
  BASE(collisionEffect33);
  CYC(b_+0, b_+3); SET_HL(0x384c);
  TAIL(label_07_033);
}

void label_07_033_hook(GB *gb) {
  BASE(label_07_033);
  uint16_t sp0_ = gb->sp;

  CALL_C(b_+0, applyDamageToBothObjects_hook, SYM(applyDamageToBothObjects), b_+3);
  CYC(b_+3, b_+6); TAIL(createClinkInteraction);
}

void collisionEffect34_hook(GB *gb) {
  BASE(collisionEffect34);
  uint16_t sp0_ = gb->sp;

  CALL_C(b_+0, createFlamePart_hook, SYM(createFlamePart), b_+3);
  CYC(b_+3, b_+4); H = B;
  CYC(b_+4, b_+6); L = 0x24;
  CYC(b_+6, b_+8); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+8, b_+11); SET_HL(0x1c2c);
  CALL_C(b_+11, applyDamageToBothObjects_hook, SYM(applyDamageToBothObjects), b_+14);
  CYC(b_+14, b_+16); TAIL(killEnemyOrPart);
}

void collisionEffect35_hook(GB *gb) {
  BASE(collisionEffect35);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+3); SET_HL(0x1c1c);
  CALL_C(b_+3, applyDamageToBothObjects_hook, SYM(applyDamageToBothObjects), SYM(killEnemyOrPart));
  TAIL(killEnemyOrPart);
}

void killEnemyOrPart_hook(GB *gb) {
  BASE(killEnemyOrPart);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); A = H8(hActiveObjectType);
  CYC(b_+3, b_+5); alu_add(gb, 0x29);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+10); alu_add(gb, 0xfb);
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void collisionEffect3c_hook(GB *gb) {
  BASE(collisionEffect3c);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  CYC(b_+2, b_+3); A = alu_inc8(gb, A);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+9); SET_HL(b_+41);
  for (;;) {
    CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+10, b_+11); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+11, b_+13);
      TAIL(collisionEffect02);
    }
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+14); alu_cp(gb, C);
    CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
    if (!(F & FZ)) {
      CYCT(b_+15, b_+17);
      continue;
    }
    CYC(b_+15, b_+17);
    break;
  }
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20); alu_and(gb, 0x7f);
  CALL_C(b_+20, cpActiveRing_hook, SYM(cpActiveRing), b_+23);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+25);
    TAIL(collisionEffect02);
  }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_bit(gb, 7, C);
  CYC(b_+27, b_+29); A = 0x40;
  if (F & FZ) {
    CYCT(b_+29, b_+32);
    TAIL(applyDamageToEnemyOrPart);
  }
  CYC(b_+29, b_+32);
  CALL_C(b_+32, collisionEffect02_hook, SYM(collisionEffect02), b_+35);
  CYC(b_+35, b_+36); H = B;
  CYC(b_+36, b_+38); L = 0x25;
  CYC(b_+38, b_+40); mem_wr(gb, HL, alu_sra(gb, mem_rd(gb, HL)));
  CYC(b_+40, b_+41); ret_effect(gb);
}

void collisionEffect0b_hook(GB *gb) {
  BASE(collisionEffect0b);
  uint16_t sp0_ = gb->sp;

  CALL_C(b_+0, func_07_47b7_hook, SYM(func_07_47b7), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = 0x0c;
  CYC(b_+6, b_+8); TAIL(label_07_027);
}

void collisionEffect21_hook(GB *gb) {
  BASE(collisionEffect21);
  CYC(b_+0, b_+2); E = 0x30;
  TAIL(label_07_027);
}

void collisionEffect2e_hook(GB *gb) {
  BASE(collisionEffect2e);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); A = H8(hActiveObjectType);
  CYC(b_+3, b_+5); alu_add(gb, 0x29);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    TAIL(collisionEffect1c);
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = L;
  CYC(b_+11, b_+13); alu_add(gb, 0x05);
  CYC(b_+13, b_+14); L = A;
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+17, b_+19); A = H8(hFF8A);
  CYC(b_+19, b_+21); alu_xor(gb, 0x10);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); L &= 0xf7;
  CYC(b_+24, b_+26); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+26, b_+27); A = L;
  CYC(b_+27, b_+29); alu_add(gb, 0xe0);
  CYC(b_+29, b_+30); L = A;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x03);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x00);
  CYC(b_+35, b_+36); H = B;
  CYC(b_+36, b_+38); L = 0x2a;
  CYC(b_+38, b_+40); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+40, b_+42); L = 0x24;
  CYC(b_+42, b_+44); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+44, b_+46); L = 0x18;
  CYC(b_+46, b_+48); A = H8(hActiveObjectType);
  CYC(b_+48, b_+49); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+49, b_+50); mem_wr(gb, HL, D);
  CYC(b_+50, b_+51); ret_effect(gb);
}

void collisionEffect29_hook(GB *gb) {
  BASE(collisionEffect29);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = 0x24;
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+5, func_07_47b7_hook, SYM(func_07_47b7), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0xaa;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x9e);
  CYC(b_+14, b_+16); L = 0xae;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x00);
  CYC(b_+18, b_+20); L = 0xa4;
  CYC(b_+20, b_+22); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+22, b_+24); L = 0x84;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x05);
  CYC(b_+26, b_+28); L = 0x9a;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); alu_and(gb, 0xc0);
  CYC(b_+31, b_+33); alu_or(gb, 0x02);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+36); L = 0x87;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x1e);
  CYC(b_+38, b_+40); L = 0x90;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x05);
  CYC(b_+42, b_+44); L = 0x94;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x00);
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0xfa);
  CYC(b_+49, b_+51); L = 0x8b;
  CYC(b_+51, b_+53); C = 0x0b;
  CYC(b_+53, b_+54); A = mem_rd(gb, BC);
  CYC(b_+54, b_+55); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+58); C = 0x0d;
  CYC(b_+58, b_+59); A = mem_rd(gb, BC);
  CYC(b_+59, b_+60); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+60, b_+61); L = alu_inc8(gb, L);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+63); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+63, b_+65);
  } else {
    CYC(b_+63, b_+65);
    CYC(b_+65, b_+67); mem_wr(gb, HL, 0xff);
  }
  CALL_C(b_+67, getRandomNumber_hook, SYM(getRandomNumber), b_+70);
  CYC(b_+70, b_+72); alu_and(gb, 0x18);
  CYC(b_+72, b_+74); E = 0x89;
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+77); A = 0x1c;
  CYC(b_+77, b_+80); TAIL(applyDamageToLink);
}

void collisionEffect36_hook(GB *gb) {
  BASE(collisionEffect36);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); A = H8(hActiveObjectType);
  CYC(b_+3, b_+5); alu_add(gb, 0x2a);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xa0);
  CYC(b_+8, b_+10); alu_add(gb, 0xfa);
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+13, b_+15); A = 0x1e;
  CALL_C(b_+15, cpActiveRing_hook, SYM(cpActiveRing), b_+18);
  CYC(b_+18, b_+20); A = 0xf8;
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
  } else {
    CYC(b_+20, b_+22);
    CYC(b_+22, b_+23); alu_xor(gb, A);
  }
  CYC(b_+23, b_+26); SET_HL(w1Link_damageToApply);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = 0x2c;
  CYC(b_+29, b_+31); A = H8(hFF8A);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+34); L = 0x2d;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x08);
  CYC(b_+36, b_+38); L = 0x2b;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x0c);
  CYC(b_+40, b_+43); A = mem_rd(gb, wIsLinkBeingShocked);
  CYC(b_+43, b_+44); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+44, b_+46);
  } else {
    CYC(b_+44, b_+46);
    CYC(b_+46, b_+47); A = alu_inc8(gb, A);
    CYC(b_+47, b_+50); mem_wr(gb, wIsLinkBeingShocked, A);
  }
  CYC(b_+50, b_+51); H = B;
  CYC(b_+51, b_+53); L = 0x24;
  CYC(b_+53, b_+55); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+55, b_+57); A = 0x1c;
  CYC(b_+57, b_+60); TAIL(applyDamageToLink);
}

void collisionEffect37_hook(GB *gb) {
  BASE(collisionEffect37);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  CYC(b_+2, b_+4); alu_add(gb, 0x2b);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wWarpsDisabled);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, w1Link_state);
  CYC(b_+16, b_+18); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); A = E;
  CYC(b_+20, b_+22); alu_add(gb, 0xf9);
  CYC(b_+22, b_+23); E = A;
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); A = 0x0c;
  CYC(b_+27, b_+30); mem_wr(gb, wLinkForceState, A);
  CYC(b_+30, b_+32); A = 0x1c;
  CYC(b_+32, b_+35); TAIL(applyDamageToEnemyOrPart);
}

void collisionEffect38_hook(GB *gb) {
  BASE(collisionEffect38);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); A = H8(hActiveObjectType);
  CYC(b_+3, b_+5); alu_add(gb, 0x24);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+8, b_+10); alu_add(gb, 0xe2);
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x60);
  CYC(b_+13, b_+15); alu_add(gb, 0x09);
  CYC(b_+15, b_+16); L = A;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x00);
  CYC(b_+18, b_+20); A = 0x1c;
  CYC(b_+20, b_+23); TAIL(applyDamageToEnemyOrPart);
}

void collisionEffect39_hook(GB *gb) {
  BASE(collisionEffect39);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void collisionEffect3b_hook(GB *gb) {
  BASE(collisionEffect3b);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+2); A = 0x02;
  CALL_C(b_+2, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+5);
  CYC(b_+5, b_+7); A = 0x1c;
  CYC(b_+7, b_+10); TAIL(applyDamageToEnemyOrPart);
}

void collisionEffect3e_hook(GB *gb) {
  BASE(collisionEffect3e);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void collisionEffect3f_hook(GB *gb) {
  BASE(collisionEffect3f);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void applyDamageToBothObjects_hook(GB *gb) {
  BASE(applyDamageToBothObjects);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); A = H;
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CALL_C(b_+2, applyDamageToLink_hook, SYM(applyDamageToLink), b_+5);
  CYC(b_+5, b_+6); SET_HL(pop_effect(gb));
  CYC(b_+6, b_+7); A = L;
  CYC(b_+7, b_+10); TAIL(applyDamageToEnemyOrPart);
}

void createFlamePart_hook(GB *gb) {
  BASE(createFlamePart);
  uint16_t sp0_ = gb->sp;

  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x12);
  CYC(b_+6, b_+8); L = 0xd6;
  CYC(b_+8, b_+10); A = H8(hActiveObjectType);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, HL, D);
  CYC(b_+12, b_+13); ret_effect(gb);
}

void createClinkInteraction_hook(GB *gb) {
  BASE(createClinkInteraction);
  uint16_t sp0_ = gb->sp;

  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
  } else {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+7); mem_wr(gb, HL, 0x07);
    CYC(b_+7, b_+9); A = H8(hFF8F);
    CYC(b_+9, b_+10); L = A;
    CYC(b_+10, b_+12); A = H8(hFF8D);
    CYC(b_+12, b_+13); alu_sub(gb, L);
    CYC(b_+13, b_+15); A = alu_sra(gb, A);
    CYC(b_+15, b_+16); alu_add(gb, L);
    CYC(b_+16, b_+18); L = 0x4b;
    CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+19, b_+21); A = H8(hFF8E);
    CYC(b_+21, b_+22); L = A;
    CYC(b_+22, b_+24); A = H8(hFF8C);
    CYC(b_+24, b_+25); alu_sub(gb, L);
    CYC(b_+25, b_+27); A = alu_sra(gb, A);
    CYC(b_+27, b_+28); alu_add(gb, L);
    CYC(b_+28, b_+30); L = 0x4d;
    CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  }
  CYC(b_+31, b_+32); ret_effect(gb);
}

void applyDamageToEnemyOrPart_hook(GB *gb) {
  BASE(applyDamageToEnemyOrPart);
  CYC(b_+0, b_+3); SET_HL(b_+88);
  CYC(b_+3, b_+4); collision_add_a_to_hl_from_rst(gb, b_+4);
  CYC(b_+4, b_+6); A = H8(hActiveObjectType);
  CYC(b_+6, b_+8); alu_add(gb, 0x29);
  CYC(b_+8, b_+9); E = A;
  CYC(b_+9, b_+11); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+11, b_+13);
  } else {
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+15); C = 0x28;
    CYC(b_+15, b_+16); A = mem_rd(gb, BC);
    CYC(b_+16, b_+17); C = A;
    CYC(b_+17, b_+18); A = mem_rd(gb, DE);
    CYC(b_+18, b_+19); alu_add(gb, C);
    if (F & FC) {
      CYCT(b_+19, b_+21);
    } else {
      CYC(b_+19, b_+21);
      CYC(b_+21, b_+22); alu_xor(gb, A);
    }
    CYC(b_+22, b_+23); mem_wr(gb, DE, A);
    if (!(F & FZ)) {
      CYCT(b_+23, b_+25);
    } else {
      CYC(b_+23, b_+25);
      CYC(b_+25, b_+26); C = E;
      CYC(b_+26, b_+27); A = E;
      CYC(b_+27, b_+29); alu_add(gb, 0xfb);
      CYC(b_+29, b_+30); E = A;
      CYC(b_+30, b_+31); A = mem_rd(gb, DE);
      CYC(b_+31, b_+33); A &= 0x7f;
      CYC(b_+33, b_+34); mem_wr(gb, DE, A);
      CYC(b_+34, b_+35); E = C;
    }
  }
  CYC(b_+35, b_+36); E = alu_inc8(gb, E);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+37, b_+38); C = A;
  CYC(b_+38, b_+40); alu_bit(gb, 6, C);
  if (F & FZ) {
    CYCT(b_+40, b_+42);
  } else {
    CYC(b_+40, b_+42);
    CYC(b_+42, b_+44); A = H8(hFF90);
    CYC(b_+44, b_+46); alu_or(gb, 0x80);
    CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  }
  CYC(b_+47, b_+48); E = alu_inc8(gb, E);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+49, b_+51); alu_bit(gb, 5, C);
  if (F & FZ) {
    CYCT(b_+51, b_+53);
  } else {
    CYC(b_+51, b_+53);
    CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  }
  CYC(b_+54, b_+55); E = alu_inc8(gb, E);
  CYC(b_+55, b_+56); E = alu_inc8(gb, E);
  CYC(b_+56, b_+58); alu_bit(gb, 4, C);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (F & FZ) {
    CYCT(b_+59, b_+61);
  } else {
    CYC(b_+59, b_+61);
    CYC(b_+61, b_+62); mem_wr(gb, DE, A);
    CYC(b_+62, b_+64); A = H8(hFF8A);
    CYC(b_+64, b_+66); alu_xor(gb, 0x10);
    CYC(b_+66, b_+67); E = alu_dec8(gb, E);
    CYC(b_+67, b_+68); mem_wr(gb, DE, A);
    CYC(b_+68, b_+69); E = alu_inc8(gb, E);
  }
  CYC(b_+69, b_+70); E = alu_inc8(gb, E);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+71, b_+73); alu_bit(gb, 3, C);
  if (F & FZ) {
    CYCT(b_+73, b_+75);
  } else {
    CYC(b_+73, b_+75);
    CYC(b_+75, b_+76); mem_wr(gb, DE, A);
  }
  CYC(b_+76, b_+77); A = C;
  CYC(b_+77, b_+79); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+79, b_+80); ret_effect(gb); return;
  }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+83); SET_HL(b_+168);
  CYC(b_+83, b_+84); collision_add_a_to_hl_from_rst(gb, b_+84);
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+88); TAIL(playSound_b00);
}

void func_07_47b7_hook(GB *gb) {
  BASE(func_07_47b7);
  CYC(b_+0, b_+2); C = 0x01;
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+5); alu_cp(gb, 0x24);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = H8(hActiveObjectType);
  CYC(b_+8, b_+10); alu_add(gb, 0x3f);
  CYC(b_+10, b_+11); E = A;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_cpl(gb);
  CYC(b_+13, b_+15); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); H = B;
  CYC(b_+17, b_+19); L = 0x2a;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x40);
  CYC(b_+21, b_+23); L = 0x24;
  CYC(b_+23, b_+25); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+25, b_+27); A = H8(hActiveObjectType);
  CYC(b_+27, b_+29); alu_add(gb, 0x2a);
  CYC(b_+29, b_+30); E = A;
  CYC(b_+30, b_+32); A = 0x9a;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+34); A = E;
  CYC(b_+34, b_+36); alu_add(gb, 0x04);
  CYC(b_+36, b_+37); E = A;
  CYC(b_+37, b_+38); alu_xor(gb, A);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+40); ret_effect(gb);
}

void applyDamageToLink_paramE_hook(GB *gb) {
  BASE(applyDamageToLink_paramE);
  CYC(b_+0, b_+1); A = E;
  TAIL(applyDamageToLink);
}

void applyDamageToLink_hook(GB *gb) {
  BASE(applyDamageToLink);
  CYC(b_+0, b_+1); push_effect(gb, AF);
  CYC(b_+1, b_+3); A = H8(hActiveObjectType);
  CYC(b_+3, b_+5); alu_add(gb, 0x3e);
  CYC(b_+5, b_+6); E = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+10); W8(wTmpcec0) = A;
  CYC(b_+10, b_+11); SET_AF(pop_effect(gb));
  CYC(b_+11, b_+14); SET_HL(b_+78);
  CYC(b_+14, b_+15); collision_add_a_to_hl_from_rst(gb, b_+15);
  CYC(b_+15, b_+17); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+17, b_+19);
  } else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+21); A = H8(hActiveObjectType);
    CYC(b_+21, b_+23); alu_add(gb, 0x28);
    CYC(b_+23, b_+24); E = A;
    CYC(b_+24, b_+25); A = mem_rd(gb, DE);
    CYC(b_+25, b_+27); C = 0x25;
    CYC(b_+27, b_+28); mem_wr(gb, BC, A);
  }
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); E = A;
  CYC(b_+30, b_+32); C = 0x2a;
  CYC(b_+32, b_+33); A = mem_rd(gb, BC);
  CYC(b_+33, b_+34); C = A;
  CYC(b_+34, b_+37); A = W8(wTmpcec0);
  CYC(b_+37, b_+38); alu_or(gb, C);
  CYC(b_+38, b_+40); C = 0x2a;
  CYC(b_+40, b_+41); mem_wr(gb, BC, A);
  CYC(b_+41, b_+42); C = alu_inc8(gb, C);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+43, b_+45); alu_bit(gb, 5, E);
  if (F & FZ) {
    CYCT(b_+45, b_+47);
  } else {
    CYC(b_+45, b_+47);
    CYC(b_+47, b_+48); mem_wr(gb, BC, A);
  }
  CYC(b_+48, b_+49); C = alu_inc8(gb, C);
  CYC(b_+49, b_+51); A = H8(hFF8A);
  CYC(b_+51, b_+52); mem_wr(gb, BC, A);
  CYC(b_+52, b_+53); C = alu_inc8(gb, C);
  CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+54, b_+56); alu_bit(gb, 4, E);
  if (F & FZ) {
    CYCT(b_+56, b_+58);
  } else {
    CYC(b_+56, b_+58);
    CYC(b_+58, b_+59); mem_wr(gb, BC, A);
  }
  CYC(b_+59, b_+60); C = alu_inc8(gb, C);
  CYC(b_+60, b_+61); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+61, b_+63); alu_bit(gb, 4, E);
  if (F & FZ) {
    CYCT(b_+63, b_+65);
  } else {
    CYC(b_+63, b_+65);
    CYC(b_+65, b_+66); mem_wr(gb, BC, A);
  }
  CYC(b_+66, b_+67); A = E;
  CYC(b_+67, b_+69); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+69, b_+70); ret_effect(gb); return;
  }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+73); SET_HL(b_+138);
  CYC(b_+73, b_+74); collision_add_a_to_hl_from_rst(gb, b_+74);
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CYC(b_+75, b_+78); TAIL(playSound_b00);
}
