#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void objectRunMovementScript_body_hook(GB *gb);

static uint16_t objectMovementScript_jump_table(GB *gb) {
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

static void objectMovementScript_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0e:6b2d, bare global; called from objectLoadMovementScript in bank0. Falls through into
// objectRunMovementScript_body.
// @param hl Script address
void objectLoadMovementScript_body_hook(GB *gb) {
  CYC(0x6b2d, 0x6b2f); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(0x6b2f, 0x6b31); alu_add(gb, OBJ_SUBID);
  CYC(0x6b31, 0x6b32); E = A;
  CYC(0x6b32, 0x6b33); A = mem_rd(gb, DE);
  CYC(0x6b33, 0x6b34); objectMovementScript_addDoubleIndexToHl_from_rst(gb, 0x6b34);
  CYC(0x6b34, 0x6b35); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6b35, 0x6b36); H = mem_rd(gb, HL);
  CYC(0x6b36, 0x6b37); L = A;
  CYC(0x6b37, 0x6b38); A = E;
  CYC(0x6b38, 0x6b3a); alu_add(gb, 0x0e); // Object.speed-Object.subid
  CYC(0x6b3a, 0x6b3b); E = A;
  CYC(0x6b3b, 0x6b3c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6b3c, 0x6b3d); mem_wr(gb, DE, A);
  CYC(0x6b3d, 0x6b3e); A = E;
  CYC(0x6b3e, 0x6b40); alu_add(gb, 0xf8); // Object.direction-Object.speed
  CYC(0x6b40, 0x6b41); E = A;
  CYC(0x6b41, 0x6b42); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6b42, 0x6b43); mem_wr(gb, DE, A);
  CYC(0x6b43, 0x6b44); A = E;
  CYC(0x6b44, 0x6b46); alu_add(gb, 0x28); // Object.var30-Object.direction
  CYC(0x6b46, 0x6b47); E = A;
  CYC(0x6b47, 0x6b48); A = L;
  CYC(0x6b48, 0x6b49); mem_wr(gb, DE, A);
  CYC(0x6b49, 0x6b4a); E = alu_inc8(gb, E);
  CYC(0x6b4a, 0x6b4b); A = H;
  CYC(0x6b4b, 0x6b4c); mem_wr(gb, DE, A);
  objectRunMovementScript_body_hook(gb); return; // fallthrough
}

// 0e:6b4c, bare global; called from objectRunMovementScript in bank0, also falls into from
// objectLoadMovementScript_body.
void objectRunMovementScript_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b4c, 0x6b4e); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(0x6b4e, 0x6b50); alu_add(gb, 0x30); // Object.var30
  CYC(0x6b50, 0x6b51); E = A;
  CYC(0x6b51, 0x6b52); A = mem_rd(gb, DE);
  CYC(0x6b52, 0x6b53); L = A;
  CYC(0x6b53, 0x6b54); E = alu_inc8(gb, E);
  CYC(0x6b54, 0x6b55); A = mem_rd(gb, DE);
  CYC(0x6b55, 0x6b56); H = A;

nextOp:
  CYC(0x6b56, 0x6b57); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  PUSH(0x6b57, HL);
  {
    CYC(0x6b58, 0x6b59); push_effect(gb, 0x6b59);
    uint16_t target = objectMovementScript_jump_table(gb);
    if (target == 0x6b67) goto cmd00_jump;
    if (target == 0x6b6d) goto moveUp;
    if (target == 0x6b83) goto moveRight;
    if (target == 0x6b99) goto moveDown;
    if (target == 0x6baf) goto moveLeft;
    if (target == 0x6bc5) goto wait;
    if (target == 0x6bda) goto setstate;
    HANDOFF(target);
  }

cmd00_jump:
  SET_HL(POP(0x6b67));
  CYC(0x6b68, 0x6b69); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6b69, 0x6b6a); H = mem_rd(gb, HL);
  CYC(0x6b6a, 0x6b6b); L = A;
  CYC(0x6b6b, 0x6b6d); goto nextOp; // jr

moveUp:
  SET_BC(POP(0x6b6d));
  CYC(0x6b6e, 0x6b6f); H = D;
  CYC(0x6b6f, 0x6b71); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(0x6b71, 0x6b73); alu_add(gb, OBJ_VAR32);
  CYC(0x6b73, 0x6b74); L = A;
  CYC(0x6b74, 0x6b75); A = mem_rd(gb, BC);
  CYC(0x6b75, 0x6b76); mem_wr(gb, HL, A);
  CYC(0x6b76, 0x6b77); A = L;
  CYC(0x6b77, 0x6b79); alu_add(gb, 0xd7); // Object.angle-Object.var32
  CYC(0x6b79, 0x6b7a); L = A;
  CYC(0x6b7a, 0x6b7c); mem_wr(gb, HL, 0x00); // ANGLE_UP
  CYC(0x6b7c, 0x6b7e); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(0x6b7e, 0x6b7f); L = A;
  CYC(0x6b7f, 0x6b81); mem_wr(gb, HL, 0x08);
  CYC(0x6b81, 0x6b83); goto storePointer; // jr

