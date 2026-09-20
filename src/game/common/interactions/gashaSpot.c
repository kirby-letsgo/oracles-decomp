#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodeb6_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void interactionCodeb6_addAToHl(GB *gb, uint16_t return_address) {
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

static void interactionCodeb6_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_GASHA_SPOT
// ==================================================================================================
void interactionCodeb6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x43f5, 0x43f7); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x43f7, 0x43f8); A = mem_rd(gb, DE);
  CYC(0x43f8, 0x43f9); push_effect(gb, 0x43f9);
  switch (interactionCodeb6_jump_table(gb)) {
    case 0x4409: goto state0;
    case 0x447b: goto state1;
    case 0x4493: goto state2;
    case 0x44c4: goto state3;
    case 0x44ef: goto state4;
    case 0x4510: goto state5;
    case 0x45c8: goto state6;
    case 0x4663: goto state7;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4409, 0x440b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x440b, 0x440c); A = mem_rd(gb, DE);
  CYC(0x440c, 0x440d); E = alu_inc8(gb, E);
  CYC(0x440d, 0x440e); mem_wr(gb, DE, A);
  CYC(0x440e, 0x4411); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(0x4411, checkFlag_hook, 0x0205, 0x4414);
  if (!(F & FZ)) { CYCT(0x4414, 0x4416); goto seedPlanted; } // jr nz
  CYC(0x4414, 0x4416);
  CYC(0x4416, 0x4418); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x4418, 0x4419); A = mem_rd(gb, DE);
  CYC(0x4419, 0x441a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x441a, 0x441c); goto l_442b; } // jr nz
  CYC(0x441a, 0x441c);
  CYC(0x441c, 0x441e); A = 0x28; // DISCOVERY_RING
  CALL_C(0x441e, cpActiveRing_hook, 0x23b0, 0x4421);
  if (!(F & FZ)) { CYCT(0x4421, 0x4423); goto l_442b; } // jr nz
  CYC(0x4421, 0x4423);
  CYC(0x4423, 0x4425); A = 0xa2; // SND_COMPASS
  CYC(0x4425, 0x4427); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x4427, 0x4428); mem_wr(gb, DE, A);
  CALL_C(0x4428, playSound_b00_hook, 0x0c98, 0x442b);

l_442b:
  CALL_C(0x442b, objectGetTileAtPosition_hook, 0x1444, 0x442e);
  CYC(0x442e, 0x4430); alu_cp(gb, 0xd2); // TILEINDEX_SOFT_SOIL
  if (!(F & FZ)) { CYCT(0x4430, 0x4431); ret_effect(gb); return; } // ret nz
  CYC(0x4430, 0x4431);

  // @unearthed
  CALL_C(0x4431, interactionIncState_hook, 0x23e0, 0x4434);
  CYC(0x4434, 0x4436); A = 0x0a;
  CALL_C(0x4436, objectSetCollideRadius_hook, 0x24a1, 0x4439);
  CYC(0x4439, 0x443b); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x443b, 0x443e); objectAddToAButtonSensitiveObjectList_hook(gb); return; // jp

