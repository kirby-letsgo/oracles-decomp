#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t seedOnTree_jump_table(GB *gb) {
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

static void seedOnTree_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void seedOnTree_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode10_hook(GB *gb);
void seedOnTree_giveSeed_hook(GB *gb);

void partCode10_hook(GB *gb) {
  BASE(partCode10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_cp(gb, 0x02); // PARTSTATUS_DEAD
  if (F & FZ) { CYCT(b_+4, b_+7); goto dead; } // jp z
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); E = 0xc4; // Part.state
  CYC(b_+9, b_+11); A = 0x02;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);

normalStatus:
  CYC(b_+12, b_+14); E = 0xc4; // Part.state
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  {
    CYC(b_+15, b_+16); push_effect(gb, b_+16);
    uint16_t target = seedOnTree_jump_table(gb);
    if (target == b_+26) goto state0;
    if (target == b_+65) goto state1;
    if (target == b_+66) goto state2;
    if (target == b_+67) goto state3;
    goto state4;
  }

state0:
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = 0xc2; // Part.subid
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+35); SET_HL(b_+55); // @oamData
  CYC(b_+35, b_+36); seedOnTree_addDoubleIndexToHl_from_rst(gb, b_+36);
  CYC(b_+36, b_+38); E = 0xdd; // Part.oamTileIndexBase
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+40, b_+41); mem_wr(gb, DE, A); // [oamTileIndexBase]
  CYC(b_+41, b_+42); SET_HL(HL + 1);
  CYC(b_+42, b_+43); E = alu_dec8(gb, E);
  CYC(b_+43, b_+44); A = mem_rd(gb, HL);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A); // [oamFlags]
  CYC(b_+45, b_+46); E = alu_dec8(gb, E);
  CYC(b_+46, b_+47); mem_wr(gb, DE, A); // [oamFlagsBackup]
  CYC(b_+47, b_+49); A = 0x01;
  CALL_C(b_+49, partSetAnimation_hook, SYM(partSetAnimation), b_+52);
  CYC(b_+52, b_+55); objectSetVisiblec3_hook(gb); return; // jp

state1:
  RET(b_+65); return; // ret

state2:
  RET(b_+66); return; // ret

state3:
  CALL_C(b_+67, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+70);
  if (F & FC) { CYCT(b_+70, b_+72); goto giveToLink; } // jr c
  CYC(b_+70, b_+72);
  CALL_C(b_+72, objectApplySpeed_hook, SYM(objectApplySpeed), b_+75);
  CYC(b_+75, b_+77); C = 0x20;
  CALL_C(b_+77, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+80);
  if (!(F & FC)) { RET_TAKEN(b_+80); return; } // ret nc
  CYC(b_+80, b_+81);

giveToLink:
  CYC(b_+81, b_+82); H = D;
  CYC(b_+82, b_+84); L = 0xc4; // Part.state
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x04);
  CYC(b_+86, b_+87); L = alu_inc8(gb, L);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x00);
  RET(b_+89); return; // ret

state4:
  CYC(b_+90, b_+92); E = 0xc5; // Part.substate
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  {
    CYC(b_+93, b_+94); push_effect(gb, b_+94);
    uint16_t target = seedOnTree_jump_table(gb);
    if (target == b_+98) goto substate0;
    goto substate1;
  }

substate0:
  CYC(b_+98, b_+100); E = 0xc2; // Part.subid
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+102); L = A;
  CYC(b_+102, b_+104); alu_add(gb, 0x20); // TREASURE_EMBER_SEEDS
  CALL_C(b_+104, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+107);
  if (F & FC) { CYCT(b_+107, b_+109); goto giveSeedAndSomething; } // jr c
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+111); E = 0xc5; // Part.substate
  CYC(b_+111, b_+113); A = 0x01;
  CYC(b_+113, b_+114); mem_wr(gb, DE, A);
  CYC(b_+114, b_+115); A = L; // [subid]
  CYC(b_+115, b_+118); SET_HL(b_+129); // @textIndices
  CYC(b_+118, b_+119); seedOnTree_addAToHl_from_rst(gb, b_+119);
  CYC(b_+119, b_+120); C = mem_rd(gb, HL);
  CYC(b_+120, b_+122); B = 0x00; // >TX_0000
  CALL_C(b_+122, showText_hook, SYM(showText), b_+125);
  CYC(b_+125, b_+127); C = 0x06;
  CYC(b_+127, b_+129); seedOnTree_giveSeed_hook(gb); return; // jr

