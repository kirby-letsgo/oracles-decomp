#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/miscBoyNpcs.s, bank $08.

// Counts var37 down to zero; Z when it was already zero.
void s_func_6ac1_hook(GB *gb) {
  BASE(func_6ac1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x77; // Interaction.var37
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+7); return;
}

// Decrements var36.
void s_func_6abc_hook(GB *gb) {
  BASE(func_6abc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x76; // Interaction.var36
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

static uint16_t miscBoyNpcs_d_jump_table(GB *gb) {
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

static void miscBoyNpcs_d_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode3e@func_68e9
static void miscBoyNpcs_d_func_68e9(GB *gb) {
  BASE(interactionCode3e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+193, s_getRandomNumber, SYM(getRandomNumber), b_+196);
  CYC(b_+196, b_+198); alu_and(gb, 0x3f);
  CYC(b_+198, b_+200); alu_add(gb, 0x78);
  CYC(b_+200, b_+201); H = D;
  CYC(b_+201, b_+203); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+203, b_+204); mem_wr(gb, HL, A);
  RET(b_+204); return;
}

// interactionCode3e@continue@var03_01
static void miscBoyNpcs_d_continue_var03_01(GB *gb) {
  BASE(interactionCode3e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+115, b_+116); H = D;
  CYC(b_+116, b_+118); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+118, b_+119); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+119, b_+120); push_effect(gb, AF);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+121, b_+122); mem_wr(gb, HL, A);
  CALL_C(b_+122, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+125);
  CYC(b_+125, b_+126); SET_AF(pop_effect(gb));
  CYC(b_+126, b_+128); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+128, b_+129); mem_wr(gb, DE, A);
  CYC(b_+129, b_+130); E = alu_inc8(gb, E);
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+134); SET_HL(SYM(table_6ac9));
  CYC(b_+134, b_+135); miscBoyNpcs_d_add_double_index(gb, b_+135);
  CYC(b_+135, b_+136); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+136, b_+137); H = mem_rd(gb, HL);
  CYC(b_+137, b_+138); L = A;
  CYC(b_+138, b_+140); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);
  CYC(b_+141, b_+142); miscBoyNpcs_d_add_double_index(gb, b_+142);
  CYC(b_+142, b_+143); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+143, b_+144); H = mem_rd(gb, HL);
  CYC(b_+144, b_+145); L = A;
  CALL_C(b_+145, s_interactionSetScript, SYM(interactionSetScript), b_+148);
  CALL_C(b_+148, s_interactionRunScript, SYM(interactionRunScript), b_+151);
  CYC(b_+151, b_+154);
  TAIL(objectSetVisible82);
}

// getSunkenCityNPCVisibleSubId@main
static void miscBoyNpcs_d_main(GB *gb) {
  BASE(getSunkenCityNPCVisibleSubId);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+14, b_+16); A = 0x28;
  CALL_C(b_+16, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+19);
  CYC(b_+19, b_+21); B = 0x04;
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto xorARet; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x1f;
  CALL_C(b_+25, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+28);
  CYC(b_+28, b_+30); B = 0x03;
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto xorARet; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); A = 0x40;
  CALL_C(b_+34, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+37);
  CYC(b_+37, b_+39); B = 0x00;
  if (!(F & FC)) { CYCT(b_+39, b_+41); goto xorARet; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); C = A;
  CALL_C(b_+42, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+45);
  if (F & FZ) { CYCT(b_+45, b_+47); goto L_5895; }
  CYC(b_+45, b_+47);
L_5895:
  CYC(b_+47, b_+48); A = C;
  CALL_C(b_+48, s_getHighestSetBit, SYM(getHighestSetBit), b_+51);
  CYC(b_+51, b_+53); B = 0x02;
  CYC(b_+53, b_+55); alu_cp(gb, 0x07);
  if (!(F & FC)) { RET_TAKEN(b_+55); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+57); B = alu_dec8(gb, B);
  CYC(b_+57, b_+58); A = C;
  CYC(b_+58, b_+60); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto xorARet; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+63); B = alu_dec8(gb, B);
