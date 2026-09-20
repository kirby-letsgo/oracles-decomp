#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t lighting_jump_table(GB *gb) {
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

static void lighting_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5553, 0x5555); E = 0xc4; // Part.state
  CYC(0x5555, 0x5556); A = mem_rd(gb, DE);
  {
    CYC(0x5556, 0x5557); push_effect(gb, 0x5557);
    uint16_t target = lighting_jump_table(gb);
    if (target == 0x555d) goto state0;
    if (target == 0x5580) goto state1;
    goto state2;
  }

state0:
  CYC(0x555d, 0x555f); A = 0x01;
  CYC(0x555f, 0x5560); mem_wr(gb, DE, A);
  CALL_C(0x5560, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5563);
  CYC(0x5563, 0x5565); E = 0xf0; // Part.var30
  CYC(0x5565, 0x5567); alu_and(gb, 0x06);
  CYC(0x5567, 0x5568); mem_wr(gb, DE, A);
  CYC(0x5568, 0x5569); H = D;
  CYC(0x5569, 0x556b); L = 0xcf; // Part.zh
  CYC(0x556b, 0x556d); mem_wr(gb, HL, 0xc0);
  CYC(0x556d, 0x556f); L = 0xd7; // Part.relatedObj1+1
  CYC(0x556f, 0x5570); A = mem_rd(gb, HL);
  CYC(0x5570, 0x5571); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5571); return; } // ret z
  CYC(0x5571, 0x5572);
  CYC(0x5572, 0x5574); L = 0xc6; // Part.counter1
  CYC(0x5574, 0x5576); mem_wr(gb, HL, 0x1e);
  CYC(0x5576, 0x5578); L = 0xcb; // Part.yh
  CYC(0x5578, 0x557a); A = H8(hEnemyTargetY);
  CYC(0x557a, 0x557b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x557b, 0x557c); L = alu_inc8(gb, L);
  CYC(0x557c, 0x557e); A = H8(hEnemyTargetX);
  CYC(0x557e, 0x557f); mem_wr(gb, HL, A);
  RET(0x557f); return; // ret

state1:
  CALL_C(0x5580, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5583);
  if (!(F & FZ)) { RET_TAKEN(0x5583); return; } // ret nz
  CYC(0x5583, 0x5584);
  CYC(0x5584, 0x5585); L = E;
  CYC(0x5585, 0x5586); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5586, 0x5588); A = 0xd2; // SND_LIGHTNING
  CALL_C(0x5588, playSound_b00_hook, 0x0c98, 0x558b);
  CYC(0x558b, 0x558e); objectSetVisible81_hook(gb); return; // jp

state2:
  CALL_C(0x558e, partAnimate_hook, 0x2978, 0x5591);
  CYC(0x5591, 0x5593); E = 0xe1; // Part.animParameter
  CYC(0x5593, 0x5594); A = mem_rd(gb, DE);
  CYC(0x5594, 0x5595); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x5595, 0x5598); partDelete_hook(gb); return; } // jp z
  CYC(0x5595, 0x5598);
  CYC(0x5598, 0x559b); push_effect(gb, 0x559b); goto func_55a6; // call $55a6 (own burn; local, inlined)

resume_559b:
  CYC(0x559b, 0x559d); E = 0xc3; // Part.var03
  CYC(0x559d, 0x559e); A = mem_rd(gb, DE);
  CYC(0x559e, 0x559f); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x559f); return; } // ret z
  CYC(0x559f, 0x55a0);
  CYC(0x55a0, 0x55a2); A = 0xff;
  CYC(0x55a2, 0x55a5); mem_wr(gb, 0xcfd2, A);
  RET(0x55a5); return; // ret

func_55a6:
  CYC(0x55a6, 0x55a8); E = 0xe1; // Part.animParameter
  CYC(0x55a8, 0x55a9); A = mem_rd(gb, DE);
  CYC(0x55a9, 0x55ab); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x55ab, 0x55ae); push_effect(gb, 0x55ae); goto func_55e7; } // call nz (taken; local, inlined)
  CYC(0x55ab, 0x55ae);

