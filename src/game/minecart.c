#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static void minecart_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static uint16_t minecart_jump_table(GB *gb) {
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

void specialObjectCode_minecart_b06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x563e, minecartCreateCollisionItem_hook, 0x57dd, 0x5641);
  CYC(0x5641, 0x5643); E = 0x04;
  CYC(0x5643, 0x5644); A = mem_rd(gb, DE);
  CYC(0x5644, 0x5645); push_effect(gb, 0x5645);
  switch (minecart_jump_table(gb)) {
    case 0x5649:
      break;
    case 0x5675:
      goto state1;
    default:
      hook_continue(gb, HL, sp0_);
      return;
  }

  CYC(0x5649, 0x564b); A = 0x01;
  CYC(0x564b, 0x564c); mem_wr(gb, DE, A);
  CYC(0x564c, 0x564f); SET_HL(0x41f7);
  CYC(0x564f, 0x5651); E = 0x05;
  CALL_C(0x5651, interBankCall_hook, 0x008a, 0x5654);
  CYC(0x5654, 0x5655); H = D;
  CYC(0x5655, 0x5657); L = 0x10;
  CYC(0x5657, 0x5659); mem_wr(gb, HL, 0x28);
  CYC(0x5659, 0x565b); L = 0x08;
  CYC(0x565b, 0x565c); A = mem_rd(gb, HL);
  CALL_C(0x565c, specialObjectSetAnimation_hook, 0x2b0a, 0x565f);
  CYC(0x565f, 0x5660); A = D;
  CYC(0x5660, 0x5663); W8(wLinkObjectIndex) = A;
  CALL_C(0x5663, setCameraFocusedObjectToLink_hook, 0x12f0, 0x5666);
  CALL_C(0x5666, clearVar3fForParentItems_hook, 0x2c72, 0x5669);
  CALL_C(0x5669, clearPegasusSeedCounter_hook, 0x2a85, 0x566c);
  CYC(0x566c, 0x566f); SET_HL(w1Link_z);
  CYC(0x566f, 0x5670); alu_xor(gb, A);
  CYC(0x5670, 0x5671); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5671, 0x5672); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5672, 0x5675);
  objectSetVisiblec2_hook(gb);
  return;

state1:
  CYC(0x5675, 0x5678); A = W8(wPaletteThread_mode);
  CYC(0x5678, 0x5679); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5679, 0x567a); ret_effect(gb); return;
  }
  CYC(0x5679, 0x567a);
  CALL_C(0x567a, retIfTextIsActive_hook, 0x1859, 0x567d);
  CYC(0x567d, 0x5680); A = W8(wScrollMode);
  CYC(0x5680, 0x5682); alu_and(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x5682, 0x5683); ret_effect(gb); return;
  }
  CYC(0x5682, 0x5683);
  CYC(0x5683, 0x5686); A = W8(wDisabledObjects);
  CYC(0x5686, 0x5688); alu_and(gb, 0x81);
  if (!(F & FZ)) {
    CYCT(0x5688, 0x5689); ret_effect(gb); return;
  }
  CYC(0x5688, 0x5689);
  CYC(0x5689, 0x568c); SET_HL(w1Link_collisionType);
  CYC(0x568c, 0x568e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x568e, 0x568f); alu_xor(gb, A);
  CYC(0x568f, 0x5691); L = 0x2d;
  CYC(0x5691, 0x5692); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5692, 0x5693); H = D;
  CYC(0x5693, 0x5695); L = 0x0b;
  CYC(0x5695, 0x5696); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5696, 0x5697); B = A;
  CYC(0x5697, 0x5699); alu_and(gb, 0x0f);
  CYC(0x5699, 0x569b); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x569b, 0x569d);
    goto animate;
  }
  CYC(0x569b, 0x569d);
  CYC(0x569d, 0x569e); L = alu_inc8(gb, L);
  CYC(0x569e, 0x569f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x569f, 0x56a0); C = A;
  CYC(0x56a0, 0x56a2); alu_and(gb, 0x0f);
  CYC(0x56a2, 0x56a4); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x56a4, 0x56a6);
    goto animate;
  }
  CYC(0x56a4, 0x56a6);
  CALL_C(0x56a6, minecartCheckCollisions_hook, 0x570c, 0x56a9);
  if (F & FC) {
    CYCT(0x56a9, 0x56ab);
    goto minecart_stopped;
  }
  CYC(0x56a9, 0x56ab);
  CYC(0x56ab, 0x56ac); H = D;
  CYC(0x56ac, 0x56ae); L = 0x08;
  CYC(0x56ae, 0x56af); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x56af, 0x56b1); A = alu_swap(gb, A);
  CYC(0x56b1, 0x56b2); alu_rrca(gb);
  CYC(0x56b2, 0x56b3); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x56b3, 0x56b5);
    goto animate;
  }
  CYC(0x56b3, 0x56b5);
  CYC(0x56b5, 0x56b6); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x56b6, 0x56b7); A = mem_rd(gb, HL);
  CALL_C(0x56b7, specialObjectSetAnimation_hook, 0x2b0a, 0x56ba);

