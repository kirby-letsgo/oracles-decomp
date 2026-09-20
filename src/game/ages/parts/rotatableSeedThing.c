#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode33), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode33), (from), (to), true)

static uint16_t rotatableSeedThing_jump_table(GB *gb) {
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

static void rotatableSeedThing_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_ROTATABLE_SEED_THING
void partCode33_hook(GB *gb) {
  BASE(partCode33);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); alu_and(gb, 0x03);
  CYC(b_+6, b_+8); E = 0xc4; // Part.state
  {
    CYC(b_+8, b_+9); push_effect(gb, b_+9);
    uint16_t target = rotatableSeedThing_jump_table(gb);
    if (target == b_+97) goto subid1;
    if (target == b_+137) goto subid2;
    if (target == b_+227) goto subid3;
    goto subid0;
  }

subid0:
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+19, b_+21); goto subid0_state0; } // jr z
  CYC(b_+19, b_+21);

func_64f2:
  CALL_C(b_+21, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+24);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } // ret nz
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); E = 0xf0; // Part.var30
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+32); goto func_657e; // jp

subid0_state0:
  CYC(b_+32, b_+33); C = B;
  CYC(b_+33, b_+35); C = alu_rlc(gb, C);
  CYC(b_+35, b_+37); A = 0x01;
  if (!(F & FC)) { CYCT(b_+37, b_+39); goto L_6506; } // jr nc
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); A = 0xff;

L_6506:
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = 0xf1; // Part.var31
  CYC(b_+44, b_+45); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+45, b_+47); C = alu_rlc(gb, C);
  CYC(b_+47, b_+49); A = 0x3c;
  if (!(F & FC)) { CYCT(b_+49, b_+51); goto L_6511; } // jr nc
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+52); alu_add(gb, A);

L_6511:
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  CYC(b_+53, b_+55); L = 0xc6; // Part.counter1
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);

func_6515:
  CYC(b_+56, b_+57); A = B;
  CYC(b_+57, b_+58); alu_rrca(gb);
  CYC(b_+58, b_+59); alu_rrca(gb);
  CYC(b_+59, b_+61); alu_and(gb, 0x03);
  CYC(b_+61, b_+63); E = 0xc8;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+67); push_effect(gb, b_+67); goto func_6588; // call $6588 (own burn; local, inlined)

func_6515_afterFunc6588:
  CALL_C(b_+67, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+70);
  CYC(b_+70, b_+72); H = 0xcf;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x0a);
  CALL_C(b_+74, objectSetVisible83_hook, SYM(objectSetVisible83), b_+77);
  CALL_C(b_+77, getFreePartSlot_hook, SYM(getFreePartSlot), b_+80);
  if (!(F & FZ)) {
    RET_TAKEN(b_+80);
    if (gb->pc == b_+116 && gb->sp == sp0_) goto func_6551;
    if (gb->pc == b_+216 && gb->sp == sp0_) goto subid2_state0_afterFunc6515;
    return;
  } // ret nz
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x33); // PART_ROTATABLE_SEED_THING
  CYC(b_+83, b_+84); L = alu_inc8(gb, L);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x03);
  CYC(b_+86, b_+88); L = 0xd6; // Part.relatedObj1
  CYC(b_+88, b_+90); A = 0xc0;
  CYC(b_+90, b_+91); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+91, b_+92); mem_wr(gb, HL, D);
  CYC(b_+92, b_+93); H = D;
  CYC(b_+93, b_+95); L = 0xc4; // Part.state
  CYC(b_+95, b_+96); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+96);
  if (gb->pc == b_+116 && gb->sp == sp0_) goto func_6551;
  if (gb->pc == b_+216 && gb->sp == sp0_) goto subid2_state0_afterFunc6515;
  return; // ret

subid1:
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(b_+98, b_+100); goto subid1_state0; } // jr z
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+101); H = D;
  CYC(b_+101, b_+103); L = 0xc3; // Part.var03
  CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+106); L = 0xd8; // Part.relatedObj2
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+107, b_+108); H = mem_rd(gb, HL);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+110); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+110); return; } // ret z
  CYC(b_+110, b_+111);
  CYC(b_+111, b_+113); goto func_64f2; // jr

subid1_state0:
  CYC(b_+113, b_+116); push_effect(gb, b_+116); goto subid0_state0; // call $64fd (own burn; local, inlined)

func_6551:
  CYC(b_+116, b_+118); E = 0xc2; // Part.subid
  CYC(b_+118, b_+119); A = mem_rd(gb, DE);
  CYC(b_+119, b_+121); alu_bit(gb, 4, A);
  CYC(b_+121, b_+124); SET_HL(wToggleBlocksState);
  if (F & FZ) { CYCT(b_+124, b_+126); goto L_655e; } // jr z
  CYC(b_+124, b_+126);
  CYC(b_+126, b_+129); SET_HL(wActiveTriggers);

L_655e:
  CYC(b_+129, b_+131); E = 0xd8; // Part.relatedObj2
  CYC(b_+131, b_+132); A = L;
  CYC(b_+132, b_+133); mem_wr(gb, DE, A);
  CYC(b_+133, b_+134); E = alu_inc8(gb, E);
  CYC(b_+134, b_+135); A = H;
  CYC(b_+135, b_+136); mem_wr(gb, DE, A);
  RET(b_+136);
  if (gb->pc == b_+219 && gb->sp == sp0_) goto subid2_state0_afterFunc6551;
  return; // ret