moveRight:
  SET_BC(POP(0x6b83));
  CYC(0x6b84, 0x6b85); H = D;
  CYC(0x6b85, 0x6b87); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(0x6b87, 0x6b89); alu_add(gb, OBJ_VAR33);
  CYC(0x6b89, 0x6b8a); L = A;
  CYC(0x6b8a, 0x6b8b); A = mem_rd(gb, BC);
  CYC(0x6b8b, 0x6b8c); mem_wr(gb, HL, A);
  CYC(0x6b8c, 0x6b8d); A = L;
  CYC(0x6b8d, 0x6b8f); alu_add(gb, 0xd6); // Object.angle-Object.var33
  CYC(0x6b8f, 0x6b90); L = A;
  CYC(0x6b90, 0x6b92); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT
  CYC(0x6b92, 0x6b94); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(0x6b94, 0x6b95); L = A;
  CYC(0x6b95, 0x6b97); mem_wr(gb, HL, 0x09);
  CYC(0x6b97, 0x6b99); goto storePointer; // jr

moveDown:
  SET_BC(POP(0x6b99));
  CYC(0x6b9a, 0x6b9b); H = D;
  CYC(0x6b9b, 0x6b9d); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(0x6b9d, 0x6b9f); alu_add(gb, OBJ_VAR32);
  CYC(0x6b9f, 0x6ba0); L = A;
  CYC(0x6ba0, 0x6ba1); A = mem_rd(gb, BC);
  CYC(0x6ba1, 0x6ba2); mem_wr(gb, HL, A);
  CYC(0x6ba2, 0x6ba3); A = L;
  CYC(0x6ba3, 0x6ba5); alu_add(gb, 0xd7); // Object.angle-Object.var32
  CYC(0x6ba5, 0x6ba6); L = A;
  CYC(0x6ba6, 0x6ba8); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x6ba8, 0x6baa); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(0x6baa, 0x6bab); L = A;
  CYC(0x6bab, 0x6bad); mem_wr(gb, HL, 0x0a);
  CYC(0x6bad, 0x6baf); goto storePointer; // jr

moveLeft:
  SET_BC(POP(0x6baf));
  CYC(0x6bb0, 0x6bb1); H = D;
  CYC(0x6bb1, 0x6bb3); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(0x6bb3, 0x6bb5); alu_add(gb, OBJ_VAR33);
  CYC(0x6bb5, 0x6bb6); L = A;
  CYC(0x6bb6, 0x6bb7); A = mem_rd(gb, BC);
  CYC(0x6bb7, 0x6bb8); mem_wr(gb, HL, A);
  CYC(0x6bb8, 0x6bb9); A = L;
  CYC(0x6bb9, 0x6bbb); alu_add(gb, 0xd6); // Object.angle-Object.var33
  CYC(0x6bbb, 0x6bbc); L = A;
  CYC(0x6bbc, 0x6bbe); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(0x6bbe, 0x6bc0); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(0x6bc0, 0x6bc1); L = A;
  CYC(0x6bc1, 0x6bc3); mem_wr(gb, HL, 0x0b);
  CYC(0x6bc3, 0x6bc5); goto storePointer; // jr

wait:
  SET_BC(POP(0x6bc5));
  CYC(0x6bc6, 0x6bc7); H = D;
  CYC(0x6bc7, 0x6bc9); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(0x6bc9, 0x6bcb); alu_add(gb, OBJ_COUNTER1);
  CYC(0x6bcb, 0x6bcc); L = A;
  CYC(0x6bcc, 0x6bcd); A = mem_rd(gb, BC);
  CYC(0x6bcd, 0x6bce); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x6bce, 0x6bcf); L = alu_dec8(gb, L);
  CYC(0x6bcf, 0x6bd1); mem_wr(gb, HL, 0x0c);
  goto storePointer; // fallthrough

storePointer:
  CYC(0x6bd1, 0x6bd2); SET_BC(BC + 1); // inc bc
  CYC(0x6bd2, 0x6bd3); A = L;
  CYC(0x6bd3, 0x6bd5); alu_add(gb, 0x2c); // Object.var30-Object.state
  CYC(0x6bd5, 0x6bd6); L = A;
  CYC(0x6bd6, 0x6bd7); mem_wr(gb, HL, C);
  CYC(0x6bd7, 0x6bd8); L = alu_inc8(gb, L);
  CYC(0x6bd8, 0x6bd9); mem_wr(gb, HL, B);
  RET(0x6bd9); return; // ret

setstate:
  SET_BC(POP(0x6bda));
  CYC(0x6bdb, 0x6bdc); H = D;
  CYC(0x6bdc, 0x6bde); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(0x6bde, 0x6be0); alu_add(gb, OBJ_COUNTER1);
  CYC(0x6be0, 0x6be1); L = A;
  CYC(0x6be1, 0x6be2); A = mem_rd(gb, BC);
  CYC(0x6be2, 0x6be3); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x6be3, 0x6be4); L = alu_dec8(gb, L);
  CYC(0x6be4, 0x6be5); SET_BC(BC + 1); // inc bc
  CYC(0x6be5, 0x6be6); A = mem_rd(gb, BC);
  CYC(0x6be6, 0x6be7); mem_wr(gb, HL, A); // [state]
  CYC(0x6be7, 0x6be9); goto storePointer; // jr
}