seedPlanted:
  CYC(0x443e, 0x443f); A = mem_rd(gb, DE);
  CYC(0x443f, 0x4442); SET_HL(wGashaSpotKillCounters);
  CYC(0x4442, 0x4443); interactionCodeb6_addAToHl(gb, 0x4443);
  CYC(0x4443, 0x4444); A = mem_rd(gb, HL);
  CYC(0x4444, 0x4446); alu_cp(gb, 40);
  if (F & FC) { CYCT(0x4446, 0x4448); goto delete_; } // jr c
  CYC(0x4446, 0x4448);

  // @killedEnoughEnemies
  CALL_C(0x4448, getFreePartSlot_hook, 0x3e8e, 0x444b);
  if (!(F & FZ)) { CYCT(0x444b, 0x444c); ret_effect(gb); return; } // ret nz
  CYC(0x444b, 0x444c);
  CYC(0x444c, 0x444e); mem_wr(gb, HL, 0x17); // PART_GASHA_TREE
  CYC(0x444e, 0x444f); L = alu_inc8(gb, L);
  CYC(0x444f, 0x4451); mem_wr(gb, HL, 0x01);
  CYC(0x4451, 0x4453); L = 0xd6; // Part.relatedObj1
  CYC(0x4453, 0x4455); A = INTERACTION_BASE; // Interaction.start
  CYC(0x4455, 0x4456); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4456, 0x4457); mem_wr(gb, HL, D);
  CYC(0x4457, 0x4458); H = D;
  CYC(0x4458, 0x445a); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x445a, 0x445c); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x445c, 0x445d); A = mem_rd(gb, DE);
  CYC(0x445d, 0x445e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x445e, 0x4460); alu_add(gb, 0xf8);
  CYC(0x4460, 0x4461); mem_wr(gb, DE, A);
  CYC(0x4461, 0x4463); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4463, 0x4464); A = mem_rd(gb, DE);
  CYC(0x4464, 0x4465); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4465, 0x4467); alu_add(gb, 0x08);
  CYC(0x4467, 0x4468); mem_wr(gb, DE, A);
  CYC(0x4468, 0x446a); A = 0x04;
  CALL_C(0x446a, objectSetCollideRadius_hook, 0x24a1, 0x446d);
  CYC(0x446d, 0x446f); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x446f, 0x4471); mem_wr(gb, HL, 0x03);
  CYC(0x4471, 0x4473); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4473, 0x4475); mem_wr(gb, HL, 0x0a);
  CALL_C(0x4475, interactionInitGraphics_hook, 0x15fb, 0x4478);
  CYC(0x4478, 0x447b); objectSetVisible83_hook(gb); return; // jp

state1:
  CYC(0x447b, 0x447d); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x447d, 0x447e); A = mem_rd(gb, DE);
  CYC(0x447e, 0x447f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x447f, 0x4480); ret_effect(gb); return; } // ret z
  CYC(0x447f, 0x4480);

  // @pressedAButton
  CYC(0x4480, 0x4481); alu_xor(gb, A);
  CYC(0x4481, 0x4482); mem_wr(gb, DE, A);
  CYC(0x4482, 0x4485); SET_BC(0x3509); // TX_3509
  CYC(0x4485, 0x4488); A = mem_rd(gb, wNumGashaSeeds);
  CYC(0x4488, 0x4489); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4489, 0x448b); goto l_4490; } // jr z
  CYC(0x4489, 0x448b);
  CALL_C(0x448b, interactionIncState_hook, 0x23e0, 0x448e);
  CYC(0x448e, 0x4490); C = 0x00; // <TX_3500

l_4490:
  CYC(0x4490, 0x4493); showText_hook(gb); return; // jp

state2:
  CYC(0x4493, 0x4496); A = mem_rd(gb, wSelectedTextOption);
  CYC(0x4496, 0x4497); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4497, 0x4499); goto l_449d; } // jr z
  CYC(0x4497, 0x4499);
  CYC(0x4499, 0x449b); A = 0x01;
  CYC(0x449b, 0x449c); mem_wr(gb, DE, A); // [state]
  RET(0x449c); return;

l_449d:
  CALL_C(0x449d, objectGetTileAtPosition_hook, 0x1444, 0x44a0);
  CYC(0x44a0, 0x44a1); C = L;
  CYC(0x44a1, 0x44a3); A = 0xf5; // TILEINDEX_SOFT_SOIL_PLANTED
  CALL_C(0x44a3, setTile_hook, 0x3a9c, 0x44a6);
  CYC(0x44a6, 0x44a8); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x44a8, 0x44a9); A = mem_rd(gb, DE);
  CYC(0x44a9, 0x44ac); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(0x44ac, setFlag_hook, 0x020e, 0x44af);
  CYC(0x44af, 0x44b0); A = mem_rd(gb, DE);
  CYC(0x44b0, 0x44b2); L = 0x4f; // wGashaSpotKillCounters low byte
  CYC(0x44b2, 0x44b3); interactionCodeb6_addAToHl(gb, 0x44b3);
  CYC(0x44b3, 0x44b5); mem_wr(gb, HL, 0x00);
  CYC(0x44b5, 0x44b7); L = 0xbe; // wNumGashaSeeds low byte
  CYC(0x44b7, 0x44b8); A = mem_rd(gb, HL);
  CYC(0x44b8, 0x44ba); alu_sub(gb, 0x01);
  CYC(0x44ba, 0x44bb); alu_daa(gb);
  CYC(0x44bb, 0x44bc); mem_wr(gb, HL, A);
  CYC(0x44bc, 0x44be); A = 0x5e; // SND_GETSEED
  CALL_C(0x44be, playSound_b00_hook, 0x0c98, 0x44c1);