subid2:
  CYC(b_+137, b_+138); A = mem_rd(gb, DE);
  CYC(b_+138, b_+139); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+139, b_+141); goto subid2_state0; } // jr z
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+142); H = D;
  CYC(b_+142, b_+144); L = 0xf2; // Part.var32
  CYC(b_+144, b_+145); E = L;
  CYC(b_+145, b_+146); B = mem_rd(gb, HL);
  CYC(b_+146, b_+148); L = 0xc3; // Part.var03
  CYC(b_+148, b_+149); C = mem_rd(gb, HL);
  CYC(b_+149, b_+151); L = 0xd8; // Part.relatedObj2
  CYC(b_+151, b_+152); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+152, b_+153); H = mem_rd(gb, HL);
  CYC(b_+153, b_+154); L = A;
  CYC(b_+154, b_+155); A = mem_rd(gb, HL);
  CYC(b_+155, b_+156); alu_and(gb, C);
  CYC(b_+156, b_+157); C = A;
  CYC(b_+157, b_+158); alu_xor(gb, B);
  if (F & FZ) { RET_TAKEN(b_+158); return; } // ret z
  CYC(b_+158, b_+159);
  CYC(b_+159, b_+160); A = C;
  CYC(b_+160, b_+161); mem_wr(gb, DE, A);

func_657e:
  CYC(b_+161, b_+162); H = D;
  CYC(b_+162, b_+164); L = 0xf1; // Part.var31
  CYC(b_+164, b_+166); E = 0xc8;
  CYC(b_+166, b_+167); A = mem_rd(gb, DE);
  CYC(b_+167, b_+168); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+168, b_+170); alu_and(gb, 0x03);
  CYC(b_+170, b_+171); mem_wr(gb, DE, A);

func_6588:
  CYC(b_+171, b_+172); B = A;
  CYC(b_+172, b_+175); SET_HL(b_+187); // table_6598
  CYC(b_+175, b_+176); rotatableSeedThing_addDoubleIndexToHl_from_rst(gb, b_+176);
  CYC(b_+176, b_+178); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+178, b_+179); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+179, b_+180); mem_wr(gb, DE, A);
  CYC(b_+180, b_+181); E = alu_inc8(gb, E);
  CYC(b_+181, b_+182); A = mem_rd(gb, HL);
  CYC(b_+182, b_+183); mem_wr(gb, DE, A);
  CYC(b_+183, b_+184); A = B;
  CYC(b_+184, b_+187); partSetAnimation_hook(gb);
  if (gb->pc == b_+67 && gb->sp == sp0_) goto func_6515_afterFunc6588;
  return; // jp

subid2_state0:
  CYC(b_+195, b_+196); C = B;
  CYC(b_+196, b_+198); C = alu_rlc(gb, C);
  CYC(b_+198, b_+200); A = 0x01;
  if (!(F & FC)) { CYCT(b_+200, b_+202); goto L_65a9; } // jr nc
  CYC(b_+200, b_+202);
  CYC(b_+202, b_+204); A = 0xff;

L_65a9:
  CYC(b_+204, b_+206); C = alu_rlc(gb, C);
  if (!(F & FC)) { CYCT(b_+206, b_+208); goto L_65ae; } // jr nc
  CYC(b_+206, b_+208);
  CYC(b_+208, b_+209); alu_add(gb, A);

L_65ae:
  CYC(b_+209, b_+210); H = D;
  CYC(b_+210, b_+212); L = 0xf1; // Part.var31
  CYC(b_+212, b_+213); mem_wr(gb, HL, A);
  CYC(b_+213, b_+216); push_effect(gb, b_+216); goto func_6515; // call $6515 (own burn; local, inlined)

subid2_state0_afterFunc6515:
  CYC(b_+216, b_+219); push_effect(gb, b_+219); goto func_6551; // call $6551 (own burn; local, inlined)

subid2_state0_afterFunc6551:
  CYC(b_+219, b_+221); E = 0xc3; // Part.var03
  CYC(b_+221, b_+222); A = mem_rd(gb, DE);
  CYC(b_+222, b_+223); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+223, b_+225); E = 0xf2; // Part.var32
  CYC(b_+225, b_+226); mem_wr(gb, DE, A);
  RET(b_+226); return; // ret

subid3:
  CYC(b_+227, b_+228); A = mem_rd(gb, DE);
  CYC(b_+228, b_+229); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+229, b_+231); func_65d5_hook(gb); return; } // jr z
  CYC(b_+229, b_+231);
  CYC(b_+231, b_+233); A = 0x21;
  CALL_C(b_+233, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+236);
  CYC(b_+236, b_+237); E = L;
  CYC(b_+237, b_+238); A = mem_rd(gb, HL);
  CYC(b_+238, b_+239); mem_wr(gb, DE, A);
  CYC(b_+239, b_+241); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+241, b_+242); E = L;
  CYC(b_+242, b_+243); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+243, b_+244); mem_wr(gb, DE, A);
  CYC(b_+244, b_+245); E = alu_inc8(gb, E);
  CYC(b_+245, b_+246); A = mem_rd(gb, HL);
  CYC(b_+246, b_+247); mem_wr(gb, DE, A);
  RET(b_+247); return; // ret
}

void func_65d5_hook(GB *gb) {
  BASE(func_65d5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+8); SET_BC((SYM(data_0bfd) + 3));
  CALL_C(b_+8, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0xc4; // Part.state
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = 0xcf; // Part.zh
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0xf2);
  RET(b_+19); return; // ret
}
