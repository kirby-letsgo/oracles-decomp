#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4a04, 0x4a06); goto normalStatus; } // jr z
  CYC(0x4a04, 0x4a06);
  CYC(0x4a06, 0x4a08); alu_cp(gb, 0x02); // PARTSTATUS_DEAD
  if (F & FZ) { CYCT(0x4a08, 0x4a0b); goto dead; } // jp z
  CYC(0x4a08, 0x4a0b);
  CYC(0x4a0b, 0x4a0d); E = 0xc4; // Part.state
  CYC(0x4a0d, 0x4a0f); A = 0x02;
  CYC(0x4a0f, 0x4a10); mem_wr(gb, DE, A);

normalStatus:
  CYC(0x4a10, 0x4a12); E = 0xc4; // Part.state
  CYC(0x4a12, 0x4a13); A = mem_rd(gb, DE);
  {
    CYC(0x4a13, 0x4a14); push_effect(gb, 0x4a14);
    uint16_t target = seedOnTree_jump_table(gb);
    if (target == 0x4a1e) goto state0;
    if (target == 0x4a45) goto state1;
    if (target == 0x4a46) goto state2;
    if (target == 0x4a47) goto state3;
    goto state4;
  }

state0:
  CYC(0x4a1e, 0x4a20); A = 0x01;
  CYC(0x4a20, 0x4a21); mem_wr(gb, DE, A);
  CYC(0x4a21, 0x4a23); E = 0xc2; // Part.subid
  CYC(0x4a23, 0x4a24); A = mem_rd(gb, DE);
  CYC(0x4a24, 0x4a27); SET_HL(0x4a3b); // @oamData
  CYC(0x4a27, 0x4a28); seedOnTree_addDoubleIndexToHl_from_rst(gb, 0x4a28);
  CYC(0x4a28, 0x4a2a); E = 0xdd; // Part.oamTileIndexBase
  CYC(0x4a2a, 0x4a2b); A = mem_rd(gb, DE);
  CYC(0x4a2b, 0x4a2c); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4a2c, 0x4a2d); mem_wr(gb, DE, A); // [oamTileIndexBase]
  CYC(0x4a2d, 0x4a2e); SET_HL(HL + 1);
  CYC(0x4a2e, 0x4a2f); E = alu_dec8(gb, E);
  CYC(0x4a2f, 0x4a30); A = mem_rd(gb, HL);
  CYC(0x4a30, 0x4a31); mem_wr(gb, DE, A); // [oamFlags]
  CYC(0x4a31, 0x4a32); E = alu_dec8(gb, E);
  CYC(0x4a32, 0x4a33); mem_wr(gb, DE, A); // [oamFlagsBackup]
  CYC(0x4a33, 0x4a35); A = 0x01;
  CALL_C(0x4a35, partSetAnimation_hook, 0x2988, 0x4a38);
  CYC(0x4a38, 0x4a3b); objectSetVisiblec3_hook(gb); return; // jp

state1:
  RET(0x4a45); return; // ret

state2:
  RET(0x4a46); return; // ret

state3:
  CALL_C(0x4a47, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x4a4a);
  if (F & FC) { CYCT(0x4a4a, 0x4a4c); goto giveToLink; } // jr c
  CYC(0x4a4a, 0x4a4c);
  CALL_C(0x4a4c, objectApplySpeed_hook, 0x201d, 0x4a4f);
  CYC(0x4a4f, 0x4a51); C = 0x20;
  CALL_C(0x4a51, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4a54);
  if (!(F & FC)) { RET_TAKEN(0x4a54); return; } // ret nc
  CYC(0x4a54, 0x4a55);

giveToLink:
  CYC(0x4a55, 0x4a56); H = D;
  CYC(0x4a56, 0x4a58); L = 0xc4; // Part.state
  CYC(0x4a58, 0x4a5a); mem_wr(gb, HL, 0x04);
  CYC(0x4a5a, 0x4a5b); L = alu_inc8(gb, L);
  CYC(0x4a5b, 0x4a5d); mem_wr(gb, HL, 0x00);
  RET(0x4a5d); return; // ret

state4:
  CYC(0x4a5e, 0x4a60); E = 0xc5; // Part.substate
  CYC(0x4a60, 0x4a61); A = mem_rd(gb, DE);
  {
    CYC(0x4a61, 0x4a62); push_effect(gb, 0x4a62);
    uint16_t target = seedOnTree_jump_table(gb);
    if (target == 0x4a66) goto substate0;
    goto substate1;
  }