delete_:
  CYC(0x44c1, 0x44c4); interactionDelete_hook(gb); return; // jp

state3:
  CYC(0x44c4, 0x44c6); E = INTERACTION_BASE + OBJ_VAR2A;
  CYC(0x44c6, 0x44c7); A = mem_rd(gb, DE);
  CYC(0x44c7, 0x44c9); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x44c9, 0x44ca); ret_effect(gb); return; } // ret nz
  CYC(0x44c9, 0x44ca);
  CYC(0x44ca, 0x44cc); A = 0x80; // DISABLE_ALL_BUT_INTERACTIONS
  CYC(0x44cc, 0x44cf); mem_wr(gb, wDisabledObjects, A);
  CYC(0x44cf, 0x44d2); mem_wr(gb, wMenuDisabled, A);
  CYC(0x44d2, 0x44d3); H = D;
  CYC(0x44d3, 0x44d5); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x44d5, 0x44d7); mem_wr(gb, HL, 0x04);
  CYC(0x44d7, 0x44d9); A = 0x06;
  CALL_C(0x44d9, objectSetCollideRadius_hook, 0x24a1, 0x44dc);
  CYC(0x44dc, 0x44df); SET_BC(0xfec0); // -$140
  CALL_C(0x44df, objectSetSpeedZ_hook, 0x239d, 0x44e2);
  CYC(0x44e2, 0x44e4); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x44e4, 0x44e6); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(0x44e6, objectGetAngleTowardLink_hook, 0x1e9c, 0x44e9);
  CYC(0x44e9, 0x44eb); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x44eb, 0x44ec); mem_wr(gb, DE, A);
  CYC(0x44ec, 0x44ef); objectSetVisible80_hook(gb); return; // jp

state4:
  CYC(0x44ef, 0x44f2); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(0x44f2, 0x44f3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x44f3, 0x44f6); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x44f3, 0x44f6);
  CALL_C(0x44f6, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x44f9);
  if (F & FC) { CYCT(0x44f9, 0x44fb); goto l_4503; } // jr c
  CYC(0x44f9, 0x44fb);
  CALL_C(0x44fb, objectApplySpeed_hook, 0x201d, 0x44fe);
  CYC(0x44fe, 0x4500); C = 0x20;
  CYC(0x4500, 0x4503); objectUpdateSpeedZ_paramC_hook(gb); return; // jp

l_4503:
  CALL_C(0x4503, interactionIncState_hook, 0x23e0, 0x4506);
  CYC(0x4506, 0x4508); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x4508, 0x450a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x450a, 0x450d); SET_BC(0x3501); // TX_3501
  CYC(0x450d, 0x4510); showText_hook(gb); return; // jp

state5:
  CYC(0x4510, 0x4513); SET_HL(wGashaSpotFlags);
  CYC(0x4513, 0x4515); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4515, 0x4517); goto l_451d; } // jr nz
  CYC(0x4515, 0x4517);
  CYC(0x4517, 0x4519); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)
  CYC(0x4519, 0x451b); B = 0x04; // GASHATREASURE_TIER3_RING
  CYCT(0x451b, 0x451d); goto spawnTreasure; // jr

l_451d:
  CYC(0x451d, 0x451f); C = 0x00;
  CYC(0x451f, 0x4522); SET_HL(wGashaMaturity + 1);
  CYC(0x4522, 0x4523); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x4523, 0x4525); A = alu_srl(gb, A);
  if (!(F & FZ)) { CYCT(0x4525, 0x4527); goto l_4533; } // jr nz
  CYC(0x4525, 0x4527);
  CYC(0x4527, 0x4528); A = mem_rd(gb, HL);
  CYC(0x4528, 0x4529); alu_rra(gb);
  CYC(0x4529, 0x452c); SET_HL(0x46fb); // @gashaMaturityValues