xorARet:
  CYC(b_+63, b_+64); alu_xor(gb, A);
  RET(b_+64); return;
}

// checkHoronVillageNPCShouldBeSeen_body@main
static void miscBoyNpcs_d_checkHoronVillageNPCShouldBeSeen_body_main(GB *gb) {
  BASE(checkHoronVillageNPCShouldBeSeen_body);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+13); SET_HL(SYM(conditionalHoronNPCLookupTable));
  CYC(b_+13, b_+14); miscBoyNpcs_d_add_double_index(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); H = mem_rd(gb, HL);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+18); push_effect(gb, HL);
  CALL_C(b_+18, s_checkNPCStage_hook, SYM(checkNPCStage), b_+21);
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); miscBoyNpcs_d_add_double_index(gb, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); H = mem_rd(gb, HL);
  CYC(b_+28, b_+29); L = A;
L_57ef:
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+33); A = alu_dec8(gb, A);
  CYC(b_+33, b_+34); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto L_57ef; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); alu_scf(gb);
  RET(b_+37); return;
}

// INTERAC_MISC_BOY_NPCS
void s_interactionCode3e_hook(GB *gb) {
  BASE(interactionCode3e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); goto state1; }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); alu_and(gb, 0x0f);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+20); alu_and(gb, 0xf0);
  CYC(b_+20, b_+22); A = alu_swap(gb, A);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+25); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto nonVar03_03; }
  CYC(b_+25, b_+27);
  CALL_L(b_+27, miscBoyNpcs_d_main, b_+30);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+34, b_+37); TAIL(interactionDelete); }
  CYC(b_+34, b_+37);
  CYC(b_+37, b_+39); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+39, b_+41); goto l_continue; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); A = 0x16;
  CALL_C(b_+43, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+46);
  CYC(b_+46, b_+48); A = 0x6e;
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto L_685c; }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); A = 0x5e;
L_685c:
  CYC(b_+52, b_+55); SET_HL(wActiveRoom);
  CYC(b_+55, b_+56); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+56, b_+59); TAIL(interactionDelete); }
  CYC(b_+56, b_+59);
  CYC(b_+59, b_+61);
  goto l_continue;
nonVar03_03:
  CYC(b_+61, b_+63); alu_add(gb, 0x04);
  CYC(b_+63, b_+64); B = A;
  CALL_L(b_+64, miscBoyNpcs_d_checkHoronVillageNPCShouldBeSeen_body_main, b_+67);
  if (!(F & FC)) { CYCT(b_+67, b_+70); TAIL(interactionDelete); }
  CYC(b_+67, b_+70);
l_continue:
  CYC(b_+70, b_+72); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+72, b_+73); A = B;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CYC(b_+74, b_+75); E = alu_inc8(gb, E);
  CYC(b_+75, b_+76); A = mem_rd(gb, DE);
  CYC(b_+76, b_+77); push_effect(gb, b_+77);
  do { uint16_t jt_ = (miscBoyNpcs_d_jump_table(gb));
    if (jt_ == b_+85) goto continue_var03_00;
    if (jt_ == b_+115) goto continue_var03_01;
    if (jt_ == b_+154) goto continue_var03_02;
    HANDOFF(HL);
  } while (0);
continue_var03_00:
  CALL_L(b_+85, miscBoyNpcs_d_continue_var03_01, b_+88);
  CALL_C(b_+88, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+91);
  if (!(F & FZ)) { CYCT(b_+91, b_+93); goto L_6899; }
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x83);
  CYC(b_+95, b_+98); SET_BC(0x00fd);
  CALL_C(b_+98, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+101);
  CYC(b_+101, b_+103); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+106); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+106, b_+107); mem_wr(gb, HL, A);
  CYC(b_+107, b_+109); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+109, b_+110); A = mem_rd(gb, HL);
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+112, b_+113); mem_wr(gb, HL, A);
L_6899:
  CYC(b_+113, b_+115);
  goto func_68e9;
