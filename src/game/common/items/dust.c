#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t dust_jump_table(GB *gb) {
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

static void dust_set_oam_tile_from_animation(GB *gb) {
  BASE(itemCode1a);
  CYC(b_+121, b_+122); H = D;
  CYC(b_+122, b_+124); L = 0x21;
  CYC(b_+124, b_+125); A = mem_rd(gb, HL);
  CYC(b_+125, b_+126); B = A;
  CYC(b_+126, b_+128); alu_and(gb, 0x7f);
  CYC(b_+128, b_+130); L = 0x1d;
  CYC(b_+130, b_+131); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+131, b_+132); ret_effect(gb);
}

// itemCode1a@initializeNextDustCloud: when subid bit 0 is set, arms the next cloud (var30..var33)
// at Link's position unless the current one (var34) is still active. Identical in both games.
static void dust_initialize_next_cloud(GB *gb) {
  BASE(itemCode1a__initializeNextDustCloud);
  H = D;
  L = OBJ_SUBID;
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  L = OBJ_USE_TEXT_ID;
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) CYCT(b_+12, b_+14);
  else {
    CYC(b_+12, b_+14);
    L = OBJ_VAR34;
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+18); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
    CYC(b_+18, b_+19);
  }
  A = 0x80;
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_xor(gb, A);
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+27); A = mem_rd(gb, w1Link_yh);
  alu_add(gb, 0x05);
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+30, b_+33); A = mem_rd(gb, w1Link_xh);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+35); ret_effect(gb);
}

void itemCode1a_hook(GB *gb) {
  BASE(itemCode1a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dust_jump_table(gb));
    if (jt_ == b_+10) {
      CALL_C(b_+10, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+13);
      CALL_C(b_+13, itemIncSubstate_hook, SYM(itemIncSubstate), b_+16);
      CYC(b_+16, b_+19); SET_HL(w1Link_yh);
      CALL_C(b_+19, objectTakePosition_hook, SYM(objectTakePosition), b_+22);
      CYC(b_+22, b_+23); alu_xor(gb, A);
      CALL_C(b_+23, itemSetAnimation_hook, SYM(itemSetAnimation), b_+26);
      CYC(b_+26, b_+29); objectSetVisible80_hook(gb); return;
    }
    else if (jt_ == b_+29) {
      CALL_C(b_+29, itemAnimate_hook, SYM(itemAnimate), b_+32);
      CYC(b_+32, b_+35); push_effect(gb, b_+35);
      dust_set_oam_tile_from_animation(gb);
      CYC(b_+35, b_+36); A = mem_rd(gb, HL);
      CYC(b_+36, b_+37); A = alu_inc8(gb, A);
      CYC(b_+37, b_+39); alu_and(gb, 0xfb);
      CYC(b_+39, b_+41); alu_xor(gb, 0x60);
      CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(b_+42, b_+43); mem_wr(gb, HL, A);
      CYC(b_+43, b_+45); alu_bit(gb, 7, B);
      if (F & FZ) {
        CYCT(b_+45, b_+46); ret_effect(gb); return;
      }
      CYC(b_+45, b_+46);
      CYC(b_+46, b_+48); A = 0x0b;
      CYC(b_+48, b_+49); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+49, b_+50); mem_wr(gb, HL, A);
      CYC(b_+50, b_+52); L = 0x0e;
      CYC(b_+52, b_+53); alu_xor(gb, A);
      CYC(b_+53, b_+54); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+54, b_+55); mem_wr(gb, HL, A);
      CALL_C(b_+55, objectSetInvisible_hook, SYM(objectSetInvisible), b_+58);
      CYC(b_+58, b_+61); itemIncSubstate_hook(gb); return;
    }
    else if (jt_ == b_+61) {
      CALL_C(b_+61, checkPegasusSeedCounter_hook, SYM(checkPegasusSeedCounter), b_+64);
      if (F & FZ) {
        CYCT(b_+64, b_+67); itemDelete_hook(gb); return;
      }
      CYC(b_+64, b_+67);
      CYC(b_+67, b_+70); push_effect(gb, b_+70); dust_initialize_next_cloud(gb);
      CALL_C(b_+70, itemDecCounter1_hook, SYM(itemDecCounter1), b_+73);
      CYC(b_+73, b_+75); alu_bit(gb, 0, mem_rd(gb, HL));
      CYC(b_+75, b_+77); L = 0x30;
      if (F & FZ) CYCT(b_+77, b_+79);
      else {
        CYC(b_+77, b_+79);
        CYC(b_+79, b_+81); L = 0x34;
      }
      CYC(b_+81, b_+83); alu_bit(gb, 7, mem_rd(gb, HL));
      if (F & FZ) {
        CYCT(b_+83, b_+86); objectSetInvisible_hook(gb); return;
      }
      CYC(b_+83, b_+86);
      CYC(b_+86, b_+87); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+87, b_+88); A = mem_rd(gb, HL);
      CYC(b_+88, b_+90); alu_cp(gb, 0x82);
      if (F & FC) {
        CYCT(b_+90, b_+92);
        goto update_cloud;
      }
      CYC(b_+90, b_+92);
      CYC(b_+92, b_+94); mem_wr(gb, HL, 0x80);
      CYC(b_+94, b_+95); L = alu_inc8(gb, L);
      CYC(b_+95, b_+96); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+96, b_+97); A = mem_rd(gb, HL);
      CYC(b_+97, b_+98); L = alu_dec8(gb, L);
      CYC(b_+98, b_+100); alu_cp(gb, 0x03);
      if (!(F & FC)) {
        CYCT(b_+100, b_+102);
        goto clear_cloud;
      }
      CYC(b_+100, b_+102);

update_cloud:
      CYC(b_+102, b_+103); L = alu_inc8(gb, L);
      CYC(b_+103, b_+104); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+104, b_+105); A = alu_inc8(gb, A);
      CYC(b_+105, b_+106); C = A;
      CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+107, b_+109); E = 0x0b;
      CYC(b_+109, b_+110); mem_wr(gb, DE, A);
      CYC(b_+110, b_+111); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+111, b_+113); E = 0x0d;
      CYC(b_+113, b_+114); mem_wr(gb, DE, A);
      CYC(b_+114, b_+115); A = C;
      CALL_C(b_+115, itemSetAnimation_hook, SYM(itemSetAnimation), b_+118);
      CALL_C(b_+118, objectSetVisible80_hook, SYM(objectSetVisible80), b_+121);
      dust_set_oam_tile_from_animation(gb);
      return;

clear_cloud:
      CYC(b_+132, b_+133); alu_xor(gb, A);
      CYC(b_+133, b_+134); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+134, b_+135); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+135, b_+136); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+136, b_+137); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+137, b_+140); objectSetInvisible_hook(gb); return;
    }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
