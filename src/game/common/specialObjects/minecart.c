#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(specialObjectCode_minecart_b06), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(specialObjectCode_minecart_b06), (from), (to), true)

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
  BASE(specialObjectCode_minecart_b06);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, minecartCreateCollisionItem_hook, SYM(minecartCreateCollisionItem), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (minecart_jump_table(gb));
    if (jt_ == b_+11) { break; }
    else if (jt_ == b_+55) { goto state1; }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);

  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+17); SET_HL((SYM(nextToKeyDoor) + 49));
  CYC(b_+17, b_+19); E = 0x05;
  CALL_C(b_+19, interBankCall_hook, 0x008a, b_+22);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = 0x10;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x28);
  CYC(b_+27, b_+29); L = 0x08;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CALL_C(b_+30, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+33);
  CYC(b_+33, b_+34); A = D;
  CYC(b_+34, b_+37); W8(wLinkObjectIndex) = A;
  CALL_C(b_+37, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+40);
  CALL_C(b_+40, clearVar3fForParentItems_hook, SYM(clearVar3fForParentItems), b_+43);
  CALL_C(b_+43, clearPegasusSeedCounter_hook, SYM(clearPegasusSeedCounter), b_+46);
  CYC(b_+46, b_+49); SET_HL(w1Link_z);
  CYC(b_+49, b_+50); alu_xor(gb, A);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+52, b_+55);
  objectSetVisiblec2_hook(gb);
  return;

state1:
  CYC(b_+55, b_+58); A = W8(wPaletteThread_mode);
  CYC(b_+58, b_+59); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+59, b_+60); ret_effect(gb); return;
  }
  CYC(b_+59, b_+60);
  CALL_C(b_+60, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+63);
  CYC(b_+63, b_+66); A = W8(wScrollMode);
  CYC(b_+66, b_+68); alu_and(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+68, b_+69); ret_effect(gb); return;
  }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+72); A = W8(wDisabledObjects);
  CYC(b_+72, b_+74); alu_and(gb, 0x81);
  if (!(F & FZ)) {
    CYCT(b_+74, b_+75); ret_effect(gb); return;
  }
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+78); SET_HL(w1Link_collisionType);
  CYC(b_+78, b_+80); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+80, b_+81); alu_xor(gb, A);
  CYC(b_+81, b_+83); L = 0x2d;
  CYC(b_+83, b_+84); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+84, b_+85); H = D;
  CYC(b_+85, b_+87); L = 0x0b;
  CYC(b_+87, b_+88); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+88, b_+89); B = A;
  CYC(b_+89, b_+91); alu_and(gb, 0x0f);
  CYC(b_+91, b_+93); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+93, b_+95);
    goto animate;
  }
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+96); L = alu_inc8(gb, L);
  CYC(b_+96, b_+97); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+97, b_+98); C = A;
  CYC(b_+98, b_+100); alu_and(gb, 0x0f);
  CYC(b_+100, b_+102); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+102, b_+104);
    goto animate;
  }
  CYC(b_+102, b_+104);
  CALL_C(b_+104, minecartCheckCollisions_hook, SYM(minecartCheckCollisions), b_+107);
  if (F & FC) {
    CYCT(b_+107, b_+109);
    goto minecart_stopped;
  }
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+110); H = D;
  CYC(b_+110, b_+112); L = 0x08;
  CYC(b_+112, b_+113); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+113, b_+115); A = alu_swap(gb, A);
  CYC(b_+115, b_+116); alu_rrca(gb);
  CYC(b_+116, b_+117); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+117, b_+119);
    goto animate;
  }
  CYC(b_+117, b_+119);
  CYC(b_+119, b_+120); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL);
  CALL_C(b_+121, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+124);

animate:
  CYC(b_+124, b_+125); H = D;
  CYC(b_+125, b_+127); L = 0x35;
  CYC(b_+127, b_+128); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+128, b_+130); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYC(b_+130, b_+132);
    CYC(b_+132, b_+134); mem_wr(gb, HL, 0x1a);
    CYC(b_+134, b_+136); A = 0x80;
    CALL_C(b_+136, playSound_b00_hook, SYM(playSound_b00), b_+139);
  } else {
    CYCT(b_+130, b_+132);
  }
  CALL_C(b_+139, objectApplySpeed_hook, SYM(objectApplySpeed), b_+142);
  CYC(b_+142, b_+145);
  specialObjectAnimate_hook(gb);
  return;

