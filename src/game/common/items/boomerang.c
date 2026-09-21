#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(itemCode06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boomerang_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == b_+63) { goto state1; }
    else if (jt_ == b_+147) { goto state2; }
    else if (jt_ == b_+164) { goto state3; }
    else if (jt_ == b_+192) { goto state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+14, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+17);
  CYC(b_+17, b_+19); A = 0x18;
  CALL_C(b_+19, loadWeaponGfx_b00_hook, SYM(loadWeaponGfx_b00), b_+22);
  CALL_C(b_+22, itemIncState_hook, SYM(itemIncState), b_+25);
  CYC(b_+25, b_+27); L = 0x10;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x41);
  CYC(b_+29, b_+31); L = 0x06;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x28);
  CYC(b_+33, b_+35); C = 0xff;
  CYC(b_+35, b_+37); A = 0x0d;
  CALL_C(b_+37, cpActiveRing_hook, SYM(cpActiveRing), b_+40);
  if (F & FZ) {
    CYCT(b_+40, b_+42);
    goto increase_damage;
  }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); A = 0x29;
  CALL_C(b_+44, cpActiveRing_hook, SYM(cpActiveRing), b_+47);
  if (!(F & FZ)) {
    CYCT(b_+47, b_+49);
    goto make_visible;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); C = 0xfe;

increase_damage:
  CYC(b_+51, b_+53); L = 0x28;
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); alu_add(gb, C);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);

make_visible:
  CALL_C(b_+56, objectSetVisible82_hook, SYM(objectSetVisible82), b_+59);
  CYC(b_+59, b_+60); alu_xor(gb, A);
  CYC(b_+60, b_+63);
  TAIL(itemSetAnimation);

state1:
  CYC(b_+63, b_+65); E = 0x2a;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+67, b_+69);
    goto return_to_link;
  }
  CYC(b_+67, b_+69);
  CALL_C(b_+69, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+72);
  if (!(F & FC)) {
    CYCT(b_+72, b_+74);
    goto no_collision;
  }
  CYC(b_+72, b_+74);
  CALL_C(b_+74, itemCheckCanPassSolidTile_hook, SYM(itemCheckCanPassSolidTile), b_+77);
  if (!(F & FZ)) {
    CYCT(b_+77, b_+79);
    goto hit_wall;
  }
  CYC(b_+77, b_+79);

no_collision:
  CALL_C(b_+79, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+82);
  if (!(F & FC)) {
    CYCT(b_+82, b_+84);
    goto return_to_link;
  }
  CYC(b_+82, b_+84);
  CYC(b_+84, b_+86); E = 0x34;
  CYC(b_+86, b_+87); A = mem_rd(gb, DE);
  CALL_C(b_+87, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+90);
  CALL_C(b_+90, itemDecCounter1_hook, SYM(itemDecCounter1), b_+93);
  if (!(F & FZ)) {
    CYCT(b_+93, b_+95);
    goto update_speed_and_animation;
  }
  CYC(b_+93, b_+95);

return_to_link:
  CALL_C(b_+95, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+98);
  CYC(b_+98, b_+99); C = A;
  CYC(b_+99, b_+100); H = D;
  CYC(b_+100, b_+102); L = 0x0b;
  CYC(b_+102, b_+104); A = 0xf0;
  CYC(b_+104, b_+105); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+105, b_+107);
    goto set_angle;
  }
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+109); L = 0x0d;
  CYC(b_+109, b_+110); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+110, b_+112);
    goto set_angle;
  }
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+114); L = 0x09;
  CYC(b_+114, b_+115); A = C;
  CYC(b_+115, b_+116); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+116, b_+118); alu_add(gb, 0x08);
  CYC(b_+118, b_+120); alu_cp(gb, 0x11);
  if (F & FC) {
    CYCT(b_+120, b_+122);
    goto next_state;
  }
  CYC(b_+120, b_+122);

set_angle:
  CYC(b_+122, b_+124); L = 0x09;
  CYC(b_+124, b_+125); mem_wr(gb, HL, C);
  CYC(b_+125, b_+127);
  goto next_state;

