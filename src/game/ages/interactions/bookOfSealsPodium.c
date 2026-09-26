#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t book_of_seals_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void book_add_a_to_hl(GB *gb, uint16_t ra) {
  push_effect(gb, ra); burn_rom(gb,0,0x10,0x11,false); alu_add(gb,L); burn_rom(gb,0,0x11,0x12,false); L=A;
  if (F&FC) { burn_rom(gb,0,0x12,0x13,false); burn_rom(gb,0,0x13,0x14,false); H=alu_inc8(gb,H); burn_rom(gb,0,0x14,0x15,false); }
  else burn_rom(gb,0,0x12,0x13,true);
  ret_effect(gb);
}

// @param[out] c subid; the interaction's script pointer is set to that podium's path list.
void interactionCodeb4__func_69ce_hook(GB *gb) {
  BASE(interactionCodeb4);
  CYC(b_+279, b_+281); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+283); C = A;
  CYC(b_+283, b_+286); SET_HL(b_+334); // @bookPathLists
  CYC(b_+286, b_+287); book_add_a_to_hl(gb, b_+287);
  CYC(b_+287, b_+288); A = mem_rd(gb, HL);
  CYC(b_+288, b_+289); book_add_a_to_hl(gb, b_+289);
  CYC(b_+289, b_+291); E = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(b_+291, b_+292); A = L;
  CYC(b_+292, b_+293); mem_wr(gb, DE, A);
  CYC(b_+293, b_+294); E = alu_inc8(gb, E);
  CYC(b_+294, b_+295); A = H;
  CYC(b_+295, b_+296); mem_wr(gb, DE, A);
  CYC(b_+296, b_+297); ret_effect(gb);
}

void interactionCodeb4__state0_hook(GB *gb) {
  BASE(interactionCodeb4);
  uint16_t sp0_=gb->sp; CYC(b_+14,b_+16); A=1; CYC(b_+16,b_+17); mem_wr(gb,DE,A); CYC(b_+17,b_+19); A=6;
  CALL_C(b_+19,objectSetCollideRadius_hook,SYM(objectSetCollideRadius),b_+22); CALL_C(b_+22,interactionInitGraphics_hook,SYM(interactionInitGraphics),b_+25);
  CALL_C(b_+25,interactionSetAlwaysUpdateBit_hook,SYM(interactionSetAlwaysUpdateBit),b_+28); CYC(b_+28,b_+30); A=0x12; CALL_C(b_+30,interactionSetHighTextIndex_hook,SYM(interactionSetHighTextIndex),b_+33);
  CYC(b_+33,b_+35); E=0x71; CALL_C(b_+35,objectAddToAButtonSensitiveObjectList_hook,SYM(objectAddToAButtonSensitiveObjectList),b_+38); CYC(b_+38,b_+40); E=0x42; CYC(b_+40,b_+41); A=mem_rd(gb,DE); CYC(b_+41,b_+42); alu_or(gb,A);
  if (!(F&FZ)) { CYCT(b_+42,b_+43); ret_effect(gb); return; } CYC(b_+42,b_+43); CYC(b_+43,b_+46); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions); CYC(b_+46,b_+48); B=0x10;
  if (F&FZ) CALL_C_CC(b_+48,clearMemory_hook,SYM(clearMemory),b_+51); else CYC(b_+48,b_+51); CALL_C(b_+51,getThisRoomFlags_hook,SYM(getThisRoomFlags),b_+54); CYC(b_+54,b_+56); alu_bit(gb,6,A);
  if (!(F&FZ)) { CYCT(b_+56,b_+57); ret_effect(gb); return; } CYC(b_+56,b_+57); CALL_C(b_+57,interactionIncState_hook,SYM(interactionIncState),b_+60); CYC(b_+60,b_+63); SET_HL((SYM(interactionCoded8__fillNextGroupWithLava) + 10)); CYC(b_+63,b_+66); interactionSetScript_hook(gb); return;
}

void interactionCodeb4__activatedBook_hook(GB *gb) {
  BASE(interactionCodeb4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+80, b_+82); A = 0x01;
  CALL_C(b_+82, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+85);
  CALL_C(b_+85, interactionCodeb4__func_69ce_hook, b_+279, b_+88);
  CYC(b_+88, b_+89); A = C;
  CYC(b_+89, b_+92); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+92, b_+95); SET_HL(b_+123); // @textTable
  CYC(b_+95, b_+96); book_add_a_to_hl(gb, b_+96);
  CYC(b_+96, b_+97); C = mem_rd(gb, HL);
  CYC(b_+97, b_+99); B = 0x12;
  CALL_C(b_+99, showText_hook, SYM(showText), b_+102);
  CYC(b_+102, b_+104); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(b_+104, b_+107); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+107, b_+110); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+110, b_+111); H = D;
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+113, b_+115); A = 0xd3;
  CYC(b_+115, b_+116); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+116, b_+118); A = 0x03;
  CYC(b_+118, b_+119); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+119, b_+120); L = alu_inc8(gb, L);
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x02);
  CYC(b_+122, b_+123); ret_effect(gb);
}

