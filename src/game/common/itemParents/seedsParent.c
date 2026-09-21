#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t seeds_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void seeds_parent_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
    ret_effect(gb);
  }
}

void parentItemGenericState1_hook(GB *gb) {
  BASE(parentItemGenericState1);
  CYC(b_+0, b_+2); E = 0x21;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+4, b_+7); specialObjectAnimate_optimized_hook(gb); }
  else { CYC(b_+4, b_+7); CYC(b_+7, b_+10); clearParentItem_hook(gb); }
}

void clearSelfIfNoSeeds_hook(GB *gb) {
  BASE(clearSelfIfNoSeeds);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wSatchelSelectedSeeds);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); seeds_parent_add_a_to_hl(gb);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); B |= 0x20;
  CYC(b_+8, b_+11); SET_HL(wNumEmberSeeds);
  CYC(b_+11, b_+12); push_effect(gb, b_+12); seeds_parent_add_a_to_hl(gb);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+15); ret_effect(gb); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
  CYC(b_+16, b_+19); TAIL(clearParentItem);
}

void parentItemCode_satchel_hook(GB *gb) {
  BASE(parentItemCode_satchel);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (seeds_parent_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == SYM(parentItemGenericState1) && hook_enabled_at(gb, SYM(parentItemGenericState1))) { parentItemGenericState1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+8, b_+11); A = W8(w1Companion_id);
  CYC(b_+11, b_+13); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(b_+13, b_+16); goto clear; }
  CYC(b_+13, b_+16);
  CALL_C(b_+16, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+19);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); goto clear; }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+25); A = W8(wLinkSwimmingState);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+26, b_+29); goto clear; }
  CYC(b_+26, b_+29);
  CALL_C(b_+29, clearSelfIfNoSeeds_hook, SYM(clearSelfIfNoSeeds), b_+32);
  CYC(b_+32, b_+33); A = B;
  CYC(b_+33, b_+35); alu_cp(gb, 0x22);
  if (F & FZ) { CYCT(b_+35, b_+37); goto pegasus; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+38); push_effect(gb, BC);
  CALL_C(b_+38, parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+41);
  CYC(b_+41, b_+42); SET_BC(pop_effect(gb));
  CYC(b_+42, b_+43); push_effect(gb, BC);
  CYC(b_+43, b_+45); C = 0x00;
  CYC(b_+45, b_+47); E = 0x01;
  CALL_C(b_+47, itemCreateChildWithID_hook, SYM(itemCreateChildWithID), b_+50);
  CYC(b_+50, b_+51); SET_BC(pop_effect(gb));
  if (F & FC) { CYCT(b_+51, b_+54); goto clear; }
  CYC(b_+51, b_+54);
  CYC(b_+54, b_+55); A = B;
  CYC(b_+55, b_+58); TAIL(decNumActiveSeeds);

pegasus:
  CYC(b_+58, b_+61); SET_HL(wPegasusSeedCounter);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+62, b_+63); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+63, b_+65); goto clear; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); A = 0x03;
  CYC(b_+67, b_+68); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0xc0);
  CYC(b_+70, b_+71); A = B;
  CALL_C(b_+71, decNumActiveSeeds_hook, SYM(decNumActiveSeeds), b_+74);
  CYC(b_+74, b_+77); SET_HL(w1ReservedItemF);
  CYC(b_+77, b_+79); A = 0x03;
  CYC(b_+79, b_+80); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x1a);

clear:
  CYC(b_+82, b_+85); TAIL(clearParentItem);
}

void parentItemCode_slingshot_hook(GB *gb) {
  BASE(parentItemCode_slingshot);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0,b_+2); E=4; CYC(b_+2,b_+3); A=mem_rd(gb,DE); CYC(b_+3,b_+4); push_effect(gb,b_+4);
  do { uint16_t jt_ = (seeds_parent_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+37) { goto state1; }
    else if (jt_ == b_+95) { goto state2; }
    else { hook_continue(gb,HL,sp0_); return; }
  } while (0);
state0:
  CYC(b_+10,b_+12);A=1;CALL_C(b_+12,clearSelfIfNoSeeds_hook,SYM(clearSelfIfNoSeeds),b_+15);CALL_C(b_+15,updateLinkDirectionFromAngle_hook,SYM(updateLinkDirectionFromAngle),b_+18);CALL_C(b_+18,parentItemLoadAnimationAndIncState_hook,SYM(parentItemLoadAnimationAndIncState),b_+21);CALL_C(b_+21,itemCreateChild_hook,SYM(itemCreateChild),b_+24);
  CYC(b_+24,b_+27);A=W8(wLinkAngle);CYC(b_+27,b_+29);alu_bit(gb,7,A);if(F&FZ){CYCT(b_+29,b_+31);CYC(b_+121,b_+122);alu_rrca(gb);CYC(b_+122,b_+123);alu_rrca(gb);CYC(b_+123,b_+125);goto update_angle;}CYC(b_+29,b_+31);CYC(b_+31,b_+34);A=W8(w1Link_direction);CYC(b_+34,b_+35);alu_add(gb,A);CYC(b_+35,b_+37);goto update_angle;
