#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodea5_jump_table(GB *gb) {
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

// 0b:5d0e, called from interactionCodea5@state2 and @state4.
// @param[out] zflag z if reached touching book
void interactionCodea5_updateMapleAngle_hook(GB *gb) {
  BASE(interactionCodea5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+253, b_+256); SET_HL(w1Companion_yh);
  CYC(b_+256, b_+257); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+257, b_+258); B = A;
  CYC(b_+258, b_+259); L = alu_inc8(gb, L);
  CYC(b_+259, b_+260); C = mem_rd(gb, HL);
  CYC(b_+260, b_+263); A = mem_rd(gb, wMapleState);
  CYC(b_+263, b_+265); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(b_+265, b_+267); goto l_5d27; } // jr z
  CYC(b_+265, b_+267);
  CYC(b_+267, b_+269); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+269, b_+270); A = mem_rd(gb, DE);
  CYC(b_+270, b_+271); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+271, b_+273); goto l_5d27; } // jr nz
  CYC(b_+271, b_+273);
  CYC(b_+273, b_+275); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+275, b_+276); A = mem_rd(gb, DE);
  CYC(b_+276, b_+277); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+277, b_+278); ret_effect(gb); return; } // ret z
  CYC(b_+277, b_+278);

l_5d27:
  CALL_C(b_+278, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+281);
  CYC(b_+281, b_+283); A = (uint8_t)(A ^ 0x10);
  CYC(b_+283, b_+286); mem_wr(gb, w1Companion_angle, A);
  CYC(b_+286, b_+287); alu_or(gb, D);
  RET(b_+287); return;
}

// ==================================================================================================
// INTERAC_TOUCHING_BOOK
// ==================================================================================================
void interactionCodea5_hook(GB *gb) {
  BASE(interactionCodea5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodea5_jump_table(gb));
    if (jt_ == b_+22) { goto state0; }
    else if (jt_ == b_+77) { goto state1; }
    else if (jt_ == b_+88) { goto state2; }
    else if (jt_ == b_+114) { goto state3; }
    else if (jt_ == b_+145) { goto state4; }
    else if (jt_ == b_+169) { goto state5; }
    else if (jt_ == b_+191) { goto state6; }
    else if (jt_ == b_+205) { goto state7; }
    else if (jt_ == b_+228) { goto state8; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+22, b_+24); A = 0x01;
  CYC(b_+24, b_+27); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+27, b_+30); SET_HL(w1Link_knockbackCounter);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+32, b_+33); ret_effect(gb); return; } // ret nz
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); A = 0x01;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+36, objectTakePosition_hook, SYM(objectTakePosition), b_+39);
  CYC(b_+39, b_+42); SET_BC(0x3850);
  CALL_C(b_+42, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+45);
  CYC(b_+45, b_+47); alu_and(gb, 0x1c);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+53); SET_BC(0xff00); // -$100
  CALL_C(b_+53, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+56);
  CYC(b_+56, b_+58); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(b_+60, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+63);
  CALL_C(b_+63, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+66);
  CYC(b_+66, b_+69); A = mem_rd(gb, w1Link_visible);
  CYC(b_+69, b_+71); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+74); A = 0x57; // SND_GAINHEART
  CYC(b_+74, b_+77); playSound_b00_hook(gb); return; // jp

state1:
  CYC(b_+77, b_+79); C = 0x20;
  CALL_C(b_+79, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+82);
  if (!(F & FZ)) { CYCT(b_+82, b_+85); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+82, b_+85);
  CYC(b_+85, b_+88); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(b_+88, interactionCodea5_updateMapleAngle_hook, b_+253, b_+91);
  if (!(F & FZ)) { CYCT(b_+91, b_+92); ret_effect(gb); return; } // ret nz
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+94); A = 0xff;
  CYC(b_+94, b_+97); mem_wr(gb, w1Companion_angle, A);
  CALL_C(b_+97, interactionIncState_hook, SYM(interactionIncState), b_+100);
  CALL_C(b_+100, objectSetInvisible_hook, SYM(objectSetInvisible), b_+103);
  CYC(b_+103, b_+105); A = 0x5e; // SND_GETSEED
  CALL_C(b_+105, playSound_b00_hook, SYM(playSound_b00), b_+108);
  CYC(b_+108, b_+111); SET_BC(0x070e); // TX_070e
  CYC(b_+111, b_+114); showText_hook(gb); return; // jp

state3:
  CALL_C(b_+114, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+117);
  CYC(b_+117, b_+120); SET_HL(w1Link_xh);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+121, b_+123); B = 0xf0;
  CYC(b_+123, b_+125); alu_cp(gb, 0x58);
  if (!(F & FC)) { CYCT(b_+125, b_+127); goto l_5c92; } // jr nc
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+129); B = 0x10;

