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
  TAIL(objectRunMovementScript_body); // fallthrough
}

// 0e:6b4c, bare global; called from objectRunMovementScript in bank0, also falls into from
// objectLoadMovementScript_body.
void objectRunMovementScript_body_hook(GB *gb) {
  BASE(objectRunMovementScript_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+O(2), b_+OE(4)); alu_add(gb, 0x30); // Object.var30
  CYC(b_+O(4), b_+OE(5)); E = A;
  CYC(b_+O(5), b_+OE(6)); A = mem_rd(gb, DE);
  CYC(b_+O(6), b_+OE(7)); L = A;
  CYC(b_+O(7), b_+OE(8)); E = alu_inc8(gb, E);
  CYC(b_+O(8), b_+OE(9)); A = mem_rd(gb, DE);
  CYC(b_+O(9), b_+OE(10)); H = A;

nextOp:
  CYC(b_+O(10), b_+OE(11)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  PUSH(b_+O(11), HL);
  {
    CYC(b_+O(12), b_+OE(13)); push_effect(gb, b_+OE(13));
    uint16_t target = objectMovementScript_jump_table(gb);
    if (target == b_+O(27)) goto cmd00_jump;
    if (target == b_+O(33)) goto moveUp;
    if (target == b_+O(55)) goto moveRight;
    if (target == b_+O(77)) goto moveDown;
    if (target == b_+O(99)) goto moveLeft;
    if (target == b_+O(121)) goto wait;
    if (!game_seasons && target == b_+142) goto setstate;
    HANDOFF(target);
  }

cmd00_jump:
  SET_HL(POP(b_+O(27)));
  CYC(b_+O(28), b_+OE(29)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(29), b_+OE(30)); H = mem_rd(gb, HL);
  CYC(b_+O(30), b_+OE(31)); L = A;
  CYC(b_+O(31), b_+OE(33)); goto nextOp; // jr

moveUp:
  SET_BC(POP(b_+O(33)));
  CYC(b_+O(34), b_+OE(35)); H = D;
  CYC(b_+O(35), b_+OE(37)); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+O(37), b_+OE(39)); alu_add(gb, OBJ_VAR32);
  CYC(b_+O(39), b_+OE(40)); L = A;
  CYC(b_+O(40), b_+OE(41)); A = mem_rd(gb, BC);
  CYC(b_+O(41), b_+OE(42)); mem_wr(gb, HL, A);
  CYC(b_+O(42), b_+OE(43)); A = L;
  CYC(b_+O(43), b_+OE(45)); alu_add(gb, 0xd7); // Object.angle-Object.var32
  CYC(b_+O(45), b_+OE(46)); L = A;
  CYC(b_+O(46), b_+OE(48)); mem_wr(gb, HL, 0x00); // ANGLE_UP
  CYC(b_+O(48), b_+OE(50)); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(b_+O(50), b_+OE(51)); L = A;
  CYC(b_+O(51), b_+OE(53)); mem_wr(gb, HL, 0x08);
  CYC(b_+O(53), b_+OE(55)); goto storePointer; // jr

moveRight:
  SET_BC(POP(b_+O(55)));
  CYC(b_+O(56), b_+OE(57)); H = D;
  CYC(b_+O(57), b_+OE(59)); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+O(59), b_+OE(61)); alu_add(gb, OBJ_VAR33);
  CYC(b_+O(61), b_+OE(62)); L = A;
  CYC(b_+O(62), b_+OE(63)); A = mem_rd(gb, BC);
  CYC(b_+O(63), b_+OE(64)); mem_wr(gb, HL, A);
  CYC(b_+O(64), b_+OE(65)); A = L;
  CYC(b_+O(65), b_+OE(67)); alu_add(gb, 0xd6); // Object.angle-Object.var33
  CYC(b_+O(67), b_+OE(68)); L = A;
  CYC(b_+O(68), b_+OE(70)); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT
  CYC(b_+O(70), b_+OE(72)); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(b_+O(72), b_+OE(73)); L = A;
  CYC(b_+O(73), b_+OE(75)); mem_wr(gb, HL, 0x09);
  CYC(b_+O(75), b_+OE(77)); goto storePointer; // jr

