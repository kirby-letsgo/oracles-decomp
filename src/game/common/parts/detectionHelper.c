#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t detectionHelper_jump_table(GB *gb) {
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

static void detectionHelper_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode0e_hook(GB *gb);
void detectionHelper_spawnCollisionHelper_hook(GB *gb);

void partCode0e_hook(GB *gb) {
  BASE(partCode0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); partDelete_hook(gb); return; } // jp nz
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); E = 0xc2; // Part.subid
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); E = 0xc4; // Part.state
  {
    CYC(b_+8, b_+9); push_effect(gb, b_+9);
    uint16_t target = detectionHelper_jump_table(gb);
    if (target == b_+17) goto subid0;
    if (target == b_+117) goto subid1;
    goto subid2; // subid2 and subid3 alias the same address
  }

subid0:
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+19, b_+21); goto subid0_state0; } // jr z
  CYC(b_+19, b_+21);

  CYC(b_+21, b_+23); A = 0x00; // Object.enabled
  CALL_C(b_+23, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+31); partDelete_hook(gb); return; } // jp z
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+33); E = 0xc9; // Part.angle
  CYC(b_+33, b_+34); A = L;
  CYC(b_+34, b_+36); alu_or(gb, 0x09); // Object.angle
  CYC(b_+36, b_+37); L = A;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CALL_C(b_+39, objectTakePosition_hook, SYM(objectTakePosition), b_+42);
  CALL_C(b_+42, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+45);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto L_48f2; } // jr nz
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x0f); // [counter1]
  CYC(b_+49, b_+51); E = 0xc9; // Part.angle
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); B = A;
  CYC(b_+53, b_+55); E = 0x01;
  CYC(b_+55, b_+58); detectionHelper_spawnCollisionHelper_hook(gb);

L_48f2:
  CYC(b_+58, b_+59); H = D;
  CYC(b_+59, b_+61); L = 0xc7; // Part.counter2
  CYC(b_+61, b_+62); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+62); return; } // ret nz
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x06); // [counter2]
  CYC(b_+65, b_+67); L = 0xc3; // Part.var03
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+69); A = alu_inc8(gb, A);
  CYC(b_+69, b_+71); alu_and(gb, 0x03);
  CYC(b_+71, b_+72); mem_wr(gb, HL, A);
  CYC(b_+72, b_+73); C = A;
  CYC(b_+73, b_+75); L = 0xc9; // Part.angle
  CYC(b_+75, b_+76); B = mem_rd(gb, HL);
  CYC(b_+76, b_+78); E = 0x02;
  CYC(b_+78, b_+81); detectionHelper_spawnCollisionHelper_hook(gb);
  CYC(b_+81, b_+83); E = 0x03;
  detectionHelper_spawnCollisionHelper_hook(gb); ret_effect(gb); return; // falls through: the helper's ret is this routine's

subid0_state0:
  CYC(b_+108, b_+109); H = D;
  CYC(b_+109, b_+110); L = E;
  CYC(b_+110, b_+111); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid]
  CYC(b_+111, b_+113); L = 0xc6; // Part.counter1
  CYC(b_+113, b_+114); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+114, b_+115); L = alu_inc8(gb, L);
  CYC(b_+115, b_+116); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter2]
  RET(b_+116); return; // ret

subid1:
  CYC(b_+117, b_+118); A = mem_rd(gb, DE);
  CYC(b_+118, b_+119); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+119, b_+121); goto subid1_state0; } // jr z
  CYC(b_+119, b_+121);

subid1_state1:
  CALL_C(b_+121, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+124);
  if (F & FC) { CYCT(b_+124, b_+126); goto sawLink; } // jr c
  CYC(b_+124, b_+126);
  CALL_C(b_+126, objectApplyComponentSpeed_hook, SYM(objectApplyComponentSpeed), b_+129);
  CALL_C(b_+129, objectCheckSimpleCollision_hook, SYM(objectCheckSimpleCollision), b_+132);
  if (F & FZ) { RET_TAKEN(b_+132); return; } // ret z
  CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); goto delete; // jr

sawLink:
  CYC(b_+135, b_+137); A = 0x3b; // Object.var3b
  CALL_C(b_+137, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+140);
  CYC(b_+140, b_+142); mem_wr(gb, HL, 0xff);

delete:
  CYC(b_+142, b_+145); TAIL(partDelete); // jp