substate0:
  CYC(0x4a66, 0x4a68); E = 0xc2; // Part.subid
  CYC(0x4a68, 0x4a69); A = mem_rd(gb, DE);
  CYC(0x4a69, 0x4a6a); L = A;
  CYC(0x4a6a, 0x4a6c); alu_add(gb, 0x20); // TREASURE_EMBER_SEEDS
  CALL_C(0x4a6c, checkTreasureObtained_hook, 0x1748, 0x4a6f);
  if (F & FC) { CYCT(0x4a6f, 0x4a71); goto giveSeedAndSomething; } // jr c
  CYC(0x4a6f, 0x4a71);
  CYC(0x4a71, 0x4a73); E = 0xc5; // Part.substate
  CYC(0x4a73, 0x4a75); A = 0x01;
  CYC(0x4a75, 0x4a76); mem_wr(gb, DE, A);
  CYC(0x4a76, 0x4a77); A = L; // [subid]
  CYC(0x4a77, 0x4a7a); SET_HL(0x4a85); // @textIndices
  CYC(0x4a7a, 0x4a7b); seedOnTree_addAToHl_from_rst(gb, 0x4a7b);
  CYC(0x4a7b, 0x4a7c); C = mem_rd(gb, HL);
  CYC(0x4a7c, 0x4a7e); B = 0x00; // >TX_0000
  CALL_C(0x4a7e, showText_hook, 0x1872, 0x4a81);
  CYC(0x4a81, 0x4a83); C = 0x06;
  CYC(0x4a83, 0x4a85); seedOnTree_giveSeed_hook(gb); return; // jr

giveSeedAndSomething:
  CYC(0x4a92, 0x4a94); C = 0x06;
  CYC(0x4a94, 0x4a97); push_effect(gb, 0x4a97); seedOnTree_giveSeed_hook(gb);

relatedObj2Something:
  CYC(0x4a97, 0x4a99); A = 0x00; // Object.enabled
  CALL_C(0x4a99, objectGetRelatedObject2Var_hook, 0x2164, 0x4a9c);
  CYC(0x4a9c, 0x4a9d); A = mem_rd(gb, HL);
  CYC(0x4a9d, 0x4a9e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4a9e, 0x4aa0); goto delete; } // jr z
  CYC(0x4a9e, 0x4aa0);
  CYC(0x4aa0, 0x4aa1); A = L;
  CYC(0x4aa1, 0x4aa3); alu_add(gb, 0x03); // Object.var03 - Object.enabled
  CYC(0x4aa3, 0x4aa4); L = A;
  CYC(0x4aa4, 0x4aa6); mem_wr(gb, HL, 0x01);

delete:
  CYC(0x4aa6, 0x4aa9); partDelete_hook(gb); return; // jp

substate1:
  CALL_C(0x4aa9, retIfTextIsActive_hook, 0x1859, 0x4aac);
  CYC(0x4aac, 0x4aae); goto relatedObj2Something; // jr

dead:
  CYC(0x4aae, 0x4aaf); H = D;
  CYC(0x4aaf, 0x4ab1); L = 0xe4; // Part.collisionType
  CYC(0x4ab1, 0x4ab3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x4ab3, 0x4ab6); A = mem_rd(gb, 0xcfc0);
  CYC(0x4ab6, 0x4ab7); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4ab7); return; } // ret nz
  CYC(0x4ab7, 0x4ab8);
  CYC(0x4ab8, 0x4aba); A = 0x19; // TREASURE_SEED_SATCHEL
  CALL_C(0x4aba, checkTreasureObtained_hook, 0x1748, 0x4abd);
  if (F & FC) { CYCT(0x4abd, 0x4abf); goto knockOffTree; } // jr c
  CYC(0x4abd, 0x4abf);
  CYC(0x4abf, 0x4ac0); A = D;
  CYC(0x4ac0, 0x4ac3); mem_wr(gb, 0xcfc0, A);
  CYC(0x4ac3, 0x4ac6); SET_BC(0x0035); // TX_0035
  CYC(0x4ac6, 0x4ac9); showText_hook(gb); return; // jp

knockOffTree:
  CYC(0x4ac9, 0x4acc); SET_BC(0xfec0); // -0x140
  CALL_C(0x4acc, objectSetSpeedZ_hook, 0x239d, 0x4acf);
  CYC(0x4acf, 0x4ad1); L = 0xe9; // Part.health
  CYC(0x4ad1, 0x4ad3); A = 0x03;
  CYC(0x4ad3, 0x4ad4); mem_wr(gb, HL, A);
  CYC(0x4ad4, 0x4ad6); L = 0xc4; // Part.state
  CYC(0x4ad6, 0x4ad7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4ad7, 0x4ad9); mem_wr(gb, HL, 0x00); // [substate]
  CYC(0x4ad9, 0x4ada); L = alu_inc8(gb, L);
  CYC(0x4ada, 0x4adc); mem_wr(gb, HL, 0x02); // [counter1]
  CYC(0x4adc, 0x4ade); L = 0xd0; // Part.speed
  CYC(0x4ade, 0x4ae0); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(0x4ae0, objectGetAngleTowardLink_hook, 0x1e9c, 0x4ae3);
  CYC(0x4ae3, 0x4ae5); E = 0xc9; // Part.angle
  CYC(0x4ae5, 0x4ae6); mem_wr(gb, DE, A);
  RET(0x4ae6); return; // ret
}

void seedOnTree_giveSeed_hook(GB *gb) {
  CYC(0x4a8a, 0x4a8c); E = 0xc2; // Part.subid
  CYC(0x4a8c, 0x4a8d); A = mem_rd(gb, DE);
  CYC(0x4a8d, 0x4a8f); alu_add(gb, 0x20); // TREASURE_EMBER_SEEDS
  CYC(0x4a8f, 0x4a92); giveTreasure_hook(gb); return; // jp
}