// interactionCodeb4@spawnAllPodiums: subid 0 spawns the five podiums from @podiumPositions.
static void book_of_seals_spawn_all_podiums(GB *gb) {
  BASE(interactionCodeb4__spawnAllPodiums);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+0, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+3);
  A = 0x01;
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  alu_or(gb, A);
  CYC(b_+9, b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  SET_BC(SYM(interactionCodeb4__podiumPositions));
  E = 0x05;
  CYC(b_+11, b_+16);
  for (;;) {
    CALL_C(b_+16, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+19);
    if (!(F & FZ)) { CYCT(b_+19, b_+20); ret_effect(gb); return; }
    CYC(b_+19, b_+20);
    CYC(b_+20, b_+22); mem_wr(gb, HL, 0xb4);
    L = alu_inc8(gb, L);
    CYC(b_+22, b_+23);
    CYC(b_+23, b_+24); mem_wr(gb, HL, E);
    L = INTERACTION_BASE + OBJ_YH;
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+27); A = mem_rd(gb, BC);
    CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
    L = alu_inc8(gb, L);
    SET_BC(BC + 1);
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+31); A = mem_rd(gb, BC);
    CYC(b_+31, b_+32); mem_wr(gb, HL, A);
    SET_BC(BC + 1);
    E = alu_dec8(gb, E);
    CYC(b_+32, b_+34);
    if (!(F & FZ)) { CYCT(b_+34, b_+36); continue; }
    CYC(b_+34, b_+36);
    break;
  }
  CYC(b_+36, b_+37); ret_effect(gb);
}

// interactionCodeb4@replaceTiles: replaces one tile per two frames from the list at scriptPtr,
// then re-enables everything and advances the state.
static void book_of_seals_replace_tiles(GB *gb) {
  BASE(interactionCodeb4__replaceTiles);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  A = 0x81;
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+5); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+5, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x02);
  L = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_or(gb, A);
  CYC(b_+17, b_+18);
  if (F & FZ) {
    CYCT(b_+18, b_+20);
    CYC(b_+38, b_+41); mem_wr(gb, wDisabledObjects, A);
    CYC(b_+41, b_+44); mem_wr(gb, wMenuDisabled, A);
    E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
    CYC(b_+44, b_+46);
    CYC(b_+46, b_+47); mem_wr(gb, DE, A);
    CYC(b_+47, b_+50);
    TAIL(interactionIncState);
  }
  CYC(b_+18, b_+20);
  C = A;
  E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); push_effect(gb, HL);
  CALL_C(b_+25, setTile_hook, SYM(setTile), b_+28);
  CYC(b_+28, b_+29); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(b_+29, b_+30); ret_effect(gb); return; }
  CYC(b_+29, b_+30);
  E = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  A = L;
  CYC(b_+30, b_+33);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+38); ret_effect(gb);
}

void interactionCodeb4__state1_hook(GB *gb) {
  BASE(interactionCodeb4);
  uint16_t sp0_=gb->sp; CYC(b_+66,b_+67); E=alu_inc8(gb,E); CYC(b_+67,b_+68); A=mem_rd(gb,DE); CYC(b_+68,b_+69); alu_or(gb,A); if (F&FZ) { CYCT(b_+69,b_+72); push_effect(gb,b_+72); book_of_seals_spawn_all_podiums(gb); } else CYC(b_+69,b_+72); CALL_C(b_+72,objectSetPriorityRelativeToLink_withTerrainEffects_hook,SYM(objectSetPriorityRelativeToLink_withTerrainEffects),b_+75); CYC(b_+75,b_+77); E=0x71; CYC(b_+77,b_+78); A=mem_rd(gb,DE); CYC(b_+78,b_+79); alu_or(gb,A); if (F&FZ) { CYCT(b_+79,b_+80); ret_effect(gb); return; } CYC(b_+79,b_+80); interactionCodeb4__activatedBook_hook(gb); return;
}