l_452c:
  CYC(0x452c, 0x452d); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x452d, 0x452f); goto l_4533; } // jr nc
  CYC(0x452d, 0x452f);
  CYC(0x452f, 0x4530); SET_HL(HL + 1); // inc hl
  CYC(0x4530, 0x4531); C = alu_inc8(gb, C);
  CYCT(0x4531, 0x4533); goto l_452c; // jr

l_4533:
  CYC(0x4533, 0x4535); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4535, 0x4536); A = mem_rd(gb, DE);
  CYC(0x4536, 0x4539); SET_HL(0x4714); // @gashaSpotRanks
  CYC(0x4539, 0x453a); interactionCodeb6_addAToHl(gb, 0x453a);
  CYC(0x453a, 0x453b); A = mem_rd(gb, HL);
  CYC(0x453b, 0x453c); interactionCodeb6_addAToHl(gb, 0x453c);
  CYC(0x453c, 0x453d); A = C;
  CYC(0x453d, 0x453e); alu_add(gb, A);
  CYC(0x453e, 0x453f); C = A;
  CYC(0x453f, 0x4540); alu_add(gb, A);
  CYC(0x4540, 0x4541); alu_add(gb, A);
  CYC(0x4541, 0x4542); alu_add(gb, C);
  CYC(0x4542, 0x4543); interactionCodeb6_addAToHl(gb, 0x4543);
  CALL_C(0x4543, getRandomIndexFromProbabilityDistribution_hook, 0x0464, 0x4546);
  CYC(0x4546, 0x4547); A = B;
  CYC(0x4547, 0x4549); alu_cp(gb, 0x06); // GASHATREASURE_POTION
  if (!(F & FZ)) { CYCT(0x4549, 0x454b); goto notPotion; } // jr nz
  CYC(0x4549, 0x454b);
  CYC(0x454b, 0x454d); A = 0x2f; // TREASURE_POTION
  CALL_C(0x454d, checkTreasureObtained_hook, 0x1748, 0x4550);
  if (!(F & FC)) { CYCT(0x4550, 0x4552); goto decGashaMaturity; } // jr nc
  CYC(0x4550, 0x4552);
  CYC(0x4552, 0x4555); SET_HL(wLinkMaxHealth);
  CYC(0x4555, 0x4556); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x4556, 0x4557); mem_wr(gb, HL, A);
  CYCT(0x4557, 0x4559); goto decGashaMaturity; // jr

notPotion:
  CYC(0x4559, 0x455b); alu_cp(gb, 0x00); // GASHATREASURE_HEART_PIECE
  if (!(F & FZ)) { CYCT(0x455b, 0x455d); goto decGashaMaturity; } // jr nz
  CYC(0x455b, 0x455d);
  CYC(0x455d, 0x4560); SET_HL(wGashaSpotFlags);
  CYC(0x4560, 0x4562); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4562, 0x4564); goto l_4565; } // jr z
  CYC(0x4562, 0x4564);
  CYC(0x4564, 0x4565); B = alu_inc8(gb, B);

l_4565:
  CYC(0x4565, 0x4567); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1))); // set 1,(hl)

decGashaMaturity:
  CYC(0x4567, 0x456a); SET_HL(wGashaMaturity);
  CYC(0x456a, 0x456b); A = mem_rd(gb, HL);
  CYC(0x456b, 0x456d); alu_sub(gb, 0xc8); // 200
  CYC(0x456d, 0x456e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x456e, 0x456f); A = mem_rd(gb, HL);
  CYC(0x456f, 0x4571); alu_sbc(gb, 0x00);
  CYC(0x4571, 0x4572); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(0x4572, 0x4574); goto spawnTreasure; } // jr nc
  CYC(0x4572, 0x4574);
  CYC(0x4574, 0x4575); alu_xor(gb, A);
  CYC(0x4575, 0x4576); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x4576, 0x4577); mem_wr(gb, HL, A);

