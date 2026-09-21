#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t item_post_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else { burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H); }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

static uint16_t item_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else { burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H); }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

static void update_item_hook(GB *gb) {
  BASE(updateItems);
  CYC(b_+76, b_+78); E = 0x01;
  CYC(b_+78, b_+79); A = mem_rd(gb, DE);
  CYC(b_+79, b_+80); push_effect(gb, b_+80);
  hook_handoff(gb, item_jump_table(gb));
}

void updateItems_hook(GB *gb) {
  BASE(updateItems);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0;
  CYC(b_+2, b_+5); A = mem_rd(gb, wScreenVariables);
  CYC(b_+5, b_+7); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+7, b_+9); goto dont_update; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+12, b_+14); alu_and(gb, 0x90);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto dont_update; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto dont_update; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); A = mem_rd(gb, wOamEnd);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+26, b_+28); goto setup; }
  CYC(b_+26, b_+28);
dont_update:
  CYC(b_+28, b_+29); B = alu_inc8(gb, B);
setup:
  CYC(b_+29, b_+32); SET_HL(wcc8b);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_and(gb, 0xfe);
  CYC(b_+35, b_+36); alu_or(gb, B);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  CYC(b_+37, b_+38); alu_xor(gb, A);
  CYC(b_+38, b_+41); mem_wr(gb, wScentSeedActive, A);
  CYC(b_+41, b_+43); A = 0;
  CYC(b_+43, b_+45); H8(hActiveObjectType) = A;
  CYC(b_+45, b_+47); D = 0xd6;
  CYC(b_+47, b_+48); A = D;
loop:
  CYC(b_+48, b_+50); H8(hActiveObject) = A;
  CYC(b_+50, b_+52); E = 0;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+54, b_+56); goto next; }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); E = 0x04;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+60, b_+62); goto update; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+65); A = mem_rd(gb, wcc8b);
  CYC(b_+65, b_+66); alu_or(gb, A);
update:
  if (F & FZ) CALL_C_CC(b_+66, update_item_hook, b_+76, b_+69);
  else CYC(b_+66, b_+69);
next:
  CYC(b_+69, b_+70); D = alu_inc8(gb, D);
  CYC(b_+70, b_+71); A = D;
  CYC(b_+71, b_+73); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+73, b_+75); goto loop; }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+76);
  ret_effect(gb);
}

void itemCodeNilPost_hook(GB *gb) {
  BASE(itemCodeNilPost);
  CYC(b_+0, b_+1);
  ret_effect(gb);
}

void updateItemPost_hook(GB *gb) {
  BASE(updateItemPost);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+2); E = 0x01;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (item_post_jump_table(gb));
    if (jt_ == SYM(itemCodeNilPost) && hook_enabled_at(gb, SYM(itemCodeNilPost))) { itemCodeNilPost_hook(gb); return; }
    else if (jt_ == SYM(itemCode00Post) && hook_enabled_at(gb, SYM(itemCode00Post))) { itemCode00Post_hook(gb); return; }
    else if (jt_ == SYM(itemCode04Post) && hook_enabled_at(gb, SYM(itemCode04Post))) { itemCode04Post_hook(gb); return; }
    else if (jt_ == SYM(itemCode08Post) && hook_enabled_at(gb, SYM(itemCode08Post))) { itemCode08Post_hook(gb); return; }
    else if (jt_ == SYM(itemCode0aPost) && hook_enabled_at(gb, SYM(itemCode0aPost))) { itemCode0aPost_hook(gb); return; }
    else if (jt_ == SYM(itemCode0bPost) && hook_enabled_at(gb, SYM(itemCode0bPost))) { itemCode0bPost_hook(gb); return; }
    else if (jt_ == SYM(itemCode0cPost) && hook_enabled_at(gb, SYM(itemCode0cPost))) { itemCode0cPost_hook(gb); return; }
    else if (jt_ == SYM(itemCode0fPost) && hook_enabled_at(gb, SYM(itemCode0fPost))) { itemCode0fPost_hook(gb); return; }
    else if (jt_ == SYM(itemCode13Post) && hook_enabled_at(gb, SYM(itemCode13Post))) { itemCode13Post_hook(gb); return; }
    else if (jt_ == SYM(itemCode1dPost) && hook_enabled_at(gb, SYM(itemCode1dPost))) { itemCode1dPost_hook(gb); return; }
    else if (jt_ == SYM(itemCode1ePost) && hook_enabled_at(gb, SYM(itemCode1ePost))) { itemCode1ePost_hook(gb); return; }
    else if (jt_ == SYM(itemDelete) && hook_enabled_at(gb, SYM(itemDelete))) { itemDelete_hook(gb); return; }
    else { HANDOFF(jt_); }
  } while (0);
}

void updateItemsPost_hook(GB *gb) {
  BASE(updateItemsPost);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); H8(hActiveObjectType) = A;
  CYC(b_+3, b_+5); D = 0xd6;
  CYC(b_+5, b_+6); A = D;
loop:
  CYC(b_+6, b_+8); H8(hActiveObject) = A;
  CYC(b_+8, b_+10); E = 0;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+12, updateItemPost_hook, SYM(updateItemPost), b_+15);
  else CYC(b_+12, b_+15);
  CYC(b_+15, b_+16); D = alu_inc8(gb, D);
  CYC(b_+16, b_+17); A = D;
  CYC(b_+17, b_+19); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+19, b_+21); goto loop; }
  CYC(b_+19, b_+21);
  itemCodeNilPost_hook(gb);
}