// Waits for the script (the player placing the book), then plays the jingle and takes the book.
void interactionCodeb4__state2_hook(GB *gb) {
  BASE(interactionCodeb4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+129, b_+130); E = alu_inc8(gb, E);
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+132); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+132, b_+135); push_effect(gb, b_+135); book_of_seals_spawn_all_podiums(gb); }
  else CYC(b_+132, b_+135);
  CALL_C(b_+135, interactionRunScript_hook, SYM(interactionRunScript), b_+138);
  if (!(F & FC)) { CYCT(b_+138, b_+139); ret_effect(gb); return; }
  CYC(b_+138, b_+139);
  CALL_C(b_+139, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+142);
  CYC(b_+142, b_+144); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+144, playSound_b00_hook, SYM(playSound_b00), b_+147);
  CYC(b_+147, b_+149); A = 0x55; // TREASURE_BOOK_OF_SEALS
  CALL_C(b_+149, loseTreasure_hook, SYM(loseTreasure), b_+152);
  CYC(b_+152, b_+154);
  TAIL(interactionCodeb4__activatedBook);
}

void interactionCodeb4__state3_hook(GB *gb) {
  BASE(interactionCodeb4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+154, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+157);
  CYC(b_+157, b_+160); push_effect(gb, b_+160); book_of_seals_replace_tiles(gb);
  CYC(b_+160, b_+163); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+163, b_+164); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+164, b_+165); ret_effect(gb); return; }
  CYC(b_+164, b_+165);
  CYC(b_+165, b_+167); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+167, b_+169); A = 0xf4;
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);
  CYC(b_+170, b_+173);
  TAIL(interactionCodeb4__func_69ce);
}

void interactionCodeb4__state4_hook(GB *gb) {
  BASE(interactionCodeb4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+223, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+226);
  CYC(b_+226, b_+228); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+228, b_+229); A = mem_rd(gb, DE);
  CYC(b_+229, b_+230); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(b_+230, b_+232);
    CYC(b_+232, b_+233); alu_xor(gb, A);
    CYC(b_+233, b_+234); mem_wr(gb, DE, A);
    CYC(b_+234, b_+236); E = INTERACTION_BASE + OBJ_SUBID;
    CYC(b_+236, b_+237); A = mem_rd(gb, DE);
    CYC(b_+237, b_+240); SET_HL(b_+123); // @textTable
    CYC(b_+240, b_+241); book_add_a_to_hl(gb, b_+241);
    CYC(b_+241, b_+242); C = mem_rd(gb, HL);
    CYC(b_+242, b_+244); B = 0x12;
    CYC(b_+244, b_+247);
    TAIL(showText);
  }
  CYCT(b_+230, b_+232);
  CYC(b_+247, b_+250); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+250, b_+252); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+252, b_+253); A = mem_rd(gb, DE);
  CYC(b_+253, b_+254); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+254, b_+255); ret_effect(gb); return; }
  CYC(b_+254, b_+255);
  CALL_C(b_+255, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+258);
  CYC(b_+258, b_+261); push_effect(gb, b_+261); book_of_seals_replace_tiles(gb);
  CYC(b_+261, b_+264); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+264, b_+265); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+265, b_+266); ret_effect(gb); return; }
  CYC(b_+265, b_+266);
  CALL_C(b_+266, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+269);
  CYC(b_+269, b_+271); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+271, b_+273); A = 0x01;
  CYC(b_+273, b_+274); mem_wr(gb, DE, A);
  CYC(b_+274, b_+276); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+276, b_+277); alu_xor(gb, A);
  CYC(b_+277, b_+278); mem_wr(gb, DE, A);
  CYC(b_+278, b_+279); ret_effect(gb);
}

void interactionCodeb4_hook(GB *gb) {
  BASE(interactionCodeb4);
  uint16_t sp0_=gb->sp; CYC(b_+0,b_+2); E=0x44; CYC(b_+2,b_+3); A=mem_rd(gb,DE); CYC(b_+3,b_+4); push_effect(gb,b_+4); do { uint16_t jt_ = (book_of_seals_jump_table(gb));
    if (jt_ == b_+14) { interactionCodeb4__state0_hook(gb); return; }
    else if (jt_ == b_+66) { interactionCodeb4__state1_hook(gb); return; }
    else if (jt_ == b_+129) { interactionCodeb4__state2_hook(gb); return; }
    else if (jt_ == b_+154) { interactionCodeb4__state3_hook(gb); return; }
    else if (jt_ == b_+223) { interactionCodeb4__state4_hook(gb); return; }
    else { hook_continue(gb,HL,sp0_); return; }
  } while (0);
}