animate:
  CYC(0x56ba, 0x56bb); H = D;
  CYC(0x56bb, 0x56bd); L = 0x35;
  CYC(0x56bd, 0x56be); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x56be, 0x56c0); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYC(0x56c0, 0x56c2);
    CYC(0x56c2, 0x56c4); mem_wr(gb, HL, 0x1a);
    CYC(0x56c4, 0x56c6); A = 0x80;
    CALL_C(0x56c6, playSound_b00_hook, 0x0c98, 0x56c9);
  } else {
    CYCT(0x56c0, 0x56c2);
  }
  CALL_C(0x56c9, objectApplySpeed_hook, 0x201d, 0x56cc);
  CYC(0x56cc, 0x56cf);
  specialObjectAnimate_hook(gb);
  return;

minecart_stopped:
  CYC(0x56cf, 0x56d1); E = 0x04;
  CYC(0x56d1, 0x56d3); A = 0x02;
  CYC(0x56d3, 0x56d4); mem_wr(gb, DE, A);
  CALL_C(0x56d4, clearVar3fForParentItems_hook, 0x2c72, 0x56d7);
  CYC(0x56d7, 0x56d9); A = 0x81;
  CYC(0x56d9, 0x56dc); W8(wLinkInAir) = A;
  CYC(0x56dc, 0x56df); SET_HL(w1Link_angle);
  CYC(0x56df, 0x56e1); E = 0x09;
  CYC(0x56e1, 0x56e2); A = mem_rd(gb, DE);
  CYC(0x56e2, 0x56e3); mem_wr(gb, HL, A);
  CYC(0x56e3, 0x56e5); L = 0x0b;
  CYC(0x56e5, 0x56e6); A = mem_rd(gb, HL);
  CYC(0x56e6, 0x56e8); alu_add(gb, 0x06);
  CYC(0x56e8, 0x56e9); mem_wr(gb, HL, A);
  CYC(0x56e9, 0x56eb); L = 0x0f;
  CYC(0x56eb, 0x56ed); mem_wr(gb, HL, 0xfa);
  CYC(0x56ed, 0x56ef); L = 0x10;
  CYC(0x56ef, 0x56f1); mem_wr(gb, HL, 0x14);
  CYC(0x56f1, 0x56f3); L = 0x14;
  CYC(0x56f3, 0x56f5); mem_wr(gb, HL, 0x40);
  CYC(0x56f5, 0x56f6); L = alu_inc8(gb, L);
  CYC(0x56f6, 0x56f8); mem_wr(gb, HL, 0xfe);
  CYC(0x56f8, 0x56fa); L = 0x1a;
  CYC(0x56fa, 0x56fc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(0x56fc, 0x56fe); A = 0xd0;
  CYC(0x56fe, 0x5701); W8(wLinkObjectIndex) = A;
  CALL_C(0x5701, setCameraFocusedObjectToLink_hook, 0x12f0, 0x5704);
  CYC(0x5704, 0x5706); B = 0x16;
  CALL_C(0x5706, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x5709);
  CYC(0x5709, 0x570c);
  objectDelete_useActiveObjectType_hook(gb);
}

