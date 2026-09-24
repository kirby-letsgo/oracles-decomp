#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/holesFloortrap.s.
// PART_HOLES_FLOORTRAP
// Variables:
// var30 - pointer to tile at part's position
// $ccbf - set to 1 when button in hallway to D3 miniboss is pressed

static uint16_t holesFloortrap_jump_table(GB *gb) {
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

static void holesFloortrap_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// partCode0a@spreadHorizontal
static void holesFloortrap_spreadHorizontal(GB *gb) {
  BASE(partCode0a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+257, b_+259); H = 0x01;
  CYC(b_+259, b_+261); B = 0xcf;
  CYC(b_+261, b_+263); E = PART_BASE + OBJ_VAR30;
  CYC(b_+263, b_+264); A = mem_rd(gb, DE);
  CYC(b_+264, b_+265); L = A;
  CYC(b_+265, b_+267); E = PART_BASE + OBJ_YH;
  CYC(b_+267, b_+268); A = mem_rd(gb, DE);
  CYC(b_+268, b_+269); E = A;
  CYC(b_+269, b_+270); alu_sub(gb, H);
  CYC(b_+270, b_+271); C = A;
  CYC(b_+271, b_+272); A = mem_rd(gb, BC);
  CYC(b_+272, b_+273); alu_cp(gb, L);
  if (F & FZ) { CYCT(b_+273, b_+275); goto L_63e8; }
  CYC(b_+273, b_+275);
  CYC(b_+275, b_+276); A = E;
  CYC(b_+276, b_+277); alu_add(gb, H);
  CYC(b_+277, b_+278); C = A;
  CYC(b_+278, b_+279); A = mem_rd(gb, BC);
  CYC(b_+279, b_+280); alu_cp(gb, L);
  if (!(F & FZ)) { RET_TAKEN(b_+280); return; }
  CYC(b_+280, b_+281);
L_63e8:
  CYC(b_+281, b_+282); A = C;
  CYC(b_+282, b_+284); E = PART_BASE + OBJ_YH;
  CYC(b_+284, b_+285); mem_wr(gb, DE, A);
  RET(b_+285); return;
}

// partCode0a@spreadVertical
static void holesFloortrap_spreadVertical(GB *gb) {
  BASE(partCode0a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+253, b_+255); H = 0x10;
  CYC(b_+255, b_+257);
  goto spread;

spread:
  CYC(b_+259, b_+261); B = 0xcf;
  CYC(b_+261, b_+263); E = PART_BASE + OBJ_VAR30;
  CYC(b_+263, b_+264); A = mem_rd(gb, DE);
  CYC(b_+264, b_+265); L = A;
  CYC(b_+265, b_+267); E = PART_BASE + OBJ_YH;
  CYC(b_+267, b_+268); A = mem_rd(gb, DE);
  CYC(b_+268, b_+269); E = A;
  CYC(b_+269, b_+270); alu_sub(gb, H);
  CYC(b_+270, b_+271); C = A;
  CYC(b_+271, b_+272); A = mem_rd(gb, BC);
  CYC(b_+272, b_+273); alu_cp(gb, L);
  if (F & FZ) { CYCT(b_+273, b_+275); goto L_63e8; }
  CYC(b_+273, b_+275);
  CYC(b_+275, b_+276); A = E;
  CYC(b_+276, b_+277); alu_add(gb, H);
  CYC(b_+277, b_+278); C = A;
  CYC(b_+278, b_+279); A = mem_rd(gb, BC);
  CYC(b_+279, b_+280); alu_cp(gb, L);
  if (!(F & FZ)) { RET_TAKEN(b_+280); return; }
  CYC(b_+280, b_+281);
L_63e8:
  CYC(b_+281, b_+282); A = C;
  CYC(b_+282, b_+284); E = PART_BASE + OBJ_YH;
  CYC(b_+284, b_+285); mem_wr(gb, DE, A);
  RET(b_+285); return;
}

// partCode0a@breakFloorsAtInterval
static void holesFloortrap_breakFloorsAtInterval(GB *gb) {
  BASE(partCode0a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+228, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+231);
  if (!(F & FZ)) { RET_TAKEN(b_+231); return; }
  CYC(b_+231, b_+232);
  CYC(b_+232, b_+234); mem_wr(gb, HL, 0x08);
  CYC(b_+234, b_+236); L = PART_BASE + OBJ_VAR30;
  CYC(b_+236, b_+238); A = 0x4d;
  CYC(b_+238, b_+239); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+239, b_+241); A = 0xf3;
  if (F & FZ) { CYCT(b_+241, b_+243); goto L_63c4; }
  CYC(b_+241, b_+243);
  CYC(b_+243, b_+245); A = 0xf4;
L_63c4:
  CYC(b_+245, b_+247); L = PART_BASE + OBJ_YH;
  CYC(b_+247, b_+248); C = mem_rd(gb, HL);
  CALL_C(b_+248, s_breakCrackedFloor, SYM(breakCrackedFloor), b_+251);
  CYC(b_+251, b_+252); alu_xor(gb, A);
  RET(b_+252); return;
}

