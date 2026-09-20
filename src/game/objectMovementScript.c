#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(objectLoadMovementScript_body);
  CYC(b_+0, b_+2); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+2, b_+4); alu_add(gb, OBJ_SUBID);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); objectMovementScript_addDoubleIndexToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+11); A = E;
  CYC(b_+11, b_+13); alu_add(gb, 0x0e); // Object.speed-Object.subid
  CYC(b_+13, b_+14); E = A;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); A = E;
  CYC(b_+17, b_+19); alu_add(gb, 0xf8); // Object.direction-Object.speed
  CYC(b_+19, b_+20); E = A;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); A = E;
  CYC(b_+23, b_+25); alu_add(gb, 0x28); // Object.var30-Object.direction
  CYC(b_+25, b_+26); E = A;
  CYC(b_+26, b_+27); A = L;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); E = alu_inc8(gb, E);
  CYC(b_+29, b_+30); A = H;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  objectRunMovementScript_body_hook(gb); return; // fallthrough
}

// 0e:6b4c, bare global; called from objectRunMovementScript in bank0, also falls into from
// objectLoadMovementScript_body.
void objectRunMovementScript_body_hook(GB *gb) {
  BASE(objectRunMovementScript_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+2, b_+4); alu_add(gb, 0x30); // Object.var30
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); L = A;
  CYC(b_+7, b_+8); E = alu_inc8(gb, E);
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); H = A;

nextOp:
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  PUSH(b_+11, HL);
  {
    CYC(b_+12, b_+13); push_effect(gb, b_+13);
    uint16_t target = objectMovementScript_jump_table(gb);
    if (target == b_+27) goto cmd00_jump;
    if (target == b_+33) goto moveUp;
    if (target == b_+55) goto moveRight;
    if (target == b_+77) goto moveDown;
    if (target == b_+99) goto moveLeft;
    if (target == b_+121) goto wait;
    if (target == b_+142) goto setstate;
    HANDOFF(target);
  }

cmd00_jump:
  SET_HL(POP(b_+27));
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+29, b_+30); H = mem_rd(gb, HL);
  CYC(b_+30, b_+31); L = A;
  CYC(b_+31, b_+33); goto nextOp; // jr

moveUp:
  SET_BC(POP(b_+33));
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+37, b_+39); alu_add(gb, OBJ_VAR32);
  CYC(b_+39, b_+40); L = A;
  CYC(b_+40, b_+41); A = mem_rd(gb, BC);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A);
  CYC(b_+42, b_+43); A = L;
  CYC(b_+43, b_+45); alu_add(gb, 0xd7); // Object.angle-Object.var32
  CYC(b_+45, b_+46); L = A;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x00); // ANGLE_UP
  CYC(b_+48, b_+50); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(b_+50, b_+51); L = A;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x08);
  CYC(b_+53, b_+55); goto storePointer; // jr

moveRight:
  SET_BC(POP(b_+55));
  CYC(b_+56, b_+57); H = D;
  CYC(b_+57, b_+59); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+59, b_+61); alu_add(gb, OBJ_VAR33);
  CYC(b_+61, b_+62); L = A;
  CYC(b_+62, b_+63); A = mem_rd(gb, BC);
  CYC(b_+63, b_+64); mem_wr(gb, HL, A);
  CYC(b_+64, b_+65); A = L;
  CYC(b_+65, b_+67); alu_add(gb, 0xd6); // Object.angle-Object.var33
  CYC(b_+67, b_+68); L = A;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT
  CYC(b_+70, b_+72); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(b_+72, b_+73); L = A;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x09);
  CYC(b_+75, b_+77); goto storePointer; // jr

moveDown:
  SET_BC(POP(b_+77));
  CYC(b_+78, b_+79); H = D;
  CYC(b_+79, b_+81); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+81, b_+83); alu_add(gb, OBJ_VAR32);
  CYC(b_+83, b_+84); L = A;
  CYC(b_+84, b_+85); A = mem_rd(gb, BC);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A);
  CYC(b_+86, b_+87); A = L;
  CYC(b_+87, b_+89); alu_add(gb, 0xd7); // Object.angle-Object.var32
  CYC(b_+89, b_+90); L = A;
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+92, b_+94); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(b_+94, b_+95); L = A;
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x0a);
  CYC(b_+97, b_+99); goto storePointer; // jr

moveLeft:
  SET_BC(POP(b_+99));
  CYC(b_+100, b_+101); H = D;
  CYC(b_+101, b_+103); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+103, b_+105); alu_add(gb, OBJ_VAR33);
  CYC(b_+105, b_+106); L = A;
  CYC(b_+106, b_+107); A = mem_rd(gb, BC);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A);
  CYC(b_+108, b_+109); A = L;
  CYC(b_+109, b_+111); alu_add(gb, 0xd6); // Object.angle-Object.var33
  CYC(b_+111, b_+112); L = A;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(b_+114, b_+116); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(b_+116, b_+117); L = A;
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x0b);
  CYC(b_+119, b_+121); goto storePointer; // jr

wait:
  SET_BC(POP(b_+121));
  CYC(b_+122, b_+123); H = D;
  CYC(b_+123, b_+125); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+125, b_+127); alu_add(gb, OBJ_COUNTER1);
  CYC(b_+127, b_+128); L = A;
  CYC(b_+128, b_+129); A = mem_rd(gb, BC);
  CYC(b_+129, b_+130); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+130, b_+131); L = alu_dec8(gb, L);
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x0c);
  goto storePointer; // fallthrough

storePointer:
  CYC(b_+133, b_+134); SET_BC(BC + 1); // inc bc
  CYC(b_+134, b_+135); A = L;
  CYC(b_+135, b_+137); alu_add(gb, 0x2c); // Object.var30-Object.state
  CYC(b_+137, b_+138); L = A;
  CYC(b_+138, b_+139); mem_wr(gb, HL, C);
  CYC(b_+139, b_+140); L = alu_inc8(gb, L);
  CYC(b_+140, b_+141); mem_wr(gb, HL, B);
  RET(b_+141); return; // ret

setstate:
  SET_BC(POP(b_+142));
  CYC(b_+143, b_+144); H = D;
  CYC(b_+144, b_+146); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+146, b_+148); alu_add(gb, OBJ_COUNTER1);
  CYC(b_+148, b_+149); L = A;
  CYC(b_+149, b_+150); A = mem_rd(gb, BC);
  CYC(b_+150, b_+151); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+151, b_+152); L = alu_dec8(gb, L);
  CYC(b_+152, b_+153); SET_BC(BC + 1); // inc bc
  CYC(b_+153, b_+154); A = mem_rd(gb, BC);
  CYC(b_+154, b_+155); mem_wr(gb, HL, A); // [state]
  CYC(b_+155, b_+157); goto storePointer; // jr
}