static void minecart_check_door(GB *gb, uint16_t sp0_) {
  CYC(0x57c3, 0x57c4); A = C;
  CYC(0x57c4, 0x57c6); alu_sub(gb, 0x7c);
  CYC(0x57c6, 0x57c8); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(0x57c8, 0x57c9); ret_effect(gb); return;
  }
  CYC(0x57c8, 0x57c9);
  CYC(0x57c9, 0x57cb); alu_add(gb, 0x0c);
  CYC(0x57cb, 0x57cc); alu_add(gb, A);
  CYC(0x57cc, 0x57cd); B = A;
  CALL_C(0x57cd, getFreeInteractionSlot_hook, 0x3aef, 0x57d0);
  if (!(F & FZ)) {
    CYCT(0x57d0, 0x57d1); ret_effect(gb); return;
  }
  CYC(0x57d0, 0x57d1);
  CYC(0x57d1, 0x57d3); mem_wr(gb, HL, 0x1e);
  CYC(0x57d3, 0x57d5); L = 0x49;
  CYC(0x57d5, 0x57d6); mem_wr(gb, HL, B);
  CYC(0x57d6, 0x57d8); L = 0x4b;
  CYC(0x57d8, 0x57da); A = H8(hFF8B);
  CYC(0x57da, 0x57db); mem_wr(gb, HL, A);
  CYC(0x57db, 0x57dc); alu_scf(gb);
  CYC(0x57dc, 0x57dd); ret_effect(gb);
}

void minecartCheckCollisions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x570c, getTileAtPosition_hook, 0x1447, 0x570f);
  CYC(0x570f, 0x5710); E = A;
  CYC(0x5710, 0x5711); C = L;
  CYC(0x5711, 0x5712); H = D;
  CYC(0x5712, 0x5714); L = 0x08;
  CYC(0x5714, 0x5715); A = mem_rd(gb, HL);
  CYC(0x5715, 0x5717); A = alu_swap(gb, A);
  CYC(0x5717, 0x571a); SET_HL(0x5783);
  CYC(0x571a, 0x571b); minecart_add_a_to_hl_from_rst(gb, 0x571b);

find_track:
  CYC(0x571b, 0x571c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x571c, 0x571d); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x571d, 0x571f);
    goto no_track_found;
  }
  CYC(0x571d, 0x571f);
  CYC(0x571f, 0x5720); alu_cp(gb, E);
  if (F & FZ) {
    CYCT(0x5720, 0x5722);
    goto track_found;
  }
  CYC(0x5720, 0x5722);
  CYC(0x5722, 0x5724); A = 0x04;
  CYC(0x5724, 0x5725); minecart_add_a_to_hl_from_rst(gb, 0x5725);
  CYC(0x5725, 0x5727);
  goto find_track;

track_found:
  CYC(0x5727, 0x5728); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5728, 0x5729); alu_add(gb, C);
  CYC(0x5729, 0x572a); C = A;
  CYC(0x572a, 0x572c); H8(hFF8B) = A;
  CYC(0x572c, 0x572e); B = 0xce;
  CYC(0x572e, 0x572f); A = mem_rd(gb, BC);
  CYC(0x572f, 0x5731); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x5731, 0x5732); ret_effect(gb); return;
  }
  CYC(0x5731, 0x5732);
  CYC(0x5732, 0x5734); B = 0xcf;
  CYC(0x5734, 0x5735); A = mem_rd(gb, BC);
  CYC(0x5735, 0x5737); alu_cp(gb, 0x5f);
  if (F & FZ) {
    CYCT(0x5737, 0x5739);
    goto stop_minecart;
  }
  CYC(0x5737, 0x5739);
  CYC(0x5739, 0x573a); C = A;
  CYC(0x573a, 0x573c); B = 0x03;

check_linked_track:
  CYC(0x573c, 0x573d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x573d, 0x573e); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x573e, 0x5740);
    goto update_direction;
  }
  CYC(0x573e, 0x5740);
  CYC(0x5740, 0x5741); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5741, 0x5743);
    goto check_linked_track;
  }
  CYC(0x5741, 0x5743);
  CYC(0x5743, 0x5745);
  goto no_track_found;

