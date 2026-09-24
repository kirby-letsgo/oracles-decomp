#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/explorersCrypt4ArmosButtonPuzzle.s.
// INTERAC_D7_4_ARMOS_BUTTON_PUZZLE

static uint16_t explorersCrypt4ArmosButtonPuzzle_jump_table(GB *gb) {
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

// interactionCode66@subid1@func_55c8
static void explorersCrypt4ArmosButtonPuzzle_subid1_func_55c8(GB *gb) {
  BASE(interactionCode66);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+92, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+95);
  CYC(b_+95, b_+96); C = A;
  CYC(b_+96, b_+98); A = 0x03;
  CYC(b_+98, b_+100); mem_wr(gb, IO_SVBK, A);
  CYC(b_+100, b_+102); B = 0xdf;
  CYC(b_+102, b_+103); A = mem_rd(gb, BC);
  CYC(b_+103, b_+104); B = A;
  CYC(b_+104, b_+105); alu_xor(gb, A);
  CYC(b_+105, b_+107); mem_wr(gb, IO_SVBK, A);
  CYC(b_+107, b_+108); A = B;
  CYC(b_+108, b_+111);
  TAIL(setTile);
}

// INTERAC_D7_4_ARMOS_BUTTON_PUZZLE
void s_interactionCode66_hook(GB *gb) {
  BASE(interactionCode66);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (explorersCrypt4ArmosButtonPuzzle_jump_table(gb));
    if (jt_ == b_+111) goto subid0;
    if (jt_ == b_+8) goto subid1;
    HANDOFF(HL);
  } while (0);
subid1:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (explorersCrypt4ArmosButtonPuzzle_jump_table(gb));
    if (jt_ == b_+16) goto subid1_state0;
    if (jt_ == b_+60) goto subid1_state1;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+22);
  CYC(b_+22, b_+24); A = 0x27;
  CALL_C(b_+24, s_objectMimicBgTile, SYM(objectMimicBgTile), b_+27);
  CYC(b_+27, b_+29); A = 0x06;
  CALL_C(b_+29, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+32);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x28);
  CYC(b_+36, b_+38); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x10);
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x02);
  CYC(b_+43, b_+45); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+45, b_+46); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+47, b_+48); push_effect(gb, DE);
  CALL_L(b_+48, explorersCrypt4ArmosButtonPuzzle_subid1_func_55c8, b_+51);
  CYC(b_+51, b_+52); SET_DE(pop_effect(gb));
  CYC(b_+52, b_+54); A = 0x71;
  CALL_C(b_+54, s_playSound, SYM(playSound_b00), b_+57);
  CYC(b_+57, b_+60);
  TAIL(objectSetVisible82);
subid1_state1:
  CALL_C(b_+60, s_objectApplySpeed, SYM(objectApplySpeed), b_+63);
  CALL_C(b_+63, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+66);
  CALL_C(b_+66, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+69);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x10);
  CYC(b_+72, b_+73); L = alu_inc8(gb, L);
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+74, b_+76); goto L_55bc; }
  CYC(b_+74, b_+76);
  CALL_C(b_+76, s_interactionCheckAdjacentTileIsSolid, SYM(interactionCheckAdjacentTileIsSolid), b_+79);
  if (F & FZ) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
L_55bc:
  CALL_C(b_+80, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+83);
  CYC(b_+83, b_+84); C = A;
  CYC(b_+84, b_+86); A = 0x27;
  CALL_C(b_+86, s_setTile, SYM(setTile), b_+89);
  CYC(b_+89, b_+92);
  TAIL(interactionDelete);

subid0:
  CYC(b_+111, b_+113); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+113, b_+114); A = mem_rd(gb, DE);
  CYC(b_+114, b_+115); push_effect(gb, b_+115);
  do { uint16_t jt_ = (explorersCrypt4ArmosButtonPuzzle_jump_table(gb));
    if (jt_ == b_+121) goto subid0_state0;
    if (jt_ == b_+172) goto subid0_state1;
    if (jt_ == b_+265) goto subid0_state2;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CYC(b_+121, b_+123); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+123, b_+125); A = 0x01;
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+128); A = 0x03;
  CYC(b_+128, b_+130); mem_wr(gb, IO_SVBK, A);
  CYC(b_+130, b_+132); B = 0xdf;
  CYC(b_+132, b_+135); SET_HL(b_+164 /* @subid0@table_5610 */);
  CYC(b_+135, b_+137); A = 0xa3;
L_55f5:
  CYC(b_+137, b_+138); C = mem_rd(gb, HL);
  CYC(b_+138, b_+139); SET_HL(HL + 1);
  CYC(b_+139, b_+140); mem_wr(gb, BC, A);
  CYC(b_+140, b_+141); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+141, b_+143); goto L_55f5; }
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+144); H = B;
  CYC(b_+144, b_+146); L = 0x17;
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0xa0);
  CYC(b_+148, b_+150); L = 0x3b;
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0xa0);
  CYC(b_+152, b_+154); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+154, b_+156); mem_wr(gb, HL, 0xa0);
  CYC(b_+156, b_+158); L = 0x57;
  CYC(b_+158, b_+160); mem_wr(gb, HL, 0xa2);
  CYC(b_+160, b_+161); alu_xor(gb, A);
  CYC(b_+161, b_+163); mem_wr(gb, IO_SVBK, A);
  RET(b_+163); return;