continue_var03_01:
  CYC(b_+115, b_+116); H = D;
  CYC(b_+116, b_+118); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+118, b_+119); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+119, b_+120); push_effect(gb, AF);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+121, b_+122); mem_wr(gb, HL, A);
  CALL_C(b_+122, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+125);
  CYC(b_+125, b_+126); SET_AF(pop_effect(gb));
  CYC(b_+126, b_+128); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+128, b_+129); mem_wr(gb, DE, A);
  CYC(b_+129, b_+130); E = alu_inc8(gb, E);
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+134); SET_HL(SYM(table_6ac9));
  CYC(b_+134, b_+135); miscBoyNpcs_d_add_double_index(gb, b_+135);
  CYC(b_+135, b_+136); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+136, b_+137); H = mem_rd(gb, HL);
  CYC(b_+137, b_+138); L = A;
  CYC(b_+138, b_+140); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);
  CYC(b_+141, b_+142); miscBoyNpcs_d_add_double_index(gb, b_+142);
  CYC(b_+142, b_+143); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+143, b_+144); H = mem_rd(gb, HL);
  CYC(b_+144, b_+145); L = A;
  CALL_C(b_+145, s_interactionSetScript, SYM(interactionSetScript), b_+148);
  CALL_C(b_+148, s_interactionRunScript, SYM(interactionRunScript), b_+151);
  CYC(b_+151, b_+154);
  TAIL(objectSetVisible82);
continue_var03_02:
  CYC(b_+154, b_+157); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+157, b_+159); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+159, b_+162); TAIL(interactionDelete); }
  CYC(b_+159, b_+162);
  CALL_L(b_+162, miscBoyNpcs_d_continue_var03_01, b_+165);
  CYC(b_+165, b_+168); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+168, b_+170); alu_cp(gb, 0x00);
  if (!(F & FZ)) { RET_TAKEN(b_+170); return; }
  CYC(b_+170, b_+171);
  CYC(b_+171, b_+172); H = D;
  CYC(b_+172, b_+174); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+174, b_+176); mem_wr(gb, HL, 0x08);
  CYC(b_+176, b_+178); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+178, b_+180); mem_wr(gb, HL, 0x28);
  CYC(b_+180, b_+182); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+182, b_+184); mem_wr(gb, HL, 0x62);
  CYC(b_+184, b_+186); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+186, b_+188); mem_wr(gb, HL, 0x28);
  CYC(b_+188, b_+190); A = 0x06;
  CYC(b_+190, b_+193);
  TAIL(interactionSetAnimation);
func_68e9:
  CALL_C(b_+193, s_getRandomNumber, SYM(getRandomNumber), b_+196);
  CYC(b_+196, b_+198); alu_and(gb, 0x3f);
  CYC(b_+198, b_+200); alu_add(gb, 0x78);
  CYC(b_+200, b_+201); H = D;
  CYC(b_+201, b_+203); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+203, b_+204); mem_wr(gb, HL, A);
  RET(b_+204); return;
state1:
  CYC(b_+205, b_+207); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+207, b_+208); A = mem_rd(gb, DE);
  CYC(b_+208, b_+209); push_effect(gb, b_+209);
  do { uint16_t jt_ = (miscBoyNpcs_d_jump_table(gb));
    if (jt_ == b_+217) goto state1_var03_00;
    if (jt_ == b_+273) goto state1_var03_01;
    if (jt_ == b_+306) goto state1_var03_02;
    if (jt_ == b_+300) goto state1_var03_03;
    HANDOFF(HL);
  } while (0);