spawnTreasure:
  CYC(0x4577, 0x4578); A = B;
  CYC(0x4578, 0x457a); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x457a, 0x457b); mem_wr(gb, DE, A);
  CYC(0x457b, 0x457e); SET_HL(0x4700); // @gashaTreasures
  CYC(0x457e, 0x457f); interactionCodeb6_addDoubleIndex(gb, 0x457f);
  CYC(0x457f, 0x4580); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4580, 0x4581); C = mem_rd(gb, HL);
  CYC(0x4581, 0x4583); alu_cp(gb, 0x2d); // TREASURE_RING
  if (!(F & FZ)) { CYCT(0x4583, 0x4585); goto l_4588; } // jr nz
  CYC(0x4583, 0x4585);
  CALL_C(0x4585, getRandomRingOfGivenTier_hook, 0x17e0, 0x4588);

l_4588:
  CYC(0x4588, 0x4589); B = A;
  CALL_C(0x4589, giveTreasure_hook, 0x171c, 0x458c);
  CYC(0x458c, 0x458f); SET_HL(wLinkForceState);
  CYC(0x458f, 0x4591); A = 0x04; // LINK_STATE_04
  CYC(0x4591, 0x4592); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4592, 0x4594); mem_wr(gb, HL, 0x01); // [wcc50]
  CYC(0x4594, 0x4597); SET_HL(w1Link_yh);
  CYC(0x4597, 0x459a); SET_BC(0xf300);
  CALL_C(0x459a, objectTakePositionWithOffset_hook, 0x2277, 0x459d);
  CALL_C(0x459d, interactionIncState_hook, 0x23e0, 0x45a0);
  CYC(0x45a0, 0x45a2); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x45a2, 0x45a4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x45a4, 0x45a6); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x45a6, 0x45a7); A = mem_rd(gb, DE);
  CYC(0x45a7, 0x45a9); alu_cp(gb, 0x00); // GASHATREASURE_HEART_PIECE
  CYC(0x45a9, 0x45ab); A = 0x4c; // SND_GETITEM
  if (!(F & FZ)) { CALL_C_CC(0x45ab, playSound_b00_hook, 0x0c98, 0x45ae); } else { CYC(0x45ab, 0x45ae); } // call nz
  CALL_C(0x45ae, interactionInitGraphics_hook, 0x15fb, 0x45b1);
  CYC(0x45b1, 0x45b3); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x45b3, 0x45b4); A = mem_rd(gb, DE);
  CYC(0x45b4, 0x45b7); SET_HL(0x45be); // @lowTextIndices
  CYC(0x45b7, 0x45b8); interactionCodeb6_addAToHl(gb, 0x45b8);
  CYC(0x45b8, 0x45b9); C = mem_rd(gb, HL);
  CYC(0x45b9, 0x45bb); B = 0x35; // >TX_3500
  CYC(0x45bb, 0x45be); showText_hook(gb); return; // jp

