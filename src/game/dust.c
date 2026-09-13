#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

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
  CYC(0x657d, 0x657e); H = D;
  CYC(0x657e, 0x6580); L = 0x21;
  CYC(0x6580, 0x6581); A = mem_rd(gb, HL);
  CYC(0x6581, 0x6582); B = A;
  CYC(0x6582, 0x6584); alu_and(gb, 0x7f);
  CYC(0x6584, 0x6586); L = 0x1d;
  CYC(0x6586, 0x6587); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x6587, 0x6588); ret_effect(gb);
}

void itemCode1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6504, 0x6506); E = 0x05;
  CYC(0x6506, 0x6507); A = mem_rd(gb, DE);
  CYC(0x6507, 0x6508); push_effect(gb, 0x6508);
  switch (dust_jump_table(gb)) {
    case 0x650e:
      CALL_C(0x650e, itemLoadAttributesAndGraphics_hook, 0x4993, 0x6511);
      CALL_C(0x6511, itemIncSubstate_hook, 0x23ef, 0x6514);
      CYC(0x6514, 0x6517); SET_HL(w1Link_yh);
      CALL_C(0x6517, objectTakePosition_hook, 0x2274, 0x651a);
      CYC(0x651a, 0x651b); alu_xor(gb, A);
      CALL_C(0x651b, itemSetAnimation_hook, 0x49e2, 0x651e);
      CYC(0x651e, 0x6521); objectSetVisible80_hook(gb); return;
    case 0x6521:
      CALL_C(0x6521, itemAnimate_hook, 0x49d9, 0x6524);
      CYC(0x6524, 0x6527); push_effect(gb, 0x6527);
      dust_set_oam_tile_from_animation(gb);
      CYC(0x6527, 0x6528); A = mem_rd(gb, HL);
      CYC(0x6528, 0x6529); A = alu_inc8(gb, A);
      CYC(0x6529, 0x652b); alu_and(gb, 0xfb);
      CYC(0x652b, 0x652d); alu_xor(gb, 0x60);
      CYC(0x652d, 0x652e); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(0x652e, 0x652f); mem_wr(gb, HL, A);
      CYC(0x652f, 0x6531); alu_bit(gb, 7, B);
      if (F & FZ) {
        CYCT(0x6531, 0x6532); ret_effect(gb); return;
      }
      CYC(0x6531, 0x6532);
      CYC(0x6532, 0x6534); A = 0x0b;
      CYC(0x6534, 0x6535); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x6535, 0x6536); mem_wr(gb, HL, A);
      CYC(0x6536, 0x6538); L = 0x0e;
      CYC(0x6538, 0x6539); alu_xor(gb, A);
      CYC(0x6539, 0x653a); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x653a, 0x653b); mem_wr(gb, HL, A);
      CALL_C(0x653b, objectSetInvisible_hook, 0x1e7b, 0x653e);
      CYC(0x653e, 0x6541); itemIncSubstate_hook(gb); return;
    case 0x6541:
      CALL_C(0x6541, checkPegasusSeedCounter_hook, 0x2be8, 0x6544);
      if (F & FZ) {
        CYCT(0x6544, 0x6547); itemDelete_hook(gb); return;
      }
      CYC(0x6544, 0x6547);
      CALL_ROM(0x6547, 0x6590);
      CALL_C(0x654a, itemDecCounter1_hook, 0x23d6, 0x654d);
      CYC(0x654d, 0x654f); alu_bit(gb, 0, mem_rd(gb, HL));
      CYC(0x654f, 0x6551); L = 0x30;
      if (F & FZ) CYCT(0x6551, 0x6553);
      else {
        CYC(0x6551, 0x6553);
        CYC(0x6553, 0x6555); L = 0x34;
      }
      CYC(0x6555, 0x6557); alu_bit(gb, 7, mem_rd(gb, HL));
      if (F & FZ) {
        CYCT(0x6557, 0x655a); objectSetInvisible_hook(gb); return;
      }
      CYC(0x6557, 0x655a);
      CYC(0x655a, 0x655b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(0x655b, 0x655c); A = mem_rd(gb, HL);
      CYC(0x655c, 0x655e); alu_cp(gb, 0x82);
      if (F & FC) {
        CYCT(0x655e, 0x6560);
        goto update_cloud;
      }
      CYC(0x655e, 0x6560);
      CYC(0x6560, 0x6562); mem_wr(gb, HL, 0x80);
      CYC(0x6562, 0x6563); L = alu_inc8(gb, L);
      CYC(0x6563, 0x6564); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(0x6564, 0x6565); A = mem_rd(gb, HL);
      CYC(0x6565, 0x6566); L = alu_dec8(gb, L);
      CYC(0x6566, 0x6568); alu_cp(gb, 0x03);
      if (!(F & FC)) {
        CYCT(0x6568, 0x656a);
        goto clear_cloud;
      }
      CYC(0x6568, 0x656a);

update_cloud:
      CYC(0x656a, 0x656b); L = alu_inc8(gb, L);
      CYC(0x656b, 0x656c); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x656c, 0x656d); A = alu_inc8(gb, A);
      CYC(0x656d, 0x656e); C = A;
      CYC(0x656e, 0x656f); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x656f, 0x6571); E = 0x0b;
      CYC(0x6571, 0x6572); mem_wr(gb, DE, A);
      CYC(0x6572, 0x6573); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x6573, 0x6575); E = 0x0d;
      CYC(0x6575, 0x6576); mem_wr(gb, DE, A);
      CYC(0x6576, 0x6577); A = C;
      CALL_C(0x6577, itemSetAnimation_hook, 0x49e2, 0x657a);
      CALL_C(0x657a, objectSetVisible80_hook, 0x1e57, 0x657d);
      dust_set_oam_tile_from_animation(gb);
      return;

clear_cloud:
      CYC(0x6588, 0x6589); alu_xor(gb, A);
      CYC(0x6589, 0x658a); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x658a, 0x658b); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x658b, 0x658c); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x658c, 0x658d); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x658d, 0x6590); objectSetInvisible_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