resume_55ae:
  CYC(0x55ae, 0x55b0); E = 0xe1; // Part.animParameter
  CYC(0x55b0, 0x55b1); A = mem_rd(gb, DE);
  CYC(0x55b1, 0x55b3); alu_and(gb, 0x0e);
  CYC(0x55b3, 0x55b6); SET_HL(0x55da); // table_55da
  CYC(0x55b6, 0x55b7); lighting_addAToHl_from_rst(gb, 0x55b7);
  CYC(0x55b7, 0x55b9); E = 0xe6; // Part.collisionRadiusY
  CYC(0x55b9, 0x55ba); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x55ba, 0x55bb); mem_wr(gb, DE, A);
  CYC(0x55bb, 0x55bc); E = alu_inc8(gb, E);
  CYC(0x55bc, 0x55bd); A = mem_rd(gb, HL);
  CYC(0x55bd, 0x55be); mem_wr(gb, DE, A);
  CYC(0x55be, 0x55c0); E = 0xe1; // Part.animParameter
  CYC(0x55c0, 0x55c1); A = mem_rd(gb, DE);
  CYC(0x55c1, 0x55c3); alu_and(gb, 0x70);
  CYC(0x55c3, 0x55c5); A = alu_swap(gb, A);
  CYC(0x55c5, 0x55c8); SET_HL(0x55e2); // table_55e2
  CYC(0x55c8, 0x55c9); lighting_addAToHl_from_rst(gb, 0x55c9);
  CYC(0x55c9, 0x55cb); E = 0xcf; // Part.zh
  CYC(0x55cb, 0x55cc); A = mem_rd(gb, HL);
  CYC(0x55cc, 0x55cd); mem_wr(gb, DE, A);
  CYC(0x55cd, 0x55cf); E = 0xe1; // Part.animParameter
  CYC(0x55cf, 0x55d0); A = mem_rd(gb, DE);
  CYC(0x55d0, 0x55d2); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x55d2, 0x55d3); ret_effect(gb); goto resume_559b; } // ret z (taken; pops to caller)
  CYC(0x55d2, 0x55d3);
  CYC(0x55d3, 0x55d4); A = alu_dec8(gb, A);
  CYC(0x55d4, 0x55d5); mem_wr(gb, DE, A);
  CYC(0x55d5, 0x55d7); A = 0x06;
  CYC(0x55d7, 0x55da); setScreenShakeCounter_hook(gb); goto resume_559b; // jp (tail call; its own ret pops to caller)

func_55e7:
  CYC(0x55e7, 0x55e9); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x55e9, 0x55ea); mem_wr(gb, DE, A);
  CYC(0x55ea, 0x55ec); alu_and(gb, 0x0e);
  CYC(0x55ec, 0x55ee); alu_sub(gb, 0x02);
  CYC(0x55ee, 0x55ef); B = A;
  CYC(0x55ef, 0x55f1); E = 0xf0; // Part.var30
  CYC(0x55f1, 0x55f2); A = mem_rd(gb, DE);
  CYC(0x55f2, 0x55f3); alu_add(gb, B);
  CYC(0x55f3, 0x55f6); SET_HL(0x5603); // table_5603
  CYC(0x55f6, 0x55f7); lighting_addAToHl_from_rst(gb, 0x55f7);
  CYC(0x55f7, 0x55f8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x55f8, 0x55f9); C = mem_rd(gb, HL);
  CYC(0x55f9, 0x55fa); B = A;
  CALL_C(0x55fa, getFreeInteractionSlot_hook, 0x3aef, 0x55fd);
  if (!(F & FZ)) { CYCT(0x55fd, 0x55fe); ret_effect(gb); goto resume_55ae; } // ret nz (taken; pops to caller)
  CYC(0x55fd, 0x55fe);
  CYC(0x55fe, 0x5600); mem_wr(gb, HL, 0x08);
  CYC(0x5600, 0x5603); objectCopyPositionWithOffset_hook(gb); goto resume_55ae; // jp (tail call; its own ret pops to caller)
}