state6:
  CYC(0x45c8, 0x45cb); SET_HL(wNumRupees);
  CYC(0x45cb, 0x45ce); A = mem_rd(gb, wDisplayedRupees);
  CYC(0x45ce, 0x45cf); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x45cf, 0x45d0); ret_effect(gb); return; } // ret nz
  CYC(0x45cf, 0x45d0);
  CYC(0x45d0, 0x45d1); L = alu_inc8(gb, L);
  CYC(0x45d1, 0x45d4); A = mem_rd(gb, wDisplayedRupees + 1);
  CYC(0x45d4, 0x45d5); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x45d5, 0x45d6); ret_effect(gb); return; } // ret nz
  CYC(0x45d5, 0x45d6);
  CYC(0x45d6, 0x45d8); L = 0xaa; // wLinkHealth low byte
  CYC(0x45d8, 0x45db); A = mem_rd(gb, wDisplayedHearts);
  CYC(0x45db, 0x45dc); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x45dc, 0x45dd); ret_effect(gb); return; } // ret nz
  CYC(0x45dc, 0x45dd);
  CYC(0x45dd, 0x45df); A = 0x91; // SND_FAIRYCUTSCENE
  CALL_C(0x45df, playSound_b00_hook, 0x0c98, 0x45e2);
  CYC(0x45e2, 0x45e4); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x45e4, 0x45e5); A = mem_rd(gb, DE);
  CYC(0x45e5, 0x45e8); SET_HL(0x4897); // @gfxHeaderToLoadWhenTreeDisappears
  CYC(0x45e8, 0x45e9); interactionCodeb6_addAToHl(gb, 0x45e9);
  CYC(0x45e9, 0x45ea); A = mem_rd(gb, HL);
  CYC(0x45ea, 0x45eb); push_effect(gb, AF); // push af
  CYC(0x45eb, 0x45ed); alu_sub(gb, 0x39); // GFXH_GASHA_TREE_DISAPPEARED - OBJ_GFXH_04
  CYC(0x45ed, 0x45f0); mem_wr(gb, wLoadedTreeGfxActive, A);
  CYC(0x45f0, 0x45f2); A = 0x3d; // GFXH_GASHA_TREE_DISAPPEARED
  CALL_C(0x45f2, loadGfxHeader_hook, 0x0626, 0x45f5);
  CYC(0x45f5, 0x45f6); SET_AF(pop_effect(gb)); // pop af
  CYC(0x45f6, 0x45f8); alu_cp(gb, 0x3d);
  if (!(F & FZ)) { CALL_C_CC(0x45f8, loadGfxHeader_hook, 0x0626, 0x45fb); } else { CYC(0x45f8, 0x45fb); } // call nz
  CYC(0x45fb, 0x45fd); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(0x45fd, 0x45fe); D = A;
  CYC(0x45fe, 0x45ff); H = D;
  CYC(0x45ff, 0x4601); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x4601, 0x4603); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x4603, 0x4604); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4604, 0x4605); mem_wr(gb, DE, A);
  CYC(0x4605, 0x4607); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4607, 0x4608); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4608, 0x4609); mem_wr(gb, DE, A);
  CYC(0x4609, 0x460b); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x460b, 0x460d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(0x460d, objectGetTileCollisions_hook, 0x14ad, 0x4610);
  CYC(0x4610, 0x4611); alu_xor(gb, A);
  CYC(0x4611, 0x4612); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4612, 0x4613); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x4613, 0x4614); A = L;
  CYC(0x4614, 0x4616); alu_sub(gb, 0x10);
  CYC(0x4616, 0x4617); L = A;
  CYC(0x4617, 0x4618); alu_xor(gb, A);
  CYC(0x4618, 0x4619); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4619, 0x461a); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x461a, 0x461b); H = A;
  CYC(0x461b, 0x461c); E = L;
  CYC(0x461c, 0x461d); A = L;
  CYC(0x461d, 0x461f); alu_and(gb, 0xf0);
  CYC(0x461f, 0x4620); L = A;
  CYC(0x4620, 0x4621); A = E;
  CYC(0x4621, 0x4623); alu_and(gb, 0x0f);
  CYC(0x4623, 0x4625); L = alu_sla(gb, L);
  CYC(0x4625, 0x4627); H = alu_rl(gb, H);
  CYC(0x4627, 0x4628); alu_add(gb, L);
  CYC(0x4628, 0x4629); L = A;
  CYC(0x4629, 0x462b); L = alu_sla(gb, L);
  CYC(0x462b, 0x462d); H = alu_rl(gb, H);
  CYC(0x462d, 0x4630); SET_BC(w3VramTiles);
  CYC(0x4630, 0x4631); alu_add_hl(gb, BC);
  CYC(0x4631, 0x4633); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x4633, 0x4634); A = L;
  CYC(0x4634, 0x4635); mem_wr(gb, DE, A);
  CYC(0x4635, 0x4636); E = alu_inc8(gb, E);
  CYC(0x4636, 0x4637); A = H;
  CYC(0x4637, 0x4638); mem_wr(gb, DE, A);
  CYC(0x4638, 0x463b); SET_BC(0x0400);
  CYC(0x463b, 0x463c); alu_add_hl(gb, BC);
  CYC(0x463c, 0x463e); A = hram_rd(gb, R_SVBK);
  CYC(0x463e, 0x463f); push_effect(gb, AF); // push af
  CYC(0x463f, 0x4641); A = 0x03; // :w3VramAttributes
  CYC(0x4641, 0x4643); hram_wr(gb, R_SVBK, A);
  CYC(0x4643, 0x4645); B = 0x04;