moveDown:
  SET_BC(POP(b_+O(77)));
  CYC(b_+O(78), b_+OE(79)); H = D;
  CYC(b_+O(79), b_+OE(81)); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+O(81), b_+OE(83)); alu_add(gb, OBJ_VAR32);
  CYC(b_+O(83), b_+OE(84)); L = A;
  CYC(b_+O(84), b_+OE(85)); A = mem_rd(gb, BC);
  CYC(b_+O(85), b_+OE(86)); mem_wr(gb, HL, A);
  CYC(b_+O(86), b_+OE(87)); A = L;
  CYC(b_+O(87), b_+OE(89)); alu_add(gb, 0xd7); // Object.angle-Object.var32
  CYC(b_+O(89), b_+OE(90)); L = A;
  CYC(b_+O(90), b_+OE(92)); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+O(92), b_+OE(94)); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(b_+O(94), b_+OE(95)); L = A;
  CYC(b_+O(95), b_+OE(97)); mem_wr(gb, HL, 0x0a);
  CYC(b_+O(97), b_+OE(99)); goto storePointer; // jr

moveLeft:
  SET_BC(POP(b_+O(99)));
  CYC(b_+O(100), b_+OE(101)); H = D;
  CYC(b_+O(101), b_+OE(103)); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+O(103), b_+OE(105)); alu_add(gb, OBJ_VAR33);
  CYC(b_+O(105), b_+OE(106)); L = A;
  CYC(b_+O(106), b_+OE(107)); A = mem_rd(gb, BC);
  CYC(b_+O(107), b_+OE(108)); mem_wr(gb, HL, A);
  CYC(b_+O(108), b_+OE(109)); A = L;
  CYC(b_+O(109), b_+OE(111)); alu_add(gb, 0xd6); // Object.angle-Object.var33
  CYC(b_+O(111), b_+OE(112)); L = A;
  CYC(b_+O(112), b_+OE(114)); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(b_+O(114), b_+OE(116)); alu_add(gb, 0xfb); // Object.state-Object.angle
  CYC(b_+O(116), b_+OE(117)); L = A;
  CYC(b_+O(117), b_+OE(119)); mem_wr(gb, HL, 0x0b);
  CYC(b_+O(119), b_+OE(121)); goto storePointer; // jr

wait:
  SET_BC(POP(b_+O(121)));
  CYC(b_+O(122), b_+OE(123)); H = D;
  CYC(b_+O(123), b_+OE(125)); A = hram_rd(gb, (uint8_t)hActiveObjectType);
  CYC(b_+O(125), b_+OE(127)); alu_add(gb, OBJ_COUNTER1);
  CYC(b_+O(127), b_+OE(128)); L = A;
  CYC(b_+O(128), b_+OE(129)); A = mem_rd(gb, BC);
  if (!game_seasons) {
    CYC(b_+129, b_+130); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
    CYC(b_+130, b_+131); L = alu_dec8(gb, L);
  } else {
    CYC(b_+S(127), b_+S(128)); mem_wr(gb, HL, A);
    CYC(b_+S(128), b_+S(129)); A = L;
    CYC(b_+S(129), b_+S(131)); alu_add(gb, 0xfe);
    CYC(b_+S(131), b_+S(132)); L = A;
  }
  CYC(b_+O(131), b_+OE(133)); mem_wr(gb, HL, 0x0c);
  goto storePointer; // fallthrough

storePointer:
  CYC(b_+O(133), b_+OE(134)); SET_BC(BC + 1); // inc bc
  if (!game_seasons) { CYC(b_+134, b_+135); A = L; }
  CYC(b_+O(135), b_+OE(137)); alu_add(gb, 0x2c); // Object.var30-Object.state
  CYC(b_+O(137), b_+OE(138)); L = A;
  CYC(b_+O(138), b_+OE(139)); mem_wr(gb, HL, C);
  CYC(b_+O(139), b_+OE(140)); L = alu_inc8(gb, L);
  CYC(b_+O(140), b_+OE(141)); mem_wr(gb, HL, B);
  RET(b_+O(141)); return; // ret

setstate:
  if (!game_seasons) {
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
}