state1:
  CYC(b_+37,b_+39);A=1;CALL_C(b_+39,clearSelfIfNoSeeds_hook,SYM(clearSelfIfNoSeeds),b_+42);CALL_C(b_+42,parentItemCheckButtonPressed_hook,SYM(parentItemCheckButtonPressed),b_+45);if(!(F&FZ)){CYCT(b_+45,b_+47);goto check_angle;}CYC(b_+45,b_+47);CYC(b_+47,b_+50);A=W8(wIsSeedShooterInUse);CYC(b_+50,b_+51);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+51,b_+54);clearParentItem_hook(gb);return;}CYC(b_+51,b_+54);CYC(b_+54,b_+56);E=0x19;CYC(b_+56,b_+58);A=0xd0;CYC(b_+58,b_+59);mem_wr(gb,DE,A);CYC(b_+59,b_+61);A=1;CALL_C(b_+61,clearSelfIfNoSeeds_hook,SYM(clearSelfIfNoSeeds),b_+64);CYC(b_+64,b_+65);push_effect(gb,BC);CYC(b_+65,b_+67);E=1;CALL_C(b_+67,itemCreateChildWithID_hook,SYM(itemCreateChildWithID),b_+70);CYC(b_+70,b_+72);E=9;CYC(b_+72,b_+73);A=mem_rd(gb,DE);CYC(b_+73,b_+74);alu_add(gb,A);CYC(b_+74,b_+75);alu_add(gb,A);CYC(b_+75,b_+77);L=9;CYC(b_+77,b_+78);mem_wr(gb,HL,A);CYC(b_+78,b_+79);SET_BC(pop_effect(gb));CYC(b_+79,b_+80);A=B;CALL_C(b_+80,decNumActiveSeeds_hook,SYM(decNumActiveSeeds),b_+83);CALL_C(b_+83,itemIncState_hook,SYM(itemIncState),b_+86);CYC(b_+86,b_+88);L=7;CYC(b_+88,b_+90);mem_wr(gb,HL,0x0c);CYC(b_+90,b_+92);A=0xcb;CYC(b_+92,b_+95);playSound_b00_hook(gb);return;
state2:
  CALL_C(b_+95,itemDecCounter2_hook,SYM(itemDecCounter2),b_+98);if(!(F&FZ)){CYCT(b_+98,b_+99);ret_effect(gb);return;}CYC(b_+98,b_+99);CYC(b_+99,b_+102);A=W8(wLinkAngle);CYC(b_+102,b_+103);push_effect(gb,AF);CYC(b_+103,b_+105);L=9;CYC(b_+105,b_+106);A=mem_rd(gb,HL);CYC(b_+106,b_+107);alu_add(gb,A);CYC(b_+107,b_+108);alu_add(gb,A);CYC(b_+108,b_+111);W8(wLinkAngle)=A;CALL_C(b_+111,updateLinkDirectionFromAngle_hook,SYM(updateLinkDirectionFromAngle),b_+114);CYC(b_+114,b_+115);SET_AF(pop_effect(gb));CYC(b_+115,b_+118);W8(wLinkAngle)=A;CYC(b_+118,b_+121);clearParentItem_hook(gb);return;
check_angle:
  CYC(b_+125,b_+128);A=W8(wGameKeysJustPressed);CYC(b_+128,b_+130);alu_and(gb,0xf0);if(!(F&FZ))CYCT(b_+130,b_+132);else{CYC(b_+130,b_+132);CALL_C(b_+132,itemDecCounter2_hook,SYM(itemDecCounter2),b_+135);if(!(F&FZ)){CYCT(b_+135,b_+137);goto animation;}CYC(b_+135,b_+137);}CYC(b_+137,b_+140);A=W8(wLinkAngle);CYC(b_+140,b_+141);alu_rrca(gb);CYC(b_+141,b_+142);alu_rrca(gb);if(F&FC){CYCT(b_+142,b_+144);goto animation;}CYC(b_+142,b_+144);CYC(b_+144,b_+145);H=D;CYC(b_+145,b_+147);L=9;CYC(b_+147,b_+148);alu_sub(gb,mem_rd(gb,HL));if(F&FZ){CYCT(b_+148,b_+150);goto animation;}CYC(b_+148,b_+150);CYC(b_+150,b_+152);alu_bit(gb,2,A);CYC(b_+152,b_+154);A=0xff;if(!(F&FZ))CYCT(b_+154,b_+156);else{CYC(b_+154,b_+156);CYC(b_+156,b_+158);A=1;}CYC(b_+158,b_+159);alu_add(gb,mem_rd(gb,HL));
update_angle:
  CYC(b_+159,b_+160);H=D;CYC(b_+160,b_+162);L=9;CYC(b_+162,b_+164);alu_and(gb,7);CYC(b_+164,b_+165);mem_wr(gb,HL,A);CYC(b_+165,b_+167);L=7;CYC(b_+167,b_+169);mem_wr(gb,HL,0x10);
animation:
  CALL_C(b_+169,isLinkUnderwater_hook,SYM(isLinkUnderwater),b_+172);CYC(b_+172,b_+174);A=0x48;if(!(F&FZ))CYCT(b_+174,b_+176);else{CYC(b_+174,b_+176);CYC(b_+176,b_+179);A=W8(w1Companion_id);CYC(b_+179,b_+181);alu_cp(gb,0x0a);CYC(b_+181,b_+183);A=0x40;if(F&FZ)CYCT(b_+183,b_+185);else{CYC(b_+183,b_+185);CYC(b_+185,b_+187);A=0x38;}}CYC(b_+187,b_+188);H=D;CYC(b_+188,b_+190);L=9;CYC(b_+190,b_+191);alu_add(gb,mem_rd(gb,HL));CYC(b_+191,b_+193);L=0x31;CYC(b_+193,b_+194);mem_wr(gb,HL,A);CYC(b_+194,b_+196);L=0x3f;CYC(b_+196,b_+198);mem_wr(gb,HL,4);CYC(b_+198,b_+199);ret_effect(gb);
}