subid1_state0:
  CYC(b_+145, b_+146); A = alu_inc8(gb, A);
  CYC(b_+146, b_+147); mem_wr(gb, DE, A);
  CYC(b_+147, b_+149); E = 0xc9; // Part.angle
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); alu_add(gb, 0x04);
  CYC(b_+152, b_+154); alu_and(gb, 0x08);
  CYC(b_+154, b_+155); alu_rrca(gb);
  CYC(b_+155, b_+156); alu_rrca(gb);
  CYC(b_+156, b_+159); SET_HL(b_+170); // @collisionRadii
  CYC(b_+159, b_+160); detectionHelper_addAToHl_from_rst(gb, b_+160);
  CYC(b_+160, b_+162); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+162, b_+163); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+163, b_+164); mem_wr(gb, DE, A);
  CYC(b_+164, b_+165); E = alu_inc8(gb, E);
  CYC(b_+165, b_+166); A = mem_rd(gb, HL);
  CYC(b_+166, b_+167); mem_wr(gb, DE, A);
  CYC(b_+167, b_+170); goto initSpeed; // jp

subid2:
  CYC(b_+174, b_+175); A = mem_rd(gb, DE);
  CYC(b_+175, b_+176); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+176, b_+178); goto subid2_state0; } // jr z
  CYC(b_+176, b_+178);

  CALL_C(b_+178, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+181);
  if (!(F & FZ)) { CYCT(b_+181, b_+183); goto subid1_state1; } // jr nz
  CYC(b_+181, b_+183);
  CYC(b_+183, b_+185); goto delete; // jr

subid2_state0:
  CYC(b_+185, b_+186); H = D;
  CYC(b_+186, b_+187); L = E;
  CYC(b_+187, b_+188); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+188, b_+190); L = 0xc6; // Part.counter1
  CYC(b_+190, b_+192); mem_wr(gb, HL, 0x04);
  CYC(b_+192, b_+194); L = 0xc3; // Part.var03
  CYC(b_+194, b_+195); A = mem_rd(gb, HL);
  CYC(b_+195, b_+196); A = alu_inc8(gb, A);
  CYC(b_+196, b_+197); alu_add(gb, A);
  CYC(b_+197, b_+198); L = alu_dec8(gb, L);
  CYC(b_+198, b_+200); alu_bit(gb, 0, mem_rd(gb, HL)); // [subid]
  if (!(F & FZ)) { CYCT(b_+200, b_+202); goto L_4984; } // jr nz
  CYC(b_+200, b_+202);
  CYC(b_+202, b_+203); alu_cpl(gb);
  CYC(b_+203, b_+204); A = alu_inc8(gb, A);

L_4984:
  CYC(b_+204, b_+206); L = 0xc9; // Part.angle
  CYC(b_+206, b_+207); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+207, b_+209); alu_and(gb, 0x1f);
  CYC(b_+209, b_+210); mem_wr(gb, HL, A);

initSpeed:
  CYC(b_+210, b_+211); H = D;
  CYC(b_+211, b_+213); L = 0xc9; // Part.angle
  CYC(b_+213, b_+214); C = mem_rd(gb, HL);
  CYC(b_+214, b_+216); B = 0x64; // SPEED_280
  CYC(b_+216, b_+218); A = 0x04;
  CYC(b_+218, b_+221); TAIL(objectSetComponentSpeedByScaledVelocity); // jp
}

void detectionHelper_spawnCollisionHelper_hook(GB *gb) {
  BASE(partCode0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+83, getFreePartSlot_hook, SYM(getFreePartSlot), b_+86);
  if (!(F & FZ)) { CYCT(b_+86, b_+87); return; } // ret nz
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x0e); // PART_DETECTION_HELPER
  CYC(b_+89, b_+90); L = alu_inc8(gb, L);
  CYC(b_+90, b_+91); mem_wr(gb, HL, E);
  CYC(b_+91, b_+92); L = alu_inc8(gb, L);
  CYC(b_+92, b_+93); mem_wr(gb, HL, C);
  CALL_C(b_+93, objectCopyPosition_hook, SYM(objectCopyPosition), b_+96);
  CYC(b_+96, b_+98); L = 0xc9; // Part.angle
  CYC(b_+98, b_+99); mem_wr(gb, HL, B);
  CYC(b_+99, b_+101); L = 0xd6; // Part.relatedObj1
  CYC(b_+101, b_+102); E = L;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  CYC(b_+103, b_+104); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+104, b_+105); E = alu_inc8(gb, E);
  CYC(b_+105, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+107); mem_wr(gb, HL, A);
  CYC(b_+107, b_+108); return; // ret
}
