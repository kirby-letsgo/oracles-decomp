#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode51), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode51), (from), (to), true)

static uint16_t partCode51_jump_table(GB *gb) {
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

static void partCode51_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void partCode51_hook(GB *gb) {
  BASE(partCode51);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x0e);
  if (F & FZ) { CYCT(b_+8, b_+11); partDelete_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = 0xc2; // Part.subid
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); E = 0xc4; // Part.state
  {
    CYC(b_+16, b_+17); push_effect(gb, b_+17);
    uint16_t target = partCode51_jump_table(gb);
    if (target == b_+23) goto subid0;
    if (target == b_+123) goto subid1;
    goto subid2;
  }

subid0:
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto L_5b96; } // jr nz
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+29); L = E;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+30, b_+32); L = 0xc6; // Part.counter1
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x40);
  CYC(b_+34, b_+36); L = 0xe8; // Part.damage
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0xf0);
  CYC(b_+38, b_+40); L = 0xda; // Part.visible
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x02);
  CYC(b_+42, b_+44); A = 0x5c; // SND_ENERGYTHING
  CALL_C(b_+44, playSound_b00_hook, SYM(playSound_b00), b_+47);

L_5b96:
  CALL_C(b_+47, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+50);
  if (F & FZ) { CYCT(b_+50, b_+53); partDelete_hook(gb); return; } // jp z
  CYC(b_+50, b_+53);
  CYC(b_+53, b_+55); goto L_5ba9; // jr

subid2:
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+57, b_+59); goto L_5bb2; } // jr z
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); E = 0xe1; // Part.animParameter
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); alu_rlca(gb);
  if (F & FC) { CYCT(b_+63, b_+66); partDelete_hook(gb); return; } // jp c
  CYC(b_+63, b_+66);

L_5ba9:
  CYC(b_+66, b_+68); E = 0xda; // Part.visible
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+71); alu_xor(gb, 0x80);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+75); partAnimate_hook(gb); return; // jp

L_5bb2:
  CYC(b_+75, b_+76); H = D;
  CYC(b_+76, b_+77); L = E;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+78, b_+80); L = 0xe4; // Part.collisionType
  CYC(b_+80, b_+82); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+82, b_+84); L = 0xc9; // Part.angle
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+87); B = 0x01;
  CYC(b_+87, b_+89); alu_cp(gb, 0x0c);
  if (F & FC) { CYCT(b_+89, b_+91); goto L_5bc8; } // jr c
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+92); B = alu_inc8(gb, B);
  CYC(b_+92, b_+94); alu_cp(gb, 0x19);
  if (F & FC) { CYCT(b_+94, b_+96); goto L_5bc8; } // jr c
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+97); B = alu_inc8(gb, B);

L_5bc8:
  CYC(b_+97, b_+98); A = B;
  CYC(b_+98, b_+99); A = alu_dec8(gb, A);
  CYC(b_+99, b_+101); alu_and(gb, 0x01);
  CYC(b_+101, b_+104); SET_HL(b_+119); // table_5bde
  CYC(b_+104, b_+105); partCode51_addDoubleIndexToHl_from_rst(gb, b_+105);
  CYC(b_+105, b_+107); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+107, b_+108); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+110); E = alu_inc8(gb, E);
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CYC(b_+111, b_+112); mem_wr(gb, DE, A);
  CYC(b_+112, b_+113); A = B;
  CALL_C(b_+113, partSetAnimation_hook, SYM(partSetAnimation), b_+116);
  CYC(b_+116, b_+119); objectSetVisible83_hook(gb); return; // jp

subid1:
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  {
    CYC(b_+124, b_+125); push_effect(gb, b_+125);
    uint16_t target = partCode51_jump_table(gb);
    if (target == b_+131) goto state0;
    if (target == b_+152) goto state1;
    goto state2;
  }