minecart_stopped:
  CYC(b_+145, b_+147); E = 0x04;
  CYC(b_+147, b_+149); A = 0x02;
  CYC(b_+149, b_+150); mem_wr(gb, DE, A);
  CALL_C(b_+150, clearVar3fForParentItems_hook, SYM(clearVar3fForParentItems), b_+153);
  CYC(b_+153, b_+155); A = 0x81;
  CYC(b_+155, b_+158); W8(wLinkInAir) = A;
  CYC(b_+158, b_+161); SET_HL(w1Link_angle);
  CYC(b_+161, b_+163); E = 0x09;
  CYC(b_+163, b_+164); A = mem_rd(gb, DE);
  CYC(b_+164, b_+165); mem_wr(gb, HL, A);
  CYC(b_+165, b_+167); L = 0x0b;
  CYC(b_+167, b_+168); A = mem_rd(gb, HL);
  CYC(b_+168, b_+170); alu_add(gb, 0x06);
  CYC(b_+170, b_+171); mem_wr(gb, HL, A);
  CYC(b_+171, b_+173); L = 0x0f;
  CYC(b_+173, b_+175); mem_wr(gb, HL, 0xfa);
  CYC(b_+175, b_+177); L = 0x10;
  CYC(b_+177, b_+179); mem_wr(gb, HL, 0x14);
  CYC(b_+179, b_+181); L = 0x14;
  CYC(b_+181, b_+183); mem_wr(gb, HL, 0x40);
  CYC(b_+183, b_+184); L = alu_inc8(gb, L);
  CYC(b_+184, b_+186); mem_wr(gb, HL, 0xfe);
  CYC(b_+186, b_+188); L = 0x1a;
  CYC(b_+188, b_+190); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(b_+190, b_+192); A = 0xd0;
  CYC(b_+192, b_+195); W8(wLinkObjectIndex) = A;
  CALL_C(b_+195, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+198);
  CYC(b_+198, b_+200); B = 0x16;
  CALL_C(b_+200, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+203);
  CYC(b_+203, SYM(minecartCheckCollisions));
  objectDelete_useActiveObjectType_hook(gb);
}

static void minecart_check_door(GB *gb, uint16_t sp0_) {
  BASE(minecartCheckCollisions);
  CYC(b_+183, b_+184); A = C;
  CYC(b_+184, b_+186); alu_sub(gb, 0x7c);
  CYC(b_+186, b_+188); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(b_+188, b_+189); ret_effect(gb); return;
  }
  CYC(b_+188, b_+189);
  CYC(b_+189, b_+191); alu_add(gb, 0x0c);
  CYC(b_+191, b_+192); alu_add(gb, A);
  CYC(b_+192, b_+193); B = A;
  CALL_C(b_+193, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+196);
  if (!(F & FZ)) {
    CYCT(b_+196, b_+197); ret_effect(gb); return;
  }
  CYC(b_+196, b_+197);
  CYC(b_+197, b_+199); mem_wr(gb, HL, 0x1e);
  CYC(b_+199, b_+201); L = 0x49;
  CYC(b_+201, b_+202); mem_wr(gb, HL, B);
  CYC(b_+202, b_+204); L = 0x4b;
  CYC(b_+204, b_+206); A = H8(hFF8B);
  CYC(b_+206, b_+207); mem_wr(gb, HL, A);
  CYC(b_+207, b_+208); alu_scf(gb);
  CYC(b_+208, SYM(minecartCreateCollisionItem)); ret_effect(gb);
}

void minecartCheckCollisions_hook(GB *gb) {
  BASE(minecartCheckCollisions);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getTileAtPosition_hook, SYM(getTileAtPosition), b_+3);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+5); C = L;
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = 0x08;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+14); SET_HL(b_+119);
  CYC(b_+14, b_+15); minecart_add_a_to_hl_from_rst(gb, b_+15);