// partCode0a@init_StoreTileAtPartInVar30
static void holesFloortrap_init_StoreTileAtPartInVar30(GB *gb) {
  BASE(partCode0a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+213, b_+215); A = 0x01;
  CYC(b_+215, b_+216); mem_wr(gb, DE, A);
  CYC(b_+216, b_+217); H = D;
  CYC(b_+217, b_+219); L = PART_BASE + OBJ_YH;
  CYC(b_+219, b_+220); A = mem_rd(gb, HL);
  CYC(b_+220, b_+221); C = A;
  CYC(b_+221, b_+223); B = 0xcf;
  CYC(b_+223, b_+224); A = mem_rd(gb, BC);
  CYC(b_+224, b_+226); L = PART_BASE + OBJ_VAR30;
  CYC(b_+226, b_+227); mem_wr(gb, HL, A);
  RET(b_+227); return;
}

// partCode0a@subid4@setPositionToCrackTile
static void holesFloortrap_subid4_setPositionToCrackTile(GB *gb) {
  BASE(partCode0a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+121, b_+123); E = PART_BASE + OBJ_COUNTER2;
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+127); SET_HL(b_+146 /* @subid4@crackedTileTable */);
  CYC(b_+127, b_+128); holesFloortrap_add_double_index(gb, b_+128);
  CYC(b_+128, b_+129); A = mem_rd(gb, HL);
  CYC(b_+129, b_+130); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+130, b_+133); TAIL(partDelete); }
  CYC(b_+130, b_+133);
  CYC(b_+133, b_+134); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+134, b_+136); E = PART_BASE + OBJ_COUNTER1;
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  CYC(b_+137, b_+138); A = mem_rd(gb, HL);
  CYC(b_+138, b_+140); E = PART_BASE + OBJ_YH;
  CYC(b_+140, b_+141); mem_wr(gb, DE, A);
  CYC(b_+141, b_+142); H = D;
  CYC(b_+142, b_+144); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+144, b_+145); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+145); return;
}

// PART_HOLES_FLOORTRAP
// Variables:
// var30 - pointer to tile at part's position
// $ccbf - set to 1 when button in hallway to D3 miniboss is pressed
void s_partCode0a_hook(GB *gb) {
  BASE(partCode0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (holesFloortrap_jump_table(gb));
    if (jt_ == b_+14) goto subid0;
    if (jt_ == b_+50) goto subidStub;
    if (jt_ == b_+53) goto subid2;
    if (jt_ == b_+85) goto subid4;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+14, b_+16); E = PART_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (holesFloortrap_jump_table(gb));
    if (jt_ == b_+22) goto subid0_state0;
    if (jt_ == b_+30) goto subid0_state1;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CALL_L(b_+22, holesFloortrap_init_StoreTileAtPartInVar30, b_+25);
  CYC(b_+25, b_+27); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x08);
  RET(b_+29); return;
subid0_state1:
  CYC(b_+30, b_+33); A = mem_rd(gb, wEyePuzzleCorrectDirection);
  CYC(b_+33, b_+34); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CALL_L(b_+35, holesFloortrap_breakFloorsAtInterval, b_+38);
  if (!(F & FZ)) { RET_TAKEN(b_+38); return; }
  CYC(b_+38, b_+39);
  CALL_L(b_+39, holesFloortrap_spreadVertical, b_+42);
  if (F & FZ) { RET_TAKEN(b_+42); return; }
  CYC(b_+42, b_+43);
  CALL_L(b_+43, holesFloortrap_spreadHorizontal, b_+46);
  if (F & FZ) { RET_TAKEN(b_+46); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+50);
  TAIL(partDelete);
subidStub:
  CYC(b_+50, b_+53);
  TAIL(partDelete);
subid2:
  CYC(b_+53, b_+55); E = PART_BASE + OBJ_STATE;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); push_effect(gb, b_+57);
  do { uint16_t jt_ = (holesFloortrap_jump_table(gb));
    if (jt_ == b_+61) goto subid3_state0;
    if (jt_ == b_+69) goto subid3_state1;
    HANDOFF(HL);
  } while (0);
