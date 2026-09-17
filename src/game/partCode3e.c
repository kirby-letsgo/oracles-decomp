#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t partCode3e_jump_table(GB *gb) {
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

// PART_3e: tracks the enemy index of every loaded Ambi Guard in Part.var30-3f
void partCode3e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x70ed, 0x70ef); E = 0xc4; // Part.state
  CYC(0x70ef, 0x70f0); A = mem_rd(gb, DE);
  {
    CYC(0x70f0, 0x70f1); push_effect(gb, 0x70f1);
    uint16_t target = partCode3e_jump_table(gb);
    if (target == 0x7110) goto state1;
    if (target == 0x7129) goto state2;
    if (target == 0x714b) goto state3;
    goto state0;
  }

state0:
  CYC(0x70f9, 0x70fa); H = D;
  CYC(0x70fa, 0x70fb); L = E;
  CYC(0x70fb, 0x70fc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x70fc, 0x70fe); E = 0xf0; // Part.var30
  CYC(0x70fe, 0x7101); SET_HL(0xd081); // FIRST_ENEMY_INDEX, Enemy.id

L_7101:
  CYC(0x7101, 0x7102); A = mem_rd(gb, HL);
  CYC(0x7102, 0x7104); alu_cp(gb, 0x54); // ENEMY_AMBI_GUARD
  if (!(F & FZ)) { CYCT(0x7104, 0x7106); goto L_7109; } // jr nz
  CYC(0x7104, 0x7106);
  CYC(0x7106, 0x7107); A = H;
  CYC(0x7107, 0x7108); mem_wr(gb, DE, A);
  CYC(0x7108, 0x7109); E = alu_inc8(gb, E);

L_7109:
  CYC(0x7109, 0x710a); H = alu_inc8(gb, H);
  CYC(0x710a, 0x710b); A = H;
  CYC(0x710b, 0x710d); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(0x710d, 0x710f); goto L_7101; } // jr c
  CYC(0x710d, 0x710f);
  RET(0x710f); return; // ret

state1:
  CYC(0x7110, 0x7113); SET_HL(0xd700);

L_7113:
  CYC(0x7113, 0x7115); L = 0x24; // Object.collisionType
  CYC(0x7115, 0x7116); A = mem_rd(gb, HL);
  CYC(0x7116, 0x7118); alu_cp(gb, 0x98);
  if (F & FZ) { CYCT(0x7118, 0x711a); goto L_7121; } // jr z
  CYC(0x7118, 0x711a);
  CYC(0x711a, 0x711b); H = alu_inc8(gb, H);
  CYC(0x711b, 0x711c); A = H;
  CYC(0x711c, 0x711e); alu_cp(gb, 0xdc);
  if (F & FC) { CYCT(0x711e, 0x7120); goto L_7113; } // jr c
  CYC(0x711e, 0x7120);
  RET(0x7120); return; // ret

L_7121:
  CYC(0x7121, 0x7123); A = 0x02;
  CYC(0x7123, 0x7124); mem_wr(gb, DE, A);
  CYC(0x7124, 0x7126); E = 0xd9; // Part.relatedObj2+1
  CYC(0x7126, 0x7127); A = H;
  CYC(0x7127, 0x7128); mem_wr(gb, DE, A);
  RET(0x7128); return; // ret

state2:
  CYC(0x7129, 0x712a); H = D;
  CYC(0x712a, 0x712c); L = 0xc4; // Part.state
  CYC(0x712c, 0x712d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x712d, 0x712f); L = 0xc6; // Part.counter1
  CYC(0x712f, 0x7131); mem_wr(gb, HL, 0x3c);
  CYC(0x7131, 0x7133); L = 0xd9; // Part.relatedObj2+1
  CYC(0x7133, 0x7134); B = mem_rd(gb, HL);
  CYC(0x7134, 0x7136); E = 0xf0; // Part.var30

L_7136:
  CYC(0x7136, 0x7137); A = mem_rd(gb, DE);
  CYC(0x7137, 0x7138); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7138); return; } // ret z
  CYC(0x7138, 0x7139);
  CYC(0x7139, 0x713a); H = A;
  CYC(0x713a, 0x713c); L = 0xba; // Enemy.var3a
  CYC(0x713c, 0x713e); mem_wr(gb, HL, 0xff);
  CYC(0x713e, 0x7140); L = 0x98; // Enemy.relatedObj2
  CYC(0x7140, 0x7142); mem_wr(gb, HL, 0x00);
  CYC(0x7142, 0x7143); L = alu_inc8(gb, L);
  CYC(0x7143, 0x7144); mem_wr(gb, HL, B);
  CYC(0x7144, 0x7145); E = alu_inc8(gb, E);
  CYC(0x7145, 0x7146); A = E;
  CYC(0x7146, 0x7148); alu_cp(gb, 0xf4); // Part.var34
  if (F & FC) { CYCT(0x7148, 0x714a); goto L_7136; } // jr c
  CYC(0x7148, 0x714a);
  RET(0x714a); return; // ret

state3:
  CALL_C(0x714b, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x714e);
  if (!(F & FZ)) { RET_TAKEN(0x714e); return; } // ret nz
  CYC(0x714e, 0x714f);
  CYC(0x714f, 0x7150); L = E;
  CYC(0x7150, 0x7152); mem_wr(gb, HL, 0x01);
  RET(0x7152); return; // ret
}