subid0_state1:
  CYC(b_+172, b_+175); SET_HL(wActiveTriggers);
  CYC(b_+175, b_+177); alu_bit(gb, 4, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+177, b_+179); goto L_562a; }
  CYC(b_+177, b_+179);
  CYC(b_+179, b_+181); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+181, b_+183); goto L_562a; }
  CYC(b_+181, b_+183);
  CYC(b_+183, b_+185); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+185, b_+187); C = 0x32;
  if (!(F & FZ)) CALL_C_CC(b_+187, s_func_5694_hook, SYM(func_5694), b_+190);
  else CYC(b_+187, b_+190);
L_562a:
  CYC(b_+190, b_+193); SET_HL(wActiveTriggers);
  CYC(b_+193, b_+195); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+195, b_+197); goto L_563c; }
  CYC(b_+195, b_+197);
  CYC(b_+197, b_+199); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+199, b_+201); goto L_563c; }
  CYC(b_+199, b_+201);
  CYC(b_+201, b_+203); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+203, b_+205); C = 0x52;
  if (!(F & FZ)) CALL_C_CC(b_+205, s_func_5694_hook, SYM(func_5694), b_+208);
  else CYC(b_+205, b_+208);
L_563c:
  CYC(b_+208, b_+211); SET_HL(wActiveTriggers);
  CYC(b_+211, b_+213); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+213, b_+215); goto L_564e; }
  CYC(b_+213, b_+215);
  CYC(b_+215, b_+217); alu_bit(gb, 2, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+217, b_+219); goto L_564e; }
  CYC(b_+217, b_+219);
  CYC(b_+219, b_+221); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+221, b_+223); C = 0x95;
  if (!(F & FZ)) CALL_C_CC(b_+223, s_func_56a5_hook, SYM(func_56a5), b_+226);
  else CYC(b_+223, b_+226);
L_564e:
  CYC(b_+226, b_+229); SET_HL(wActiveTriggers);
  CYC(b_+229, b_+231); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+231, b_+233); goto L_5660; }
  CYC(b_+231, b_+233);
  CYC(b_+233, b_+235); alu_bit(gb, 3, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+235, b_+237); goto L_5660; }
  CYC(b_+235, b_+237);
  CYC(b_+237, b_+239); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+239, b_+241); C = 0x97;
  if (!(F & FZ)) CALL_C_CC(b_+241, s_func_56a5_hook, SYM(func_56a5), b_+244);
  else CYC(b_+241, b_+244);
L_5660:
  CYC(b_+244, b_+247); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+247, b_+248); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+248); return; }
  CYC(b_+248, b_+249);
  CALL_C(b_+249, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+252);
  CYC(b_+252, b_+254); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+254, b_+257); TAIL(interactionDelete); }
  CYC(b_+254, b_+257);
  CYC(b_+257, b_+259); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+259, b_+261); A = 0x3c;
  CYC(b_+261, b_+262); mem_wr(gb, DE, A);
  CYC(b_+262, b_+265);
  TAIL(interactionIncState);
subid0_state2:
  CALL_C(b_+265, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+268);
  if (!(F & FZ)) { RET_TAKEN(b_+268); return; }
  CYC(b_+268, b_+269);
  CYC(b_+269, b_+271); A = 0xa3;
  CALL_C(b_+271, s_findTileInRoom, SYM(findTileInRoom), b_+274);
  if (!(F & FZ)) { CYCT(b_+274, b_+276); goto L_5688; }
  CYC(b_+274, b_+276);
  CYC(b_+276, b_+278); A = 0x5a;
  CALL_C(b_+278, s_playSound, SYM(playSound_b00), b_+281);
  CYC(b_+281, b_+284);
  TAIL(interactionDelete);
L_5688:
  CYC(b_+284, b_+287); SET_BC(0x3001);
  CALL_C(b_+287, s_createTreasure, SYM(createTreasure), b_+290);
  CALL_C(b_+290, s_objectCopyPosition, SYM(objectCopyPosition), b_+293);
  CYC(b_+293, b_+296);
  TAIL(interactionDelete);
}

void s_func_5694_hook(GB *gb) {
  BASE(func_5694);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xcf;
L_5696:
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+5); alu_cp(gb, 0x27);
  CYC(b_+5, b_+7); E = 0x18;
  if (F & FZ) CALL_C_CC(b_+7, s_func_56b8_hook, SYM(func_56b8), b_+10);
  else CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); C = alu_inc8(gb, C);
  CYC(b_+11, b_+12); A = C;
  CYC(b_+12, b_+14); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17);
  goto L_5696;
}

void s_func_56a5_hook(GB *gb) {
  BASE(func_56a5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xcf;
L_56a7:
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+5); alu_cp(gb, 0x27);
  CYC(b_+5, b_+7); E = 0x10;
  if (F & FZ) CALL_C_CC(b_+7, s_func_56b8_hook, SYM(func_56b8), b_+10);
  else CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); A = C;
  CYC(b_+11, b_+13); alu_sub(gb, 0x10);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+16); alu_and(gb, 0xf0);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19);
  goto L_56a7;
}

void s_func_56b8_hook(GB *gb) {
  BASE(func_56b8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x66);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x01);
  CYC(b_+9, b_+10); push_effect(gb, BC);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+12, s_setShortPosition_paramC, SYM(setShortPosition_paramC), b_+15);
  CYC(b_+15, b_+16); SET_BC(pop_effect(gb));
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+18, b_+19); mem_wr(gb, HL, E);
  RET(b_+19); return;
}