state1_var03_00:
  CYC(b_+217, b_+219); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+219, b_+220); A = mem_rd(gb, DE);
  CYC(b_+220, b_+221); push_effect(gb, b_+221);
  do { uint16_t jt_ = (miscBoyNpcs_d_jump_table(gb));
    if (jt_ == b_+225) goto state1_var03_00_substate0;
    if (jt_ == b_+248) goto state1_var03_00_substate1;
    HANDOFF(HL);
  } while (0);
state1_var03_00_substate0:
  CALL_C(b_+225, s_func_6abc_hook, SYM(func_6abc), b_+228);
  if (!(F & FZ)) { CYCT(b_+228, b_+230); goto L_691d; }
  CYC(b_+228, b_+230);
  CYC(b_+230, b_+232); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+232, b_+234); mem_wr(gb, HL, 0x01);
  CALL_C(b_+234, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+237);
  CYC(b_+237, b_+240); SET_HL(wccd4);
  CYC(b_+240, b_+242); mem_wr(gb, HL, 0x01);
  CALL_C(b_+242, s_interactionAnimate, SYM(interactionAnimate), b_+245);
L_691d:
  CYC(b_+245, b_+248);
  goto state1_var03_00_runScriptPushLinkAwayUpdateDrawPriority;
state1_var03_00_substate1:
  CYC(b_+248, b_+251); A = mem_rd(gb, wccd4);
  CYC(b_+251, b_+253); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+253, b_+255); goto state1_var03_00_runScriptPushLinkAwayUpdateDrawPriority; }
  CYC(b_+253, b_+255);
  CALL_L(b_+255, miscBoyNpcs_d_func_68e9, b_+258);
  CYC(b_+258, b_+260); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+260, b_+262); mem_wr(gb, HL, 0x00);
  CYC(b_+262, b_+264); A = 0x08;
  CALL_C(b_+264, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+267);
state1_var03_00_runScriptPushLinkAwayUpdateDrawPriority:
  CALL_C(b_+267, s_interactionRunScript, SYM(interactionRunScript), b_+270);
  CYC(b_+270, b_+273);
  TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
state1_var03_01:
  CYC(b_+273, b_+274); H = D;
  CYC(b_+274, b_+276); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+276, b_+277); A = mem_rd(gb, HL);
  CYC(b_+277, b_+279); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+279, b_+281); goto state1_var03_03; }
  CYC(b_+279, b_+281);
  CALL_C(b_+281, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+284);
  if (!(F & FZ)) { CYCT(b_+284, b_+286); goto L_6951; }
  CYC(b_+284, b_+286);
  CALL_C(b_+286, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+289);
  CYC(b_+289, b_+290); alu_xor(gb, A);
  CYC(b_+290, b_+292); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+292, b_+293); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+293, b_+294); mem_wr(gb, HL, A);
  CALL_C(b_+294, s_beginJump_hook, SYM(beginJump), b_+297);
L_6951:
  CALL_C(b_+297, s_updateSpeedZ_hook, SYM(updateSpeedZ), b_+300);
state1_var03_03:
  CALL_C(b_+300, s_interactionRunScript, SYM(interactionRunScript), b_+303);
  CYC(b_+303, b_+306);
  TAIL(interactionAnimateAsNpc);
state1_var03_02:
  CYC(b_+306, b_+309); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+309, b_+311); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+311, b_+314); goto state1_var03_03; }
  CYC(b_+311, b_+314);
  CYC(b_+314, b_+316); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+316, b_+317); A = mem_rd(gb, DE);
  CYC(b_+317, b_+318); push_effect(gb, b_+318);
  do { uint16_t jt_ = (miscBoyNpcs_d_jump_table(gb));
    if (jt_ == b_+344) goto state1_var03_02_substate0;
    if (jt_ == b_+388) goto state1_var03_02_substate1;
    if (jt_ == b_+403) goto state1_var03_02_substate2;
    if (jt_ == b_+432) goto state1_var03_02_substate3;
    if (jt_ == b_+444) goto state1_var03_02_substate4;
    if (jt_ == b_+478) goto state1_var03_02_substate5;
    if (jt_ == b_+498) goto state1_var03_02_substate6;
    if (jt_ == b_+505) goto state1_var03_02_substate7;
    if (jt_ == b_+545) goto state1_var03_02_substate8;
    if (jt_ == b_+563) goto state1_var03_02_substate9;
    if (jt_ == b_+599) goto state1_var03_02_substateB;
    if (jt_ == b_+638) goto state1_var03_02_substateC;
    HANDOFF(HL);
  } while (0);