l_5c92:
  CYC(b_+129, b_+130); alu_add(gb, B);
  CYC(b_+130, b_+132); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+132, b_+133); mem_wr(gb, DE, A);
  CYC(b_+133, b_+134); L = alu_dec8(gb, L);
  CYC(b_+134, b_+135); A = mem_rd(gb, HL); // [w1Link.yh]
  CYC(b_+135, b_+137); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+139); alu_xor(gb, A);
  CYC(b_+139, b_+142); mem_wr(gb, w1Companion_angle, A);
  CYC(b_+142, b_+145); interactionIncState_hook(gb); return; // jp

state4:
  CALL_C(b_+145, interactionCodea5_updateMapleAngle_hook, b_+253, b_+148);
  if (!(F & FZ)) { CYCT(b_+148, b_+149); ret_effect(gb); return; } // ret nz
  CYC(b_+148, b_+149);
  CYC(b_+149, b_+152); SET_HL(w1Companion_angle);
  CYC(b_+152, b_+154); A = 0xff;
  CYC(b_+154, b_+155); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+155, b_+156); A = mem_rd(gb, HL); // [w1Companion.direction]
  CYC(b_+156, b_+158); A = (uint8_t)(A ^ 0x02);
  CYC(b_+158, b_+159); H = alu_dec8(gb, H);
  CYC(b_+159, b_+160); mem_wr(gb, HL, A); // [w1Link.direction]
  CALL_C(b_+160, interactionIncState_hook, SYM(interactionIncState), b_+163);
  CYC(b_+163, b_+166); SET_BC(0x070f); // TX_070f
  CYC(b_+166, b_+169); showText_hook(gb); return; // jp

state5:
  CALL_C(b_+169, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+172);
  CYC(b_+172, b_+175); A = mem_rd(gb, w1Companion_direction);
  CYC(b_+175, b_+177); A = (uint8_t)(A ^ 0x02);
  CYC(b_+177, b_+179); A = (uint8_t)(A | (1 << 7));
  CYC(b_+179, b_+182); mem_wr(gb, w1Companion_direction, A);
  CALL_C(b_+182, interactionIncState_hook, SYM(interactionIncState), b_+185);
  CYC(b_+185, b_+188); SET_BC(0x0710); // TX_0710
  CYC(b_+188, b_+191); showText_hook(gb); return; // jp

state6:
  CALL_C(b_+191, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+194);
  CYC(b_+194, b_+197); A = mem_rd(gb, w1Companion_direction);
  CYC(b_+197, b_+199); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+199, b_+202); mem_wr(gb, w1Companion_direction, A);
  CYC(b_+202, b_+205); interactionIncState_hook(gb); return; // jp

state7:
  CYC(b_+205, b_+208); SET_BC((SYM(interactionCodeac) + 17)); // TREASURE_TRADEITEM, TRADEITEM_MAGIC_OAR
  CALL_C(b_+208, createTreasure_hook, SYM(createTreasure), b_+211);
  if (!(F & FZ)) { CYCT(b_+211, b_+212); ret_effect(gb); return; } // ret nz
  CYC(b_+211, b_+212);
  CYC(b_+212, b_+214); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+214, b_+216); A = 0x02;
  CYC(b_+216, b_+217); mem_wr(gb, DE, A);
  CYC(b_+217, b_+218); push_effect(gb, DE);
  CYC(b_+218, b_+221); SET_DE(w1Link_yh);
  CALL_C(b_+221, objectCopyPosition_rawAddress_hook, SYM(objectCopyPosition_rawAddress), b_+224);
  CYC(b_+224, b_+225); SET_DE(pop_effect(gb));
  CYC(b_+225, b_+228); interactionIncState_hook(gb); return; // jp

state8:
  CYC(b_+228, b_+230); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+230, b_+231); A = mem_rd(gb, DE);
  CYC(b_+231, b_+232); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+232, b_+234); goto l_5cfe; } // jr z
  CYC(b_+232, b_+234);
  CYC(b_+234, b_+235); A = alu_dec8(gb, A);
  CYC(b_+235, b_+236); mem_wr(gb, DE, A);
  RET(b_+236); return;

l_5cfe:
  CALL_C(b_+237, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+240);
  CYC(b_+240, b_+242); A = 0x01; // DISABLE_LINK
  CYC(b_+242, b_+245); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+245, b_+247); A = 0x02;
  CYC(b_+247, b_+250); mem_wr(gb, w1Companion_substate, A);
  CYC(b_+250, b_+253); interactionDelete_hook(gb); return; // jp
}