find_track:
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+17, b_+19);
    goto no_track_found;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); alu_cp(gb, E);
  if (F & FZ) {
    CYCT(b_+20, b_+22);
    goto track_found;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = 0x04;
  CYC(b_+24, b_+25); minecart_add_a_to_hl_from_rst(gb, b_+25);
  CYC(b_+25, b_+27);
  goto find_track;

track_found:
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); alu_add(gb, C);
  CYC(b_+29, b_+30); C = A;
  CYC(b_+30, b_+32); H8(hFF8B) = A;
  CYC(b_+32, b_+34); B = 0xce;
  CYC(b_+34, b_+35); A = mem_rd(gb, BC);
  CYC(b_+35, b_+37); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+37, b_+38); ret_effect(gb); return;
  }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); B = 0xcf;
  CYC(b_+40, b_+41); A = mem_rd(gb, BC);
  CYC(b_+41, b_+43); alu_cp(gb, 0x5f);
  if (F & FZ) {
    CYCT(b_+43, b_+45);
    goto stop_minecart;
  }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+46); C = A;
  CYC(b_+46, b_+48); B = 0x03;

check_linked_track:
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+49, b_+50); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+50, b_+52);
    goto update_direction;
  }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+53, b_+55);
    goto check_linked_track;
  }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57);
  goto no_track_found;

stop_minecart:
  CYC(b_+57, b_+58); alu_scf(gb);
  CYC(b_+58, b_+59); ret_effect(gb);
  return;

update_direction:
  CYC(b_+59, b_+60); A = E;
  CYC(b_+60, b_+62); alu_sub(gb, 0x59);
  CYC(b_+62, b_+64); alu_cp(gb, 0x06);
  if (F & FC) {
    CYCT(b_+64, b_+66);
    goto dispatch_track;
  }
  CYC(b_+64, b_+66);

no_track_found:
  CYC(b_+66, b_+68); A = 0x06;

dispatch_track:
  CYC(b_+68, b_+70); E = 0x08;
  CYC(b_+70, b_+71); push_effect(gb, b_+71);
  do { uint16_t jt_ = (minecart_jump_table(gb));
    if (jt_ == b_+85) {
      CYC(b_+85, b_+86); A = mem_rd(gb, DE);
      CYC(b_+86, b_+88); alu_xor(gb, 0x01);
      CYC(b_+88, b_+89); mem_wr(gb, DE, A);
      CYC(b_+89, b_+90); ret_effect(gb);
      return;
    }
    else if (jt_ == b_+90) {
      CYC(b_+90, b_+91); A = mem_rd(gb, DE);
      CYC(b_+91, b_+93); alu_xor(gb, 0x03);
      CYC(b_+93, b_+94); mem_wr(gb, DE, A);
      CYC(b_+94, b_+95); ret_effect(gb);
      return;
    }
    else if (jt_ == b_+95) {
      CYC(b_+95, b_+96); A = mem_rd(gb, DE);
      CYC(b_+96, b_+98); alu_and(gb, 0x02);
      CYC(b_+98, b_+100); alu_or(gb, 0x01);
      CYC(b_+100, b_+101); mem_wr(gb, DE, A);
      CYC(b_+101, b_+102); ret_effect(gb);
      return;
    }
    else if (jt_ == b_+102) {
      CYC(b_+102, b_+103); A = mem_rd(gb, DE);
      CYC(b_+103, b_+105); alu_and(gb, 0x02);
      CYC(b_+105, b_+106); mem_wr(gb, DE, A);
      CYC(b_+106, b_+107); ret_effect(gb);
      return;
    }
    else if (jt_ == b_+107) {
      CYC(b_+107, b_+110); push_effect(gb, b_+110);
      minecart_check_door(gb, gb->sp);
      if (!(F & FC)) {
        CYCT(b_+110, b_+112);
        goto reverse_direction;
      }
      CYC(b_+110, b_+112);
      CYC(b_+112, b_+113); alu_xor(gb, A);
      CYC(b_+113, b_+114); ret_effect(gb);
      return;
    }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);

reverse_direction:
  CYC(b_+114, b_+115); A = mem_rd(gb, DE);
  CYC(b_+115, b_+117); alu_xor(gb, 0x02);
  CYC(b_+117, b_+118); mem_wr(gb, DE, A);
  CYC(b_+118, b_+119); ret_effect(gb);
}

void minecartCreateCollisionItem_hook(GB *gb) {
  BASE(minecartCreateCollisionItem);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x36;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, getFreeItemSlot_hook, SYM(getFreeItemSlot), b_+8);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+9); ret_effect(gb); return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = 0x36;
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x1d);
  CYC(b_+17, SYM(specialObjectCode_raft_b06)); ret_effect(gb);
}