state1_var03_02_substate0:
  CALL_C(b_+344, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+347);
  if (!(F & FC)) { CYCT(b_+347, b_+349); goto L_698a; }
  CYC(b_+347, b_+349);
  CYC(b_+349, b_+350); H = D;
  CYC(b_+350, b_+352); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+352, b_+354); mem_wr(gb, HL, 0x0c);
L_698a:
  CALL_C(b_+354, s_func_6ac1_hook, SYM(func_6ac1), b_+357);
  if (!(F & FZ)) { CYCT(b_+357, b_+360); TAIL_S(runScriptSetPriorityRelativeToLink_withTerrainEffects); }
  CYC(b_+357, b_+360);
  CALL_C(b_+360, s_objectApplySpeed, SYM(objectApplySpeed), b_+363);
  CYC(b_+363, b_+365); alu_cp(gb, 0x4b);
  if (F & FC) { CYCT(b_+365, b_+367); goto L_69a9; }
  CYC(b_+365, b_+367);
  CALL_C(b_+367, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+370);
  CYC(b_+370, b_+373); SET_BC(0xfe80);
  CALL_C(b_+373, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+376);
  CYC(b_+376, b_+378); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+378, b_+380); mem_wr(gb, HL, 0x14);
  CYC(b_+380, b_+382); A = 0x09;
  CALL_C(b_+382, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+385);
L_69a9:
  CYC(b_+385, b_+388);
  TAIL_S(animateRunScript);
state1_var03_02_substate1:
  CYC(b_+388, b_+391); A = mem_rd(gb, wUnknown);
  CYC(b_+391, b_+392); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+392); return; }
  CYC(b_+392, b_+393);
  CYC(b_+393, b_+394); A = alu_inc8(gb, A);
  CYC(b_+394, b_+397); mem_wr(gb, wUnknown, A);
  CALL_C(b_+397, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+400);
  CYC(b_+400, b_+403);
  TAIL(objectSetVisiblec2);
state1_var03_02_substate2:
  CYC(b_+403, b_+405); C = 0x20;
  CALL_C(b_+405, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+408);
  if (!(F & FZ)) { CYCT(b_+408, b_+411); TAIL(objectApplySpeed); }
  CYC(b_+408, b_+411);
  CALL_C(b_+411, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+414);
  CYC(b_+414, b_+416); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+416, b_+418); mem_wr(gb, HL, 0x28);
  CALL_C(b_+418, s_objectCenterOnTile, SYM(objectCenterOnTile), b_+421);
  CYC(b_+421, b_+423); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+423, b_+424); A = mem_rd(gb, HL);
  CYC(b_+424, b_+426); alu_sub(gb, 0x05);
  CYC(b_+426, b_+427); mem_wr(gb, HL, A);
  CYC(b_+427, b_+429); A = 0x06;
  CYC(b_+429, b_+432);
  TAIL(interactionSetAnimation);
state1_var03_02_substate3:
  CALL_C(b_+432, s_func_6abc_hook, SYM(func_6abc), b_+435);
  if (!(F & FZ)) { RET_TAKEN(b_+435); return; }
  CYC(b_+435, b_+436);
  CALL_C(b_+436, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+439);
  CYC(b_+439, b_+441); A = 0x05;
  CYC(b_+441, b_+444);
  TAIL(interactionSetAnimation);
