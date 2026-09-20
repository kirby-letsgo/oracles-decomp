#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void sword_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void sword_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t sword_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void itemCode05_hook(GB *gb) {
  BASE(itemCode05);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemTransferKnockbackToLink_hook, SYM(itemTransferKnockbackToLink), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (sword_jump_table(gb));
    if (jt_ == b_+29) {
      CYC(b_+29, b_+31); A = 0x1a;
      CALL_C(b_+31, loadWeaponGfx_b00_hook, SYM(loadWeaponGfx_b00), b_+34);
      CALL_C(b_+34, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+37);
      CYC(b_+37, b_+39); alu_and(gb, 0x07);
      CYC(b_+39, b_+42); SET_HL(b_+21);
      CYC(b_+42, b_+43); sword_add_a_to_hl(gb, b_+43);
      CYC(b_+43, b_+44); A = mem_rd(gb, HL);
      CALL_C(b_+44, playSound_b00_hook, SYM(playSound_b00), b_+47);
      CYC(b_+47, b_+49); E = 0x31;
      CYC(b_+49, b_+50); alu_xor(gb, A);
      CYC(b_+50, b_+51); mem_wr(gb, DE, A);
      goto jt_fall4;
    }
    else if (jt_ == b_+51) {
      jt_fall4:;
      CALL_C(b_+51, loadAttributesAndGraphicsAndIncState_hook, SYM(loadAttributesAndGraphicsAndIncState), b_+54);
      CYC(b_+54, b_+57); A = W8(wSwordLevel);
      CYC(b_+57, b_+60); SET_HL(b_+106);
      CYC(b_+60, b_+61); sword_add_double_index(gb, b_+61);
      CYC(b_+61, b_+63); E = 0x24;
      CYC(b_+63, b_+64); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+64, b_+65); mem_wr(gb, DE, A);
      CYC(b_+65, b_+66); C = mem_rd(gb, HL);
      CYC(b_+66, b_+68); E = 0x31;
      CYC(b_+68, b_+69); A = mem_rd(gb, DE);
      CYC(b_+69, b_+70); alu_or(gb, A);
      CYC(b_+70, b_+71); A = C;
      CYC(b_+71, b_+72); mem_wr(gb, DE, A);
      if (!(F & FZ)) {
        CYCT(b_+72, b_+74);
        goto set_damage;
      }
      CYC(b_+72, b_+74);
      CYC(b_+74, b_+76); A = 0x3e;
      CALL_C(b_+76, cpActiveRing_hook, SYM(cpActiveRing), b_+79);
      if (!(F & FZ)) {
        CYCT(b_+79, b_+81);
        goto set_damage;
      }
      CYC(b_+79, b_+81);
      CALL_C(b_+81, getRandomNumber_hook, SYM(getRandomNumber), b_+84);
      CYC(b_+84, b_+85); alu_or(gb, A);
      CYC(b_+85, b_+87); C = 0xff;
      if (!(F & FZ)) {
        CYCT(b_+87, b_+89);
        goto set_damage;
      }
      CYC(b_+87, b_+89);
      CYC(b_+89, b_+91); A = 0xd2;
      CALL_C(b_+91, playSound_b00_hook, SYM(playSound_b00), b_+94);
      CYC(b_+94, b_+96); C = 0xf4;