stop_minecart:
  CYC(0x5745, 0x5746); alu_scf(gb);
  CYC(0x5746, 0x5747); ret_effect(gb);
  return;

update_direction:
  CYC(0x5747, 0x5748); A = E;
  CYC(0x5748, 0x574a); alu_sub(gb, 0x59);
  CYC(0x574a, 0x574c); alu_cp(gb, 0x06);
  if (F & FC) {
    CYCT(0x574c, 0x574e);
    goto dispatch_track;
  }
  CYC(0x574c, 0x574e);

no_track_found:
  CYC(0x574e, 0x5750); A = 0x06;

dispatch_track:
  CYC(0x5750, 0x5752); E = 0x08;
  CYC(0x5752, 0x5753); push_effect(gb, 0x5753);
  switch (minecart_jump_table(gb)) {
    case 0x5761:
      CYC(0x5761, 0x5762); A = mem_rd(gb, DE);
      CYC(0x5762, 0x5764); alu_xor(gb, 0x01);
      CYC(0x5764, 0x5765); mem_wr(gb, DE, A);
      CYC(0x5765, 0x5766); ret_effect(gb);
      return;
    case 0x5766:
      CYC(0x5766, 0x5767); A = mem_rd(gb, DE);
      CYC(0x5767, 0x5769); alu_xor(gb, 0x03);
      CYC(0x5769, 0x576a); mem_wr(gb, DE, A);
      CYC(0x576a, 0x576b); ret_effect(gb);
      return;
    case 0x576b:
      CYC(0x576b, 0x576c); A = mem_rd(gb, DE);
      CYC(0x576c, 0x576e); alu_and(gb, 0x02);
      CYC(0x576e, 0x5770); alu_or(gb, 0x01);
      CYC(0x5770, 0x5771); mem_wr(gb, DE, A);
      CYC(0x5771, 0x5772); ret_effect(gb);
      return;
    case 0x5772:
      CYC(0x5772, 0x5773); A = mem_rd(gb, DE);
      CYC(0x5773, 0x5775); alu_and(gb, 0x02);
      CYC(0x5775, 0x5776); mem_wr(gb, DE, A);
      CYC(0x5776, 0x5777); ret_effect(gb);
      return;
    case 0x5777:
      CYC(0x5777, 0x577a); push_effect(gb, 0x577a);
      minecart_check_door(gb, gb->sp);
      if (!(F & FC)) {
        CYCT(0x577a, 0x577c);
        goto reverse_direction;
      }
      CYC(0x577a, 0x577c);
      CYC(0x577c, 0x577d); alu_xor(gb, A);
      CYC(0x577d, 0x577e); ret_effect(gb);
      return;
    default:
      hook_continue(gb, HL, sp0_);
      return;
  }

reverse_direction:
  CYC(0x577e, 0x577f); A = mem_rd(gb, DE);
  CYC(0x577f, 0x5781); alu_xor(gb, 0x02);
  CYC(0x5781, 0x5782); mem_wr(gb, DE, A);
  CYC(0x5782, 0x5783); ret_effect(gb);
}

void minecartCreateCollisionItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x57dd, 0x57df); E = 0x36;
  CYC(0x57df, 0x57e0); A = mem_rd(gb, DE);
  CYC(0x57e0, 0x57e1); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x57e1, 0x57e2); ret_effect(gb); return;
  }
  CYC(0x57e1, 0x57e2);
  CALL_C(0x57e2, getFreeItemSlot_hook, 0x2cf9, 0x57e5);
  if (!(F & FZ)) {
    CYCT(0x57e5, 0x57e6); ret_effect(gb); return;
  }
  CYC(0x57e5, 0x57e6);
  CYC(0x57e6, 0x57e8); E = 0x36;
  CYC(0x57e8, 0x57ea); A = 0x01;
  CYC(0x57ea, 0x57eb); mem_wr(gb, DE, A);
  CYC(0x57eb, 0x57ec); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x57ec, 0x57ee); mem_wr(gb, HL, 0x1d);
  CYC(0x57ee, 0x57ef); ret_effect(gb);
}