hit_wall:
  CALL_C(b_+127, objectCreateClinkInteraction_hook, SYM(objectCreateClinkInteraction), b_+130);
  CYC(b_+130, b_+131); H = D;
  CYC(b_+131, b_+133); L = 0x09;
  CYC(b_+133, b_+134); A = mem_rd(gb, HL);
  CYC(b_+134, b_+136); alu_xor(gb, 0x10);
  CYC(b_+136, b_+137); mem_wr(gb, HL, A);

next_state:
  CYC(b_+137, b_+139); L = 0x04;
  CYC(b_+139, b_+140); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+140, b_+142); L = 0x16;
  CYC(b_+142, b_+143); alu_xor(gb, A);
  CYC(b_+143, b_+144); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);
  CYC(b_+145, b_+147);
  goto update_speed_and_animation;

state2:
  CALL_C(b_+147, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+150);
  CALL_C(b_+150, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+153);
  CYC(b_+153, b_+156); SET_BC(0x140a);
  CALL_C(b_+156, itemCheckWithinRangeOfLink_hook, SYM(itemCheckWithinRangeOfLink), b_+159);
  if (F & FC) CALL_C_CC(b_+159, itemIncState_hook, SYM(itemIncState), b_+162);
  else CYC(b_+159, b_+162);
  CYC(b_+162, b_+164);
  goto update_speed_and_animation;

state3:
  CALL_C(b_+164, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+167);
  CYC(b_+167, b_+169); E = 0x09;
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);
  CYC(b_+170, b_+173); SET_BC(0x0402);
  CALL_C(b_+173, itemCheckWithinRangeOfLink_hook, SYM(itemCheckWithinRangeOfLink), b_+176);
  if (!(F & FC)) {
    CYCT(b_+176, b_+178);
    goto update_speed_and_animation;
  }
  CYC(b_+176, b_+178);
  CALL_C(b_+178, itemIncState_hook, SYM(itemIncState), b_+181);
  CYC(b_+181, b_+183); L = 0x06;
  CYC(b_+183, b_+185); mem_wr(gb, HL, 0x04);
  CYC(b_+185, b_+187); L = 0x24;
  CYC(b_+187, b_+189); mem_wr(gb, HL, 0x00);
  CYC(b_+189, b_+192);
  TAIL(objectSetInvisible);

state4:
  CALL_C(b_+192, itemDecCounter1_hook, SYM(itemDecCounter1), b_+195);
  if (F & FZ) {
    CYCT(b_+195, b_+198);
    TAIL(itemDelete);
  }
  CYC(b_+195, b_+198);
  CYC(b_+198, b_+201); A = W8(wLinkObjectIndex);
  CYC(b_+201, b_+202); H = A;
  CYC(b_+202, b_+204); L = 0x0b;
  CYC(b_+204, b_+207);
  TAIL(objectTakePosition);

update_speed_and_animation:
  CALL_C(b_+207, objectApplySpeed_hook, SYM(objectApplySpeed), b_+210);
  CYC(b_+210, b_+211); H = D;
  CYC(b_+211, b_+213); L = 0x21;
  CYC(b_+213, b_+214); A = mem_rd(gb, HL);
  CYC(b_+214, b_+215); alu_or(gb, A);
  CYC(b_+215, b_+217); mem_wr(gb, HL, 0x00);
  CYC(b_+217, b_+219); A = 0x78;
  if (!(F & FZ)) CALL_C_CC(b_+219, playSound_b00_hook, SYM(playSound_b00), b_+222);
  else CYC(b_+219, b_+222);
  CYC(b_+222, b_+225);
  TAIL(itemAnimate);
}

void itemCheckWithinRangeOfLink_hook(GB *gb) {
  BASE(itemCheckWithinRangeOfLink);
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+5); E = 0x0b;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); alu_add(gb, C);
  CYC(b_+8, b_+9); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); L = 0x0d;
  CYC(b_+12, b_+14); E = 0x0d;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); alu_add(gb, C);
  CYC(b_+17, b_+18); alu_cp(gb, B);
  CYC(b_+18, b_+19); ret_effect(gb);
}
