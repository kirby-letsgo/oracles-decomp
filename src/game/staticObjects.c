#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x16, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x16, (from), (to), true)

static uint16_t static_object_jump_table(GB *gb) {
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

void parseStaticObjects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5015, 0x5018); SET_DE(0xcd80);
next:
  CYC(0x5018, 0x5019); C = E;
  CYC(0x5019, 0x501a); A = mem_rd(gb, DE);
  CYC(0x501a, 0x501b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x501b, 0x501d); goto no_match; }
  CYC(0x501b, 0x501d);
  CYC(0x501d, 0x501e); E = alu_inc8(gb, E);
  CYC(0x501e, 0x501f); A = mem_rd(gb, DE);
  CYC(0x501f, 0x5020); B = A;
  CYC(0x5020, 0x5023); A = mem_rd(gb, 0xcc30);
  CYC(0x5023, 0x5024); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x5024, 0x5026); goto found; }
  CYC(0x5024, 0x5026);
no_match:
  CYC(0x5026, 0x5027); A = C;
  CYC(0x5027, 0x5029); alu_add(gb, 0x08);
  CYC(0x5029, 0x502a); E = A;
  CYC(0x502a, 0x502c); alu_cp(gb, 0xc0);
  if (F & FC) { CYCT(0x502c, 0x502e); goto next; }
  CYC(0x502c, 0x502e);
  CYC(0x502e, 0x502f);
  ret_effect(gb);
  return;
found:
  CYC(0x502f, 0x5030); E = alu_dec8(gb, E);
  CYC(0x5030, 0x5031); A = mem_rd(gb, DE);
  CYC(0x5031, 0x5033); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x5033, 0x5035); goto no_match; }
  CYC(0x5033, 0x5035);
  CYC(0x5035, 0x5037); alu_and(gb, 0x7f);
  CYC(0x5037, 0x5038); push_effect(gb, 0x5038);
  switch (static_object_jump_table(gb)) {
    case 0x5044: goto end;
    case 0x504a: goto interaction;
    case 0x5051: goto enemy;
    case 0x5058: goto part;
    default: hook_handoff(gb, HL); return;
  }
end:
  CYC(0x5044, 0x5045); A = E;
  CYC(0x5045, 0x5047); alu_add(gb, 0x08);
  CYC(0x5047, 0x5048); E = A;
  CYC(0x5048, 0x504a); goto next;
interaction:
  CALL_C(0x504a, getFreeInteractionSlot_hook, 0x3aef, 0x504d);
  if (!(F & FZ)) { CYCT(0x504d, 0x504f); goto end; }
  CYC(0x504d, 0x504f);
  CYC(0x504f, 0x5051); goto copy;
enemy:
  CALL_C(0x5051, getFreeEnemySlot_hook, 0x2e27, 0x5054);
  if (!(F & FZ)) { CYCT(0x5054, 0x5056); goto end; }
  CYC(0x5054, 0x5056);
  CYC(0x5056, 0x5058); goto copy;
part:
  CALL_C(0x5058, getFreePartSlot_hook, 0x3e8e, 0x505b);
  if (!(F & FZ)) { CYCT(0x505b, 0x505d); goto end; }
  CYC(0x505b, 0x505d);
copy:
  CYC(0x505d, 0x505e); E = alu_inc8(gb, E);
  CYC(0x505e, 0x505f); E = alu_inc8(gb, E);
  CYC(0x505f, 0x5060); A = mem_rd(gb, DE);
  CYC(0x5060, 0x5062); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x5062, 0x5064); goto copy_data; }
  CYC(0x5062, 0x5064);
  CYC(0x5064, 0x5065); L = alu_dec8(gb, L);
  CYC(0x5065, 0x5067); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));
  CYC(0x5067, 0x5068); L = alu_inc8(gb, L);
copy_data:
  CYC(0x5068, 0x506a); alu_and(gb, 0x7f);
  CYC(0x506a, 0x506b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x506b, 0x506c); E = alu_inc8(gb, E);
  CYC(0x506c, 0x506d); A = mem_rd(gb, DE);
  CYC(0x506d, 0x506e); mem_wr(gb, HL, A);
  CYC(0x506e, 0x506f); A = L;
  CYC(0x506f, 0x5071); alu_add(gb, 0x09);
  CYC(0x5071, 0x5072); L = A;
  CYC(0x5072, 0x5073); E = alu_inc8(gb, E);
  CYC(0x5073, 0x5074); A = mem_rd(gb, DE);
  CYC(0x5074, 0x5075); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5075, 0x5076); L = alu_inc8(gb, L);
  CYC(0x5076, 0x5077); E = alu_inc8(gb, E);
  CYC(0x5077, 0x5078); A = mem_rd(gb, DE);
  CYC(0x5078, 0x5079); mem_wr(gb, HL, A);
  CYC(0x5079, 0x507a); A = L;
  CYC(0x507a, 0x507c); alu_add(gb, 0x09);
  CYC(0x507c, 0x507d); L = A;
  CYC(0x507d, 0x507e); A = E;
  CYC(0x507e, 0x5080); alu_and(gb, 0xf8);
  CYC(0x5080, 0x5081); E = A;
  CYC(0x5081, 0x5082); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5082, 0x5083); mem_wr(gb, HL, D);
  CYC(0x5083, 0x5085); goto end;
}

static void add_double_index_to_hl(GB *gb) {
  CYC(0x508e, 0x508f); push_effect(gb, 0x508f);
  burn_rom(gb, 0, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x001f, 0x0020, false); ret_effect(gb);
}

void loadStaticObjects_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5085, clearStaticObjects_hook, 0x319f, 0x5088);
  CYC(0x5088, 0x508b); A = mem_rd(gb, 0xcc39);
  CYC(0x508b, 0x508e); SET_HL(0x50a7);
  add_double_index_to_hl(gb);
  CYC(0x508f, 0x5090); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5090, 0x5091); H = mem_rd(gb, HL);
  CYC(0x5091, 0x5092); L = A;
  CYC(0x5092, 0x5095); SET_DE(0xcd80);
  for (;;) {
    CYC(0x5095, 0x5096); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5096, 0x5098); alu_cp(gb, 0xff);
    if (F & FZ) { CYCT(0x5098, 0x5099); ret_effect(gb); return; }
    CYC(0x5098, 0x5099);
    CYC(0x5099, 0x509a); mem_wr(gb, DE, A);
    CYC(0x509a, 0x509c); B = 5;
    for (;;) {
      CYC(0x509c, 0x509d); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x509d, 0x509e); E = alu_inc8(gb, E);
      CYC(0x509e, 0x509f); mem_wr(gb, DE, A);
      CYC(0x509f, 0x50a0); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(0x50a0, 0x50a2); continue; }
      CYC(0x50a0, 0x50a2);
      break;
    }
    CYC(0x50a2, 0x50a3); E = alu_inc8(gb, E);
    CYC(0x50a3, 0x50a4); E = alu_inc8(gb, E);
    CYC(0x50a4, 0x50a5); E = alu_inc8(gb, E);
    CYC(0x50a5, 0x50a8);
  }
}