l_4645:
  CYC(0x4645, 0x4647); C = 0x04;
  CYC(0x4647, 0x4648); push_effect(gb, BC); // push bc

l_4648:
  CYC(0x4648, 0x4649); A = mem_rd(gb, HL);
  CYC(0x4649, 0x464b); alu_and(gb, 0xf0);
  CYC(0x464b, 0x464d); alu_or(gb, 0x04);
  CYC(0x464d, 0x464e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x464e, 0x464f); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x464f, 0x4651); goto l_4648; } // jr nz
  CYC(0x464f, 0x4651);
  CYC(0x4651, 0x4654); SET_BC(0x001c);
  CYC(0x4654, 0x4655); alu_add_hl(gb, BC);
  CYC(0x4655, 0x4656); SET_BC(pop_effect(gb)); // pop bc
  CYC(0x4656, 0x4657); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x4657, 0x4659); goto l_4645; } // jr nz
  CYC(0x4657, 0x4659);
  CYC(0x4659, 0x465a); SET_AF(pop_effect(gb)); // pop af
  CYC(0x465a, 0x465c); hram_wr(gb, R_SVBK, A);
  CALL_C(0x465c, interactionIncState_hook, 0x23e0, 0x465f);
  CYC(0x465f, 0x4661); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4661, 0x4663); mem_wr(gb, HL, 0x08);

state7:
  CYC(0x4663, 0x4664); H = D;
  CYC(0x4664, 0x4666); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4666, 0x4667); A = mem_rd(gb, HL);
  CYC(0x4667, 0x4668); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4668, 0x466a); goto counter1Done; } // jr z
  CYC(0x4668, 0x466a);
  CYC(0x466a, 0x466b); A = alu_dec8(gb, A);
  CYC(0x466b, 0x466c); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(0x466c, 0x466d); ret_effect(gb); return; } // ret nz
  CYC(0x466c, 0x466d);

counter1Done:
  CYC(0x466d, 0x466f); A = 0x08;
  CYC(0x466f, 0x4670); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4670, 0x4671); A = mem_rd(gb, HL);
  CYC(0x4671, 0x4672); A = alu_inc8(gb, A);
  CYC(0x4672, 0x4673); mem_wr(gb, HL, A);
  CYC(0x4673, 0x4675); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x4675, 0x4677); goto counter2Done; } // jr nc
  CYC(0x4675, 0x4677);
  CYC(0x4677, 0x467a); SET_HL(0x481d); // @treeDisappearanceFrames - 1
  CYC(0x467a, 0x467b); interactionCodeb6_addAToHl(gb, 0x467b);
  CYC(0x467b, 0x467c); A = mem_rd(gb, HL);
  CYC(0x467c, 0x467d); interactionCodeb6_addAToHl(gb, 0x467d);
  CYC(0x467d, 0x467f); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x467f, 0x4680); A = mem_rd(gb, DE);
  CYC(0x4680, 0x4681); C = A;
  CYC(0x4681, 0x4682); E = alu_inc8(gb, E);
  CYC(0x4682, 0x4683); A = mem_rd(gb, DE);
  CYC(0x4683, 0x4684); D = A;
  CYC(0x4684, 0x4685); E = C;
  CYC(0x4685, 0x4686); push_effect(gb, HL); // push hl
  CYC(0x4686, 0x4687); push_effect(gb, DE); // push de
  CYC(0x4687, 0x4688); SET_HL(pop_effect(gb)); // pop hl
  CYC(0x4688, 0x4689); SET_DE(pop_effect(gb)); // pop de
  CYC(0x4689, 0x468b); A = hram_rd(gb, R_SVBK);
  CYC(0x468b, 0x468c); push_effect(gb, AF); // push af
  CYC(0x468c, 0x468e); A = 0x03; // :w3VramTiles
  CYC(0x468e, 0x4690); hram_wr(gb, R_SVBK, A);
  CYC(0x4690, 0x4692); B = 0x04;