state1_var03_02_substate4:
  CYC(b_+444, b_+446); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+446, b_+449); A = mem_rd(gb, wUnknown);
  CYC(b_+449, b_+450); mem_wr(gb, DE, A);
  CYC(b_+450, b_+451); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+451); return; }
  CYC(b_+451, b_+452);
  CALL_C(b_+452, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+455);
  CYC(b_+455, b_+458); SET_BC(0xfd40);
  CALL_C(b_+458, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+461);
  CYC(b_+461, b_+463); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+463, b_+465); mem_wr(gb, HL, 0xf6);
  CYC(b_+465, b_+467); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+467, b_+469); mem_wr(gb, HL, 0x28);
  CYC(b_+469, b_+471); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+471, b_+473); mem_wr(gb, HL, 0x00);
  CYC(b_+473, b_+475); A = 0x53;
  CYC(b_+475, b_+478);
  TAIL(playSound_b00);
state1_var03_02_substate5:
  CYC(b_+478, b_+480); C = 0x20;
  CALL_C(b_+480, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+483);
  if (!(F & FZ)) { CYCT(b_+483, b_+486); TAIL(objectApplySpeed); }
  CYC(b_+483, b_+486);
  CALL_C(b_+486, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+489);
  CYC(b_+489, b_+491); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+491, b_+493); mem_wr(gb, HL, 0x10);
  CYC(b_+493, b_+495); L = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+495, b_+497); mem_wr(gb, HL, 0x00);
  RET(b_+497); return;
state1_var03_02_substate6:
  CALL_C(b_+498, s_func_6abc_hook, SYM(func_6abc), b_+501);
  if (!(F & FZ)) { RET_TAKEN(b_+501); return; }
  CYC(b_+501, b_+502);
  CYC(b_+502, b_+505);
  TAIL(interactionIncSubstate);
state1_var03_02_substate7:
  CALL_C(b_+505, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+508);
  if (!(F & FC)) { CYCT(b_+508, b_+510); goto L_6a2b; }
  CYC(b_+508, b_+510);
  CYC(b_+510, b_+511); H = D;
  CYC(b_+511, b_+513); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+513, b_+515); mem_wr(gb, HL, 0x0c);
L_6a2b:
  CALL_C(b_+515, s_func_6ac1_hook, SYM(func_6ac1), b_+518);
  if (!(F & FZ)) { CYCT(b_+518, b_+521); TAIL_S(runScriptSetPriorityRelativeToLink_withTerrainEffects); }
  CYC(b_+518, b_+521);
  CALL_C(b_+521, s_objectApplySpeed, SYM(objectApplySpeed), b_+524);
  CYC(b_+524, b_+526); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+526, b_+527); A = mem_rd(gb, DE);
  CYC(b_+527, b_+529); alu_cp(gb, 0x28);
  if (!(F & FC)) { CYCT(b_+529, b_+531); goto L_6a46; }
  CYC(b_+529, b_+531);
  CALL_C(b_+531, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+534);
  CYC(b_+534, b_+536); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+536, b_+538); mem_wr(gb, HL, 0x06);
  CYC(b_+538, b_+540); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+540, b_+542); mem_wr(gb, HL, 0x18);
L_6a46:
  CYC(b_+542, b_+545);
  TAIL_S(animateRunScript);
state1_var03_02_substate8:
  CALL_C(b_+545, s_func_6abc_hook, SYM(func_6abc), b_+548);
  if (!(F & FZ)) { RET_TAKEN(b_+548); return; }
  CYC(b_+548, b_+549);
  CYC(b_+549, b_+551); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+551, b_+552); A = mem_rd(gb, HL);
  CYC(b_+552, b_+554); A = alu_swap(gb, A);
  CYC(b_+554, b_+555); alu_rlca(gb);
  CYC(b_+555, b_+557); alu_add(gb, 0x05);
  CALL_C(b_+557, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+560);
  CYC(b_+560, b_+563);
  TAIL(interactionIncSubstate);