giveSeedAndSomething:
  CYC(b_+142, b_+144); C = 0x06;
  CYC(b_+144, b_+147); push_effect(gb, b_+147); seedOnTree_giveSeed_hook(gb);

relatedObj2Something:
  CYC(b_+147, b_+149); A = 0x00; // Object.enabled
  CALL_C(b_+149, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+152);
  CYC(b_+152, b_+153); A = mem_rd(gb, HL);
  CYC(b_+153, b_+154); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+154, b_+156); goto delete; } // jr z
  CYC(b_+154, b_+156);
  CYC(b_+156, b_+157); A = L;
  CYC(b_+157, b_+159); alu_add(gb, 0x03); // Object.var03 - Object.enabled
  CYC(b_+159, b_+160); L = A;
  CYC(b_+160, b_+162); mem_wr(gb, HL, 0x01);

delete:
  CYC(b_+162, b_+165); partDelete_hook(gb); return; // jp

substate1:
  CALL_C(b_+165, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+168);
  CYC(b_+168, b_+170); goto relatedObj2Something; // jr

dead:
  CYC(b_+170, b_+171); H = D;
  CYC(b_+171, b_+173); L = 0xe4; // Part.collisionType
  CYC(b_+173, b_+175); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+175, b_+178); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+178, b_+179); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+179); return; } // ret nz
  CYC(b_+179, b_+180);
  CYC(b_+180, b_+182); A = 0x19; // TREASURE_SEED_SATCHEL
  CALL_C(b_+182, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+185);
  if (F & FC) { CYCT(b_+185, b_+187); goto knockOffTree; } // jr c
  CYC(b_+185, b_+187);
  CYC(b_+187, b_+188); A = D;
  CYC(b_+188, b_+191); mem_wr(gb, wRoomLayoutEnd, A);
  CYC(b_+191, b_+194); SET_BC(0x0035); // TX_0035
  CYC(b_+194, b_+197); showText_hook(gb); return; // jp

knockOffTree:
  CYC(b_+197, b_+200); SET_BC(0xfec0); // -0x140
  CALL_C(b_+200, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+203);
  CYC(b_+203, b_+205); L = 0xe9; // Part.health
  CYC(b_+205, b_+207); A = 0x03;
  CYC(b_+207, b_+208); mem_wr(gb, HL, A);
  CYC(b_+208, b_+210); L = 0xc4; // Part.state
  CYC(b_+210, b_+211); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+211, b_+213); mem_wr(gb, HL, 0x00); // [substate]
  CYC(b_+213, b_+214); L = alu_inc8(gb, L);
  CYC(b_+214, b_+216); mem_wr(gb, HL, 0x02); // [counter1]
  CYC(b_+216, b_+218); L = 0xd0; // Part.speed
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(b_+220, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+223);
  CYC(b_+223, b_+225); E = 0xc9; // Part.angle
  CYC(b_+225, b_+226); mem_wr(gb, DE, A);
  RET(b_+226); return; // ret
}

void seedOnTree_giveSeed_hook(GB *gb) {
  BASE(partCode10);
  CYC(b_+134, b_+136); E = 0xc2; // Part.subid
  CYC(b_+136, b_+137); A = mem_rd(gb, DE);
  CYC(b_+137, b_+139); alu_add(gb, 0x20); // TREASURE_EMBER_SEEDS
  CYC(b_+139, b_+142); giveTreasure_hook(gb); return; // jp
}