set_damage:
      CYC(b_+96, b_+98); E = 0x3a;
      CYC(b_+98, b_+99); A = C;
      CYC(b_+99, b_+100); mem_wr(gb, DE, A);
      CYC(b_+100, b_+102); E = 0x04;
      CYC(b_+102, b_+104); A = 0x01;
      CYC(b_+104, b_+105); mem_wr(gb, DE, A);
      CYC(b_+105, b_+108); objectSetVisible82_hook(gb); return;
    }
    else if (jt_ == b_+114) {
      CYC(b_+114, b_+116); E = 0x24;
      CYC(b_+116, b_+118); A = 0x88;
      CYC(b_+118, b_+119); mem_wr(gb, DE, A);
      goto jt_fall5;
    }
    else if (jt_ == b_+119) {
      jt_fall5:;
      CYC(b_+119, b_+120); H = D;
      CYC(b_+120, b_+122); L = 0x1b;
      CYC(b_+122, b_+123); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+123, b_+124); mem_wr(gb, HL, A);
      CYC(b_+124, b_+125); ret_effect(gb); return;
    }
    else if (jt_ == b_+125) {
      CYC(b_+125, b_+127); E = 0x31;
      CYC(b_+127, b_+128); A = mem_rd(gb, DE);
      CYC(b_+128, b_+130); E = 0x3a;
      CYC(b_+130, b_+131); mem_wr(gb, DE, A);
      CYC(b_+131, b_+132); ret_effect(gb); return;
    }
    else if (jt_ == b_+132) {
      CYC(b_+132, b_+133); H = D;
      CYC(b_+133, b_+135); L = 0x06;
      CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+136, b_+138); alu_bit(gb, 2, mem_rd(gb, HL));
      CYC(b_+138, b_+140); L = 0x1b;
      CYC(b_+140, b_+141); A = mem_rd(gb, HL); SET_HL(HL + 1);
      if (!(F & FZ)) CYCT(b_+141, b_+143);
      else {
        CYC(b_+141, b_+143);
        CYC(b_+143, b_+145); A = 0x0d;
      }
      CYC(b_+145, b_+146); mem_wr(gb, HL, A);
      CYC(b_+146, b_+147); ret_effect(gb); return;
    }
    else if (jt_ == b_+147) {
      CYC(b_+147, b_+149); A = 0x08;
      CALL_C(b_+149, tryBreakTileWithSword_calculateLevel_hook, SYM(tryBreakTileWithSword_calculateLevel), b_+152);
      CYC(b_+152, b_+155); itemDelete_hook(gb); return;
    }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t bank09_jump_table_sword(GB *gb) {
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

void interactionCode5e__afterCall6e68_hook(GB *gb) {
  BASE(interactionCode5e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+19, objectSetInvisible_hook, SYM(objectSetInvisible), b_+22);
  CYC(b_+22, b_+24); A = 0x00;
  CALL_C(b_+24, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+27);
  CYC(b_+27, b_+29); L = 0x7f;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); A = alu_inc8(gb, A);
  CYC(b_+31, b_+33); L = 0x40;
  CYC(b_+33, b_+34); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+34, b_+37); interactionDelete_hook(gb); return;
  }
  CYC(b_+34, b_+37);
  CYC(b_+37, b_+39); L = 0x61;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); B = A;
  CYC(b_+41, b_+43); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+43, b_+44); ret_effect(gb); return;
  }
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+45); A = B;
  CYC(b_+45, b_+47); alu_and(gb, 0x7f);
  PUSH(b_+47, HL);
  CYC(b_+48, b_+49); H = D;
  CYC(b_+49, b_+51); L = 0x77;
  CYC(b_+51, b_+52); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+52, b_+54); goto L_6e8f;
  }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+55); mem_wr(gb, HL, A);
  CALL_C(b_+55, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+58);
L_6e8f:
  SET_HL(POP(b_+58));
  CALL_C(b_+59, objectTakePosition_hook, SYM(objectTakePosition), b_+62);
  CYC(b_+62, b_+65); objectSetVisible83_hook(gb); return;
}

void interactionCode5e_hook(GB *gb) {
  BASE(interactionCode5e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  SET_HL(bank09_jump_table_sword(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+8) { goto L_6e5d; }
    else if (jt_ == b_+19) { goto L_6e68; }
    else { HANDOFF(HL); }
  } while (0);
L_6e5d:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0xff;
  CYC(b_+13, b_+15); E = 0x77;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CALL_C(b_+16, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+19);
  interactionCode5e__afterCall6e68_hook(gb);
  return;
L_6e68:
  interactionCode5e__afterCall6e68_hook(gb);
}