state1_var03_02_substate9:
  CALL_C(b_+563, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+566);
  if (!(F & FC)) { CYCT(b_+566, b_+568); goto L_6a65; }
  CYC(b_+566, b_+568);
  CYC(b_+568, b_+569); H = D;
  CYC(b_+569, b_+571); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+571, b_+573); mem_wr(gb, HL, 0x0c);
L_6a65:
  CALL_C(b_+573, s_func_6ac1_hook, SYM(func_6ac1), b_+576);
  if (!(F & FZ)) { CYCT(b_+576, b_+578); TAIL_S(runScriptSetPriorityRelativeToLink_withTerrainEffects); }
  CYC(b_+576, b_+578);
  CALL_C(b_+578, s_objectApplySpeed, SYM(objectApplySpeed), b_+581);
  CYC(b_+581, b_+583); alu_cp(gb, 0x18);
  if (!(F & FC)) { CYCT(b_+583, b_+585); goto L_6a7c; }
  CYC(b_+583, b_+585);
  CALL_C(b_+585, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+588);
  CYC(b_+588, b_+590); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+590, b_+592); mem_wr(gb, HL, 0x06);
  CYC(b_+592, b_+594); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+594, b_+596); mem_wr(gb, HL, 0x10);
L_6a7c:
  CYC(b_+596, b_+599);
  TAIL_S(animateRunScript);
state1_var03_02_substateB:
  CALL_C(b_+599, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+602);
  if (!(F & FC)) { CYCT(b_+602, b_+604); goto L_6a89; }
  CYC(b_+602, b_+604);
  CYC(b_+604, b_+605); H = D;
  CYC(b_+605, b_+607); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+607, b_+609); mem_wr(gb, HL, 0x0c);
L_6a89:
  CALL_C(b_+609, s_func_6ac1_hook, SYM(func_6ac1), b_+612);
  if (!(F & FZ)) { CYCT(b_+612, b_+614); TAIL_S(runScriptSetPriorityRelativeToLink_withTerrainEffects); }
  CYC(b_+612, b_+614);
  CALL_C(b_+614, s_objectApplySpeed, SYM(objectApplySpeed), b_+617);
  CYC(b_+617, b_+619); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+619, b_+620); A = mem_rd(gb, DE);
  CYC(b_+620, b_+622); alu_cp(gb, 0x62);
  if (F & FC) { CYCT(b_+622, b_+624); goto L_6aa3; }
  CYC(b_+622, b_+624);
  CALL_C(b_+624, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+627);
  CYC(b_+627, b_+629); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+629, b_+631); mem_wr(gb, HL, 0x06);
  CYC(b_+631, b_+633); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+633, b_+635); mem_wr(gb, HL, 0x08);
L_6aa3:
  CYC(b_+635, b_+638);
  TAIL_S(animateRunScript);
state1_var03_02_substateC:
  CALL_C(b_+638, s_func_6abc_hook, SYM(func_6abc), b_+641);
  if (!(F & FZ)) { RET_TAKEN(b_+641); return; }
  CYC(b_+641, b_+642);
  CYC(b_+642, b_+644); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+644, b_+646); mem_wr(gb, HL, 0x00);
  CYC(b_+646, b_+648); A = 0x06;
  CYC(b_+648, b_+651);
  TAIL(interactionSetAnimation);
}

void s_animateRunScript_hook(GB *gb) {
  BASE(animateRunScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_interactionAnimate, SYM(interactionAnimate), b_+3);
  s_runScriptSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return; // falls through
}

void s_runScriptSetPriorityRelativeToLink_withTerrainEffects_hook(GB *gb) {
  BASE(runScriptSetPriorityRelativeToLink_withTerrainEffects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_interactionRunScript, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