subid3_state0:
  CALL_L(b_+61, holesFloortrap_init_StoreTileAtPartInVar30, b_+64);
  CYC(b_+64, b_+66); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x20);
  RET(b_+68); return;
subid3_state1:
  CYC(b_+69, b_+72); A = mem_rd(gb, wEyePuzzleCorrectDirection);
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+73); return; }
  CYC(b_+73, b_+74);
  CALL_L(b_+74, holesFloortrap_breakFloorsAtInterval, b_+77);
  if (!(F & FZ)) { RET_TAKEN(b_+77); return; }
  CYC(b_+77, b_+78);
  CALL_C(b_+78, s_seasonsFunc_10_63ed_hook, SYM(seasonsFunc_10_63ed), b_+81);
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; }
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+85);
  TAIL(partDelete);
subid4:
  CYC(b_+85, b_+86); H = D;
  CYC(b_+86, b_+88); L = PART_BASE + OBJ_STATE;
  CYC(b_+88, b_+89); A = mem_rd(gb, HL);
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+90, b_+92); goto L_6337; }
  CYC(b_+90, b_+92);
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x01);
  CYC(b_+94, b_+96); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x08);
  CYC(b_+98, b_+99); L = alu_inc8(gb, L);
  CYC(b_+99, b_+101); mem_wr(gb, HL, 0x00);
  CALL_L(b_+101, holesFloortrap_subid4_setPositionToCrackTile, b_+104);
L_6337:
  CYC(b_+104, b_+106); A = 0x3c;
  CALL_C(b_+106, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+109);
  CALL_C(b_+109, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+112);
  if (!(F & FZ)) { RET_TAKEN(b_+112); return; }
  CYC(b_+112, b_+113);
  CYC(b_+113, b_+115); L = PART_BASE + OBJ_YH;
  CYC(b_+115, b_+116); C = mem_rd(gb, HL);
  CYC(b_+116, b_+118); A = 0xf4;
  CALL_C(b_+118, s_breakCrackedFloor, SYM(breakCrackedFloor), b_+121);
  CYC(b_+121, b_+123); E = PART_BASE + OBJ_COUNTER2;
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+127); SET_HL(b_+146 /* @subid4@crackedTileTable */);
  CYC(b_+127, b_+128); holesFloortrap_add_double_index(gb, b_+128);
  CYC(b_+128, b_+129); A = mem_rd(gb, HL);
  CYC(b_+129, b_+130); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+130, b_+133); TAIL(partDelete); }
  CYC(b_+130, b_+133);
  CYC(b_+133, b_+134); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+134, b_+136); E = PART_BASE + OBJ_COUNTER1;
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  CYC(b_+137, b_+138); A = mem_rd(gb, HL);
  CYC(b_+138, b_+140); E = PART_BASE + OBJ_YH;
  CYC(b_+140, b_+141); mem_wr(gb, DE, A);
  CYC(b_+141, b_+142); H = D;
  CYC(b_+142, b_+144); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+144, b_+145); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+145); return;
}

void s_seasonsFunc_10_63ed_hook(GB *gb) {
  BASE(seasonsFunc_10_63ed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); C = 0x10;
  CYC(b_+6, b_+9); SET_HL(wRoomLayout);
L_63f6:
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+11, b_+13); goto L_6414; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = L;
  CYC(b_+14, b_+16); alu_cp(gb, 0xae);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+18); alu_add(gb, C);
  CYC(b_+18, b_+20); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto L_640a; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(b_+24, b_+26); goto L_640a; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); L = A;
  CYC(b_+27, b_+29);
  goto L_63f6;
L_640a:
  CYC(b_+29, b_+30); A = C;
  CYC(b_+30, b_+31); alu_cpl(gb);
  CYC(b_+31, b_+32); A = alu_inc8(gb, A);
  CYC(b_+32, b_+33); C = A;
  CYC(b_+33, b_+34); A = L;
  CYC(b_+34, b_+35); alu_add(gb, C);
  CYC(b_+35, b_+36); A = alu_inc8(gb, A);
  CYC(b_+36, b_+37); L = A;
  CYC(b_+37, b_+39);
  goto L_63f6;
L_6414:
  CYC(b_+39, b_+40); A = L;
  CYC(b_+40, b_+42); E = PART_BASE + OBJ_YH;
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+44); alu_or(gb, D);
  RET(b_+44); return;
}