state0:
  CYC(b_+131, b_+132); H = D;
  CYC(b_+132, b_+133); L = E;
  CYC(b_+133, b_+134); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+134, b_+136); L = 0xdd; // Part.oamTileIndexBase
  CYC(b_+136, b_+137); A = mem_rd(gb, HL);
  CYC(b_+137, b_+139); alu_add(gb, 0x0e);
  CYC(b_+139, b_+140); mem_wr(gb, HL, A);
  CYC(b_+140, b_+142); L = 0xc6; // Part.counter1
  CYC(b_+142, b_+144); mem_wr(gb, HL, 0x18);
  CYC(b_+144, b_+146); A = 0x04;
  CALL_C(b_+146, partSetAnimation_hook, SYM(partSetAnimation), b_+149);
  CYC(b_+149, b_+152); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(b_+152, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+155);
  if (!(F & FZ)) { CYCT(b_+155, b_+157); goto animate; } // jr nz
  CYC(b_+155, b_+157);
  CYC(b_+157, b_+158); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+158, b_+159); L = E;
  CYC(b_+159, b_+160); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+160, b_+162); L = 0xe4; // Part.collisionType
  CYC(b_+162, b_+164); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+164, b_+166); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+166, b_+168); A = 0x05;
  CYC(b_+168, b_+169); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+169, b_+170); mem_wr(gb, HL, A);
  CYC(b_+170, b_+172); L = 0xcb; // Part.yh
  CYC(b_+172, b_+173); A = mem_rd(gb, HL);
  CYC(b_+173, b_+175); alu_add(gb, 0x08);
  CYC(b_+175, b_+176); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+176, b_+177); L = alu_inc8(gb, L);
  CYC(b_+177, b_+178); A = mem_rd(gb, HL);
  CYC(b_+178, b_+180); alu_sub(gb, 0x10);
  CYC(b_+180, b_+181); mem_wr(gb, HL, A);
  CALL_C(b_+181, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+184);
  CYC(b_+184, b_+186); E = 0xc9; // Part.angle
  CYC(b_+186, b_+187); mem_wr(gb, DE, A);
  CYC(b_+187, b_+188); C = A;
  CYC(b_+188, b_+190); B = 0x50;
  CYC(b_+190, b_+192); A = 0x02;
  CYC(b_+192, b_+195); objectSetComponentSpeedByScaledVelocity_hook(gb); return; // jp

state2:
  CALL_C(b_+195, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+198);
  if (!(F & FC)) { CYCT(b_+198, b_+200); goto L_5c3c; } // jr nc
  CYC(b_+198, b_+200);
  CYC(b_+200, b_+202); B = 0x56; // INTERAC_EXPLOSION
  CALL_C(b_+202, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+205);
  CYC(b_+205, b_+207); A = 0x3c;
  if (F & FZ) { CALL_C_CC(b_+207, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+210); } else { CYC(b_+207, b_+210); } // call z
  CYC(b_+210, b_+213); partDelete_hook(gb); return; // jp

L_5c3c:
  CALL_C(b_+213, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+216);
  CYC(b_+216, b_+217); A = mem_rd(gb, HL);
  CYC(b_+217, b_+219); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+219, b_+221); goto L_5c56; } // jr nz
  CYC(b_+219, b_+221);
  CALL_C(b_+221, getFreePartSlot_hook, SYM(getFreePartSlot), b_+224);
  if (!(F & FZ)) { CYCT(b_+224, b_+226); goto L_5c56; } // jr nz
  CYC(b_+224, b_+226);
  CYC(b_+226, b_+228); mem_wr(gb, HL, 0x51); // PART_51
  CYC(b_+228, b_+229); L = alu_inc8(gb, L);
  CYC(b_+229, b_+231); mem_wr(gb, HL, 0x02);
  CYC(b_+231, b_+233); L = 0xc9; // Part.angle
  CYC(b_+233, b_+234); E = L;
  CYC(b_+234, b_+235); A = mem_rd(gb, DE);
  CYC(b_+235, b_+236); mem_wr(gb, HL, A);
  CALL_C(b_+236, objectCopyPosition_hook, SYM(objectCopyPosition), b_+239);

L_5c56:
  CALL_C(b_+239, objectApplyComponentSpeed_hook, SYM(objectApplyComponentSpeed), b_+242);

animate:
  CYC(b_+242, SYM(partCode52)); partAnimate_hook(gb); return; // jp
}