l_4692:
  CYC(0x4692, 0x4694); C = 0x04;
  CYC(0x4694, 0x4695); push_effect(gb, BC); // push bc

l_4695:
  CYC(0x4695, 0x4696); A = mem_rd(gb, DE);
  CYC(0x4696, 0x4697); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4697, 0x4698); SET_DE(DE + 1); // inc de
  CYC(0x4698, 0x4699); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x4699, 0x469b); goto l_4695; } // jr nz
  CYC(0x4699, 0x469b);
  CYC(0x469b, 0x469e); SET_BC(0x001c);
  CYC(0x469e, 0x469f); alu_add_hl(gb, BC);
  CYC(0x469f, 0x46a0); SET_BC(pop_effect(gb)); // pop bc
  CYC(0x46a0, 0x46a1); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x46a1, 0x46a3); goto l_4692; } // jr nz
  CYC(0x46a1, 0x46a3);
  CYC(0x46a3, 0x46a4); SET_AF(pop_effect(gb)); // pop af
  CYC(0x46a4, 0x46a6); hram_wr(gb, R_SVBK, A);
  CYC(0x46a6, 0x46a8); A = 0x29; // UNCMP_GFXH_29
  CALL_C(0x46a8, loadUncompressedGfxHeader_hook, 0x05da, 0x46ab);
  CYC(0x46ab, 0x46ad); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(0x46ad, 0x46ae); D = A;
  CYC(0x46ae, 0x46b0); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x46b0, 0x46b1); A = mem_rd(gb, DE);
  CYC(0x46b1, 0x46b3); alu_add(gb, 0x1f); // UNCMP_GFXH_20 - 1
  CALL_C(0x46b3, loadUncompressedGfxHeader_hook, 0x05da, 0x46b6);
  CALL_C(0x46b6, reloadTileMap_hook, 0x12fc, 0x46b9);
  CYC(0x46b9, 0x46bb); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(0x46bb, 0x46bc); D = A;
  RET(0x46bc); return;

counter2Done:
  CYC(0x46bd, 0x46be); alu_xor(gb, A);
  CYC(0x46be, 0x46c1); mem_wr(gb, wDisabledObjects, A);
  CYC(0x46c1, 0x46c4); mem_wr(gb, wMenuDisabled, A);
  CYC(0x46c4, 0x46c6); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x46c6, 0x46c7); A = mem_rd(gb, DE);
  CYC(0x46c7, 0x46ca); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(0x46ca, unsetFlag_hook, 0x0218, 0x46cd);
  CYC(0x46cd, 0x46cf); A = 0x4e; // TILEINDEX_GASHA_TREE_TL
  CALL_C(0x46cf, findTileInRoom_hook, 0x15cc, 0x46d2);
  if (!(F & FZ)) { CYCT(0x46d2, 0x46d3); ret_effect(gb); return; } // ret nz
  CYC(0x46d2, 0x46d3);
  CYC(0x46d3, 0x46d5); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x46d5, 0x46d6); A = mem_rd(gb, DE);
  CYC(0x46d6, 0x46d9); SET_BC(0x46eb); // @tileReplacements
  CALL_C(0x46d9, addAToBc_hook, 0x006d, 0x46dc);
  CYC(0x46dc, 0x46dd); A = mem_rd(gb, BC);
  CYC(0x46dd, 0x46de); B = A;
  CYC(0x46de, 0x46df); A = B;
  CYC(0x46df, 0x46e0); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x46e0, 0x46e1); mem_wr(gb, HL, A);
  CYC(0x46e1, 0x46e3); A = 0x0f;
  CYC(0x46e3, 0x46e4); alu_add(gb, L);
  CYC(0x46e4, 0x46e5); L = A;
  CYC(0x46e5, 0x46e6); A = B;
  CYC(0x46e6, 0x46e7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x46e7, 0x46e8); mem_wr(gb, HL, A);
  CYC(0x46e8, 0x46eb); interactionDelete_hook(gb); return; // jp
}
